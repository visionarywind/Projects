# 项目定位与边界

- 文档目的：说明项目解决的问题、组成和边界。
- 适用范围：Megatron-LM `main` at `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`。
- 证据状态：已确认（能力描述）；设计动机部分为推断。
- 最后更新：2026-09-10
- 前置阅读：[仓库 README](../../README.md)
- 后续阅读：[总体架构](architecture.md)

## 结论摘要

项目将“可复用的大模型训练基础组件”和“可直接参考的训练程序”放在同一仓库。Core 的边界是模型、并行、数据、优化、检查点和推理 API；顶层脚本和 `megatron/training` 负责把这些组件编排成完整训练作业。[README.md:15-21] [README.md:65-87]

## 解决的问题

大规模 Transformer 训练同时需要 GPU kernel、模型结构、分布式通信、流水线调度、数据迭代、梯度/优化器和容错检查点。Core 把这些职责按可组合组件拆开，并支持 TP、PP、DP、EP、CP 及 FP16/BF16/FP8/FP4。[README.md:17-19]

## 系统边界

| 边界内 | 边界外/由调用方提供 |
|---|---|
| 模型模块、ModuleSpec、通信组和 pipeline schedule | CUDA、NCCL、PyTorch、Transformer Engine |
| tokenizer/dataset builder、optimizer、分布式 checkpoint | 训练数据准备、集群调度和 GPU 物理资源 |
| 参考入口、examples、tools、测试和文档 | Megatron Bridge 的 HF 转换工作流（外部仓库） |

“边界外”是依赖或外部系统，不代表项目不支持它们；它们通过 Python/CUDA/分布式 API 进入运行时。

## 能力与非目标

已确认能力包括：GPU 优化 Transformer building blocks、模型架构、MoE、并行策略、混合精度、推理和模型导出。[README.md:17-19] 该仓库不是一个独立的集群调度器，也不是数据标注系统；SLURM/容器由 CI 或部署环境提供。

## 版本和状态

源码版本通过 Git 确认：分支 `main`，HEAD `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`。README 显示发布版本 `0.19.0`，但当前浅克隆的 `git describe` 为 `3703d4e33`，两者是不同的发布/源码标识，不能互相替代。

## 相关文档

- [架构](architecture.md)
- [依赖地图](dependency-map.md)
- [模块注册表](../01-modules/module-registry.md)

## 源码证据摘要

`README.md:15-21,65-87`; `pyproject.toml:21-28,69-143`; `pretrain_gpt.py:33-80`。

## 未解决问题

本文未验证具体 GPU 型号、CUDA/NCCL 版本和生产部署拓扑；请以目标容器和集群配置为准。

## 下一步阅读建议

阅读 `architecture.md` 后进入 [D01](../80-demos/D01-simple-mcore-training/README.md)。
