# 推理方向：专家级面试题

- 题数：12；稳定 ID：I-E001–I-E012
- 证据规则：C/I；生产环境/压测 U
- 侧重：第一性原理、失效边界、可证伪论断、生产决策

### I-E001 从不变量角度推理：推理服务启动时必须建立的哪些不变量一旦被破坏就会导致静默错误？

**元数据：** 知识域：`I01 推理服务生命周期与不变量`；能力目标：`C01 从第一性原理推导运行时不变量`；题型：原理推导与失效分析；频率：中频；前置/后续：`I-B001`（双阶段差异）、`I-I004`（continuous batching 机制）；证据状态：源码 C，运行结论 U。

**背景知识：** SGLang 推理服务启动时建立三层池——`ReqToTokenPool`（请求 row 到 token location 的 int32 映射）、`TokenToKVPoolAllocator`/`PagedTokenToKVPoolAllocator`（可分配的 slot/page 索引）、`MHATokenToKVPool`/`MLATokenToKVPool`（真实 GPU KV tensor）。`RadixCache` 不拥有独立 KV 数据副本，而是把已有 slot 索引挂在 radix node 上，用 `lock_ref` 保护仍被请求使用的节点。`sglang/90-cross-module/pooling-and-resource-management.md`。这三层的所有权必须严格分离：row 的拥有者是 scheduler 中的 `Req`，底层 tensor 的拥有者是 pool，radix node 的 `lock_ref` 必须与请求/host 引用配对。`insights/02-kv-pool-lifecycle-and-ownership.md`。`pooling-and-resource-management.md` 的资源所有权表还列出了 Mamba state、CUDA custom memory pool 和 CUDA Graph static buffers 的 ownership 契约——这些也是启动时必须建立的不变量。此外，scheduler 初始化顺序（加载模型权重 → KV memory pools → attention backends → CUDA graphs → prewarm）本身就是不变量链——任何顺序颠倒都会导致 backend 与 pool 的不匹配。`sglang/01-modules/M04-scheduler-batching/README.md`。

**问题分析：** 1) 从三层池模型推导启动时必须建立的核心不变量；2) 分析每个不变量被破坏后的具体症状——重点区分"立即崩溃"与"静默错误"；3) 给出可证伪的判定条件：什么观测信号表明某个不变量已被破坏但服务仍在运行。

**具体答案：** 面试中可以这样回答：

> 推理服务启动时必须建立以下不变量，按"破坏后是否静默"分为两类：
>
> **立即崩溃型（易检测）：** 一是 KV physical tensor 的地址/layout/dtype 与 attention backend 契约不一致——graph replay 直接失败或 kernel 读错；二是 page size 与 allocator 模式不匹配（`page_size==1` 选 slot allocator，其他选 paged allocator），分配时 debug assert 失败；三是 CUDA Graph static buffer shape 与 capture bucket 不匹配，首次 replay 即报错。`sglang/90-cross-module/pooling-and-resource-management.md` 的资源所有权表列出了这些依赖。
>
> **静默错误型（难检测）：** 一是 radix `lock_ref` 与请求实际引用不对等——某请求已结束但 lock_ref 未归零，其 KV 节点永远不被淘汰（缓存泄漏，表现为可用 token 持续下降但 free 统计正常）；二是 `ReqToTokenPool.free(req)` 不清零整行 token 映射，新请求复用该 row 后读到旧 token location（attention 读错上下文，输出语义错误但无 exception）；三是 chunked prefill 的 `cache_protected_len` 与 `prefix_indices` 不对齐——partial page 被提前释放，下一轮 extend 从错误位置继续写入。`sglang/01-modules/M08-kv-cache/README.md`。四是 CUDA Graph padded batch 的 row 0 被误写真实数据——dummy request index 指向了合法 KV slot，导致首个或末位请求的结果被覆盖。
>
> 判据：不变量破坏的静默信号包括 free pages 计数闭合但 GPU memory snapshot 持续上涨、attention 输出数值与 eager 模式不一致（仅 graph 路径复现）、abort 率正常但输出质量下降。`insights/02-kv-pool-lifecycle-and-ownership.md` 的故障树把"free/release 统计不一致"分为 Radix lock/ref 泄漏、deferred release 未 merge、host backup/device value 状态不一致三类，分别对应上述三种静默破坏。从第一性原理看，所有静默错误的共同特征是"系统状态机进入了未被显式处理的状态"——三层池的 ownership 协议只定义了正常路径（allocate → use → free）的状态转移，对异常路径（abort mid-transfer、overlap future failure、partial commit）的处理如果不完整，就会留下悬挂状态。这正是 I-E007 故障模型中"根因"层的来源。具体到 SGLang 的 `ScheduleBatch.retract_decode`——它按 retraction order 释放请求，普通请求尝试释放 KV 并做 host backup后重新入队，无法恢复的请求设置 abort。如果在 retraction 过程中 allocator 的 free list 与 radix cache 的 lock_ref 不同步（如先 free 了 slot 但 radix node 仍持有引用），就会产生悬挂索引——新请求分配到该 slot 后读到旧 KV 数据。`sglang/01-modules/M04-scheduler-batching/README.md` 的 retraction 段落。

**技术洞察：**

```text
# 核心不变量集合（启动时建立，运行时维护）
INVARIANT_1: ∀ slot s ∈ allocator, |owners(s)| ≤ 1    # 单 owner 约束
INVARIANT_2: lock_ref(node) = active_refs(node)         # 引用计数配对
INVARIANT_3: committed_len(req) ≤ allocated_len(req)    # 逻辑 ≤ 物理
INVARIANT_4: row_mapping[row] 指向的请求仍持有该 row    # row 生命周期
INVARIANT_5: graph_static_buffer.address == capture_address  # 地址稳定
INVARIANT_6: page_aligned(free_index) 当 page_size > 1   # 释放对齐

# 静默破坏的观测信号
silent_corruption ← (free_count_correct ∧ gpu_mem_growing)
                  ∨ (output_diverges_eager_vs_graph)
                  ∨ (quality_drops ∧ abort_rate_normal)
```

C：`sglang/90-cross-module/pooling-and-resource-management.md` 资源所有权表；`sglang/01-modules/M08-kv-cache/README.md` 的 lock_ref/prefix 描述；`insights/04-design-decision-and-validation.md` 的不变量 oracle 定义（无 double free，live index 互斥，释放后 sentinal 不被新请求读到）。I：不变量与静默信号的映射推导——关键洞察是"free 计数正确"不等于"ownership 正确"，需要同时检查 live index 集合的互斥性和可达性。U：具体哪些场景会触发静默错误需 GPU 与模型实验验证。

**图文说明：** 三层池模型的 ownership 图——从 `ReqToTokenPool`（row 映射）到 `Allocator`（slot/page 索引）到 `KVCache`（物理 tensor），RadixCache 用虚线箭头表示"引用而非拥有"。标注每个箭头的不变量约束：row 唯一绑定（INVARIANT_4）、slot 单 owner（INVARIANT_1）、radix lock_ref 配对（INVARIANT_2）、graph 地址稳定（INVARIANT_5）。在 overlap loop 路径上额外标注 WAR hazard 点：scheduler 修改 static buffer 而 graph 仍在读（INVARIANT_5 的运行时破坏）。破坏任一条虚线或实线箭头即标注为对应的静默/立即错误类型。建议在图上用红色标注 overlap 模式下的额外 WAR hazard 点——`last_batch`、`result_queue` 中的 batch 和当前 `batch` 可能同时代表不同生命周期阶段，"当前 Python 变量"不能直接等同于"设备上唯一正在执行的 batch"。

**拓展知识：** 与 I-E007 衔接：不变量破坏是故障树的底层根因——radix lock_ref 泄漏对应"KV leak"分支，free/release 统计不一致对应"allocator 碎片"分支；与 I-E011 衔接：KV cache 放置到 host/remote 后新增的不变量（device/host page id 显式映射）更容易被破坏——HiCache 的 `LogicalHostPool` 只维护 page-aligned 的逻辑 anchor，实际 host pool 按 `slot_page_size` 管理，device index、host index 和传输页 id 必须由 transfer 层显式转换。`sglang/01-modules/M13-disaggregation-hicache/README.md`。与 I-E009 衔接：不变量的破坏不仅是可靠性问题——恶意请求可以通过制造特定模式的不变量违反（如大量不同 prefix 触发 eviction storm）来实现资源耗尽攻击。与 I-E006 衔接：长上下文推理中 KV 容量上限的计算依赖于"每请求 KV bytes = seq_len × C_token"——若 page 对齐的 partial tail 不变量被破坏（partial page 被提前释放或未对齐访问），实际可用容量将低于理论值，使容量规划失准。

**常见追问及回答要点：**

1. **问：如何在不重启服务的前提下检测不变量已被破坏？** 答：对比 `free_pages + release_pages + evictable_size + protected_size` 之和是否等于总容量——不等说明存在泄漏或 double-free；在 overlap 路径对比 eager 与 graph 的 attention 输出差异；定期 dump `lock_ref > 0` 的 node 集合并验证对应请求是否仍活跃。
2. **问：flush_cache 能修复不变量破坏吗？** 答：只能修复部分。flush 重置 radix tree、request pool 和 allocator，但前提是 engine idle——非 idle 时强清会造成悬挂访问。`sglang/01-modules/M08-kv-cache/README.md` 注明 flush 只在 idle 时执行。运行时的不变量破坏（如 row 映射残留）需要请求级修复而非全局 flush。
3. **问：CUDA Graph 的不变量为什么比普通 eager 更难维护？** 答：graph replay 要求地址和 shape 契约跨 iteration 稳定，但请求长度和 batch 组成每轮变化。metadata 被刻意拆成 graph 外（动态）和 graph 内（静态）两部分；生命周期判断错误会形成 WAR race，且只在特定 batch 序列下复现。`sglang/01-modules/M09-attention-cuda-graph/README.md`。CUDA Graph 的 capture stream 和 backend session 还引入了 stream-ordered 的内存生命周期——graph 内引用的 buffer 必须在 graph 对象销毁后才能释放，这与动态 KV pool 的按需释放语义冲突，需要通过 global graph memory pool 隔离管理。

**易错点与反例：**

- 反例："不变量破坏必然导致 OOM 或 crash"——错误。row 映射残留和 lock_ref 泄漏不触发 OOM，只表现为静默的数据错误或容量缓慢下降。
- 反例："三层池的不变量由 Python GC 自动维护"——错误。slot 索引通过 tensor 拼接维护 free list，radix node 通过显式 inc/dec lock_ref 管理引用，两者均不受 Python GC 控制。`sglang/90-cross-module/pooling-and-resource-management.md`。
- 易错：混淆 `available_size()` 与真实可用容量——`available_size()` 计入 free 和 deferred release，但"可立即按当前顺序取出的连续页"还取决于 `merge_and_sort_free()`。`sglang/01-modules/M08-kv-cache/README.md`。
- 易错：认为"启动后不变量由框架自动维护"——实际上每个新功能的引入（新 eviction strategy、新 backend、新并行维度）都可能破坏既有不变量。例如开启 DP Attention 后 attention TP 宽度从 `tp_size` 变为 `tp_size / dp_size`——若不更新 graph capture bucket 中的 shape 假设，graph replay 将读到错误 head 数的数据。

**实践与验证：** [U] 构造 page_size=64 + partial page 场景，验证 partial tail 是否在 chunked prefill 后被正确处理（不泄漏、不提前释放）；构造 lock_ref 不对称的 abort/finish 路径，观察 evictable_size 是否回到预期。`insights/04-design-decision-and-validation.md` 的 allocator 正确性实验设计。oracle：无 double free，live index 集合互斥，释放后 sentinal 不被新请求读到。当前未执行 GPU 实验。

**运行时不变量监控（生产建议）：** 在设计验证之上，生产环境应部署轻量级的不变量监控——后台线程每 30 秒执行一次 "consistency check"：遍历 radix tree 的所有 leaf node，验证 `lock_ref` 之和等于 active request 的引用数；遍历 allocator 的 free list，验证 `free + release + evictable + protected = total_capacity`。这些检查在 CPU 上执行，不阻塞 GPU forward。`insights/04-design-decision-and-validation.md` 的资源计数闭合 oracle 提供了检查清单。当检测到不一致时，根据严重程度选择：(a) 仅记录 metrics 供后续分析（轻微不一致），(b) 触发 flush_cache 重置（中等不一致），(c) 优雅退出进程并触发重启（严重不一致，可能已产生错误输出）。

**形式化验证边界：** 对于需要严格正确性的场景（如金融/医疗推理），可使用 TLA+ 建模三层池的 ownership 协议——定义 `alloc`、`use`、`free`、`evict`、`ref`、`unref` 六个 action，不变量 `∀s, |owners(s)| ≤ 1` 作为 safety property。TLA+ 模型可发现协议在并发 eviction + retraction 组合下的边界条件——这是单元测试难以覆盖的交错场景。`insights/06-speculative-decoding-as-transaction.md` 的事务建模方法可以扩展到不变量验证。补充验证思路：在 overlap loop 中插入"一致性 snapshot"——每 N 轮冻结 `last_batch`、当前 `batch`、`result_queue` 的 KV index 集合，验证三者互不重叠（WAR 安全）。`sglang/01-modules/M04-scheduler-batching/README.md` 的 overlap loop 文档指出"当前 Python 变量不能直接等同于设备上唯一正在执行的 batch"——这正是 WAR hazard 的根源。

**scheduler 初始化顺序作为不变量链：** SGLang 的 Scheduler 初始化按严格顺序执行：加载模型权重 → 创建 KV memory pools → 初始化 attention backends → 初始化 CUDA graphs → prewarm sampling → 处理 capture 后的 KV pool resize。`sglang/01-modules/M04-scheduler-batching/README.md`。这个顺序本身就是一个不变量链——CUDA Graph capture 依赖 attention backend 已就绪，attention backend 依赖 KV pool 已分配。如果初始化顺序被修改（如在 KV pool 创建前尝试 graph capture），会导致 backend 使用未初始化的 pool 引用。生产中的教训：任何"优化启动速度"的并行化初始化尝试都必须验证这个顺序不变量。

**不变量与性能调优的冲突：** 某些性能调优可能隐式破坏不变量——例如开启 `enable_mixed_chunk` 允许 prefill 和 decode 混合到同一 batch，这增加了 `PrefillAdder` 的复杂度（需要同时计算 prefill token 和 decode token 的预算），也增加了 `ScheduleBatch` 的 prepare 逻辑复杂度（需要正确设置 forward_mode）。`sglang/01-modules/M04-scheduler-batching/README.md` 的 chunked prefill 段落。调优新功能时应同步更新不变量清单——这是"配置即正确性"的工程实践。

**与 Mamba/Hybrid 模型的不变量差异：** Mamba 模型使用 recurrent state 而非 KV cache——`MambaPool` 为每个 Mamba layer 分配 conv/temporal state，`HybridReqToTokenPool` 把 request row 与 Mamba index 一对一映射。`sglang/90-cross-module/pooling-and-resource-management.md` 的 Mamba 段落。Mamba 的不变量包括：conv window 的 temporal 一致性（decode 第 t 步的 conv state 必须基于 t-1 步的状态更新，不能跳过），以及 overlap schedule 中的 ping-pong buffer 生命周期（异步传输中的 buffer 不能被下一轮复用）。这些不变量在 attention-only 模型中不存在——增加模型类型时必须扩展不变量清单。

**不变量破坏的级联效应：** 一个不变量的破坏可能触发级联——radix lock_ref 泄漏 → evictable_size 不减少 → admission 拒绝新请求 → waiting queue 积压 → TTFT 上升 → 客户端重试 → 请求风暴。`insights/02-kv-pool-lifecycle-and-ownership.md` 的故障树展示了这种级联路径。在排查时，应区分"首发异常"（最初被破坏的不变量）和"继发异常"（因级联产生的症状）——优化继发异常而不修复首发异常是常见的反模式。

**chunked prefill 的额外不变量：** chunked prefill 引入了"同一 Req 跨多轮执行"的场景——必须保证 request row 复用（`ReqToTokenPool.alloc` 对已有 `holds_kv` 的 chunked request 复用原 row）、partial page 的 `cache_protected_len` 跟踪（page_size > 1 时 partial page 属于请求工作集但尚未成为完整 radix node）、以及下一轮 input range 从已提交长度继续（`extend_range` 的正确计算）。`sglang/01-modules/M08-kv-cache/README.md` 的 chunked prefill 段落。如果 chunked prefill 在 chunk 边界处错误释放了已提交的 KV 前缀，下一轮 extend 将读取未定义的 KV——这是"先写后读"（WAR）的时序错误在 chunked prefill 中的表现。修复方式是在 chunk 边界处调用 `cache_unfinished_req` 将已提交前缀插入 radix tree，确保后续 chunk 可以 prefix match。

**I-E001 总结性论断：** 推理服务的不变量集合不是一个静态列表——它随部署配置（page_size、chunked prefill、CUDA Graph、speculative decoding、PD disaggregation）的组合而增长。每次添加新配置维度时，应执行"不变量审计"：列出该维度引入的新 owner、新引用、新生命周期阶段，并逐一验证相应的 inc/dec、alloc/free、capture/release 是否配对。这是"配置组合爆炸"下的正确性保障策略——也是专家级工程师与初级工程师的核心区别之一。

**I-E001 的专家级思考框架：** 回答本题的关键不是罗列不变量清单——任何工程师都可以从源码中找出几个不变量。专家级的回答必须包含三个层次：(1) 从三层池模型的第一性原理推导不变量集合（ownership 分离 → 6 个不变量）；(2) 区分"立即崩溃"与"静默错误"——只有后者是真正的生产威胁，因为前者会被监控系统捕获；(3) 给出可证伪的检测信号和修复策略的优先级——优先修复可能导致错误输出的静默错误（row 映射残留），其次修复只导致性能退化的错误（lock_ref 泄漏）。这个框架可以推广到任何分布式系统的可靠性分析。

**与 overlap schedule 的深度关联：** overlap 模式将"当前 Python 变量 ≠ 设备上唯一正在执行的 batch"这一事实转化为不变量挑战。在普通 event loop 中，只有一个 batch 在设备上执行；在 overlap loop 中，`last_batch`（等待 result processing）、`result_queue` 中的 batch（GPU 已执行但 CPU 未处理）、当前 `batch`（正在准备或发射）可能同时存在。`sglang/01-modules/M04-scheduler-batching/README.md` 的 overlap loop 描述。这三个 batch 的 KV index 集合必须互不重叠——这是 INVARIANT_1（单 owner）在 overlap 模式下的强化版本。attention backend 通过 `SharedReadEnds` 声明共享读取何时结束，为这个不变量提供了运行时验证机制。

