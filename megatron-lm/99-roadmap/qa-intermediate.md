# 中级面试题：Megatron-LM
- 题数：100
- 适用对象：已经理解基本概念，能够阅读模块文档和调用链的读者
- 证据锚点：当前知识库记录的 checkout `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`。
- 作答要求：每题包含参考答案、小白解释、技术分析和拓展分析；涉及 GPU/多卡/性能的结论必须区分静态源码理解与实际运行验证。
- 项目一句话：Megatron-LM/Megatron Core 面向大规模 Transformer 训练，把模型装配、并行拓扑、pipeline 调度、数据、优化器和分布式 checkpoint 组合成训练作业。
- 主要证据：README、00-overview、01-modules/M01-core-models、module-registry、D01 最小训练 Demo 和构建/运行文档。

## A. 入口与配置（1-10）

### 1. Megatron-LM 中「pretrain_gpt.py」的输入、输出和状态变化是什么？

**参考答案：** pretrain 将 provider、dataset、optimizer 和 training loop 组合成正式作业；它还要处理日志、保存、恢复和终止。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。pretrain 将 provider、dataset、optimizer 和 training loop 组合成正式作业；它还要处理日志、保存、恢复和终止。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** pretrain 将 provider、dataset、optimizer 和 training loop 组合成正式作业；它还要处理日志、保存、恢复和终止。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 2. 修改或排查「pretrain_hybrid.py」时应该先看哪些边界？

**参考答案：** pretrain 将 provider、dataset、optimizer 和 training loop 组合成正式作业；它还要处理日志、保存、恢复和终止。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。pretrain 将 provider、dataset、optimizer 和 training loop 组合成正式作业；它还要处理日志、保存、恢复和终止。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** pretrain 将 provider、dataset、optimizer 和 training loop 组合成正式作业；它还要处理日志、保存、恢复和终止。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 3. 为什么「examples/run_simple_mcore_train_loop.py」不能只按表面函数名理解？

**参考答案：** examples 通常是最小可读的真实 API 用法，适合先验证初始化、模型、数据、schedule、optimizer 和 checkpoint 的连接。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。examples 通常是最小可读的真实 API 用法，适合先验证初始化、模型、数据、schedule、optimizer 和 checkpoint 的连接。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** examples 通常是最小可读的真实 API 用法，适合先验证初始化、模型、数据、schedule、optimizer 和 checkpoint 的连接。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 4. 围绕「args 解析」设计一个 focused test 应该覆盖什么？

**参考答案：** 「args 解析」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「args 解析」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「args 解析」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 5. 「YAML/CLI 配置」常见的中级误区是什么？

**参考答案：** 「YAML/CLI 配置」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「YAML/CLI 配置」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「YAML/CLI 配置」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 6. Megatron-LM 中「TransformerConfig 映射」的输入、输出和状态变化是什么？

**参考答案：** TransformerConfig 是模型超参数和执行选项的集中表达；它把层数、隐藏维度、并行相关配置和精度策略传给模型构造。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。TransformerConfig 是模型超参数和执行选项的集中表达；它把层数、隐藏维度、并行相关配置和精度策略传给模型构造。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** TransformerConfig 是模型超参数和执行选项的集中表达；它把层数、隐藏维度、并行相关配置和精度策略传给模型构造。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 7. 修改或排查「global args」时应该先看哪些边界？

**参考答案：** 「global args」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「global args」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「global args」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 8. 为什么「模型大小参数」不能只按表面函数名理解？

**参考答案：** 「模型大小参数」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「模型大小参数」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「模型大小参数」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 9. 围绕「训练超参」设计一个 focused test 应该覆盖什么？

**参考答案：** 「训练超参」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「训练超参」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「训练超参」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 10. 「初始化顺序」常见的中级误区是什么？

**参考答案：** 「初始化顺序」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「初始化顺序」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「初始化顺序」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“入口与配置”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## B. 初始化与进程组（11-20）

