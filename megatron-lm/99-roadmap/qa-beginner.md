# 入门级面试题：Megatron-LM

- 文档目的：解释 99-roadmap/qa-beginner.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/megatron-lm HEAD 3703d4e33（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 99-roadmap/qa-beginner.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

- 题数：100
- 适用对象：第一次接触项目、Transformer、GPU 训练/推理系统的读者
- 证据锚点：当前知识库记录的 checkout `3703d4e33a3a2b2d11ebcc8e41f45af7ce7d1eda`。
- 作答要求：每题包含参考答案、小白解释、技术分析和拓展分析；涉及 GPU/多卡/性能的结论必须区分静态源码理解与实际运行验证。
- 项目一句话：Megatron-LM/Megatron Core 面向大规模 Transformer 训练，把模型装配、并行拓扑、pipeline 调度、数据、优化器和分布式 checkpoint 组合成训练作业。
- 主要证据：README、00-overview、01-modules/M01-core-models、module-registry、D01 最小训练 Demo 和构建/运行文档。

## A. 基础概念（1-10）

### 1. 什么是「大模型训练」，它在 Megatron-LM 中解决什么问题？

**参考答案：** 训练把大量样本反复送入模型，通过 forward 得到 loss、backward 得到梯度，再由 optimizer 更新参数；Megatron 将这条循环拆成可并行的组件。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。训练把大量样本反复送入模型，通过 forward 得到 loss、backward 得到梯度，再由 optimizer 更新参数；Megatron 将这条循环拆成可并行的组件。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 训练把大量样本反复送入模型，通过 forward 得到 loss、backward 得到梯度，再由 optimizer 更新参数；Megatron 将这条循环拆成可并行的组件。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 2. Megatron-LM 里为什么需要理解「Transformer」？

**参考答案：** Transformer 由 embedding、注意力和前馈网络等层组成；训练时同一序列的 token 可并行计算，而注意力 mask 决定每个位置能看到哪些输入。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。Transformer 由 embedding、注意力和前馈网络等层组成；训练时同一序列的 token 可并行计算，而注意力 mask 决定每个位置能看到哪些输入。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** Transformer 由 embedding、注意力和前馈网络等层组成；训练时同一序列的 token 可并行计算，而注意力 mask 决定每个位置能看到哪些输入。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 3. 请用小白能懂的话解释「token」的作用。

**参考答案：** 文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 4. 如果「batch」出错，初学者最容易观察到什么现象？

**参考答案：** batch 是一次更新或一次 forward 处理的样本集合；增大 batch 通常提高设备利用率，但也增加显存、通信和单步等待。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。batch 是一次更新或一次 forward 处理的样本集合；增大 batch 通常提高设备利用率，但也增加显存、通信和单步等待。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** batch 是一次更新或一次 forward 处理的样本集合；增大 batch 通常提高设备利用率，但也增加显存、通信和单步等待。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 5. 「microbatch」和相邻模块之间通常传递什么数据？

**参考答案：** microbatch 是把一个训练 batch 切成、交给 pipeline schedule 单独推进的小份；它让不同 pipeline stage 可以交错工作。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。microbatch 是把一个训练 batch 切成、交给 pipeline schedule 单独推进的小份；它让不同 pipeline stage 可以交错工作。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** microbatch 是把一个训练 batch 切成、交给 pipeline schedule 单独推进的小份；它让不同 pipeline stage 可以交错工作。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 6. 什么是「forward」，它在 Megatron-LM 中解决什么问题？

**参考答案：** forward 用当前参数把输入变成 hidden/logits 或 loss；pipeline 场景下每个 stage 只持有模型的一部分，输入输出还带有 stage 契约。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。forward 用当前参数把输入变成 hidden/logits 或 loss；pipeline 场景下每个 stage 只持有模型的一部分，输入输出还带有 stage 契约。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** forward 用当前参数把输入变成 hidden/logits 或 loss；pipeline 场景下每个 stage 只持有模型的一部分，输入输出还带有 stage 契约。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 7. Megatron-LM 里为什么需要理解「backward」？

**参考答案：** backward 沿 autograd 图计算参数梯度；pipeline 中 activation 必须跨 forward/backward 的正确 microbatch 配对，否则梯度会错位。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。backward 沿 autograd 图计算参数梯度；pipeline 中 activation 必须跨 forward/backward 的正确 microbatch 配对，否则梯度会错位。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** backward 沿 autograd 图计算参数梯度；pipeline 中 activation 必须跨 forward/backward 的正确 microbatch 配对，否则梯度会错位。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 8. 请用小白能懂的话解释「loss」的作用。

**参考答案：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 9. 如果「gradient」出错，初学者最容易观察到什么现象？

**参考答案：** gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** gradient 是参数对 loss 的导数；并行训练要在正确的 group 上同步或分片梯度，并在 optimizer.step 前完成必要的 finalize。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 10. 「optimizer」和相邻模块之间通常传递什么数据？

**参考答案：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** optimizer 根据梯度、学习率和自身状态更新参数；Adam 还保存一阶/二阶动量，因此 checkpoint 不能只保存模型权重。 本题属于“基础概念”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## B. 项目定位（11-20）

### 11. 什么是「Megatron-LM」，它在 Megatron-LM 中解决什么问题？

**参考答案：** Megatron-LM 同时包含可复用的 Megatron Core 和参考训练入口；前者提供组件，后者负责把配置、数据、模型和训练循环编排起来。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。Megatron-LM 同时包含可复用的 Megatron Core 和参考训练入口；前者提供组件，后者负责把配置、数据、模型和训练循环编排起来。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** Megatron-LM 同时包含可复用的 Megatron Core 和参考训练入口；前者提供组件，后者负责把配置、数据、模型和训练循环编排起来。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 12. Megatron-LM 里为什么需要理解「Megatron Core」？

