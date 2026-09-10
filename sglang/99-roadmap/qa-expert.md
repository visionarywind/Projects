# 专家级 QA：SGLang 运行时设计、正确性与系统优化

- 题数：50
- 适用对象：能够独立阅读 SGLang 关键源码、设计跨模块修改并分析 GPU/分布式运行时问题的读者。
- 作答要求：除结论外，应给出不变量、失败模式、验证方法和必要的权衡；涉及性能或硬件时，明确哪些是源码证据、哪些必须通过实验确认。
- 证据范围：以 checkout `f1a512c51c73ab660cf41e1af3110c7c11e3b600` 为背景。专家级题目不把静态代码阅读等同于真实多卡、模型或 kernel 运行验证。

## A. 运行时不变量与并发正确性（1-10）

### 1. 如何形式化一个请求从进入到完成的生命周期不变量？

**参考答案：** 可按所有权阶段描述：本地 `ReqState` 建立后必须能按 `rid` 找回；dispatch 后 scheduler 至少拥有一个 `Req` 或明确终态；进入 batch 后 request row、逻辑 token 位置和 KV slot 映射一致；每个输出只能被消费一次；终态必须同时收敛 scheduler、IPC、本地 state、KV row/slot 和 radix 引用。任何异常路径都必须收敛到终态或可重试状态，不能留下孤儿 owner。

**验证方法：** 为每个阶段加状态转移断言和资源计数，使用正常完成、客户端取消、scheduler 异常、worker 异常和重复 abort 做模型化测试。

### 2. 为什么“每个 token 只有一个 KV owner”仍不足以证明 cache 正确？

**参考答案：** 还必须证明 owner 的逻辑位置正确、attention metadata 会索引到它、cache node 的 lock/ref覆盖所有读者、异步 kernel 已完成后才允许复用，以及 eviction 后不会保留悬空 index。唯一 ownership 只能避免 double free，不能避免错位读取或过早释放。

### 3. 如何区分 scheduler 的线性化点与设备执行完成点？

**参考答案：** scheduler 接纳/提交 batch 是控制面的线性化点，决定请求逻辑状态；GPU stream event 或 worker result 返回才是对应计算可消费的完成点。allocator、cache 和输出处理不能把前者当成后者，否则会在异步执行中提前复用 buffer。设计文档应明确每个字段在哪个点变得可观察。

### 4. overlap 模式下如何证明 batch snapshot 不会被下一轮写坏？

**参考答案：** 对 snapshot 中每个字段列出生产者、消费者、写入时机和完成信号；设备 tensor 要么复制到独立 buffer，要么由 event 保证读完后才复用；Python 容器要么 immutable，要么采用版本号/引用计数。仅复制外层 dataclass 不够，因为内部 list、tensor 和 closure 仍可能别名。

### 5. 一个请求被同时标记 finished 和 aborted 是否一定是 bug？

**参考答案：** 不一定，客户端取消可能与 EOS/length 结果并发到达；关键是定义终态优先级和幂等处理。对外只能发布一次终态，scheduler、ReqState、KV cleanup 和 abort message 必须允许重复或迟到事件而不 double free。若两个原因都被暴露，也要规定协议字段语义。

### 6. 如何设计跨进程消息的幂等性？

**参考答案：** 给请求和控制消息保留 `rid`、单调 sequence 或 batch/version 信息；接收方维护已处理终态/序号，重复 output、abort、finish 不重复改变资源；未知 rid 进入可观测的丢弃或错误路径而非访问空对象。幂等不等于无序安全，仍需定义迟到消息能否覆盖新状态。

### 7. 为什么 Python GC 不能作为 GPU 资源回收协议？

**参考答案：** GC 时机不可预测，循环引用、异步 stream、C++/Rust extension 持有的引用和 CUDA caching allocator 都可能使物理资源晚于逻辑终态释放。GPU buffer 还需要 stream/event 顺序保证。应由 scheduler/KV allocator/worker 明确释放，并把 Python 对象析构作为最后兜底，而不是唯一机制。

### 8. 如何证明 batch filter 对所有并行数组保持同一个置换？

**参考答案：** 先定义旧行到新行的 permutation，再让 `reqs`、rids、seq lens、sampling 参数、grammar objects、logprob buffers、KV metadata 和输出数组全部应用同一 permutation；删除行后用唯一 sentinel 检查各字段仍属于同一请求。不能分别根据各自的有效条件过滤，否则会产生长度相同但语义错位。

