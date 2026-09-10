# 中级 QA：从请求链到运行时实现

- 题数：50
- 适用对象：已经掌握入门概念，能够阅读 Python 调度器、batch 和 IPC 代码的读者。
- 作答要求：除了给出结论，还要指出状态所在进程、数据结构、资源所有权和失败后的清理动作。
- 证据范围：以 checkout `f1a512c51c73ab660cf41e1af3110c7c11e3b600` 为准；未执行的 GPU/模型/多卡行为不能写成运行事实。

## A. 请求状态、IPC 与配置（1-10）

### 1. `GenerateReqInput` 如何变成 `TokenizedGenerateReqInput`？

**参考答案：** 入口先规范化字符串、token ids、sampling 参数、stream 和扩展字段，然后 TokenizerManager 负责 tokenization 与长度/特殊 token 校验，生成适合跨进程传输的 tokenized 对象。原始请求面向 API，tokenized 对象面向 scheduler；二者不能混为同一所有权层。

**考查点：** 输入 schema、tokenization 边界、IPC 数据契约。

### 2. `_init_req_state` 为什么早于 dispatch？

**参考答案：** 本地状态必须先存在，才能在异步 dispatch、批量输出、取消或异常返回时用 `rid` 找到调用者的 event、输出列表和清理信息。若先发送后建 state，scheduler 的快速响应可能找不到接收方。

**考查点：** 事件时序和 `rid_to_state` 生命周期。

### 3. dispatch 前失败和 dispatch 后失败的清理有何不同？

**参考答案：** dispatch 前 scheduler 尚未拥有请求，通常删除本地 `ReqState` 并返回错误即可；dispatch 后 scheduler 可能已经分配 row、进入 waiting 或持有 cache，必须发送 abort，再清理本地 state，避免孤儿请求。

**考查点：** 跨进程所有权转移。

### 4. 批量输出为什么不能按 Python 列表位置直接交给调用者？

**参考答案：** batch 中的请求会动态加入、完成、过滤或重排。输出必须使用结果携带的 `rids[i]` 查 `rid_to_state`，再把第 i 项 token/text/logprob 送给对应请求；位置只在当前 batch 快照内有意义。

**考查点：** batch 行索引与逻辑请求 ID 的区别。

### 5. 流式 detokenization 为什么需要 `last_output_offset` 或 chunks？

**参考答案：** token 到文本不是一一对应，UTF-8、子词边界和特殊 token 可能使当前增量文本需要等待后续 token。offset/chunks 记录已经发送的文本边界，避免重复发送或切断字符。

**考查点：** token/text 两种序列的生命周期。

### 6. ZMQ 和 startup `Pipe` 为什么承担不同任务？

**参考答案：** startup pipe 适合父子进程一次性的 ready/初始化故障信号；ZMQ 适合稳态下持续的请求、结果、abort 和控制消息。二者的连接时序、阻塞语义和故障处理不能互换假设。

**考查点：** 控制面与数据面的 IPC 分工。

### 7. `ServerArgs.resolve_once` 的“once”解决什么问题？

**参考答案：** 它在模型、设备、并行规模等输入固定后计算派生值，并缓存 resolution 结果，保证不同角色看到一致配置，避免 scheduler/worker 在热路径重复推导或出现分歧。

**考查点：** raw config、derived config 和配置一致性。

### 8. 为什么配置应投影成 role-specific runtime bags？

**参考答案：** tokenizer、scheduler 和 worker 关心的字段不同；投影可以缩小接口、明确所有权和序列化边界，并把不应在某角色出现的设备/模型细节隔离。它不是简单复制整个 CLI Namespace。

**考查点：** 配置设计和跨进程边界。

### 9. `ReqState.finished` 与 scheduler-side finish reason 分别解决什么问题？

**参考答案：** scheduler-side finish reason 描述模型运行和资源状态，如 EOS、length、abort 或 retraction failure；`ReqState.finished` 表示 API 侧已经收到足够的终态并唤醒调用者。前者发生后，后者仍需要经过输出 IPC 和清理。

**考查点：** 两个进程中的终态不是同一个布尔值。

