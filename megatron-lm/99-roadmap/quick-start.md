# 快速上手

- 文档目的：解释 99-roadmap/quick-start.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/quick-start.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


1. 先读根 README 的 5 分钟快速理解。
2. 读 D01：理解 Core API 的最小训练链。
3. 读 M02：理解 TP/PP/DP/CP/EP group 和 schedule。
4. 读 M03：理解正式 `pretrain` 如何编排模型、数据、优化器。
5. 根据任务进入 M01/M04/M05/M06。

## 不建议直接做的事

- 不要在 docs-only checkout 根目录运行上游训练命令。
- 不要把未运行的 GPU/NCCL 命令写成已验证。
- 不要修改原始源码目录，除非任务明确要求。

## 最小验证顺序

```text
静态阅读 -> markdown diff/check -> D01 torchrun -> focused unit tests -> 正式 pretrain smoke
```

硬件相关命令需在上游源码和正确容器/依赖中运行。

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
