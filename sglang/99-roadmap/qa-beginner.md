# 入门级 QA：SGLang 与大模型推理

- 题数：100
- 适用对象：第一次接触 SGLang、Transformer 推理和 GPU 服务系统的读者
- 使用方式：先遮住“参考答案”自测；回答不仅要说名词，还要说明对象、阶段和数据如何变化。
- 证据范围：本题集以当前知识库和 checkout `f1a512c51c73ab660cf41e1af3110c7c11e3b600` 为背景。涉及实际 GPU、模型或多卡运行的题目，源码理解不等于运行验证。

## A. 大模型推理基础（1-25）

### 1. 什么是大语言模型推理？

**参考答案：** 给定输入 token，模型执行前向计算得到 logits，再根据采样策略选择下一个 token，重复这一过程直到停止。推理服务还要负责请求接收、排队、显存管理、批处理、输出和清理。

### 2. 什么是 token？

**参考答案：** token 是 tokenizer 将文本切分后的离散编号，可能对应字符、子词或一段常见字符串。模型实际接收的是 token id，而不是原始字符串。

### 3. tokenizer 在推理链路中做什么？

**参考答案：** tokenizer 将文本转成 input ids，并在返回阶段把生成 token ids 转成文本。它还会校验输入长度、处理特殊 token 和流式增量文本。

### 4. 什么是 prompt？

**参考答案：** prompt 是请求提供给模型的上下文 token 序列，可以包含系统指令、用户问题、历史对话和多模态占位信息。

### 5. 什么是 prefill？

**参考答案：** prefill 是第一次处理输入上下文的阶段。模型通常一次计算多个输入 token，并把每层 attention 的 K/V 写入 KV cache。

### 6. 什么是 decode？

**参考答案：** decode 是已经有上下文 cache 后，每次为请求生成一个或少量新 token 的阶段。它通常受单步延迟和 KV cache 读写影响较大。

### 7. prefill 和 decode 的主要区别是什么？

**参考答案：** prefill 处理大量新输入 token，计算密集且可并行；decode 每个请求通常只处理一个新 token，但需要读取长上下文 KV，常更受内存带宽和 batch 调度影响。

### 8. 什么是 logits？

**参考答案：** logits 是模型对词表中每个 token 的未归一化分数。对 logits 做 softmax 可得到概率分布，但实际采样常先应用温度、惩罚和约束。

### 9. 什么是 softmax？

**参考答案：** softmax 将一组分数转换为和为 1 的非负概率。推理中通常不必显式保存完整概率，而是在 logits 上做变换后选择 token。

### 10. greedy decoding 是什么？

**参考答案：** 每一步选择 logits 最大的 token。它确定性较强，但不一定得到最有质量或最多样的文本。

### 11. temperature 做什么？

**参考答案：** temperature 调整 logits 的尖锐程度。较低温度更偏向高分 token，较高温度增加随机性；温度为零通常对应特殊的贪心路径。

### 12. top-k 和 top-p 有什么区别？

**参考答案：** top-k 只保留分数最高的固定 k 个 token；top-p 按概率从高到低累积，保留达到 p 的动态数量 token。二者都减少低概率候选。

### 13. 什么是 EOS？

**参考答案：** EOS 是模型定义的结束 token。生成到 EOS 后，请求通常可以标记完成并停止继续 decode。

### 14. 什么是 stop string？

**参考答案：** stop string 是用户指定的文本停止条件。它可能跨越多个 token，因此服务需要维护增量文本或足够的 token 窗口来判断。

### 15. 什么是最大新 token 数？

**参考答案：** 它限制一次请求最多生成多少 token，防止请求无限 decode，并参与 scheduler 的 KV 容量预算。

### 16. 什么是上下文长度？

**参考答案：** 上下文长度是模型能处理的输入与生成序列上限。它限制 KV cache 容量、位置编码范围和请求 admission。

### 17. 什么是 KV cache？

