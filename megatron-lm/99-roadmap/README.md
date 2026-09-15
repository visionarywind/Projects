# 开发实践层

- 文档目的：解释 99-roadmap/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


本目录把 Megatron-LM / Megatron Core 的源码分析转换为阅读、调试、测试、修改和面试复习路径。涉及 GPU、NCCL、多节点、模型训练或性能的数据，若没有实际执行记录，都应标记为“未验证”。

## 文档导航

- [分级 QA 题库](qa.md)
- [入门级 100 题](qa-beginner.md)
- [中级 100 题](qa-intermediate.md)
- [高级 100 题](qa-advanced.md)
- [专家级 100 题](qa-expert.md)

## 题库结构

每题都包含参考答案、小白解释、技术分析和拓展分析，覆盖 Transformer 训练基础、Megatron Core 模型装配、多维并行、pipeline 调度、优化器、分布式 checkpoint、性能诊断和系统级故障分析。

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
