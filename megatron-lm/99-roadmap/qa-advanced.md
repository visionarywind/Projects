# 高级面试题：Megatron-LM

- 文档目的：解释 99-roadmap/qa-advanced.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/qa-advanced.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

- 题数：100
- 适用对象：能够跨模块分析架构、性能和故障的读者
- 证据锚点：当前知识库记录的 checkout `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`。
- 作答要求：每题包含参考答案、小白解释、技术分析和拓展分析；涉及 GPU/多卡/性能的结论必须区分静态源码理解与实际运行验证。
- 项目一句话：Megatron-LM/Megatron Core 面向大规模 Transformer 训练，把模型装配、并行拓扑、pipeline 调度、数据、优化器和分布式 checkpoint 组合成训练作业。
- 主要证据：README、00-overview、01-modules/M01-core-models、module-registry、D01 最小训练 Demo 和构建/运行文档。

## A. 多维并行组合（1-10）

### 1. 从架构角度分析「TP×PP」的正确性与性能权衡。

**参考答案：** 「TP×PP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「TP×PP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「TP×PP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 2. 如何定位与「DP×TP」相关的跨模块故障？

**参考答案：** 「DP×TP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「DP×TP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「DP×TP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 3. 如果要优化「CP×TP」，怎样避免局部加速导致整体退化？

**参考答案：** 「CP×TP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「CP×TP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「CP×TP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 4. 设计一个验证「EP×DP」不变量的实验或测试矩阵。

**参考答案：** 「EP×DP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「EP×DP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「EP×DP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 5. 「VP 交错」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「VP 交错」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「VP 交错」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「VP 交错」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 6. 从架构角度分析「sequence parallel」的正确性与性能权衡。

**参考答案：** 「sequence parallel」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「sequence parallel」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「sequence parallel」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 7. 如何定位与「expert parallel groups」相关的跨模块故障？

**参考答案：** 「expert parallel groups」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「expert parallel groups」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「expert parallel groups」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 8. 如果要优化「rank order」，怎样避免局部加速导致整体退化？

**参考答案：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 9. 设计一个验证「placement」不变量的实验或测试矩阵。

**参考答案：** 「placement」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「placement」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「placement」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 10. 「group alias」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「group alias」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「group alias」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「group alias」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“多维并行组合”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## B. Pipeline 性能（11-20）

### 11. 从架构角度分析「bubble」的正确性与性能权衡。

**参考答案：** 「bubble」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「bubble」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「bubble」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 12. 如何定位与「1F1B」相关的跨模块故障？

**参考答案：** 1F1B 在 pipeline warmup 后交替推进 forward 和 backward；它减少空闲但需要保存恰当 activation 并正确进行 P2P。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。1F1B 在 pipeline warmup 后交替推进 forward 和 backward；它减少空闲但需要保存恰当 activation 并正确进行 P2P。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 1F1B 在 pipeline warmup 后交替推进 forward 和 backward；它减少空闲但需要保存恰当 activation 并正确进行 P2P。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 13. 如果要优化「interleaved 1F1B」，怎样避免局部加速导致整体退化？

**参考答案：** 1F1B 在 pipeline warmup 后交替推进 forward 和 backward；它减少空闲但需要保存恰当 activation 并正确进行 P2P。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。1F1B 在 pipeline warmup 后交替推进 forward 和 backward；它减少空闲但需要保存恰当 activation 并正确进行 P2P。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 1F1B 在 pipeline warmup 后交替推进 forward 和 backward；它减少空闲但需要保存恰当 activation 并正确进行 P2P。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 14. 设计一个验证「microbatch 数」不变量的实验或测试矩阵。

**参考答案：** microbatch 是把一个训练 batch 切成、交给 pipeline schedule 单独推进的小份；它让不同 pipeline stage 可以交错工作。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。microbatch 是把一个训练 batch 切成、交给 pipeline schedule 单独推进的小份；它让不同 pipeline stage 可以交错工作。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** microbatch 是把一个训练 batch 切成、交给 pipeline schedule 单独推进的小份；它让不同 pipeline stage 可以交错工作。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 15. 「activation memory」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「activation memory」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「activation memory」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「activation memory」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 16. 从架构角度分析「P2P latency」的正确性与性能权衡。

