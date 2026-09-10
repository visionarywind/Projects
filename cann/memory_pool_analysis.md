# Runtime 显存池设计与源码分析

本文针对当前工作区中的 Runtime SOMA（Stream Ordered Memory Allocation）显存池实现，目标是让第一次阅读代码的人理解：

1. 显存池为什么要分成 Runtime 软件层和 Driver/HAL 设备层；
2. 一个分配请求如何从 API 走到设备；
3. Segment、空闲集合、分配表和 stream/event 依赖分别解决什么问题；
4. 这些数据结构的复杂度、并发代价和当前源码中的风险。

分析范围是 Runtime/Driver 适配层。内核态驱动最终如何选择物理页，不在当前源码中，因此只依据 HAL 契约分析到 halMemPoolMalloc/Free。

## 一、先看设计思路：为什么是“两级显存池”

普通 rtMalloc 每次都向驱动申请一块独立内存，频繁申请/释放会产生驱动调用开销、地址映射开销和碎片。SOMA 的思路是：

1. 创建池时，一次性向 Driver 预留一大段连续 VA，并创建一个 HBM HugePage 池；
2. Runtime 在这段地址范围内切分小块，自己维护每个小块的状态；
3. 分配/释放只修改 Runtime 的 segment 元数据；
4. 真正的设备映射和解除映射通过 AICPU 的 SomaMemMng 任务，在指定 stream 上执行。

可以把它理解成仓库管理：

- Driver/HAL 池是一个大仓库；
- Segment 是仓库中的货架；
- SegmentManager 是仓库管理员；
- freeSegs_ 是当前可以直接取货的货架；
- cachedSegs_ 是已经归还但因为 stream 依赖暂时不能立即复用的货架；
- allocedMap_ 是“用户地址到货架”的快速索引；
- PoolRegistry 是所有仓库的总目录。

Runtime 逻辑架构文档也明确把 MemoryPool 放在 Runtime 核心层，把 HAL 作为 Driver 适配入口，见 [architecture.md](/home/mtuser/workspace/cann/runtime/docs/zh/design/architecture.md:11)。

## 二、用一个例子走一遍

假设创建一个 64MB 显存池，最小块和池对齐粒度都是 2MB。

初始时只有一个 segment：

~~~text
0x10000000 ─────────────────────────────── 64MB ─────────────────────────────── 0x14000000
             [ FREE ]
             freeSegs_ = { (64MB, 0x10000000) }
             cachedSegs_ = { }
             allocedMap_ = { }
~~~

### 1. Stream 7 申请 8MB

SegmentManager 从 freeSegs_ 找到最小的可容纳块，左侧切出 8MB：

~~~text
[ BUSY, stream=7, 8MB ] [ FREE, 56MB ]
 0x10000000              0x10800000
~~~

此时：

~~~text
allocedMap_[0x10000000] = Segment(8MB, BUSY)
freeSegs_ = { (56MB, 0x10800000) }
busySize_ = 8MB
reserveSize_ = 8MB
~~~

注意：size_ 表示整个池的 VA 范围（64MB）；reserveSize_ 表示当前已提交/保留的 segment 总量（源码注释为 allocated + cached），初始值为 0，分配新 FREE segment 时才递增。因此 reserveSize_ 不等于整个 VA 池大小，也不一定等于当前 busySize_。

### 2. Stream 7 再申请 4MB

继续从剩余 FREE segment 左侧切分：

~~~text
[ BUSY 8MB, s7 ] [ BUSY 4MB, s7 ] [ FREE 52MB ]
0x10000000       0x10800000          0x10c00000
~~~

### 3. 异步释放 8MB

rtMemPoolFreeAsync 默认不是 forceFree。Runtime 从 allocedMap_ 删除该地址，然后把它标为 CACHED：

~~~text
[ CACHED 8MB, s7 ] [ BUSY 4MB, s7 ] [ FREE 52MB ]
~~~

为什么不马上进入 FREE？因为设备上可能仍有 stream 7 的任务使用这块地址。只有满足 stream 顺序或 event 依赖后，其他 stream 才能安全复用。

### 4. 再申请 4MB

设计意图是：

1. 先从 cachedSegs_ 中找大小足够的块；
2. 如果同一个 stream，直接复用；
3. 如果是不同 stream，检查 event/sequence 依赖；
4. 复用时再切分，并把 reuse flag 传给设备侧。