### 9. 为什么异常处理需要区分“未提交”“已提交未执行”和“已执行待回传”？

**参考答案：** 未提交只需回滚本地对象；已提交未执行可能需要撤销 admission、row 和 KV；已执行待回传还要防止重复输出、等待 stream/event 和完成 IPC。统一调用一个 `cleanup()` 容易在某阶段释放尚未安全释放的资源，或遗漏另一阶段的 owner。

### 10. 如何用状态机检查一条新增 abort 路径是否完整？

**参考答案：** 列出所有状态和合法边：created、dispatched、waiting、running、retracted、finished、aborted、cleaned；对每条边标注消息、资源转移和可重复次数，再检查任意中断点是否存在终止边。重点验证 abort 与 chunked、overlap、speculative verify、stream disconnect 和 shutdown 的组合。

## B. KV、缓存与内存系统（11-20）

### 11. 如何推导一个请求的 KV 显存需求并指出常见漏项？

**参考答案：** 基本量由层数、KV heads、head dimension、每 token 的 K/V 元素数、dtype、序列 token 数和副本/并行分片决定；实际还要加入 page/block 对齐浪费、speculative 临时 token、beam/候选分支、multimodal token、padding、host backup 和 graph buffers。公式只能估算容量，不能替代 allocator 的实际 segment 规则。

### 12. Radix Cache eviction 的安全条件是什么？

**参考答案：** 节点没有活跃请求或 cache lock/ref，所有相关 kernel/event 已完成，子节点和 values 的 ownership 已处理，request row 不再引用其 KV，且 eviction 不会破坏正在构造的 prefix match。应先解除逻辑可见性，再按安全顺序释放物理 slot，避免新请求在半释放节点上命中。

### 13. cache salt 或 extra key 的变化为什么会影响安全性而不只是命中率？

**参考答案：** 若不同租户、会话或语义条件不应共享前缀，key 隔离是结果正确性和数据隔离的一部分；错误忽略 salt/extra key 可能让请求复用不兼容的 KV 或暴露另一个上下文的计算结果。修改 key 组成必须同步 match、insert、finished cache 和 eviction 测试。

### 14. page size 的选择如何影响吞吐、碎片与命中率？

**参考答案：** 大 page 降低 metadata 和管理开销，但 partial page 浪费更多、短前缀命中更粗；小 page 提升细粒度复用，却增加 radix nodes、索引访问和 allocator 管理成本。最佳值依赖上下文分布、batch、attention kernel 和硬件访存，不能只按单请求容量选择。

### 15. 如何诊断“显存足够但 allocator 报无可用 segment”？

**参考答案：** 检查物理 free bytes 与符合请求大小/对齐/连续性约束的 free segments 是否不同；再看 page fragmentation、protected nodes、retraction backup、graph/static buffers、不同 KV pool dtype 和跨设备分配。记录 segment histogram 和请求所需形状，不能只看 `nvidia-smi`。

### 16. prefix cache 何时可能降低总体性能？

**参考答案：** 前缀很短或复用率低时，match、lock/ref、CPU tree traversal 和 metadata 成本可能超过省下的 prefill；高 churn 会频繁插入/evict，污染 cache 并增加同步；命中后 batch 形状也可能变得碎片化。应以命中 token、match latency、prefill latency、eviction 和端到端 TTFT 共同评估。

### 17. 如何设计 KV cache 的 differential test？

**参考答案：** 对同一请求序列分别运行启用/禁用 prefix cache、不同 page size、不同 batch 顺序和重复请求，比较最终 token、logprob、序列长度和 stop reason；再检查 allocator/free count、radix node 数和请求完成后的资源归零。数值比较需固定采样 seed，并区分允许的浮点误差。

### 18. host backup 与 GPU KV 的一致性如何验证？

**参考答案：** 给每个逻辑 token 写入可辨识 sentinel 或校验摘要，执行 GPU→host backup、释放、恢复和再次 decode，验证恢复后的 indices、内容、seq lens 与未撤回基线一致；同时测试中途 abort、重复 restore 和容量不足。真实模型测试还要覆盖异步 copy 的 event 顺序。

### 19. 为什么 KV cache 的逻辑长度和物理分配长度可能不同？

**参考答案：** prefix 命中、page 对齐、padding、speculative 临时 token、beam 分支和 cache protection 都会使物理 slot 数与用户可见序列长度不同。调度预算若把二者混为一谈，会低估显存或错误地释放尾部。