**prefix cache 的不变量特殊性：** radix cache 的 prefix sharing 引入了"一个 KV node 被多个请求引用"的场景——`lock_ref` 正是为此设计。但 prefix match 还有 page alignment 约束：token 前缀命中长度不一定等于任意 token 边界，未对齐尾部不能作为完整 radix node。`sglang/01-modules/M08-kv-cache/README.md` 的 page alignment 描述。如果系统把未对齐的 partial tail 当作可共享 prefix，后续请求将读取未定义的 K/V 值。这是不变量 INVARIANT_6（page_aligned free_index）的另一种表现形式——不仅 free 需要对齐，radix insert 和 match 也需要对齐。在支持 bigram/Eagle 的变体中，key 的视图转换进一步增加了对齐约束的复杂性。

**跨请求 KV 隔离的形式化论证：** 为什么 allocator 必须保证"一个 slot 同时只被一个 owner 引用"？形式化证明：假设 slot s 同时被请求 A 和请求 B 引用——A 的 attention 读取 s 时可能看到 B 写入的值（如果 B 的 KV update 尚未 flush），或者 B 的 eviction 释放了 s 而 A 仍在读取（UAF）。这两种情况都违反了"请求级隔离"的安全属性。`sglang/90-cross-module/pooling-and-resource-management.md` 的资源所有权表将"double free/漏 free"列为 KV slot/page index 的主要风险——这正是 INVARIANT_1 被破坏的后果。在 DP Attention 模式下，不同 DP replica 的 token 工作集不同，但 KV slot 的 ownership 仍然遵循单 owner 约束——DP attention gather 是通过 communication 聚合 token buffer，而非共享 KV slot。

**与 SWA（Sliding Window Attention）的不变量交互：** SWA 限制了 attention 的有效范围为最近 w 个 token——超出窗口的 KV 理论上不再被 attention 读取，但仍占用显存直到 eviction。这引入了一个微妙的不变量："逻辑可访问 KV 范围"≠"物理持有 KV 范围"。`sglang/01-modules/M08-kv-cache/README.md` 提到 SWA 使用双 allocator（一个管理窗口内 KV，一个管理窗口外可 evict 的 KV）。如果 SWA 的窗口滑动与 KV allocator 的 eviction 不同步——例如窗口已滑动但旧 KV 尚未标记为可 evict——将导致实际可用容量低于理论值。这在长上下文 SWA 场景下尤为显著——窗口大小 w 可能远小于 seq_len，但 O(seq_len - w) 的过期 KV 必须及时回收。

**I-E001 总结——从不变量到系统可靠性：** 推理服务的可靠性本质上是对不变量的维护能力。启动时建立的 6 个不变量（单 owner、引用配对、逻辑≤物理、row 生命周期、地址稳定、释放对齐）是系统正确性的"基底"——任何上层功能（continuous batching、speculative decode、PD disaggregation）的正确性都建立在这些不变量之上。专家级的系统思考方式是：当发现一个异常时，首先问"哪个不变量被破坏了？"——这比直接问"哪个组件有 bug?"更高效，因为不变量定义了系统的"合法状态空间"，任何偏离合法状态的行为都可以被精确定位。`insights/04-design-decision-and-validation.md` 的最小验证矩阵（allocator 正确性→prefix cache→graph 边界→offload→PD→性能压测）提供了从不变量到系统级正确性的逐层验证路线。

**面试回答技巧：** 回答本题时，建议采用"推导→分类→判据→验证"的四步结构——先从三层池模型推导不变量集合（展示第一性原理思维），再按"立即崩溃 vs 静默错误"分类（展示工程判断力），然后给出每个静默错误的观测信号（展示可证伪性），最后描述验证实验设计（展示实践能力）。这个结构本身就是一个"专家级回答"的模板——它证明候选人不仅能理解单个不变量，还能建立系统化的不变量思维框架。

**分层评分标准：** 4分：能列出三层池的基本不变量；6分：能区分立即崩溃与静默错误，给出可观测的判定信号；8分：能从第一性原理推导不变量集合，分析 overlap/graph 路径下的 WAR race 条件，并给出修复策略的优先级。10分：能设计一套不变量的运行时验证框架——在不停止服务的前提下，通过后台线程周期性检查 `free + release + evictable + protected = capacity`、`lock_ref` 与 active request 的一致性、radix tree 节点的 page alignment，并将检查结果作为 metrics 暴露——这是"自愈系统"的基础设施层。

**证据与来源：** `sglang/90-cross-module/pooling-and-resource-management.md`（三层池模型与所有权表、CUDA Graph 显存边界）；`sglang/01-modules/M08-kv-cache/README.md`（lock_ref/page alignment/paged allocator）；`insights/02-kv-pool-lifecycle-and-ownership.md`（lifecycle 表与故障树）；`sglang/01-modules/M09-attention-cuda-graph/README.md`（graph 不变量与 metadata 拆分）；`sglang/01-modules/M04-scheduler-batching/README.md`（retraction 流程中的 ownership 闭环）；`insights/04-design-decision-and-validation.md`（allocator 正确性 oracle）。

### I-E002 从性能建模角度：CUDA Graph 的收益何时递减甚至为负？给出可证伪的量化判据。

**元数据：** 知识域：`I07 CUDA Graph 收益建模`；能力目标：`C07 从 roofline 与开销分解推导收益拐点`；题型：量化建模与可证伪判据；频率：高频；前置/后续：`I-B001`（双阶段差异）、`I-E001`（不变量）；证据状态：源码 C，性能数字 U。

**背景知识：** CUDA Graph 把形状稳定的一段 GPU 操作预先录制，后续用固定 buffer 重放，消除 per-launch 的 CPU overhead 和 kernel launch pipeline 的填充延迟。SGLang 的 decode graph 和 prefill graph 使用独立 runner、bucket 和生命周期。`sglang/01-modules/M09-attention-cuda-graph/README.md`。graph eligibility 会检查 ForwardMode、batch size 是否在 capture bucket 内、padding waste 是否超阈值、是否有 dynamic embedding/replace embed、speculative width 是否匹配、LoRA/DSA variant 是否有对应 graph key。不满足条件时回退 eager 是设计行为。Decode graph 与 prefill graph 不仅独立 capture，还使用不同的 eligibility 策略——decode graph 要求 fixed decode width 和 batch size 在 capture bucket 内，prefill graph 则允许 `load_batch` 把实际 token 数向 capture bucket 填充，但不超 padding waste 阈值时仍可 replay。`sglang/01-modules/M09-attention-cuda-graph/README.md` 的 prefill graph 段落。

**问题分析：** 1) 建立 CUDA Graph 的收益模型——从开销分解角度量化 graph 替代了什么、保留了什么；2) 推导收益递减的充分条件——per-launch overhead 占比下降时 graph 的边际收益趋零；3) 给出可证伪的量化判据：什么测量数据能证明"在当前配置下 graph 已经不值得"。

**具体答案：** 面试中可以这样回答：

> CUDA Graph 的收益来自消除 per-launch overhead。设 eager 单次 decode step 时间为 `T_eager = T_launch + T_kernel`，graph replay 时间为 `T_graph = T_replay + T_kernel'`，其中 `T_kernel' ≈ T_kernel`（graph 内 kernel 可能因固定地址略快）。单次 step 的 graph 收益 `Δ = T_eager - T_graph ≈ T_launch - T_replay`。
>
> 收益递减的充分条件是 `T_launch / T_kernel → 0`。这发生在：
> 1. **大 model + 长 sequence**：`T_kernel` 随模型大小和 KV cache 读取量增长，而 `T_launch` 基本固定——大模型的 kernel 时间远超 launch 开销，graph 的边际收益趋近于零。
> 2. **大 batch decode**：batch 增大使 `T_kernel` 增长（更多 token 参与 attention/FFN），`T_launch` 不变，收益比 `Δ/T_eager` 下降。
> 3. **graph fallback 频繁**：当 `can_run_graph` 频繁返回 false（dynamic embedding、speculative ragged verify、padding waste 超阈值），capture 和维护 graph 的固定成本（显存占用、capture 时间、warmup 开销）分摊到的 replay 次数减少。
>
> **收益为负的充分条件**：graph 的固定成本（显存 + capture 时间）超过累计 launch 节省。公式：`N_replay × (T_launch - T_replay) < C_capture + C_mem`。其中 `C_capture` 是 warmup + autotune + capture stream 的时间，`C_mem` 是 global graph memory pool 占用的显存。`sglang/90-cross-module/pooling-and-resource-management.md`。
>
> **可证伪判据**：固定模型、硬件、batch 分布，测量 eager vs graph 的 TPOT 差值 `δ`。若 `δ < ε`（如 ε=2% TPOT 改善），且 graph 显存开销 > 5% HBM，则结论为"当前配置下 graph 净收益为负"。此判据可通过关闭 graph 前后对比吞吐/延迟/P99 来验证。

**技术洞察：**

```text
# 收益模型
benefit_per_step = T_launch_eager - T_replay_graph
relative_benefit = benefit_per_step / T_eager

# 收益递减条件（ridge point 类比）
T_kernel grows with: model_size × seq_len × batch_size
T_launch ~ constant (CPU dispatch overhead)

→ relative_benefit ∝ 1/T_kernel

# 净收益为负条件
N_replay × benefit_per_step < C_capture + C_mem × opportunity_cost

# 可证伪判据
falsify_if: (TPOT_eager - TPOT_graph) / TPOT_eager < 0.02
          ∧ graph_mem_overhead / HBM_total > 0.05
          ∧ fallback_rate > 0.1
```

C：`sglang/01-modules/M09-attention-cuda-graph/README.md` 的 eligibility 条件列表；`sglang/90-cross-module/pooling-and-resource-management.md` 的 graph memory pool 描述；`sglang/01-modules/M04-scheduler-batching/README.md` 的 `run_batch` 到模型执行路径。I：收益模型与判据推导——关键 insight 是 graph 的收益不是绝对值而是相对值，`T_launch / T_kernel` 比值决定了 graph 的边际收益；当 kernel 时间增长（大模型/大 batch），比值趋零，graph 退化为纯成本中心。另一个关键 insight 是 graph 的显存开销在显存紧张时产生间接成本——`global_graph_memory_pool` 占用 HBM 但不服务请求，可能导致更频繁的 retraction，间接降低吞吐。U：具体的 ε 阈值需 workload-specific benchmark 确定。

**图文说明：** 横轴为 `T_kernel / T_launch` 比值（随模型/序列/batch 增长），纵轴为 `relative_benefit` 的递减曲线。标注三个区：左区（小模型/小 batch，graph 收益显著）、中区（中等配置，收益递减）、右区（大模型/大 batch/高频 fallback，收益趋零或为负）。竖线标注 ridge point——具体位置因硬件/模型/量化方案而异，实测需用 profiler 采样。

**拓展知识：** 与 I-E009 衔接：graph 显存开销在安全层面属于资源消耗型攻击面——攻击者可通过大量并发请求挤占 HBM，使 graph memory pool 的显存被挤压，间接减少可用于服务的 KV 容量；与 I-E012 衔接：TP/DP 并行度增加时每个 rank 的 `T_kernel` 下降，graph 相对收益上升——这是 graph 与并行的正交互。与 I-E006 衔接：长上下文推理中 CUDA Graph 的 capture bucket 按 seq_len 分桶——超长序列可能超出最大 capture bucket，导致 graph 无法覆盖所有 decode 长度，频繁 fallback 到 eager。这意味着长上下文场景下 graph 的收益天然受限于 bucket 设计。

**常见追问及回答要点：**

1. **问：prefill graph 与 decode graph 的收益曲线一样吗？** 答：不一样。prefill 的 `T_kernel` 远大于 decode（大 GEMM），所以 prefill graph 的 relative_benefit 通常更小。但 prefill 的 `T_launch` 也更大（更多 kernel），所以绝对收益可能相近。prefill graph 还受 padding waste 约束更强——aggregate bucket 拆分后实际 token 数与 capture bucket 不一致时 waste 更大。`sglang/01-modules/M09-attention-cuda-graph/README.md` 的 prefill graph 段落。
2. **问：如何实测 fallback rate？** 答：通过 scheduler metrics 记录 `can_run_graph=True` 与 `False` 的 batch 数分布，按 fallback 原因分类（padding waste / dynamic embedding / speculative / LoRA variant）。高 fallback 率意味着 capture 成本分摊不足。
3. **问：CUDA Graph 对 P99 延迟的影响是什么？** 答：graph replay 消除了 launch 的 CPU 侧抖动（driver scheduling、kernel validation），使 step 时间更稳定——这是 graph 对尾延迟的隐藏收益，但在 `T_kernel` 很大的场景下，这种稳定性收益可能不足以抵消显存开销。实测中应区分 "graph 减少 TPOT 方差" 和 "graph 减少 TPOT 均值"——前者改善 P99，后者改善 P50，两者可以独立变化。
4. **问：warmup/autotune 对 graph 收益的影响？** 答：`DecodeCudaGraphRunner.capture` 先 warmup/autotune，根据结果重新决定 torch.compile bucket；然后恢复 capture-time buffer。warmup 的质量决定了 graph 内 kernel 是否接近最优——若 warmup 的 batch 分布与生产分布偏差大，graph 内 kernel 可能未针对实际 shape 优化，`T_kernel'` 反而大于 eager 的 auto-tuned kernel。

**易错点与反例：**

- 反例："CUDA Graph 总是减少延迟"——错误。graph 只消除 launch overhead，不改变 kernel 执行时间；若 `T_kernel >> T_launch`，graph 对端到端延迟几乎无改善。
- 反例："graph 开启等于实际 replay"——错误。每个 batch 都要通过 mode、bucket、variant 和 backend eligibility 检查；fallback 到 eager 是常态而非异常。`insights/05-sglang-inference-technical-insights.md` 常见错误心智模型。
- 易错：忽略 graph 显存开销——global graph memory pool 占用 HBM 但不服务请求，在显存紧张时可能触发更频繁的 retraction，间接损害吞吐。

**实践与验证：** [U] 固定模型与硬件，分别测量 eager 和 graph 模式下的 TPOT/P99/fallback_rate，绘制 `relative_benefit` 随 batch_size 变化的曲线。控制温度、时钟与后台负载。oracle：小 batch 下 graph 收益应显著（>5% TPOT 改善），大 batch 下收益趋近于零。当前未执行 GPU 实验。

**分层评分标准：** 4分：能说明 CUDA Graph 消除 launch overhead 的基本原理；6分：能推导收益递减的充分条件，给出量化公式；8分：能给出可证伪的判据，分析 prefill vs decode 的非对称收益，并讨论 graph 显存开销的间接影响（retraction 率、P99）；10分：能结合具体硬件参数（HBM bandwidth、kernel launch latency、L2 cache size）做数值化的收益预估，并给出"在目标硬件和 workload 下是否开启 graph"的决策流程——这需要从 roofline 分析出发，结合 `T_launch` 的 profiling 数据和 graph memory pool 的 footprint 计算净收益。

**证据与来源：** `sglang/01-modules/M09-attention-cuda-graph/README.md`（eligibility 与 capture 生命周期）；`sglang/90-cross-module/pooling-and-resource-management.md`（graph memory pool 与显存开销）；`insights/05-sglang-inference-technical-insights.md`（insight 四 CUDA Graph 协商模型）；`sglang/01-modules/M04-scheduler-batching/README.md`（run_batch 到模型执行的 forward path 选择）；`cuda/99-roadmap/performance-guide.md`（stream 并发与异步边界——graph replay 的异步语义）。

**多 variant 场景的 graph 管理：** 当部署同时服务多个 LoRA adapter 或多种 speculative 配置时，graph key 按 batch/stream/variant 区分——每个 (batch_size, LoRA_id, speculative_width) 组合都需要独立的 capture。variant 组合爆炸会使 graph capture 时间和显存占用急剧增加。SGLang 的 `DecodeCudaGraphRunner.capture` 按大 batch 到小 batch 的逆序遍历 capture buckets，每个 bucket 还可能按 LoRA variant 和 DSA dense/sparse variant 各捕获一份图。`sglang/01-modules/M09-attention-cuda-graph/README.md` 的 capture 段落。在多租户场景下，应在 variant 数和 graph 显存开销之间取舍——可以只 capture 最常用的 top-K variant，其余回退 eager。

**与 I-E005 的关系：** PD 分离架构中 prefill 节点和 decode 节点独立管理各自的 CUDA Graph——decode 节点的 graph 只需覆盖 decode shape（batch size × decode width），不需要 prefill 的 aggregate token bucket。这使得 decode 节点的 graph 更轻量、capture 更快、variant 更少。

**capture 失败的降级策略：** CUDA Graph capture 可能因显存不足（warmup 时 allocate 的 buffer 超出 HBM 预算）或 backend 不支持（如 TRT-LLM MLA 在 DCP+speculative 组合下拒绝 capture）而失败。`sglang/01-modules/M09-attention-cuda-graph/README.md` 的 backend registry 提到某些 backend 在特定组合下直接拒绝——这不是 bug 而是能力协商。生产系统应实现分级降级：首选 graph replay → 回退 eager → 若 eager 也失败则 retract 请求。监控 capture 成功率和 fallback 比例的 trending 是容量规划的早期预警信号。

**graph replay 与自定义 memory pool 的交互：** SGLang 的 KV cache 可能在 `torch.cuda.use_mem_pool(custom_mem_pool)` 区域创建 buffer——CUDA Graph 的 static buffer 也通过 `global_graph_memory_pool` 管理。`sglang/90-cross-module/pooling-and-resource-management.md` 的 CUDA Graph 与自定义 memory pool 边界段落。这两个 pool 的生命周期独立：graph pool 在 runner teardown 时释放，KV pool 在 flush 时释放。若在 graph 仍在用时 flush KV pool（非 idle 状态强清），可能导致 graph replay 读取已释放的 KV 地址——这是 I-E001 中不变量 INVARIANT_5 的运行时破坏。正确的顺序是：先 drain 所有 in-flight request → 确认 graph 不再 replay → flush KV pool → teardown graph runner。

**piecewise CUDA Graph 与 traditional graph 的差异：** SGLang 的 `PiecewiseCudaGraphRunner` 将 graph capture 分成多个 segment——在需要动态决策的点（如 sampling）前后分段录制，中间插入 eager execution。`sglang/01-modules/M09-attention-cuda-graph/README.md` 提到了 piecewise runner 的存在。这本质上是对 graph 的"收益递减"问题的妥协：全量 graph 收益高但 eligibility 限制多，piecewise graph 放宽了 eligibility（允许在分段点插入动态操作）但收益降低（只有部分 kernel 被录制）。选择 traditional 还是 piecewise 取决于 workload 中 dynamic embedding / speculative / LoRA switching 的频率——高频场景 piecewise 的总收益可能反而更高（因为 fallback 减少）。