**参考答案：** Transformer attention 中已经计算出的 key/value 张量。后续 decode 复用它们，避免每一步重新计算全部历史 token。

### 18. 为什么 KV cache 会成为瓶颈？

**参考答案：** 每个请求、每层、每个 token 都需要存储 K/V，长上下文和大 batch 会快速消耗 GPU 显存；容量不足会限制并发，甚至触发 retraction 或 abort。

### 19. 什么是 batch？

**参考答案：** batch 是一次模型 forward 同时处理的一组请求或 token。推理 batch 不一定要求每个请求长度相同，通常需要额外的长度和索引 metadata。

### 20. 什么是 continuous batching？

**参考答案：** continuous batching 允许请求在不同时间加入或离开运行 batch，而不是等待整个静态 batch 完成，从而提高 GPU 利用率和服务吞吐。

### 21. 静态 batching 和 continuous batching 的差别是什么？

**参考答案：** 静态 batching 在开始时固定成员；continuous batching 每轮根据等待队列、运行请求和资源预算重新规划，能混合不同请求的 prefill/decode 生命周期。

### 22. 什么是吞吐和延迟？

**参考答案：** 吞吐表示单位时间处理的 token 或请求数量；延迟表示请求等待或完成所需时间。服务通常要在吞吐、首 token 延迟和单 token 延迟之间权衡。

### 23. TTFT 和 TPOT 通常分别表示什么？

**参考答案：** TTFT 是从请求进入到首个输出 token 的时间；TPOT 常指后续每个输出 token 的平均时间。prefill、排队和 decode 会分别影响它们。

### 24. 什么是流式输出？

**参考答案：** 服务每生成一部分 token 就返回增量文本，而不是等待完整结果。它降低用户感知延迟，但增加状态、断连和增量拼接的复杂度。

### 25. 为什么推理服务不能只调用一次模型 forward？

**参考答案：** 自回归生成需要根据上一步 token 再计算下一步，直到满足停止条件；服务还要在每一步维护 cache、batch、请求状态和输出。

## B. SGLang 总体结构（26-50）

### 26. SGLang 的 SRT 是什么？

**参考答案：** SRT 是 SGLang Runtime，负责请求管理、调度、模型执行、KV cache、采样、输出和设备后端等运行时职责。

### 27. SGLang 的普通请求主链是什么？

**参考答案：** 客户端/API 或 Engine → TokenizerManager → IPC → Scheduler → ScheduleBatch → worker/ModelRunner → result processor → 输出 IPC → ReqState/HTTP 或 Engine 返回。

### 28. `ServerArgs` 负责什么？

**参考答案：** 它承载服务启动参数和解析结果，例如模型路径、并行规模、调度和 backend 配置。解析后的配置会发布给不同 runtime 角色。

### 29. 为什么 raw CLI 参数不是最终运行时配置？

**参考答案：** 某些值需要根据模型、设备、并行拓扑或其他参数派生。SGLang 会先 resolution，再将角色所需的配置投影到各进程，避免热路径反复推导。

### 30. Engine 和 server API 有什么关系？

**参考答案：** Engine 提供离线或 Python 调用入口，HTTP/API 提供网络服务入口；它们最终可以汇入类似的 TokenizerManager、Scheduler 和模型执行主线。

### 31. TokenizerManager 的职责是什么？

**参考答案：** 它管理输入规范化、tokenization、请求本地状态、向 scheduler dispatch，以及接收输出后进行增量文本组织和返回。

### 32. `ReqState` 是什么？

**参考答案：** `ReqState` 是 tokenizer/API 进程中的本地请求状态，保存输出列表、完成状态、事件、原始请求、文本 chunks 和 abort/dispatched 信息。

### 33. scheduler-side `Req` 和 `ReqState` 有什么区别？

**参考答案：** `ReqState` 负责调用者一侧的异步等待和输出汇聚；`Req` 负责 scheduler 侧的 token、生成、KV、队列、finish 和批次状态。二者通过 `rid` 关联。

### 34. `rid` 为什么重要？