### 20. 如果将 KV layout 改为新 block 格式，影响面如何完整枚举？

**参考答案：** 必须同时审查 allocator、request-to-token mapping、Radix values、attention backend/kernel、metadata builder、CUDA Graph buffers、host backup、retraction、prefix serialization、dtype/quantization和测试 fixture。还要核对模型层写入 K/V 的 stride、TP/CP 分片和不同 device backend，不能只替换一个 pool 类。

## C. GPU 执行、数值与采样（21-30）

### 21. 如何定义 eager 与 CUDA Graph 路径的等价性？

**参考答案：** 对相同 `ForwardBatch` 语义，二者应产生等价的 hidden/logits、KV 写入、辅助输出和 `can_run_graph` 后续状态；允许浮点误差，但不能改变 mask、位置、请求行或 sampling 输入。比较前要固定 seed、同步 stream、排除 capture warmup，并覆盖 fallback 边界。

### 22. graph replay 中动态 batch 如何通过静态 buffer 表达？

**参考答案：** 通常 capture 若干支持的 batch width，把本轮有效请求写入预分配 input/metadata buffer，其余行用 padding/sentinel；kernel 通过有效长度和 mask忽略 dummy。必须保证 row 0、padding token、seq lens 和 collective 的 dummy 语义一致，不能只复制 input ids。

### 23. 为什么 graph capture 期间的随机采样尤其敏感？

**参考答案：** RNG state、seed、batch 行和 replay 次数都影响结果；若 graph 内外对 RNG 消耗数量不同，后续请求会产生不可预测偏移。实现需明确 sampler 是否在 graph 内、每请求 seed 如何映射、过滤/重排如何保持对应关系，并用固定 seed 做多轮 replay 对照。

### 24. 如何审查一个新的 logits processor 是否改变采样语义？

**参考答案：** 明确它作用于原始 logits 还是已变换 logits，是否可修改非法值、是否影响 logprob 返回、是否在 grammar mask 前后执行、是否支持 batch/device/dtype，以及 temperature/top-k/top-p 的顺序。用小词表手算边界输入，再用真实 batch 验证行隔离和 deterministic seed。

### 25. `top-p` 在数值实现中有哪些边界陷阱？

**参考答案：** 需要处理排序稳定性、累积概率舍入、至少保留一个 token、NaN/Inf logits、温度极端值和与 grammar mask 后全被屏蔽的情况。CPU 参考实现和 GPU kernel 应在这些边界上比较，而不能只测正常随机 logits。

### 26. logprob、top-logprobs 与 sampled token 的一致性应满足什么关系？

**参考答案：** sampled token 的返回 logprob 必须来自与采样使用的同一有效 logits 语义；top-logprobs 的候选集合、排序、token id 和概率归一化范围要明确是否包含 grammar mask、penalty 和 temperature。跨 batch filter 或 PP rank 传输时必须保持 token 行与 rid 对齐。

### 27. grammar constrained decoding 如何处理“没有合法 token”的状态？

**参考答案：** 需要定义明确的失败语义，例如报约束错误、回退到允许的终止 token或终止请求；不能让全是 `-inf` 的 logits 静默产生任意 token或 NaN。该状态还要释放 grammar、mask、KV和输出状态，并通过协议暴露可诊断 finish reason。

### 28. 为什么 tokenizer 版本变化可能使 grammar cache 失效？

**参考答案：** grammar 到合法 token 的编译结果依赖 tokenizer 的 vocab、special token、token boundary 和 id 映射；只用 schema/regex 作为 cache key 会复用不兼容 mask。缓存 key 至少要包含 tokenizer/grammar 语义版本和相关选项。

### 29. speculative decoding 的 acceptance 计算如何保持概率正确？

**参考答案：** 取决于具体算法，需要按 draft 与 target 分布计算接受概率和必要的 residual sampling；不能简单把 target argmax 与 draft token 比较。实现要区分候选生成、target logits、accepted prefix、rejected residual 和 RNG 消耗，并验证不同温度/top-p下的分布统计。

### 30. speculative rollback 如何与 KV 写入原子性配合？

**参考答案：** 候选 token 的 KV 可以写入临时或可回滚区域；只有 accepted prefix 确认后才把对应长度提交为请求历史，拒绝部分必须回收或覆盖，且 attention metadata不能暴露未提交 token。提交、seq lens、grammar state、stop检测和输出发布必须按同一 acceptance 结果推进。