**CUDA Graph 与 custom all-reduce 的兼容性：** TP 的 custom all-reduce（如 PyTorch 的 `all_reduce` 优化）在 decode 的 all-reduce 中使用 symmetric memory 操作——这些操作在 CUDA Graph capture 期间需要特殊的 address 固定。`sglang/01-modules/M20-parallel-strategies/README.md` 的 TP collective 段落。若 custom all-reduce 的 buffer 地址在 replay 期间改变，graph 内的 all-reduce 将写入错误地址。这要求 custom all-reduce 的 buffer 要么在 capture 前预分配并固定，要么在 graph 外执行——后者使 graph 只覆盖 attention + FFN，all-reduce 走 eager。

**CUDA Graph 与 overlap schedule 的交互：** 在 overlap 模式下，当前 batch 的 GPU forward 与上一批的 CPU result processing 交错——如果上一批的 result 处理涉及 CUDA Graph buffer 的释放或修改，可能形成 WAR race。`sglang/01-modules/M04-scheduler-batching/README.md` 的 overlap loop 段落。SGLang 通过 backend 的 `SharedReadEnds` 声明来保护——decode/verify 模式可以指向 replay 内的读结束点，其他模式采用保守的 fence。`sglang/01-modules/M09-attention-cuda-graph/README.md`。这意味着 CUDA Graph 的固定地址约束（INVARIANT_5）在 overlap 模式下需要额外的 lifetime 管理——graph 内引用的 buffer 必须等到"共享读结束"后才能释放。

**预判 graph 收益的决策树：** 对于一个新的部署配置，可以用以下决策树预判 CUDA Graph 是否值得开启：(1) 模型大小——7B 及以下建议开启（launch overhead 占比大），70B 以上需实测；(2) workload 动态性——若 speculative + LoRA + dynamic embedding 组合使用，fallback 率高，可能不值得 capture 大量 variant；(3) 显存余量——HBM 使用 > 85% 时 graph memory pool 的额外开销可能触发 retraction。这个决策树可以直接转化为 production 的自动化配置推荐。

**graph 收益的量化 profiling 方法：** 要在生产环境中精确评估 CUDA Graph 的收益，推荐以下 profiling 流程：(1) 使用 Nsight Systems 采集 1000 个 decode step 的 trace，分离 `T_launch`（host→device dispatch 时间）和 `T_kernel`（device 执行时间）；(2) 计算 `T_launch / T_eager` 比值——若 < 2%，graph 不值得开启；(3) 采集 graph 模式下的 trace，对比 `T_replay + T_kernel'` 与 `T_eager`；(4) 计算显存开销——`torch.cuda.memory_snapshot()` 在 graph capture 前后的差值即为 `C_mem`。这三个数据点（launch 占比、replay 节省、显存开销）构成了 graph 收益的完整画像。`cuda/99-roadmap/performance-guide.md` 的 profiler 注意事项。

**I-E002 的可证伪论断总结：** "CUDA Graph 的收益在 `T_launch / T_kernel < 2%` 时递减至零"——这是一个可证伪的论断，因为 `T_launch` 和 `T_kernel` 都可以通过 profiling 直接测量。证伪条件：若 profiling 显示 `T_launch / T_kernel < 2%` 但 graph 仍带来 > 5% 的 TPOT 改善，则论断被证伪——可能原因是 graph 内 kernel 因固定地址的 cache 局部性而显著快于 eager（`T_kernel' << T_kernel`），这种情况下 graph 的收益来自 kernel 加速而非 launch 消除。这提醒我们：收益递减的判据应同时考虑分子（launch 占比）和分母（kernel 加速效应）。

**与 piecewise graph 的关系：** SGLang 的 `PiecewiseCudaGraphRunner` 在动态决策点（如 sampling）前后分段录制——这放宽了 eligibility 但降低了收益。`sglang/01-modules/M09-attention-cuda-graph/README.md`。Piecewise graph 的收益曲线介于 full graph 和 eager 之间——在 fallback 率高的场景下，piecewise 的总收益可能高于 full graph（因为 fallback 到 eager 的次数减少），但单次 replay 的收益低于 full graph（因为部分 kernel 仍是 eager）。这是一个"收益-鲁棒性"的权衡——production 系统应在两者间选择使总吞吐最大的方案。E002 的可证伪判据也应区分 full graph 和 piecewise graph 的 fallback rate——若 piecewise 的 fallback rate 比 full graph 低 50% 以上，则 piecewise 的净收益可能更高。

### I-E003 证明 speculative decoding 的语义等价性：什么条件下 speculative 输出与标准采样分布一致？

**元数据：** 知识域：`I11 Speculative decoding 正确性`；能力目标：`C11 从分布论推导语义等价条件`；题型：数学证明与条件分析；频率：中频；前置/后续：`I-B002`（speculative decoding 概念）、`I-E001`（不变量）；证据状态：源码 C，数值验证 U。

**背景知识：** Speculative decoding 的核心承诺：draft 产生候选，target 验证后提交，最终输出分布应与标准 target 采样一致。SGLang 将其建模为事务协议——`BaseCommitted → Proposed → Verifying → Decided → Committing → BaseCommitted`。提交不变量：`visible_output_len == committed_target_token_len == grammar_consumed_len == request_valid_kv_len`。`insights/06-speculative-decoding-as-transaction.md`。不同算法使用 greedy match、rejection sampling、MTP/EAGLE、ngram、DFlash 等 proposal/verification 方式。`sglang/01-modules/M11-speculative-decoding/README.md`。

**问题分析：** 1) 形式化定义"语义等价"——speculative 输出序列的联合分布等于标准 target 采样分布；2) 从 rejection sampling 的接受-拒绝机制推导等价的充要条件；3) 分析各算法（greedy/sampling/MTP/EAGLE）在什么条件下满足、什么条件下违反等价性。

**具体答案：** 面试中可以这样回答：

> 语义等价的严格定义：设 target 模型在上下文 c 上的采样分布为 P(x|c)，speculative 解码生成的 token 序列 `x_1, ..., x_n` 的联合分布为 Q(x_1...x_n)。等价意味着 `Q = ∏ P(x_i | x_<i)`（即标准自回归分解）。
>
> **定理（Leviathan et al. 2023, Chen et al. 2023）：** rejection sampling框架下，若接受规则为"以概率 `min(1, P(x)/Q(x))` 接受 draft 候选 x"（Q 为 draft 分布），则最终接受的 token 的边缘分布精确等于 P。证明：接受概率密度 `∝ Q(x) · min(1, P(x)/Q(x)) = min(Q(x), P(x))`；标准化后恰好归一化为 P(x)。此证明不依赖 draft 模型质量——无论 Q 与 P 差距多大，接受边缘分布都精确等于 P。
>
> **等价成立的条件：**
> 1. **Greedy decoding**：argmin 等价于 argmax 概率——若 draft 候选 `x* = argmax Q(x)` 且 target 验证 `x* = argmax P(x)`，则两者输出 identical。此等价性不依赖 draft 质量，只依赖 target 的 argmax 一致性。
> 2. **Rejection sampling with temperature/top-k/top-p**：只要 target 的 logits transform（temperature scaling、top-k/top-p filtering）在验证时完整重算，且接受概率使用变换后的 P 和 Q，等价性成立。
>
> **等价失效的条件：**
> 1. **MTP/EAGLE 等训练型 draft**：draft 模型的训练目标是最小化 KL divergence(Q||P)，但不等价于保证接受后的边缘分布等于 P。训练型 draft 可能在 argmax 上与 target 高度一致（高 acceptance rate），但 sampling 模式下若跳过显式的 rejection sampling 接受步骤而直接 greedily match，输出分布会偏向 draft 的分布 Q。
> 2. **Speculative + Grammar**：grammar mask 在 draft 和 target 分步应用时，若 draft 在 grammar-restricted 子集上采样而 target 在全空间验证，接受概率计算的基础分布不一致，等价性被破坏。`insights/06-speculative-decoding-as-transaction.md` 的 grammar 约束段落。
> 3. **Penalty/RNG 状态错位**：repetition penalty 依赖已提交 token 历史，seeded sampling 依赖随机数消费顺序。若 reject 后 penalty/RNG 状态被错误推进，后续 token 的采样分布发生偏移。
>
> **可证伪验证**：固定 seed 和 greedy 参数，普通 decode 与 speculative decode 生成相同长度，逐 token 比较。Greedy 下应严格相等；sampling 下应使用统计检验（卡方检验或 KL divergence）比较输出分布。若 greedy 模式下发现 token 不等，则 speculative 实现的 commit 路径存在 bug；若 greedy 相等但 sampling 分布偏差，则 rejection sampling 的接受概率计算有误。

**技术洞察：**

```text
# 语义等价的形式化
Standard:  Q_std(x_1...x_n) = ∏_{i=1}^{n} P(x_i | x_<i, c)
Speculative: Q_spec(x_1...x_n) = proposal_distribution × acceptance_filter

# Rejection sampling 等价证明
accept_density(x) ∝ Q_draft(x) · min(1, P_target(x) / Q_draft(x))
                 = min(Q_draft(x), P_target(x))
normalizer = ∫ min(Q(y), P(y)) dy
accepted_marginal(x) = min(Q(x), P(x)) / normalizer  ≠ P(x)  ← 错误！

# 修正：正确的 rejection sampling 接受规则
accept_prob(x) = min(1, P(x)/Q(x))   # 以概率 accept
reject_prob(x) = 1 - accept_prob(x)  # 拒绝后从 P(x) - min(P,Q) 重采样
→ accepted_marginal(x) = P(x)  ✓  （经重采样修正后）

# 失效判据
¬equivalent ← greedy_match_without_rejection_sampling
             ∨ grammar_mask_mismatch_between_draft_and_target
             ∨ penalty/RNG_desync_after_reject
```

C：`insights/06-speculative-decoding-as-transaction.md` 的事务模型与 grammar 约束；`sglang/01-modules/M11-speculative-decoding/README.md` 的算法列表；Leviathan et al. 2023 的 rejection sampling 等价性定理。I：rejection sampling 等价性证明概要——关键步骤是拒绝后从残差分布 `P(x) - min(P(x), Q(x))` 重采样 replacement token，缺少此步则等价性不成立。U：实际输出分布偏差需大样本统计验证。

**图文说明：** 状态机图——从 `BaseCommitted` 出发，draft 产生候选（虚线箭头，标注 draft 分布 Q），target 验证（实线箭头，标注 target 分布 P），acceptance 决策节点分为两支：accept（提交，分布 = P）或 reject（回滚/重标注 replacement）。在 reject 分支上标注关键条件：replacement token 必须从 P 的重采样残差分布中抽取，否则输出分布偏移。

**拓展知识：** 与 I-E004 衔接：speculative decode 使用 quantized KV cache 时，draft 和 target 从同一量化 KV 读取，量化误差在 verify 中被放大；与 I-E001 衔接：speculative 的 committed_len 不变量是语义等价的必要非充分条件。

**常见追问及回答要点：**

1. **问：MTP/EAGLE 的 acceptance rate 很高（如 90%+），为什么还可能分布不等价？** 答：高 acceptance rate 只说明 draft 与 target 的 argmax 或 top 区域高度重叠，但 rejection sampling 的边缘分布等价性依赖于接受概率的精确计算。若实现中用 greedy match（即"逐个比较 draft 与 target argmax 直到不一致"）而非显式概率接受，则 acceptance rate 高 ≠ 分布等价。greedy match 是 rejection sampling 在 greedy 模式下的正确实现，但在 sampling 模式下需要完整的概率比计算。
2. **问：bonus token 如何影响等价性？** 答：speculative decode 在最后 accepted token 之后可能追加一个 bonus token（来自 target 的完整前向）。该 bonus token 从 P(x|c + accepted) 采样，与标准自回归的下一步采样分布一致——因此 bonus token 不破坏等价性，反而是维持等价的必要步骤（补全 accepted 长度 + 1 的分布）。
3. **问：decoupled speculative（draft 与 target 分离进程）如何保持等价性？** 答：IPC 只传递候选 token 和 committed length，不传递 draft 分布 Q。target verify 仍在本地使用完整的 P 和 Q 计算接受概率，等价性条件与同进程一致。风险在于 IPC 延迟导致 timeout 后 abort——这破坏可用性但不破坏已提交部分的正确性。`insights/06-speculative-decoding-as-transaction.md` 的 decoupled 段落。
4. **问：logprob 在 speculative decode 中应返回哪个分布的 log probability？** 答：这是语义等价的灰色地带。严格来说，rejection sampling 保证的是"接受的 token 来自 P"，但 logprob 返回的是 `log P(x)`（target 分布）还是 `log Q(x)`（draft 分布）取决于实现。正确的工程实践是返回 `log P(x)` 并在 rejection 点标记——因为用户关心的是"target 模型认为这个 token 的概率是多少"。

**易错点与反例：**

- 反例："speculative decoding 只是加速，不影响输出"——错误。若实现中 reject 后 penalty/RNG 状态错位，或 grammar mask 在 draft/target 不一致，输出分布会偏移。这直接关联 I-E004——KV 量化误差导致 attention 分布偏移时，speculative 的 acceptance rate 下降，进一步减少 speculative 的收益。两者叠加可能使 speculative 在长序列量化场景下完全失效。
- 反例："acceptance rate = 100% 意味着输出一定等价"——错误。100% acceptance 可能是 draft 完全拟合 target（Q=P），也可能是 acceptance 阈值过于宽松（接受概率未精确计算）。
- 易错：混淆"per-token 接受概率"与"block acceptance rate"——语义等价要求每个被提交 token 的边缘分布等于 P，block-level acceptance rate 是效率指标，不等于分布等价。

**实践与验证：** [U] 固定模型、prompt、seed，greedy 模式下比较普通 decode 与 speculative decode 的逐 token 相等性；sampling 模式下采集大样本，比较输出的 unigram/bigram 分布。oracle：greedy 下 token-by-token 严格相等；sampling 下 KL divergence 低于阈值。`insights/06-speculative-decoding-as-transaction.md` 的 14 节最小 correctness oracle。当前未执行 GPU 实验。

**分层评分标准：** 4分：能说明 speculative decoding 的基本流程与 target 验证机制；6分：能从 rejection sampling 推导采样等价的充要条件，分析 greedy 与 sampling 的不同；8分：能形式化证明等价性定理，推导失效条件，并给出可证伪的数值验证方案。

**证据与来源：** `insights/06-speculative-decoding-as-transaction.md`（事务模型与 grammar/penalty 约束）；`sglang/01-modules/M11-speculative-decoding/README.md`（算法矩阵）；Leviathan et al. 2023 "Fast Inference from Transformers via Speculative Decoding"（原始等价性证明）。

**形式化验证的局限：** 即使 rejection sampling 的数学证明成立，实现中仍可能因数值精度问题偏离等价性——FP16 下的 `P(x)/Q(x)` 比值在接近 1 时可能因浮点误差导致错误的 accept/reject 决策。实践中应使用对数空间计算 `log P(x) - log Q(x)` 避免除法。`insights/06-speculative-decoding-as-transaction.md` 的 grammar 与 sampling 段落讨论了数值正确性与算法正确性的分离——"数学等价"不等于"bitwise 等价"，实现必须在可接受的数值误差内保持语义等价。

**speculative 与 CUDA Graph 的负交互：** speculative verify 比普通 decode 更难 graph 化——每请求 candidate width 可能不同、accepted length 是运行后才知道的动态值、graph capture width 与 live proposal width 必须兼容。`sglang/01-modules/M09-attention-cuda-graph/README.md` 的 ragged verify 段落。当 graph eligibility 因 speculative width 不匹配而返回 false 时，verify 走 eager——这在高 acceptance rate 场景下是可接受的（大多数 token 来自 draft，verify 开销本身小），但在低 acceptance rate 场景下 eager verify 可能成为瓶颈。

**temperature 对等价性的微妙影响：** temperature scaling 在 logits 变换中改变 token 概率分布。在 rejection sampling 中，`P(x)/Q(x)` 的比值需要在**变换后**的分布上计算——即 temperature-scaled P 和 temperature-scaled Q。若 draft 模型使用与 target 不同的 temperature（或 draft 不使用 temperature），`P(x)/Q(x)` 的比值计算基础不一致，等价性被破坏。工程上应确保 draft 验证时使用 target 的完整 logits transform pipeline——包括 temperature、top-k/top-p filtering 和 grammar mask——而不仅仅是 argmax 比较。

**多轮 speculative 的累积分布偏差：** 在 k-token 的 speculative 中，第 i 个 token 的接受决策依赖于前 i-1 个 token 都被接受——这是一个级联概率。若每轮的 acceptance probability 为 `p_i`，则 k 个 token 全部接受的概率为 `∏p_i`。在 sampling 模式下，rejection sampling 保证的是"每个被接受的 token 边缘分布 = P"，但连续多轮的联合分布是否等于标准自回归的联合分布 `∏P(x_i|x_<i)` 取决于 replacement token 是否从正确的条件分布 P(x_i | x_<i) 中采样。理论上 rejection sampling 保证这一点，但数值误差在多轮级联中可能累积——这是语义等价在长 block 下的实际边界。

**I-E003 的专家级思考框架：** 本题的核心是"从算法证明到系统实现"的鸿沟。数学上 rejection sampling 保证了语义等价，但系统实现中的三个鸿沟可能导致偏离：(a) 数值精度——FP16 下的 `P(x)/Q(x)` 计算误差；(b) 状态同步——reject 后 penalty/RNG 状态的回滚完整性；(c) 复合约束——grammar mask 在 draft 和 target 的不同应用顺序。专家级的回答应该不仅给出等价成立的条件（数学证明），还要给出等价失效的条件（实现边界），并提供可证伪的实验设计——这正是"专家题必须从原理推导非罗列事实"的要求。

**I-E003 的扩展——n-gram speculative 的等价性：** n-gram speculative 使用历史 token 的重复模式作为 draft——它不依赖小模型，而是在已生成文本中查找匹配的 n-gram 作为候选。其等价性条件更简单：n-gram draft 的候选是从已提交文本中提取的，不是从任何模型采样的——因此它天然是 target 的条件概率的一个近似。只要验证时使用 target 的完整 logits transform（包括 temperature、top-k/top-p、grammar），rejection sampling 的等价性仍然成立。n-gram 的优势是零额外计算成本（draft 只是字符串匹配），但劣势是 acceptance rate 依赖文本的重复度——创意写作场景低，代码/模板场景高。

