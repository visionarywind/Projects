# M06 实现：HGEMM 从入口到 Tensor Core 写回

- 文档目的：解释 01-modules/M06-hgemm-tensorcore/implementation.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-hgemm-tensorcore/implementation.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 1. 证据范围和阅读方式

- 对应提交：`main` / `4513b31`。
- 状态约定：**已确认**表示源码直接显示；**推断**表示由布局、模板或调用关系推导；**未知**表示没有在当前源码中看到或没有 GPU 实验支持。
- 本文以可追踪的代表路径为主，不把同目录中名字相似的 kernel 当作相同布局。

## 2. 从 Python 选项到真正 kernel

### 2.1 动态扩展路径

`hgemm.py` 暴露矩阵尺寸、warmup/iteration、MMA/WMMA/cuBLAS/CuTe 等开关；这只是选择层，不是算法实现本体。[kernels/hgemm/hgemm.py:18-177] **已确认**

`tools/utils.py:get_build_sources()` 把 naive、cuBLAS、WMMA、MMA stage、TN/swizzle、CuTe 和 PyBind 源文件放入同一构建源集合。[kernels/hgemm/tools/utils.py:18-32] **已确认**。动态加载优先尝试已安装 wheel，失败后构建源码扩展。[kernels/hgemm/tools/utils.py:109-147] **已确认**。

PyBind 层导出的是一组不同名字的函数；函数名携带 MMA atom、tile、stage、swizzle、TN 或 accumulator 信息，因此调用者必须继续追到对应 `.cu` 的 launcher/kernel。[kernels/hgemm/pybind/hgemm.cc:7-121,123-181] **已确认**。

```text
hgemm.py 参数
  └─ tools/utils.py: sources + CUDA flags
      └─ PyBind 导出函数
          └─ 对应 launcher / 模板实例
              └─ global A/B → shared → registers → MMA
                  └─ accumulator → global C
```

### 2.2 两类所有权模型

| 路径 | 输入/输出所有权 | 临时资源 | 错误/同步边界 |
|---|---|---|---|
| Python 动态扩展或 wheel | PyTorch tensor 由调用者持有，kernel 借用 `data_ptr`；输出通常由 PyTorch 分配 | PyTorch allocator、可能的 cuBLAS handle | Python benchmark 显式 warmup/synchronize；每个 kernel 是否显式检查需逐实现核对 |
| standalone makefile/interview 类路径 | host/device buffer 由 C/C++ 显式分配 | `cudaMalloc`、events、handle | 可见 `cudaGetLastError`/sync/check 的实现取决于具体 benchmark |

Python benchmark 中 cublas tag 在计时前初始化、结束后销毁，且 warmup 和 measured iterations 都同步。[kernels/hgemm/hgemm.py:210-328] **已确认**。这不等于所有自定义 kernel 都具有相同的 launch-error 检查；这是模块级**未知/需逐函数核对**的边界。

## 3. 代表实现：128×128 staged MMA

### 3.1 编译期几何

`hgemm_mma_m16n8k16_mma2x4_warp4x4_stages_kernel` 使用 `mma.sync.aligned.m16n8k16` 作为 Tensor Core 原子，模板参数进一步决定每个 warp 的 M/N tile、A/B padding、stage 数和 block swizzle。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:120-128] **已确认**。

源码 launcher 的代表常量为 `BM=128`、`BN=128`、`BK=16`、256 threads；每个 block 负责一个 128×128 的 C tile，并沿 K 维遍历 16-wide tile。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1900-1959] **已确认**。

```text
C block tile (128×128)
┌──────────────────────────────┐
│ warp (m=0,n=0) ...           │  8 warps / 256 threads
│ ...                          │
└──────────────────────────────┘
       ▲
       └─ each K step: A[128×16] × B[16×128]
```