**参考答案：** `rid` 是跨输入、scheduler、输出和本地状态的关联键。批量输出必须用它把第 i 项结果匹配到正确的 `ReqState`。

### 35. Scheduler 的核心职责是什么？

**参考答案：** 接收请求、维护 waiting/running 状态、决定 admission、管理 prefill/decode 批次、协调 KV 资源、调用 worker、处理结果和应对资源不足。

### 36. `ScheduleBatch` 是什么？

**参考答案：** 它是 scheduler 为一次 forward 准备的工作对象，包含请求集合、KV pool/allocator、prefix cache、forward mode、序列长度和 device 输入 metadata。

### 37. `ForwardBatch` 是什么？

**参考答案：** 它是模型 worker 侧一次 forward 所需的输入和 metadata 快照，由 `ScheduleBatch` 构造，包含 input ids、cache locations、序列长度和 sampling 信息等。

### 38. `TpModelWorker` 做什么？

**参考答案：** 它为模型执行构造 `ForwardBatch`，调用 `ModelRunner`，处理 PP rank 差异、prefill-only、verify、sampling 和 `GenerationBatchResult` 组织。

### 39. `ModelRunner` 做什么？

**参考答案：** 它持有模型和执行 backend，负责 forward、attention metadata、eager/graph 选择、logits 处理和 sampling。

### 40. `GenerationBatchResult` 包含什么？

**参考答案：** 它可以包含 logits、next token ids、CUDA Graph 可运行信息、PP proxy tensors 以及专家或 indexer 等辅助结果。

### 41. 为什么 worker 不直接返回最终文本？

**参考答案：** worker 主要处理 device tensor 和 token ids；停止判断、请求状态、KV 生命周期、detokenization 和协议输出分散在 scheduler 与 tokenizer/API 层。

### 42. 什么是 ready 信号？

**参考答案：** ready 表示子进程已完成关键初始化并可以服务，不只是 OS 进程已经创建。SGLang 会在 scheduler/worker 初始化后通过 pipe 等机制通知父进程。

### 43. 为什么 process started 不等于 ready？

**参考答案：** 子进程启动后还要发布配置、初始化分布式通信、加载模型、建立 KV pool 和 backend；任一步失败都可能导致进程存在但 runtime 不可用。

### 44. SGLang 中常见的进程边界有哪些？

**参考答案：** 主进程/Engine、TokenizerManager、Scheduler、Detokenizer，以及 worker/device 执行上下文。具体部署模式可能改变边界。

### 45. IPC 在请求链路中做什么？

**参考答案：** IPC 将 tokenized request、控制请求和输出对象跨进程传输。它还承载启动 ready、abort、shutdown 等控制语义。

### 46. 为什么 IPC 字段变化危险？

**参考答案：** 字段要同时满足输入 normalize、序列化、scheduler 消费、结果输出和旧/不同进程边界的对齐要求；漏改一处可能导致静默错位或无法清理。

### 47. SGLang 的模块 ID M03/M04/M05 分别表示什么？

**参考答案：** M03 是 Tokenizer 与请求状态，M04 是 Scheduler 与连续批处理，M05 是模型执行。模块 ID 表示职责边界，不等于单个目录。

### 48. 读源码时为什么要区分控制流、数据流和生命周期？

**参考答案：** 函数调用顺序不等于对象所有权或资源释放顺序。请求可能跨进程，batch 可能被 overlap 延迟持有，KV tensor 也可能脱离 Python 对象继续存在。

### 49. 什么是控制面和执行面？

**参考答案：** 控制面负责配置、进程、调度、请求和状态；执行面负责 GPU tensor、模型层、attention、通信和 kernel。SGLang 在两者之间传递计划和结果。

### 50. 为什么源码分析要标记“已确认、推断、未知”？

**参考答案：** 静态阅读可以确认调用和字段，但不能自动证明 GPU 性能、实际模型输出或所有部署变体。标记证据等级可避免把推测当成运行事实。

