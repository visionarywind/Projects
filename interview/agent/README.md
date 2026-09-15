# AI Agent 开发领域专家级面试题库

本题库面向希望系统掌握 AI Agent 开发的后端、算法、平台工程师和技术负责人，覆盖从模型交互到生产治理的完整链路。题目服务于技术面试、系统设计、源码/机制理解和项目复盘，不以背诵某个框架 API 为目标。

## 当前配置与状态

- 语言：中文
- 目标规模：入门、中等级、高级、专家级各 100 题，共 400 题
- 稳定编号：`AG-B001`–`AG-B100`、`AG-I001`–`AG-I100`、`AG-A001`–`AG-A100`、`AG-E001`–`AG-E100`
- 当前交付：四级完整 400 题（每级 100 题）；后续进入质量审计、证据补强和授权实验阶段
- 证据原则：事实、推断、建议和待验证实验明确区分；未运行的模型/API/压测/安全实验不写成已验证结果

## 导航

- [领域总览](00-overview/domain-overview.md) · [知识地图](00-overview/knowledge-map.md) · [能力地图](00-overview/competency-map.md)
- [覆盖矩阵](00-overview/coverage-matrix.md) · [术语表](00-overview/terminology.md) · [资料与证据](00-overview/sources-and-evidence.md)
- [生成状态](00-overview/generation-state.md)
- [入门级题目](01-question-bank/beginner.md) · [中等级题目](01-question-bank/intermediate.md) · [高级题目](01-question-bank/advanced.md) · [专家级题目](01-question-bank/expert.md)
- [知识域索引](02-knowledge-domains/README.md)

> 当前已交付四级完整 400 题；实践、排障、系统设计、模拟面试、质量审计和路线页尚未在当前批次建立。请以 `00-overview/generation-state.md` 和 `04-review/quality-audit.md` 为准。

## 范围边界

纳入 Agent 与 workflow 的边界、运行时状态机、模型和上下文管理、结构化输出、工具调用与协议、规划编排、记忆、RAG/grounding、多 Agent、人机审批、评估和可观测性、安全治理、可靠性、部署扩缩、成本和产品演进。Transformer 训练、GPU kernel 和纯推理服务只在影响 Agent 行为时交叉引用现有题库；框架 API 作为版本化案例而不是普遍事实。

## 学习顺序

1. 阅读总览、术语和知识地图，建立 Agent、tool、memory、RAG、workflow 的边界。
2. 完成入门级：能画最小执行循环、写工具 schema、定义 oracle。
3. 完成中等级：追踪 run、设计记忆和规划、排查超时并建立分层评估。
4. 完成高级：在长任务、预算、并发、权限和恢复约束下做生产设计。
5. 完成专家级：评审不变量、评估器、威胁模型和平台演进路线。
6. 用实践、回放、故障注入和模拟面试检验迁移能力。

## 质量和证据

每题包含统一十段模板、元数据、图示或结构化示例、追问、反例、验证 oracle 和四级评分。`[已确认]` 表示资料直接支持，`[推断]` 表示由证据推导，`[待验证]` 表示需运行确认，`[建议]` 表示推荐方案，`[存在争议]` 表示版本或实现有分歧。题库不执行凭据、高成本或破坏性实验。