**参考答案：** Megatron Core 将模型、并行、数据、优化器、checkpoint 等能力模块化，便于替换实现，但也使配置矩阵和接口契约更复杂。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。Megatron Core 将模型、并行、数据、优化器、checkpoint 等能力模块化，便于替换实现，但也使配置矩阵和接口契约更复杂。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** Megatron Core 将模型、并行、数据、优化器、checkpoint 等能力模块化，便于替换实现，但也使配置矩阵和接口契约更复杂。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 13. 请用小白能懂的话解释「参考训练脚本」的作用。

**参考答案：** 入口脚本通常解析参数、定义 model_provider 和 dataset provider，再把控制权交给 training runtime；它不是模型层本身。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。入口脚本通常解析参数、定义 model_provider 和 dataset provider，再把控制权交给 training runtime；它不是模型层本身。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 入口脚本通常解析参数、定义 model_provider 和 dataset provider，再把控制权交给 training runtime；它不是模型层本身。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 14. 如果「Core 库」出错，初学者最容易观察到什么现象？

**参考答案：** 「Core 库」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「Core 库」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「Core 库」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 15. 「PyTorch」和相邻模块之间通常传递什么数据？

**参考答案：** PyTorch 提供 tensor、autograd 和 distributed 原语，Megatron 在其上组织模型并行和训练状态；真正的 kernel/通信效果取决于运行环境。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。PyTorch 提供 tensor、autograd 和 distributed 原语，Megatron 在其上组织模型并行和训练状态；真正的 kernel/通信效果取决于运行环境。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** PyTorch 提供 tensor、autograd 和 distributed 原语，Megatron 在其上组织模型并行和训练状态；真正的 kernel/通信效果取决于运行环境。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 16. 什么是「CUDA」，它在 Megatron-LM 中解决什么问题？

**参考答案：** CUDA 是 GPU 执行和内存的运行时基础；Python 层调用成功不代表 kernel 已按预期执行，异步错误常需同步后才暴露。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。CUDA 是 GPU 执行和内存的运行时基础；Python 层调用成功不代表 kernel 已按预期执行，异步错误常需同步后才暴露。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** CUDA 是 GPU 执行和内存的运行时基础；Python 层调用成功不代表 kernel 已按预期执行，异步错误常需同步后才暴露。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 17. Megatron-LM 里为什么需要理解「NCCL」？

**参考答案：** NCCL 为 GPU rank 提供 collective 和点对点通信；所有参与 rank 必须以一致顺序调用兼容 shape 的通信操作。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。NCCL 为 GPU rank 提供 collective 和点对点通信；所有参与 rank 必须以一致顺序调用兼容 shape 的通信操作。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** NCCL 为 GPU rank 提供 collective 和点对点通信；所有参与 rank 必须以一致顺序调用兼容 shape 的通信操作。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 18. 请用小白能懂的话解释「Transformer Engine」的作用。

**参考答案：** Transformer Engine 提供面向 Transformer 的低精度实现和 recipe；是否启用会改变模块 spec、dtype 和数值/性能路径。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。Transformer Engine 提供面向 Transformer 的低精度实现和 recipe；是否启用会改变模块 spec、dtype 和数值/性能路径。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** Transformer Engine 提供面向 Transformer 的低精度实现和 recipe；是否启用会改变模块 spec、dtype 和数值/性能路径。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 19. 如果「examples」出错，初学者最容易观察到什么现象？

**参考答案：** examples 通常是最小可读的真实 API 用法，适合先验证初始化、模型、数据、schedule、optimizer 和 checkpoint 的连接。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。examples 通常是最小可读的真实 API 用法，适合先验证初始化、模型、数据、schedule、optimizer 和 checkpoint 的连接。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** examples 通常是最小可读的真实 API 用法，适合先验证初始化、模型、数据、schedule、optimizer 和 checkpoint 的连接。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 20. 「tools」和相邻模块之间通常传递什么数据？

**参考答案：** tools 包含训练辅助、推理、转换或分析入口；它们复用 Core，但参数与生命周期不一定等同于正式训练脚本。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。tools 包含训练辅助、推理、转换或分析入口；它们复用 Core，但参数与生命周期不一定等同于正式训练脚本。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** tools 包含训练辅助、推理、转换或分析入口；它们复用 Core，但参数与生命周期不一定等同于正式训练脚本。 本题属于“项目定位”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## C. 模型装配（21-30）

### 21. 什么是「TransformerConfig」，它在 Megatron-LM 中解决什么问题？

**参考答案：** TransformerConfig 是模型超参数和执行选项的集中表达；它把层数、隐藏维度、并行相关配置和精度策略传给模型构造。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。TransformerConfig 是模型超参数和执行选项的集中表达；它把层数、隐藏维度、并行相关配置和精度策略传给模型构造。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** TransformerConfig 是模型超参数和执行选项的集中表达；它把层数、隐藏维度、并行相关配置和精度策略传给模型构造。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 22. Megatron-LM 里为什么需要理解「ModuleSpec」？

**参考答案：** ModuleSpec 描述某个模块应使用哪个实现以及如何组合子模块；它把“选择实现”和“实例化对象”分开。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。ModuleSpec 描述某个模块应使用哪个实现以及如何组合子模块；它把“选择实现”和“实例化对象”分开。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** ModuleSpec 描述某个模块应使用哪个实现以及如何组合子模块；它把“选择实现”和“实例化对象”分开。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 23. 请用小白能懂的话解释「model_provider」的作用。