### 11. Megatron-LM 中「torch.cuda.set_device」的输入、输出和状态变化是什么？

**参考答案：** CUDA 是 GPU 执行和内存的运行时基础；Python 层调用成功不代表 kernel 已按预期执行，异步错误常需同步后才暴露。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。CUDA 是 GPU 执行和内存的运行时基础；Python 层调用成功不代表 kernel 已按预期执行，异步错误常需同步后才暴露。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** CUDA 是 GPU 执行和内存的运行时基础；Python 层调用成功不代表 kernel 已按预期执行，异步错误常需同步后才暴露。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 12. 修改或排查「init_process_group」时应该先看哪些边界？

**参考答案：** 「init_process_group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「init_process_group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「init_process_group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 13. 为什么「initialize_model_parallel」不能只按表面函数名理解？

**参考答案：** 「initialize_model_parallel」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「initialize_model_parallel」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「initialize_model_parallel」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 14. 围绕「RankGenerator」设计一个 focused test 应该覆盖什么？

**参考答案：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 15. 「TP group」常见的中级误区是什么？

**参考答案：** 「TP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「TP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「TP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 16. Megatron-LM 中「PP group」的输入、输出和状态变化是什么？

**参考答案：** 「PP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「PP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「PP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 17. 修改或排查「DP group」时应该先看哪些边界？

**参考答案：** 「DP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「DP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「DP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 18. 为什么「CP group」不能只按表面函数名理解？

**参考答案：** 「CP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「CP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「CP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 19. 围绕「EP group」设计一个 focused test 应该覆盖什么？

**参考答案：** 「EP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「EP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「EP group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 20. 「destroy_model_parallel」常见的中级误区是什么？

**参考答案：** 「destroy_model_parallel」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「destroy_model_parallel」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「destroy_model_parallel」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“初始化与进程组”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## C. 模型实现（21-30）

### 21. Megatron-LM 中「GPTModel.__init__」的输入、输出和状态变化是什么？

**参考答案：** GPTModel 负责 embedding、decoder、输出头以及 forward 契约，但不负责整个训练循环或 optimizer.step。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。GPTModel 负责 embedding、decoder、输出头以及 forward 契约，但不负责整个训练循环或 optimizer.step。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** GPTModel 负责 embedding、decoder、输出头以及 forward 契约，但不负责整个训练循环或 optimizer.step。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 22. 修改或排查「pre_process」时应该先看哪些边界？

**参考答案：** 「pre_process」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「pre_process」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「pre_process」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 23. 为什么「post_process」不能只按表面函数名理解？

**参考答案：** 「post_process」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「post_process」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「post_process」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 24. 围绕「embedding sharing」设计一个 focused test 应该覆盖什么？

**参考答案：** embedding 把 token ID 映射为 hidden 向量；共享输入 embedding 与输出 head 时，参数 storage、分片和 checkpoint 语义要保持一致。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。embedding 把 token ID 映射为 hidden 向量；共享输入 embedding 与输出 head 时，参数 storage、分片和 checkpoint 语义要保持一致。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** embedding 把 token ID 映射为 hidden 向量；共享输入 embedding 与输出 head 时，参数 storage、分片和 checkpoint 语义要保持一致。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 25. 「TransformerBlock.forward」常见的中级误区是什么？

**参考答案：** TransformerBlock 通常串联归一化、self-attention 和 MLP；残差连接和 tensor shape 是上下层之间的核心契约。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。TransformerBlock 通常串联归一化、self-attention 和 MLP；残差连接和 tensor shape 是上下层之间的核心契约。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** TransformerBlock 通常串联归一化、self-attention 和 MLP；残差连接和 tensor shape 是上下层之间的核心契约。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 26. Megatron-LM 中「LayerNorm」的输入、输出和状态变化是什么？

**参考答案：** 「LayerNorm」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「LayerNorm」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「LayerNorm」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 27. 修改或排查「SelfAttention」时应该先看哪些边界？

**参考答案：** attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 28. 为什么「MLP」不能只按表面函数名理解？