但是当前源码中 SegmentManager::TryToReuse 直接返回 nullptr，见 [stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:140)。因此当前实际流程会跳过 CACHED 块，只尝试 freeSegs_。如果 FREE 块耗尽，可能返回 RT_ERROR_MEM_POOL_ALLOC。这是理解当前代码时必须区分的“设计预留”和“实际行为”。

### 5. 异步设备流程

一次 rtMemPoolMallocAsync 不只是修改 Host 侧数据结构：

~~~text
rtMemPoolMallocAsync
  → SegmentManager::SegmentAlloc
  → halMemPoolAsyncConfig(..., false)
  → 在目标 stream 提交 SomaMemMng/MALLOC
  → AICPU SomaMemMngTsKernel
  → halMemPoolMalloc(pool, va, size, subCmd)
~~~

释放流程类似：

~~~text
rtMemPoolFreeAsync
  → SegmentFree（软件状态变为 CACHED 或 FREE）
  → halMemPoolAsyncConfig(..., true)
  → SomaMemMng/FREE
  → halMemPoolFree(pool, va, size, subCmd)
~~~

AICPU 侧的实际调用在 [hwts_kernel_soma.cpp](/home/mtuser/workspace/cann/runtime/src/aicpu_sched/aicpu_schedule/core/hwts_kernel/hwts_kernel_soma.cpp:26)。

## 三、关键数据结构

### 1. Segment：地址顺序的双向链表节点

Segment 定义在 [stream_mem_pool.hpp](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.hpp:51)：

~~~text
Segment
├── basePtr       这块内存的起始 VA
├── size          这块内存的大小
├── prev / next   地址相邻的前后 segment
├── streamId      最后一次使用它的 stream
├── graphId       所属 graph
├── eventId       关联 event
├── seqId         stream 序列号
└── state         FREE / CACHED / BUSY
~~~

双向链表的作用是快速找到物理地址相邻的 segment。它解决的是“合并邻居”和“切分地址区间”，不是查找最合适大小的块。

SegmentState 有三种状态：

- FREE：可以直接再次分配；
- BUSY：已经返回给用户；
- CACHED：软件上已释放，但需要等待 stream/event 依赖后复用。

### 2. SegmentManager：一个池的本地分配器

SegmentManager 的核心成员见 [stream_mem_pool.hpp](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.hpp:122)：

~~~text
allocedMap_ : unordered_map<basePtr, Segment*>
freeSegs_   : set<Segment*, (size, basePtr)>
cachedSegs_ : set<Segment*, (size, basePtr)>

Segment 链表：按 basePtr 的地址顺序
state_      ：stream/event reuse 配置
busySize_   ：BUSY 总字节数
reserveSize_：已保留总字节数
~~~

这里有三个不同索引：

1. allocedMap_ 按地址索引，释放时可以 O(1) 找到 segment；
2. freeSegs_ 按大小索引，分配时可以做 best-fit；
3. Segment 的 prev/next 按地址索引，合并时可以直接找邻居。

这是一种典型的“多索引维护”：每个索引服务一种操作，但每次切分、合并、状态转移都必须同步更新多个容器。

### 3. PoolRegistry：多个池的全局目录

PoolRegistry 的成员见 [stream_mem_pool.hpp](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.hpp:180)：

~~~text
entries_        : set<SegmentManager*>，按池起始地址排序
poolOwnership_ : unordered_map<SegmentManager*, shared_ptr<SegmentManager>>
eventsMap_      : eventId → (streamId, seqId)
sequenceMap_    : (consumerStream, producerStream) → seqId
streamSeqId_    : streamId → 当前序号
~~~

entries_ 用于根据任意指针定位所属池。查找逻辑是先找不大于该地址的前驱池，再判断：

~~~text
poolBase <= ptr < poolBase + poolSize
~~~

poolOwnership_ 解决池对象的生命周期问题，避免 Registry 只保存裸指针。

## 四、关键源码的逐行/分段分析

下面按执行顺序分析关键代码。这里的“逐行”以每条有独立语义的源码行为为单位，而不是机械重复日志和括号。

### 1. Segment 构造和 SplitLeft

[stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:22) 的构造函数把新 segment 初始化为：

~~~text
basePtr = 传入起始地址
size    = 传入大小
prev/next = nullptr 或传入邻居
streamId/graphId/eventId/seqId = 无效值
state = FREE
~~~

这保证了新建的块默认是“未分配且没有依赖”的。

