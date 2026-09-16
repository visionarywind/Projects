# M05：显存池执行流与生命周期

## 1. 同步普通 `muMemAlloc`

```text
API 进入
  -> 当前 Context
  -> Core Memory::GeneralAlloc
  -> HAL MemMgr::Allocate
       -> key 查找/创建自动 pool
       -> FullAllocate
            -> free bucket 查找
            -> 或创建 chunk
            -> segment split
  -> Core Context 登记 Memory
  -> MemoryTracker 登记
  -> 返回 chunk VA + offset
```

关键点：普通 API 返回前，物理/虚拟 HAL chunk 已由 `ChunkAllocate` 创建；它不是 stream command。`Context::CreateMemory` 在 `Memory::Init` 成功后才加入 context/tracker。[`src/driver/mu_memory.cpp:271-304`、`src/musa/core/context.cpp:1037-1086`]

## 2. 同步 free

```text
muMemFree_v2
  -> MemoryTracker.FindRange
  -> 要求传入的是 allocation base（offset == 0）
  -> Memory::Synchronize
  -> Context::DestroyMemory / tracker untrack
  -> shared_ptr 最后释放
  -> Memory::~Memory
       -> HAL pool Free(base, aligned size)
       -> segment merge/reuse/possible chunk Destroy
```

`Memory::Synchronize` 对 virtual memory 会遍历其 physical tracker 并等待相关 context；因此普通 free 不是单纯删除指针。[`src/driver/mu_memory.cpp:716-755`、`src/musa/core/memory.cpp:117-145`]

## 3. `muMemAllocAsync`

### 3.1 API 和 stream 解析

`muapiMemAllocAsync`：

- `dptr == nullptr` 返回 invalid value；
- `bytesize == 0` 将 dptr 置零但保持 success（与同步路径不同）；
- 取得 TLS context，并用 `Context::InfoStream` 验证 stream；
- 调用 `pStream->CmdMemAlloc`。[`src/driver/mu_memory.cpp:310-346`]

`muMemAllocFromPoolAsync` 走相同命令，但显式传入 Core pool；imported IPC pool 被拒绝。[`src/driver/mu_memory.cpp:349-390`]

### 3.2 `AsyncMemAlloc`

```text
step 1: pPool->CreateMemory
          -> pool FullAllocate virtual segment
step 2: new physical Core Memory
          -> Memory::Init(general, flags=0)
          -> direct HAL CreateMemory（非 suballocatable）
step 3: virt->Bind(physical)
          -> pPool->ModifyAccess
          -> Stream::CmdPaging
          -> ResolveDependencyAndQueueCommand
```

源码 [`src/musa/core/stream.cpp:554-600`] 明确展示了 virtual reserve、physical allocation、bind、mapping 四步。mapping 失败会先 `Unbind`，再 `pPool->DestroyMemory(virt)`；physical shared pointer 离开作用域后释放。

`pPool->SetStream(this)` 是 pool 级写入，而非单次 allocation 字段；因此多个 stream 共享同一 Core pool 时，访问更新所用 stream 的并发语义需要目标运行验证。[`src/musa/core/stream.cpp:561-570`]

## 4. `muMemFreeAsync`

API 先查 pointer 对应 Memory：

- IPC/external：直接 `Context::DestroyMemory`；
- general/pitched/managed：`Memory::Synchronize` 后销毁；
- virtual 且有 pool：转到 `Stream::CmdMemFree`；
- virtual 无 pool 或未知类型：错误。[`src/driver/mu_memory.cpp:393-449`]

非 capture 的 `AsyncMemFree`：

```text
lookup virt + physical + pPool
  -> pPool->DisableAccess
       -> 组装 PROT_NONE paging
       -> CmdPaging 入队
  -> CallbackCommand 入队
       callback:
         virt->DestroyPhysMemories()
         if (!virt->IsGraphAlloc(): pPool->DestroyMemory(virt)
```

源码 [`src/musa/core/stream.cpp:634-671`]。因此普通 async virtual free 的 pool segment 在 callback 完成后才回收；graph allocation 则故意不在该 callback 中归还，graph 生命周期负责归还。

## 5. command 完成和 pool 更新

`Command::Wait` 在 command 未完成时调用 HAL `MemMgr::UpdateUserPools`，`Stream::WaitFinish` 在等待后再次调用。[`src/musa/core/command/command.cpp:238-247`、`src/musa/core/stream.cpp:1113-1121`]