### 10. 为什么 process alive 不能作为 readiness 检查？

**参考答案：** scheduler 进程活着并不代表 distributed group、模型权重、KV pool、attention backend 和 CUDA Graph 已初始化。父进程应等待明确 ready 信号，并处理初始化异常或超时。

**考查点：** 启动状态机。

## B. Scheduler、admission 与 batch（11-25）

### 11. `handle_generate_request` 构造 scheduler-side `Req` 时为什么要复制很多请求字段？

**参考答案：** scheduler 必须在不依赖 API 对象的情况下完成 token 推进、sampling、stream、logprob、LoRA、session、priority、routing 和 cache key 等决策。复制的是跨边界所需的值/引用协议，不代表两个对象共享全部生命周期。

**考查点：** API request 与运行时 Req 的职责分离。

### 12. `waiting_queue`、`running_batch` 和 `last_batch` 的时间含义分别是什么？

**参考答案：** waiting queue 是尚未被当前计划接纳的请求；running batch 是 scheduler 当前认为处于运行生命周期的请求集合；`last_batch` 是最近一次交给 worker 或 overlap 结果处理所对应的 batch 快照。overlap 下它们可能同时代表不同时间点。

**考查点：** 控制状态与设备执行状态不等价。

### 13. `PrefillAdder` 为什么同时检查 token budget 和 page/KV budget？

**参考答案：** token budget 限制本轮算力和输入规模，page/KV budget 限制本轮要写入的 cache 存储；一个请求可能算力预算足够但 KV slot 不够，也可能有 slot 却超过 attention/backend 的 token 上限。

**考查点：** admission 的多资源约束。

### 14. prefix hit 如何改变 prefill 的输入？

**参考答案：** prefix match 返回已存在的 KV indices，保存到 `prefix_indices`；`prepare_for_extend` 从 `get_fill_ids()` 中跳过命中部分，只把未命中区间作为本轮 input，并按完整序列长度建立 metadata。

**考查点：** token 输入长度、prefix 长度和序列长度的区别。

### 15. `extend_range` 为什么不能简单用 `len(input_ids)` 表示？

**参考答案：** 本轮输入可能因 prefix hit、chunking 或已有 cache 只覆盖序列的一个区间；`extend_range` 描述逻辑序列中需要写入的起止位置，必须连接旧 cache、当前 token 和新 cache location。

**考查点：** 逻辑位置与本轮局部输入的映射。

### 16. chunked prefill 的下一轮需要保留哪些信息？

**参考答案：** 至少要保留请求 row、已分配 KV 映射、当前 fill/extend 进度、prefix 命中信息、序列长度、sampling/session 元数据和继续调度所需的状态。只保留剩余文本而丢掉 row 会破坏已有 KV 对齐。

**考查点：** 跨调度轮的请求生命周期。

### 17. `prepare_for_extend` 和 `prepare_for_decode` 的核心分叉是什么？

**参考答案：** extend 处理一个或多个尚未写入 cache 的输入 token，并计算 prefix/extend lengths；decode 通常为每个请求推进一个 token、分配下一位置并把 seq lens 增加一。speculative 分支可使 decode 的 token 数变化，但语义仍是推进已有序列。

**考查点：** forward mode 与 allocator 行为。

### 18. 为什么 admission 需要考虑 running batch，而不只看 waiting queue？

**参考答案：** 当前运行请求已经占用 KV 和 batch 行，还可能决定 decode 优先级、fairness、DP padding、PP microbatch 和本轮可用 token budget。忽略它会过度接纳新 prefill 或饿死 decode。

**考查点：** continuous batching 的跨阶段调度。

### 19. decode retraction 的一般步骤是什么？

**参考答案：** 先发现 decode 所需容量不足，再依据策略选择可回收请求；释放或备份可撤回请求的 KV/row 状态，将它们放回可继续调度的位置；不可安全恢复的请求设置 abort；重新检查 capacity 后再运行 batch。

**考查点：** 降级路径不是简单删除请求。

### 20. host backup 在 retraction 中解决什么问题？

