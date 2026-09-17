# M05：MemoryPool 逐函数实现

## 1. `MemMgr::Allocate`：先分池，再分段

源码 [`src/hal/m3d/memMgr.cpp:81-147`] 的步骤：

1. `size == 0` 返回 `errorInvalidValue`。
2. `size + alignment - 1` 溢出返回 `errorOutOfMemory`。
3. device-local 请求大于设备 `totalGlobalMem` 返回 `errorOutOfMemory`。
4. 若调用方传入 pool，验证 `MakeKey(poolInfo) == MakeKey(allocInfo)`。
5. 否则构造 `MemoryPoolInfo`，从 `m_PoolRefs` 按 key 获取自动 pool。
6. 找不到兼容 pool 时创建 `usageFlags.userManaged=false` 的 pool，默认：
   - `minEnlargeChunkSize = 2 MiB`；
   - `reuseCountLimit = UINT64_MAX`；
   - `reuseSizeLimit = 2 MiB`；
   - 初始 `size = allocInfo.size`。
7. 将 pool 返回给调用方（如果提供 `ppMemoryPool`），再调用 `pPool->FullAllocate`。

注意：这里的 2 MiB 是 HAL `MemoryPool::s_DefaultChunkAllocSize`，不是 Core default/graph pool 使用的 32 MiB。[`src/hal/m3d/memoryPool.h:99-102`、`src/musa/core/memoryPool.h:118`]

## 2. `FullAllocate`：失败后只扩一个 chunk

```text
FullAllocate
  ├─ 断言 alloc key == pool key
  ├─ recursive_mutex lock
  ├─ SubAllocate
  ├─ 若 errorNotFound：ChunkAllocate
  └─ ChunkAllocate 成功后再次 SubAllocate
```

源码 [`src/hal/m3d/memoryPool.cpp:82-95`] 没有循环扩容，也没有在当前函数内做多次 chunk 尝试；第二次 `SubAllocate` 失败就直接返回。底层 `ChunkAllocate` 是否因 M3D 自身策略继续拆分只属于下游实现，当前代码不能外推。

## 3. `SubAllocate`：Log2 bucket + 对齐检查

### 3.1 计算候选 bucket

```text
indexLow  = Log2(request.size)
indexHigh = Log2(request.size + alignment - 1)  // alignment > 1
```

对齐可能使一个请求需要落入更高的 size class。[`src/hal/m3d/memoryPool.cpp:97-103`]

### 3.2 assured-fit（默认）

默认 `SelectPolicy::assuredFit`：

1. 在 `m_EltMappingHash` 中寻找 `indexHigh+1` 以上的第一个非空 bucket。
2. 如果找到普通 bucket，只尝试该 bucket free list 的第一个 segment（`tryLimit=1`）。
3. 如果找到的是 `s_FreeTableLimit` 哨兵，则从 `indexHigh` 向下到 `indexLow` 遍历 bucket，并允许无限尝试。
4. 找不到 segment 返回 `errorNotFound`。

### 3.3 best-fit（代码已支持，默认未启用）

若 policy 改为 `bestFit`，在 `indexLow..endIndex` 的非空 bucket 中逐 bucket 调用 `FindBucket(..., UINT64_MAX)`，找到第一个满足实际地址对齐和容量的 segment。注释称其目标是减少切分和碎片；本版本构造函数仍把 selection 设为 `assuredFit`。[`src/hal/m3d/memoryPool.cpp:54,105-130`]

### 3.4 `FindBucket`

对 free list 顺序遍历：

```text
alignedBase = AlignUp(segment.base, alignment)
segment.base + segment.size >= alignedBase + request.size
```

满足则选中；否则按 `tryLimit` 限制尝试次数。该比较同时处理 prefix alignment padding 和实际 request size。[`src/hal/m3d/memoryPool.cpp:14-40`]

## 4. `ChunkAllocate`：对齐、量化、底层创建

源码 [`src/hal/m3d/memoryPool.cpp:153-211`]：

```text
chunkSize = request.size
if request.alignment > pool.chunkAlignment:
    chunkSize += request.alignment - pool.chunkAlignment
chunkSize = AlignUp(chunkSize, pool.chunkAlignment)
chunkSize = AlignUp(chunkSize, pool.chunkAllocSize)
```

随后：

