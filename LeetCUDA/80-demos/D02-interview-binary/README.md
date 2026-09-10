# D02 Interview binary：多架构、显式资源和整合 benchmark

## Demo 目标

D02 以仓库真实的 `build.sh` 和 `notes-v2.cu` 为入口，展示 standalone CUDA 程序从架构选择、编译/链接、CLI phase 到 host/device buffer、kernel、同步、D2H、误差和释放的完整路径。它同时连接 interview 内的 SGEMM/HGEMM/FlashAttention 章节，但不把单文件整合入口误读成统一生产 runtime。

- 目标版本：`main` / `0983c65`。
- 当前状态：静态源码追踪已完成代表路径；编译、运行和 benchmark **未验证**。
- 相关模块：[M01](../../01-modules/M01-foundation/README.md)、[M05](../../01-modules/M05-gemv-sgemm/README.md)、[M06](../../01-modules/M06-hgemm-tensorcore/README.md)、[M07](../../01-modules/M07-flash-attention/README.md)、[M09](../../01-modules/M09-interview-benchmark/README.md)。

## 步骤

| ID | 内容 |
|---|---|
| D02-S01 | [构建与运行](build-and-run.md) |
| D02-S02 | [执行追踪](execution-trace.md) |
| D02-S03 | [数据/状态追踪](data-and-state-trace.md) |
| D02-S04 | [调试 walkthrough](debug-walkthrough.md) |
| D02-S05 | [失败路径](failure-paths.md) |
| D02-S06 | [修改练习](modification-exercises.md) |

## 真实构建入口

`build.sh` 固定 `/usr/local/cuda/bin/nvcc`，支持 `sm_86`、`sm_89`、`sm_90a`、`sm_120a`，为不同架构设置 gencode/defines，编译 `notes-v2.cu` 再链接架构相关 binary。[kernels/interview/build.sh:13-81,134-185] **已确认**。

## 代表路径

### BlockReduce test

`test_block_reduce` 展示 host malloc、CPU reference、cudaMalloc、H2D、cudaMemset、kernel launch、`cudaGetLastError`、device sync、D2H、误差计算和 free/cudaFree。[kernels/interview/notes-v2.cu:510-542] **已确认**。

### HGEMM benchmark

`bench_hgemm_tflops` / `launch_timed_hgemm_mma` 使用 128×128 tile、256 threads、stages/swizzle 组合、shared-memory feasibility、warmup、CUDA events、D2H correctness 和资源清理。[kernels/interview/notes-v2.cu:2420-2579] **已确认**。warmup 后的 `cudaDeviceSynchronize()` 有未包在 `check()` 中的差异，需作为审计项记录。

### FlashAttention benchmark

D=64/128 dispatch、Br/Bc、动态 shared memory、独立 timing stream、events、warmup、D2H、correctness、event/stream/TMA map 和 buffer 清理路径位于 `[kernels/interview/notes-v2.cu:3094-3190,3400-3497]`。**已确认源码结构，未运行**。

## 深度审计表

| 审计项 | 结论 | 状态 |
|---|---|---|
| 入口落地 | build.sh → notes-v2 binary → CLI → test/bench → kernel | 已完成 |
| 正常路径 | BlockReduce/HGEMM/FA 代表资源流可追踪 | 已完成 |
| 关键分支 | arch、phase、SMEM feasible、stage/swizzle、D/TMA | 部分完成 |
| 异常路径 | `check()`、skip infeasible/unaligned；所有 CLI 错误分支仍需完整核对 | 部分完成 |
| 清理 | host/device/event/stream/handle/TMA map 代表路径已记录 | 已完成 |
| 数据生命周期 | malloc→H2D→kernel→sync→D2H→error→free | 已完成 |
| 执行上下文 | default/timing stream、events、sync 已记录 | 部分完成 |
| 行级证据 | build、test、HGEMM、FA 代表锚点 | 已完成 |
| Demo 映射 | D02-S01…S06 与 M01/M05/M06/M07/M09 | 已完成 |
| 总体 | 源码级完整，运行矩阵和性能未验证 | 部分完成 |

## 不应声称

编译脚本存在不等于当前 nvcc/CUTLASS/cuDNN 可用；sm90a/sm120a 分支存在不等于本机可运行；benchmark 函数存在不等于性能数字已复现。
