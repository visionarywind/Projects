# M06 接口

- 文档目的：解释 01-modules/M06-hgemm-tensorcore/interfaces.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-hgemm-tensorcore/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


Python CLI 选择 `--wmma`、`--mma`、`--cuda`、`--cute-tn` 及 `--*-all` 等家族，并传递 M/N/K、warmup、iters 等参数。[kernels/hgemm/hgemm.py:18-177]

扩展由 `CUDAExtension(name="toy_hgemm", ...)` 构建，include 和 NVCC flags 覆盖多个实现目录。[kernels/hgemm/setup.py:42-67]

实现之间的 layout、accumulator、可用 SM 和 stride 不是统一 ABI，必须按导出函数核对。

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
