# 开发实践层

- 文档目的：解释 99-roadmap/README.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/sglang HEAD 78be4b50af（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


本目录把源码分析转换为可执行的阅读、调试、测试和修改路径。所有涉及 GPU、模型、通信或端到端输出的命令，若未在本批执行，都明确标记为“未验证”。

## 文档导航

- [快速上手](quick-start.md)
- [阅读路线](reading-guide.md)
- [调试指南](debugging-guide.md)
- [功能开发配方](feature-development-recipes.md)
- [测试配方](testing-recipes.md)
- [性能指南](performance-guide.md)
- [风险登记](risk-register.md)
- [技术债务](technical-debt.md)
- [后续步骤](next-steps.md)
- [分级 QA 题库](qa.md)：入门级、中级、高级、专家级各 100 题；每题包含参考答案、小白解释、技术分析和拓展分析。

## 当前边界

知识库以 checkout `78be4b50af88e9ea72d75b4c3a3e42b7297d2501` 为证据锚点。M03-M10 已有部分实现层文章，D01 仍是静态深度解剖；真实模型、GPU、CUDA Graph、多卡通信和完整测试尚未验证。

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
