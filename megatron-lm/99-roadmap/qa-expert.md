# 专家级面试题：Megatron-LM
- 题数：100
- 适用对象：能够设计系统级修改、验证不变量和定位复杂分布式/GPU 问题的读者
- 证据锚点：当前知识库记录的 checkout `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`。
- 作答要求：每题包含参考答案、小白解释、技术分析和拓展分析；涉及 GPU/多卡/性能的结论必须区分静态源码理解与实际运行验证。
- 项目一句话：Megatron-LM/Megatron Core 面向大规模 Transformer 训练，把模型装配、并行拓扑、pipeline 调度、数据、优化器和分布式 checkpoint 组合成训练作业。
- 主要证据：README、00-overview、01-modules/M01-core-models、module-registry、D01 最小训练 Demo 和构建/运行文档。

## A. 系统不变量（1-10）

### 1. 如何形式化「rank 拓扑不变量」的系统级不变量？

**参考答案：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 2. 围绕「collective 顺序」设计专家级故障注入和观测方案。

**参考答案：** 「collective 顺序」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「collective 顺序」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「collective 顺序」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 3. 如果重构「参数 shard ownership」，如何证明不会破坏端到端语义？

**参考答案：** 「参数 shard ownership」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「参数 shard ownership」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「参数 shard ownership」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 4. 请给出「activation 生命周期」的成本模型、失败模式和验证边界。

**参考答案：** 「activation 生命周期」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「activation 生命周期」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「activation 生命周期」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 5. 在维护者视角下，如何审查「gradient commit point」相关变更？

**参考答案：** gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 6. 如何形式化「optimizer 原子性」的系统级不变量？

**参考答案：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 7. 围绕「checkpoint 一致性」设计专家级故障注入和观测方案。

**参考答案：** 「checkpoint 一致性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「checkpoint 一致性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「checkpoint 一致性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 8. 如果重构「seed/state 一致性」，如何证明不会破坏端到端语义？

**参考答案：** 「seed/state 一致性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「seed/state 一致性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「seed/state 一致性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 9. 请给出「异常终态」的成本模型、失败模式和验证边界。

**参考答案：** 「异常终态」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「异常终态」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「异常终态」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 10. 在维护者视角下，如何审查「资源释放」相关变更？

**参考答案：** 「资源释放」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「资源释放」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「资源释放」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“系统不变量”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## B. 拓扑推导（11-20）

### 11. 如何形式化「RankGenerator 公式」的系统级不变量？

**参考答案：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 12. 围绕「TP/PP/DP/CP/EP group 枚举」设计专家级故障注入和观测方案。

**参考答案：** 「TP/PP/DP/CP/EP group 枚举」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「TP/PP/DP/CP/EP group 枚举」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「TP/PP/DP/CP/EP group 枚举」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 13. 如果重构「rank order 变更」，如何证明不会破坏端到端语义？

**参考答案：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 14. 请给出「虚拟 pipeline 映射」的成本模型、失败模式和验证边界。

**参考答案：** 「虚拟 pipeline 映射」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「虚拟 pipeline 映射」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「虚拟 pipeline 映射」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 15. 在维护者视角下，如何审查「多节点 placement」相关变更？

**参考答案：** 「多节点 placement」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「多节点 placement」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「多节点 placement」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 16. 如何形式化「local/global rank」的系统级不变量？

**参考答案：** global rank 用于整个 world 的身份和通信拓扑；跨节点时相同 local rank 仍可能属于不同物理节点。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。global rank 用于整个 world 的身份和通信拓扑；跨节点时相同 local rank 仍可能属于不同物理节点。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** global rank 用于整个 world 的身份和通信拓扑；跨节点时相同 local rank 仍可能属于不同物理节点。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 17. 围绕「device binding」设计专家级故障注入和观测方案。

**参考答案：** 「device binding」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「device binding」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「device binding」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 18. 如果重构「别名 group」，如何证明不会破坏端到端语义？

**参考答案：** 「别名 group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「别名 group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「别名 group」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 19. 请给出「destroy 顺序」的成本模型、失败模式和验证边界。

**参考答案：** 「destroy 顺序」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「destroy 顺序」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「destroy 顺序」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 20. 在维护者视角下，如何审查「deadlock 证明」相关变更？