- pool 非 physical property：构造 `memoryTypeVirtual` chunk，通过 `m_pDevice->GetPlatform().CreateMemory` 创建虚拟地址区间；
- pool 含 physical property：构造 `memoryTypeAlloc`，把 alloc info 的 size/alignment 改为 chunk 值，通过 `m_pDevice->CreateMemory` 创建物理 chunk；
- `base` 对 virtual chunk 取 `GetDeviceVirtualAddress()`，否则使用 `chunkAlignment`；
- 用 chunk 返回 size 建立一个初始 `ResSegment(base,size)`，挂入 segment/free list，增加 `m_TotalSize` 和 `m_FreeSize`。

这里的 `chunkAllocSize` 是向上量化单位，不是简单的“至少申请 request 大小”。例如 request 小于量化单位时会获得一个量化后的 chunk；request 大于量化单位时会向上取整。

## 5. `ResourceSplit`：prefix/suffix 双切分

源码 [`src/hal/m3d/memoryPool.cpp:358-413`]：

1. `alignedBase = AlignUp(pMemRes->base, alignment)`。
2. 从 free bucket 移除原 segment。
3. 若存在前置对齐空间，创建一个 prefix segment，保留为 free；原 segment 改为对齐起点之后的部分。
4. 若当前 segment 大于 request，创建 suffix segment，插入 free list；当前 segment 改为 request size。
5. virtual pool 将 busy range `{alignedBase,size}` 插入 `m_SegmentTracker`；physical-only pool 不插 tracker。
6. 返回 `alignedBase`。

prefix/suffix 都复制 `tag`、`chunkBase`、`pChunkMem` 和 `lazyFreeCount`；空间链表负责相邻关系，free bucket 负责候选查找。

## 6. `Free`：精确删除、左右合并、完整 chunk 判断

源码 [`src/hal/m3d/memoryPool.cpp:214-259`]：

1. 持锁，以 `{base,size}` 在 `m_SegmentTracker` 查找；找不到只记录 warning。
2. 从 busy tracker 删除，并增加 `m_FreeSize`。
3. 若左邻居存在且不 busy：从 free list 和 segment list 移除左邻居，把其 base/size 合到当前 segment。
4. 若右邻居存在且不 busy：同样合并。
5. `ResourceRemove`：
   - 合并结果覆盖整个 chunk（`isLeftMost && isRightMost`）时递增 `lazyFreeCount`；
   - 若 `lazyFreeCount > m_ReuseCountLimit` 或 chunk size 大于 `m_ReuseSizeLimit`，销毁 chunk 和 segment；
   - 否则保留为 free segment；
   - 非完整 chunk 永远回到 free list。

这里的 `lazyFreeCount` 是“该完整 chunk 归还次数”的计数，从实现位置看不是 event/semaphore 的完成次数。[`src/hal/m3d/memoryPool.cpp:318-331`]

## 7. Free list 与位图

`FreeListInsert` 使用 `Log2(segment.size)` 选 bucket：

- `optimal` insertion 按 size 升序插入；
- 默认 `fast` 直接头插；
- 设置 `busy=false` 并置 `m_EltMappingHash` 对应位。[`src/hal/m3d/memoryPool.cpp:415-458`]

`FreeListRemove` 修复前后链，空 bucket 时清除位图，并将 segment 标记 `busy=true`。[`src/hal/m3d/memoryPool.cpp:460-478`]

## 8. Trim 与析构

`TrimPool(value)` 持锁，直到 `m_TotalSize <= value`：从 bucket 扫描，只挑选同时为 leftmost/rightmost 的空闲 segment，移除 list，减少 total/free，调用 chunk `Destroy`。[`src/hal/m3d/memoryPool.cpp:480-510`]

这意味着：

- 有 busy segment 的 chunk 不会被 trim；
- 一个 chunk 即使大部分空闲，只要还有一个 busy segment，也不会被 trim；
- trim 释放的是整个 chunk，不会把一个 chunk 拆成更小的底层 allocation。

HAL pool 析构会遍历所有 segment；若仍 busy，打印 `cleanup unfreed memory sub-allocation`，随后销毁每个 rightmost chunk 并删除 segment。[`src/hal/m3d/memoryPool.cpp:63-80`]

## 9. Core `Memory` 的 pool 释放边界

`Memory::~Memory` 对 suballocatable HAL memory 分两路：

- `m_pPool == nullptr`：调用设备 MemMgr `Free`，由 pool key 重新找自动 pool；
- `m_pPool != nullptr`：以 `GetAllocationGranularity(...MINIMUM)` 对逻辑 size 向上取整，然后调用该 Core pool 的 HAL `Free`。[`src/musa/core/memory.cpp:360-379`]

