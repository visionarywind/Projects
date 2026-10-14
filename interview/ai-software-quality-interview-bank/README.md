# AI 辅助软件工程与质量面试题库

本题库把 AI vibe-coding、Agent 协作与软件工程质量结合起来，覆盖从需求与提示词、代码库理解、代码生成，到测试、审查、发布、观测、排障、安全和治理的完整闭环。

## 当前状态

- 目标规模：入门、中等级、高级、专家级各 15 题，共 60 题（精选题，非批量凑数）。
- 稳定编号：`AQ-B001`–`AQ-B015`、`AQ-I001`–`AQ-I015`、`AQ-A001`–`AQ-A015`、`AQ-E001`–`AQ-E015`。
- 知识域：K01–K12 共 12 个域，见 `02-knowledge-domains/`。
- 事实规则：生成代码、模型能力、性能、安全和实验结论按 `[已确认]`、`[推断]`、`[待验证]` 标注；本仓库无 AI 软件工程工具链源码沉淀，运行类结论以 `[待验证]` 为准。

## 导航

- 题库：[入门题 15](01-question-bank/beginner.md) · [中等级题 15](01-question-bank/intermediate.md) · [高级题 15](01-question-bank/advanced.md) · [专家题 15](01-question-bank/expert.md)
- 知识域：[总览](02-knowledge-domains/README.md)
- 概览：[领域总览](00-overview/domain-overview.md) · [知识地图](00-overview/knowledge-map.md) · [能力地图](00-overview/competency-map.md) · [覆盖矩阵](00-overview/coverage-matrix.md) · [术语表](00-overview/terminology.md) · [资料与证据](00-overview/sources-and-evidence.md)
- 实践：[实践实验](03-practice/hands-on-labs.md) · [排障案例](03-practice/debugging-cases.md) · [系统设计](03-practice/system-design-cases.md) · [模拟面试](03-practice/mock-interview-routes.md)
- 复习：[质量审计](04-review/quality-audit.md) · [常见错误](04-review/common-mistakes.md) · [专家清单](04-review/expert-checklist.md)
- 路线：[30 天路线](99-roadmap/30-day-plan.md) · [90 天路线](99-roadmap/90-day-plan.md)

## 知识域（K01–K12）

| 域 | 名称 |
|---|---|
| K01 | vibe-coding 生命周期与人机协作 |
| K02 | 提示词、上下文与代码库理解 |
| K03 | Agent 规划、工具与编排 |
| K04 | 生成代码正确性与可维护性 |
| K05 | 测试生成、评测与回归 |
| K06 | Code Review、静态分析与门禁 |
| K07 | CI/CD、发布、灰度与回滚 |
| K08 | 可观测性、调试与故障排查 |
| K09 | 安全、隐私、注入与供应链 |
| K10 | 可靠性、性能、容量与成本 |
| K11 | 架构、接口、技术债与演进 |
| K12 | 质量度量、合规与组织治理 |

## 使用方式

按 `AQ-B` → `AQ-I` → `AQ-A` → `AQ-E` 学习。每题先给结论，再解释证据、约束、反例和验证；AI 生成的代码必须经过人工审查、测试、最小权限检查和可回滚发布。
