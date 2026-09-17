# Memory 与 MemoryPool：调用链

- 模块：M05 Memory / MemoryPool
- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`
- 证据状态：普通、异步和 graph memory 路径已静态追到 Core/HAL/M3D 适配边界；M3D 子模块内部、内核驱动和真实硬件行为未验证。

## 1. 普通 device allocation：API 到 chunk + offset

```text
muMemAlloc / muMemAlloc_v2
  -> muapiMemAlloc_v2(dptr, bytesize)                       [src/driver/mu_memory.cpp:271-304]
     ├─ InitPlatform()
     ├─ 校验 dptr / bytesize
     ├─ TlsCtxTop() -> 当前 Context
     ├─ MemoryCreateInfo{type=memoryTypeGeneral,
     │                    flags=Virtual|DeviceMapped|SubAllocatable}
     └─ pContext->CreateMemory(&pMemory, createInfo)
        -> Context::CreateMemory                            [src/musa/core/context.cpp:1037-1086]
           ├─ 检查 capture 状态
           ├─ std::make_shared<Memory>(this)
           ├─ Memory::Init(createInfo)                      [src/musa/core/memory.cpp:382-429]
           │  └─ Memory::GeneralAlloc                       [src/musa/core/memory.cpp:470-515]
           │     ├─ 构造 Hal::MemoryAllocInfo
           │     ├─ 组合 heap/property/view capability
           │     └─ SubAllocatable -> IMemMgr::Allocate
           │        -> Hal::M3d::MemMgr::Allocate           [src/hal/m3d/memMgr.cpp:81-147]
           │           ├─ 检查 size/alignment/totalGlobalMem
           │           ├─ MakeKey 查找/创建自动 MemoryPool
           │           └─ pPool->FullAllocate(...)
           │              -> SubAllocate
           │              -> [无匹配] ChunkAllocate
           │              -> 再次 SubAllocate
           │              -> ResourceSplit
           ├─ peer accessible 时 MapToPeers
           ├─ ctxCrit->AddMemory(pMemory)
           ├─ Platform::MemoryTracker.TrackMemory(memory_sp)
           └─ 设置 sequence ID 并返回
     └─ *dptr = pMemory->GetDevicePointer()
```

返回值不是独立 HAL allocation 的裸地址，而是 `pChunkMem` 的 device virtual address 加上 `m_Offset`。一个 HAL chunk 可对应多个 Core `Memory` 对象。[`src/musa/core/memory.cpp:345-379,431-460`]

## 2. 自动 pool lookup 与 chunk 扩容

```text
MemMgr::Allocate
  -> MemoryPoolInfo{type, heap, property, viewCapability, numaId}
  -> MakeKey(MemoryPoolInfo)
  -> m_PoolRefs.Get(key)
  -> 兼容 pool
       -> FullAllocate
  -> 不存在/属性不兼容
       -> CreatePoolNoLock(userManaged=false)
       -> minEnlargeChunkSize=2 MiB
       -> reuseCountLimit=UINT64_MAX
       -> reuseSizeLimit=2 MiB
       -> FullAllocate
```

`FullAllocate` 只有一次“分配失败后扩一个 chunk 再重试”的机会；它不是循环扩容器：

```text
FullAllocate [src/hal/m3d/memoryPool.cpp:82-95]
  -> lock(m_Lock)
  -> SubAllocate [97-151]
       -> Log2 bucket + bitmap + FindBucket
       -> ResourceSplit [358-413]
  -> errorNotFound
       -> ChunkAllocate [153-211]
            -> virtual chunk 或 physical chunk
            -> ResourceAdd
       -> SubAllocate 再试一次
```

`SubAllocate` 的候选查找按 `Log2(size)` 和 `Log2(size + alignment - 1)` 确定范围；`FindBucket` 仍必须检查地址对齐和实际容量，因此 bucket 命中不等于一定可分配。[`src/hal/m3d/memoryPool.cpp:14-40,97-151`]

## 3. chunk、segment 和返回 offset

```text
ChunkAllocate
  -> chunkSize = request.size
  -> 若 alignment > chunkAlignment，加 alignment - chunkAlignment
  -> AlignUp(chunkAlignment)
  -> AlignUp(chunkAllocSize)
  -> CreateMemory(virtual 或 alloc)
  -> ResSegment(base, returnedSize)
  -> ResourceAdd -> segment list + free bucket