**speculative 与 logprob 返回的语义一致性：** 当用户请求 logprob 输出时，speculative decode 应返回 target 模型（P）的 log probability 而非 draft 模型（Q）的。这是因为 rejection sampling 保证了"接受的 token 边缘分布 = P"，所以 `log P(x)` 是正确的一致性度量。若返回 `log Q(x)`，用户可能观察到同一 token 在不同 run 中的 logprob 不同（因为 Q 是 draft 的近似分布），导致困惑。实现中应在 verify 完成后使用 target logits 计算 logprob——这增加了 verify 的 compute 开销（需要保存完整的 target logits 而非仅 argmax），但对正确性是必要的。

**I-E003 的终极可证伪实验：** 对于任意 speculative decoding 实现，终极验证方法是"分布等价性测试"——在相同 seed 下分别运行普通 decode 和 speculative decode，采集 10000 个 token 的输出，使用 Kolmogorov-Smirnov 检验比较两个样本的 unigram 分布。若 p-value > 0.05，则接受"分布等价"假设；若 p-value < 0.01，则拒绝等价假设并定位到具体的 distribution shift。这个测试的统计功效（power）与样本量成正比——生产验证应使用足够大的样本量以确保检测灵敏度。

### I-E004 从误差传播角度：KV 量化误差如何在自回归解码中累积？何时会导致输出质量崩溃？

**元数据：** 知识域：`I12 KV Cache 量化与误差传播`；能力目标：`C12 从数值分析推导误差累积上界`；题型：数学推导与临界分析；频率：中频；前置/后续：`I-B003`（KV cache 概念）、`I-E003`（speculative 误差放大）；证据状态：原理 C，数值 U。

**背景知识：** KV cache 存储每层每 token 的 key/value 向量。KV 量化（FP8/INT8/INT4）将 `head_dim` 维的 K/V 从 FP16/BF15 压缩到低精度表示，减少 decode 阶段的显存占用和带宽需求。decode 阶段的算术强度 `I_decode ≈ FLOPs_decode / KV_cache_bytes ∝ 1/seq_len`，KV 量化直接降低分母。`sglang/01-modules/M08-kv-cache/README.md` 提到 FP8 将存储 dtype 改为 uint8。PagedAttention 使用 block table 映射逻辑/物理 block。`insights/arxiv-pdf/2309.06180-pagedattention-vllm.md`。

**问题分析：** 1) 建立 KV 量化误差的数学模型——量化噪声在 attention score 计算中的传播路径；2) 推导误差在自回归解码中的累积方式——每步 attention 对历史 KV 的读取都会引入量化误差；3) 给出质量崩溃的临界条件——量化精度、序列长度、模型深度三者的关系。

**具体答案：** 面试中可以这样回答：

> 设 KV cache 第 l 层第 t 个 token 的精确值为 `(K_{l,t}, V_{l,t})`，量化后为 `(K̂_{l,t}, V̂_{l,t}) = (K_{l,t} + ε^K_{l,t}, V_{l,t} + ε^V_{l,t})`，其中 `ε` 为量化误差，界为 `‖ε‖ ≤ δ · ‖K‖`（δ 为相对量化精度，FP8 约 1%/INT8 约 0.5%/INT4 约 6%）。
>
> **误差传播路径**——attention 计算中，query Q_t 与所有历史 key K̂_{l,≤t} 做点积得到 score s_i = Q_t · K̂_{l,i} = Q_t · K_{l,i} + Q_t · ε^K_{l,i}。量化误差在 softmax 非线性后被放大：当两个 token 的真实 score 接近（`s_i ≈ s_j`）时，误差 `Q_t · (ε^K_i - ε^K_j)` 可能导致 softmax 概率排序翻转。
>
> **累积机制**——自回归解码第 t 步的输出由 attention output `o_t = Σ softmax(s_i) · V̂_{l,i}` 决定。误差来源包括两部分：(a) score 计算中的 key 量化误差影响权重分布；(b) weighted sum 中的 value 量化误差直接影响输出值。随 t 增长，参与 attention 的 token 数增加，误差在 weighted sum 中积累。
>
> **临界条件推导**——设 attention 分布的熵为 H(p)，量化误差导致的 score 扰动为 Δs。当 `Δs > min_{i≠j} |s_i - s_j|` 时，softmax 排序可能翻转。定义临界序列长度 `L_c` 为：
>
> ```text
> L_c ≈ 1 / (δ · ‖Q‖ · √d_head)   # d_head 为 head 维度
> ```
>
> 当实际序列长度 `L > L_c` 时，量化误差导致至少一个 token 的 attention weight 显著偏移的概率超过阈值。对于 FP8（δ≈1%），`L_c` 约为数千 token；对于 INT4（δ≈6%），`L_c` 仅为数百 token。
>
> **质量崩溃的实证信号**：输出 perplexity 在特定长度后突然上升（而非渐变），表明 attention 的"长距离依赖"能力因量化误差而断裂。这种突变行为可以理解为 softmax 的"相变"——当量化误差积累到超过 attention score 的最小间隔时，softmax 的概率分布从"集中"跳变为"分散"，输出质量随之崩溃。

**技术洞察：**

```text
# 量化误差模型
K̂ = K + ε_K,  ‖ε_K‖ ≤ δ · ‖K‖
V̂ = V + ε_V,  ‖ε_V‖ ≤ δ · ‖V‖

# Attention score 误差
s_i = Q · K_i
ŝ_i = Q · K̂_i = s_i + Q · ε_K_i
Δs_i = Q · ε_K_i  ≤ ‖Q‖ · δ · ‖K‖

# 排序翻转条件（两 token 竞争）
flip_prob ≈ Pr[Δs_i - Δs_j > s_j - s_i]
         ≈ Φ((s_i - s_j) / (δ · ‖Q‖ · ‖K‖ · √2))

# 累积上界（简化）
output_error_t ≤ Σ_{i=1}^{t} |softmax_error_i| · ‖V‖ + Σ softmax_i · δ · ‖V‖
              ≈ δ · ‖V‖ · t · O(1/t) + δ · ‖V‖   # 第一项因 softmax 归一化有界
              = O(δ · ‖V‖)                          # 不随 t 线性增长！

# 但实际崩溃来自排序翻转（非线性效应）
# 当参与 token 数 t 增长，至少一次翻转的概率 → 1
collision_prob(t) ≈ 1 - (1 - p_flip)^{C(t,2)} → 1 as t → ∞
```

C：`sglang/01-modules/M08-kv-cache/README.md` 的 FP8 dtype 描述；`insights/arxiv-pdf/2309.06180-pagedattention-vllm.md` 的 block/量化无关。I：误差传播与临界条件推导。U：具体 δ 值与 `L_c` 需实测不同量化方案在不同模型上的表现。

**图文说明：** 横轴序列长度 L，纵轴 attention 输出与 FP16 baseline 的 cosine similarity。FP8 曲线在 L<2000 时接近 1.0，之后缓慢下降；INT4 曲线在 L<500 时即明显下降。标注 `L_c` 临界点和"质量崩溃区"（cosine similarity < 0.9）。注意：图示为示意曲线，实际形状因模型/任务/量化方案而异。

**拓展知识：** 与 I-E011 衔接：KV 量化与 KV offload 可组合——量化减少每 token 字节，offload 增加放置层级；与 I-E009 衔接：低精度 KV cache 可能成为侧信道攻击的信息泄露面。

**常见追问及回答要点：**

1. **问：per-channel 量化 vs per-token 量化对误差传播的影响有何不同？** 答：per-channel 量化沿 head_dim 维度做 scale，对每个 head 独立校准，误差分布更均匀；per-token 量化对每个 token 的 KV 向量独立 scale，能更好地适应 magnitude 变化但在 attention score 的 cross-token 比较中引入额外 scale 差异。从误差传播角度，per-channel 在 score 计算中的误差更小（scale 一致），但 per-token 在 value 还原中的误差更小。
2. **问：量化误差在 MoE 模型中与 dense 模型中有何不同？** 答：MoE 模型的路由器（gate）依赖 hidden state 的精确值，KV 量化不直接影响路由（KV 来自 attention 层而非 FFN 层），但 attention output 的量化误差会通过残差流传播到 gate——误差越大，路由错误的概率越高。
3. **问：如何通过调度策略缓解 KV 量化误差？** 答：对超长序列做 chunked prefill 而非一次性 prefill 不能减少量化误差（误差在 decode 阶段累积），但可以通过"部分 KV 重算"定期用 FP16 重新计算最近 N 个 token 的 KV，重置误差累积窗口。具体实现中，可以在每 K 步 decode 后触发一次"KV refresh"——对最近 R 个 token 的 KV 用原始精度重写。代价是额外的计算和显存读写，收益是误差窗口从 L 降至 R。
4. **问：KV 量化与 speculative decode 的交互效应？** 答：speculative decode 的 verify 阶段从量化 KV 读取上下文——若量化误差导致 attention 分布偏移，target 的 argmax 可能与 FP16 基线不同，进而降低 acceptance rate。这是量化与 speculative 的负交互：高 acceptance rate 的前提是 target 行为接近 FP16 基线，量化越狠，该前提越不成立。生产决策中应先测量量化后的 acceptance rate 衰减——若从 90% 降至 70%，speculative 的收益从"平均每次提交 2-3 token"降至"1-2 token"，可能不再值得其额外复杂度。
5. **问：MLA（Multi-head Latent Attention）模型的 KV 量化与普通 MHA 有何不同？** 答：MLA 将 K/V 压缩为共享的 latent vector（`kv_cache_dim` 远小于 `n_heads × head_dim`），天然具有更小的 KV bytes per token。量化 MLA 的 latent 向量时，误差传播路径不同于 MHA——latent 的量化误差会影响所有共享该 latent 的 head，误差被放大 `n_heads` 倍。因此 MLA 对 KV 量化更敏感——FP8 MLA 可能等效于 INT4 MHA 的误差水平。

**易错点与反例：**

- 反例："KV 量化只影响显存不影响精度"——错误。decode 阶段每步从量化 KV 读取，量化误差直接影响 attention 计算。
- 反例："量化误差随序列长度线性累积"——错误。weighted sum 的误差理论上因 softmax 归一化而有界，但 softmax 排序翻转是非线性效应，导致"突然崩溃"而非"线性退化"。
- 易错：混淆 weight quantization 与 KV quantization——权重量化影响所有 token 的计算，KV 量化影响 decode 阶段的 attention 读取，两者的误差传播路径完全不同。

**实践与验证：** [U] 固定模型（Llama-3-8B），分别用 FP16/FP8/INT4 KV cache 做长文本生成，测量不同输出长度下的 perplexity 与 cosine similarity（对比 FP16 baseline）。oracle：FP8 在 4K 长度内与 FP16 无明显差异，INT4 在 1K 长度后出现质量下降。当前未执行 GPU 实验。

**分层评分标准：** 4分：能说明 KV 量化的基本机制与 decode 带宽收益；6分：能推导量化误差在 attention 中的传播路径，给出临界长度公式；8分：能分析 softmax 排序翻转的非线性效应，区分"线性累积"与"突变崩溃"，并提出可验证的缓解策略。

**证据与来源：** `sglang/01-modules/M08-kv-cache/README.md`（FP8 dtype）；`sglang/01-modules/M09-attention-cuda-graph/README.md`（quantized KV access）；Frantar et al. 2023 "GPTQ"（量化误差分析基础）；xiao et al. 2024 "SmoothQuant"（per-channel vs per-token 量化）。

**与 KV cache placement 的交互：** 量化后的 KV cache 更适合 offload 到 DRAM/远程——因为每 token bytes 减少，同等带宽下可以传输更多 token。`insights/arxiv-pdf/2508.13231` 的 Dynamic KV Cache Placement 论文提供了量化 KV 在异构内存中的放置建模。但需要注意：量化 KV 的 offload/restore 引入了额外的 quantize/dequantize 步骤，其计算开销必须计入 placement 判据——只有当 `transfer_time_saving > quantize_overhead` 时，offload 量化 KV 才优于本地 FP16 KV。

**FP8 KV 的生产部署考量：** FP8 KV 需要 attention kernel 支持 FP8 读取——FlashInfer 等 backend 已支持 quantized KV access，但不是所有 backend 都支持。`sglang/01-modules/M09-attention-cuda-graph/README.md` 的 backend 能力协商段落。部署前需验证：(a) 目标 GPU 的 FP8 tensor core 支持（Ada Lovelace / Hopper 架构），(b) attention backend 的 FP8 KV 路径数值正确性，(c) 与 speculative decode 的兼容性（verify 读取 FP8 KV 的 acceptance rate 是否在可接受范围内）。

**NUMA 拓扑对 KV 读取带宽的影响：** 在多 GPU 部署中，KV cache 的读取可能跨 NUMA 节点——CPU 侧 scheduler 的 prefix match 在本地 NUMA 执行，但 GPU 读取 KV 时若 PCIe 拓扑导致跨 NUMA 访问，实际可用带宽下降 30-50%。`sglang/01-modules/M07-分布式并行.md` 的 NUMA 感知部署。这对 decode 阶段的带宽瓶颈有放大效应——理论上的 `BW_hbm / KV_bytes_per_req` 需要乘以 NUMA 效率因子。

**量化 KV 的 attention kernel 适配：** FP8 KV 的 attention 计算需要在 kernel 内部做 dequantize（FP8 → BF16/FP16）后再做矩阵乘——这引入了额外的计算和 register 压力。FlashInfer 等 backend 通过 fused dequantize + attention kernel 减少 overhead，但并非所有 backend 都支持。`sglang/01-modules/M09-attention-cuda-graph/README.md` 的 backend 能力协商。若 backend 不支持 quantized KV，则量化只能应用于 KV storage（减少显存），不能应用于 compute——需要显式 dequantize 后再传入 attention kernel，增加了 HBM 读取带宽（先读量化值再展开为 FP16，两次读取）。

**I-E004 的可证伪实验设计：** 固定模型和 workload，逐步降低 KV 精度（FP16 → FP8 → INT8 → INT4），在固定序列长度（如 4K、8K、16K）下测量输出质量指标（perplexity、任务 accuracy）。若在某精度-长度组合下质量指标突变（下降 > 10%），则验证了"质量崩溃"假说；若质量渐变下降，则崩溃是非线性的反例。

**与 I-E003 和 I-E011 的三方交互：** KV 量化误差（E004）影响 speculative acceptance rate（E003），进而影响 PD 分离中 KV transfer 的字节数（E011）——量化减少 KV bytes 使传输更快，但增加 acceptance rate 衰减的风险。这三个问题构成一个"三角权衡"：量化精度越低 → KV 传输越快（正）但 acceptance rate 越低（负）且输出质量崩溃风险越高（负）。生产配置需要在这个三角中找到帕累托最优点——通常通过固定两个维度、扫描第三个维度来确定。

### I-E005 从系统权衡角度：PD 分离不是银弹——什么条件下它劣于混合部署？给出量化边界。

**元数据：** 知识域：`I13 Prefill/Decode 分离部署`；能力目标：`C13 从通信-计算权衡推导 PD 分离的优劣边界`；题型：量化建模与边界推导；频率：高频；前置/后续：`I-B001`（双阶段差异）、`I-E008`（状态迁移）；证据状态：论文 C，跨机传输 U。

**背景知识：** PD disaggregation 将 prefill 与 decode 放到不同实例，通过 KV transfer 连接。P/D-Serve 论文指出三个挑战：请求/prefix 多样性导致混合池不足、workload 估计不准造成 P/D 比例失调、cluster-level RDMA 上 block-fixed D2D KVCache transfer 难以达到理想利用率。`insights/arxiv-pdf/2408.08147-pd-serve.md`。SGLang 的 M13 模块实现 sender/receiver 契约：`registered → metadata/transfer → poll DONE → commit`，异常路径 `poll FAILED/TIMEOUT → abort/clear`。`sglang/01-modules/M13-disaggregation-hicache/README.md`。

**问题分析：** 1) 建立 PD 分离的成本-收益模型——收益来自资源专业化，成本来自 KV transfer；2) 推导 PD 劣于混合部署的充分条件——当 transfer 成本超过专业化收益时；3) 给出量化边界公式，参数化模型大小、序列长度、请求速率、网络带宽。

**具体答案：** 面试中可以这样回答：

> PD 分离的收益 = prefill 节点专注 compute-bound 任务（大 batch GEMM）+ decode 节点专注 bandwidth-bound 任务（大 KV cache 并发）。成本 = KV transfer 延迟 + staging buffer 显存占用 + 双节点资源冗余。
>
> **PD 劣于混合部署的充分条件：**
>
> 设单请求 KV 传输字节为 `B_KV = layers × 2 × kv_heads × head_dim × dtype_bytes × seq_len`，网络带宽为 `W_net`，请求到达率为 λ。
>
> 1. **短序列 + 高带宽模型**：当 `seq_len` 很小（如 < 256），`B_KV` 很小，但 transfer 的固定开销（connection setup、metadata exchange、poll latency）占主导。若 `T_transfer_fixed > T_prefill_compute`，PD 的 prefill 专业化收益被 transfer 开销吞没。
>
> 2. **小模型 + 低请求率**：小模型（7B）的 prefill 计算很快，PD 分离减少的 target iterations 有限；低请求率下 P/D 节点利用率不均，资源碎片化。设 `U_p` 为 prefill 节点利用率，`U_d` 为 decode 节点利用率——当 `min(U_p, U_d) < 0.5` 时，混合部署的总 GPU 利用率更高。
>
> 3. **长 prefill + 极短 decode**：若大部分请求的输出长度极短（如分类/提取任务，输出 < 10 token），decode 阶段的资源专业化几乎无收益，但 transfer 成本不变。
>
> **量化边界公式：**
>
> ```text
> # PD 净收益
> Net_benefit = (T_mixed - T_PD) - T_transfer - T_staging
>
> # 临界条件
> Net_benefit < 0  ⇔  T_transfer > (T_mixed - T_PD) - T_staging
>
> # T_transfer 估算
> T_transfer ≈ B_KV / W_net + T_roundtrip + T_staging_copy
>           = (L × C_per_token) / W_net + T_rt + L × C_per_token / W_hbm
>
> # 代入典型值：70B model, seq_len=2048, FP16 KV
> C_per_token ≈ 672 × 2 × 8 × 128 × 2 bytes ≈ 2.7 MB/token
> B_KV ≈ 2048 × 2.7MB ≈ 5.5 GB
> T_transfer ≈ 5.5GB / 100Gbps + 0.5ms + 5.5GB/2TB/s ≈ 440ms + 0.5ms + 2.7ms ≈ 443ms
>
> # 若混合部署的单请求总时间为 600ms，PD 专业化节省 100ms
> # 则 Net_benefit = 100ms - 443ms < 0  → PD 不划算
> ```
>
> 此边界对网络带宽极度敏感——若 W_net 从 100Gbps 升至 400Gbps，T_transfer 降至 ~111ms，Net_benefit 变为正。