**参考答案：** 「deadlock 证明」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「deadlock 证明」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「deadlock 证明」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“拓扑推导”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## C. 调度理论（21-30）

### 21. 如何形式化「pipeline bubble 公式」的系统级不变量？

**参考答案：** 「pipeline bubble 公式」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「pipeline bubble 公式」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「pipeline bubble 公式」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 22. 围绕「microbatch 最优值」设计专家级故障注入和观测方案。

**参考答案：** microbatch 是把一个训练 batch 切成、交给 pipeline schedule 单独推进的小份；它让不同 pipeline stage 可以交错工作。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。microbatch 是把一个训练 batch 切成、交给 pipeline schedule 单独推进的小份；它让不同 pipeline stage 可以交错工作。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** microbatch 是把一个训练 batch 切成、交给 pipeline schedule 单独推进的小份；它让不同 pipeline stage 可以交错工作。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 23. 如果重构「interleaving 成本」，如何证明不会破坏端到端语义？

**参考答案：** 「interleaving 成本」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「interleaving 成本」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「interleaving 成本」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 24. 请给出「P2P 背压」的成本模型、失败模式和验证边界。

**参考答案：** 「P2P 背压」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「P2P 背压」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「P2P 背压」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 25. 在维护者视角下，如何审查「stage imbalance 建模」相关变更？

**参考答案：** 「stage imbalance 建模」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「stage imbalance 建模」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「stage imbalance 建模」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 26. 如何形式化「activation recompute tradeoff」的系统级不变量？

**参考答案：** 「activation recompute tradeoff」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「activation recompute tradeoff」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「activation recompute tradeoff」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 27. 围绕「schedule correctness」设计专家级故障注入和观测方案。

**参考答案：** 「schedule correctness」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「schedule correctness」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「schedule correctness」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 28. 如果重构「zero bubble 思路」，如何证明不会破坏端到端语义？

**参考答案：** 「zero bubble 思路」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「zero bubble 思路」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「zero bubble 思路」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 29. 请给出「异步调度风险」的成本模型、失败模式和验证边界。

**参考答案：** 「异步调度风险」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「异步调度风险」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「异步调度风险」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 30. 在维护者视角下，如何审查「调度测试矩阵」相关变更？

**参考答案：** 「调度测试矩阵」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「调度测试矩阵」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「调度测试矩阵」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“调度理论”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## D. 成本模型（31-40）

### 31. 如何形式化「FLOPs 估算」的系统级不变量？

**参考答案：** 「FLOPs 估算」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「FLOPs 估算」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「FLOPs 估算」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 32. 围绕「attention O(S^2)」设计专家级故障注入和观测方案。

**参考答案：** attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 33. 如果重构「MLP FLOPs」，如何证明不会破坏端到端语义？

**参考答案：** MLP/FFN 通常先扩展 hidden 维度再投影回来；它常是计算量大、适合张量并行或融合 kernel 的模块。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。MLP/FFN 通常先扩展 hidden 维度再投影回来；它常是计算量大、适合张量并行或融合 kernel 的模块。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** MLP/FFN 通常先扩展 hidden 维度再投影回来；它常是计算量大、适合张量并行或融合 kernel 的模块。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 34. 请给出「通信字节数」的成本模型、失败模式和验证边界。

**参考答案：** 「通信字节数」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「通信字节数」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「通信字节数」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 35. 在维护者视角下，如何审查「KV/activation 内存」相关变更？

**参考答案：** 「KV/activation 内存」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「KV/activation 内存」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「KV/activation 内存」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 36. 如何形式化「optimizer state 内存」的系统级不变量？

**参考答案：** optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 37. 围绕「checkpoint I/O」设计专家级故障注入和观测方案。

**参考答案：** 「checkpoint I/O」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「checkpoint I/O」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「checkpoint I/O」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 38. 如果重构「网络拓扑」，如何证明不会破坏端到端语义？

**参考答案：** 「网络拓扑」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「网络拓扑」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「网络拓扑」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 39. 请给出「MFU 分解」的成本模型、失败模式和验证边界。

**参考答案：** 「MFU 分解」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「MFU 分解」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「MFU 分解」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 40. 在维护者视角下，如何审查「端到端瓶颈归因」相关变更？

