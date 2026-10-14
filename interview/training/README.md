# 训练方向面试题

本目录以 Megatron-LM 风格训练系统为证据基座，覆盖从入口、数据、Transformer 计算图到并行（DP/TP/PP/CP/EP）、MoE、混合精度、checkpoint、容错和训练—服务交付的完整链路。

## 结构

- `00-overview/`：知识地图、能力地图、覆盖矩阵、资料证据、生成状态。
- `01-question-bank/`：四级题库，每级 **15 题**，稳定 ID 为 `T-B###`、`T-I###`、`T-A###`、`T-E###`。
- `02-knowledge-domains/`：13 个知识域（K01–K13）机制详解与题目索引。

## 导航

- [知识地图](00-overview/knowledge-map.md) · [能力地图](00-overview/competency-map.md) · [覆盖矩阵](00-overview/coverage-matrix.md)
- [资料与证据](00-overview/sources-and-evidence.md) · [生成状态](00-overview/generation-state.md)
- 题库：[入门级 15 题](01-question-bank/beginner.md) · [中等级 15 题](01-question-bank/intermediate.md) · [高级 15 题](01-question-bank/advanced.md) · [专家级 15 题](01-question-bank/expert.md)
- 知识域：[总览](02-knowledge-domains/README.md)

## 知识域（K01–K13）

| 域 | 名称 |
|---|---|
| K01 | 训练入口与生命周期 |
| K02 | 分布式初始化与通信组 |
| K03 | 数据并行 |
| K04 | 张量并行 |
| K05 | 流水线并行与调度 |
| K06 | 序列与上下文并行 |
| K07 | MoE 与专家并行 |
| K08 | 优化器与梯度更新 |
| K09 | 混合精度与数值稳定 |
| K10 | 检查点、恢复与容错 |
| K11 | 显存、性能与吞吐 |
| K12 | 数据管线与交付 |
| K13 | 模型结构与计算图 |

## 单题标准

每题包含：元数据、背景知识、问题分析、具体答案、技术洞察、图文说明、拓展知识、常见追问及回答要点、易错点与反例、实践与验证、分层评分标准、证据与来源。题型混合概念、原理、流程、实现、取舍、测试、排障、性能、工程和开放设计。

## 证据规则

源码/文档明确结论标 `C`，跨文档推断标 `I`；GPU、NCCL、多机、多进程、模型下载和性能结论除非有运行记录，均标 `U / 待验证`。版本锚点：Megatron-LM `main` @ `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`。