**参考答案：** 「P2P latency」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「P2P latency」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「P2P latency」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 17. 如何定位与「schedule overlap」相关的跨模块故障？

**参考答案：** 「schedule overlap」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「schedule overlap」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「schedule overlap」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 18. 如果要优化「stage imbalance」，怎样避免局部加速导致整体退化？

**参考答案：** 「stage imbalance」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「stage imbalance」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「stage imbalance」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 19. 设计一个验证「forward/backward dependency」不变量的实验或测试矩阵。

**参考答案：** backward 沿 autograd 图计算参数梯度；pipeline 中 activation 必须跨 forward/backward 的正确 microbatch 配对，否则梯度会错位。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。backward 沿 autograd 图计算参数梯度；pipeline 中 activation 必须跨 forward/backward 的正确 microbatch 配对，否则梯度会错位。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** backward 沿 autograd 图计算参数梯度；pipeline 中 activation 必须跨 forward/backward 的正确 microbatch 配对，否则梯度会错位。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 20. 「吞吐 vs 延迟」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「吞吐 vs 延迟」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「吞吐 vs 延迟」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「吞吐 vs 延迟」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline 性能”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## C. 通信优化（21-30）

### 21. 从架构角度分析「all-reduce」的正确性与性能权衡。

**参考答案：** 「all-reduce」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「all-reduce」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「all-reduce」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 22. 如何定位与「reduce-scatter」相关的跨模块故障？

**参考答案：** 「reduce-scatter」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「reduce-scatter」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「reduce-scatter」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 23. 如果要优化「all-gather」，怎样避免局部加速导致整体退化？

**参考答案：** 「all-gather」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「all-gather」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「all-gather」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 24. 设计一个验证「P2P」不变量的实验或测试矩阵。

**参考答案：** 「P2P」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「P2P」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「P2P」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 25. 「overlap_comm」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「overlap_comm」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「overlap_comm」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「overlap_comm」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 26. 从架构角度分析「bucket」的正确性与性能权衡。

**参考答案：** 「bucket」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「bucket」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「bucket」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 27. 如何定位与「gradient fusion」相关的跨模块故障？

**参考答案：** gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 28. 如果要优化「NCCL stream」，怎样避免局部加速导致整体退化？

**参考答案：** NCCL 为 GPU rank 提供 collective 和点对点通信；所有参与 rank 必须以一致顺序调用兼容 shape 的通信操作。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。NCCL 为 GPU rank 提供 collective 和点对点通信；所有参与 rank 必须以一致顺序调用兼容 shape 的通信操作。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** NCCL 为 GPU rank 提供 collective 和点对点通信；所有参与 rank 必须以一致顺序调用兼容 shape 的通信操作。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 29. 设计一个验证「barrier」不变量的实验或测试矩阵。

**参考答案：** 「barrier」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「barrier」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「barrier」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 30. 「通信 profiling」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「通信 profiling」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「通信 profiling」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「通信 profiling」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“通信优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## D. 内存优化（31-40）

### 31. 从架构角度分析「activation checkpointing」的正确性与性能权衡。

**参考答案：** 「activation checkpointing」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「activation checkpointing」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「activation checkpointing」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 32. 如何定位与「recompute」相关的跨模块故障？

**参考答案：** 「recompute」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「recompute」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「recompute」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 33. 如果要优化「sequence parallel memory」，怎样避免局部加速导致整体退化？

**参考答案：** 「sequence parallel memory」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「sequence parallel memory」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「sequence parallel memory」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 34. 设计一个验证「optimizer state sharding」不变量的实验或测试矩阵。

**参考答案：** optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 35. 「gradient buffer」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 36. 从架构角度分析「parameter sharding」的正确性与性能权衡。

**参考答案：** 「parameter sharding」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「parameter sharding」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「parameter sharding」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 37. 如何定位与「CPU offload」相关的跨模块故障？