**参考答案：** 「端到端瓶颈归因」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「端到端瓶颈归因」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「端到端瓶颈归因」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“成本模型”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## E. 数值专家题（41-50）

### 41. 如何形式化「FP8 amax 同步」的系统级不变量？

**参考答案：** FP8 用更低位宽换吞吐和显存，依赖 scale/amax recipe；scale 统计、通信和算子支持共同决定数值结果。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。FP8 用更低位宽换吞吐和显存，依赖 scale/amax recipe；scale 统计、通信和算子支持共同决定数值结果。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** FP8 用更低位宽换吞吐和显存，依赖 scale/amax recipe；scale 统计、通信和算子支持共同决定数值结果。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 42. 围绕「per-tensor scale」设计专家级故障注入和观测方案。

**参考答案：** 「per-tensor scale」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「per-tensor scale」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「per-tensor scale」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 43. 如果重构「per-channel scale」，如何证明不会破坏端到端语义？

**参考答案：** 「per-channel scale」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「per-channel scale」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「per-channel scale」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 44. 请给出「FP4 误差」的成本模型、失败模式和验证边界。

**参考答案：** FP4 进一步压缩权重或激活，误差和 kernel/layout 约束更强；不能只看 dtype 名称判断可用性。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。FP4 进一步压缩权重或激活，误差和 kernel/layout 约束更强；不能只看 dtype 名称判断可用性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** FP4 进一步压缩权重或激活，误差和 kernel/layout 约束更强；不能只看 dtype 名称判断可用性。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 45. 在维护者视角下，如何审查「LayerNorm 精度」相关变更？

**参考答案：** 「LayerNorm 精度」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「LayerNorm 精度」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「LayerNorm 精度」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 46. 如何形式化「softmax 稳定性」的系统级不变量？

**参考答案：** 「softmax 稳定性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「softmax 稳定性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「softmax 稳定性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 47. 围绕「grad underflow」设计专家级故障注入和观测方案。

**参考答案：** 「grad underflow」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「grad underflow」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「grad underflow」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 48. 如果重构「reduction order」，如何证明不会破坏端到端语义？

**参考答案：** 「reduction order」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「reduction order」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「reduction order」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 49. 请给出「确定性与性能」的成本模型、失败模式和验证边界。

**参考答案：** 「确定性与性能」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「确定性与性能」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「确定性与性能」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 50. 在维护者视角下，如何审查「tolerance 设计」相关变更？

**参考答案：** 「tolerance 设计」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「tolerance 设计」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「tolerance 设计」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“数值专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## F. MoE 专家题（51-60）

### 51. 如何形式化「router 稳定性」的系统级不变量？

**参考答案：** 「router 稳定性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「router 稳定性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「router 稳定性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 52. 围绕「expert capacity overflow」设计专家级故障注入和观测方案。

**参考答案：** 「expert capacity overflow」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「expert capacity overflow」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「expert capacity overflow」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 53. 如果重构「token permutation」，如何证明不会破坏端到端语义？

**参考答案：** 文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 54. 请给出「all-to-all correctness」的成本模型、失败模式和验证边界。

**参考答案：** 「all-to-all correctness」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「all-to-all correctness」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「all-to-all correctness」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 55. 在维护者视角下，如何审查「load balance loss」相关变更？

**参考答案：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 56. 如何形式化「expert parallel checkpoint」的系统级不变量？

**参考答案：** 「expert parallel checkpoint」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「expert parallel checkpoint」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「expert parallel checkpoint」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 57. 围绕「shared expert」设计专家级故障注入和观测方案。

**参考答案：** 「shared expert」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「shared expert」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「shared expert」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 58. 如果重构「分层 MoE」，如何证明不会破坏端到端语义？

**参考答案：** 「分层 MoE」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「分层 MoE」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「分层 MoE」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 59. 请给出「MoE+PP」的成本模型、失败模式和验证边界。

**参考答案：** 「MoE+PP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「MoE+PP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「MoE+PP」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 60. 在维护者视角下，如何审查「MoE fault injection」相关变更？

**参考答案：** 「MoE fault injection」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「MoE fault injection」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「MoE fault injection」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“MoE 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## G. Checkpoint 专家题（61-70）