SplitLeft 位于 [stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:46)：

1. requested size 大于当前 size：返回 nullptr，表示无法切分；
2. requested size 等于当前 size 且不要求强制切分：直接返回当前节点；
3. 否则 new 一个左侧 segment；
4. 新 segment 继承原 segment 的 eventId、streamId、seqId；
5. 修正 prev->next 和当前节点的 prev；
6. 当前节点的 size 减少，basePtr 向右移动；
7. 返回左侧新节点。

这段代码只改变链表和大小，不操作 Driver，也不改变 free/cached 集合。集合的插入和删除由调用者负责。

复杂度为 O(1)，但会产生一次 Host 侧 Segment 对象分配。

### 2. SegmentManager 构造

[stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:80) 的构造函数：

1. 保存传入的初始 segment；
2. 从 segment 设置 base_ 和 size_；
3. 强制把初始 segment 状态设为 FREE；
4. 插入 freeSegs_。

因此一个新池的 Runtime 元数据初始状态只有一个大 FREE 块，后续通过 SplitLeft 逐渐产生更多节点。

### 3. TryToReuse：当前实现的关键缺口

[stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:140) 当前实现只做了四件事：

~~~text
忽略 size
忽略 streamId
忽略 state
忽略 flag
返回 nullptr
~~~

这意味着已经写好的 SingleStreamReuse、StreamEventReuse、StreamInternalReuse 不会被 SegmentAlloc 调用到。它们更像是后续实现预留的策略函数。

### 4. SegmentAlloc

[stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:150) 的执行顺序如下：

1. 检查 tail_ 是否为空；
2. 检查是否为不支持分配的 IPC pool；
3. 获取 SegmentManager 的 mutex_；
4. 调用 TryToReuse；
5. 若没有复用块，调用 AllocFromFreeSegs；
6. 将返回块标记为 BUSY；
7. 写入 streamId；
8. 插入 allocedMap_；
9. 增加 reserveSize_；
10. 若复用 cached 块，则从 cachedSegs_ 删除；
11. 对 cached 块执行 SplitLeft；
12. 将新块标记为 BUSY 并插入 allocedMap_；
13. 增加 busySize_ 和峰值 maxBusySize_。

值得注意的是，当前代码在分配新 FREE 块时增加 reserveSize_，而从 cachedSegs_ 复用时不增加 reserveSize_。这符合“cached 已经计入 reserve”的定义。

### 5. AllocFromFreeSegs：best-fit

[stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:346)：

1. 构造一个 size 等于请求大小的临时 Segment；
2. 调用 freeSegs_.lower_bound；
3. 找到第一个 size 不小于请求的块；
4. 从 freeSegs_ 删除；
5. 调用 SplitLeft；
6. 如果右侧仍有剩余块，将剩余块重新插入 freeSegs_；
7. 返回左侧块。

这是按大小排序的 best-fit 近似实现。set 的成员函数 lower_bound 是 O(log f)，其中 f 是 FREE 块数量。

### 6. SegmentFree：普通释放和强制释放

[stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:198)：

1. 获取 mutex_；
2. 在 allocedMap_ 中查找精确的 basePtr；
3. 找不到则返回 RT_ERROR_POOL_PTR_NOTFOUND；
4. 从 allocedMap_ 删除；
5. busySize_ 减少；
6. forceFree=true：状态变为 FREE，并调用 MergeIntoFreeSegs；
7. forceFree=false：读取 stream 序列号，状态变为 CACHED，并调用 MergeIntoCachedSegs。

这里要求用户传入的是 segment 的起始地址，而不是 segment 内部任意地址。PoolRegistry 可以识别“地址是否属于某个池”，但 allocedMap_ 只保存精确基地址。

### 7. CheckMergeRules：为什么不是所有邻居都能合并

[stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:336) 要求：

~~~text
两者都不是 tail_
state 相同
streamId 相同
graphId 相同
eventId 相同
seqId 相同
~~~

这样做的目的不是单纯减少碎片，而是避免把具有不同执行依赖的内存块合并后错误复用。

例如两个 CACHED 块虽然地址相邻，但分别来自不同 stream，或者对应不同 seqId，就不能简单合并，否则会丢失依赖信息。

### 8. MergeIntoFreeSegs / MergeIntoCachedSegs

两个函数都采用相同模式：

1. 向前检查邻居；
2. 如果邻居状态和依赖满足条件，从对应 set 删除；
3. 调用 MergeLeft 合并；
4. 再向后检查邻居；
5. 最后把合并后的大块重新插入 set。