## C. 调度、批处理与 KV（51-75）

### 51. Scheduler 每轮普通主循环大致做什么？

**参考答案：** 接收请求，计算下一批计划，更新 running batch；有 batch 时执行并处理结果，无 batch 时进行 idle 维护，再保存本轮 batch 状态。

### 52. waiting queue 和 running batch 分别表示什么？

**参考答案：** waiting queue 中的请求尚未进入当前执行计划；running batch 中的请求已经被 scheduler 接纳，正在经历 prefill、decode 或相关结果处理。

### 53. 什么是 admission？

**参考答案：** admission 是判断等待请求能否加入执行批次的过程，需要同时考虑 token budget、KV 容量、请求数、prefix 命中、优先级和 backend 约束。

### 54. `PrefillAdder` 的职责是什么？

**参考答案：** 它根据剩余 token/page budget、请求状态、prefix cache 和 allocator 能力，逐个判断请求能否加入 prefill，并处理 chunked prefill 等情况。

### 55. 为什么不能简单把 waiting list 全部拼成 batch？

**参考答案：** GPU 显存、KV slots、最大 token 数、attention tile、priority、chunking 和 running batch 都有约束，全部加入可能无法执行或破坏公平性。

### 56. 什么是 chunked prefill？

**参考答案：** 当完整输入无法在一轮预算内处理时，将一个请求的 prefill 拆成多个 chunk，跨多个调度轮次完成，同时保留其部分 KV 和请求状态。

### 57. chunked request 为什么需要复用 request row？

**参考答案：** 同一个请求跨 chunk 继续执行，需要保留原有 KV 映射和生命周期；重新分配 row 可能丢失或混淆已有 cache 位置。

### 58. `prepare_for_extend` 做什么？

**参考答案：** 将 batch 设为 extend 模式，取每个请求未命中 prefix 的 fill ids，计算 extend/prefix/sequence lengths，并通过 allocator 分配本轮输出 cache locations。

### 59. `prepare_for_decode` 做什么？

**参考答案：** 将 batch 设为 decode 模式，为每个请求分配一个或相应数量的下一 token cache 位置，并更新序列长度 metadata。

### 60. extend 和 decode 的 `input_ids` 有什么不同？

**参考答案：** extend 通常包含尚未写入 cache 的多个输入 token；decode 通常每个请求只推进一个新 token，具体 speculative 分支可能改变数量。

### 61. 什么是 prefix cache？

**参考答案：** 它缓存已经计算过的 token 前缀及其 KV indices，使后续具有相同前缀的请求可以跳过重复 prefill。

### 62. Radix Cache 为什么叫 radix？

**参考答案：** 它以 token 序列前缀构成树/基数结构，共享相同前缀节点，而不是为每个完整请求独立保存一份键值。

### 63. page alignment 为什么重要？

**参考答案：** KV cache 常按 page/block 管理；只有对齐的 token 区间适合作为可复用 cache 节点，尾部 partial page 需要特殊保护和释放逻辑。

### 64. `ReqToTokenPool` 保存什么映射？

**参考答案：** 它把 scheduler 请求 row 和逻辑 token 位置映射到实际 KV slot。模型 attention 通过这类 metadata 找到请求历史的 KV。

### 65. 为什么 request pool 的第 0 行是 padding row？

**参考答案：** CUDA Graph 或 padded batch 可能使用 0 作为 dummy request index；保留安全的零行可避免 dummy 读写污染真实请求。

### 66. `prefix_indices` 表示什么？

**参考答案：** 它表示请求已经命中或拥有的 prefix KV indices，scheduler 可据此跳过对应输入 token，并从未命中区间继续 extend。

### 67. `extend_range` 表示什么？

**参考答案：** 它描述本轮 prefill/extend 需要写入的请求序列区间，连接 token 输入长度、序列长度和 KV allocation。

### 68. 什么是 decode retraction？

