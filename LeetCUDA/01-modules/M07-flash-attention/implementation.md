# M07 实现：FlashAttention MMA tiling-QKV 路径

## 1. 证据范围

本文聚焦 `flash_attn_mma.py → pybind/flash_attn.cc → mma/basic/flash_attn_mma_tiling_qkv.cu` 的代表路径。split-Q、split-KV、shared-QKV、swizzle、CuTe/TMA 和 F32 accumulator 变体只在差异表中标注，不能把 tiling-QKV 的 layout 自动套用到所有变体。

## 2. 构建与调用入口

`flash_attn_mma.py` 定义 shape/reference/benchmark CLI。[kernels/flash-attn/flash_attn_mma.py:22-55] **已确认**。脚本的 source builder 收集 basic、swizzle、CuTe 和 PyBind 源文件。[kernels/flash-attn/flash_attn_mma.py:80-138] **已确认**。CUDA flags 根据设备名和参数设置 `BUILD_FLASH_ATTN_MMA_L20`、`BUILD_FLASH_ATTN_MMA_4090`、`BUILD_FLASH_ATTN_MMA_3080` 等宏。[kernels/flash-attn/flash_attn_mma.py:150-203] **已确认**。动态入口为 `torch.utils.cpp_extension.load(...)`。[kernels/flash-attn/flash_attn_mma.py:220-227] **已确认**。

PyBind 层声明和导出 split-KV、split-Q、shared-KV、shared-QKV、tiling-QK/QKV、swizzle、F32 accumulator 和 CuTe 等函数。[kernels/flash-attn/pybind/flash_attn.cc:7-223] **已确认**。

```text
flash_attn_mma.py
  ├─ build sources/flags
  ├─ load flash_attn_lib
  ├─ run selected tags
  └─ compare with torch SDPA / official flash-attn / unfused torch
```

## 3. 输入布局和 block 责任

代表 kernel 文件头说明 Q/K/V/O 均为 `[batch_size, num_heads, seq_len, head_dim]`，一个 block 处理一个 Q tile，K/V sequence tile 在 block 内循环。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:2-16] **已确认**。

模板参数约束 MMA atom 必须是 `m16n8k16`，stage 只能是 1 或 2，padding 是 8 的倍数，`Br >= Bc`，且输出 accumulator storage 必须在 F16/F32 间显式选择。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:50-99] **已确认**。

代表几何：

- `Br = 16 * kMmaTileSeqLenQ`；
- `Bc = 8 * kWarpTileSeqLenK`；
- `kNumThreads = 32 * kMmaTileSeqLenQ`；
- grid.x 为 Q tile；grid.y 为 `batch * head`；
- `warp_QP` 负责 Q/P；`warp_KV` 固定为 0 表示共享 KV。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:94-124] **已确认**。

global offset 直接按 B/H/N/D 展开线性地址。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:125-132] **已确认**。因此输入必须匹配连续的四维布局；binding 层是否统一检查 rank/device/contiguous，在当前已读区间未见，状态为**未知/需逐函数核对**。

## 4. Shared-memory 复用

shared memory 布局：

```cpp
half *Q_tile_smem = smem;
half *K_tile_smem = Q_tile_smem + kStage * Q_tile_size;
half *V_tile_smem = Q_tile_smem;
```

证据：[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:156-170]。Q 和 K 使用独立区域，而 V 复用 Q 的区域。**已确认**。

```text
阶段 1: Q_tile_smem holds Q, K_tile_smem holds K
阶段 2: Q no longer needed in smem; V_tile_smem aliases Q_tile_smem
```

**推断**：这种复用降低 shared-memory footprint，但要求 QKᵀ 阶段之后、P@V 之前的同步和顺序正确；若重排代码或加 early-return，需要重新审计。

## 5. Register 状态

代表 kernel 使用：

- `lane_block_row_max_old` 和 `lane_block_row_sum_old` 保存 online softmax 历史状态；
- `R_Q`、`R_K`、`R_V` 保存 MMA 输入 fragment；
- `R_S` 先保存 logits，随后就地变为概率 P；
- `R_O`/`R_D` 保存输出 accumulator；
- `kOStorageAccFloat32` 决定部分输出 accumulator 的存储位宽。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:172-200] **已确认**。