链表调整是 O(k)，但每次从 set 删除都需要 O(log n)，因此一次合并 k 个邻居的复杂度约为 O(k log n)。

### 9. 三种复用策略

SingleStreamReuse [stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:246)：

- 从 cachedSegs_ 找 size 足够的候选；
- 只接受 segment->streamId == 当前 streamId；
- 成功返回 REUSE_FLAG_STANDARD。

StreamInternalReuse [stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:310)：

- 找到大小足够的 cached 块；
- 不要求同一个 stream；
- 根据 reuseType 返回 STANDARD 或 INTERNAL。

StreamEventReuse [stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:276)：

- 复制 PoolRegistry 的 sequenceMap；
- 遍历大小足够的 cached 块；
- 查找当前 stream 与原 stream 的序列关系；
- 只有 map 中的序号已经覆盖 segment->seqId 时才允许复用。

三者都使用通用 std::lower_bound(cachedSegs_.begin(), cachedSegs_.end(), ...)。cachedSegs_ 是 std::set，但 set 迭代器是双向迭代器，所以迭代器移动是线性的；不能简单按 O(log c) 估计整个函数。

### 10. TrimCachedSegs

[stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:383) 从 cachedSegs_.begin() 开始清理：

1. 只要 reserveSize_ 大于目标值，就继续；
2. 如果当前 cached 块完全可以释放，直接从 cached set 删除；
3. 如果释放量只需要该块的一部分，则先 SplitLeft；
4. 被释放的部分进入 freeSegs_；
5. 更新 reserveSize_。

它优先从按大小排序的 set 起点处理小块。这样可以精确达到目标，但会产生多次 set erase/insert。

### 11. PoolRegistry 地址查找

InMemPoolRegion [stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:624)：

1. 获取 Registry mutex；
2. 对按池起始地址排序的 entries_ 调用 upper_bound；
3. 回退到前一个池；
4. 判断 ptr 是否小于 poolBase + poolSize。

复杂度是 O(log p)，p 为池数量。

FindMemPoolByPtr [stream_mem_pool.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/stream_mem_pool.cc:640) 也是 O(log p)，之后通过 poolOwnership_ 做一次平均 O(1) 的 HashMap 查找。

### 12. rtMemPoolMallocAsync 的 API 到 AICPU

ApiImplSoma::MemPoolMallocAsync [api_impl_soma.cc](/home/mtuser/workspace/cann/runtime/src/runtime/api/impl/api_impl_soma.cc:65)：

1. 检查 devPtr、poolId、stream；
2. 检查 stream 是否属于当前 Context；
3. 按 DEVICE_POOL_MIN_BLOCK_SIZE（2MB）向上对齐；
4. 通过 PoolRegistry 查询池；
5. 调用 SomaApi::AllocFromMemPool；
6. 设置 halMemPoolAsyncConfig(..., false)；
7. 构造 AicpuPoolCtxArgs；
8. 通过 SomaAicpuKernelLaunch 提交 MALLOC；
9. 如果 AICPU 下发失败，回滚软件侧 segment。

这体现了“先在 Host 侧分配 VA，再在 stream 上提交设备操作”的异步设计。

### 13. rtMemPoolFreeAsync

ApiImplSoma::MemPoolFreeAsync [api_impl_soma.cc](/home/mtuser/workspace/cann/runtime/src/runtime/api/impl/api_impl_soma.cc:112)：

1. 检查 ptr、stream 和 Context；
2. 通过 InMemPoolRegion 判断地址是否在 SOMA 池范围；
3. 池内地址：找到池、查询 allocSize、调用 halMemPoolAsyncConfig(..., true)；
4. 调用 FreeToMemPool，默认 forceFree=false，因此进入 CACHED；
5. 提交 AICPU FREE；
6. 非池内地址：分配一个 callback 参数，在 stream 上注册 Host Callback；
7. callback 执行时调用 ApiImpl::DevFreeStatic。

这样，rtMemPoolFreeAsync 可以兼容池内指针和普通 rtMalloc 指针，但也引入了额外的地址范围查询和 callback 管理。

## 五、复杂度分析

下面使用这些符号：