**参考答案：** 它把暂时从 GPU KV pool 移出的请求所需的 cache 或恢复信息保存在 host 侧，使请求稍后能重新挂回逻辑序列。备份容量或状态不满足时，不能假设请求可恢复。

**考查点：** GPU 资源与恢复资源的双重所有权。

### 21. 为什么 retraction 可能导致 abort 而不是 requeue？

**参考答案：** beam group、备份不足、请求状态不一致、外部取消或 cache ownership 无法安全恢复时，继续 requeue 会造成错误结果或泄漏。scheduler 应发送明确 finish reason 并执行终态清理。

**考查点：** 可恢复性判定。

### 22. `ScheduleBatch.init_new` 中 `is_prefill_only` 为什么取所有请求的合取？

**参考答案：** batch 级 worker 行为只有在所有请求都不需要 decode 时才可走 prefill-only 语义；混合 batch 仍必须保留正常生成路径，否则部分请求会拿不到合法 next token。

**考查点：** request 属性如何提升为 batch 属性。

### 23. scheduler 为什么把 `ScheduleBatch` 和 `ForwardBatch` 分开？

**参考答案：** `ScheduleBatch` 是调度和资源所有权对象，可能跨轮保存；`ForwardBatch` 是本次设备执行的不可变或快照式输入，适合构造 tensor、attention metadata 和 sampling view。分开可减少 overlap 下的别名污染。

**考查点：** 计划对象与执行快照的生命周期。

### 24. batch 中请求完成后，为什么不能只从 `reqs` 列表删除？

**参考答案：** 还要同步 token pool row、KV slots、radix lock/ref、sampling batch arrays、输出索引、running/waiting 状态和本地 rid state。只删列表会留下资源或错误行对齐。

**考查点：** 终态清理的不变量。

### 25. 如何区分“调度饥饿”和“模型执行慢”？

**参考答案：** 看请求在 waiting 的停留时间、admission 原因和 batch 计划；若长期没有被接纳，是预算、优先级或 retraction 策略问题；若已进入 batch 但每轮 forward/通信耗时高，则应检查模型、attention、采样、graph fallback 和设备利用率。

**考查点：** 从状态时间线定位性能瓶颈。

## C. KV、执行、采样与并行（26-40）

### 26. `ReqToTokenPool` 的 row 与 token-to-KV allocator 的 slot 有什么区别？

**参考答案：** row 是每个 scheduler 请求的映射表索引；row 的每个逻辑 token 位置存一个实际 KV slot。allocator 管理 slot 的物理可用性，pool 管理“哪个请求的哪个位置指向哪个 slot”。

**考查点：** 两级映射和释放顺序。

### 27. request pool 的 row 0 为什么不应分配给真实请求？

**参考答案：** row 0 是 padding/dummy 行，通常全零并作为 CUDA Graph 或 padded batch 的安全默认索引。将它给真实请求会使真实 token 映射与 dummy 读写混淆。

**考查点：** sentinel 资源的安全语义。

### 28. `cache_finished_req` 为什么需要 page alignment？

**参考答案：** radix cache 的可复用节点通常按 page 边界管理；未对齐尾部可能不能独立成为稳定 cache 节点。插入对齐前缀后，要释放重复、未插入和尾部不受保护的 KV，避免 double ownership。

**考查点：** cache key、values 和物理释放的一致性。

### 29. cache lock/reference 的错误会产生什么现象？

**参考答案：** 少释放会造成 KV 和 radix node 泄漏、容量逐步下降；过早释放会让仍在使用的节点被 eviction，产生错误 attention indices 或结果损坏；因此请求、迁移、匹配和完成路径必须成对更新。

**考查点：** 引用计数与显存生命周期。

### 30. `ForwardBatch.init_new` 为什么强调不 mutate 输入？

**参考答案：** scheduler 可能保留同一个 `ScheduleBatch` 供 overlap result processing、下一轮状态更新或调试；原地改写 input ids、seq lens 或 mode 会让后续代码看到不一致快照。显式 keyword override 使别名和所有权可审计。

**考查点：** snapshot 设计和 WAR 风险。

### 31. PP last rank 和 non-last rank 的 result contract 有什么差别？