**推断**：模板中 `MMA_TILE_M=2`、`MMA_TILE_N=4` 和 `WARP_TILE_M/WARP_TILE_N=4` 表示一个 warp 内由多个 `m16n8k16` 原子覆盖更大的 warp fragment；精确 lane-to-fragment ABI 由 `LDMATRIX`/`HMMA` PTX 约定，不应仅用普通二维线程公式替代。

### 3.2 Shared-memory stage

普通 staged 路径声明二维 stage 缓冲：

```cpp
__shared__ half s_a[K_STAGE][BM][BK + A_PAD];
__shared__ half s_b[K_STAGE][BK][BN + B_PAD];
```

证据：[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:137-142]。A/B 各自有 `K_STAGE` 份缓冲，额外 padding 改变行步长，目标是改善 shared-memory bank access；padding 的具体收益仍需 profiler 验证。**已确认/性能效果未验证**。

在 K 循环中，`smem_sel=(k+1)%K_STAGE` 选择下一阶段写入位置，`smem_sel_next=k%K_STAGE` 选择当前计算位置；因此 stage parity 是循环索引的一部分，而不是运行时队列对象。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:769-839] **已确认**。

### 3.3 Global→shared 的异步流水

预取阶段用 `CP_ASYNC_CG`/相关宏把 A/B 的 16-byte 片段送入 shared memory，随后 `CP_ASYNC_COMMIT_GROUP()` 提交 group；等待 `CP_ASYNC_WAIT_GROUP(K_STAGE-2)` 后再 `__syncthreads()`，使 shared-memory 内容对整个 block 可见。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:691-729] **已确认**。

```text
时间 ─────────────────────────────────────────────►
stage k       cp.async(A/B,k+1) ──┐
                                   ├─ wait + block sync
                                   └─ ldmatrix(k) → HMMA(k)
stage k+1     cp.async(A/B,k+2) ────────────────┐
                                                └─ 与 HMMA(k) 重叠
```

**推断**：stage 越多可以容纳更多在途搬运，但 shared memory、寄存器和 occupancy 的代价也上升；源码只给出配置和容量，不提供跨 GPU 的最优 stage 结论。

### 3.4 Shared→register→MMA

每个线程的地址计算先由 `tid` 派生 `warp_id`、`lane_id`、`warp_m/warp_n` 以及 A/B 的 shared-memory load 坐标；dsmem 变体显式展示了这些映射。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:663-674] **已确认**。

`LDMATRIX_X4`/`LDMATRIX_X2_T` 等宏将 shared-memory fragment 装入寄存器；`HMMA16816` 再用 `mma.sync.aligned.m16n8k16` 累加到 `RC`。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:769-839] **已确认**。

```text
shared A (lane-mapped) ──ldmatrix──> RA[buffer][warp-M][fragment]
shared B (lane-mapped) ──ldmatrix──> RB[buffer][warp-N][fragment]
                                   │
                                   └─ HMMA16816 ──> RC[M][N][2]
```

## 4. dsmem/k32 与 register double buffer

### 4.1 动态 shared memory

`hgemm_mma_m16n8k16_mma2x4_warp4x4x2_stages_dsmem_kernel` 使用 `extern __shared__ half smem[]`，手工把 `s_a` 和 `s_b` 切成连续区域，并通过 stage/stride offset 访问。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:632-661] **已确认**。

这种路径不是简单地把静态数组改成动态数组：`WARP_TILE_K`、MMA-K store offset 和 stage offset 都参与地址布局。修改其中一个模板参数必须同步检查 launcher 的 shared-memory 字节数，否则可能得到 launch failure 或越界；当前没有 GPU 实验确认所有组合。**已确认风险/运行结果未知**。

### 4.2 线程和 warp

源码直接计算：

```cpp
const int tid = threadIdx.y * blockDim.x + threadIdx.x;
const int warp_id = tid / WARP_SIZE;
const int lane_id = tid % WARP_SIZE;
const int warp_m = warp_id % 2;
const int warp_n = warp_id / 2;
```