ResourceSplit
  -> 从 free bucket 移除完整 segment
  -> prefix：保存对齐 padding
  -> 当前 segment 对齐到 alignedBase
  -> suffix：保存 request 后剩余空间
  -> virtual pool 插入 m_SegmentTracker[{alignedBase,size}]
  -> 返回 alignedBase
  -> offset = alignedBase - chunkBase
```

`m_FreeSize` 按实际 segment 变化；Core `m_RequestedBytes` 按用户逻辑请求变化，二者不是同一统计。[`src/hal/m3d/memoryPool.cpp:153-211,358-413`、`src/musa/core/memoryPool.cpp:380-427`]

## 4. 普通 free：同步边界、精确 range 和合并

```text
muMemFree / muMemFree_v2(dptr)                              [src/driver/mu_memory.cpp:716-755]
  -> Platform::Get().GetMemoryByDevicePointer(dptr, &offset)
  -> 要求 offset == 0（allocation base）
  -> pMemory->Synchronize()
  -> Context::DestroyMemory(pMemory)                         [src/musa/core/context.cpp:1089-1097]
       ├─ 从 Context CriticalBase 移除
       └─ Platform::MemoryTracker.UntrackMemory
  -> shared_ptr 最后释放
  -> Memory::~Memory()                                       [src/musa/core/memory.cpp:360-379]
       ├─ 自动 pool：MemMgr::Free
       └─ Core pool：m_pPool->Hal()->Free(aligned size)
            -> MemoryPool::Free                                  [src/hal/m3d/memoryPool.cpp:214-259]
               ├─ m_SegmentTracker 精确查找 {base,size}
               ├─ erase busy range
               ├─ 与左侧 free segment 合并
               ├─ 与右侧 free segment 合并
               ├─ 完整 chunk -> ResourceRemove
               └─ 否则 FreeListInsert
```

`Memory::Synchronize` 对 virtual memory 还要处理其 physical tracker 和关联 context，因此 free 的第一步不是直接删除指针。Core pool free 的 size 重新按 minimum allocation granularity 对齐，以满足 HAL interval lookup 契约。[`src/musa/core/memory.cpp:117-145,360-379,431-460`]

## 5. `muMemAllocAsync`：虚拟地址、物理 backing、stream mapping

```text
muapiMemAllocAsync                                      [src/driver/mu_memory.cpp:310-346]
  -> TLS Context
  -> Context::InfoStream(stream)
  -> Stream::CmdMemAlloc
       ├─ capture active -> CaptureMemAlloc
       └─ normal -> AsyncMemAlloc                           [src/musa/core/stream.cpp:554-600]
            -> 选择显式 pool 或 Device::GetMemoryPool()
            -> pPool->SetStream(this)
            -> pPool->CreateMemory(&virt, &virtAddr, size)
                 -> Core pool virtual suballocation
                 -> HAL FullAllocate / segment tracker
            -> physical->Init(general, flags=0)
                 -> non-suballocatable physical HAL memory
            -> virt->Bind(physical)
            -> pPool->ModifyAccess(virt, physical, size, ..., this)
                 -> MemoryPaging
                 -> CmdPaging
                 -> ResolveDependencyAndQueueCommand
            -> 返回 virtAddr
```

地址预留在 API 调用中发生；映射可见性由 stream command ordering 决定。mapping 失败时先解除 bind，再销毁 virtual Core allocation；physical shared pointer 随失败路径离开作用域后释放。`SetStream` 是 pool 级字段，多 stream 共享 pool 的并发语义仍需运行验证。

## 6. `muMemFreeAsync`：先 unmap，再 callback 回收

```text
muapiMemFreeAsync(dptr, stream)                             [src/driver/mu_memory.cpp:393-449]
  -> 根据 dptr 找 Memory
  -> virtual + pool -> Stream::CmdMemFree
       -> AsyncMemFree                                    [src/musa/core/stream.cpp:634-671]
          -> 找 virt / physical / pPool
          -> pPool->DisableAccess(...)
               -> PROT_NONE paging
               -> CmdPaging 入队
          -> CallbackCommand 入队
               callback:
                 virt->DestroyPhysMemories()
                 if (!virt->IsGraphAlloc():
                     pPool->DestroyMemory(virt))