这说明 FlashAttention 的核心不是生成完整 attention matrix，而是在寄存器和 shared memory 中流式保留每个 Q row 的 `m/l/O` 状态。

## 6. QKᵀ → online softmax → P@V

### 6.1 K/V sequence loop

kernel 沿 K/V sequence tile 循环；stage > 1 时提前加载 Q/K，随后 wait/sync。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:201-246] **已确认**。

### 6.2 QKᵀ

在 head_dim loop 中，Q 用 `LDMATRIX_X4`、K 用 `LDMATRIX_X2` 加载，然后用 `HMMA16816` 执行 Q@Kᵀ。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:248-391] **已确认**。

### 6.3 Online softmax

源码从 `R_S` logits 中计算 row max，用四 lane 组做 row reduction，再计算 `exp(scale*S-row_max)` 和 row sum，并把 `R_S` 就地转换为 P。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:401-488] **已确认**。

数学状态：

```text
m_new = max(m_old, m_tile)
l_new = exp(m_old - m_new) * l_old + l_tile
O_new = exp(m_old - m_new) * O_old + P_tile @ V_tile
```

源码中对旧 O 的 rescale 和最终 `1/l_final` 归一化分别出现在 `[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:537-570]`、`[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:708-721]`。**已确认**。

### 6.4 V 和 P@V

V 通过 shared-memory 路径加载，随后 `LDMATRIX_X2_T` 读入寄存器，`HMMA16816` 执行 P@V。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:490-513,620-658] **已确认**。

## 7. 输出写回

输出写回通过 `__shfl_sync` 在四 lane 组内收集 fragment，`lane_id % 4 == 0` 的 lane 执行 128-bit store，并复用 `R_Q/R_K` 作为临时寄存器。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:727-797] **已确认**。

这一路径要求参与 lane 和 head_dim/tile 边界满足模板假设。**未知**：非整 tile、非 contiguous layout 或不同 stream 下的错误传播没有在本文中被运行验证。

## 8. Launcher 与约束

launcher 根据 `kHeadDim` 决定 Q/K/P/V tile；`kHeadDim < 128` 和 `>=128` 使用不同 tile 组合，`kOStorageAccFloat32 = (kHeadDim < 256) ? 1 : 0`。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:799-879] **已确认**。

硬约束：

```cpp
assert(QKV_seqlen % max(Br, Bc) == 0);
```

证据：[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:845-849]。这意味着代表变体不应被文档描述为自然支持任意 sequence length。**已确认**。

head dimension dispatch 支持 32、64、96、128、256、512、1024；其他值抛出 `headdim not support!`。`stages > 1` 会映射到 stage 2，否则 stage 1。[kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:881-946] **已确认**。

## 9. 变体差异表

| 变体族 | 可确认共同点 | 不能直接继承的字段 |
|---|---|---|
| split-KV | 由 PyBind 暴露并参与 benchmark/tag | block 责任、reduce 归并、输出布局 |
| split-Q | 由 PyBind 暴露 | warp 分工、Q tile 数、共享 KV 策略 |
| shared-KV/shared-QKV | 目标是减少 shared load/通信 | alias 关系和 sync 点 |
| tiling-QK/tiling-QKV | QK/PV 更细粒度 tiling | Br/Bc、register 数组形状 |
| swizzle Q/QK/QKV | 改善 shared/global 访问 | 地址公式、layout contract |
| F32 accumulator | PyBind 有独立导出 | accumulator 位宽、store、误差阈值 |
| CuTe/TMA | 使用 CUTLASS/CuTe/TMA 抽象 | 架构、descriptor、TMA map 生命周期 |

## 10. 当前未验证边界

- GPU 构建、运行、benchmark、profile 全部未执行；
- rank/device/dtype/contiguous 检查未在代表 binding 中完整确认；
- `QKV_seqlen` 非 `max(Br,Bc)` 倍数；
- `D` 不在 dispatch 表；
- stage 2 在不同架构上的 shared-memory 可行性；
- official flash-attn、SDPA、unfused torch 的安装和误差比较。