**参考答案：** model_provider 是应用层和 Core 模型之间的装配回调；它把解析后的配置交给 builder 或直接构造模型。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。model_provider 是应用层和 Core 模型之间的装配回调；它把解析后的配置交给 builder 或直接构造模型。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** model_provider 是应用层和 Core 模型之间的装配回调；它把解析后的配置交给 builder 或直接构造模型。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 24. 如果「gpt_builder」出错，初学者最容易观察到什么现象？

**参考答案：** gpt_builder 根据 spec、TE、MoE 或异构层选项生成 GPT 所需 ModuleSpec；分支选择错误会在构造或第一次 forward 才暴露。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。gpt_builder 根据 spec、TE、MoE 或异构层选项生成 GPT 所需 ModuleSpec；分支选择错误会在构造或第一次 forward 才暴露。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** gpt_builder 根据 spec、TE、MoE 或异构层选项生成 GPT 所需 ModuleSpec；分支选择错误会在构造或第一次 forward 才暴露。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 25. 「GPTModel」和相邻模块之间通常传递什么数据？

**参考答案：** GPTModel 负责 embedding、decoder、输出头以及 forward 契约，但不负责整个训练循环或 optimizer.step。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。GPTModel 负责 embedding、decoder、输出头以及 forward 契约，但不负责整个训练循环或 optimizer.step。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** GPTModel 负责 embedding、decoder、输出头以及 forward 契约，但不负责整个训练循环或 optimizer.step。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 26. 什么是「embedding」，它在 Megatron-LM 中解决什么问题？

**参考答案：** embedding 把 token ID 映射为 hidden 向量；共享输入 embedding 与输出 head 时，参数 storage、分片和 checkpoint 语义要保持一致。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。embedding 把 token ID 映射为 hidden 向量；共享输入 embedding 与输出 head 时，参数 storage、分片和 checkpoint 语义要保持一致。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** embedding 把 token ID 映射为 hidden 向量；共享输入 embedding 与输出 head 时，参数 storage、分片和 checkpoint 语义要保持一致。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 27. Megatron-LM 里为什么需要理解「TransformerBlock」？

**参考答案：** TransformerBlock 通常串联归一化、self-attention 和 MLP；残差连接和 tensor shape 是上下层之间的核心契约。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。TransformerBlock 通常串联归一化、self-attention 和 MLP；残差连接和 tensor shape 是上下层之间的核心契约。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** TransformerBlock 通常串联归一化、self-attention 和 MLP；残差连接和 tensor shape 是上下层之间的核心契约。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 28. 请用小白能懂的话解释「attention」的作用。

**参考答案：** attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** attention 根据 query、key、value 和 mask 聚合上下文；训练的全序列 attention 与推理的 KV cache/增量路径在内存和 shape 上不同。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 29. 如果「MLP」出错，初学者最容易观察到什么现象？

**参考答案：** MLP/FFN 通常先扩展 hidden 维度再投影回来；它常是计算量大、适合张量并行或融合 kernel 的模块。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。MLP/FFN 通常先扩展 hidden 维度再投影回来；它常是计算量大、适合张量并行或融合 kernel 的模块。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** MLP/FFN 通常先扩展 hidden 维度再投影回来；它常是计算量大、适合张量并行或融合 kernel 的模块。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 30. 「LM head」和相邻模块之间通常传递什么数据？

**参考答案：** LM head 把最终 hidden 投影到词表 logits；词表维度大，分片、权重共享和 loss 计算会影响通信与显存。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。LM head 把最终 hidden 投影到词表 logits；词表维度大，分片、权重共享和 loss 计算会影响通信与显存。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** LM head 把最终 hidden 投影到词表 logits；词表维度大，分片、权重共享和 loss 计算会影响通信与显存。 本题属于“模型装配”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## D. 数据流程（31-40）

### 31. 什么是「dataset」，它在 Megatron-LM 中解决什么问题？

**参考答案：** dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** dataset 把原始 token 或样本组织成训练样本；索引、切分和随机顺序必须与 rank 数及恢复语义协调。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 32. Megatron-LM 里为什么需要理解「tokenizer」？

**参考答案：** tokenizer 决定文本到 token ID 的映射；更换 tokenizer 会改变 vocab、序列长度、special token 和 checkpoint/数据兼容性。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。tokenizer 决定文本到 token ID 的映射；更换 tokenizer 会改变 vocab、序列长度、special token 和 checkpoint/数据兼容性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** tokenizer 决定文本到 token ID 的映射；更换 tokenizer 会改变 vocab、序列长度、special token 和 checkpoint/数据兼容性。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 33. 请用小白能懂的话解释「DataLoader」的作用。

**参考答案：** DataLoader 负责按 batch 迭代样本；它应与 data parallel rank 的数据分工配合，避免多个 rank 意外消费相同样本。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。DataLoader 负责按 batch 迭代样本；它应与 data parallel rank 的数据分工配合，避免多个 rank 意外消费相同样本。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** DataLoader 负责按 batch 迭代样本；它应与 data parallel rank 的数据分工配合，避免多个 rank 意外消费相同样本。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 34. 如果「tokens」出错，初学者最容易观察到什么现象？

**参考答案：** 文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 文本先由 tokenizer 转成整数 ID，模型只处理这些 ID；词表、special token 和序列长度会直接影响数据形状和显存。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 35. 「position_ids」和相邻模块之间通常传递什么数据？

