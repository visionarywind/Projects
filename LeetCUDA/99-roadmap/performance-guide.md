# 性能指南

- 文档目的：解释 99-roadmap/performance-guide.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：`main` / `4513b31`。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-10
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/performance-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 优化顺序

1. 先确认输出正确。
2. 确认计时包含同步且排除首次编译。
3. 判断瓶颈是 global memory、shared memory、计算、同步还是 occupancy。
4. 再选择 vectorization、tiling、padding/swizzle、double buffer、Tensor Core 或融合。
5. 与 torch/cuBLAS/官方 FlashAttention 在相同条件下比较。

## 机制对照

- elementwise：pack/vector load。
- SGEMM：shared tile、thread tile、double buffer。
- HGEMM：WMMA/MMA/CuTe/WGMMA、padding/swizzle、stages。
- FlashAttention：融合、online softmax、Q/K/V tile 复用。
- NMS：mask 并行化受 ordered resolve 限制。

## 纪律

历史 benchmark 只用于学习趋势，不能作为当前硬件保证；提交性能改动时同时记录环境和 correctness。

证据：`[kernels/sgemm/sgemm.cu:88-482]`；`[kernels/hgemm/README.md:195-204,305-307]`；`[kernels/flash-attn/README.md:14-26]`。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
