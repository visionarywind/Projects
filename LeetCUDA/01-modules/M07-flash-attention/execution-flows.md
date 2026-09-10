# M07 执行流：Q/K/V 分块与 online softmax

## 1. Python 到 kernel

```text
CLI args
  → get_build_sources()
  → get_build_cuda_cflags()
  → torch cpp_extension.load()
  → PyBind selected function
  → head-dim/stage dispatch
  → CUDA block per Q tile
  → reference/check + benchmark
```

证据：[kernels/flash-attn/flash_attn_mma.py:80-227]、[kernels/flash-attn/pybind/flash_attn.cc:7-223]、[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:799-946]。

## 2. 一个 Q tile 的控制流

```text
A. 计算 batch/head/Q_tile 的 global offsets
B. 协作装载 Q；必要时装载当前 K/V tile
C. 对 head_dim 的每个 16-wide tile:
   C1. ldmatrix(Q/K)
   C2. HMMA → logits S
D. 四 lane row max / scale / exp / row sum
E. S 就地变为 P
F. 装载 V，HMMA(P,V) → O_tile
G. 用 m_old/m_new 重缩放旧 O
H. K/V tile 结束后继续下一 tile
I. 用 1/l_final 归一化并 shuffle/store O
```

QK 和 PV 的源码区间分别是 `[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:248-391]`、`620-658`；softmax/rescale 是 `401-488,537-570,665-721`。**已确认**。

## 3. 数据流与内存位置

| 状态 | 首要位置 | 生命周期 |
|---|---|---|
| Q/K/V global tensor | global memory | Python tensor 生命周期；kernel 借用 |
| Q/K tile | shared memory | 当前 block/Q tile 与 K/V tile |
| V tile | alias Q shared region | Q 使用结束后复用；依赖顺序 |
| Q/K/V fragments | registers | 单个 MMA/head-dim 子块 |
| logits S/P | registers | 一个 K/V tile；S 就地转 P |
| m/l | registers | 跨 K/V tiles 的每行 online 状态 |
| O accumulator | registers | 跨 K/V tiles，最终写回 |
| O | global memory | 输出 tensor |

Q/K/V shared-memory 尺寸和 alias 见 `[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:156-170]`，寄存器数组见 `[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:172-200]`。

## 4. Stage 1/2

- `stages <= 1` 选择 stage 1；
- `stages > 1` 选择 stage 2；
- stage 2 会在当前 tile 计算时预取下一所需数据，使用 wait/sync；
- stage 不是任意整数的通用队列接口。

证据：[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:201-246,881-946]。**已确认**。

## 5. 同步和执行上下文

代表路径使用 `CP_ASYNC_WAIT_GROUP` 与 `__syncthreads()` 保证 shared-memory 数据可读。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:201-246]。warp-level row reduction、MMA 和 `__shfl_sync` 依赖参与线程满足模板假设。

代码在 load 计算出的 Q row 超出 sequence length 时有 block-level early return：[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:151-154]。结合后续协作加载和同步，非整 tile 的行为不能仅凭 `div_ceil` 推断安全，状态为**已确认风险/未验证**。

Python benchmark 负责 warmup、同步、计时和 reference comparison 的外部观察。[kernels/flash-attn/flash_attn_mma.py:285-413]。当前代表 binding 的统一 launch-error、stream、rank/device/contiguous 契约未完整确认。

## 6. 错误路径

| 失败点 | 行为/证据 | 状态 |
|---|---|---|
| 未支持 head dimension | `TORCH_CHECK`/异常文本 `headdim not support!` | 已确认 |
| sequence 非整 tile | `assert(QKV_seqlen % max(Br,Bc)==0)` | 已确认 |
| stage 参数 | 仅折叠为 1 或 2 | 已确认 |
| block early return + sync | 可能形成协作参与不一致 | 风险，未运行 |
| shared memory 超限 | launcher 设置动态上限；可行性未实测 | 风险，未运行 |
| 数值误差 | Python 与 torch/SDPA/official FA 比较路径存在 | 已确认框架；结果未验证 |
| CUDA launch error | 当前代表 binding 未见统一检查 | 未知/需逐函数审计 |

## 7. 资源与清理

Python tensor 和动态扩展的主体生命周期由 PyTorch 管理；kernel 不拥有 Q/K/V/O 的 storage。扩展 cache、临时 reference tensor 和 benchmark 对象由脚本/allocator 管理。CuTe/TMA 变体可能有额外 descriptor/TMA 生命周期，但本文代表 tiling-QKV 不将其推断到所有变体。

## 8. Demo 映射

- D01 用 NMS 展示 Python dynamic extension 的加载、tensor 借用、同步观察和错误边界；不覆盖 attention 算法。
- D02 的 Interview FlashAttention benchmark 展示显式 stream/event、dynamic shared memory、D2H correctness 和资源释放，证据 `[kernels/interview/notes-v2.cu:3094-3190,3400-3497]`。
- M07 Python 命令当前仅是未验证执行配方。
