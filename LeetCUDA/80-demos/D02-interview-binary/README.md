# D02 Interview binary：多架构、显式资源和整合 benchmark

- 文档目的：解释 80-demos/D02-interview-binary/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D02-interview-binary/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## Demo 目标

D02 以仓库真实的 `build.sh` 和 `notes-v2.cu` 为入口，展示 standalone CUDA 程序从架构选择、编译/链接、CLI phase 到 host/device buffer、kernel、同步、D2H、误差和释放的完整路径。它同时连接 interview 内的 SGEMM/HGEMM/FlashAttention 章节，但不把单文件整合入口误读成统一生产 runtime。

- 目标版本：`main` / `4513b31`。
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

## 文档元数据（规范补充）

- 文档目的：说明 `80-demos/D02-interview-binary/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

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