并将线程映射到 A 的 M/K 和 B 的 K/N 片段。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:663-674] **已确认**。这说明 block 的 warp 排列与 C tile 的二维位置绑定；不应把 `threadIdx.x` 单独当作全局矩阵坐标。

### 4.3 寄存器双缓冲

累加器和输入 fragment 的形状在源码中分别体现为：

```cpp
uint32_t RC[WARP_TILE_M][WARP_TILE_N][2];
uint32_t RA[2][WARP_TILE_M][4];
uint32_t RB[2][WARP_TILE_N][2];
int reg_store_idx = 0;
int reg_load_idx = 1;
```

证据：[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:678-686,731-735]。每个 K 迭代翻转 `reg_store_idx/reg_load_idx`，令一组寄存器保存当前/下一 fragment；这是寄存器级软件流水，不是额外 CUDA stream。**已确认**。

## 5. 输出写回：fragment 到 C

普通路径和 dsmem 路径不会直接让每个 lane 写完整 C tile。collective store 先通过 `__shfl_sync` 在四 lane 组内交换 accumulator fragment，再由 `lane_id % 4 == 0` 的 lane 用 128-bit store 写回。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1850-1885] **已确认**。

```text
lane 0 ─┐
lane 1 ─┼─ shuffle fragment → lane 0 owns 4 values → LDST128BITS(C)
lane 2 ─┤
lane 3 ─┘
```

这段写回依赖 warp 内 lane 活跃且 mask 正确；它不是通用的任意线程子集 API。**推断**：若未来加入边界早退或改变 warp participation，必须重新审计 `__shfl_sync` 的参与 mask。

sm90+ 的 `stmatrix` 相关 collective-store 分支在源码注释中标为尚未测试；这是**源码声明已确认、行为未知**，不能从模板存在推断为可运行。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:536-567]

## 6. Launcher、容量和架构分支

launcher 会调用 `cudaFuncSetAttribute(...MaxDynamicSharedMemorySize, 98304)`，计算 `smem_max_size`，再以 `<<<grid, block, smem_max_size>>>` 启动 dsmem kernel。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1900-1926] **已确认**。源码随后枚举 stage 及 padding/stride 组合。[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1930-1959]。

**未知**：当前环境的 device shared-memory 上限、attribute 是否成功、具体 M/N/K 尾 tile 是否安全，均未执行验证。HGEMM 文档因此把“支持 stage 2/3/4/5”理解为源码有 launcher 配置，不理解为所有 GPU 都可运行。

## 7. 变体边界

| 族 | 主要抽象 | 需要单独确认的事实 |
|---|---|---|
| naive | 直接 CUDA Core 乘加 | 边界与 stride |
| WMMA | warp fragment API | fragment layout、arch guard |
| MMA | PTX `m16n8k16` | lane mapping、A/B layout、store |
| swizzle/staged | stage + padding/swizzle | bank conflict、smem 容量、尾 tile |
| TN | 转置/column-major 约定 | Python 转换和 kernel stride |
| CuTe | CUTLASS/CuTe layout/tile | 第三方 header 版本和实例化 |
| WGMMA | warp-group Hopper 指令 | SM90a、TMA/descriptor 和编译条件 |
| cuBLAS | NVIDIA library baseline | handle、layout、workspace、同步 |

表中的“需要单独确认”不是实现缺陷结论，而是不同 kernel family 不能共享同一输入契约的审计清单。

## 8. 当前未验证边界

- M/N/K 非 128/16 tile 倍数；
- 非 contiguous tensor、current stream 和异步错误传播；
- F16/F32 accumulator 的数值误差；
- dynamic shared-memory attribute 和 stage 5；
- sm_80、sm_89、sm_90a 的实际编译/运行；
- sm90 `stmatrix` 分支。

以上仅可作为测试计划，不能写成已支持矩阵。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
