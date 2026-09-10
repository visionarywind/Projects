# 高级 QA：架构推理、性能诊断与修改设计

- 题数：50
- 适用对象：能够跨 scheduler、KV、worker、并行通信和输出边界进行源码推理的读者。
- 作答要求：回答应明确假设、证据边界、状态不变量、性能代价和测试方案；只引用静态源码时必须说明尚未完成运行验证。
- 证据范围：以 checkout `f1a512c51c73ab660cf41e1af3110c7c11e3b600` 为准。高级题中涉及 CUDA Graph、collective、模型权重和硬件性能的结论，除非有实验记录，只能作为源码层推理。

## A. 调度、KV ownership 与 overlap（1-15）

### 1. continuous batching 中“尽量提高吞吐”和“保证 decode 进度”如何冲突？

**参考答案：** 新 prefill 有大量可并行 token，可能提高瞬时吞吐，但会消耗 token budget、KV pages、attention/PP 资源并延迟已有请求的下一步 decode，直接恶化 TPOT。调度器需要在 running batch 的 decode 需求、waiting 的 prefill、chunk size、priority 和容量之间做 admission，而不是最大化单轮 token 数。

**验证/设计：** 记录 waiting time、TTFT、TPOT、每轮 prefill/decode token、KV 使用量和 retraction 次数，在相同请求混合负载下比较策略。

### 2. prefix cache 命中为什么既降低 prefill 计算又可能增加资源管理复杂度？

**参考答案：** 命中前缀后可以跳过重复 attention/KV 写入，缩短 prefill；但 scheduler 必须把 radix node 的 indices 接到请求 row，增加 lock/ref，计算未命中区间的 range，并在完成时区分重复、插入和尾部 KV 的所有权。命中不是“复制一个 Python 列表”即可完成。

**验证/设计：** 用空命中、完整命中、跨 page 命中、partial page 和 cache salt 不同的请求覆盖 insert/evict/free 不变量。

### 3. page alignment 与 cache correctness 的关系是什么？

**参考答案：** 若 radix node 只代表稳定的 page 区间，partial page 可能包含仍属于请求但不应被独立复用的 token。错误地把尾部插入 cache 会让另一个请求引用未完整建立的 KV；过度保留则造成容量泄漏。因此 key 长度、values 长度和 free range 必须由同一对齐规则推导。

**验证/设计：** 对 token 长度为 page size 的前后边界逐一断言 node key、KV indices、保护长度和释放区间。

### 4. `cache_protected_len` 解决哪种竞态？

**参考答案：** 它标记当前请求中不能因普通 cache cleanup 而释放或重用的前缀/工作区长度，尤其在 unfinished/chunked request 或 overlap 尚有设备读取时。它不是总序列长度，也不是简单的 prefix hit 长度；必须结合当前 cache node 和执行完成时机理解。

**考查点：** 逻辑保护范围、设备使用完成与 Python 状态更新的错位。

### 5. 设计一个证明 decode retraction 安全的最小不变量集合。

**参考答案：** 对每个请求：逻辑 token 位置最多对应一个有效 KV slot；有效 slot 至少由一个明确 owner 持有；撤回后要么 host backup 足以恢复全部所需状态，要么请求进入终态；request row 不被其他请求复用到恢复完成；radix lock/ref 与实际使用一致；重排不会改变 rid 和输出顺序。

**验证/设计：** 在“撤回—恢复—再次 decode”“撤回—abort”“恢复期间 cache eviction”三条路径注入断言和泄漏统计。

### 6. overlap 模式为什么会出现 WAR（write-after-read）风险？

**参考答案：** 上一批的 result processor 可能仍读取 batch 的 req、seq lens、sampling 或 output metadata，而 scheduler/下一轮已原地更新同一对象。即使 Python 控制流看似先后，GPU stream 和异步 tensor 仍可能延迟使用。因此需要 snapshot、显式 event/barrier 或 immutable forward view。

**考查点：** Python 对象生命周期不等于设备读完成。

### 7. 如何判断一个字段能否安全从 `ScheduleBatch` 借用到 `ForwardBatch`？

**参考答案：** 需要回答四件事：字段是否会在 forward 完成前被 scheduler 写入；其 tensor 是否可能被下一轮复用/resize；其 device stream 上的生产和消费是否有同步；ForwardBatch 是否会越过原对象生命周期。任何一项不明确，都应复制、snapshot 或建立明确事件契约，而不是凭性能直觉省略拷贝。