**参考答案：** last rank 拥有最终 hidden/logits，通常负责 logits processing 和 sampling，能产出 next token；non-last rank 只提供传给下游 stage 的 hidden/proxy tensors，不能把中间结果当作最终文本或 token。

**考查点：** pipeline stage 的输出契约。

### 32. prefill-only 为什么不能走普通 decode cleanup？

**参考答案：** prefill-only 可能只请求 logprob、embedding 或输入相关结果，不应因为 dummy token 触发生成长度递增、EOS 判断或下一轮 decode。结果处理必须根据 mode 区分 token 推进和输入完成。

**考查点：** mode-specific state transition。

### 33. `SamplingBatchInfo` 为什么是 batch 级而不是每个请求临时创建？

**参考答案：** GPU sampler 需要把多个请求的 temperature、top-k/top-p、seed、penalty、grammar 和 logits processor 打包成对齐的 tensor/数组。batch 级对象减少重复构造，但每次过滤和重排都必须同步所有字段。

**考查点：** CPU metadata 与 GPU 行对齐。

### 34. grammar mask 更新时最容易出现哪类 bug？

**参考答案：** 请求完成或 batch 重排后，grammar 对象仍按旧行索引；或者 mask 更新使用了旧 token 状态；还可能在 overlap 延迟 sampling 时闭包持有旧 GPU tensor。结果可能是错误拒绝/允许 token、显存增长或崩溃。

**考查点：** 约束状态、索引和延迟执行。

### 35. logits penalty、grammar mask、sampling 的顺序为什么是接口契约？

**参考答案：** penalty/bias 改变分数，grammar mask 将非法项置为不可选，sampler 再进行温度、截断和随机选择。改变顺序可能使非法 token 恢复、使 penalty 作用于错误集合或改变概率语义，因此不能只看最终函数名。

**考查点：** 数值变换的组合语义。

### 36. CUDA Graph eligibility 检查应放在哪里理解？

**参考答案：** scheduler/worker 先构造本轮 mode、batch size、token width 和 metadata，ModelRunner/graph runner 再判断 capture buffer、shape、backend、DP/MLP sync、speculative width 等条件。不能仅以“decode”这个标签推断一定能 replay。

**考查点：** graph 条件是多维契约。

### 37. attention metadata 为什么要区分 out-of-graph 和 in-graph 初始化？

**参考答案：** graph 外可以根据本轮动态请求构造 CPU/GPU metadata；graph 内必须写入已捕获的稳定 buffer/address，不能临时分配或执行不允许的同步操作。两条路径共享语义但不共享任意实现细节。

**考查点：** capture/replay 的内存地址和动态 shape限制。

### 38. TP、PP 和 DP 的 group 不能只用 global rank 相等来推断，为什么？

**参考答案：** global rank 是分布式进程编号，GPU id 受节点、base GPU、step 和设备可见性影响；TP/PP/DP group 是按拓扑和并行配置构造的子集合。必须读取 group 初始化和 placement，而不是把 rank 当物理设备。

**考查点：** logical rank 与 physical placement。

### 39. `world_size = tp_size * pp_size` 什么时候不够？

**参考答案：** 引入 DP、EP、attention CP、DCP、MoE 专用组或多节点副本后，world size 还要反映额外并行维度和 group 组织；即使基础模型组满足乘积，也不能据此推断所有 collective 的参与者。

**考查点：** 多维并行拓扑。

### 40. 为什么 group 创建顺序可能导致 deadlock？

**参考答案：** 分布式初始化和 collective 要求参与者以一致顺序进入通信。不同 rank 若按不同条件创建 group 或调用 barrier，某些 rank 会等待永远不会到达的参与者。审查时要看所有 rank 的控制流和初始化同步，而不是只看一个 rank。

**考查点：** 分布式控制流一致性。

## D. 加载、故障与测试（41-50）

### 41. checkpoint 分片和 TP 分片有什么区别？

**参考答案：** checkpoint 分片是文件/存储组织，可能按参数、层或 safetensors 文件拆开；TP 分片是运行时按计算维度把权重切到 rank。loader 需要将文件中的参数映射到目标模型参数，并根据 `weight_loader` 执行必要的切分、拼接或复制。

