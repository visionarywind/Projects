# 技术债务

- 文档目的：解释 99-roadmap/technical-debt.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/technical-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


1. M01 还需逐 backend 分析 local/TE/inference attention 与 MLP。
2. M02 还需逐分支覆盖交错 pipeline 和 overlap P2P。
3. M03 还需补全 fault tolerance、async checkpoint、full CUDA graph。
4. M04 还需对真实 indexed dataset 和 SFT/packing 分支做深入分析。
5. M05 还需 round-trip 实测和异步保存清理流程。
6. M06 还需选择代表 Demo 并运行 offline/server smoke。
7. 跨模块文档需要随源码行号精化。

这些债务不阻塞当前知识库导航，但会影响把文档标记为“已完成”。

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