- n：一个池中全部 Segment 节点数；
- f：freeSegs_ 节点数；
- c：cachedSegs_ 节点数；
- a：allocedMap_ 节点数；
- p：PoolRegistry 中池数量；
- q：sequenceMap_ 条目数；
- s：streamSeqId_ 条目数；
- k：一次释放/Trim 实际合并或处理的 segment 数。

| 操作 | 当前/理论复杂度 | 原因 |
|---|---:|---|
| SplitLeft / MergeLeft | O(1) | 只改链表指针和大小 |
| freeSegs_ lower_bound | O(log f) | 使用 set 成员函数 |
| 从 FREE 分配 | 平均 O(log f) | set 查找、删除、插入；HashMap 插入平均 O(1) |
| SegmentAlloc 当前实现 | 平均 O(log f) | TryToReuse 立即返回 nullptr |
| SingleStreamReuse | O(c) | 通用 lower_bound + 线性过滤 |
| StreamInternalReuse | O(c) | 通用 lower_bound + 候选扫描 |
| StreamEventReuse | O(q + c) | 复制 sequenceMap + 扫描 cached |
| 强制释放 | O(k log n) | 邻居合并和 set 操作 |
| 普通异步释放 | O(s + k log n) | GetStreamSeqId 返回整个 map 副本 |
| GetAllocSize | 平均 O(1) | allocedMap_ 地址查找 |
| Get/SetAttribute | O(1) | 常量字段访问，但需要加锁 |
| TrimCachedSegs | O(t log n) | t 为本次处理的 cached 块数 |
| TrimTo 最坏 | O(c log n) | 可能遍历全部 cached 块 |
| PoolRegistry 注册/删除 | O(log p) | entries_ 是有序 set |
| PoolRegistry 地址范围查询 | O(log p) | entries_ 上的 upper_bound/lower_bound |
| EnumerateMemPools | O(p) | 遍历所有 ownership |
| GetSequenceMap | O(q) | 返回整个 unordered_map 的副本 |
| GetStreamSeqId | O(s) | 返回整个 unordered_map 的副本 |
| RemoveSeqMap | O(q) | 扫描全部 sequenceMap_ |
| SegmentManager 析构 | O(n) | 逐个释放链表节点 |

注意：Driver 的 halMemPoolCreate、halMemPoolTrim、halMemPoolMalloc、halMemPoolFree 是外部调用，源码无法给出其内部复杂度。上表只描述 Runtime Host 侧元数据操作。

## 六、并发和性能综合考量

### 1. 优点

1. 大池一次创建，减少频繁向 Driver 申请/释放的次数；
2. 常规分配只操作 Host 侧元数据，避免每次建立新的 VA 映射；
3. best-fit 可以减少明显的内部碎片；
4. 双向链表可以在 O(1) 时间找到相邻节点；
5. stream/event/sequence 元数据让异步复用具备正确性基础；
6. AICPU 任务把真正的设备操作放进指定 stream，保持 stream-order 语义；
7. busySize、reserveSize 及 high-water mark 支持内存使用统计。

### 2. 主要代价

1. 每个 Segment 都是一个 Host 对象，频繁切分会产生 new/delete；
2. 一个 SegmentManager 只有一把 mutex_，同一池上的并发分配、释放、Trim、属性查询会串行；
3. PoolRegistry 也有全局 mutex_，多池、多 stream 事件更新会竞争；
4. cached 复用函数使用通用 lower_bound，并且需要线性扫描；
5. sequenceMap/streamSeqId 以返回副本的方式读取，可能在锁内复制大量数据；
6. 分配/释放不仅有 Host 元数据成本，还有 AICPU kernel 下发和设备侧执行成本；
7. 2MB 最小分配粒度会放大大量小对象的内部碎片。

### 3. 锁粒度

SegmentAlloc、SegmentFree、TrimTo、GetAttribute、SetAttribute 都持有 SegmentManager::mutex_。因此同一个池中的多个线程不能同时修改 segment 状态。

SegmentFree 的普通路径还会在持有 manager mutex 的情况下访问 PoolRegistry::GetStreamSeqId，形成 manager mutex → registry mutex 的嵌套。当前事件回调主要只拿 Registry mutex，但这种锁层级仍需要在后续扩展时保持一致，避免反向加锁导致死锁。

## 七、潜在问题

### 1. TryToReuse 是最严重的问题

当前 TryToReuse 是空实现，而 SegmentFree(false) 会把块放入 cachedSegs_。如果后续分配只看 freeSegs_，缓存块就不能复用，可能出现：