**技术洞察：**

```text
# PD 分离收益模型
T_mixed = T_prefill + T_decode          # 混合部署单请求时间
T_PD = max(T_prefill, T_decode/k) + T_transfer  # PD 分离（k 为 decode 节点数）

# 收益正的条件
Profit > 0 ⇔ T_transfer < T_mixed - T_PD

# 参数化
T_transfer = α × seq_len × model_size / W_net + T_overhead
T_mixed_prefill ∝ seq_len × model_size / compute_per_gpu
T_mixed_decode ∝ output_len × model_size / (bandwidth_per_gpu × batch_size)

# 临界模型大小（给定 seq_len, W_net, T_overhead）
model_size_critical = (T_mixed - T_PD - T_overhead) × W_net / (α × seq_len)

# 失效区（PD 不划算）
PD_suboptimal ← seq_len < seq_critical
             ∨ model_size < model_critical
             ∨ W_net < bandwidth_critical
             ∨ λ < λ_critical (利用率不足)
```

C：`insights/arxiv-pdf/2408.08147-pd-serve.md` 的三个挑战；`sglang/01-modules/M13-disaggregation-hicache/README.md` 的 transfer 契约。I：成本-收益模型与边界推导。U：具体临界值需跨机 RDMA/RoCE 实验验证。

**图文说明：** 二维热力图——横轴 `seq_len`，纵轴 `model_size`，颜色表示 PD 净收益（绿=正，红=负）。标注临界曲线 `Profit=0`。在曲线左下方（短序列+小模型）为红色失效区；右上为绿色收益区。叠加等值线标注网络带宽 W_net=100/200/400Gbps 时的临界偏移——带宽越高，失效区越小。

**拓展知识：** 与 I-E008 衔接：PD 分离增加了状态迁移的复杂度（KV ownership 跨实例）；与 I-E003 衔接：speculative decode 在 PD 分离架构中使 transfer 内容变为 draft+target KV 的组合。

**常见追问及回答要点：**

1. **问：PD 分离中 prefix caching 如何影响 transfer 成本？** 答：prefix caching 使重复前缀的 KV 无需重算，但 PD 架构中 KV 仍需从 prefill 节点传输到 decode 节点。若 decode 节点已有该前缀的缓存（来自之前的请求），可以只传输 suffix 部分——但这要求 prefix cache 跨实例共享或 decode 节点已有本地副本，引入了缓存一致性问题。
2. **问：workload 波动时如何动态调整 P/D 比例？** 答：P/D-Serve 提出动态资源编排——根据实时请求到达率和平均 prompt 长度调整 prefill/decode 节点比例。但资源重分配涉及 KV routing 表更新、连接迁移、负载再均衡，其自身的 overhead 在快速波动场景下可能超过收益。
3. **问：PD 分离对 TTFT 的影响？** 答：TTFT 增加约 `T_transfer + T_staging`——请求在 prefill 完成后不能立即 decode，必须等待 KV 传输完成。对于 TTFT SLO 极严的场景（< 200ms），PD 分离可能直接违反 SLO。

**易错点与反例：**

- 反例："PD 分离总是提升吞吐"——错误。PD 分离提升的是资源专业化效率，但在短序列/小模型/低频场景下，transfer 开销可能使总吞吐低于混合部署。
- 反例："KV transfer 时间可以忽略"——错误。70B 模型 2048 token 的 KV 约 5.5GB，100Gbps 网络下纯传输时间 ~440ms，与 decode 单步时间可比。
- 易错：忽略 staging buffer 显存——PD 分离的 decode 节点需要 staging buffer 接收 KV，占用 HBM 但不服务请求，减少了可用于 decode batch 的显存。

**实践与验证：** [U] 固定模型（Llama-3-70B）、网络（100Gbps RoCE），测量混合部署 vs PD 分离在 seq_len ∈ {256, 1024, 4096, 8192} 下的 TTFT/TPOT/吞吐。oracle：短序列下混合部署更优，长序列下 PD 分离更优。当前未执行跨机实验。

**分层评分标准：** 4分：能说明 PD 分离的基本原理与 KV transfer 流程；6分：能建立成本-收益模型，推导失效的充分条件；8分：能给出参数化的量化边界公式，分析网络带宽/模型大小/序列长度的敏感性，并讨论动态调整的 overhead。

**证据与来源：** `insights/arxiv-pdf/2408.08147-pd-serve.md`（P/D-Serve 三个挑战）；`sglang/01-modules/M13-disaggregation-hicache/README.md`（transfer 契约与 staging）；`insights/05-sglang-inference-technical-insights.md`（insight 九 P/D 与 HiCache）。

### I-E006 从容量理论角度：长上下文推理的显存-延迟-SLO 三者如何相互约束？推导容量上限。

**元数据：** 知识域：`I15 长上下文容量建模`；能力目标：`C15 从显存预算推导并发-SLO 的理论上限`；题型：理论推导与容量规划；频率：高频；前置/后续：`I-B003`（KV cache 概念）、`I-E004`（KV 量化）；证据状态：原理 C，性能数字 U。

**背景知识：** 长上下文推理的核心约束是 KV cache 的显存占用。每 token 的 KV bytes = `layers × 2 × kv_heads × head_dim × dtype_bytes`。PagedAttention 用 block table 映射逻辑/物理 block。`insights/arxiv-pdf/2309.06180-pagedattention-vllm.md`。SGLang 的容量预算：`available_hbm - model_weights - runtime_activations - cuda_graph_static_buffers - safety_margin → physical_kv_budget → max_total_tokens`。`sglang/90-cross-module/pooling-and-resource-management.md`。Robust KV Cache Management 论文强调输出长度不确定下的 reservation 问题——under-reservation 触发 preemption，over-reservation 浪费 memory。`insights/arxiv-pdf/2607.16892`。

**问题分析：** 1) 建立显存-并发-延迟的三维约束模型；2) 推导在给定 SLO 下可容纳的最大并发请求数；3) 分析量化、offload、稀疏 attention 如何移动容量边界。

**具体答案：** 面试中可以这样回答：

> 设 HBM 容量为 `M_total`，模型权重占 `M_weights`，运行时 workspace 占 `M_runtime`，CUDA Graph static buffers 占 `M_graph`，安全余量占 `M_safety`。可用于 KV cache 的显存：
>
> ```text
> M_KV = M_total - M_weights - M_runtime - M_graph - M_safety
> ```
>
> 每个请求的 KV 显存需求 = `seq_len × C_token + output_len × C_token`，其中 `C_token = layers × 2 × kv_heads × head_dim × dtype_bytes`。由于 output_len 未知，需用历史分布的分位数 `Q_output(p)` 做 reservation。
>
> **并发上限**：`N_max = floor(M_KV / (seq_len × C_token + Q_output(p) × C_token))`。
>
> **延迟约束**：长 prefill 占用 compute 资源，挤压同 batch 其他请求的 TTFT。设 SLO_TTFT 为 T 毫秒，则单轮 prefill token 预算 `B_prefill = T / t_prefill_per_token`。超过此预算的 prompt 必须 chunked prefill——引入 chunk 状态管理开销但降低 TTFT 峰值。
>
> **SLO-容量权衡**：对 P99_TPOT 的 SLO 要求约束了 decode batch 大小——batch 越大，KV 总读取量越大，单步时间越长。设 SLO_TPOT 为 τ 毫秒，则最大 decode batch `B_decode_max = τ / (C_token × active_requests / bandwidth)`。
>
> **三者联立**：
>
> ```text
> N_max × (seq_len + Q_output) × C_token ≤ M_KV        # 显存约束
> N_max ≤ B_decode_max                                    # TPOT SLO 约束
> seq_len ≤ B_prefill                                     # TTFT SLO 约束（单轮）
>
# 联立得：
> N_max = min(M_KV / ((seq_len + Q_output) × C_token),
>              B_decode_max)
> ```
>
> **边界移动策略**：KV 量化（FP8）使 `C_token` 减半，`N_max` 翻倍，但量化误差的累积效应在超长序列下可能提前触达质量崩溃——此时容量上限被质量上限取代，`N_max` 不再是约束，`L_c`（临界长度）才是。这意味着长上下文推理的真实上限不是"能放多少请求"，而是"在质量可接受的前提下能处理多长序列"——容量规划与质量保障在此交汇。CPU offload 使 `M_KV` 增加 host DRAM 容量但引入 PCIe 延迟，可能违反 TPOT SLO；稀疏 attention（滑动窗口）使有效 `seq_len` 从全长变为窗口大小——这是"精度换容量"的典型取舍，SWA 的窗口大小 `w` 直接决定了 attention 的理论召回能力。NEO 论文证明 GPU-CPU 非对称流水在特定 batch 分布下可以同时利用两者，但其调度复杂度随请求长度方差增大而急剧上升。

**容量规划的实践建议：** 生产中的容量规划应基于历史 P95 输出长度而非 `max_context_len` 做 reservation——按 max 预留会导致 95% 以上的容量被浪费。论文 "Robust KV Cache Management" 提出按请求类别（task type）分别设置 reservation quantile——代码生成任务用 P99，对话任务用 P95，摘要任务用 P90。`insights/arxiv-pdf/2607.16892`。

**max_total_tokens 的用户配置：** SGLang 暴露 `max_total_tokens` 作为用户可调参数——增大它可以提升并发但增加显存压力，减小它限制并发但保证单请求质量。`sglang/90-cross-module/pooling-and-resource-management.md`。生产配置应通过 profiling 找到拐点：逐步增大 `max_total_tokens` 直到 TTFT 或 P99_TPOT 开始恶化，然后回退 10-15% 作为安全余量。

**输出长度分布的非平稳性：** 论文 "Robust KV Cache Management" 强调 workload 分布具有 non-stationarity——早上的代码生成任务平均输出长度可能远大于晚上的对话任务。`insights/arxiv-pdf/2607.16892`。固定 reservation quantile 无法适应这种变化——需要在线估计输出长度分布（exponentially weighted moving average），动态调整 reservation 策略。

**I-E006 的总结性论断：** 长上下文推理的显存-延迟-SLO 三者约束不是一个简单的"取最小值"——它们在不同 workload 分布下轮流成为瓶颈。短序列高并发时 TPOT SLO 是瓶颈（bandwidth-bound），长序列低并发时 TTFT SLO 是瓶颈（compute-bound），混合分布时容量碎片是瓶颈（allocator fragmentation）。`insights/05-sglang-inference-technical-insights.md` 的端到端性能因果树提供了诊断框架。生产优化的优先级应基于当前 workload 分布的瓶颈分析——而非均匀地优化所有维度。

**技术洞察：**

```text
# 长上下文容量上限公式
N_max = min(floor(M_KV / ((L_in + L_out_est) × C_token)),
            floor(τ_TPOT × BW_hbm / (C_token × avg_active_ratio)))

# 对 N_max 的敏感性分析
∂N_max/∂L_in < 0     # 输入越长，并发越低（线性反比）
∂N_max/∂C_token < 0  # 每 token bytes 越大，并发越低
∂N_max/∂M_KV > 0     # 显存越大，并发越高
∂N_max/∂τ_TPOT > 0   # SLO 越宽松，并发越高

# 量化收益（FP16 → FP8）
N_max_FP8 / N_max_FP16 ≈ (C_token_FP16 / C_token_FP8) = 2.0

# 稀疏 attention 收益（全 attention → 滑动窗口 w）
N_max_sparse / N_max_full ≈ L_in / w   (当 L_in >> w)
```

C：`sglang/90-cross-module/pooling-and-resource-management.md` 的容量预算公式；`insights/arxiv-pdf/2309.06180-pagedattention-vllm.md` 的 block 模型；`insights/arxiv-pdf/2607.16892` 的 reservation 问题。I：联立约束公式推导。U：具体数值需目标硬件和模型 profile 验证。

**图文说明：** 三维轴图——X 轴 `seq_len`，Y 轴 `N_concurrent`，Z 轴 `TPOT`。绘制两个曲面：显存约束曲面（`N × seq_len = M_KV / C_token`，双曲线）和 TPOT SLO 曲面（`TPOT = N × C_token × factor / BW`，平面）。两曲面的交线即为可行域的边界——在交线左下方为绿色可行区，右上为红色违反区。标注量化/稀疏 attention 操作后的曲面偏移方向。

**拓展知识：** 与 I-E004 衔接：KV 量化直接降低 `C_token` 扩大容量；与 I-E005 衔接：PD 分离将单实例的容量约束转化为跨实例的容量-带宽联合约束。

**常见追问及回答要点：**

1. **问：输出长度不确定时如何避免 over-reservation？** 答：使用 SLO-aware 的 reservation 策略——按历史分布的分位数 `Q_output(0.95)` 做初始 reservation，输出超过 reservation 时触发 partial preemption（释放并备份 KV，请求回到 waiting queue）。论文 "Robust KV Cache Management" 提出同时考虑 GPU 并行配置、请求类别路由、共享 prefix caching 的联合优化。`insights/arxiv-pdf/2607.16892`。
2. **问：长 context 下 prefix caching 对容量的影响？** 答：prefix caching 减少的是 prefill 计算量（命中的前缀无需重算），但命中的 KV 仍需占用显存。在容量维度上，prefix cache 是"已占用的缓存"而非"释放的显存"——只有 eviction 释放显存。Radix cache 通过 `lock_ref` 保护正在使用的 prefix，防止被 evict。
3. **问：CP（Context Parallel）如何改变容量约束？** 答：CP 将长序列的 attention 和 KV 切分到多个 rank，每个 rank 只持有 `seq_len / CP_size` 的 KV——等效于将 `C_token` 降为 `C_token / CP_size`。但 CP 引入 all-gather 通信开销，可能违反 TPOT SLO。因此 CP 增加容量上限但降低吞吐——需要在两者间取舍。`sglang/01-modules/M20-parallel-strategies/README.md` 的 CP 描述。
4. **问：长上下文场景下 prefix caching 与 capacity 的关系是否非线性？** 答：是。短序列的 prefix caching 收益小（计算量少），长序列的 prefix hit 收益巨大（省去的 prefill 计算量正比于 `seq_len`）。但长序列的 KV 也占用更多显存——一条 128K 请求的 KV 可能占满 `N_max` 中的大部分。这是长上下文特有的"prefix caching 悖论"：最需要缓存的长序列，其缓存结果也最占空间。工程上通过分层缓存（radix cache 在 HBM，evictable 的 offload 到 host）缓解。

**易错点与反例：**

- 反例："显存越大并发越高，线性关系"——错误。并发受 TPOT SLO 约束，显存增加到一定程度后，瓶颈从显存转为带宽，并发不再增长。
- 反例："prefix caching 增加可用显存"——错误。prefix caching 减少计算，但 KV 仍占用显存——只有 eviction 释放显存。
- 易错：混淆 `max_total_tokens` 与并发数——`max_total_tokens` 是所有活跃请求的 KV token 总和上限，不等于请求数。单个长序列请求可能占用大量 token 额度。

**实践与验证：** [U] 固定模型（Llama-3-70B，不同 seq_len），测量实际可用并发数 vs 理论 `N_max`。记录 KV 显存占用、TPOT、retraction 频率。oracle：实际并发 ≤ 理论 `N_max`，且 TPOT 随并发增长而上升直到违反 SLO。当前未执行 GPU 实验。

**分层评分标准：** 4分：能说明 KV cache 显存占用公式与并发的基本关系；6分：能联立显存/SLO/延迟三约束，推导并发上限；8分：能分析量化/稀疏 attention/CP 对边界的移动效果，给出敏感性分析和生产配置建议。

**证据与来源：** `sglang/90-cross-module/pooling-and-resource-management.md`（容量预算与三层池）；`insights/arxiv-pdf/2309.06180-pagedattention-vllm.md`（PagedAttention block 模型）；`insights/arxiv-pdf/2607.16892`（Robust KV Management reservation）；`sglang/01-modules/M20-parallel-strategies/README.md`（CP 对 KV 的分片）。

### I-E007 从事故模型角度：推理服务故障的根因、触发条件与放大因素如何分层？用一个 TTFT 劣化案例说明。

**元数据：** 知识域：`I16 推理服务故障模型`；能力目标：`C16 分层故障建模与 TTFT 劣化案例分析`；题型：系统建模与案例分析；频率：高频；前置/后续：`I-B005`（性能调试）、`I-E001`（不变量）；证据状态：源码 C，压测 U。

**背景知识：** SGLang 的端到端性能因果树：TTFT/TPOT/P99 异常时，先看 GPU 是否繁忙——不繁忙则问题在 tokenize/IPC/grammar queue/scheduler/admission/transfer wait；繁忙则分 prefill 还是 decode——prefill 看 prefix miss/chunk/attention kernel，decode 看 batch size/KV bandwidth/graph fallback。`insights/05-sglang-inference-technical-insights.md` 的 10 节。故障矩阵列出了 prefill budget 不足、decode KV 不足、graph ineligible、grammar 失败、overlap future 失败、speculative reject、transfer timeout、client disconnect、worker/rank crash 的正确降级与不可接受半状态。

**问题分析：** 1) 建立根因-触发-放大三层模型，与 OSI 分层或事故链模型类比；2) 用 TTFT 劣化案例逐层分析：从症状（TTFT 上升）→ 直接原因（prefill 延迟增加）→ 根因（KV 碎片/retraction）；3) 分析放大因素——什么让局部故障扩散为全局 SLO 违反。

**具体答案：** 面试中可以这样回答：

