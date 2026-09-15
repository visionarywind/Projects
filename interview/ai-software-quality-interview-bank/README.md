# AI 辅助软件工程与质量面试题库

本题库把 AI vibe-coding、Agent 协作和软件工程质量结合起来，覆盖从需求与提示词、代码库理解、代码生成，到测试、审查、发布、观测、排障、安全和治理的完整闭环。

## 当前状态

- 目标规模：入门、中等级、高级、专家级各 101 题，共 404 题。
- 稳定编号：AQ-B001–AQ-B101、AQ-I001–AQ-I101、AQ-A001–AQ-A101、AQ-E001–AQ-E101。
- 交付方式：一次性全量生成；每级 101 题，其中前 100 题按十类题型各 10 题，第 101 题为端到端综合题；状态见 00-overview/generation-state.md。
- 事实规则：生成代码、模型能力、性能、安全和实验结论需按 [已确认]、[推断]、[待验证]、[建议] 或 [存在争议] 标注。

## 导航

- [领域总览](00-overview/domain-overview.md)
- [知识地图](00-overview/knowledge-map.md)
- [能力地图](00-overview/competency-map.md)
- [覆盖矩阵](00-overview/coverage-matrix.md)
- [术语表](00-overview/terminology.md)
- [资料与证据](00-overview/sources-and-evidence.md)
- [入门题](01-question-bank/beginner.md)
- [中等级题](01-question-bank/intermediate.md)
- [高级题](01-question-bank/advanced.md)
- [专家题](01-question-bank/expert.md)
- [知识域详解](02-knowledge-domains/README.md)
- [实践实验](03-practice/hands-on-labs.md)
- [排障案例](03-practice/debugging-cases.md)
- [系统设计](03-practice/system-design-cases.md)
- [模拟面试](03-practice/mock-interview-routes.md)
- [质量审计](04-review/quality-audit.md)
- [30 天路线](99-roadmap/30-day-plan.md)

## 使用方式

按 AQ-B → AQ-I → AQ-A → AQ-E 学习。每题先给结论，再解释证据、约束、反例和验证；AI 生成的代码必须经过人工审查、测试、最小权限检查和可回滚发布。

## 端到端质量与稳定性开放题

新增 AQ-B101、AQ-I101、AQ-A101、AQ-E101，沿同一案例递进训练：风险识别 → 测试/审查/发布流程 → 质量保障平台 → 质量稳定性治理体系。