**参考答案：** position_ids 为 token 提供位置；位置编码方案、序列并行和 packed sequence 都要求它与 attention 输入保持对齐。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。position_ids 为 token 提供位置；位置编码方案、序列并行和 packed sequence 都要求它与 attention 输入保持对齐。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** position_ids 为 token 提供位置；位置编码方案、序列并行和 packed sequence 都要求它与 attention 输入保持对齐。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 36. 什么是「attention mask」，它在 Megatron-LM 中解决什么问题？

**参考答案：** attention mask 限制可见关系；mask 错一位可能不崩溃却让模型偷看未来 token，属于高危静默正确性问题。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。attention mask 限制可见关系；mask 错一位可能不崩溃却让模型偷看未来 token，属于高危静默正确性问题。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** attention mask 限制可见关系；mask 错一位可能不崩溃却让模型偷看未来 token，属于高危静默正确性问题。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 37. Megatron-LM 里为什么需要理解「labels」？

**参考答案：** labels 是 loss 的目标 token；shift、ignore index 和 padding 处理决定每个位置是否真的参与梯度。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。labels 是 loss 的目标 token；shift、ignore index 和 padding 处理决定每个位置是否真的参与梯度。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** labels 是 loss 的目标 token；shift、ignore index 和 padding 处理决定每个位置是否真的参与梯度。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 38. 请用小白能懂的话解释「loss mask」的作用。

**参考答案：** loss mask 指出哪些 label 计入损失；分母和 mask 归约不一致会造成不同 batch/不同 rank 的 loss 不可比。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。loss mask 指出哪些 label 计入损失；分母和 mask 归约不一致会造成不同 batch/不同 rank 的 loss 不可比。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** loss mask 指出哪些 label 计入损失；分母和 mask 归约不一致会造成不同 batch/不同 rank 的 loss 不可比。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 39. 如果「mock dataset」出错，初学者最容易观察到什么现象？

**参考答案：** MockGPTDataset 用可控数据验证训练骨架，不等于真实数据质量；它适合检查字段、shape、梯度和 checkpoint 闭环。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。MockGPTDataset 用可控数据验证训练骨架，不等于真实数据质量；它适合检查字段、shape、梯度和 checkpoint 闭环。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** MockGPTDataset 用可控数据验证训练骨架，不等于真实数据质量；它适合检查字段、shape、梯度和 checkpoint 闭环。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 40. 「真实数据集」和相邻模块之间通常传递什么数据？

**参考答案：** 真实数据集还涉及索引文件、预处理、缓存、数据顺序和数据质量；问题可能在模型前就已发生。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。真实数据集还涉及索引文件、预处理、缓存、数据顺序和数据质量；问题可能在模型前就已发生。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 真实数据集还涉及索引文件、预处理、缓存、数据顺序和数据质量；问题可能在模型前就已发生。 本题属于“数据流程”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## E. 并行基础（41-50）

### 41. 什么是「rank」，它在 Megatron-LM 中解决什么问题？

**参考答案：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 42. Megatron-LM 里为什么需要理解「world size」？

**参考答案：** world size 是参与 default process group 的进程总数；并行维度乘积和实际 world size 不一致会使初始化或通信失败。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。world size 是参与 default process group 的进程总数；并行维度乘积和实际 world size 不一致会使初始化或通信失败。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** world size 是参与 default process group 的进程总数；并行维度乘积和实际 world size 不一致会使初始化或通信失败。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 43. 请用小白能懂的话解释「tensor parallelism」的作用。

**参考答案：** TP 把同一层的权重或激活沿计算维度切到多个 rank；局部计算结束后通常需要 all-reduce/all-gather 等通信。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。TP 把同一层的权重或激活沿计算维度切到多个 rank；局部计算结束后通常需要 all-reduce/all-gather 等通信。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** TP 把同一层的权重或激活沿计算维度切到多个 rank；局部计算结束后通常需要 all-reduce/all-gather 等通信。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 44. 如果「pipeline parallelism」出错，初学者最容易观察到什么现象？

**参考答案：** PP 把连续层分配给不同 stage；microbatch 在 stage 间传递 activation，前后向顺序决定 bubble 和显存。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。PP 把连续层分配给不同 stage；microbatch 在 stage 间传递 activation，前后向顺序决定 bubble 和显存。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** PP 把连续层分配给不同 stage；microbatch 在 stage 间传递 activation，前后向顺序决定 bubble 和显存。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 45. 「data parallelism」和相邻模块之间通常传递什么数据？

**参考答案：** DP 在不同 rank 保留模型副本、处理不同数据，并同步梯度；它的通信和有效 batch 规模受 accumulation 影响。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。DP 在不同 rank 保留模型副本、处理不同数据，并同步梯度；它的通信和有效 batch 规模受 accumulation 影响。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** DP 在不同 rank 保留模型副本、处理不同数据，并同步梯度；它的通信和有效 batch 规模受 accumulation 影响。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 46. 什么是「context parallelism」，它在 Megatron-LM 中解决什么问题？

**参考答案：** CP 沿上下文维度拆分序列或 attention 工作；它能降低单卡长序列压力，但增加跨 rank 的数据交换和 mask 契约。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。CP 沿上下文维度拆分序列或 attention 工作；它能降低单卡长序列压力，但增加跨 rank 的数据交换和 mask 契约。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** CP 沿上下文维度拆分序列或 attention 工作；它能降低单卡长序列压力，但增加跨 rank 的数据交换和 mask 契约。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 47. Megatron-LM 里为什么需要理解「expert parallelism」？