`Memory::InitFromPool` 也在进入 HAL `FullAllocate` 前按同一 device allocation granularity 对 size 向上取整，但保存的 `m_Shape` 仍是用户原始 size。[`src/musa/core/memory.cpp:431-460`]

因此 free 的 `size` 不能随意替换为用户原始字节；必须与 pool allocation 时的 segment size 对齐，否则 interval lookup 可能失败。当前 Core 代码通过再次 `AlignUp` 保持这一契约。

## 10. Core pool 的属性和统计实现

- `SetAttribute(RELEASE_THRESHOLD)` 直接写 HAL pool threshold。
- `RESERVED_MEM_*` 读取 HAL total/high watermark。
- `USED_MEM_*` 读取 Core `m_RequestedBytes`/high watermark。
- `SetAccess` 修改 location map 后，对已有 allocation 收集 `(virt,physical,size)` 并调用 `ModifyAccess`。
- `ModifyAccess` 对跨设备 READWRITE 先 `OpenPeerMemory`，再组装 `MemoryPaging`，通过 `m_pStream->CmdPaging` 排队。[`src/musa/core/memoryPool.cpp:101-198,201-376`]

三个 reuse attribute 当前在 Core 中只转换为 disable flags 并由 GetAttribute 返回；全局静态查找只发现构造、Set/Get 三类引用，没有发现它们改变 HAL `SelectPolicy`、dependency 检查或 `Free` 逻辑。因此在该源码版本中，应将其运行效果标记为未接线/未知，而不是把 API 名称直接当作已经实现的调度行为。

## 11. IPC pool handle 的实现范围

`MemoryPool::Export` 和 `InitFromHandle` 共享的是一段 `IpcMemPoolShmem_t` metadata，而不是 HAL `IMemoryPool` 本身：

```text
export side
  -> call_once(CreateIpcMemPoolShmemIfNeed)
  -> shm_open + ftruncate + mmap
  -> owners = 1
  -> dup(fd) 返回调用方

import side
  -> dup(fd)
  -> mmap metadata
  -> owners += 1
  -> m_IsImported = true
```

[`src/musa/core/memoryPool.cpp:439-569`]

`ExternalAlloc` 初始化没有为 wrapper 设置 `m_pHalPool`。所以 imported pool handle 的作用域受到 driver API 限制：不能用于 `muMemAllocFromPoolAsync`，也不能修改 pool attribute；pointer import 走的是独立 IPC memory handle → `memoryTypeIpcImport` 路径，而不是 imported pool 的 `FullAllocate`。[`src/musa/core/memoryPool.cpp:21-64,473-510`、`src/driver/mu_memory.cpp:349-390`、`src/driver/mu_mempool.cpp:153-169,322-390`]

### 11.1 cleanup 的精确边界

析构路径先递减共享 `owners` 并 `munmap`。但源码把 `close(m_IpcFd)` 与 `shm_unlink` 都放在 `m_IpcName` 非空分支内；export owner 设置名称，而 `InitFromHandle` 创建的 imported wrapper 不设置名称。因此不能声称 imported wrapper 析构一定关闭其 dup fd。若 exporter 在 importers 之前销毁，它因 `owners != 0` 不 unlink；后续 imported wrapper 又不满足非空名称分支，最终 unlink 责任也需要运行验证。[`src/musa/core/memoryPool.cpp:575-609`]

### 11.2 静态实现风险

1. POSIX `mmap` 失败返回 `MAP_FAILED`，当前代码以 `nullptr` 判断 import/export 两处结果。[`src/musa/core/memoryPool.cpp:491-503,551-562`]
2. shared `owners` 由进程内 mutex 包裹，但不同进程不共享该 mutex；跨进程加减的原子性未建立。[`src/musa/core/memoryPool.cpp:486-503,575-603`]
3. `CreateIpcMemPoolShmemIfNeed` 用 `call_once` 包住可能失败的初始化，且中间失败没有统一 RAII rollback；首次失败后的重试和资源残留需要故障注入验证。[`src/musa/core/memoryPool.cpp:516-569`]
4. `muMemPoolDestroy` 在 driver 层未检查 live allocation；是否会造成 pool-backed `Memory::m_pPool` 悬空，需要 API contract 和运行测试确认。[`src/driver/mu_mempool.cpp:118-150`]