### 61. 如何形式化「two-phase commit」的系统级不变量？

**参考答案：** 「two-phase commit」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「two-phase commit」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「two-phase commit」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 62. 围绕「metadata schema」设计专家级故障注入和观测方案。

**参考答案：** 「metadata schema」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「metadata schema」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「metadata schema」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 63. 如果重构「异步保存 crash」，如何证明不会破坏端到端语义？

**参考答案：** 「异步保存 crash」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「异步保存 crash」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「异步保存 crash」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 64. 请给出「跨 TP/PP 重分片」的成本模型、失败模式和验证边界。

**参考答案：** 「跨 TP/PP 重分片」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「跨 TP/PP 重分片」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「跨 TP/PP 重分片」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 65. 在维护者视角下，如何审查「optimizer state migration」相关变更？

**参考答案：** optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 66. 如何形式化「RNG 恢复」的系统级不变量？

**参考答案：** 「RNG 恢复」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「RNG 恢复」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「RNG 恢复」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 67. 围绕「数据迭代恢复」设计专家级故障注入和观测方案。

**参考答案：** 「数据迭代恢复」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「数据迭代恢复」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「数据迭代恢复」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 68. 如果重构「云存储一致性」，如何证明不会破坏端到端语义？

**参考答案：** 「云存储一致性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「云存储一致性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「云存储一致性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 69. 请给出「checkpoint GC」的成本模型、失败模式和验证边界。

**参考答案：** 「checkpoint GC」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「checkpoint GC」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「checkpoint GC」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 70. 在维护者视角下，如何审查「恢复等价性证明」相关变更？

**参考答案：** 「恢复等价性证明」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「恢复等价性证明」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「恢复等价性证明」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“Checkpoint 专家题”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## H. 扩展架构（71-80）

### 71. 如何形式化「新增 Transformer 变体」的系统级不变量？

**参考答案：** Transformer 由 embedding、注意力和前馈网络等层组成；训练时同一序列的 token 可并行计算，而注意力 mask 决定每个位置能看到哪些输入。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。Transformer 由 embedding、注意力和前馈网络等层组成；训练时同一序列的 token 可并行计算，而注意力 mask 决定每个位置能看到哪些输入。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** Transformer 由 embedding、注意力和前馈网络等层组成；训练时同一序列的 token 可并行计算，而注意力 mask 决定每个位置能看到哪些输入。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 72. 围绕「新增 attention backend」设计专家级故障注入和观测方案。

**参考答案：** attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 73. 如果重构「新增 optimizer」，如何证明不会破坏端到端语义？

**参考答案：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 74. 请给出「新增 dataset 格式」的成本模型、失败模式和验证边界。

**参考答案：** dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 75. 在维护者视角下，如何审查「新增推理入口」相关变更？

**参考答案：** 「新增推理入口」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「新增推理入口」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「新增推理入口」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 76. 如何形式化「新增 precision recipe」的系统级不变量？

**参考答案：** 「新增 precision recipe」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「新增 precision recipe」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「新增 precision recipe」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 77. 围绕「新增 parallel dimension」设计专家级故障注入和观测方案。

**参考答案：** 「新增 parallel dimension」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「新增 parallel dimension」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「新增 parallel dimension」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 78. 如果重构「对接外部调度器」，如何证明不会破坏端到端语义？

**参考答案：** 「对接外部调度器」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「对接外部调度器」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「对接外部调度器」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 79. 请给出「API 稳定性」的成本模型、失败模式和验证边界。

**参考答案：** 「API 稳定性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「API 稳定性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「API 稳定性」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 80. 在维护者视角下，如何审查「文档证据更新」相关变更？

**参考答案：** 「文档证据更新」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「文档证据更新」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「文档证据更新」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“扩展架构”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## I. 观测与调试（81-90）

### 81. 如何形式化「分布式 trace schema」的系统级不变量？

**参考答案：** 「分布式 trace schema」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「分布式 trace schema」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「分布式 trace schema」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 82. 围绕「collective sequence id」设计专家级故障注入和观测方案。

**参考答案：** 「collective sequence id」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「collective sequence id」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「collective sequence id」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 83. 如果重构「per-rank heartbeat」，如何证明不会破坏端到端语义？