**参考答案：** EP 将 MoE experts 分布到不同 rank；router 产生的 token dispatch/combine 是动态通信，负载不均会拖慢最忙 rank。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。EP 将 MoE experts 分布到不同 rank；router 产生的 token dispatch/combine 是动态通信，负载不均会拖慢最忙 rank。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** EP 将 MoE experts 分布到不同 rank；router 产生的 token dispatch/combine 是动态通信，负载不均会拖慢最忙 rank。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 48. 请用小白能懂的话解释「process group」的作用。

**参考答案：** ProcessGroup 定义一次通信的参与者集合；同一个 global rank 可属于多个语义不同的 group，调用点必须选对 group。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。ProcessGroup 定义一次通信的参与者集合；同一个 global rank 可属于多个语义不同的 group，调用点必须选对 group。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** ProcessGroup 定义一次通信的参与者集合；同一个 global rank 可属于多个语义不同的 group，调用点必须选对 group。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 49. 如果「local rank」出错，初学者最容易观察到什么现象？

**参考答案：** local rank 通常用于节点内设备绑定；它受 CUDA_VISIBLE_DEVICES 和 launcher 影响，不能脱离 placement 解释。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。local rank 通常用于节点内设备绑定；它受 CUDA_VISIBLE_DEVICES 和 launcher 影响，不能脱离 placement 解释。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** local rank 通常用于节点内设备绑定；它受 CUDA_VISIBLE_DEVICES 和 launcher 影响，不能脱离 placement 解释。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 50. 「global rank」和相邻模块之间通常传递什么数据？

**参考答案：** global rank 用于整个 world 的身份和通信拓扑；跨节点时相同 local rank 仍可能属于不同物理节点。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。global rank 用于整个 world 的身份和通信拓扑；跨节点时相同 local rank 仍可能属于不同物理节点。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** global rank 用于整个 world 的身份和通信拓扑；跨节点时相同 local rank 仍可能属于不同物理节点。 本题属于“并行基础”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## F. 训练循环（51-60）

### 51. 什么是「initialize_megatron」，它在 Megatron-LM 中解决什么问题？

**参考答案：** initialize_megatron 连接参数解析、设备、default process group、model parallel groups、随机种子和可选初始化；顺序本身是契约。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。initialize_megatron 连接参数解析、设备、default process group、model parallel groups、随机种子和可选初始化；顺序本身是契约。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** initialize_megatron 连接参数解析、设备、default process group、model parallel groups、随机种子和可选初始化；顺序本身是契约。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 52. Megatron-LM 里为什么需要理解「pretrain」？

**参考答案：** pretrain 将 provider、dataset、optimizer 和 training loop 组合成正式作业；它还要处理日志、保存、恢复和终止。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。pretrain 将 provider、dataset、optimizer 和 training loop 组合成正式作业；它还要处理日志、保存、恢复和终止。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** pretrain 将 provider、dataset、optimizer 和 training loop 组合成正式作业；它还要处理日志、保存、恢复和终止。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 53. 请用小白能懂的话解释「train_step」的作用。

**参考答案：** train_step 驱动一组 microbatch 的 forward/backward，再完成梯度处理和参数更新；step 边界是吞吐和 checkpoint 计数的重要单位。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。train_step 驱动一组 microbatch 的 forward/backward，再完成梯度处理和参数更新；step 边界是吞吐和 checkpoint 计数的重要单位。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** train_step 驱动一组 microbatch 的 forward/backward，再完成梯度处理和参数更新；step 边界是吞吐和 checkpoint 计数的重要单位。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 54. 如果「forward_step_func」出错，初学者最容易观察到什么现象？

**参考答案：** forward_step_func 从 batch 取出输入字段，调用 model 并返回 loss/metrics；数据字段和 loss closure 是 schedule 的接口契约。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。forward_step_func 从 batch 取出输入字段，调用 model 并返回 loss/metrics；数据字段和 loss closure 是 schedule 的接口契约。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** forward_step_func 从 batch 取出输入字段，调用 model 并返回 loss/metrics；数据字段和 loss closure 是 schedule 的接口契约。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 55. 「get_forward_backward_func」和相邻模块之间通常传递什么数据？

**参考答案：** 它根据 pipeline、virtual pipeline 和配置选择合适的 forward-backward schedule；返回的函数决定 microbatch 控制流。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。它根据 pipeline、virtual pipeline 和配置选择合适的 forward-backward schedule；返回的函数决定 microbatch 控制流。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 它根据 pipeline、virtual pipeline 和配置选择合适的 forward-backward schedule；返回的函数决定 microbatch 控制流。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 56. 什么是「no-pipeline schedule」，它在 Megatron-LM 中解决什么问题？

**参考答案：** 「no-pipeline schedule」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「no-pipeline schedule」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「no-pipeline schedule」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 57. Megatron-LM 里为什么需要理解「1F1B」？

**参考答案：** 1F1B 在 pipeline warmup 后交替推进 forward 和 backward；它减少空闲但需要保存恰当 activation 并正确进行 P2P。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。1F1B 在 pipeline warmup 后交替推进 forward 和 backward；它减少空闲但需要保存恰当 activation 并正确进行 P2P。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 1F1B 在 pipeline warmup 后交替推进 forward 和 backward；它减少空闲但需要保存恰当 activation 并正确进行 P2P。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 58. 请用小白能懂的话解释「梯度同步」的作用。

**参考答案：** 梯度同步把不同 data-parallel rank 的贡献合并或分片；同步时机错误会让 rank 使用不同参数继续训练。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。梯度同步把不同 data-parallel rank 的贡献合并或分片；同步时机错误会让 rank 使用不同参数继续训练。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 梯度同步把不同 data-parallel rank 的贡献合并或分片；同步时机错误会让 rank 使用不同参数继续训练。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 59. 如果「参数更新」出错，初学者最容易观察到什么现象？

