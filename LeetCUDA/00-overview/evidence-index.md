# 源码证据索引

- 文档目的：集中索引支撑关键结论的源码位置。
- 对应源码版本：`4513b31`。
- 证据状态：已确认索引；行号需随源码变更维护。
- 最后更新：2026-09-15

| 主题 | 证据 | 支撑结论 |
|---|---|---|
| 项目定位 | `README.md:15-16` | CUDA kernels、HGEMM、FlashAttention、博客主线 |
| 快速构建 | `README.md:53-105` | 子模块、CUDNN、SM 架构和 interview bench |
| 贡献边界 | `CONTRIBUTE.md:2-3,5-31` | 学习优先、pre-commit、新 kernel 流程 |
| 子模块 | `.gitmodules:1-5` | CUTLASS 与 cudnn-frontend 来源 |
| 普通扩展加载 | `kernels/elementwise/elementwise.py:9-24` | `load()` + CUDA flags |
| 普通 benchmark | `kernels/elementwise/elementwise.py:27-66` | warmup/sync/计时/输出 |
| elementwise launcher | `kernels/elementwise/elementwise.cu:140-189` | dtype、shape、grid/block、绑定 |
| NMS 输入契约 | `kernels/nms/nms.cu:126-150` | dtype/device/dim/empty |
| NMS phase 1 | `kernels/nms/nms.cu:22-71` | warp ballot bitmask |
| NMS phase 2 | `kernels/nms/nms.cu:85-109` | 单 block 顺序 resolve/shared sync |
| NMS 索引映射 | `kernels/nms/nms.cu:152-189` | stable score sort 与原始 index |
| NMS 对拍 | `kernels/nms/nms.py:87-121` | fixed/random/torchvision 回归 |
| HGEMM Python 参数 | `kernels/hgemm/hgemm.py:18-177` | 算法开关和尺寸 |
| HGEMM benchmark | `kernels/hgemm/hgemm.py:210-328` | cublas handle、sync、TFLOPS |
| HGEMM wheel | `kernels/hgemm/setup.py:42-95` | CUDAExtension、arch flags、依赖 |
| FlashAttention build | `kernels/flash-attn/flash_attn_mma.py:80-203,220-227` | sources/flags/宏/动态 load |
| FlashAttention benchmark | `kernels/flash-attn/flash_attn_mma.py:285-413` | 过滤、warmup、误差/TFLOPS |
| Interview 层次 | `kernels/interview/README.md:6-16` | common→base→GEMM→attention→notes |
| Interview build | `kernels/interview/build.sh:134-185` | compile/link/arch outputs |
| Interview error | `kernels/interview/notes-v2.cu:92-97` | CUDA error exit |
| SGEMM tile | `kernels/sgemm/sgemm.cu:36-86` | shared tile + K loop |
| SGEMM thread tile | `kernels/sgemm/sgemm.cu:88-166` | 128 tile、8x8 register tile、vec4 |
| SGEMM double buffer | `kernels/sgemm/sgemm.cu:347-482` | 双缓冲和同步策略 |
| M06 staged MMA geometry/pipeline | `kernels/hgemm/mma/basic/hgemm_mma_stage.cu:120-142,632-839` | 128×128 block tile, stages, dsmem mapping, cp.async/ldmatrix/HMMA |
| M06 collective store/SMEM launcher | `kernels/hgemm/mma/basic/hgemm_mma_stage.cu:1850-1959` | shuffle/vector store, dynamic SMEM, grid and stage dispatch |
| M06 untested architecture branch | `kernels/hgemm/mma/basic/hgemm_mma_stage.cu:536-567` | sm90 `stmatrix` is source-declared untested |
| M07 tiling-QKV layout/state | `kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:50-200` | Br/Bc, `[B,H,N,D]`, Q/K/V SMEM alias, register state |
| M07 QK/softmax/PV/store | `kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:201-797` | staged QK, online softmax, P@V, O normalization/store |
| M07 dispatch/boundary | `kernels/flash-attn/mma/basic/flash_attn_mma_tiling_qkv.cu:799-946` | D list, stage mapping, sequence full-tile assert |
| D01 NMS Demo | `80-demos/D01-nms-python/` | Python load, mask/resolve, state and failure trace |
| D02 Interview Demo | `80-demos/D02-interview-binary/` | arch build, explicit allocation, HGEMM/FA benchmark trace |
| Python tensor/extension allocation | `source/LeetCUDA/kernels/sgemm/sgemm.py:10-30,127-145` | PyTorch extension loading and benchmark tensor reuse |
| Interview SGEMM resource lifecycle | `source/LeetCUDA/kernels/interview/bench_sgemm.cu:14-22,41-72,108-210` | host/device allocation, events, handle and cleanup |
| Interview attention workspace/TMA | `source/LeetCUDA/kernels/interview/bench_attn.cu:100-137,620-710`; `source/LeetCUDA/kernels/interview/common.cuh:734-771` | cuDNN workspace, stream/event and TMA descriptor ownership |
| Graph/CPU pool boundary | `source/LeetCUDA/kernels/interview/bench_attn.cu:100-137`; `source/LeetCUDA/kernels/sgemm/sgemm.py:10-30,127-145` | 单次 benchmark/workspace 资源；无统一 graph 或 CPU pool 证据 |


每新增一条重要结论，优先在本表加入证据，再在具体文档附近引用；不要只引用 README 的性能数字来证明源码行为。

## 相关文档

- [analysis-state.md](analysis-state.md)
- [../01-modules/module-registry.md](../01-modules/module-registry.md)

## 源码证据摘要

本文件本身即为索引；代表性证据见上表。

## 未解决问题

深层 `.cuh`/Tensor Core 符号仍需按功能增量加入，当前以入口和构建证据为主。

## 下一步阅读建议

查询某一结论时先查本表，再打开对应源码。
