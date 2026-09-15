# M06 源码地图

- 文档目的：解释 01-modules/M06-hgemm-tensorcore/source-map.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-hgemm-tensorcore/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 文件/目录 | 责任 | 深度状态 |
|---|---|---|
| `hgemm.py` | CLI、算法筛选、benchmark、TFLOPS | 已完成入口 |
| `tools/utils.py` | source 集合、flags、wheel/source fallback、layout helper | 已完成代表入口 |
| `setup.py` | CUDAExtension、架构和 include | 已完成构建边界 |
| `makefile` | standalone binary targets | 部分完成 |
| `pybind/hgemm.cc` | Python export boundary 和 variant 命名 | 已完成接口索引 |
| `wmma/` | WMMA implementations | 部分完成 |
| `mma/basic/hgemm_mma_stage.cu` | 代表 staged MMA、dsmem、collective store | 已完成深度代表 |
| `mma/basic/hgemm_mma_stage_tn.cu` | TN layout staged variants | 部分完成 |
| `mma/swizzle/` | padding/swizzle variants | 部分完成 |
| `wgmma/` | Hopper warp-group paths | 部分完成 |
| `cutlass/` | CuTe/CUTLASS paths | 部分完成 |
| `naive/`, `cublas/` | baseline/reference families | 入口已知、逐文件未完成 |

## 代表源码锚点

- Python/构建：`[kernels/hgemm/hgemm.py:18-177]`、`[kernels/hgemm/tools/utils.py:18-32,109-155]`、`[kernels/hgemm/setup.py:42-95]`。
- PyBind：`[kernels/hgemm/pybind/hgemm.cc:7-181]`。
- staged MMA template/shared：`[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:120-142,632-686]`。
- pipeline/MMA/store：`[kernels/hgemm/mma/basic/hgemm_mma_stage.cu:691-839,1850-1959]`。

本文中的“部分完成”表示源码目录或入口已定位，不表示该变体已构建或运行。

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