**参考答案：** 参数更新是 optimizer 将 gradient 写回 parameter 的提交点；更新前后的梯度清理和 step 计数必须与 accumulation 对齐。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。参数更新是 optimizer 将 gradient 写回 parameter 的提交点；更新前后的梯度清理和 step 计数必须与 accumulation 对齐。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 参数更新是 optimizer 将 gradient 写回 parameter 的提交点；更新前后的梯度清理和 step 计数必须与 accumulation 对齐。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 60. 「日志」和相邻模块之间通常传递什么数据？

**参考答案：** 日志是观测训练状态的控制面数据；loss、throughput 和 memory 若统计窗口不同，不能直接比较。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。日志是观测训练状态的控制面数据；loss、throughput 和 memory 若统计窗口不同，不能直接比较。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 日志是观测训练状态的控制面数据；loss、throughput 和 memory 若统计窗口不同，不能直接比较。 本题属于“训练循环”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## G. 检查点（61-70）

### 61. 什么是「state_dict」，它在 Megatron-LM 中解决什么问题？

**参考答案：** state_dict 是模块参数/缓冲区的键值状态表示；分布式模型中它可能只是逻辑视图，不等于单个文件。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。state_dict 是模块参数/缓冲区的键值状态表示；分布式模型中它可能只是逻辑视图，不等于单个文件。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** state_dict 是模块参数/缓冲区的键值状态表示；分布式模型中它可能只是逻辑视图，不等于单个文件。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 62. Megatron-LM 里为什么需要理解「sharded state dict」？

**参考答案：** sharded state dict 让每个 rank 表示自己拥有的参数分片及元数据，避免聚合完整模型造成内存峰值。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。sharded state dict 让每个 rank 表示自己拥有的参数分片及元数据，避免聚合完整模型造成内存峰值。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** sharded state dict 让每个 rank 表示自己拥有的参数分片及元数据，避免聚合完整模型造成内存峰值。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 63. 请用小白能懂的话解释「dist_checkpointing.save」的作用。

**参考答案：** 分布式 save 按 shard 和 metadata 写 checkpoint；成功返回前要定义文件完整性、并发写入和失败重试语义。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。分布式 save 按 shard 和 metadata 写 checkpoint；成功返回前要定义文件完整性、并发写入和失败重试语义。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 分布式 save 按 shard 和 metadata 写 checkpoint；成功返回前要定义文件完整性、并发写入和失败重试语义。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 64. 如果「dist_checkpointing.load」出错，初学者最容易观察到什么现象？

**参考答案：** load 根据 checkpoint metadata 将文件 shard 映射回当前模型/optimizer；拓扑变化时可能需要 reshard，而不是简单按文件名读取。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。load 根据 checkpoint metadata 将文件 shard 映射回当前模型/optimizer；拓扑变化时可能需要 reshard，而不是简单按文件名读取。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** load 根据 checkpoint metadata 将文件 shard 映射回当前模型/optimizer；拓扑变化时可能需要 reshard，而不是简单按文件名读取。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 65. 「checkpoint directory」和相邻模块之间通常传递什么数据？

**参考答案：** 目录不仅存权重，还可能存 optimizer、RNG、迭代计数和 metadata；恢复必须验证这些状态是否来自同一次提交。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。目录不仅存权重，还可能存 optimizer、RNG、迭代计数和 metadata；恢复必须验证这些状态是否来自同一次提交。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 目录不仅存权重，还可能存 optimizer、RNG、迭代计数和 metadata；恢复必须验证这些状态是否来自同一次提交。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 66. 什么是「resume」，它在 Megatron-LM 中解决什么问题？

**参考答案：** resume 的目标是继续同一训练轨迹，因此数据位置、随机数、optimizer state、参数和 iteration 都要一致或有明确差异。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。resume 的目标是继续同一训练轨迹，因此数据位置、随机数、optimizer state、参数和 iteration 都要一致或有明确差异。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** resume 的目标是继续同一训练轨迹，因此数据位置、随机数、optimizer state、参数和 iteration 都要一致或有明确差异。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 67. Megatron-LM 里为什么需要理解「iteration state」？

**参考答案：** iteration state 决定学习率、保存/评估频率和数据进度；丢失它可能让恢复作业看似运行但实际重复或跳过数据。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。iteration state 决定学习率、保存/评估频率和数据进度；丢失它可能让恢复作业看似运行但实际重复或跳过数据。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** iteration state 决定学习率、保存/评估频率和数据进度；丢失它可能让恢复作业看似运行但实际重复或跳过数据。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 68. 请用小白能懂的话解释「optimizer state」的作用。

**参考答案：** optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** optimizer state 包含动量等历史；只加载权重会改变下一步更新，不能宣称与原训练完全等价。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 69. 如果「随机数状态」出错，初学者最容易观察到什么现象？

**参考答案：** 随机数影响 dropout、数据顺序和采样；多 rank 恢复时要保存/恢复对应状态，且 launcher 变化可能改变序列。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。随机数影响 dropout、数据顺序和采样；多 rank 恢复时要保存/恢复对应状态，且 launcher 变化可能改变序列。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 随机数影响 dropout、数据顺序和采样；多 rank 恢复时要保存/恢复对应状态，且 launcher 变化可能改变序列。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 70. 「版本兼容」和相邻模块之间通常传递什么数据？