**参考答案：** 「CPU offload」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「CPU offload」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「CPU offload」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 38. 如果要优化「fragmentation」，怎样避免局部加速导致整体退化？

**参考答案：** 「fragmentation」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「fragmentation」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「fragmentation」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 39. 设计一个验证「peak memory」不变量的实验或测试矩阵。

**参考答案：** 「peak memory」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「peak memory」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「peak memory」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 40. 「OOM 诊断」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「OOM 诊断」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「OOM 诊断」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「OOM 诊断」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“内存优化”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## E. 低精度数值（41-50）

### 41. 从架构角度分析「FP16 overflow」的正确性与性能权衡。

**参考答案：** FP16 节省存储并加速 tensor core，但动态范围较小，梯度 overflow/underflow 需要 loss scaling 或其他保护。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。FP16 节省存储并加速 tensor core，但动态范围较小，梯度 overflow/underflow 需要 loss scaling 或其他保护。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** FP16 节省存储并加速 tensor core，但动态范围较小，梯度 overflow/underflow 需要 loss scaling 或其他保护。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 42. 如何定位与「BF16 范围」相关的跨模块故障？

**参考答案：** BF16 保留较大指数范围、尾数较短；通常比 FP16 更能容忍大范围值，但精度误差仍需验证。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。BF16 保留较大指数范围、尾数较短；通常比 FP16 更能容忍大范围值，但精度误差仍需验证。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** BF16 保留较大指数范围、尾数较短；通常比 FP16 更能容忍大范围值，但精度误差仍需验证。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 43. 如果要优化「FP8 scaling」，怎样避免局部加速导致整体退化？

**参考答案：** FP8 用更低位宽换吞吐和显存，依赖 scale/amax recipe；scale 统计、通信和算子支持共同决定数值结果。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。FP8 用更低位宽换吞吐和显存，依赖 scale/amax recipe；scale 统计、通信和算子支持共同决定数值结果。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** FP8 用更低位宽换吞吐和显存，依赖 scale/amax recipe；scale 统计、通信和算子支持共同决定数值结果。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 44. 设计一个验证「FP4 量化」不变量的实验或测试矩阵。

**参考答案：** FP4 进一步压缩权重或激活，误差和 kernel/layout 约束更强；不能只看 dtype 名称判断可用性。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。FP4 进一步压缩权重或激活，误差和 kernel/layout 约束更强；不能只看 dtype 名称判断可用性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** FP4 进一步压缩权重或激活，误差和 kernel/layout 约束更强；不能只看 dtype 名称判断可用性。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 45. 「amax history」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「amax history」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「amax history」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「amax history」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 46. 从架构角度分析「loss scaling」的正确性与性能权衡。

**参考答案：** loss scaling 放大 loss 以避免低精度梯度下溢，再在更新前还原；overflow 时需跳过或处理该 step。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。loss scaling 放大 loss 以避免低精度梯度下溢，再在更新前还原；overflow 时需跳过或处理该 step。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** loss scaling 放大 loss 以避免低精度梯度下溢，再在更新前还原；overflow 时需跳过或处理该 step。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 47. 如何定位与「TE recipe」相关的跨模块故障？

**参考答案：** 「TE recipe」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「TE recipe」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「TE recipe」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 48. 如果要优化「数值漂移」，怎样避免局部加速导致整体退化？

**参考答案：** 「数值漂移」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「数值漂移」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「数值漂移」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 49. 设计一个验证「NaN 定位」不变量的实验或测试矩阵。

**参考答案：** 「NaN 定位」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「NaN 定位」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「NaN 定位」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 50. 「golden tolerance」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「golden tolerance」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「golden tolerance」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「golden tolerance」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“低精度数值”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## F. MoE 与专家并行（51-60）

### 51. 从架构角度分析「router」的正确性与性能权衡。

**参考答案：** 「router」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「router」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「router」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 52. 如何定位与「top-k expert」相关的跨模块故障？

**参考答案：** 「top-k expert」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「top-k expert」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「top-k expert」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 53. 如果要优化「capacity factor」，怎样避免局部加速导致整体退化？