> **三层故障模型：**
>
> | 层 | 定义 | 推理服务示例 |
> |---|---|---|
> | 根因（Root Cause） | 系统状态偏离正常区间的底层原因 | KV lock_ref 泄漏、allocator free list 碎片化、grammar cache 污染 |
> | 触发条件（Trigger） | 使根因产生可观测效应的具体事件 | 长 prompt 到达导致 KV 不足触发 retraction；grammar 编译 future 超时 |
> | 放大因素（Amplifier） | 将局部故障扩散为全局影响的机制 | retraction 波（一个请求 retract 释放空间→admit 新请求→再不足→再 retract）；GPU 利用率 100% 时 scheduler CPU 排队 |
>
> **TTFT 劣化案例分析：**
>
> 症状：P99 TTFT 从 200ms 升至 800ms，但 TPOT 正常，GPU 利用率从 80% 降至 60%。
>
> 直接原因（第一层）：scheduler 每轮可 admission 的 prefill token 减少——prefill batch 变小，长 prompt 排队更久。
>
> 根因推导（第二层）：检查 KV allocator 的 `free_pages` 总量正常但 `max_allocatable_segment` 下降——存在外部碎片。原因是大量短请求完成后留下不连续的 partial page，长 prefill 无法获得连续 KV 空间。
>
> 触发条件：碎片化在短请求频繁到达时逐步积累（根因持续存在），直到某个长 prompt（> 2048 token）到达时无法获得足够连续 page——这是触发点。
>
> 放大因素（第三层）：
> 1. retraction 波——长 prefill 无法 admission → 等待 → 占用 waiting queue → 更多请求等待 → scheduler CPU 开销增加。
> 2. GPU 利用率下降但 TTFT 上升——看似矛盾，实因 GPU 在等 scheduler 的 admission 决策（scheduler CPU 成为瓶颈）。
> 3. 若开启 prefix caching，碎片化的 KV 空间使 radix tree 无法插入完整 prefix——prefix hit 率进一步下降，更多请求需要完整 prefill。
>
> **可证伪验证**：若碎片化是根因，`flush_cache` 后 TTFT 应恢复——flush 重置 allocator 消除碎片。若 flush 后仍差，则根因不是碎片化而是 scheduler CPU 瓶颈或 admission 配置问题。更精细的诊断：在 flush 前后分别记录 `max_allocatable_segment` 的分布——若 flush 后显著增大，确认碎片化；若 flush 前后一致但 TTFT 仍差，转向排查 scheduler 的 prefix match 开销或 grammar queue backlog。

**技术洞察：**

```text
# 故障分层模型
RootCause ∈ {KV_fragmentation, lock_ref_leak, grammar_cache_pollution,
              admission_miscalibration, bandwidth_saturation}

Trigger ∈ {long_prompt_arrival, batch_composition_change,
            transfer_timeout, workload_burst}

Amplifier ← retraction_wave
          ∨ scheduler_cpu_bottleneck
          ∨ prefix_hit_cascade_failure
          ∨ GPU_idle_with_queue_backlog

# TTFT 劣化的诊断决策树
TTFT_high → GPU_busy?
  ├─ No → scheduler_overhead? → admission_delay? → KV_fragmentation?
  │     ├─ Yes → check free_pages vs max_allocatable_segment
  │     └─ No → check grammar_queue / tokenize / IPC
  └─ Yes → prefill_or_decode?
        ├─ prefill → prefix_miss_rate? → chunked_prefill_overhead?
        └─ decode → (TPOT also high?) → KV_bandwidth? → batch_size?
```

C：`insights/05-sglang-inference-technical-insights.md` 的因果树与故障矩阵。I：三层故障模型与 TTFT 诊断树。U：碎片化对 TTFT 的具体影响需 tiny-pool 实验验证。

**图文说明：** 鱼骨图（Ishikawa）——主轴为"TTFT 劣化"，三大骨为"资源层"（KV 碎片、allocator 不足）、"调度层"（admission delay、scheduler CPU 瓶颈）、"数据层"（prefix miss、chunked prefill 开销）。每个大骨上标注具体的根因和触发条件。在"资源层"的"KV 碎片"分支上用红色标注放大路径：碎片 → 长 prompt 等待 → retraction 波 → scheduler 过载 → 所有请求 TTFT 上升。

**拓展知识：** 与 I-E001 衔接：不变量破坏（lock_ref 泄漏）是根因层的核心成员；与 I-E009 衔接：放大因素中的"retraction 波"可被恶意流量触发。

**常见追问及回答要点：**

1. **问：如何区分 scheduler CPU 瓶颈与 KV 碎片？** 答：scheduler CPU 瓶颈的特征是 `running_batch` 未满但 admission 延迟高，且 `free_pages` 总量充足——问题在 scheduler 的 prefix match/sorting 开销而非资源不足。KV 碎片的特征是 `free_pages` 总量充足但 `max_allocatable_segment` 不足——大请求无法获得连续空间。
2. **问：retraction 波如何自动恢复？** 答：retraction 释放 KV 空间后可重新 admission，但频繁 retraction 导致请求在 waiting/running 间振荡。SGLang 的 retract_decode 按 retraction order 释放请求——普通请求尝试释放并备份后重新入队，无法恢复的请求设置 abort。放大效应的退出条件是请求到达率下降或长 prompt 处理完成。
3. **问：TTFT 与 TPOT 同时劣化说明什么？** 答：说明根因在两阶段共享资源——如 KV bandwidth 饱和（decode batch 过大，同时 prefill 写入也占用带宽）、GPU 利用率 100% 且 scheduler 无法有效交错。此时需要降低并发或增加 KV cache 容量。具体可通过降低 `max_running_requests` 或启用 KV 量化来缓解 bandwidth 压力。
4. **问：如何设计一个自动化的故障定位系统？** 答：构建基于决策树的诊断 agent——输入实时 metrics（GPU util、free pages、N_active、TTFT、TPOT、retraction rate、prefix hit rate），输出根因分类（KV 碎片 / scheduler 瓶颈 / bandwidth 饱和 / grammar backlog）。每个叶子节点关联一个可证伪实验（flush_cache / disable graph / reset grammar cache），系统自动执行实验并验证假设——这是"根因分析 skill"的系统化实现。

**易错点与反例：**

- 反例："TTFT 高 = prefill 计算慢"——片面。TTFT 高可能是 scheduler 排队延迟（admission wait），而非 prefill 本身慢。
- 反例："GPU 利用率低 = 系统正常"——错误。GPU 利用率低但 TTFT 高说明瓶颈在 CPU 侧（scheduler、tokenize、IPC），而非 GPU 侧。
- 易错：只优化触发条件（如增加 admission budget）而忽略根因（碎片化）——触发条件被压制但根因仍在，系统会在更大规模下再次违反 SLO。

**实践与验证：** [U] 构造碎片化场景——大量短请求（< 100 token）完成后立即发送长 prefill（> 2048 token），观察 TTFT 变化与 `max_allocatable_segment` 的关系。oracle：碎片化越严重，长 prompt TTFT 越长，flush 后恢复。当前未执行 GPU 实验。

**分层评分标准：** 4分：能列出推理服务的常见故障类型与降级路径；6分：能建立三层故障模型，用 TTFT 案例逐层分析；8分：能给出可证伪的诊断决策树，分析放大因素的扩散条件与退出条件，并讨论如何设计故障注入验证假设。10分：能设计故障注入实验框架——在可控环境中复现根因（如通过 tiny-pool 强制触发 fragmentation、通过 crafted grammar schema 触发 cache bloat），自动触发放大因素（retraction wave、scheduler CPU 饱和），并验证自动诊断系统的根因定位准确率。

**与可观测性的关系：** 三层故障模型的有效性依赖于充分的可观测性——必须同时暴露 KV allocator 指标（free/release/evictable/protected）、scheduler 指标（waiting/running/admission_rate/retract_rate）、attention 指标（eager/graph fallback rate）和 request 指标（TTFT/TPOT/perplexity）。`insights/05-sglang-inference-technical-insights.md` 的建议指标表提供了完整的观测清单。

**retraction 作为早期预警：** `retract_decode` 是 KV 不足的显式信号——即使尚未触发 SLO 违反，retraction 率的上升也预示着容量即将耗尽。`sglang/01-modules/M04-scheduler-batching/README.md` 的 retraction 流程。建议将 retraction rate 纳入 autoscaling 的提前触发器——在 P99 TPOT 尚有余量时提前扩容。

**故障恢复后的验证：** 故障恢复（如 retract 后请求重新 admission）不应仅检查请求能继续生成，还需验证生成质量未受损——re-attention 的 KV 是否与之前一致、grammar state 是否正确恢复、sampling RNG 是否保持连续性。`insights/05-sglang-inference-technical-insights.md` 的失败矩阵列出了每类故障的"不可接受半状态"——恢复后应断言不存在这些半状态。

**证据与来源：** `insights/05-sglang-inference-technical-insights.md`（因果树与故障矩阵）；`sglang/01-modules/M04-scheduler-batching/README.md`（retract_decode 流程）；`sglang/01-modules/M08-kv-cache/README.md`（allocator 碎片与 free list）；`insights/02-kv-pool-lifecycle-and-ownership.md`（故障树）。

### I-E008 从一致性角度：推理服务零停机升级需要哪些状态迁移保证？如何形式化验证？

**元数据：** 知识域：`I17 零停机升级与状态迁移`；能力目标：`C17 形式化状态迁移不变量与验证方案`；题型：形式化分析与验证设计；频率：中频；前置/后续：`I-E001`（不变量）、`I-E007`（故障模型）；证据状态：源码 C，生产验证 U。

**背景知识：** SGLang 的请求生命周期包含 `Req`（scheduler 进程内的可变请求状态）、`ScheduleBatch`（本轮调度工作对象）、`ForwardBatch`（设备执行视图）、`GenerationBatchResult`（待提交结果）。每个请求维护 `visible_len`、`committed_len`、`allocated_len` 三个长度。`insights/06-speculative-decoding-as-transaction.md`。零停机升级涉及：in-flight 请求的 KV ownership 迁移、grammar/sampling 状态迁移、CUDA Graph buffer 重建、连接保持。`sglang/01-modules/M04-scheduler-batching/README.md` 的初始化顺序：加载权重 → KV memory pools → attention backends → CUDA graphs → prewarm。

**问题分析：** 1) 形式化定义零停机升级需要保持的状态与不变量；2) 建立状态迁移协议——从旧进程到新进程的 ownership 转移；3) 给出形式化验证方案——如何用断言和 oracles 证明迁移正确。

**具体答案：** 面试中可以这样回答：

> **零停机升级的形式化要求：** 设旧进程状态为 `S_old`，新进程为 `S_new`。对于每个 in-flight 请求 r，升级过程必须保证：
>
> ```text
> ∀ r ∈ in_flight:
>   S_new.committed_len(r) ≥ S_old.committed_len(r)      # 不丢失已提交输出
>   S_new.KV_prefix(r) ⊇ S_old.KV_prefix(r)               # 不丢失已提交 KV
>   S_new.grammar_state(r) ≡ S_old.grammar_state(r)       # 约束状态一致
>   S_new.sampling_state(r) ≡ S_old.sampling_state(r)     # RNG 状态一致
> ```
>
> **状态迁移保证（按组件分层）：**
>
> | 组件 | 迁移策略 | 不变量 |
> |---|---|---|
> | KV cache ownership | 旧进程 serialize KV prefix → 新进程通过 HiCache restore 路径重新提交 | `committed_len` 不减少 |
> | Request state | 旧进程 snapshot `Req` 状态（`output_ids`、`committed_len`、`grammar_state`、`sampling_rng`）→ 新进程 reconstruct | `visible_output` 不变 |
> | CUDA Graph | 新进程重新 capture——不迁移 graph key/bucket | graph 在首次 replay 前 eager fallback |
> | Connection | 负载均衡器 drain 旧进程 → route 到新进程 | 无 in-flight 请求丢失 |
> | Prefix cache | 通过 HiCache 的 host/storage 层持久化 → 新进程 warm prefix cache | prefix hit 率不下降 |
>
> **形式化验证方案：**
> 1. **不变量断言**：升级前后对比所有 in-flight 请求的 `committed_len`、`output_ids`、`grammar_consumed_len`——新进程必须 ≥ 旧进程。
> 2. **Token-by-token oracle**：对升级中的请求，对比新旧进程的输出 token 序列——从升级点开始，新进程的输出必须与旧进程在升级点的"预测后续"一致（使用相同 seed）。
> 3. **KV prefix 校验**：新进程的 KV cache 中，已提交前缀的 hash 必须与旧进程 snapshot 的 hash 匹配。
> 4. **故障注入**：在升级过程中注入 abort/disconnect——断言没有"输出已提交但 KV 未提交"的半状态残留。

**技术洞察：**

```text
# 零停机升级的形式化不变量
INV_visible:  ∀r, new.visible_output(r) ⊇ old.visible_output(r)
INV_committed: ∀r, new.committed_len(r) ≥ old.committed_len(r)
INV_kv:       ∀r, new.kv_prefix(r) ⊇ old.kv_prefix(r)
INV_grammar:  ∀r, new.grammar_state(r) = old.grammar_state(r)
INV_sampling: ∀r, new.rng_state(r) = old.rng_state(r) @ cutoff_point

# 迁移协议（两阶段）
Phase 1: DRAIN
  - LB stops routing new requests to old process
  - Old process continues serving in-flight requests
  - Old process snapshots committed state for each r

Phase 2: TRANSFER + RESTORE
  - New process loads model weights + rebuilds KV pools
  - New process restores KV prefix from snapshot/HiCache
  - New process reconstructs Req state (grammar, sampling, output_ids)
  - LB starts routing to new process

# 安全条件
Safe_to_kill_old ← ∀r, new.committed_len(r) ≥ old.committed_len(r)
                  ∧ new.KV_restored(r) = true
                  ∧ no_half_state(r)
```

C：`insights/06-speculative-decoding-as-transaction.md` 的 committed_len 不变量；`sglang/01-modules/M04-scheduler-batching/README.md` 初始化顺序。I：形式化迁移协议推导。U：实际升级过程中的状态一致性需生产环境验证。

**拓展知识：** 与 I-E005 衔接：PD 分离架构中零停机升级还需迁移 KV transfer 中的 staging state；与 I-E001 衔接：升级后新进程的 graph 不变量需要重建（capture 是启动时建立的）。

**常见追问及回答要点：**

1. **问：CUDA Graph 在升级中的处理方式？** 答：CUDA Graph 的 capture batch、static buffer 与进程的 CUDA context 绑定——无法跨进程迁移。新进程必须重新执行 warmup + autotune + capture 流程。在新进程 graph capture 完成前，所有 decode 走 eager fallback——这对 TTFT/TPOT 有短期影响但不影响正确性。`sglang/01-modules/M09-attention-cuda-graph/README.md`。
2. **问：speculative decoding 请求在升级中如何迁移？** 答：speculative 请求的状态迁移更复杂——除了 `committed_len` 还需迁移 draft model 的 KV/state 和 acceptance 状态。最安全的策略是在升级前将 speculative 请求退化为普通 decode（回滚到 `BaseCommitted` 状态，丢弃未提交的 draft 候选）。`insights/06-speculative-decoding-as-transaction.md` 的状态机。
3. **问：如何验证升级后的 prefix cache 完整性？** 答：旧进程在 drain 阶段将 radix tree 中所有 evictable node 的 KV indices 写入 HiCache storage；新进程启动后从 HiCache 恢复 prefix cache。验证：对比新旧进程在相同 warmup 请求序列下的 prefix hit token 数——应完全一致。SGLang 的 `flush_cache` 只在 idle 执行，但升级中的状态迁移需要"热迁移"——engine 仍在处理请求时必须保证 radix tree 的引用一致性。`sglang/01-modules/M08-kv-cache/README.md` 的 flush 边界。
4. **问：零停机升级的形式化验证能否用 TLA+ 建模？** 答：可以。定义状态变量 `visible_output`、`committed_len`、`kv_ownership`，定义 DRAIN、TRANSFER、RESTORE 三个 action，不变量 `∀r, new.committed_len(r) ≥ old.committed_len(r)` 作为 safety property，liveness property 为 `∀r, ∃t, new.visible_output(t) = old.visible_output(t)`。TLA+ 可检测"半状态"反例（如 KV 提交但 grammar 未迁移）。

**易错点与反例：**

- 反例："零停机升级只需负载均衡器 drain 连接就够"——错误。drain 只保证新请求不路由到旧进程，不保证 in-flight 请求的状态（KV、grammar、sampling）被正确迁移。
- 反例："新进程可以直接继承旧进程的 KV cache"——错误。KV cache 的 physical tensor 与 CUDA context 绑定，无法跨进程共享——必须通过 serialize/restore 重建。
- 易错：忽略 grammar 状态迁移——grammar matcher 的状态（已匹配位置、DFA state）若不迁移，新进程的 grammar 约束会从初始状态开始，导致结构化输出不合法。

**实践与验证：** [U] 设计升级测试：固定请求流，在随机时刻触发升级，验证升级前后请求输出的 token-by-token 连续性。oracle：所有 in-flight 请求的输出在升级点前后无缝衔接，无 token 丢失或重复。当前未执行生产环境验证。

**分层评分标准：** 4分：能说明零停机升级需要保持的基本状态（请求、KV、连接）；6分：能形式化定义迁移不变量，设计两阶段迁移协议；8分：能给出完整的验证方案（token-by-token oracle、KV hash 校验、故障注入），分析 speculative/grammar 等复杂状态的迁移策略。

**证据与来源：** `insights/06-speculative-decoding-as-transaction.md`（committed_len 不变量与状态机）；`sglang/01-modules/M04-scheduler-batching/README.md`（初始化顺序与 PrefillAdder 约束）；`sglang/01-modules/M13-disaggregation-hicache/README.md`（HiCache 的 restore 路径可用于状态迁移）；`insights/05-sglang-inference-technical-insights.md`（失败矩阵的半状态定义）；`sglang/01-modules/M09-attention-cuda-graph/README.md`（CUDA Graph 的 capture 生命周期——升级中必须重建）。

**滚动升级 vs 蓝绿部署：** 滚动升级（逐个实例替换）需要完整的状态迁移协议（KV ownership、grammar state、sampling RNG），复杂度高但资源利用率好；蓝绿部署（整套切换）只需要 drain + 重定向，复杂度低但需要 2x 资源。生产中选择取决于状态迁移的实现完整度和资源成本约束。

**Engine idle 与 upgrade 的关系：** SGLang 的 `flush_cache` 只在 engine idle 时执行——升级中的状态迁移同样需要"逻辑 idle"（无 in-flight 请求处于非 committed 状态）。`sglang/01-modules/M08-kv-cache/README.md` 的 flush 边界。对于 speculative 请求，必须等待其回到 `BaseCommitted` 状态后才能视为"逻辑 idle"——否则迁移会丢失 draft 状态。

### I-E009 从安全模型角度：推理服务的攻击面如何系统枚举？限流与隔离的安全边界在哪？