~~~text
分配池 → 产生 BUSY
异步释放 → 变成 CACHED
继续分配 → TryToReuse 返回 nullptr
freeSegs_ 为空 → RT_ERROR_MEM_POOL_ALLOC
~~~

因此应该先确认这是临时分支、未完成实现，还是依赖其他未编译的实现文件。当前仓库中只找到这一份 TryToReuse 定义。

### 2. “按大小排序”不等于“快速完成复用判定”

cachedSegs_ 的排序只能快速定位大小区间，无法快速判断 stream/event 条件。当前实现仍需扫描候选块。可以增加按 streamId、reuse 类型或依赖状态的辅助索引。

### 3. 返回完整 map 副本会放大锁成本

GetSequenceMap 和 GetStreamSeqId 都返回值，调用方会复制整个 HashMap。尤其 SegmentFree 每次释放都可能复制 streamSeqId_，这会使 O(1) 的释放操作退化为 O(s)。

### 4. 单池锁限制扩展性

如果一个池被多个工作线程频繁使用，所有线程都会在同一把 mutex_ 上排队。可以考虑按 size class 分片、每 stream 分片，或者采用读写锁保护只读属性。

### 5. Segment 状态和设备状态是两套状态机

Runtime 把 segment 标记为 CACHED/BUSY，并不代表 AICPU/Driver 已经完成设备侧映射或解除映射。软件元数据复用必须严格晚于 stream/event 依赖，否则会产生设备侧 use-after-free。

### 6. Trim 的实际收益依赖 Driver

Runtime TrimTo 会更新本地 free/cached 集合，但真正把物理资源归还给设备仍依赖 halMemPoolTrim。若 Driver 不支持，Runtime 只能维护逻辑状态，不能保证系统层面的显存回收。

### 7. 隐式 Trim 当前是空实现

SomaApi::MemPoolTrimImplicit 当前直接返回成功，见 [soma.cc](/home/mtuser/workspace/cann/runtime/src/runtime/feature/soma/soma.cc:328)。普通内存分配失败时虽然会调用它，但当前版本不会真正回收缓存。

## 八、优化建议

按收益优先级建议：

1. 先实现 TryToReuse，并明确 single-stream、event、internal、opportunistic 四种策略的优先级；
2. 将 cached 集合的查找改为 cachedSegs_.lower_bound，避免通用算法在线性迭代器上移动；
3. 维护按 streamId/依赖类别的辅助索引，将复用候选过滤从 O(c) 降到接近 O(log c) 或 O(候选数)；
4. 提供 GetStreamSeqId(streamId) 单值查询，避免复制完整 map；
5. 将 Registry 按 device 分片，或者将 event/sequence 状态下沉到对应池；
6. 为 allocedMap_、freeSegs_、cachedSegs_ 做容量和分配器优化，减少 rehash/new；
7. 对小对象增加更细粒度的 size class，降低 2MB 粒度带来的内部碎片；
8. 在日志和统计中同时记录软件状态、AICPU 提交状态和 Driver 完成状态，便于定位异步复用问题；
9. 为“释放后立即再次分配”“跨 stream + event 复用”“Trim 后再分配”“池内指针误传给普通 Free”等场景增加回归测试。

## 九、总结

当前显存池采用的是：

~~~text
双向地址链表
  + 按大小排序的 free/cached set
  + 按地址哈希的 allocedMap
  + 按池基地址排序的 PoolRegistry
  + stream/event sequence 依赖
  + AICPU SomaMemMng 异步设备操作
~~~

从设计上看，它试图同时解决三个问题：

1. 通过大池和软件切分降低 Driver 调用开销；
2. 通过 stream/event 依赖保证异步复用安全；
3. 通过 free/cached 两套集合兼顾立即可用和延迟可用内存。

正常实现下，FREE best-fit 分配约为 O(log f)，地址范围查询约为 O(log p)，相邻合并约为 O(k log n)。但当前源码的实际瓶颈和风险主要来自：

- TryToReuse 尚未接通；
- cached 复用候选需要线性扫描；
- 释放路径复制完整 stream map；
- 一个池和整个 Registry 都存在串行锁；
- 软件 segment 状态与设备侧映射状态异步变化。

因此，阅读或修改这套代码时，最重要的原则是：先区分“Runtime 元数据是否可复用”和“设备侧 VA/物理页是否已经安全完成”，再讨论算法优化；只看一个 set 或一个 API 函数，容易误判整个显存池的行为。