**验证/设计：** 为字段画 producer/consumer 时间线，分别测试 normal、overlap、graph replay 和 exception path。

### 8. 为什么把请求从 running batch 移除不等于它已经停止占用显存？

**参考答案：** 可能仍有 GPU kernel、异步 copy、overlap result、radix lock 或 host backup 引用；row/slot 的逻辑释放若早于设备完成，会导致复用地址被旧 kernel 写入。正确实现需要把调度状态、allocator free、stream/event 完成和 cache reference 分开确认。

**考查点：** logical removal 与 physical reclamation。

### 9. 设计一个能区分 KV capacity 不足和 allocator 泄漏的实验。

**参考答案：** 固定模型、page size、请求长度和并发，分别运行无 prefix、重复 prefix、请求正常完成、请求 abort、chunked 和 retraction 场景；每轮记录 allocator free segments、request rows、radix nodes/refs、GPU allocated bytes 和 active requests。真实容量不足应随请求完成恢复，泄漏则在相同终态后单调积累。

**验证边界：** 还需排除 PyTorch caching allocator、CUDA Graph capture buffer 和编译缓存的稳定占用。

### 10. priority scheduling 可能如何破坏 cache locality 或公平性？

**参考答案：** 高优先级请求持续插入可能打断相同 prefix 的低优先级请求，降低 prefix reuse；若只按 priority admission，低优先级请求可能无限等待。策略应同时定义优先级、aging/fairness、cache reuse 和资源回收的关系，并用混合负载测量。

### 11. 为什么 chunked prefill 不是简单把大数组切片？

**参考答案：** 每个 chunk 都要更新逻辑序列位置、KV 写入范围、prefix/extend metadata、剩余输入、batch row 和下一次 admission；还要与 decode、prefix cache、PP microbatch 和 stop/abort 交互。切片本身不能表达跨轮 ownership。

### 12. speculative decoding 如何改变 scheduler 的资源预算？

**参考答案：** draft 一次可能提出多个 token，verify batch 的 input width、KV 写入位置和接受数量不再是普通 decode 的每请求一个；拒绝部分可能需要回滚或只提交 accepted token。scheduler 必须以算法定义的最大/实际 token 数预算 attention、KV 和输出，而不能沿用单 token 假设。

### 13. verify 失败时哪些状态不能提前提交？

**参考答案：** 未被 target 接受的 output ids、对应 KV 写入、序列长度、grammar 状态和 stop 判断不能作为最终生成状态提交；可以有临时 verify buffers，但 commit 必须按 acceptance 结果进行。否则下一轮会把被拒 token 当历史。

### 14. 如何分析“高 prefix hit 但吞吐没有提升”的原因？

**参考答案：** 分解为 cache match CPU 时间、radix lock/eviction、未命中 token 比例、attention launch、KV copy/host transfer、batch 形状、graph eligibility 和输出/通信成本。prefix hit 只减少部分 prefill FLOPs，不保证 scheduler、metadata 或 decode 成为零成本。

### 15. 如果 retraction 次数突然升高，优先检查哪些不变量？

**参考答案：** 检查 decode admission 使用的 page/token 预算是否与实际 allocator 一致，prefix/unfinished cache 是否释放，request row 是否泄漏，host backup 是否耗尽，max context/new tokens 是否改变，以及 batch filtering 是否遗留无效请求。不能只调大显存或降低 batch size而跳过 ownership 检查。

## B. CUDA Graph、attention 与 device contract（16-25）

### 16. CUDA Graph replay 对“地址稳定”究竟要求什么？

**参考答案：** 被 capture 的 kernel 参数中涉及的输入、输出和 metadata buffer 地址/布局必须符合 replay 预期；每轮可以更新预分配 buffer 的内容，但不能随意换成新 tensor 或改变 graph 假定的 shape/stride。地址稳定不等于数值不变。

### 17. 为什么 `.item()` 或 `.cpu()` 可能破坏 graph/性能设计？

**参考答案：** 它们可能触发 device-to-host 同步，把本应异步的控制流变成 CPU 等待；在 capture 中还可能是 graph 不支持的动态操作。即使功能正确，也会让每步 decode 增加同步延迟。应区分 graph 外准备 metadata 与 graph 内执行。

### 18. `can_run_graph` 应该被视为布尔优化开关还是输出契约？