```

因此普通 async free 的 virtual segment 只有在 callback 按 stream 顺序完成后才回到 HAL pool；graph allocation 则不在这个 callback 中归还，由 graph resource 生命周期负责。

## 7. wait 与 user pool trim

```text
Command::Wait                                      [src/musa/core/command/command.cpp:238-247]
  -> command 未完成时
  -> Hal::MemMgr::UpdateUserPools()

Stream::WaitFinish                                  [src/musa/core/stream.cpp:1113-1121]
  -> 等待 last command
  -> 再次 UpdateUserPools()

MemMgr::UpdateUserPools                            [src/hal/m3d/memMgr.cpp:229-235]
  -> 遍历 m_UserPools
  -> releaseThreshold = pPool->GetReleaseThreshold()
  -> pPool->TrimPool(releaseThreshold)
```

`UpdateUserPools` 本身没有扫描 event/semaphore 或 completion list；它直接做 threshold trim。不能把它等同于完整 deferred-free engine。

## 8. capture/graph memory

```text
capture active
  -> Stream::CaptureMemAlloc                         [src/musa/core/stream.cpp:538-551]
  -> Context::CreateMemAllocNode                     [src/musa/core/context.cpp:2368-2382]
  -> GraphMemoryAllocNode::Init                      [src/musa/core/node/graphMemoryAllocNode.cpp:15-48]
       -> Device::GetGraphMemoryPool()
       -> CreateMemory(virtual only)
       -> nodeParams.dptr = reserved VA
       -> graph resource 注册

GraphExec
  -> alloc/free node -> MUSA_SUBMISSION_HOST_DEVICE
  -> UniversalManager::ExecuteMemAlloc               [src/musa/core/graph/graph1/universalManager.cpp:220-301]
       -> 等依赖 -> create physical -> peer mapping -> paging -> signal
  -> UniversalManager::ExecuteMemFree                [304-367]
       -> 等依赖 -> unmap -> peer destroy -> DestroyPhysMemories -> signal

GraphResource::~GraphResource                          [src/musa/core/graph.cpp:22-29]
  -> DestroyPhysMemories
  -> graphPool->DestroyMemory(virtual)
  -> Core Memory destructor
  -> HAL pool Free / segment merge
```

`GraphMemoryFreeNode::Init` 当前是 no-op；普通 stream launch switch 中的 graph mem alloc/free command 分支存在注释掉的调用，因此不能把 graph 执行误写成普通 `CmdMemAlloc/CmdMemFree`。

## 9. internal、default、host/NUMA pool

```text
Device::GetDefaultMemoryPool / GetGraphMemoryPool
  -> Core MemoryPool wrapper
  -> HAL user-managed pool
  -> virtual + suballocatable
  -> 32 MiB chunk quantum

Device::GetInternalPool
  -> std::call_once
  -> HAL MemMgr internal pool
  -> internal allocation FullAllocate
  -> new Core Memory + InitPrealloc
  -> FreeInternalMem: delete Memory 后显式 HalPool::Free

Platform::GetDefaultHostMemoryPool / CreateHostMemoryPoolInternal
  -> host/NUMA allocation type
  -> pool key 还包含 numaId
  -> Platform 持有 default/current pool
```

default、graph、host/NUMA 的创建证据分别位于 [`src/musa/core/device.cpp:439-519`]、[`src/musa/core/platform.cpp:544-645`]；internal 生命周期位于 [`src/musa/core/device.cpp:1091-1152`]。


## 10. IPC memory-pool handle：metadata ownership，不是 HAL pool import

### 10.1 Export

```text
muMemPoolExportToShareableHandle                         [src/driver/mu_mempool.cpp:264-285]
  -> pool handle / flags / device 校验
  -> MemoryPool::Export                                [src/musa/core/memoryPool.cpp:439-469]
       -> CreateIpcMemPoolShmemIfNeed
            -> mkstemp(/tmp/mempoolXXXXXX)
            -> shm_open(MUSA_...)
            -> ftruncate(sizeof(IpcMemPoolShmem_t))
            -> mmap shared metadata
            -> owners = 1
       -> dup(m_IpcFd)
       -> 输出 POSIX fd
