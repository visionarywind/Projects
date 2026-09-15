# 推理方向面试题

本目录以 SGLang 风格在线推理系统为证据基座，覆盖协议、请求状态、调度、Transformer 执行、KV cache、采样、分布式推理、量化、speculative decoding、分离式部署、适配器、多模态、长上下文、生产运维和安全。每个难度 **100 题**，稳定 ID 为 `I-B###`、`I-I###`、`I-A###`、`I-E###`。

## 导航

- [知识地图](00-overview/knowledge-map.md) · [能力地图](00-overview/competency-map.md) · [覆盖矩阵](00-overview/coverage-matrix.md)
- [资料与证据](00-overview/sources-and-evidence.md) · [生成状态](00-overview/generation-state.md)
- [KV Cache 池化技术专题](../../insights/README.md)：静态/动态/混合显存池、分页、虚拟映射、异构 offload、生命周期和验证路线。
- [入门级 100 题](beginner.md)
- [中等级 100 题](intermediate.md)
- [高级 100 题](advanced.md)
- [专家级 100 题](expert.md)

## 单题标准

每题都包含：元数据、背景知识、问题分析、具体答案、技术洞察、图文说明、拓展知识、常见追问及回答要点、易错点与反例、实践与验证、分层评分标准、证据与来源。题型混合概念、原理、流程、实现、取舍、测试、排障、性能、工程和开放设计。

GPU、模型、网络、RDMA、多进程、压测和硬件兼容性结论除非有运行记录，均标为 `U / 待验证`。
