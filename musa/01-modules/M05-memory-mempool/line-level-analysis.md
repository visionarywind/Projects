# Memory 与 MemoryPool：行级分析

- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`
- 分析原则：只把实际读取到的函数和行号写成“已确认”；M3D 子模块和硬件行为继续标记为未知/未验证。

## 1. 入口和参数检查

| 证据 | 观察 | 结论状态 |
|---|---|---|
| [`src/driver/mu_memory.cpp:271-304`] | `muapiMemAlloc_v2` 初始化平台、检查指针/size、取 TLS context、构造 general/suballocatable create info | 已确认 |
| [`src/driver/mu_memory.cpp:310-390`] | async alloc 解析 context/stream，支持显式 pool，imported IPC pool 被拒绝 | 已确认 |
| [`src/driver/mu_memory.cpp:393-449`] | async free 按 memory 类型分流；virtual memory 进入 stream command | 已确认 |
| [`src/driver/mu_memory.cpp:716-755`] | synchronous free 通过 pointer tracker 查找、要求 allocation base、同步后销毁 | 已确认 |
| [`src/driver/mu_mempool.cpp:1-563`] | pool create/destroy、attribute、trim、default/current pool API | 已确认 |

## 2. Core object 和 tracker 边界

| 证据 | 观察 | 结论状态 |
|---|---|---|
| [`src/musa/core/context.cpp:1037-1086`] | `Context::CreateMemory` 先构造 shared `Memory`，初始化成功后才登记 Context 与 Platform tracker | 已确认 |
| [`src/musa/core/context.cpp:1089-1097`] | destroy 从 Context 集合和 Platform tracker 中移除 | 已确认 |
| [`src/musa/core/memory.cpp:345-379`] | `Memory` destructor 根据 suballocatable、`m_pPool` 分流到 MemMgr/pool 或直接 destroy | 已确认 |
| [`src/musa/core/memory.cpp:382-429`] | `Memory::Init` 按 memory type 选择初始化路径 | 已确认 |
| [`src/musa/core/memory.cpp:431-460`] | pool allocation 先按 device minimum granularity 对齐，保留用户原始 shape | 已确认 |
| [`src/musa/core/memoryPool.cpp:380-427`] | Core pool 建立 `Memory`、维护 allocation set 和 requested-byte 统计 | 已确认 |

## 3. 自动 pool lookup 与 key

| 证据 | 观察 | 结论状态 |
|---|---|---|
| [`src/hal/m3d/memMgr.h:41-115`] | `MakeKey` 将 property/view/type/heap/NUMA 压入整数 key | 已确认 |
| [`src/hal/m3d/memMgr.cpp:81-147`] | `Allocate` 检查 size、alignment 溢出、设备总显存，然后按 key 查找/创建 pool | 已确认 |
| [`src/hal/m3d/memMgr.cpp:163-197`] | user-managed、internal、自动 pool 分别注册到 user list、internal list、splay tree | 已确认 |
| [`src/hal/m3d/memMgr.cpp:229-235`] | `UpdateUserPools` 只遍历 user pool 并执行 threshold trim | 已确认 |

注意：`m_PoolRefs` 使用 `Util::SplayTree` 的删除节点最终 value ownership 尚未在当前专题中追到，因此不对其析构细节下结论。

## 4. HAL pool 分配算法

| 证据 | 观察 | 结论状态 |
|---|---|---|
| [`src/hal/m3d/memoryPool.cpp:14-40`] | `FindBucket` 对 free segment 做 AlignUp 和容量检查，并受 tryLimit 限制 | 已确认 |
| [`src/hal/m3d/memoryPool.cpp:42-61`] | 默认 policy 为 fast insertion、assuredFit、允许 split | 已确认 |
| [`src/hal/m3d/memoryPool.cpp:82-95`] | `FullAllocate` 先 SubAllocate，失败后只创建一个 chunk 并重试一次 | 已确认 |
| [`src/hal/m3d/memoryPool.cpp:97-151`] | `SubAllocate` 通过 Log2 bucket、位图和 selection policy 选候选 | 已确认 |
| [`src/hal/m3d/memoryPool.cpp:153-211`] | `ChunkAllocate` 进行 alignment/quantum 对齐，创建 virtual 或 physical chunk | 已确认到 HAL/M3D 适配边界 |
| [`src/hal/m3d/memoryPool.cpp:358-413`] | `ResourceSplit` 完成 prefix/suffix split，并为 virtual busy range 建 tracker | 已确认 |

`ChunkAllocate` 后的 `CreateGpuMemory` 是否已经在目标 GPU 上取得物理页，属于 M3D 子模块/内核驱动边界，未运行验证。

## 5. free、merge、reuse 和 trim

| 证据 | 观察 | 结论状态 |
|---|---|---|
| [`src/hal/m3d/memoryPool.cpp:214-259`] | `Free` 精确删除 busy range，左右合并，再决定 free-list 或完整 chunk remove | 已确认 |
| [`src/hal/m3d/memoryPool.cpp:318-331`] | `ResourceRemove` 只对 leftmost/rightmost 完整 chunk 递增 lazyFreeCount，并按 count/size limit destroy | 已确认 |
| [`src/hal/m3d/memoryPool.cpp:415-478`] | free list 按 Log2 size 管理，bitmap 表示非空 bucket，remove 修复链表与 busy 状态 | 已确认 |
| [`src/hal/m3d/memoryPool.cpp:480-510`] | `TrimPool` 只挑选完整空闲 chunk，直到 total 不超过 threshold 或没有可释放 chunk | 已确认 |
| [`src/hal/m3d/memoryPool.cpp:63-80`] | pool 析构遇到 busy segment 会告警，随后按 rightmost chunk 清理 | 已确认；异常/并发场景未运行验证 |

`lazyFreeCount` 的实现位置是完整 chunk 重新 free 的计数，不应命名为异步 command completion count。

## 6. Stream-ordered 和 graph 路径

| 证据 | 观察 | 结论状态 |
|---|---|---|
| [`src/musa/core/stream.cpp:538-600`] | capture alloc 建 graph node；普通 async alloc 先 reserve virtual，再创建 physical、bind、paging | 已确认 |
| [`src/musa/core/stream.cpp:634-671`] | async free 先 DisableAccess，再以 callback 销毁 physical；非 graph allocation 才销毁 virtual | 已确认 |
| [`src/musa/core/command/command.cpp:238-247`] | Wait 未完成时会调用 `UpdateUserPools` | 已确认 |
| [`src/musa/core/stream.cpp:1113-1121`] | WaitFinish 后再次调用 `UpdateUserPools` | 已确认 |
| [`src/musa/core/node/graphMemoryAllocNode.cpp:15-48`] | graph capture 阶段从 graph pool reserve virtual address | 已确认 |
| [`src/musa/core/node/graphMemoryFreeNode.cpp:6-13`] | graph free node Init 当前为 no-op | 已确认 |
| [`src/musa/core/graph/graph1/universalManager.cpp:220-367`] | graph 执行阶段完成 physical allocation、peer/paging、unmap/cleanup | 已确认到 Core/M3D 适配边界 |
| [`src/musa/core/graph.cpp:22-29`] | graph resource 析构清理 physical 并归还 virtual pool memory | 已确认 |

## 7. 统计、锁和生命周期审计关注点

- HAL pool 的 `m_Lock` 是 `recursive_mutex`，保护 segment、bucket、tracker、total/free size 的组合更新；[`src/hal/m3d/memoryPool.cpp:82-95,214-259`]
- Core pool 的 location access map、allocation set、requested bytes 是独立字段/锁域，不能直接假设与 HAL pool 使用同一把锁；[`src/musa/core/memoryPool.h:85-152`]
- `m_RequestedBytes` 表示 Core logical request，`m_TotalSize` 表示 HAL reserved chunks；二者跨大对齐、chunk 保留和 graph deferred physical allocation 时可能显著不同；[`src/musa/core/memoryPool.cpp:380-427`、`src/hal/m3d/memoryPool.h:28-35`]
- `pPool->SetStream(this)` 为 pool 级写入；多个 stream 共享 pool 时的顺序和数据竞争需要目标环境验证；[`src/musa/core/stream.cpp:561-570`]

## 8. 尚未逐行确认的范围

1. `MemoryPool::InitFromHandle` 的 imported/external pool 完整创建语义。
2. Core reuse attribute 字段在其他源码文件中的实际读取者。
3. `Util::SplayTree` 的空 key、删除和并发语义。
4. M3D 子模块的 page allocation、queue fence、paging 和错误码映射。
5. 真实硬件下 alignment、碎片、延迟和 trim 效果。