**参考答案：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 84. 请给出「CUDA event timeline」的成本模型、失败模式和验证边界。

**参考答案：** CUDA 是 GPU 执行和内存的运行时基础；Python 层调用成功不代表 kernel 已按预期执行，异步错误常需同步后才暴露。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。CUDA 是 GPU 执行和内存的运行时基础；Python 层调用成功不代表 kernel 已按预期执行，异步错误常需同步后才暴露。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** CUDA 是 GPU 执行和内存的运行时基础；Python 层调用成功不代表 kernel 已按预期执行，异步错误常需同步后才暴露。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 85. 在维护者视角下，如何审查「NCCL debug」相关变更？

**参考答案：** NCCL 为 GPU rank 提供 collective 和点对点通信；所有参与 rank 必须以一致顺序调用兼容 shape 的通信操作。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。NCCL 为 GPU rank 提供 collective 和点对点通信；所有参与 rank 必须以一致顺序调用兼容 shape 的通信操作。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** NCCL 为 GPU rank 提供 collective 和点对点通信；所有参与 rank 必须以一致顺序调用兼容 shape 的通信操作。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 86. 如何形式化「memory snapshot」的系统级不变量？

**参考答案：** 「memory snapshot」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「memory snapshot」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「memory snapshot」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 87. 围绕「loss trace」设计专家级故障注入和观测方案。

**参考答案：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 88. 如果重构「checkpoint audit」，如何证明不会破坏端到端语义？

**参考答案：** 「checkpoint audit」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「checkpoint audit」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「checkpoint audit」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 89. 请给出「fault injection」的成本模型、失败模式和验证边界。

**参考答案：** 「fault injection」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「fault injection」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「fault injection」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 90. 在维护者视角下，如何审查「长尾性能」相关变更？

**参考答案：** 「长尾性能」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「长尾性能」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「长尾性能」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“观测与调试”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## J. 专家面试设计（91-100）

### 91. 如何形式化「端到端训练时序」的系统级不变量？

**参考答案：** 「端到端训练时序」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「端到端训练时序」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「端到端训练时序」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 92. 围绕「从 loss NaN 到 root cause」设计专家级故障注入和观测方案。

**参考答案：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 93. 如果重构「从 hang 到 collective mismatch」，如何证明不会破坏端到端语义？

**参考答案：** 「从 hang 到 collective mismatch」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「从 hang 到 collective mismatch」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「从 hang 到 collective mismatch」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 94. 请给出「从 OOM 到内存模型」的成本模型、失败模式和验证边界。

**参考答案：** 「从 OOM 到内存模型」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「从 OOM 到内存模型」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「从 OOM 到内存模型」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 95. 在维护者视角下，如何审查「从慢 step 到瓶颈」相关变更？

**参考答案：** 「从慢 step 到瓶颈」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「从慢 step 到瓶颈」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「从慢 step 到瓶颈」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 96. 如何形式化「从恢复失败到状态差异」的系统级不变量？

**参考答案：** 「从恢复失败到状态差异」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「从恢复失败到状态差异」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「从恢复失败到状态差异」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 97. 围绕「从新模型到测试矩阵」设计专家级故障注入和观测方案。

**参考答案：** 「从新模型到测试矩阵」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「从新模型到测试矩阵」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「从新模型到测试矩阵」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 98. 如果重构「从代码审查到风险评级」，如何证明不会破坏端到端语义？

**参考答案：** 「从代码审查到风险评级」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「从代码审查到风险评级」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「从代码审查到风险评级」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 99. 请给出「从实验到结论」的成本模型、失败模式和验证边界。

**参考答案：** 「从实验到结论」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「从实验到结论」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「从实验到结论」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 100. 在维护者视角下，如何审查「维护者验收任务」相关变更？

**参考答案：** 「维护者验收任务」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「维护者验收任务」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「维护者验收任务」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“专家面试设计”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。必须明确不变量、提交点、失败分类、观测字段、故障注入和静态证据边界。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。


## 使用建议

先遮住答案自测，再对照四段内容复盘。回答技术题时，明确对象、输入输出、状态变化、资源所有权、失败语义和验证边界；不要把未执行的 GPU、NPU、模型、多卡或性能命令写成运行事实。