```

### 10.2 Import

```text
muMemPoolImportFromShareableHandle                       [src/driver/mu_mempool.cpp:288-319]
  -> 当前 Context / Device
  -> new Musa::MemoryPool(pDevice)
  -> MemoryPool::Init(ExternalAlloc)
  -> InitFromHandle                                   [src/musa/core/memoryPool.cpp:473-510]
       -> 只支持 POSIX fd、flags == 0
       -> dup(fd)
       -> mmap(sizeof(IpcMemPoolShmem_t))
       -> shared owners += 1
       -> m_IsImported = true
  -> 返回 Core pool handle
```

当前源码中 `InitFromHandle` 没有取得或创建 `m_pHalPool`；它建立的是 IPC metadata ownership。相应地，driver 的 `SetAttribute` 会拒绝 imported pool，且异步 pool allocation 路径也明确拒绝 imported pool。[`src/driver/mu_mempool.cpp:153-169`、`src/driver/mu_memory.cpp:349-390`]

### 11.3 Shared-memory cleanup

```text
MemoryPool::~MemoryPool
  -> DestroyIpcMemPoolShmemIfNeed                  [src/musa/core/memoryPool.cpp:575-609]
       -> owners--
       -> munmap
       -> close fd
       -> imported=false && owners==0 -> shm_unlink
```

实现审计注意：POSIX `mmap` 失败返回 `MAP_FAILED` 而不是 `nullptr`，当前 export/import 两处检查使用 `nullptr`；这是静态风险，不能在没有故障注入的情况下声称已复现。[`src/musa/core/memoryPool.cpp:491-503,551-562`]

## 11. pool API 属性和所有权边界

```text
muMemPoolCreate
  -> Hal::MemoryPoolCreateInfo
  -> new Core MemoryPool
  -> Init(GeneralAlloc)
  -> Hal::MemMgr::CreateUserPool
  -> m_pHalPool = user pool

muMemPoolDestroy
  -> reject default pool
  -> Device::ValidatePool
  -> delete Core MemoryPool
       -> DestroyUserPool(m_pHalPool)
       -> IPC metadata cleanup
```

`muMemPoolDestroy` 当前源码没有在 driver 入口显式检查 `m_MemoryAllocations` 是否为空；而 pool-backed `Memory` 的 destructor 仍需要访问 `m_pPool` 才能归还 segment。因此“有 live allocation 时 destroy pool”的行为需要专门验证，不能假设 API 已经提供安全拒绝。[`src/driver/mu_mempool.cpp:118-150`、`src/musa/core/memoryPool.cpp:86-99`、`src/musa/core/memory.cpp:360-379`]


## 12. 自动 pool registry 的 lookup/ownership

```text
MemMgr::Allocate
  -> m_PoolRefs.Get(MakeKey(poolInfo))
  -> Get 会 splay tree，并返回根节点
  -> MemMgr 再比较 type/heap/property/view capability
  -> 不兼容时 CreatePoolNoLock + Insert(key, pPool)

MemMgr::~MemMgr
  -> 删除 m_UserPools 中的 user pool
  -> m_PoolRefs.Delete(..., true)
       -> delete SplayTreeNode
       -> delete node->m_Value (automatic MemoryPool)
  -> 删除 internal pool list 中的 pool
```

`SplayTree::Get` 不存在 key 时也返回 splay 后的根节点，因此 `MemMgr::Allocate` 的后续属性比较是必要的防护；不能把 `Get` 的非空返回直接当成精确命中。[`src/util/utilSplayTree.h:87-93`、`src/hal/m3d/memMgr.cpp:116-133`]

## 13. 当前证据边界

已确认到源码适配边界：

- pool key、registry、bucket、split、merge、reuse、trim；
- Core virtual/physical bind 和 stream callback；
- graph host-device submission；
- IPC pool handle 的 metadata export/import 和 shared-memory cleanup；
- automatic pool registry 的 splay lookup 和 value ownership。

仍未确认：

- `IM3d::IDevice::CreateGpuMemory` 之后的 kernel driver/firmware 资源结果；
- M3D 子模块的实际 page size、fence 和硬件错误码；
- SplayTree `Find` 当前比较方向在真实 registry 树形下的行为；
- IPC pool 的跨进程 owners 竞态和故障回滚运行结果；
- 真实 workload 的碎片、延迟和 reserved-memory 曲线。