**参考答案：** 「capacity factor」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「capacity factor」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「capacity factor」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 54. 设计一个验证「token dispatch」不变量的实验或测试矩阵。

**参考答案：** 文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 55. 「expert load balance」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「expert load balance」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「expert load balance」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「expert load balance」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 56. 从架构角度分析「dropless MoE」的正确性与性能权衡。

**参考答案：** 「dropless MoE」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「dropless MoE」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「dropless MoE」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 57. 如何定位与「EP communication」相关的跨模块故障？

**参考答案：** 「EP communication」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「EP communication」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「EP communication」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 58. 如果要优化「aux loss」，怎样避免局部加速导致整体退化？

**参考答案：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 59. 设计一个验证「expert checkpoint」不变量的实验或测试矩阵。

**参考答案：** 「expert checkpoint」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「expert checkpoint」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「expert checkpoint」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 60. 「MoE 性能」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「MoE 性能」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「MoE 性能」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「MoE 性能」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 与专家并行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## G. 分布式 checkpoint（61-70）

### 61. 从架构角度分析「resharding」的正确性与性能权衡。

**参考答案：** 「resharding」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「resharding」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「resharding」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 62. 如何定位与「cross-rank metadata」相关的跨模块故障？

**参考答案：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 63. 如果要优化「async checkpoint worker」，怎样避免局部加速导致整体退化？

**参考答案：** 「async checkpoint worker」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「async checkpoint worker」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「async checkpoint worker」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 64. 设计一个验证「atomic save」不变量的实验或测试矩阵。

**参考答案：** 「atomic save」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「atomic save」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「atomic save」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 65. 「partial failure」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「partial failure」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「partial failure」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「partial failure」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 66. 从架构角度分析「load strictness」的正确性与性能权衡。

**参考答案：** 「load strictness」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「load strictness」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「load strictness」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 67. 如何定位与「version migration」相关的跨模块故障？

**参考答案：** 「version migration」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「version migration」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「version migration」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 68. 如果要优化「storage bandwidth」，怎样避免局部加速导致整体退化？

**参考答案：** 「storage bandwidth」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「storage bandwidth」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「storage bandwidth」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 69. 设计一个验证「checkpoint validation」不变量的实验或测试矩阵。

**参考答案：** 「checkpoint validation」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「checkpoint validation」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「checkpoint validation」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 70. 「state ownership」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「state ownership」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「state ownership」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「state ownership」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“分布式 checkpoint”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## H. 训练稳定性（71-80）

### 71. 从架构角度分析「learning rate schedule」的正确性与性能权衡。

**参考答案：** 「learning rate schedule」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「learning rate schedule」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「learning rate schedule」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 72. 如何定位与「gradient clipping」相关的跨模块故障？

**参考答案：** gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 73. 如果要优化「loss spike」，怎样避免局部加速导致整体退化？

**参考答案：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 74. 设计一个验证「data order」不变量的实验或测试矩阵。

**参考答案：** 「data order」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「data order」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「data order」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 75. 「seed」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「seed」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「seed」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「seed」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 76. 从架构角度分析「determinism」的正确性与性能权衡。

**参考答案：** 「determinism」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「determinism」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「determinism」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 77. 如何定位与「resume equivalence」相关的跨模块故障？

**参考答案：** resume 的目标是继续同一训练轨迹，因此数据位置、随机数、optimizer state、参数和 iteration 都要一致或有明确差异。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。resume 的目标是继续同一训练轨迹，因此数据位置、随机数、optimizer state、参数和 iteration 都要一致或有明确差异。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** resume 的目标是继续同一训练轨迹，因此数据位置、随机数、optimizer state、参数和 iteration 都要一致或有明确差异。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 78. 如果要优化「optimizer state」，怎样避免局部加速导致整体退化？

**参考答案：** optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 79. 设计一个验证「mixed precision stability」不变量的实验或测试矩阵。

