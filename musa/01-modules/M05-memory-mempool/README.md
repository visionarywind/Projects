# M05：Memory 与 MemoryPool

- 源码范围：`src/driver/mu_memory.cpp`、`src/driver/mu_mempool.cpp`、`src/musa/core/memory.cpp`、`src/musa/core/memoryPool.cpp`、`src/musa/core/device.cpp`、`src/musa/core/stream.cpp`、`src/musa/core/node/graphMemoryAllocNode.cpp`、`src/hal/m3d/memMgr.cpp`、`src/hal/m3d/memoryPool.cpp`
- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`
- 最后专题更新：2026-09-16
- 证据状态：**静态源码已确认**；M3D 子模块内部、目标机分配性能和真实硬件结果**未验证**。

## 结论先行

当前实现不是“一个全局显存池”，而是三条并行机制：

| 使用场景 | Core 入口 | 实际池/分配器 | 物理内存时机 |
|---|---|---|---|
| 同步 `muMemAlloc` / 普通 `Memory::GeneralAlloc` | `Memory::GeneralAlloc` | HAL `MemMgr` 按 allocation key 自动选择/创建池；随后 `FullAllocate` | 分配请求进入时直接取得 HAL chunk，再返回 chunk+offset |
| stream-ordered `muMemAllocAsync` | `Stream::AsyncMemAlloc` | `Device::GetMemoryPool()` 返回当前池或默认池 | 先从 Core pool 保留虚拟地址，再直接创建 physical memory，之后排队映射 |
| graph/capture memory alloc | `GraphMemoryAllocNode::Init` | 独立 `Device::GetGraphMemoryPool()` | graph node 创建时只保留虚拟地址；graph 执行时创建/映射 physical memory |

池底层使用 **chunk + segment 双向链表 + size-class free buckets + busy interval map**：

```text
MemoryPool
  ├─ chunk：真实 HAL/M3D memory
  ├─ ResSegment：chunk 内的一段空闲/占用区间
  ├─ m_FreeBuckets[]：按 Log2(segment.size) 分类的空闲链
  ├─ m_EltMappingHash：非空 bucket 的位图
  ├─ m_pHeadSegment：同一 pool 所有 segment 的双向链表
  └─ m_SegmentTracker：虚拟池的 busy 区间精确索引
```

因此“释放”通常不是立即销毁底层显存：释放段先合并相邻空闲段；整个 chunk 变成空闲后，再根据 lazy-free 阈值决定保留或销毁。显式 `TrimTo` 则只释放完整空闲 chunk。

## 阅读顺序

1. [`design.md`](design.md)：三类 pool 和分层取舍。
2. [`data-structures.md`](data-structures.md)：key、registry、chunk、segment、所有权。
3. [`implementation.md`](implementation.md)：`FullAllocate`、split、merge、trim 的逐步实现。
4. [`call-chains.md`](call-chains.md)：从 Driver API 到 HAL/M3D 的源码链。
5. [`execution-flows.md`](execution-flows.md)：同步、异步、graph、free 的状态时序。
6. [`line-level-analysis.md`](line-level-analysis.md)：行号证据与静态风险。

## 最小源码调用图

```text
muapiMemAlloc_v2
  -> Context::CreateMemory
  -> Memory::GeneralAlloc
  -> Hal::IMemMgr::Allocate
  -> Hal::M3d::MemMgr::Allocate
  -> pool lookup/create by MakeKey
  -> MemoryPool::FullAllocate
       -> SubAllocate
       -> [not found] ChunkAllocate
       -> SubAllocate
       -> ResourceSplit
  -> Memory { pHalMemory=chunk, m_Offset=offset }
```

```text
muapiMemAllocAsync
  -> Stream::CmdMemAlloc
  -> Stream::AsyncMemAlloc
       -> Core pool CreateMemory (virtual segment)
       -> Memory::Init(general, flags=0) (physical memory)
       -> Memory::Bind
       -> MemoryPool::ModifyAccess
       -> Stream::CmdPaging / dependency queue
```

## 重要语义

- Core `Memory` 是用户可见对象；HAL `IMemory` 是 chunk 或 physical resource。suballocation 时一个 HAL chunk 可对应多个 Core `Memory` 对象。[`src/musa/core/memory.cpp:345-379,431-460`]
- `Memory::GetDevicePointer()` 是 `HAL virtual address + m_Offset`；因此 tracker 必须同时知道底层 chunk 和 suballocation offset。[`src/musa/core/memory.h:53-55,125-139`]
- 同步普通分配的 pool key 包含 type、heap、property、view capability、NUMA ID；不同属性不会无条件共享 chunk。[`src/hal/m3d/memMgr.h:41-115`]
- `MemoryPool::CreateMemory` 统计的是 Core 请求的逻辑字节；HAL pool 的 reserved/current 字节是 chunk 总量，两者不是同一指标。[`src/musa/core/memoryPool.cpp:380-427`、`src/hal/m3d/memoryPool.h:28-35`]
- `MU_MEMPOOL_ATTR_RELEASE_THRESHOLD` 只设置 HAL pool 的 trim threshold；它不改变 `ResourceRemove` 中的 reuse count/size 条件。[`src/musa/core/memoryPool.cpp:118-123,430-437`、`src/hal/m3d/memoryPool.cpp:318-331`]
- 复用依赖 stream/command 完成边界：普通 async free 通过 paging 后的 callback 才将 virtual object 交还 pool；`Wait`/`WaitFinish` 会调用 `UpdateUserPools`。[`src/musa/core/stream.cpp:634-671,1113-1121`、`src/musa/core/command/command.cpp:238-247`]

## 当前边界

已确认：

- segment 的选择、对齐、切分、相邻合并、完整 chunk 回收；
- HAL MemMgr 的 key 打包及 internal/user/自动 pool registry；
- default、graph、host/NUMA、internal pool 的创建入口；
- async alloc/free 和 graph memory alloc/free 的 Core 级路径。

仍未知或未验证：

- `IM3d::IDevice::CreateGpuMemory`、`IM3d::IQueue` 之后的内核驱动/firmware 行为；
- M3D 实际 page size、物理碎片、分配耗时和回收性能；
- 所有异常分支在真实硬件上的错误码映射；
- `Util::SplayTree` 删除节点时对 pool value 的最终所有权细节（本专题只确认调用点）。

## 相关文档

- [`../M04-context-lifecycle/`](../M04-context-lifecycle/)
- [`../M06-stream-command/`](../M06-stream-command/)
- [`../M08-graph/`](../M08-graph/)
- [`../../90-cross-module/cross-module-call-chains.md`](../../90-cross-module/cross-module-call-chains.md)