**元数据：** 知识域：`I18 推理服务安全`；能力目标：`C18 系统枚举攻击面与安全边界分析`；题型：安全建模与边界推导；频率：中频；前置/后续：`I-E001`（不变量）、`I-E007`（故障模型）；证据状态：源码 C，安全压测 U。

**背景知识：** SGLang 的 HTTP API 入口接收 tokenize 请求，scheduler 执行 admission 与 prefix matching，worker 在 GPU 上执行 forward。PrefillAdder 控制 token budget、KV 容量、max-running-requests。`sglang/01-modules/M04-scheduler-batching/README.md`。grammar 编译消耗 CPU 和 cache 空间；speculative decoding 的 draft worker 共享 GPU 显存。`insights/05-sglang-inference-technical-insights.md` 的 grammar 与 speculative 段落。三层池模型的 ownership 约束（单 owner、lock_ref 配对）是资源隔离的基础。`sglang/90-cross-module/pooling-and-resource-management.md`。

**问题分析：** 1) 按数据平面、控制平面、资源平面系统枚举推理服务的攻击面；2) 分析限流与隔离的安全边界——什么可以在请求级阻止，什么必须在实例级阻止；3) 给出可验证的安全断言。

**具体答案：** 面试中可以这样回答：

> **攻击面系统枚举（按平面分层）：**
>
> | 平面 | 攻击面 | 攻击向量 | 影响 |
> |---|---|---|---|
> | 数据平面 | HTTP API 入口 | 超大 prompt（max_input_length 未校验）、畸形 JSON、超长流式连接 | 队列拒绝服务、OOM |
> | 数据平面 | Tokenizer | 特制的 Unicode/prompt 序列触发 tokenizer 指数级回溯 | CPU 饱和 |
> | 控制平面 | SamplingParams | 极端 temperature/top-p 值、极大 max_tokens | 资源耗尽（生成极长输出耗尽 KV） |
> | 控制平面 | Grammar | 复杂 regex/grammar schema 触发编译 cache 膨胀 | CPU/内存耗尽 |
> | 资源平面 | KV cache | 大量不同 prefix 请求耗尽 radix cache（eviction storm） | prefix hit 率崩溃，prefill 计算量激增 |
> | 资源平面 | Speculative draft | 特制输入降低 acceptance rate → verify 频繁回滚 | GPU 计算浪费 |
> | 跨平面 | Overlap future | 大量并发 structured requests → grammar compile future 堆积 | scheduler CPU 瓶颈，TTFT 劣化 |
>
> **限流的安全边界：**
>
> 限流在**请求级**可阻止的攻击：单请求资源超限（prompt 长度、max_tokens）、单 IP 请求频率。实现：admission 层的 `max_prefill_tokens`、`max_running_requests`、per-IP rate limiter。`sglang/01-modules/M04-scheduler-batching/README.md` 的 PrefillAdder。
>
> 限流在**实例级**才需要的攻击：分布式拒绝（大量 IP 各发少量请求）、KV cache 驱逐攻击（不同 prefix 的请求来自不同 IP）。实现：全局 admission rate limit + prefix-aware admission（限制低 prefix-hit 请求的并发）。
>
> **隔离的安全边界：**
>
> 请求级隔离（强）：每个请求的 KV slot 互斥——allocator 单 owner 保证一个请求无法读取另一请求的 KV。`sglang/90-cross-module/pooling-and-resource-management.md` 资源所有权表。
>
> 请求级隔离（弱）：radix cache 的 prefix sharing 使不同请求可能共享同一 KV node——若 A 的请求触发 eviction 且 B 的 prefix 依赖该 node，B 的下次请求需要重算。这不是安全漏洞但属于性能干扰。
>
> 实例级隔离：多租户场景需要物理隔离（不同的 Engine 实例）或逻辑隔离（tenant quota + priority-aware eviction）。`insights/04-design-decision-and-validation.md` 的多租户建议。

**技术洞察：**

```text
# 攻击面枚举框架
AttackSurface ⊆ DataPlane × ControlPlane × ResourcePlane

# DataPlane 攻击
  - Prompt_injection: length_exceed / malformed_unicode / adversarial_token
  - Streaming_abuse: slow_read_backpressure / half_open_connection

# ControlPlane 攻击
  - Parameter_abuse: max_tokens → ∞, temperature → NaN, regex_grammar_explosion
  - Admission_storm: burst_requests_exceed_scheduler_capacity

# ResourcePlane 攻击
  - KV_cache_eviction: unique_prefix_burst → hit_rate → 0
  - Grammar_cache_bloat: distinct_schemas_exceed_cache
  - Speculative_waste: adversarial_input_lowers_acceptance_rate

# 安全边界
Request_level_isolation ← allocator_single_owner (strong)
                       ∧ grammar_per_request_state (strong)
                       ∧ KV_no_cross_request_read (strong)

Instance_level_isolation ← tenant_quota (weak, best-effort)
                        ∨ physical_separation (strong)

# 不可在请求级阻止的攻击
Unstoppable_per_request ← distributed_unique_prefix_eviction
                         ∨ coordinated_grammar_cache_bloat
```

C：`sglang/01-modules/M04-scheduler-batching/README.md` 的 admission 约束；`sglang/90-cross-module/pooling-and-resource-management.md` 资源所有权表；`insights/05-sglang-inference-technical-insights.md` 的 grammar 段落。I：攻击面枚举与安全边界推导。U：各攻击向量的实际效果需安全压测验证。

**拓展知识：** 与 I-E007 衔接：资源平面攻击的放大因素（eviction storm → TTFT 劣化）可用三层故障模型分析；与 I-E001 衔接：allocator 单 owner 不变量是请求级隔离的安全基础。

**常见追问及回答要点：**

1. **问：prompt injection（越狱）与 prompt flooding（资源耗尽）的区别？** 答：prompt injection 是语义层面的攻击——恶意指令试图改变模型行为，需通过输入过滤/输出审核防御；prompt flooding 是资源层面的攻击——通过超大/特制 prompt 耗尽 tokenizer/KV/GPU 资源，需通过 admission budget 和 rate limiting 防御。两者属于不同安全平面。
2. **问：多租户场景下 priority-aware eviction 的安全风险？** 答：priority-aware eviction 中较小 priority 的请求先被驱逐——恶意租户可以发送大量低 priority 请求填满 KV cache，迫使高 priority 租户的请求被驱逐。安全边界需要 tenant-level KV quota（硬隔离）而非仅 priority（软隔离）。`sglang/01-modules/M08-kv-cache/README.md` 的 eviction policy 段落。
3. **问：grammar 编译 cache 的安全边界在哪？** 答：grammar cache 是跨请求共享的——恶意租户可发送大量不同 regex schema 填满 cache，驱逐其他租户的编译结果。安全边界：per-tenant grammar cache quota + cache entry TTL + 编译时间上限。

**易错点与反例：**

- 反例："请求级隔离能阻止所有跨请求攻击"——错误。radix cache prefix sharing 和 eviction storm 属于性能干扰而非安全漏洞，但可在请求级隔离之上叠加攻击效果。
- 反例："限流只放在 API 网关就够"——错误。API 网关的 rate limit 基于 IP/连接数，无法检测"大量不同 prefix 各发一个请求"的 KV cache 驱逐攻击——需要在 scheduler admission 层加 prefix-aware 限流。
- 易错：忽略 grammar cache 作为跨请求共享状态的安全意义——grammar cache 不受 allocator 单 owner 约束，是请求级隔离的盲区。

**实践与验证：** [U] 构造安全压测：发送 unique-prefix 请求 burst，测量 prefix hit 率与 TTFT 的下降曲线；发送 grammar schema burst，测量 grammar cache 命中率与 scheduler CPU 占用。oracle：prefix-aware 限流应在 prefix hit 率下降前触发限流。当前未执行安全压测。

**分层评分标准：** 4分：能列出推理服务的主要攻击面（数据/控制/资源平面）；6分：能分析限流与隔离的安全边界，区分请求级与实例级的防护能力；8分：能系统枚举所有攻击面并给出安全断言，分析多租户场景下的 attack surface 交叉，设计可验证的安全测试方案。

**证据与来源：** `sglang/01-modules/M04-scheduler-batching/README.md`（admission 约束）；`sglang/90-cross-module/pooling-and-resource-management.md`（资源所有权表）；`insights/05-sglang-inference-technical-insights.md`（grammar cache 与 sampling 段落）；`sglang/01-modules/M08-kv-cache/README.md`（eviction policy）。

### I-E010 从排队论角度：decode 阶段为何近似"带宽受限的单服务器"？这对 autoscaling 意味着什么？

**元数据：** 知识域：`I04 排队论与 autoscaling`；能力目标：`C04 从排队模型推导 autoscaling 策略`；题型：排队论建模与策略推导；频率：中频；前置/后续：`I-B001`（双阶段差异）、`I-E006`（容量约束）；证据状态：原理 C，压测 U。

**背景知识：** decode 阶段每步为所有活跃请求追加一个 token，计算量固定但需要读取全部 KV cache。算术强度 `I_decode ≈ FLOPs_decode / KV_cache_bytes ∝ 1/seq_len`，落在 roofline 的 bandwidth-bound 区。`sglang/02-request-flow/03-调度器与连续批处理.md` 的 prefill/decode 差异。continuous batching 在 iteration 边界重组 batch——动态性来自每轮重新计划，而非修改正在运行的 kernel。decode 阶段每个 step 必须处理所有 running 请求，不能像 prefill 那样选择性跳过。

**问题分析：** 1) 用排队论建模 decode 阶段——证明其等价于 M/G/1 或类似模型；2) 从模型推导服务容量与到达率的关系；3) 分析 autoscaling 的特殊性——decode 阶段的扩容触发条件和扩容有效性。

**具体答案：** 面试中可以这样回答：

> **decode 阶段建模为带宽受限的单服务器：**
>
> 将 GPU 视为一个服务器，decode 每个 step 视为一次"服务"。关键特征：
> 1. **单服务器**：decode step 是一个全局同步的 forward——所有 running 请求在一个 batch 中同时处理，不能像 prefill 那样部分处理。这等价于一个"单服务器服务整个队列"的模型。
> 2. **带宽受限**：decode 的 step 时间 `T_step ≈ N_active × KV_bytes_per_req / BW_hbm + fixed_overhead`。当 `N_active` 增长，`T_step` 线性增长——服务速率 `μ = N_active / T_step` 渐近饱和于 `BW_hbm / KV_bytes_per_req`。
> 3. **服务时间依赖队列长度**：与经典 M/M/1 不同，decode 的 service time 随 queue length (`N_active`) 增长——这是 M/G/1 with bulk service 或 processor sharing 模型的变体。
>
> **排队模型形式化：**
>
> ```text
> 到达过程: Poisson(λ) —— 新请求到达率
> 服务速率: μ(N) = BW_hbm / (N × KV_per_req)  ← 随 N 下降
> 稳定性条件: λ < μ(N_max) = BW_hbm / (N_max × KV_per_req)
> ```
>
> 当 `λ` 接近 `μ(N_max)` 时，`N_active` 趋于 `N_max`，TTFT 因 waiting queue 积压而急剧上升——这是"带宽墙"效应。
>
> **对 autoscaling 的含义：**
> 1. **扩容触发信号**：GPU utilization 接近 100%（decode bandwidth 饱和）且 P95 `N_active` 持续接近 `N_max` —— 而非简单的 CPU 利用率或队列长度。
> 2. **扩容有效性受限**：增加一个 decode 节点后，单节点 `N_active` 减半，`T_step` 近似减半——吞吐近似线性扩展。但前提是请求能有效路由到多个节点（无状态路由或 KV transfer），否则新节点空闲。
> 3. **扩容滞后代价**：新节点需要加载模型（秒级）、warmup CUDA Graph（秒级）、建立 prefix cache（分钟级）——在 bandwidth 饱和场景下，分钟级的扩容滞后可能导致大量请求 SLO 违反。
> 4. **缩容风险**：缩容需要将 running 请求的 KV 迁移到保留节点——若迁移过程中 running 请求的 KV 不一致，会违反 committed_len 不变量。

**技术洞察：**

```text
# Decode 阶段的排队模型
# 等价于 Processor Sharing (PS) 或 M/G/1 with state-dependent service rate

λ: request arrival rate (Poisson)
N: number of active decode requests (queue + in-service)
KV_per_req: per-request KV bytes = seq_len × C_token
BW_hbm: GPU memory bandwidth

T_step(N) = N × KV_per_req / BW_hbm + T_fixed
μ(N) = N / T_step(N) = BW_hbm / (KV_per_req + BW_hbm × T_fixed / N)

# 关键性质
lim_{N→∞} μ(N) = BW_hbm / KV_per_req  ← 渐近饱和
lim_{N→0} μ(N) = 1 / T_fixed           ← 轻载时受固定开销限制

# 稳定性边界
N_max_stable = (BW_hbm × SLO_TPOT - T_fixed × BW_hbm) / KV_per_req

# Autoscaling 触发条件
Scale_up ← (GPU_util > 0.95) ∧ (N_active > 0.8 × N_max) ∧ (P99_TPOT > SLO)
Scale_down ← (GPU_util < 0.4) ∧ (N_active < 0.3 × N_max)
```

C：`sglang/02-request-flow/03-调度器与连续批处理.md` 的双阶段差异；`sglang/01-modules/M08-kv-cache/README.md` 的 decode KV 读取模型。I：排队模型推导。U：实际 μ(N) 曲线与稳定性边界需压测验证。

**拓展知识：** 与 I-E006 衔接：排队模型的 `N_max_stable` 直接受 KV 容量约束；与 I-E005 衔接：PD 分离将 prefill 和 decode 的排队模型解耦——decode 节点可以独立 autoscale。

**常见追问及回答要点：**

1. **问：prefill 阶段能否用同样的排队模型？** 答：不能。prefill 是 compute-bound，服务时间取决于 prompt 长度而非队列长度——更接近 M/G/1 with variable service time。prefill 可以 chunked，服务可以被中断和恢复——这引入"可抢占"特性，与 decode 的"不可抢占"（所有 running 请求必须每步处理）根本不同。
2. **问：autoscaling 中 PD 分离 vs 混合部署的扩容效率比较？** 答：PD 分离允许 prefill 和 decode 节点独立 autoscale——decode 带宽饱和时只加 decode 节点，compute 饱和时只加 prefill 节点。混合部署的扩容粒度是"整个实例"——可能加了不需要的资源维度（如 decode 瓶颈但扩容了 prefill 能力）。
3. **问：continuous batching 对排队模型的影响？** 答：continuous batching 使 `N_active` 可以每轮变化（新请求 admission + 完成请求退出），等价于队列长度动态调整。相比 static batching（固定 batch 大小），continuous batching 减少了"等待凑满 batch"的空闲时间——使系统更接近 continuous service 而非 batch service。但在排队论意义上，这引入了"批量到达、逐个服务"与"逐个到达、批量服务"的混合特征，经典 M/M/1 的 Little's Law `L = λW` 仍然适用，但 `W`（等待时间）需要分解为 admission wait + decode time。
4. **问：autoscaling 的冷却时间（cooldown）如何设置？** 答：冷却时间应大于"新节点 ready 时间 + 状态恢复时间"——否则系统在扩容完成前再次触发扩容，导致过度配置。新节点 ready 包括模型加载（秒级）、CUDA Graph capture（秒级）、prefix cache warmup（分钟级，依赖流量重复度）。建议 cooldown = max(模型加载时间 × 2, 60s)——但实际值需根据历史扩容延迟分布校准。

**易错点与反例：**

- 反例："decode 阶段是 compute-bound，所以加 GPU 就能线性扩容"——错误。decode 是 bandwidth-bound，加 GPU 后单 GPU 的 KV 读取量不变——扩容有效性取决于请求路由（每个 GPU 的 `N_active` 是否真正减少）。PD 分离架构中 decode 节点可以独立扩容——这是其相对于混合部署的核心运维优势。

**与 GPU 实例类型的匹配：** decode 节点的带宽瓶颈意味着应选择高带宽 GPU（H100/H200 的 HBM bandwidth 显著高于 A100），而非更多 compute 的 GPU。`sglang/99-roadmap/performance-guide.md` 的分阶段测量法应在扩容决策前执行——先确认瓶颈是 decode bandwidth 而非 prefill compute，再选择对应的实例类型扩容。

**autoscaling 的稳定性陷阱：** 扩容触发后新节点需要 warmup（模型加载 + Graph capture + cache 预热）——如果冷却时间（cooldown）设置过短，系统在扩容完成前再次触发扩容，导致过度配置和成本浪费。建议 cooldown 基于历史 p95 扩容完成时间动态调整，而非固定值。

**spot/preemptible 实例的影响：** 若 decode 节点使用 spot 实例（成本更低但可能被 preempt），preemption 时的请求迁移需要 I-E008 的状态迁移协议。在 bandwidth-bound 的 decode 场景中，迁移一个 running batch 的成本很高（需要迁移 KV + committed state + sampling RNG）——应优先迁移 waiting queue 的请求（只需重新 admission），running batch 的请求等待当前 step 完成后再迁移。

**排队论模型的实际修正：** 实际的 decode 阶段更接近"处理器共享"（Processor Sharing）模型——所有 active 请求在每轮 step 中同时获得"服务"（而非顺序服务），但每轮服务量（token 数）固定为 1。这意味着在 PS 模型下，每请求的平均等待时间为 `N_active × T_step / 2`——与 M/M/1 不同，PS 的平均响应时间随 `N_active` 线性增长而非发散。`insights/05-sglang-inference-technical-insights.md` 的 continuous batching 描述支持 PS 模型——请求在 iteration 边界加入/退出 batch，类似于 PS 模型下的到达/离开。
- 反例："autoscaling 只需看 GPU utilization"——错误。GPU utilization 100% 不一定需要扩容——可能是某个 long prefill 短暂占用 GPU。正确的信号是 `N_active` 接近 `N_max` 且 TPOT 趋势上升。生产中最有效的扩容触发器是"P99 TPOT > SLO" 且 "N_active > 0.8 * N_max"的联合条件——单一指标容易产生误触发。
- 反例："autoscaling 只需看 GPU utilization"——错误。GPU utilization 100% 不一定需要扩容——可能是某个 long prefill 短暂占用 GPU。正确的信号是 `N_active` 接近 `N_max` 且 TPOT 趋势上升。
- 易错：混淆 prefill 和 decode 的扩容策略——prefill 扩容改善 TTFT，decode 扩容改善 TPOT/并发。混合部署中一个维度的扩容可能不改善另一个维度的 SLO。

