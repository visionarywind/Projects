# 资料与证据

## 证据标签

- `[已确认]`：公开标准、论文或目标版本资料直接支持。
- `[推断]`：由多个来源或不变量推导，需明确推理链。
- `[待验证]`：需要运行、压测、故障注入、权限测试或目标环境核对。
- `[建议]`：推荐的设计，不代表现有系统已有该能力。
- `[存在争议]`：不同版本、框架或研究结论有分歧。

## 主要资料入口（访问基线：2026-09-15）

| 类别 | 资料 | 用途 |
|---|---|---|
| Agent 设计 | [OpenAI Practical Guide to Building Agents](https://openai.com/business/guides-and-resources/a-practical-guide-to-building-ai-agents/) | Agent/workflow 边界、编排和生产考虑；厂商资料不泛化 |
| Agent 评估 | [Anthropic: Demystifying evals for AI agents](https://www.anthropic.com/engineering/demystifying-evals-for-ai-agents) | 多轮轨迹、环境和部署评估 |
| 安全治理 | [Anthropic: Safe and trustworthy agents](https://www.anthropic.com/news/our-framework-for-developing-safe-and-trustworthy-agents) | 工具、记忆、监督和风险边界 |
| 工具协议 | [Model Context Protocol](https://modelcontextprotocol.io/) | 协议能力发现、资源和工具边界；版本需核对 |
| 结构化契约 | [JSON Schema](https://json-schema.org/specification) | 参数校验和 schema 设计 |
| 可观测性 | [OpenTelemetry](https://opentelemetry.io/docs/) | trace/span、指标和日志语义 |
| 安全 | [OWASP LLM Top 10](https://owasp.org/www-project-top-10-for-large-language-model-applications/) | 注入、泄露和供应链风险；Agent 映射需自行验证 |
| 风险管理 | [NIST AI RMF](https://www.nist.gov/itl/ai-risk-management-framework) | 治理、测量、管理风险 |
| 原理研究 | [ReAct](https://arxiv.org/abs/2210.03629)、[Toolformer](https://arxiv.org/abs/2302.04761)、[Reflexion](https://arxiv.org/abs/2303.11366) | 规划、工具和反思机制的原始证据 |
| 评测研究 | [AgentBench](https://arxiv.org/abs/2308.03688)、[SWE-bench](https://arxiv.org/abs/2310.06770) | 任务环境和结果型评估案例 |

## 证据约束

没有锁定框架和模型版本时，只陈述机制层事实；框架 API、默认值、成功率、延迟、成本和安全效果均标 `[待验证]`。本仓库已有 [训练题库](../../training/README.md) 和 [推理题库](../../inference/README.md) 只作为交叉阅读，不替代 Agent 领域证据。