**参考答案：** 更接近输出契约：它反映当前 forward 是否满足该 graph runner 的 batch/shape/backend/同步条件，影响后续 result processing、capture 状态和性能统计。调用方不能假定请求“想用 graph”就一定得到 graph。

### 19. attention backend 的 `init_forward_metadata`、`out_graph`、`in_graph` 三者如何协作？

**参考答案：** 普通路径根据动态 `ForwardBatch` 完整初始化；graph 外路径准备并复制/写入 replay 所需 buffer；graph 内路径只消费已捕获布局并更新允许变化的内容。三者必须保持相同的逻辑含义：query/token range、KV indices、seq lens 与 page metadata 一致。

### 20. 为什么 graph capture width 与 speculative width 不匹配时应回退？

**参考答案：** graph 的 kernel launch、buffer shape 和 attention metadata 通常针对固定 token width；verify 实际 width 不匹配会造成越界、错误索引或无效计算。eager 路径虽然 launch 开销较高，但能接受动态 shape，是正确性优先的 fallback。

### 21. DP attention 中 padding 的性能与正确性代价是什么？

**参考答案：** 为让不同 rank 参与相同 collective/graph shape，较短 rank 可能 padding 到统一 token 数，浪费计算和 KV metadata；但缺少一致 padding 可能使 collective 不匹配或 graph buffer 不一致。应测量 padding ratio，并确保 mask/有效长度不把 dummy token 当真实 token。

### 22. MLP sync 条件为什么会影响 graph eligibility？

**参考答案：** 某些 DP/专家或负载平衡路径需要 rank 间同步 token/工作量；同步参与者、shape 和 buffer 必须与已 capture 的图一致。若本轮条件变化而强行 replay，可能 deadlock 或读取错误的 padding/通信范围，因此 eligibility 必须包含该条件。

### 23. 如何定位“graph 可运行但比 eager 慢”的问题？

**参考答案：** 分别测量 capture 后 replay 的 kernel 时间、graph launch 时间、metadata 准备、CPU 同步、padding、batch shape 和输出拷贝；确认比较包含相同 warmup、同步点和请求混合。graph 减少 launch overhead，不保证对小 batch、频繁 shape 变化或高 padding 场景总是更快。

### 24. attention metadata 错一行会为什么比直接崩溃更危险？

**参考答案：** 错误的 `req_pool_indices`、seq lens 或 KV indices 可能仍在合法内存范围内，kernel 不报错却读取另一个请求的历史，产生静默结果污染。测试应对每行使用可区分的 sentinel KV，并校验跨请求隔离。

### 25. 设计 attention backend 接口时，哪些信息必须显式声明？

**参考答案：** 支持的 forward modes、query/token layout、KV page/block layout、dtype/head shape、graph capture/update能力、所需 stream/event、是否允许动态 shape、输出 cache location 语义和失败/fallback条件。只声明一个 `forward` 方法会把关键契约藏在调用约定中。

## C. 多维并行、加载与模型结构（26-35）

### 26. 如何从 `rank = tp_size * pp_rank + tp_rank` 推导 TP/PP groups，并指出其局限？

**参考答案：** 在只含 TP/PP 且 rank 编号按 PP stage 外层、TP 内层排列时，固定 `pp_rank` 的连续 rank 构成 TP group，固定 `tp_rank` 的跨 stage rank 构成 PP group；例如 TP=2、PP=2 得 `[0,1]`、`[2,3]` 和 `[0,2]`、`[1,3]`。若实际 placement、DP、DCP 或 group reorder 不同，必须以初始化代码为准。

### 27. 为什么 global rank 与 GPU id 不能作为同义词？

**参考答案：** rank 是通信世界中的逻辑进程编号；GPU id 由 local rank、`CUDA_VISIBLE_DEVICES`、node rank、base GPU id 和 step 等决定。多节点时不同节点可有相同 local GPU id，错误绑定会使通信正确性、显存位置和性能同时出问题。

### 28. group alias cleanup 为什么是分布式资源问题？

**参考答案：** 同一个 process group 可能通过多个语义属性暴露；清理时重复 destroy、漏清理或保留失效 alias 会导致后续初始化使用错误对象或资源泄漏。需要明确底层 group 所有权与别名引用，而非看到每个变量都独立销毁。

### 29. checkpoint file sharding、tensor parallel loading 和 pipeline filtering 的顺序如何影响内存？