**考查点：** 存储布局与计算布局的区别。

### 42. `AutoWeightsLoader` 或参数 dispatch 为什么需要模型特定逻辑？

**参考答案：** 不同模型的 fused QKV、gate/up 投影、MoE expert、量化参数和 checkpoint 命名不同；通用 loader 发现参数后，还要交给参数对象的 loader/dispatch 处理目标 shape 和 rank 分工。

**考查点：** 通用加载框架与模型适配层。

### 43. 模型加载成功后为什么仍不能立刻对外服务？

**参考答案：** 还可能未完成 KV pool、attention backend、CUDA Graph runner、采样/grammar组件、通信预热和 ready 通知。模型对象存在只是初始化中间状态，不是服务可用状态。

**考查点：** 启动阶段的依赖闭包。

### 44. 用户请求超时后，应该沿哪条链排查？

**参考答案：** 先看 API 是否建立 `ReqState`，再看是否 dispatch；scheduler 是否 ingest/admit；是否卡在 KV allocation/retraction；worker 是否收到 batch；forward/collective 是否返回；result 是否通过 IPC；最后检查 detokenization、finish 和 event。不要只盯 HTTP access log。

**考查点：** 端到端 trace。

### 45. 子进程启动异常时，为什么要同时检查 pipe、watchdog 和 process group？

**参考答案：** pipe 可能承载初始化错误/ready，watchdog 负责发现子进程退出或失联，process group 清理负责终止残留 worker。只捕获父进程异常可能留下占用 GPU 的孤儿进程。

**考查点：** 进程故障传播和清理。

### 46. 如何测试 dispatch 后 abort 的路径？

**参考答案：** 构造一个已成功放入 scheduler 输入通道但在本地等待期间取消的请求，断言发送 abort，scheduler 不再继续生成，row/slot/radix 引用释放，`rid_to_state` 删除，并覆盖重复 abort 不会破坏状态。

**考查点：** error path、资源不变量和幂等性。

### 47. 如何为 `prepare_for_extend` 写不依赖 GPU 模型的单元测试？

**参考答案：** 使用假的 Req、pool、allocator 和 model config，给出 prefix hit、无命中、chunked、空输入边界，断言 forward mode、input ids、prefix/extend/seq lens 和分配位置；不把真实 kernel 输出混入这个状态构造测试。

**考查点：** 分层测试。

### 48. 如何测试 batch 过滤后的 sampling 参数没有错位？

**参考答案：** 构造至少三个请求，给每个请求不同 temperature、top-k、seed、grammar 和 penalty，过滤中间行后检查保留下来的每个字段都与对应 rid 一致，并运行一次可控 sampler 或检查传入 tensor。

**考查点：** 行对齐不变量。

### 49. 静态源码证据能证明 CUDA Graph 带来多少收益吗？

**参考答案：** 不能。源码可以确认 eligibility 分支、capture/replay 调用和 fallback 条件；收益需要相同模型、batch、硬件、graph/eager 对照的实际 benchmark，并报告 warmup、同步、TTFT/TPOT 和吞吐方法。

**考查点：** 静态事实与性能实验的边界。

### 50. 修改一个 IPC 字段前，应该做哪些影响分析？

**参考答案：** 搜索定义、构造、序列化、发送、接收、批量路径、旧请求兼容、错误/abort/shutdown路径和测试；再检查字段是否影响 scheduler Req、ForwardBatch、输出 rid 对齐或跨版本进程。最后增加正常、异常和清理测试，而不是只改一个 dataclass/Struct。

**考查点：** 跨模块 change-impact analysis。

## 复习路线

1. 先重画 `GenerateReqInput → ReqState → TokenizedGenerateReqInput → Req → ScheduleBatch → ForwardBatch → result`。
2. 再分别解释 row、slot、radix node、batch row 和 rid 的关系。
3. 最后用“dispatch 后取消”“KV 不足”“graph 不可运行”“PP 非 last rank”四个场景检查异常和所有权。

主要关联：M03、M04、M05、M06、M07、M08、M09、M10、M15，以及 `90-cross-module/`。
