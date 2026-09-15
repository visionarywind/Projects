# M06 HGEMM 术语

- 文档目的：解释 01-modules/M06-hgemm-tensorcore/glossary.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/LeetCUDA HEAD 4513b31（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-hgemm-tensorcore/glossary.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- `WMMA`：CUDA C++ 的 warp-level matrix multiply-accumulate API。
- `MMA`：更接近 PTX 指令形态的 matrix multiply-accumulate 路径。
- `WGMMA`：Hopper warp-group matrix multiply-accumulate 指令路径。
- `NN/TN`：A/B 矩阵布局组合；具体 stride 由实现和 launcher 约定。
- `stage`：用于重叠 tile 搬运和计算的 pipeline 缓冲阶段。
- `SMEM padding`：在 shared memory layout 中增加步长，降低 bank conflict。
- `SMEM swizzle`：重排 shared-memory 地址/线程映射以改善访问模式。
- `accumulator`：Tensor Core 乘加结果使用的寄存器或 fragment 存储。
- `CuTe/CUTLASS`：用于描述 layout、tile 和高性能矩阵运算的 NVIDIA 模板抽象。

这些术语不能替代具体 kernel 的实例化参数；实际 layout 和可用指令需以目标架构源码为准。

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
