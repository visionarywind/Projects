# 训练方向资料与证据索引

## 主要证据入口

- `../../megatron-lm/README.md`：项目入口、训练主线和版本边界。
- `../../megatron-lm/00-overview/`：运行时、数据流、设计原则、错误模型。
- `../../megatron-lm/01-modules/M01-core-models/`：TransformerConfig、ModuleSpec、GPTModel。
- `../../megatron-lm/01-modules/M02-parallelism/`：TP、PP、DP、CP、SP、EP。
- `../../megatron-lm/01-modules/M03-training-runtime/`：forward/backward、schedule、optimizer。
- `../../megatron-lm/01-modules/M04-data-pipeline/`：tokenizer、dataset、packing、mask。
- `../../megatron-lm/01-modules/M05-optimizer-checkpointing/`：优化器、分布式 checkpoint、恢复。
- `../../megatron-lm/90-cross-module/`：资源、性能、接口、变更影响和训练—推理桥接。
- `../../megatron-lm/99-roadmap/`：测试与性能验证方案。

## 证据状态

本目录是文档型题库，不等同于上游源码 checkout。源码解释可标 C/I；GPU、NCCL、多机、多进程、模型下载和性能数字默认 U。任何命令在题目中只能作为 `[待验证]` 实验方案，除非同时记录日期、版本、环境、输入、结果和日志位置。

## 交叉来源

推理桥接题引用 `../../sglang/` 时，需要同时说明 checkpoint、tokenizer、shape/dtype、并行拓扑或指标的映射关系，不能把训练和服务概念直接等同。
