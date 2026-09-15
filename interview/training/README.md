# 训练方向面试题

本目录以 Megatron-LM 风格训练系统为证据基座，覆盖从入口、数据、Transformer 计算图到并行、MoE、混合精度、checkpoint、容错和训练—服务交付的完整链路。每个难度 **100 题**，稳定 ID 为 `T-B###`、`T-I###`、`T-A###`、`T-E###`。

## 导航

- [知识地图](00-overview/knowledge-map.md) · [能力地图](00-overview/competency-map.md) · [覆盖矩阵](00-overview/coverage-matrix.md)
- [资料与证据](00-overview/sources-and-evidence.md) · [生成状态](00-overview/generation-state.md)
- [入门级 100 题](beginner.md)
- [中等级 100 题](intermediate.md)
- [高级 100 题](advanced.md)
- [专家级 100 题](expert.md)

## 单题标准

每题都包含：元数据、背景知识、问题分析、具体答案、技术洞察、图文说明、拓展知识、常见追问及回答要点、易错点与反例、实践与验证、分层评分标准、证据与来源。题型混合概念、原理、流程、实现、取舍、测试、排障、性能、工程和开放设计；不会把同一知识点简单改写成“是什么/配置错误”。

GPU、NCCL、多机、多进程、模型下载和性能结论除非有运行记录，均标为 `U / 待验证`。