**参考答案：** 若每个 rank 先加载所有文件再过滤，会产生峰值 CPU/GPU 内存；若 loader 能按文件索引、参数映射和 PP stage 先筛选，再按 TP 维度切分/复制，可降低峰值。但过滤必须不误删共享 embedding、norm 或 stage 边界参数。

### 30. `weight_loader` 为什么比 `state_dict[name] = tensor` 更重要？

**参考答案：** 参数对象可能携带 TP shard、fused parameter、quantization scale、expert index 或 transpose 语义；`weight_loader` 把 checkpoint tensor 转为当前 rank 参数的布局，并可能执行复制、切片或合并。绕过它会造成 shape 看似匹配但数值/分片错误。

### 31. `StackedParamsDispatch` 类机制的风险是什么？

**参考答案：** 它可将多个 checkpoint 参数映射到一个 fused/staked 目标参数，减少加载和运行开销；风险是偏移、顺序、shape、quant scale 与 TP shard 规则不一致。测试必须分别验证每个子参数在目标 fused tensor 的位置和 rank 视图。

### 32. 量化权重加载为什么不能只检查 dtype？

**参考答案：** 量化参数常包括 scale、zero point、group index、packed layout 和 kernel-specific metadata；dtype 相同也可能 layout 不同。必须检查 loader、模型层、kernel 对 packed shape 和辅助参数的共同契约。

### 33. MoE 的 EP 与普通 TP 有什么不同的通信压力？

**参考答案：** TP 通常切同一层的张量并做固定 collective；EP 将 expert 分布到 rank，需要按 routing 结果 dispatch token、执行本地 experts、再 combine，通信量和 token 分布动态变化，还涉及 capacity、负载均衡和 expert parallel group。

### 34. 多维并行初始化如何避免“某 rank 走特殊分支导致集体等待”？

**参考答案：** 所有 rank 必须基于一致的 resolved config 构造同一组 group 并按相同顺序进入必要 barrier/collective；不能让某个 PP stage 或无 expert rank单独跳过公共同步，除非接口明确允许。审查应建立 rank×阶段控制流表。

### 35. 模型加载、KV pool 和 attention backend 为什么必须作为同一启动闭包分析？

**参考答案：** model config 决定 hidden/head/layer/context 形状，loader 决定模型参数和 dtype，KV pool 需要层数/head/page/layout，attention backend 又消费这些 metadata。任一配置不一致可能到第一次 forward 才暴露，ready 必须晚于整个兼容性闭包完成。

## D. 采样、约束、speculative 与多模态（36-43）

### 36. top-p、grammar mask 和 temperature 的组合是否可交换？

**参考答案：** 一般不可任意交换。temperature改变分数尺度，grammar mask改变可行集合，top-p依赖排序和归一化后的累积概率；不同顺序会改变候选集合。实现和测试应把顺序当作 API 语义，而不是数学上“都是 logits 操作”。

### 37. logprob 与生成 token 的对齐为什么容易出错？

**参考答案：** 输入 prefill 的 logprob、生成 decode 的 logprob、speculative accepted/rejected token 和 batch 行过滤有不同时间点；输出必须明确每个 score 对应哪个 token、是条件于哪段历史，以及在 PP/overlap 后如何按 rid 重排。

### 38. custom logits processor 如何影响 graph 和 batch 性能？

**参考答案：** 任意 Python/动态 processor 可能需要回到 eager、产生动态 shape 或在 CPU/GPU 间同步；即便数值逻辑正确，也可能阻止 graph replay。接口应声明 device、batch、词表和可捕获性限制，并在性能文档中区分 fallback。

### 39. grammar 对象、mask tensor 和请求 row 的三种生命周期如何同步？

**参考答案：** grammar 对象随请求语法状态推进，mask tensor随每轮 batch 计算，row随 batch filter/reorder 变化。完成/abort时三者都要移除或释放；overlap时还要等延迟 sampling 读取完旧 mask，不能只按当前 Python reqs 删除。

### 40. speculative decoding 与 grammar constraint 同时启用时，谁拥有合法性状态？

**参考答案：** draft 可以提出候选，但 target/grammar 状态必须依据实际接受的 token推进；被拒候选不能推进 grammar。若 grammar 在 draft 阶段也用于剪枝，则需要明确它与 target tokenizer、accepted prefix 和 rollback 的契约。

### 41. 多模态输入为什么会影响普通 text batch 的假设？

**参考答案：** 图像/音频/video经过 processor 产生 embedding、占位 token或额外 metadata，输入 token 数与实际 embedding 数可能不一；`ForwardBatch`、position ids、attention mask、KV预算和 batch row 必须同时表达替换关系。不能把多模态请求仅当作更长字符串。