## D. 分布式、模型加载与系统边界（31-40）

### 31. 如何证明一次 collective 的参与 rank 集合与数据 shape一致？

**参考答案：** 从 resolved parallel config 推导 group membership，再对每个调用点建立 rank×branch×tensor shape 表；所有参与者必须到达同一 collective、顺序一致、dtype/count兼容，padding/empty tensor也要有定义。单 rank 日志不足，最好收集带 group id、sequence 和 shape 的全局 trace。

### 32. 为什么“某个 rank 没有 token”仍可能必须参加 collective？

**参考答案：** collective 的同步契约通常按 group 而不是有效 token 数决定；该 rank 可能需发送零/padded payload或参与 barrier，否则其他 rank 会等待。正确做法是用明确的有效长度和 padding mask表达空工作，而不是条件跳过通信。

### 33. 如何区分通信 deadlock、GPU kernel hang 和 scheduler starvation？

**参考答案：** 看所有 rank 的 collective sequence/heartbeat、CUDA stream/event、kernel launch completion、CPU scheduler loop 和请求队列；deadlock常表现为 rank 在不同 collective停住，kernel hang表现为单设备事件不完成，starvation表现为 worker空闲但请求长期未admit。需要跨进程、跨 rank时间戳，而非单一 watchdog 超时。

### 34. 新增一个并行维度时，为什么不能只扩展 rank 公式？

**参考答案：** 还要定义 group 拓扑、通信顺序、参数/激活分片、KV/attention布局、配置验证、设备 placement、启动初始化、别名清理和错误恢复。rank 公式只解决编号映射，不能决定 collective 语义和内存所有权。

### 35. 如何审查模型 loader 对 tied weights 的处理？

**参考答案：** 确认 checkpoint 中参数是否只出现一次、目标模型是否共享 storage、TP/PP rank是否需要复制或切分、保存/加载顺序是否会覆盖共享值，以及量化/LoRA wrapper 是否改变 alias。测试应比较 storage alias 和数值，不只比较 state_dict key。

### 36. 权重加载成功但输出异常，如何定位分片错误？

**参考答案：** 先在单卡/未切分参考模型上比较关键层权重和 logits，再逐 rank 检查参数 shape、offset、transpose、fused QKV顺序、TP shard、PP stage过滤和 dtype/scale；使用可识别的递增 checkpoint 而非真实随机权重，能直接看出 shard 映射错误。

### 37. 量化 kernel、loader 和模型 config 三者的契约如何建立？

**参考答案：** config决定量化类型、group size、dtype和权重布局；loader必须生成 kernel期望的 packed weights及scale/zero-point；kernel再按同样的维度和对齐解包。应有 schema级验证和小矩阵 reference test，覆盖非整除 shape、TP切分和不同 device backend。

### 38. LoRA 动态切换为什么会影响 batch 合并和 graph？

**参考答案：** 不同请求可能使用不同 adapter，导致权重选择、额外 kernel、workspace 和 batch grouping变化；动态 adapter id 还可能不符合已 capture graph 的固定输入。scheduler需要决定按 adapter分组还是支持通用路径，并把 adapter生命周期、缓存和请求取消纳入清理。

### 39. disaggregated prefill/decode 中 KV 传输的正确性边界是什么？

**参考答案：** 发送方必须传递与目标 attention layout一致的 token范围、KV内容、位置和版本；接收方要确认传输完成、校验 request/session/cache key，并在失败时回收两端 ownership。网络成功不等于目标 GPU 可立即读，仍需 device event和协议级 commit。

### 40. HiCache 或分层 cache 如何避免不同层级出现不一致？

**参考答案：** 为每个 prefix/version定义唯一性和状态，例如 GPU hot、host warm、远端/持久层；迁移要有所有权转移和校验，读取要避免旧版本覆盖新版本，eviction要考虑正在进行的 promotion/demotion。缓存命中协议应返回可验证的 layout/version，而不只是 token key。

## E. 专家级设计、观测与验证（41-50）

### 41. 如何设计 SGLang 请求路径的端到端 trace schema？

**参考答案：** 至少包含 rid、request/session version、进程/rank、batch id、batch row、阶段（tokenize、dispatch、queue、admit、prefill、decode、sample、output、cleanup）、单调时间戳、seq lens、KV使用量、graph/eager、finish reason和错误链；跨进程要用可关联的 trace id，避免把 rid单独当作全局时间线。

### 42. 为什么平均 TTFT 可能掩盖严重的服务退化？

