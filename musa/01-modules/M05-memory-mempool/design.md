# M05：显存池设计专题

## 1. 设计结论

当前显存池设计把两个问题拆开：

1. **Core pool** 管理 MUSA API 语义、当前 pool、graph pool、访问权限、虚拟地址对象和用户可见统计。
2. **HAL/M3D pool** 管理真正的 chunk、suballocation segment、free-list、合并和 chunk 释放。

```text
Driver API
  ├─ synchronous general allocation
  │    -> HAL MemMgr automatic pool registry
  └─ stream-ordered/graph allocation
       -> Core MemoryPool handle
            -> HAL user-managed MemoryPool

Core MemoryPool  ── one-to-one ── HAL IMemoryPool
                                      └─ M3D chunks/segments
```

这是一个“按资源属性隔离、按 chunk 批量向下申请、在 chunk 内部切分”的 allocator，而不是 buddy allocator 或单一线性 arena。源码没有显示 buddy tree、页级 bitmap 或按请求建独立 chunk 的固定策略；已读实现明确使用 size-class bucket 和区间链表。[`src/hal/m3d/memoryPool.h:48-150`]

## 2. 为什么有多种 pool

### 2.1 同步普通分配：属性驱动

`Memory::GeneralAlloc` 将 flags 转成 HAL allocation info：large-page heap、physical/virtual/shared VA、host/device visibility、suballocatable 和 view capabilities。带 `SubAllocatable` 时直接调用 `IMemMgr::Allocate`，由 MemMgr 根据完整 key 找到或创建自动 pool。[`src/musa/core/memory.cpp:470-515`]

这条路径的目标是让相同资源属性的普通 allocation 共享 chunk，同时阻止不能兼容的 property/view capability 混用。

### 2.2 Async pool：地址和物理资源分离

`muMemAllocAsync` 不是把同步 `GeneralAlloc` 包一层。它先从 `Device::GetMemoryPool()` 的 Core pool 中取得 virtual segment，再单独 `Memory::Init` 一个 physical memory，最后 `Bind` 并把 mapping 通过 stream 的 paging command 排队。[`src/musa/core/stream.cpp:554-600`]

这种拆法使返回地址可以遵守 stream ordering：地址预留发生在 API 调用时，physical mapping 的可见性由 stream 上的命令依赖决定。

### 2.3 Graph pool：图资源的长期虚拟地址

capture 时 `GraphMemoryAllocNode::Init` 立即从 graph pool 保留 virtual allocation，但不创建 physical allocation；graph 执行时 `UniversalManager::ExecuteMemAlloc` 才创建 physical memory、打开 peer memory 并 paging。graph resource 析构时再销毁 physical，并归还 virtual pool segment。[`src/musa/core/node/graphMemoryAllocNode.cpp:15-54`、`src/musa/core/graph/graph1/universalManager.cpp:220-301`、`src/musa/core/graph.cpp:22-29`]

因此 graph pool 的“占用”跨越 graph 对象生命周期；graph free node 的执行主要解除映射，不等价于立刻释放 graph virtual segment。[`src/musa/core/graph/graph1/universalManager.cpp:304-367`]

## 3. 设计取舍

| 取舍 | 直接收益 | 代价/边界 | 状态 |
|---|---|---|---|
| 按 property/type/heap/view/NUMA 分池 | 避免不兼容 memory 混用 | pool 数量和 chunk 碎片可能增加 | 已确认/推断 |
| chunk 批量申请 | 减少底层 CreateGpuMemory 次数 | 保留空闲 chunk 会占用 reserved memory | 已确认 |
| Log2 size bucket | 查找常见大小很快 | bucket 内仍需遍历并检查实际对齐/容量 | 已确认 |
| prefix/suffix split + 邻接 merge | 降低内部碎片 | segment 元数据和锁操作较多 | 已确认 |
| 完整 chunk 的 lazy reuse | 适合短时间 burst | 长生命周期进程可能保留大量显存 | 已确认 |
| virtual/physical 分离用于 async/graph | 能表达 stream ordering 和 graph replay | mapping、peer、physical 生命周期更复杂 | 已确认 |
| Core 与 HAL 两套统计 | 能分别观察 logical/ reserved/peak | 用户容易误读 current used 与 reserved | 已确认 |

## 4. 分配与释放不变量

源码支持以下不变量：

1. `FullAllocate` 只允许 allocation key 与 pool key 相等。[`src/hal/m3d/memoryPool.cpp:82-95`]
2. 被选中的 free segment 在 `ResourceSplit` 后标记为 busy；virtual pool 的 busy range 必须能插入 `m_SegmentTracker`。[`src/hal/m3d/memoryPool.cpp:358-413`]
3. free 时先从 busy tracker 删除，再尝试与左右空闲邻居合并，最后重新放回 free bucket 或销毁完整 chunk。[`src/hal/m3d/memoryPool.cpp:214-259`]
4. Core `Memory` 必须在 tracker 中存活到异步 mapping/free callback 完成；async free 的 callback 捕获 `virt` 和 `pPool`。[`src/musa/core/stream.cpp:634-655`]
5. internal allocation 使用 `prealloc` Core wrapper，释放由 `Device::FreeInternalMem` 显式执行，而不是依赖普通 `Memory` 析构。[`src/musa/core/device.cpp:1110-1152`、`src/musa/core/memory.cpp:360-379`]

## 5. 不应作出的推断

- 不能从 `m_ChunkAllocSize = 32/2 MiB` 推出硬件最小页大小；真正 chunk alignment 来自 `GetHeapInfos()[heap].largestPageSize`。[`src/hal/m3d/memoryPool.cpp:261-293`]
- 不能把 `m_ReuseCountLimit` 叫作“异步 command 完成计数”；当前实现只在完整 chunk 重新成为 free 时递增 `lazyFreeCount`，未见 command/event 计数逻辑。[`src/hal/m3d/memoryPool.cpp:318-331`]
- 不能把 Core 的 reuse attribute 当作已实现的 HAL reuse policy；当前已读 Core 代码只保存/返回三个布尔值，分配路径没有引用它们。[`src/musa/core/memoryPool.cpp:101-198`]
- 不能把 M3D `CreateGpuMemory` 的成功视为 GPU 硬件已分配成功；该调用之后的实现位于 M3D 子模块，当前只确认适配边界。[`src/hal/m3d/memory.cpp:398-459`]