## 12. current pool 与 destroy 的 raw-pointer 边界

Device 的 default/current/graph pool 都是 raw pointer；`GetMemoryPool()` 优先返回 `m_CurrentMemoryPool`，而 `SetCurrentMemoryPool` 只是赋值。`ValidatePool` 对非 imported pool 仅查询 HAL user-pool registry。[`src/musa/core/device.h:87-95,224-228`]

```text
muDeviceSetMemPool / muMemSetMemPool
  -> SetCurrentMemoryPool(pMemPool)
       -> m_CurrentMemoryPool = pMemPool

muMemPoolDestroy
  -> ValidatePool
  -> delete Core MemoryPool
       -> DestroyUserPool(HAL pool)
       -> Core object 消失
```

当前 `muMemPoolDestroy` 没有在删除前把 Device/Platform 的 current pointer 回退到 default。若被删除对象仍是 device current pool，后续 `Device::GetMemoryPool()` 会优先返回该非空 raw pointer，而不会触发 default fallback。这是从赋值和 destroy 顺序直接得到的静态生命周期风险，尚未运行复现。[`src/driver/mu_device.cpp:328-348`、`src/driver/mu_mempool.cpp:118-150,504-563`]

host/NUMA current pool 同样是 Platform 保存的 raw pointer；setter 只覆盖当前项，Platform 析构时只明确删除 default host/NUMA pools，再清空 current 容器。用户 pool 的真实 ownership 仍在 HAL user-pool registry/Core handle，而不是这些 current 指针。[`src/musa/core/platform.cpp:524-535,575-645`]

另一个边界是 imported wrapper：`ValidatePool` 对 imported pool 直接返回 true，但 `muMemSetMemPool` 随后无条件执行 `pMemPool->Hal()->GetInfo()`；imported wrapper 的 HAL pointer 为 null。因此将 imported pool 设为 current 的行为存在空指针风险，不能由 `ValidatePool` 的成功推导为该 API 支持 imported pool。[`src/musa/core/device.h:93-95`、`src/driver/mu_mempool.cpp:504-553`]

## 13. live allocation 与 pool teardown

Core pool allocation 同时存在三种引用：

1. `Platform::MemoryTracker` 以 shared ownership 持有 `Memory`；
2. Core pool 的 `m_MemoryAllocations` 只保存 raw pointer；
3. `Memory::m_pPool` 保存指回 Core pool 的 raw pointer。

`MemoryPool::~MemoryPool` 不先遍历或拒绝非空 allocation set，而是直接 `DestroyUserPool(m_pHalPool)`；随后 live `Memory` 最后析构时仍会通过 `m_pPool->GetDevice()` 和 `m_pPool->Hal()->Free(...)`。因此 destroy-with-live-allocation 可能同时破坏 Core pool raw pointer 与 HAL suballocation owner；实际 API 是否由外层 contract 禁止该顺序，当前实现片段没有证明。[`src/musa/core/memoryPool.cpp:86-99,380-427`、`src/musa/core/memory.cpp:360-379`]

进程退出路径有所不同：Platform 先等待 device，再 `ReleaseUnfreedMemories` 清空 Context/MemoryTracker，之后删除 host pools 和 devices；Device 析构则在 contexts/primary context 后删除 default/graph pools。这说明全局 teardown 尝试让 Memory 在 pool 之前消失，但不能替代用户显式 `muMemPoolDestroy` 的 live-allocation 检查。[`src/musa/core/platform.cpp:478-541`、`src/musa/core/device.cpp:672-700`]

## 14. IPC pointer import 的 ownership

`muMemPoolExportPointer` 从当前 Context 的 virtual Memory 找到 physical Memory，导出 physical IPC handle 并设置 `fromMempool=true`。import 侧只从 pool wrapper 取得 device，然后在当前 Context 中创建 `memoryTypeIpcImport`；当前入口没有调用 `ValidatePool`、没有验证 pool 必须是 imported wrapper，也没有调用该 pool 的 `CreateMemory`。[`src/driver/mu_mempool.cpp:322-390`]

所以 imported pointer 的实际 Core ownership 是当前 Context/Platform MemoryTracker，而不是传入 pool 的 `m_MemoryAllocations`。这只是本版本入口实现事实；是否允许任意同设备 pool handle、是否应校验 `fromMempool`，仍需结合公开 API contract 和运行测试确认。