**参考答案：** mixed precision 不是把所有 tensor 随意改 dtype，而是为不同算子选择精度并维护累积/缩放规则。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。mixed precision 不是把所有 tensor 随意改 dtype，而是为不同算子选择精度并维护累积/缩放规则。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** mixed precision 不是把所有 tensor 随意改 dtype，而是为不同算子选择精度并维护累积/缩放规则。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 80. 「long-run drift」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「long-run drift」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「long-run drift」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「long-run drift」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“训练稳定性”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## I. 性能实验（81-90）

### 81. 从架构角度分析「weak scaling」的正确性与性能权衡。

**参考答案：** 「weak scaling」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「weak scaling」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「weak scaling」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 82. 如何定位与「strong scaling」相关的跨模块故障？

**参考答案：** 「strong scaling」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「strong scaling」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「strong scaling」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 83. 如果要优化「MFU」，怎样避免局部加速导致整体退化？

**参考答案：** 「MFU」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「MFU」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「MFU」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 84. 设计一个验证「tokens/sec」不变量的实验或测试矩阵。

**参考答案：** 文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 85. 「step time breakdown」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「step time breakdown」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「step time breakdown」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「step time breakdown」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 86. 从架构角度分析「GPU utilization」的正确性与性能权衡。

**参考答案：** 「GPU utilization」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「GPU utilization」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「GPU utilization」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 87. 如何定位与「communication ratio」相关的跨模块故障？

**参考答案：** 「communication ratio」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「communication ratio」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「communication ratio」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 88. 如果要优化「pipeline bubble ratio」，怎样避免局部加速导致整体退化？

**参考答案：** 「pipeline bubble ratio」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「pipeline bubble ratio」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「pipeline bubble ratio」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 89. 设计一个验证「profiling trace」不变量的实验或测试矩阵。

**参考答案：** 「profiling trace」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「profiling trace」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「profiling trace」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 90. 「实验可复现」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「实验可复现」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「实验可复现」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「实验可复现」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“性能实验”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## J. 高级改造审计（91-100）

### 91. 从架构角度分析「新增模型」的正确性与性能权衡。

**参考答案：** 「新增模型」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「新增模型」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「新增模型」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 92. 如何定位与「新增并行维度」相关的跨模块故障？

**参考答案：** 「新增并行维度」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「新增并行维度」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「新增并行维度」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 93. 如果要优化「修改 schedule」，怎样避免局部加速导致整体退化？

**参考答案：** 「修改 schedule」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「修改 schedule」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「修改 schedule」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 94. 设计一个验证「替换 attention」不变量的实验或测试矩阵。

**参考答案：** attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 95. 「修改 checkpoint」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「修改 checkpoint」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「修改 checkpoint」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「修改 checkpoint」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 96. 从架构角度分析「新增 optimizer」的正确性与性能权衡。

**参考答案：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 97. 如何定位与「修改数据格式」相关的跨模块故障？

**参考答案：** 「修改数据格式」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「修改数据格式」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「修改数据格式」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 98. 如果要优化「修改 process group」，怎样避免局部加速导致整体退化？

**参考答案：** ProcessGroup 定义一次通信的参与者集合；同一个 global rank 可属于多个语义不同的 group，调用点必须选对 group。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。ProcessGroup 定义一次通信的参与者集合；同一个 global rank 可属于多个语义不同的 group，调用点必须选对 group。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** ProcessGroup 定义一次通信的参与者集合；同一个 global rank 可属于多个语义不同的 group，调用点必须选对 group。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 99. 设计一个验证「修改精度策略」不变量的实验或测试矩阵。

**参考答案：** 「修改精度策略」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「修改精度策略」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「修改精度策略」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 100. 「跨模块回归」在多 GPU/异步场景下有什么隐藏风险？

**参考答案：** 「跨模块回归」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「跨模块回归」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「跨模块回归」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“高级改造审计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须同时讨论正确性、资源、并发/分布式行为、性能指标和回归验证。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。


## 使用建议

先遮住答案自测，再对照四段内容复盘。回答技术题时，明确对象、输入输出、状态变化、资源所有权、失败语义和验证边界；不要把未执行的 GPU、NPU、模型、多卡或性能命令写成运行事实。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