### 42. multimodal embedding override 为什么可能使 CUDA Graph 回退？

**参考答案：** 每个请求的 embedding tensor、数量或地址可能动态变化，与已 capture 的输入 buffer/shape不匹配；强行写入可能产生错误位置。graph eligibility 应检测 override 条件，采用 eager 或稳定的预分配拷贝路径。

### 43. stop string 跨 token 时，scheduler 和 tokenizer 哪一层应负责什么？

**参考答案：** scheduler 负责 token 生成、EOS/长度等 token-level 停止；TokenizerManager/detokenizer 负责增量文本拼接和跨 token stop string 检查，再通过 abort/finish 影响后续调度。边界必须防止已经暂存但不应发送的 stop 文本泄漏。

## E. 故障诊断、性能实验与修改审计（44-50）

### 44. 一个请求出现重复 token，如何按层次排查？

**参考答案：** 先确认模型实际 `next_token_ids` 是否重复；若模型正确，检查 scheduler 是否重复处理同一 result、seq lens 是否多次递增、overlap 是否重放 batch、rid 输出聚合是否重复追加、detokenizer offset 是否回退。每层记录 batch id、rid、token index 和 finish 状态，不能只比较最终文本。

### 45. 一个请求文本正确但另一个请求收到它的输出，最可能的跨模块原因是什么？

**参考答案：** 优先检查 `rids[i]` 与 batch 行的对齐、filter/reorder 后的 sampling/output arrays、`rid_to_state` 更新和批量 IPC 序列化；如果 token 本身正确而归属错，通常是关联键/行映射问题，而不是模型权重。

### 46. 如何设计 TTFT/TPOT benchmark，避免把排队和 warmup 混在一起？

**参考答案：** 明确请求到达时间、入 waiting、admission、prefill start/end、首 token发送、每个 decode step、完成和客户端接收时间；单独报告 warmup/capture/compile，控制 prompt/output 长度、并发、streaming 和同步方法。TTFT不应只用服务器总 wall time猜测。

### 47. 如何区分 tokenizer CPU 瓶颈、scheduler 瓶颈和 GPU forward 瓶颈？

**参考答案：** 做分层对照：预先提供 token ids/关闭 detokenization观察 tokenizer；记录 ingest/admission 和 batch planning 时间；用 CUDA event/Nsight或可靠同步测 forward/kernel；同时检查队列等待和 IPC。单看 GPU utilization 不能发现 CPU 排队或输出聚合瓶颈。

### 48. 若要把一个 scheduler 字段改成 `msgspec.Struct` 字段，最危险的影响面是什么？

**参考答案：** 构造位置和默认值、序列化/反序列化、跨进程兼容、批量复制/过滤、pickle或共享对象假设、异常路径和测试 fixture 都可能受影响。还要检查字段是否被动态添加、是否依赖可变默认值，以及 worker/Tokenizer 两侧版本是否同步。

### 49. 修改 `ModelRunner.__init__` 前为什么必须先分析其初始化依赖顺序？

**参考答案：** 它连接 model config、device/dtype、loader、attention backend、memory pool、CUDA Graph 和 sampling；改变顺序可能使某组件读取未解析配置、在错误 device 分配 buffer或在 group ready 前执行 collective。应画出构造调用图，并用最小初始化测试覆盖失败清理。

### 50. 如何做一次“实现深度 + Demo 深度”的审计，而不是只检查文档存在？

**参考答案：** 实现深度要逐模块验证入口、核心状态、正常/分支/错误/cleanup路径、源码证据和测试地图；Demo深度要从真实入口追到子进程、IPC、scheduler、forward、输出和shutdown，并区分静态追踪与实际运行。最后检查引用链接、代码行号、未验证声明、题目覆盖矩阵和用户可复现命令。

## 评估标准

- 45-50：能提出不变量、实验和跨模块修改方案，并明确静态证据边界。
- 35-44：能正确解释普通主线和主要资源，但对 overlap、graph 或并行拓扑仍需补证。
- 20-34：掌握局部实现，尚不能可靠推导故障传播和性能原因。
- 0-19：先完成入门、中级题，再以 M04→M08→M05→M09→M07 顺序重读。

主要关联：M04、M05、M06、M07、M08、M09、M10、M11、M12、M13、M14、M15，以及 `90-cross-module/`。