**参考答案：** MLP/FFN 通常先扩展 hidden 维度再投影回来；它常是计算量大、适合张量并行或融合 kernel 的模块。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。MLP/FFN 通常先扩展 hidden 维度再投影回来；它常是计算量大、适合张量并行或融合 kernel 的模块。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** MLP/FFN 通常先扩展 hidden 维度再投影回来；它常是计算量大、适合张量并行或融合 kernel 的模块。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 29. 围绕「MoE layer」设计一个 focused test 应该覆盖什么？

**参考答案：** 「MoE layer」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「MoE layer」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「MoE layer」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 30. 「sharded_state_dict」常见的中级误区是什么？

**参考答案：** state_dict 是模块参数/缓冲区的键值状态表示；分布式模型中它可能只是逻辑视图，不等于单个文件。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。state_dict 是模块参数/缓冲区的键值状态表示；分布式模型中它可能只是逻辑视图，不等于单个文件。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** state_dict 是模块参数/缓冲区的键值状态表示；分布式模型中它可能只是逻辑视图，不等于单个文件。 本题属于“模型实现”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## D. ModuleSpec 与 builder（31-40）

### 31. Megatron-LM 中「显式 spec」的输入、输出和状态变化是什么？

**参考答案：** 「显式 spec」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「显式 spec」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「显式 spec」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 32. 修改或排查「TE spec」时应该先看哪些边界？

**参考答案：** 「TE spec」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「TE spec」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「TE spec」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 33. 为什么「local spec」不能只按表面函数名理解？

**参考答案：** 「local spec」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「local spec」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「local spec」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 34. 围绕「MoE spec」设计一个 focused test 应该覆盖什么？

**参考答案：** 「MoE spec」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「MoE spec」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「MoE spec」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 35. 「heterogeneous layers」常见的中级误区是什么？

**参考答案：** 「heterogeneous layers」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「heterogeneous layers」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「heterogeneous layers」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 36. Megatron-LM 中「layer spec dispatch」的输入、输出和状态变化是什么？

**参考答案：** 「layer spec dispatch」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「layer spec dispatch」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「layer spec dispatch」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 37. 修改或排查「submodule factory」时应该先看哪些边界？

**参考答案：** 「submodule factory」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「submodule factory」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「submodule factory」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 38. 为什么「配置分支」不能只按表面函数名理解？

**参考答案：** 「配置分支」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「配置分支」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「配置分支」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 39. 围绕「默认值」设计一个 focused test 应该覆盖什么？

**参考答案：** 「默认值」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「默认值」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「默认值」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 40. 「扩展新层」常见的中级误区是什么？

**参考答案：** 「扩展新层」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「扩展新层」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「扩展新层」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“ModuleSpec 与 builder”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## E. 数据管线（41-50）

### 41. Megatron-LM 中「BlendedMegatronDatasetBuilder」的输入、输出和状态变化是什么？

**参考答案：** dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 42. 修改或排查「MockGPTDataset」时应该先看哪些边界？

**参考答案：** dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 43. 为什么「IndexedDataset」不能只按表面函数名理解？

**参考答案：** dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 44. 围绕「tokenizer builder」设计一个 focused test 应该覆盖什么？

**参考答案：** tokenizer 决定文本到 token ID 的映射；更换 tokenizer 会改变 vocab、序列长度、special token 和 checkpoint/数据兼容性。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。tokenizer 决定文本到 token ID 的映射；更换 tokenizer 会改变 vocab、序列长度、special token 和 checkpoint/数据兼容性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** tokenizer 决定文本到 token ID 的映射；更换 tokenizer 会改变 vocab、序列长度、special token 和 checkpoint/数据兼容性。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 45. 「sample index」常见的中级误区是什么？

**参考答案：** 「sample index」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「sample index」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「sample index」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 46. Megatron-LM 中「sequence length」的输入、输出和状态变化是什么？