**参考答案：** checkpoint 兼容不仅是 key 名字，还包括 shape、分片布局、dtype、optimizer schema 和配置；迁移需显式测试。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。checkpoint 兼容不仅是 key 名字，还包括 shape、分片布局、dtype、optimizer schema 和配置；迁移需显式测试。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** checkpoint 兼容不仅是 key 名字，还包括 shape、分片布局、dtype、optimizer schema 和配置；迁移需显式测试。 本题属于“检查点”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## H. 精度与后端（71-80）

### 71. 什么是「FP32」，它在 Megatron-LM 中解决什么问题？

**参考答案：** FP32 动态范围和精度较高但占用更多显存/带宽；常用于参考路径或敏感累积，不代表所有模块都用它。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。FP32 动态范围和精度较高但占用更多显存/带宽；常用于参考路径或敏感累积，不代表所有模块都用它。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** FP32 动态范围和精度较高但占用更多显存/带宽；常用于参考路径或敏感累积，不代表所有模块都用它。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 72. Megatron-LM 里为什么需要理解「FP16」？

**参考答案：** FP16 节省存储并加速 tensor core，但动态范围较小，梯度 overflow/underflow 需要 loss scaling 或其他保护。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。FP16 节省存储并加速 tensor core，但动态范围较小，梯度 overflow/underflow 需要 loss scaling 或其他保护。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** FP16 节省存储并加速 tensor core，但动态范围较小，梯度 overflow/underflow 需要 loss scaling 或其他保护。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 73. 请用小白能懂的话解释「BF16」的作用。

**参考答案：** BF16 保留较大指数范围、尾数较短；通常比 FP16 更能容忍大范围值，但精度误差仍需验证。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。BF16 保留较大指数范围、尾数较短；通常比 FP16 更能容忍大范围值，但精度误差仍需验证。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** BF16 保留较大指数范围、尾数较短；通常比 FP16 更能容忍大范围值，但精度误差仍需验证。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 74. 如果「FP8」出错，初学者最容易观察到什么现象？

**参考答案：** FP8 用更低位宽换吞吐和显存，依赖 scale/amax recipe；scale 统计、通信和算子支持共同决定数值结果。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。FP8 用更低位宽换吞吐和显存，依赖 scale/amax recipe；scale 统计、通信和算子支持共同决定数值结果。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** FP8 用更低位宽换吞吐和显存，依赖 scale/amax recipe；scale 统计、通信和算子支持共同决定数值结果。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 75. 「FP4」和相邻模块之间通常传递什么数据？

**参考答案：** FP4 进一步压缩权重或激活，误差和 kernel/layout 约束更强；不能只看 dtype 名称判断可用性。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。FP4 进一步压缩权重或激活，误差和 kernel/layout 约束更强；不能只看 dtype 名称判断可用性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** FP4 进一步压缩权重或激活，误差和 kernel/layout 约束更强；不能只看 dtype 名称判断可用性。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 76. 什么是「mixed precision」，它在 Megatron-LM 中解决什么问题？

**参考答案：** mixed precision 不是把所有 tensor 随意改 dtype，而是为不同算子选择精度并维护累积/缩放规则。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。mixed precision 不是把所有 tensor 随意改 dtype，而是为不同算子选择精度并维护累积/缩放规则。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** mixed precision 不是把所有 tensor 随意改 dtype，而是为不同算子选择精度并维护累积/缩放规则。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 77. Megatron-LM 里为什么需要理解「loss scaling」？

**参考答案：** loss scaling 放大 loss 以避免低精度梯度下溢，再在更新前还原；overflow 时需跳过或处理该 step。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。loss scaling 放大 loss 以避免低精度梯度下溢，再在更新前还原；overflow 时需跳过或处理该 step。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** loss scaling 放大 loss 以避免低精度梯度下溢，再在更新前还原；overflow 时需跳过或处理该 step。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 78. 请用小白能懂的话解释「TE fallback」的作用。

**参考答案：** fallback 让缺少可选 backend 时仍可用，但实现、dtype、性能和数值容差可能不同，必须分别测试。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。fallback 让缺少可选 backend 时仍可用，但实现、dtype、性能和数值容差可能不同，必须分别测试。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** fallback 让缺少可选 backend 时仍可用，但实现、dtype、性能和数值容差可能不同，必须分别测试。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 79. 如果「Apex fallback」出错，初学者最容易观察到什么现象？

**参考答案：** fallback 让缺少可选 backend 时仍可用，但实现、dtype、性能和数值容差可能不同，必须分别测试。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。fallback 让缺少可选 backend 时仍可用，但实现、dtype、性能和数值容差可能不同，必须分别测试。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** fallback 让缺少可选 backend 时仍可用，但实现、dtype、性能和数值容差可能不同，必须分别测试。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 80. 「Torch fallback」和相邻模块之间通常传递什么数据？

**参考答案：** fallback 让缺少可选 backend 时仍可用，但实现、dtype、性能和数值容差可能不同，必须分别测试。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。fallback 让缺少可选 backend 时仍可用，但实现、dtype、性能和数值容差可能不同，必须分别测试。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** fallback 让缺少可选 backend 时仍可用，但实现、dtype、性能和数值容差可能不同，必须分别测试。 本题属于“精度与后端”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## I. 测试与运行（81-90）

### 81. 什么是「uv sync」，它在 Megatron-LM 中解决什么问题？

**参考答案：** 「uv sync」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「uv sync」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「uv sync」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 82. Megatron-LM 里为什么需要理解「editable install」？

**参考答案：** 「editable install」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「editable install」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「editable install」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 83. 请用小白能懂的话解释「torchrun」的作用。

**参考答案：** 「torchrun」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「torchrun」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「torchrun」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 84. 如果「unit tests」出错，初学者最容易观察到什么现象？

