# 开发实践层

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
- [分级 QA 题库](qa.md)：入门级 100 题、中级 50 题、高级 50 题，覆盖大模型推理与 SGLang 源码理解。

## 当前边界

知识库以 checkout `f1a512c51c73ab660cf41e1af3110c7c11e3b600` 为证据锚点。M03-M10 已有部分实现层文章，D01 仍是静态深度解剖；真实模型、GPU、CUDA Graph、多卡通信和完整测试尚未验证。