**参考答案：** 「sequence length」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「sequence length」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「sequence length」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 47. 修改或排查「batch dict」时应该先看哪些边界？

**参考答案：** batch 是一次更新或一次 forward 处理的样本集合；增大 batch 通常提高设备利用率，但也增加显存、通信和单步等待。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。batch 是一次更新或一次 forward 处理的样本集合；增大 batch 通常提高设备利用率，但也增加显存、通信和单步等待。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** batch 是一次更新或一次 forward 处理的样本集合；增大 batch 通常提高设备利用率，但也增加显存、通信和单步等待。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 48. 为什么「collate」不能只按表面函数名理解？

**参考答案：** 「collate」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「collate」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「collate」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 49. 围绕「data iterator」设计一个 focused test 应该覆盖什么？

**参考答案：** 「data iterator」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「data iterator」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「data iterator」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 50. 「数据并行切分」常见的中级误区是什么？

**参考答案：** 「数据并行切分」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「数据并行切分」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「数据并行切分」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数据管线”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## F. Pipeline schedule（51-60）

### 51. Megatron-LM 中「get_forward_backward_func」的输入、输出和状态变化是什么？

**参考答案：** 它根据 pipeline、virtual pipeline 和配置选择合适的 forward-backward schedule；返回的函数决定 microbatch 控制流。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。它根据 pipeline、virtual pipeline 和配置选择合适的 forward-backward schedule；返回的函数决定 microbatch 控制流。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 它根据 pipeline、virtual pipeline 和配置选择合适的 forward-backward schedule；返回的函数决定 microbatch 控制流。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 52. 修改或排查「forward_backward_no_pipelining」时应该先看哪些边界？

**参考答案：** backward 沿 autograd 图计算参数梯度；pipeline 中 activation 必须跨 forward/backward 的正确 microbatch 配对，否则梯度会错位。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。backward 沿 autograd 图计算参数梯度；pipeline 中 activation 必须跨 forward/backward 的正确 microbatch 配对，否则梯度会错位。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** backward 沿 autograd 图计算参数梯度；pipeline 中 activation 必须跨 forward/backward 的正确 microbatch 配对，否则梯度会错位。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 53. 为什么「forward_backward_pipelining_without_interleaving」不能只按表面函数名理解？

**参考答案：** backward 沿 autograd 图计算参数梯度；pipeline 中 activation 必须跨 forward/backward 的正确 microbatch 配对，否则梯度会错位。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。backward 沿 autograd 图计算参数梯度；pipeline 中 activation 必须跨 forward/backward 的正确 microbatch 配对，否则梯度会错位。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** backward 沿 autograd 图计算参数梯度；pipeline 中 activation 必须跨 forward/backward 的正确 microbatch 配对，否则梯度会错位。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 54. 围绕「interleaving」设计一个 focused test 应该覆盖什么？

**参考答案：** 「interleaving」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「interleaving」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「interleaving」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 55. 「virtual pipeline」常见的中级误区是什么？

**参考答案：** 「virtual pipeline」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「virtual pipeline」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「virtual pipeline」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 56. Megatron-LM 中「warmup microbatch」的输入、输出和状态变化是什么？

**参考答案：** microbatch 是把一个训练 batch 切成、交给 pipeline schedule 单独推进的小份；它让不同 pipeline stage 可以交错工作。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。microbatch 是把一个训练 batch 切成、交给 pipeline schedule 单独推进的小份；它让不同 pipeline stage 可以交错工作。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** microbatch 是把一个训练 batch 切成、交给 pipeline schedule 单独推进的小份；它让不同 pipeline stage 可以交错工作。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 57. 修改或排查「steady state」时应该先看哪些边界？

**参考答案：** 「steady state」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「steady state」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「steady state」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 58. 为什么「cooldown」不能只按表面函数名理解？

**参考答案：** 「cooldown」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「cooldown」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「cooldown」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 59. 围绕「P2P send/recv」设计一个 focused test 应该覆盖什么？