**参考答案：** 当 decode 需要的 KV 容量不足时，scheduler 暂时移除部分请求，释放或备份其资源，稍后重排；无法安全恢复时对请求 abort。

### 69. retraction 为什么优于立即 OOM？

**参考答案：** 它把全局资源压力转换成请求级降级或延迟，尽量保留服务进程和其他请求继续运行。

### 70. 哪些请求可能不能被 retraction 恢复？

**参考答案：** 例如备份资源不足、beam group 约束或请求状态不允许安全释放时，scheduler 可能设置 abort finish reason，而不是重新排队。

### 71. 什么是 prefix cache lock/reference？

**参考答案：** 它防止仍被请求或 cache 使用的 radix node 被过早回收。请求完成、迁移或重新匹配时必须正确增减引用。

### 72. 完成请求缓存时为什么要释放 duplicate KV？

**参考答案：** radix cache 可能已经拥有部分 prefix；新请求 row 中重复或未插入的 KV 不再需要独占，必须释放以避免显存泄漏。

### 73. finished cache 和 unfinished cache 有什么区别？

**参考答案：** finished request 通常按最终输入加生成结果缓存；unfinished/chunked request 需要保留可继续执行的工作集并重新匹配 prefix，保护 partial page。

### 74. priority 会影响什么？

**参考答案：** priority 可以影响等待请求的 admission 顺序、抢占/回收选择或 cache 插入优先级，具体效果取决于调度和 cache policy 实现。

### 75. 调度器如何选择 prefill 还是 decode？

**参考答案：** 它综合 running batch、waiting queue、chunked 状态、token/KV 预算、prefill delay 和配置，在没有合适新 prefill 时通常推进已有 running 请求的 decode。

## D. 模型执行、并行与输出（76-100）

### 76. `ForwardBatch.init_new` 为什么不能随意修改 `ScheduleBatch`？

**参考答案：** `ScheduleBatch` 可能被 overlap 或 scheduler 继续保存和恢复；forward 对象应承载本轮视图，隐式原地修改会造成跨轮或跨 stream 生命周期错误。

### 77. `ForwardBatch` 中的 `out_cache_loc` 是什么？

**参考答案：** 它指定本轮 forward 产生的 K/V 应写入哪些 cache 位置，通常由 scheduler/KV allocator 预先决定。

### 78. `TpModelWorker` 的 TP 是什么？

**参考答案：** Tensor Parallel 将模型参数或计算张量沿维度切分到多个 GPU，并通过 collective 协作完成一个模型层或 forward。

### 79. PP 是什么？

**参考答案：** Pipeline Parallel 按模型层或阶段切分，不同 GPU/进程负责不同层区间，hidden states 在 stage 之间传递。

### 80. DP 和 TP 的直观区别是什么？

**参考答案：** TP 让多个设备共同计算同一个请求/模型副本的张量；DP 通常让不同副本或 rank 处理不同请求/数据，并可能需要负载同步。

### 81. PP last rank 为什么通常负责 sampling？

**参考答案：** 最后一个 pipeline stage 才拥有最终 logits；前面的 rank 只有中间 hidden/proxy tensors，不能独立选择最终 token。

### 82. PP 非 last rank 返回什么？

**参考答案：** 它通常返回给下一个 stage 使用的 proxy hidden states 或相关结果，而不是最终 `next_token_ids`。

### 83. `ModelRunner.forward` 的输出是什么？

**参考答案：** 它返回包含 logits output、graph eligibility 和可能的专家/indexer辅助结果的 runner output，供 worker 组织成 generation result。

### 84. 为什么 prefill-only 请求可能返回 dummy token ids？

**参考答案：** 这类请求只需要计算输入 logprob或其他 prefill 输出，不应启动正常 decode；为保持 result shape 和接口对齐，worker 可创建与 batch size 匹配的 dummy ids。

### 85. speculative decoding 是什么？

**参考答案：** 使用较快的 draft 模型/路径提出多个候选 token，再由 target 模型验证并接受部分候选，以减少目标模型逐 token执行的成本。

