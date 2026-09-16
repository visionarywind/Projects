# 跨模块调用链索引

- 对应源码版本：`b8dce2b2f23849e8e99350c20d7eaac3c129caba`
- 证据状态：初始化、设备查询、普通内存分配、kernel launch 已静态追到 HAL/M3D 适配边界；M3D 内部到内核驱动/硬件仍未运行验证。

| 链 ID | 起点 | 终点 | 当前状态 | 主文档 |
|---|---|---|---|---|
| INIT-001 | `muapiInit` | `IM3d::CreatePlatform` / `EnumerateDevices` | Driver/Core/HAL 适配已确认 | M03 `call-chains.md`、D01 `execution-trace.md` |
| DEV-001 | `muDeviceGetAttribute` | `Hal::DeviceProperties` | API/Core 已确认 | M02/M03 `call-chains.md` |
| CTX-001 | `muDevicePrimaryCtxRetain` | `Device::LaterInit` / primary Context | 已确认到 HAL `Finalize` | D01 `execution-trace.md` |
| `MEM-001` | `muapiMemAlloc_v2` | `MemMgr::Allocate` / `MemoryPool::FullAllocate` / `IM3d::CreateGpuMemory` | 普通分配已追到 pool key、chunk、segment split 和 M3D 适配边界 | M05 `call-chains.md` |
| MEMFREE-001 | `muapiMemFree_v2` | `Context::DestroyMemory` / `Memory::~Memory` / HAL pool `Free` | 同步释放已追到精确 range、左右 merge、lazy reuse/trim 边界 | M05 `call-chains.md` |
| MEMASYNC-001 | `muMemAllocAsync` / `muMemFreeAsync` | virtual pool allocation / physical backing / paging / callback | async alloc/free 的 Core 生命周期已确认；硬件完成语义未验证 | M05 `call-chains.md` |
| MEMGRAPH-001 | graph capture memory alloc/free | `GraphExec` / `UniversalManager` physical mapping and cleanup | graph virtual reserve、host-device submission 和 resource destroy 已确认 | M05 `call-chains.md` |
| COPY-001 | `muapiMemcpyAsync` | `Stream::CmdCopyMemory` / Command 入队 | API→Stream 已确认，copy command body待补 | M06 `call-chains.md` |
| KERNEL-001 | `muapiLaunchKernel` | `CmdDispatch` / `Queue::Submit` | 已确认到 M3D cmd buffer/queue | M07 `call-chains.md` |
| GRAPH-001 | capture/graph API | `GraphExec` launch | 入口已定位，完整拓扑待补 | M08 后续 |

## INIT-001 摘要

```text
muapiInit
  -> Musa::CreatePlatform
  -> Platform::Init
  -> Hal::CreatePlatform
  -> Hal::M3d::Platform::Init
  -> IM3d::CreatePlatform
  -> EnumerateDevices / GetProperties
  -> new Hal::M3d::Device
  -> new Musa::Device
```

关键证据：[src/driver/mu_context.cpp:121-133]、[src/musa/core/platform.cpp:84-138]、[src/hal/m3d/lib.cpp:6-14]、[src/hal/m3d/platform.cpp:102-189]。

## MEM-001 摘要

```text
muapiMemAlloc_v2
  -> TlsCtxTop
  -> Context::CreateMemory
  -> Memory::Init
  -> Memory::GeneralAlloc
  -> Device.Hal().GetMemMgr()->Allocate
  -> Hal::M3d::MemMgr::Allocate
  -> MakeKey / automatic pool lookup or create
  -> MemoryPool::FullAllocate
       -> SubAllocate
       -> [errorNotFound] ChunkAllocate
       -> SubAllocate
       -> ResourceSplit
  -> Core Memory { pHalMemory=chunk, m_Offset=offset }
  -> IM3d::IDevice::CreateGpuMemory (new chunk path)
```

关键证据：[src/driver/mu_memory.cpp:271-304]、[src/musa/core/context.cpp:1037-1086]、[src/musa/core/memory.cpp:470-515]、[src/hal/m3d/memMgr.cpp:81-147]、[src/hal/m3d/memoryPool.cpp:82-211,358-413]、[src/hal/m3d/memory.cpp:398-459]。

## MEMFREE-001 摘要

```text
muapiMemFree_v2
  -> pointer tracker + allocation-base check
  -> Memory::Synchronize
  -> Context::DestroyMemory / MemoryTracker.Untrack
  -> Memory::~Memory
  -> MemMgr::Free or CorePool::Hal()->Free(aligned size)
  -> MemoryPool::Free
       -> exact busy range erase
       -> left/right merge
       -> ResourceRemove (complete chunk reuse/destroy)
       -> or FreeListInsert
```

关键证据：[src/driver/mu_memory.cpp:716-755]、[src/musa/core/memory.cpp:117-145,360-379]、[src/hal/m3d/memoryPool.cpp:214-259,318-331]。

## MEMASYNC-001 摘要

```text
muMemAllocAsync
  -> Stream::CmdMemAlloc
  -> AsyncMemAlloc
  -> Core pool CreateMemory(virtual)
  -> physical Memory::Init(flags=0)
  -> virt->Bind
  -> ModifyAccess -> CmdPaging

muMemFreeAsync
  -> Stream::CmdMemFree
  -> DisableAccess -> CmdPaging
  -> CallbackCommand
  -> DestroyPhysMemories
  -> non-graph: pool DestroyMemory(virtual)
```

关键证据：[src/driver/mu_memory.cpp:310-449]、[src/musa/core/stream.cpp:554-671]。

## MEMGRAPH-001 摘要

```text
capture
  -> CreateMemAllocNode
  -> GraphMemoryAllocNode::Init
  -> graph pool CreateMemory(virtual)
  -> GraphExec host-device submission
  -> UniversalManager::ExecuteMemAlloc / ExecuteMemFree
  -> graph resource destruction
  -> DestroyPhysMemories + graph pool DestroyMemory
```

关键证据：[src/musa/core/context.cpp:2368-2394]、[src/musa/core/node/graphMemoryAllocNode.cpp:15-48]、[src/musa/core/graph/graph1/universalManager.cpp:220-367]、[src/musa/core/graph.cpp:22-29]。

## KERNEL-001 摘要

```text
muapiLaunchKernel
  -> Context::GeneralLaunchKernel
  -> Context::CreateKernelNode
  -> Stream::CmdLaunchKernel
  -> DispatchCommand::Build
  -> Hal::M3d::CmdBuffer::CmdBindKernel / CmdDispatch
  -> DispatchCommand::Submit
  -> Hal::M3d::Queue::Submit
  -> IM3d::IQueue::Submit
```

关键证据：[src/driver/mu_module.cpp:232-272]、[src/musa/core/context.cpp:625-671]、[src/musa/core/context.cpp:2192-2218]、[src/musa/core/stream.cpp:1568-1582]、[src/musa/core/command/dispatchCommand.cpp:68-312]、[src/hal/m3d/cmdBuffer.cpp:224-227,338-364]、[src/hal/m3d/queue.cpp:178-355]。

## 使用规则

1. 本页是索引，不替代模块页；每条链的分支、错误和状态变化以模块文档为准。
2. 动态回调、生成 wrapper、MUPTI/MUASAN/GDB hook 和 M3D 子模块内部路径必须单独标记证据状态。
3. 不能从静态链接关系直接推断运行时调用；只有上面列出的函数调用链可视为当前静态确认。