**参考答案：** 「unit tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「unit tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「unit tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 85. 「functional tests」和相邻模块之间通常传递什么数据？

**参考答案：** 「functional tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「functional tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「functional tests」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 86. 什么是「autoformat」，它在 Megatron-LM 中解决什么问题？

**参考答案：** 「autoformat」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「autoformat」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「autoformat」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 87. Megatron-LM 里为什么需要理解「CI 容器」？

**参考答案：** 「CI 容器」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「CI 容器」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「CI 容器」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 88. 请用小白能懂的话解释「GPU 依赖」的作用。

**参考答案：** 「GPU 依赖」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「GPU 依赖」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「GPU 依赖」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 89. 如果「未验证命令」出错，初学者最容易观察到什么现象？

**参考答案：** 「未验证命令」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「未验证命令」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「未验证命令」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 90. 「golden value」和相邻模块之间通常传递什么数据？

**参考答案：** golden value 是可重复的参考输出；测试应同时固定输入、seed、dtype 和允许误差，避免把非确定性当 bug。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。golden value 是可重复的参考输出；测试应同时固定输入、seed、dtype 和允许误差，避免把非确定性当 bug。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** golden value 是可重复的参考输出；测试应同时固定输入、seed、dtype 和允许误差，避免把非确定性当 bug。 本题属于“测试与运行”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
## J. 常见排错（91-100）

### 91. 什么是「CUDA 不可用」，它在 Megatron-LM 中解决什么问题？

**参考答案：** CUDA 是 GPU 执行和内存的运行时基础；Python 层调用成功不代表 kernel 已按预期执行，异步错误常需同步后才暴露。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。CUDA 是 GPU 执行和内存的运行时基础；Python 层调用成功不代表 kernel 已按预期执行，异步错误常需同步后才暴露。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** CUDA 是 GPU 执行和内存的运行时基础；Python 层调用成功不代表 kernel 已按预期执行，异步错误常需同步后才暴露。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 92. Megatron-LM 里为什么需要理解「NCCL 初始化失败」？

**参考答案：** NCCL 为 GPU rank 提供 collective 和点对点通信；所有参与 rank 必须以一致顺序调用兼容 shape 的通信操作。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。NCCL 为 GPU rank 提供 collective 和点对点通信；所有参与 rank 必须以一致顺序调用兼容 shape 的通信操作。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** NCCL 为 GPU rank 提供 collective 和点对点通信；所有参与 rank 必须以一致顺序调用兼容 shape 的通信操作。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 93. 请用小白能懂的话解释「rank 配置错误」的作用。

**参考答案：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** rank 是分布式 world 中进程的逻辑编号；它不是天然的物理 GPU 编号，必须结合 local rank 和可见设备解释。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 94. 如果「显存不足」出错，初学者最容易观察到什么现象？

**参考答案：** 「显存不足」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「显存不足」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「显存不足」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 95. 「shape mismatch」和相邻模块之间通常传递什么数据？

**参考答案：** 「shape mismatch」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「shape mismatch」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「shape mismatch」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 96. 什么是「loss NaN」，它在 Megatron-LM 中解决什么问题？

**参考答案：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** loss 是衡量预测与标签差异的标量；loss mask、平均方式和数据并行归约方式会影响梯度大小以及日志可比性。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 97. Megatron-LM 里为什么需要理解「checkpoint 加载失败」？

**参考答案：** 「checkpoint 加载失败」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「checkpoint 加载失败」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「checkpoint 加载失败」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 98. 请用小白能懂的话解释「数据路径错误」的作用。

**参考答案：** 「数据路径错误」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「数据路径错误」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「数据路径错误」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 99. 如果「性能不稳定」出错，初学者最容易观察到什么现象？

**参考答案：** 「性能不稳定」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「性能不稳定」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「性能不稳定」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

**拓展分析：** 可继续追问：如果把规模从单卡扩展到 TP/PP/DP/CP/EP，哪条契约变化最大？如何用最小 mock 数据、固定 seed、单 rank 日志和全局 trace 验证？性能上至少记录 step time、tokens/sec、通信占比、峰值显存和 checkpoint I/O；不要用未执行的历史命令或单次日志宣称性能结论。
### 100. 「文档证据边界」和相邻模块之间通常传递什么数据？

**参考答案：** 「文档证据边界」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 这道题的核心结论是：先确认配置/数据/模型/并行/状态属于哪一层，再说明它如何被下游消费；不能把“代码调用成功”直接等同于“分布式训练正确”。

**小白解释：** 可以把 Megatron-LM 想成许多 GPU 合作训练一本很大的书。「文档证据边界」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 对应的是其中一个工位：工位既要做好自己的工作，也要按统一的编号、节拍和交接单与其他 GPU 配合。

**技术分析：** 「文档证据边界」需要结合上下游接口理解：它会改变输入、状态、资源或结果中的至少一项，不能只根据名称推断行为。 本题属于“常见排错”主题。在 Megatron 的训练主线中，相关对象通常沿入口、初始化、Core 模型、pipeline schedule、optimizer 或 checkpoint 边界传递。 重点检查三个层次：第一是对象和 tensor 的 shape/stride 或 shard；第二是 rank/group、microbatch、梯度或文件状态的生命周期；第三是失败时是否会留下未同步参数、错误 activation、重复数据或不完整 checkpoint。先回答“它是什么、为什么需要、数据往哪里走”，再补充一个简单例子。 现有知识库以 `pretrain_gpt.py`、`initialize_megatron`、`parallel_state`、`gpt_builder`、pipeline schedule、D01 和 `dist_checkpointing` 为主要证据；GPU/NCCL 动态结果仍需在目标环境验证。

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