HAL `MemMgr::UpdateUserPools` 遍历 `m_UserPools`，对每个 pool 执行：

```text
releaseThreshold = pool->GetReleaseThreshold()
pool->TrimPool(releaseThreshold)
```

[`src/hal/m3d/memMgr.cpp:229-235`]

当前实现中没有在 `UpdateUserPools` 内扫描 event、semaphore 或 command completion list；它的直接作用是按 threshold trim 已经成为完整 free chunk 的 user pool。异步安全性由 stream command ordering 和 callback 本身提供，不能把 UpdateUserPools 解释为完整的 deferred-free engine。

## 6. capture/graph memory alloc

```text
capture active
  -> Stream::CaptureMemAlloc
  -> Context::CreateMemAllocNode
  -> GraphMemoryAllocNode::Init
       -> Device::GetGraphMemoryPool
       -> graph pool CreateMemory（virtual only）
       -> nodeParams.dptr = reserved VA
  -> Graph::AddGraphNode
```

[`src/musa/core/stream.cpp:538-551`、`src/musa/core/context.cpp:2368-2382`、`src/musa/core/node/graphMemoryAllocNode.cpp:15-48`]

`GraphMemoryFreeNode::Init` 当前是 no-op；free node 只保存虚拟地址。[`src/musa/core/node/graphMemoryFreeNode.cpp:6-13`]

graph execution 将 alloc/free node 转成 `MUSA_SUBMISSION_HOST_DEVICE`：

- alloc：`UniversalManager::ExecuteMemAlloc` 等待依赖，创建 physical，打开 peer，paging，并 signal；[`src/musa/core/graph/graph1/universalManager.cpp:220-301`]
- free：`ExecuteMemFree` 等待依赖，解除各设备 mapping，销毁 peer mapping，`DestroyPhysMemories`，并 signal；[`src/musa/core/graph/graph1/universalManager.cpp:304-367`]

graph resource 析构时：

```text
DestroyPhysMemories
  -> pPool->DestroyMemory(virtual)
  -> tracker untrack
  -> Core Memory 析构
  -> HAL pool Free virtual segment
```

[`src/musa/core/graph.cpp:22-29`]

## 7. default/current/host/NUMA/graph pool

| pool | 创建点 | 初始 chunk quantum | owner |
|---|---|---:|---|
| device default | `Device::GetDefaultMemoryPool` | 32 MiB | Core Device 析构 |
| device current | `m_CurrentMemoryPool`，未设置时回退 default | 同 current 指向 pool | Device 仅保存 raw pointer，不拥有替代 pool |
| graph | `Device::GetGraphMemoryPool` | 32 MiB | Core Device 析构 |
| host default | `Platform::GetDefaultHostMemoryPool` | 32 MiB | Core Platform 析构 |
| host NUMA default | `CreateHostMemoryPoolInternal(numaId)` | 32 MiB | Platform 的 vector 析构 |
| internal | `Device::GetInternalPool` → HAL internal list | HAL 2 MiB 初始 chunk | HAL MemMgr list；Core wrapper release |
| synchronous auto | `MemMgr::Allocate` 按 key创建 | HAL 2 MiB quantum | HAL MemMgr splay tree |

device default/graph 的创建证据为 [`src/musa/core/device.cpp:439-519`]；host/NUMA 为 [`src/musa/core/platform.cpp:544-645`]；internal 为 [`src/musa/core/device.cpp:1091-1152`]。

## 8. trim 和进程退出

显式 `muMemPoolTrimTo` 直接调用 Core `TrimPoolToValue` → HAL `TrimPool(value)`。[`src/driver/mu_mempool.cpp:242-262`、`src/musa/core/memoryPool.cpp:430-437`]

进程/Platform 析构前会等待 device，再清理未跟踪 memory；之后删除 host/NUMA pool、清空 devices，最后销毁 HAL platform。[`src/musa/core/platform.cpp:478-541`]

Device 析构先等待 contexts，释放 primary context，再 delete default/graph Core pools。[`src/musa/core/device.cpp:680-700`]

具体 HAL splay tree value 的最终 delete 语义仍依赖 `Util::SplayTree` 实现，本文不越过已读证据。
