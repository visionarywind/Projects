# AI Runtime / Device Software 面试资料

> 面向具有多年系统软件、深度学习框架、设备软件、内存管理和网络经验的高级/资深候选人。目标是把已有经历组织成可信技术叙事，并补齐 AI Infra 面试中的原理、实现、排障和系统设计能力。

## 求职定位

**主线：** AI Framework / Device Runtime，聚焦 Memory、Stream、异步执行与性能优化。

**次线：** 集合通信、高性能网络、RDMA、分布式训练基础设施。

**扩展：** LLM Inference Runtime、KV Cache、Serving、高性能 IO 和算力平台。

用户自述的 13 年系统软件经历、MindSpore、UMD/设备软件、PyTorch 显存池、多 Stream、Host 网络、检索引擎和 Java 服务端经验，作为 `[P-已确认]` 的方向标签；具体职责、代码、版本、指标和事故仍需 `[P-待补充]`，不可编造。

## 题库总览

| 难度 | 题目 | 内容 |
|---|---|---|
| 入门级 | B001–B100 | 概念、边界、基础调用链和验证 |
| 中等级 | I001–I100 | 实现、生命周期、排障和容量 |
| 高级 | A001–A100 | 系统设计、性能、可靠性和迁移 |
| 专家级 | E001–E100 | 跨组件治理、演进、SLO、成本和事故复盘 |

每题包含十段：背景知识、问题分析、具体答案、技术洞察、图文说明、拓展知识、追问、易错点、实践与验证、评分标准。

## 导航

- [领域总览](00-overview/domain-overview.md)
- [知识地图](00-overview/knowledge-map.md)
- [能力地图](00-overview/competency-map.md)
- [覆盖矩阵](00-overview/coverage-matrix.md)
- [术语与易混概念](00-overview/terminology.md)
- [资料与证据](00-overview/sources-and-evidence.md)
- [生成状态](00-overview/generation-state.md)
- [入门级题库](01-question-bank/beginner.md)
- [中等级题库](01-question-bank/intermediate.md)
- [高级题库](01-question-bank/advanced.md)
- [专家级题库](01-question-bank/expert.md)
- [知识域配套笔记](02-knowledge-domains/)
- [实验与代码练习](03-practice/hands-on-labs.md)
- [模拟面试路线](03-practice/mock-interview-routes.md)
- [质量审计](04-review/quality-audit.md)
- [追问索引](04-review/follow-up-index.md)
- [专家级检查表](04-review/expert-checklist.md)
- [30 天路线](99-roadmap/30-day-plan.md)
- [90 天路线](99-roadmap/90-day-plan.md)

## 证据纪律

- `[P-已确认]`：用户明确提供的经历类别。
- `[S-已确认]`：工作区静态资料能支持的事实。
- `[P-待补充]`：项目模块、职责、版本、指标和真实结果。
- `[推断]`：根据通用机制进行的解释。
- `[待验证]`：需要目标版本源码、设备或实验确认的结论。
- `[建议]`：学习、验证、设计或求职建议。

默认不执行凭据、破坏性、高成本或专用硬件实验；资料中的实验是方案，不是已验证结果。
