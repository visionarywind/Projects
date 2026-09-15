# 设计原则与取舍

- 文档目的：从实现证据归纳架构原则，并区分事实和推断。
- 适用范围：Core 与参考训练路径。
- 对应源码版本：`3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`
- 证据状态：部分推断
- 最后更新：2026-09-10
- 前置阅读：[总体架构](architecture.md)
- 后续阅读：[模块注册表](../01-modules/module-registry.md)

## 结论摘要

代码体现出四个核心取舍：通过配置和 `ModuleSpec` 组合模型；通过 process groups 显式表达多维并行；通过 pipeline schedule 把 microbatch 调度从模型实现中分离；通过 sharded state dict 支持大规模 checkpoint。这样便于替换后端和并行策略，但增加了配置矩阵和动态路径的调试成本。

## 原则与证据

| 原则 | 实现证据 | 收益 | 代价 |
|---|---|---|---|
| 组件化模型装配 | `gpt_builder` 按 `spec`, TE, MoE, heterogeneous 分支 [gpt_builders.py:31-54] | 同一入口支持多模型变体 | 分支组合可能难以穷举 |
| 并行拓扑抽象 | `parallel_state` 保存 TP/PP/DP/CP/EP groups [parallel_state.py:28-165] | 统一 rank/group 查询 | 全局兼容状态可能隐式耦合 |
| 调度与模型解耦 | `get_forward_backward_func` 返回 schedule [schedules.py:53-168] | 可替换 pipeline/交错策略 | 输入 shape 和 stage 契约严格 |
| 大模型分片持久化 | Demo 从 `sharded_state_dict` 调 `dist_checkpointing.save/load` [examples/run_simple_mcore_train_loop.py:168-217] | 每 rank 管理局部 shard | checkpoint 布局和版本迁移复杂 |
| 后端可选 | optimizer import TE/Apex/Torch fallback [megatron/core/optimizer/__init__.py:13-35] | 环境适配和性能选择 | fallback 与主路径行为需分别测试 |

## 稳定契约与实现细节

较稳定的契约是 `forward_step_func` 的输入/输出、`GPTModel` 构造参数、`get_forward_backward_func` 的 schedule 选择和 sharded checkpoint API。具体 group 全局变量名称、可选 backend 分支和优化器内部 param-group 结构属于实现细节；修改前应查看测试和调用者。

## 推断与未知

“组件化是为了支持研究迭代”是合理推断，不是单一注释直接声明；README 将 Core 描述为 composable library，可作为补充证据。[README.md:17-19] 尚未通过性能实验量化每个取舍的收益。

## 相关文档

- [M01 设计](../01-modules/M01-core-models/design.md)
- [M02 设计](../01-modules/M02-parallelism/design.md)
- [M05 设计](../01-modules/M05-optimizer-checkpointing/README.md)

## 源码证据摘要

见上表。

## 未解决问题

需要在不同 GPU、PP/TP/CP 组合和 TE 安装状态下做实验，才能确认性能与兼容性边界。

## 下一步阅读建议

阅读 Demo 的修改练习并对照每条取舍。
