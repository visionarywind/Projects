# PyTorch 领域专家级面试题库

本题库面向希望从 PyTorch 基础使用进阶到框架原理、分布式训练、编译优化、性能分析和工程决策的学习者。资料基线为 PyTorch `v2.14.0`（2026-09-02 发布）；版本差异、硬件结果和未运行实验必须按事实标签标注。

## 当前状态

- 目标规模：入门、中等级、高级、专家级各 100 题，共 400 题。
- 当前进度：每级已生成 100 题，共 400 题；每级十类题型各 10 题，详见生成状态、覆盖矩阵和质量审计。
- 完成状态：见 [`00-overview/generation-state.md`](00-overview/generation-state.md)。

## 目录

- [领域总览](00-overview/domain-overview.md)
- [知识地图](00-overview/knowledge-map.md)
- [能力地图](00-overview/competency-map.md)
- [覆盖矩阵](00-overview/coverage-matrix.md)
- [知识域详解](02-knowledge-domains/README.md)
- [术语表](00-overview/terminology.md)
- [资料与证据](00-overview/sources-and-evidence.md)
- [入门级题目](01-question-bank/beginner.md)
- [中等级题目](01-question-bank/intermediate.md)
- [高级题目](01-question-bank/advanced.md)
- [专家级题目](01-question-bank/expert.md)
- [实践实验](03-practice/hands-on-labs.md)
- [故障排查案例](03-practice/debugging-cases.md)
- [系统设计案例](03-practice/system-design-cases.md)
- [模拟面试路线](03-practice/mock-interview-routes.md)
- [质量审计](04-review/quality-audit.md)
- [常见误区](04-review/common-mistakes.md)
- [追问索引](04-review/follow-up-index.md)
- [快速复习](04-review/flash-review.md)
- [专家自检](04-review/expert-checklist.md)
- [学习路线](99-roadmap/30-day-plan.md)

## 难度与编号

| 难度 | 编号范围 | 训练重点 |
|---|---|---|
| 入门级 | `B001`–`B100` | 张量、自动微分、模块、数据和最小实验 |
| 中等级 | `I001`–`I100` | 组件协作、训练流程、测试和常见故障 |
| 高级 | `A001`–`A100` | 分布式、性能、编译、容量和工程落地 |
| 专家级 | `E001`–`E100` | 原理审视、极端场景、演进、权衡和技术治理 |

每题使用统一模板，包含背景知识、问题分析、具体答案、技术洞察、图文说明、拓展知识、追问、反例、实践/验证和评分标准。事实状态使用 `[已确认]`、`[推断]`、`[待验证]`、`[建议]`、`[存在争议]`。

## 资料基线与限制

主要依据为 PyTorch 官方文档、发布说明、源码标签和公开设计资料。当前工作区没有 PyTorch 源码副本，因此源码行号引用先标为 `[待验证]`，不伪造具体行号；GPU、通信和性能结论也不会在没有运行记录时写成已验证事实。