**参考答案：** 排队和长尾请求会被平均值稀释，cache hit、graph warmup、retraction、不同 prompt 长度和多租户 priority也会混在一起。应报告 p50/p95/p99、分桶（input length、concurrency、cache hit、mode）、首 token前各阶段时间，以及失败/超时比例。

### 43. 如何设计一个能发现 silent cross-request contamination 的测试？

**参考答案：** 为不同请求使用互不相同且可追踪的 token、KV sentinel、grammar和采样参数，随机交错 admission、batch filter、retraction、prefix hit和overlap；检查每次 output、logprob、attention读取和最终cache只含本请求允许的数据。异常时保存 batch permutation、rid、row和slot映射。

### 44. 如何把源码变更风险映射为测试矩阵？

**参考答案：** 先按影响边界分类：纯 CPU schema、IPC、scheduler状态、KV ownership、device metadata、kernel数值、collective、启动/清理；每类选择正常、边界、并发、异常、恢复和性能回归测试。跨边界修改必须至少有一条上游调用链测试和一条资源清理测试。

### 45. 什么时候应该拒绝将新逻辑放入 scheduler 热路径？

**参考答案：** 若逻辑重复解析不变配置、触发 GPU/CPU同步、执行昂贵 tree traversal、复制大 tensor、做阻塞 I/O或在每 token上分配 Python 对象，应考虑启动期 resolution、batch级缓存、异步线程或预分配。例外必须由 profiling证明收益大于开销且不破坏公平/延迟。

### 46. 如何评估一个优化是否真正改善了服务，而不是只改善 kernel microbenchmark？

**参考答案：** 同时测量端到端吞吐、TTFT、TPOT、p95/p99、GPU利用率、显存、CPU占用、队列等待、cache命中、retraction、graph fallback和错误率；使用真实请求长度分布和固定 warmup/并发对照。单 kernel加速若增加 metadata、同步或 batch fragmentation，端到端可能退化。

### 47. 如何设计故障注入来验证 graceful shutdown？

**参考答案：** 分别在 tokenization、dispatch、scheduler idle/running、KV allocation、forward、collective、output IPC 和 detokenization阶段注入异常或进程退出；检查父进程收到错误、停止接收新请求、通知/终止子进程、释放 GPU/IPC/socket、唤醒等待者且不留下孤儿进程。还要测试重复 shutdown和超时强制路径。

### 48. 什么时候一个失败应重试，什么时候必须终止请求？

**参考答案：** 暂时资源不足且状态可完整保存时可 retraction/requeue；通信瞬时错误是否可重试取决于 collective 和幂等契约；输入非法、grammar无解、权重/shape不兼容或状态已部分提交且不可回滚时应终止请求或 runtime。重试策略必须防止无限循环并保留原始错误原因。

### 49. 如何审查一个跨 Python、Rust 和 native kernel 的接口变更？

**参考答案：** 对照 Python schema/调用、PyO3或FFI类型转换、Rust ownership/错误映射、C++/CUDA声明与注册、dtype/device/stream约定、构建和 wheel版本；覆盖 ABI不匹配、异常跨边界、生命周期、并发和不同平台。不能只让 Python 单元测试通过就认为扩展契约完成。

### 50. 如果要让一名新维护者接手整个 SGLang runtime，最小“专家级验收任务”是什么？

**参考答案：** 让其在固定 checkout 上画出启动和普通请求两条完整时序，解释 `ServerArgs` 到 role config、`ReqState`/`Req`/`ScheduleBatch`/`ForwardBatch` 的所有权，推导一次 prefix hit、retraction、PP proxy、graph fallback和abort的状态变化；再为一个跨模块字段变更提出影响分析、测试矩阵和性能观测方案，并明确哪些结论尚未运行验证。能完成这些才说明理解了机制而非记住术语。

## 评估标准

- 45-50：能够以不变量、时间线、拓扑和实验设计解释复杂运行时问题，并主动标注证据边界。
- 35-44：掌握跨模块正确性和性能方法，但对分布式故障、异步内存或扩展边界仍需补强。
- 20-34：能分析局部模块，尚不能独立设计端到端验证。
- 0-19：先完成入门、中级、高级题，再按 M04→M08→M05→M09→M07→跨模块专题复习。

主要关联：M03-M18、`90-cross-module/`、`99-roadmap/`，以及 SGLang 的 Python、Rust、native kernel 和分布式运行时边界。