**实践与验证：** [U] 固定模型与硬件，逐步增加请求到达率 λ，测量 `N_active`、TPOT、TTFT 的变化曲线。绘制 μ(N) 实测曲线与理论模型的对比。oracle：TPOT 在 `N_active` 达到 `N_max_stable` 前保持稳定，之后急剧上升。当前未执行 GPU 实验。

**分层评分标准：** 4分：能说明 decode 阶段的 bandwidth-bound 特性与 roofline 关系；6分：能建立排队模型，推导稳定性边界与 autoscaling 触发条件；8分：能分析 PD 分离对 autoscaling 粒度的改善，比较 continuous batching 与 static batching 的排队差异，并给出扩容滞后和缩容风险的缓解策略。

**证据与来源：** `sglang/02-request-flow/03-调度器与连续批处理.md`（双阶段差异）；`sglang/01-modules/M08-kv-cache/README.md`（decode KV 读取模型）；`insights/05-sglang-inference-technical-insights.md`（insight 三：prefill/decode 是两种资源模型）；`sglang/99-roadmap/performance-guide.md`（分阶段测量）。

### I-E011 从存储层次角度：KV cache 的放置策略（HBM/DRAM/远程）如何影响服务 SLO？推导放置判据。

**元数据：** 知识域：`I08 KV Cache 分层放置`；能力目标：`C08 从存储层次推导放置判据`；题型：存储层次建模与 SLO 分析；频率：高频；前置/后续：`I-B003`（KV cache 概念）、`I-E006`（容量约束）；证据状态：论文 C，跨机传输 U。

**背景知识：** Dynamic KV Cache Placement 论文研究在 HBM 与 off-package DRAM 间动态放置 KV cache，目标是在容量约束下最大化有效带宽/降低 latency。`insights/arxiv-pdf/2508.13231`。NEO 使用 CPU offload 扩大 batch size但引入 PCIe 延迟。`insights/arxiv-pdf/2411.01142-neo-cpu-offloading.md`。SGLang 的 HiCache 增加 host/storage 层，执行 prefetch/restore/eviction。`sglang/01-modules/M13-disaggregation-hicache/README.md`。P/D-Serve 关注跨节点 KV transfer 在 RDMA 上的利用率。`insights/arxiv-pdf/2408.08147-pd-serve.md`。

**问题分析：** 1) 建立 KV cache 的存储层次模型——HBM/DRAM/远程三级的带宽、延迟、容量参数；2) 推导 placement 判据——给定请求的访问模式，KV 应放在哪一级；3) 分析放置策略对 TTFT/TPOT/P99 的影响。

**具体答案：** 面试中可以这样回答：

> **存储层次参数化：**
>
> | 层级 | 典型带宽 | 延迟 | 容量 | 成本/bit |
> |---|---|---|---|---|
> | HBM (GPU) | ~2-3 TB/s | ~100 ns | 40-80 GB | 高 |
> | DRAM (CPU) | ~100-200 GB/s | ~100 ns | 100s GB | 中 |
> | 远程 (RDMA) | ~100-400 Gb/s | ~1-100 μs | ∞ | 低 |
>
> **放置判据推导：**
>
> 设请求 r 在 decode 步 t 需要读取 KV chunk B_r 字节。定义放置决策变量 `x_r ∈ {HBM, DRAM, Remote}`。
>
> ```text
> T_access(x_r) = latency[x_r] + B_r / bandwidth[x_r]
>
> 决策规则：
> x_r = HBM  if T_SLO_TPOT ≥ T_access(HBM)  (最快但容量有限)
> x_r = DRAM if T_access(DRAM) < T_SLO_TPOT ∧ HBM_full
> x_r = Remote if T_access(Remote) < T_SLO_TPOT ∧ DRAM_full
> else reject
> ```
>
> **判据的简化形式（仅考虑带宽）：**
>
> ```text
> 放置层级 = argmin_{tier ∈ {HBM, DRAM, Remote}}
>            { access_time(tier) + transfer_time(tier) }
>
> 其中 transfer_time 在首次放置或 restore 时非零。
>
> 对于 decode 阶段（带宽受限）：
> effective_bandwidth = min(BW_tier, BW_transfer)
> ```
>
> **对 SLO 的影响：**
> 1. **TTFT**：prefill 产生的 KV 必须写入 HBM（attention 需要低延迟读取），不能 offload——prefill 写入 DRAM 会因 PCIe 带宽低于 HBM 而成为瓶颈。
> 2. **TPOT**：decode 读取 KV 可以容忍 DRAM 延迟（`T_access(DRAM) ≈ 100ns << TPOT_SLO` 通常 ≥ 10ms），但若 DRAM 带宽不足（多个并发请求争用 PCIe），TPOT 会恶化。
> 3. **P99**：offload/restore 引入的异步传输在 queue backlog 时产生尾延迟抖动——GPU 可能在等待 KV restore 时空转。`insights/arxiv-pdf/2411.01142-neo-cpu-offloading.md` 的非对称流水设计可缓解。
>
> **放置动态性：** 年轻请求（短 seq_len）放 HBM（访问密集），老年请求（长 seq_len）offload 到 DRAM（释放 HBM 给新请求）。这等价于 LRU 但按"访问密度"（accesses per byte）排序而非最后访问时间。

**技术洞察：**

```text
# KV placement 的优化问题
# 目标：min Σ T_access(r) for all r ∈ active_requests
# 约束：Σ size(r) for r in tier ≤ capacity(tier) for each tier

# 贪心放置（按访问密度排序）
access_density(r) = expected_future_accesses(r) / size(r)
Sort active_requests by access_density DESC
Place top-K into HBM, next-M into DRAM, rest to Remote

# 放置收益
Benefit(r, tier) = T_access(HBM) - T_access(tier)  # 时间节省
Cost(r, tier) = transfer_time(r, tier)              # 迁移成本

# 净收益为正的条件
Place_in_tier(r) if Σ_future_accesses(r) × Benefit(r, tier) > Cost(r, tier)

# 对 TPOT SLO 的影响
TPOT_with_placement = T_compute + max_over_r(T_access(tier(r)))
                    ≈ T_compute + T_access(HBM)  # 大多数 r 在 HBM
TPOT_degradation = P(DRAM_access) × (T_DRAM - T_HBM)
                 + P(Remote_access) × (T_Remote - T_HBM)
```

C：`insights/arxiv-pdf/2508.13231` 的放置建模；`sglang/01-modules/M13-disaggregation-hicache/README.md` 的 HiCache staging/restore 流程。I：放置判据推导。U：具体带宽/延迟参数与放置策略效果需异构内存硬件验证。

**拓展知识：** 与 I-E004 衔接：KV 量化减少每 token bytes，使同一 KV chunk 可以放在更快层级或同一层级容纳更多请求；与 I-E005 衔接：PD 分离中的 KV transfer 是"远程放置"的一种特殊形式。

**常见追问及回答要点：**

1. **问：什么情况下 offload 到 DRAM 反而降低 TPOT？** 答：当并发请求数大、每个请求的 KV 持续被访问时——多个请求同时 restore 争用 PCIe 带宽，排队延迟导致 GPU 等待。NEO 的缓解策略是 asymmetric GPU-CPU pipelining：部分请求走 GPU attention，部分走 CPU attention，减少对 PCIe 的单向依赖。`insights/arxiv-pdf/2411.01142-neo-cpu-offloading.md`。
2. **问：prefill 产生的中间 activation 是否也可以 offload？** 答：理论上可以（eLLM 的 elastic memory 框架将 activation 和 KV 统一管理），但 attention 的中间 activation 只在当前 forward 步骤中使用——offload 的传输成本远超计算节省。只有 KV cache 具有"跨步骤复用"特性，才值得 offload。`insights/arxiv-pdf/2506.15155-ellm-elastic-memory.md`。
3. **问：KV placement 与 prefix caching 的关系？** 答：共享前缀的 KV 在 radix cache 中被多个请求引用（lock_ref > 1）——这类 KV 应优先放在 HBM 以避免多个请求同时 restore 同一 chunk。这是"按访问密度排序"的极端情况——access_density 被共享放大。

**易错点与反例：**

- 反例："DRAM 容量大，应该把所有 KV offload 到 DRAM"——错误。DRAM 带宽远低于 HBM——offload 全部 KV 会使 decode 带宽瓶颈从 HBM 转移到 PCIe，TPOT 严重恶化。
- 反例："KV offload 只影响容量不影响延迟"——错误。异步 restore 在 backlog 时产生 P99 尾延迟抖动——GPU 在等 KV 时 idle。
- 易错：混淆 KV placement 与 KV quantization——placement 解决"KV 放在哪"，quantization 解决"每 token 占多少字节"。两者正交：量化后的 KV 可以放在同一层级（容纳更多）或放到更慢层级（节省 HBM 给未量化数据）。

**实践与验证：** [U] 在 HBM/DRAM 异构系统（如 NVIDIA Grace Hopper）上测量 KV 全部在 HBM vs 部分 offload 到 DRAM 的 TPOT/P99 对比。oracle：offload 低访问密度请求后 TPOT 不变但并发容量增加；offload 高访问密度请求后 TPOT 恶化。当前未执行异构内存实验。

**分层评分标准：** 4分：能说明 KV cache 三级存储层次的基本参数与放置动机；6分：能推导放置判据的量化公式，分析对 TTFT/TPOT/P99 的非对称影响；8分：能建立访问密度的动态放置模型，分析 offload 对 P99 尾延迟的影响，并讨论与 prefix caching 的交互。

**证据与来源：** `insights/arxiv-pdf/2508.13231`（Dynamic KV Cache Placement 数学建模）；`insights/arxiv-pdf/2411.01142-neo-cpu-offloading.md`（NEO 非对称流水）；`sglang/01-modules/M13-disaggregation-hicache/README.md`（HiCache staging/restore）；`sglang/90-cross-module/pooling-and-resource-management.md`（三层池模型与 HiCache 集成）。

### I-E012 从 Amdahl 角度：推理并行的理论上限如何受 KV cache 带宽约束？给出扩展效率公式。

**元数据：** 知识域：`I10 推理并行扩展理论`；能力目标：`C10 从 Amdahl 定律推导 KV 带宽约束下的扩展效率`；题型：理论建模与公式推导；频率：中频；前置/后续：`I-B004`（并行策略）、`I-E006`（容量约束）；证据状态：源码 C，多卡性能 U。

**背景知识：** SGLang 的并行维度包括 TP（切分 linear/attention head）、PP（切分 layer range）、DP/DP Attention（切分 token/KV 工作集）、CP/DCP（切分序列维度）、EP/DP/TP（MoE 专家维度）。`sglang/01-modules/M20-parallel-strategies/README.md`。TP 的 all-gather/all-reduce 在 decode 阶段每步都发生；DP Attention 的 `(dp, cp, tp)` 布局使每个 DP replica 处理不同 token/KV。decode 阶段是 bandwidth-bound——KV cache 的读取量随序列增长。

**问题分析：** 1) 用 Amdahl 定律建模推理并行的加速比——识别不可并行化的串行部分；2) 推导 KV cache 带宽如何成为串行瓶颈——TP all-gather 每步传输 KV 的不可缩减开销；3) 给出扩展效率公式并分析其随并行度下降的规律。

**具体答案：** 面试中可以这样回答：

> **Amdahl 定律回顾：** 加速比 `S = 1 / (f_serial + f_par / N)`，其中 `f_serial` 是必须串行执行的比例，`f_par` 是可并行部分，`N` 为处理器数。
>
> **推理并行的"串行部分"来源：**
>
> 1. **TP all-reduce / all-gather**：TP 在 decode 阶段每步执行 all-reduce（row-parallel output 聚合）和 all-gather（attention TP hidden-state gather），通信量不随 TP size 减少——这是固定的 `f_serial`。
> 2. **PP pipeline bubble**：PP 的 bubble 率 ≈ `(PP_size - 1) / num_microbatches`——microbatch 数有限时 bubble 不可消除。
> 3. **DP Attention gather/reduce-scatter**：DP Attention 需要在 attention TP 间聚合 token buffer，通信量与 DP size 成正比。
>
> **KV cache 带宽约束下的扩展效率：**
>
> 设 decode 单步时间为 `T_step = T_compute(N) + T_comm(N)`。
> - `T_compute(N) = T_compute(1) / N`（理想并行，compute 可分）
> - `T_comm(N) = α + β × KV_total / N`（KV all-gather 传输 per-rank KV）
>
> 其中 `α` 为通信延迟（固定开销），`β` 为通信带宽倒数，`KV_total` 为 batch 的总 KV bytes。
>
> ```text
> Speedup(N) = T_step(1) / T_step(N)
>            = (T_compute(1) + T_comm(1)) / (T_compute(1)/N + T_comm(N))
>
> Efficiency(N) = Speedup(N) / N
>               = (T_compute(1) + T_comm(1)) / (T_compute(1) + N × T_comm(N))
> ```
>
> **当 decode 为 bandwidth-bound 时**：`T_compute(1) << T_comm(1)`（计算远快于通信），公式简化为：
>
> ```text
> Efficiency(N) ≈ T_comm(1) / (N × T_comm(N))
>               ≈ (α + β × KV_total) / (N × (α + β × KV_total / N))
>               ≈ (α + β × KV_total) / (Nα + β × KV_total)
> ```
>
> 当 `N → ∞` 时：`Efficiency ≈ (α + β × KV_total) / (Nα) → 0`——TP 的 all-gather 延迟 `α` 使效率趋于零。
>
> **KV 量化对扩展效率的影响**：FP8 KV 使 `KV_total` 减半，`β × KV_total` 下降——在 bandwidth-bound 场景下效率提升。

**技术洞察：**

```text
# Amdahl 扩展效率（KV bandwidth 约束）
T_total(N) = T_compute/N + T_comm(N)
           = T_compute/N + α + β × KV_per_rank(N)

# KV_per_rank 在不同并行策略下的行为
# TP: KV_per_rank = KV_total / TP_size (attention heads 分片)
# DP Attention: KV_per_rank = KV_total / DP_size (token 分片)
# CP: KV_per_rank = KV_total / CP_size (序列分片)
# PP: KV_per_rank = KV_total (每 rank 持有全量 KV，仅 compute 分片)

# 扩展效率公式（bandwidth-bound 近似）
E(N) = S(N)/N = T_1 / (T_compute + N × T_comm(N))

# 临界并行度（效率降至 50%）
N_50% = (T_compute + T_comm(1)) / α

# 关键推论
# 1. 大 KV_total → β×KV_total 主导 → 效率对 N 更敏感
# 2. 高带宽网络（低 α, β）→ N_50% 更大 → 可扩展性更好
# 3. KV 量化降低 KV_total → 效率曲线右移
# 4. PP 不减少 KV_per_rank → 在 decode 阶段无带宽收益
```

C：`sglang/01-modules/M20-parallel-strategies/README.md` 的并行维度与 collective 通信模式；`sglang/01-modules/M07-分布式并行.md` 的 TP all-gather/reduce-scatter。I：Amdahl 推导与 KV 带宽约束。U：具体效率曲线需多卡实验验证。

**拓展知识：** 与 I-E006 衔接：长上下文使 `KV_total` 增长，`β × KV_total` 主导效率下降更严重；与 I-E011 衔接：KV placement 在 DRAM 会进一步降低有效带宽，恶化扩展效率。

**常见追问及回答要点：**

1. **问：为什么 PP 在 decode 阶段几乎没有带宽收益？** 答：PP 按 layer range 切分——每个 PP stage 仍然需要读取全量 KV cache（KV 不随 layer 切分）。PP 减少的是 compute（每个 rank 只算部分 layers），但 decode 是 bandwidth-bound，compute 减少对 `T_step` 影响有限。PP 的真正优势在 prefill（compute-bound）阶段——但 prefill 使用 PP 会增加 pipeline bubble。`sglang/01-modules/M20-parallel-strategies/README.md` 的 PP 描述。
2. **问：CP（Context Parallel）在 decode 阶段的扩展效率如何？** 答：CP 将序列维度切分——每个 CP rank 只持有 `seq_len / CP_size` 的 KV——等效于将 `KV_total` 降为 `KV_total / CP_size`。这使得 `T_comm(N) = α + β × KV_total / (CP_size × N)`——比 TP 的带宽开销更小（CP 的通信主要是 attention 的 all-gather，而非 per-step 的 all-reduce）。但 CP 需要 attention backend 支持 ragged/split-sequence metadata。
3. **问：MoE 模型的 EP（Expert Parallel）对 KV 带宽的影响？** 答：EP 切分 expert 维度——每个 EP rank 只持有部分 expert 的权重，但 KV cache（attention 层产生）不切分。因此 EP 不影响 KV 带宽——KV 带宽仍然是全量 per-rank。EP 减少的是 MoE FFN 的计算量和 expert weight 显存。

**易错点与反例：**

- 反例："TP size 越大，decode 越快"——错误。TP 增大降低 compute 但增加 all-reduce 通信；decode 是 bandwidth-bound，compute 不是瓶颈——增大 TP 可能使通信成为新瓶颈。
- 反例："KV cache 带宽只影响不影响扩展性"——错误。KV cache 带宽是 decode 阶段的核心资源——扩展效率在 bandwidth-bound 场景下直接受 KV all-gather/reduce-scatter 通信量约束。
- 易错：混淆 prefill 和 decode 的并行策略选择——prefill 是 compute-bound（TP/PP 有效），decode 是 bandwidth-bound（DP Attention/CP 更有效，PP 几乎无效）。`sglang/01-modules/M20-parallel-strategies/README.md` 的并行维度分层。

**实践与验证：** [U] 固定模型（Llama-3-70B），测量 TP=1/2/4/8 在 decode 阶段的 TPOT 与效率。绘制 `Efficiency(N)` 曲线与理论公式对比。oracle：TP=2 时效率最高，TP≥4 时效率下降（all-reduce 开销主导）。当前未执行多 GPU 实验。

**分层评分标准：** 4分：能说明 Amdahl 定律的基本形式与推理并行的维度；6分：能推导 KV cache 带宽约束下的扩展效率公式，分析各并行策略对 KV 带宽的影响差异；8分：能给出参数化的效率公式，推导临界并行度，分析 KV 量化/placement 对效率曲线的移动效果，并给出生产配置的并行策略选择建议。

**证据与来源：** `sglang/01-modules/M20-parallel-strategies/README.md`（并行维度与 collective）；`sglang/01-modules/M07-分布式并行.md`（TP all-gather/reduce-scatter 通信模式）；`sglang/01-modules/M08-kv-cache/README.md`（KV bytes per token）；Amdahl 1967 "Validity of the Single Processor Approach to Achieving Large Scale Computing Capabilities"。