### 86. speculative verify 为什么可能跳过普通 sampling？

**参考答案：** verify 阶段的接受/拒绝逻辑由 speculative 算法控制，不应把它误当成普通 decode 的一次独立采样。

### 87. CUDA Graph 的收益是什么？

**参考答案：** 对形状和内存地址稳定的 GPU 工作，减少重复 kernel launch 和 Python 调度开销，可能降低延迟。

### 88. CUDA Graph 为什么不能覆盖所有 batch？

**参考答案：** 动态 batch、动态 token width、embedding override、metadata shape、backend 或 DP 条件可能不满足 capture/replay 要求，因此需要 eager fallback。

### 89. attention backend 做什么？

**参考答案：** 它准备 attention metadata 并调用适配硬件、模型结构和 KV layout 的 attention kernel/实现，分别服务 prefill、decode 或特殊模式。

### 90. grammar constrained decoding 是什么？

**参考答案：** 根据 JSON schema、regex 或其他 grammar 在每一步生成前计算合法 token mask，将非法 token 的 logits 屏蔽，再执行采样。

### 91. grammar mask 为什么属于运行时状态？

**参考答案：** 合法 token 集合随已经生成的 token 改变，mask 需要按请求、batch 行和 device 在每轮更新，不能只在请求开始时固定。

### 92. `SamplingBatchInfo.filter_batch` 为什么重要？

**参考答案：** 请求完成或被移除后，temperature、top-k/top-p、seed、penalty、grammar 和 custom processor 的数组都必须按同一行索引过滤，否则会把参数用于错误请求。

### 93. logits bias 和 grammar mask 的顺序为什么要明确？

**参考答案：** 不同变换的语义可能不同；SGLang 的 sampling info 明确组织 pre-grammar transforms、grammar mask 和 post-grammar bias，顺序变化可能改变结果。

### 94. `next_token_ids` 如何变成最终文本？

**参考答案：** scheduler result processor 将 token 写回 `Req` 并生成输出对象；TokenizerManager 根据 rid 找到 `ReqState`，增量 detokenize/拼接后返回文本。

### 95. 调用者断连时为什么要 abort？

**参考答案：** 调用者不再消费结果，但 scheduler 可能仍持有请求和 KV；发送 abort 可释放运行状态、cache 引用和相关资源。

### 96. 为什么完成时要删除 `rid_to_state`？

**参考答案：** 表示本地不再等待该请求，并释放状态引用；如果不删除，长期服务会积累内存和错误的重复关联。

### 97. overlap loop 的基本思想是什么？

**参考答案：** 当前 batch 的 GPU forward 与上一 batch 的 CPU result processing 交错执行，以隐藏部分 CPU 开销，但需要 snapshot、stream/event 和共享数据读写同步。

### 98. 为什么 `last_batch` 不一定是设备上当前唯一 batch？

**参考答案：** overlap 模式中 result queue、上一批、当前批可能处于不同生命周期阶段；Python 变量名不能简单等同于 GPU 实际执行状态。

### 99. 一个完整请求结束前至少要清理哪些东西？

**参考答案：** 本地 ReqState、scheduler-side Req、waiting/running 引用、KV row/slots、radix lock/reference、IPC 临时资源以及必要的输出/abort状态。

### 100. 初学者如何判断自己真正理解了 SGLang？

**参考答案：** 能从一个 prompt 追踪到 tokenized object、rid state、scheduler Req、admission、ScheduleBatch、ForwardBatch、ModelRunner/sample、result processor、BatchStrOutput、最终文本和 cleanup，并能说明每一步所在进程、设备和资源所有权。

## 自测标准

- 90-100：能独立讲清普通请求主线，并定位主要状态对象。
- 70-89：掌握主线，但需要回看 KV、IPC、并行或输出清理。
- 50-69：具备术语基础，建议按 M03→M04→M08→M05 顺序重读。
- 0-49：先阅读 `01-concepts/` 和总览，再重新作答。