**参考答案：** 「P2P send/recv」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「P2P send/recv」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「P2P send/recv」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 60. 「activation 保存」常见的中级误区是什么？

**参考答案：** 「activation 保存」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「activation 保存」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「activation 保存」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Pipeline schedule”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## G. 梯度和优化器（61-70）

### 61. Megatron-LM 中「finalize_model_grads」的输入、输出和状态变化是什么？

**参考答案：** 「finalize_model_grads」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「finalize_model_grads」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「finalize_model_grads」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 62. 修改或排查「gradient all-reduce」时应该先看哪些边界？

**参考答案：** gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 63. 为什么「reduce-scatter」不能只按表面函数名理解？

**参考答案：** 「reduce-scatter」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「reduce-scatter」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「reduce-scatter」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 64. 围绕「grad buffer」设计一个 focused test 应该覆盖什么？

**参考答案：** 「grad buffer」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「grad buffer」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「grad buffer」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 65. 「distributed optimizer」常见的中级误区是什么？

**参考答案：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 66. Megatron-LM 中「param group」的输入、输出和状态变化是什么？

**参考答案：** 「param group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「param group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「param group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 67. 修改或排查「Adam step」时应该先看哪些边界？

**参考答案：** 「Adam step」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「Adam step」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「Adam step」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 68. 为什么「zero grad」不能只按表面函数名理解？

**参考答案：** 「zero grad」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「zero grad」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「zero grad」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 69. 围绕「clip grad」设计一个 focused test 应该覆盖什么？

**参考答案：** 「clip grad」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「clip grad」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「clip grad」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 70. 「overlap grad reduce」常见的中级误区是什么？

**参考答案：** 「overlap grad reduce」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「overlap grad reduce」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「overlap grad reduce」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“梯度和优化器”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## H. 检查点细节（71-80）

### 71. Megatron-LM 中「sharded_state_dict metadata」的输入、输出和状态变化是什么？

**参考答案：** state_dict 是模块参数/缓冲区的键值状态表示；分布式模型中它可能只是逻辑视图，不等于单个文件。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。state_dict 是模块参数/缓冲区的键值状态表示；分布式模型中它可能只是逻辑视图，不等于单个文件。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** state_dict 是模块参数/缓冲区的键值状态表示；分布式模型中它可能只是逻辑视图，不等于单个文件。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 72. 修改或排查「rank-local shard」时应该先看哪些边界？

**参考答案：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 73. 为什么「strict load」不能只按表面函数名理解？

**参考答案：** 「strict load」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「strict load」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「strict load」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 74. 围绕「resharding」设计一个 focused test 应该覆盖什么？

**参考答案：** 「resharding」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「resharding」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「resharding」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 75. 「async save」常见的中级误区是什么？

**参考答案：** 「async save」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「async save」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「async save」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 76. Megatron-LM 中「optimizer checkpoint」的输入、输出和状态变化是什么？

**参考答案：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 77. 修改或排查「RNG checkpoint」时应该先看哪些边界？

**参考答案：** 「RNG checkpoint」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「RNG checkpoint」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「RNG checkpoint」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 78. 为什么「release 兼容」不能只按表面函数名理解？

**参考答案：** 「release 兼容」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「release 兼容」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「release 兼容」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 79. 围绕「路径布局」设计一个 focused test 应该覆盖什么？

**参考答案：** 「路径布局」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「路径布局」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「路径布局」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 80. 「恢复验证」常见的中级误区是什么？

**参考答案：** 「恢复验证」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「恢复验证」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「恢复验证」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“检查点细节”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## I. 测试设计（81-90）

### 81. Megatron-LM 中「test_parallel_state」的输入、输出和状态变化是什么？

**参考答案：** 「test_parallel_state」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「test_parallel_state」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「test_parallel_state」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 82. 修改或排查「pipeline schedule tests」时应该先看哪些边界？

**参考答案：** 「pipeline schedule tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「pipeline schedule tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「pipeline schedule tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 83. 为什么「model unit tests」不能只按表面函数名理解？

**参考答案：** 「model unit tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「model unit tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「model unit tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 84. 围绕「dataset tests」设计一个 focused test 应该覆盖什么？

**参考答案：** dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 85. 「optimizer tests」常见的中级误区是什么？

**参考答案：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 86. Megatron-LM 中「checkpoint tests」的输入、输出和状态变化是什么？

**参考答案：** 「checkpoint tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「checkpoint tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「checkpoint tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 87. 修改或排查「single test」时应该先看哪些边界？

**参考答案：** 「single test」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「single test」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「single test」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 88. 为什么「torch.distributed.run」不能只按表面函数名理解？

**参考答案：** 「torch.distributed.run」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「torch.distributed.run」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「torch.distributed.run」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 89. 围绕「golden values」设计一个 focused test 应该覆盖什么？

**参考答案：** golden value 是可重复的参考输出；测试应同时固定输入、seed、dtype 和允许误差，避免把非确定性当 bug。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。golden value 是可重复的参考输出；测试应同时固定输入、seed、dtype 和允许误差，避免把非确定性当 bug。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** golden value 是可重复的参考输出；测试应同时固定输入、seed、dtype 和允许误差，避免把非确定性当 bug。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 90. 「skip 条件」常见的中级误区是什么？

**参考答案：** 「skip 条件」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「skip 条件」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「skip 条件」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## J. 中级故障（91-100）

### 91. Megatron-LM 中「初始化 hang」的输入、输出和状态变化是什么？

**参考答案：** 「初始化 hang」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「初始化 hang」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「初始化 hang」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 92. 修改或排查「collective 不匹配」时应该先看哪些边界？

**参考答案：** 「collective 不匹配」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「collective 不匹配」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「collective 不匹配」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 93. 为什么「pipeline stage shape 错」不能只按表面函数名理解？

**参考答案：** 「pipeline stage shape 错」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「pipeline stage shape 错」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「pipeline stage shape 错」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 94. 围绕「参数未同步」设计一个 focused test 应该覆盖什么？

**参考答案：** 「参数未同步」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「参数未同步」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「参数未同步」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 95. 「数据重复」常见的中级误区是什么？

**参考答案：** 「数据重复」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「数据重复」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「数据重复」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 96. Megatron-LM 中「loss 不下降」的输入、输出和状态变化是什么？

**参考答案：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 97. 修改或排查「恢复后不一致」时应该先看哪些边界？

**参考答案：** 「恢复后不一致」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「恢复后不一致」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「恢复后不一致」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 98. 为什么「TE 缺失 fallback」不能只按表面函数名理解？

**参考答案：** fallback 让缺少可选 backend 时仍可用，但实现、dtype、性能和数值容差可能不同，必须分别测试。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。fallback 让缺少可选 backend 时仍可用，但实现、dtype、性能和数值容差可能不同，必须分别测试。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** fallback 让缺少可选 backend 时仍可用，但实现、dtype、性能和数值容差可能不同，必须分别测试。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 99. 围绕「配置组合冲突」设计一个 focused test 应该覆盖什么？

**参考答案：** 「配置组合冲突」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「配置组合冲突」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「配置组合冲突」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 100. 「日志误读」常见的中级误区是什么？

**参考答案：** 日志是观测训练状态的控制面数据；loss、throughput 和 memory 若统计窗口不同，不能直接比较。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。日志是观测训练状态的控制面数据；loss、throughput 和 memory 若统计窗口不同，不能直接比较。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 日志是观测训练状态的控制面数据；loss、throughput 和 memory 若统计窗口不同，不能直接比较。 本题属于“中级故障”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须指出调用者、输入输出、生命周期、所有权和 focused test，不能停留在名词定义。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。


## 使用建议

先遮住答案自测，再对照四段内容复盘。回答技术题时，明确对象、输入输出、状态变化、资源所有权、失败语义和验证边界；不要把未执行的 GPU、NPU、模型、多卡或性能命令写成运行事实。
