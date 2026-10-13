# Memory 与 MemoryPool：例子

- 文档目的：memory mempool：用法示例与边界/错误输入说明。
- 适用范围：M05 模块的《Memory 与 MemoryPool：例子》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


- 模块：M05 Memory / MemoryPool
- 以下例子是基于本地静态源码的**阅读模型**，不是已运行程序；每个例子给出从 Driver API 到 HAL 的可核对调用链。

## 1. 同步普通分配与释放

```text
MUdeviceptr dptr;
muMemAlloc(&dptr, N);
  -> muapiMemAlloc_v2                                [src/driver/mu_memory.cpp:271-304]
       -> pContext->CreateMemory(&pMemory, createInfo)
            -> Memory::Init(general, Virtual|DeviceMapped|SubAllocatable)
                 -> GeneralAlloc                       [src/musa/core/memory.cpp:595-641]
                      -> MemMgr::Allocate               [src/hal/m3d/memMgr.cpp:81-147]
                           -> 按 MakeKey 找/建自动 pool
                           -> FullAllocate              [src/hal/m3d/memoryPool.cpp:85-98]
                                -> SubAllocate/ChunkAllocate
                                -> 返回 pMemory + offset
  dptr = pMemory->GetDevicePointer()   // chunk VA + offset

muMemFree(dptr);
  -> muapiMemFree_v2                                   [src/driver/mu_memory.cpp:722-786]
       -> offset == 0 校验
       -> Memory::Synchronize / Context::DestroyMemory
       -> 最后 shared_ptr 释放 -> Memory::~Memory       [src/musa/core/memory.cpp:485-505]
            -> MemMgr::Free -> HAL pool Free             [src/hal/m3d/memoryPool.cpp:225-270]
```

## 2. 从 stream 异步分配（地址保留与映射分离）

```text
muMemAllocAsync(&dptr, N, stream);
  -> muapiMemAllocAsync                                 [src/driver/mu_memory.cpp:310-347]
       -> Stream::CmdMemAlloc                           [src/musa/core/stream.cpp:577-587]
            -> AsyncMemAlloc                            [src/musa/core/stream.cpp:529-575]
                 1. pPool->CreateMemory(&virt, &addr, N)  // 保留虚拟 segment
                 2. physical->Init(general, flags=0)      // 创建物理 backing
                 3. virt->Bind(physical)                  // 绑定
                 4. pPool->ModifyAccess -> CmdPaging      // 映射入队
  dptr = param.virtAddress
```

注意：`dptr` 在 API 返回时就可用作地址，但 mapping 的可见性由 stream 命令顺序决定；失败路径先 `Unbind` 再 `ReleaseMemoryPoolAllocation`。[src/musa/core/stream.cpp:563-573]

## 3. 异步释放（unmap 先、回收后）

```text
muMemFreeAsync(dptr, stream);
  -> muapiMemFreeAsync                                  [src/driver/mu_memory.cpp:399-456]
       -> Stream::CmdMemFree                            [src/musa/core/stream.cpp:655-668]
            -> AsyncMemFree                             [src/musa/core/stream.cpp:608-653]
                 1. pPool->DisableAccess(virt, physical)  // PROT_NONE paging 入队
                 2. CallbackCommand 入队
                      callback:
                        DestroyPhysMemories()
                        非 graph -> ReleaseMemoryPoolAllocation(pool, memory)
```

callback 捕获 `[virt, graphMemory, pPool, freeGeneration]`；`virt`/`pPool` 是 raw pointer，不延长 pool 生命周期。

## 4. memcpy 与 memset 的公共收敛链

```text
muMemcpyHtoD(dstDev, srcHost, N);
  -> muapiMemcpyHtoD_v2                                 [src/driver/mu_memory.cpp:845-855]
       -> Context::GeneralMemcpy(TlsCtxTop(), nullptr, copy3D, true)
            -> CreateMemcpyNode                         [src/musa/core/context.cpp:2154-2172]
            -> Stream::CmdCopyMemory                    [src/musa/core/stream.cpp:696-753]
                 -> AsyncMemcpyCommand / SyncMemcpyCommand
                 -> ResolveDependencyAndQueueCommand

muMemsetD32(dstDev, val, N);
  -> muapiMemsetD32_v2                                  [src/driver/mu_memory.cpp:1678-1687]
       -> Context::GeneralMemset(TlsCtxTop(), nullptr, memsetParams, true)
            -> CreateMemsetNode                         [src/musa/core/context.cpp:2210-2228]
            -> Stream::CmdMemset                        [src/musa/core/stream.cpp:754-763]
                 -> MemsetCommand
                 -> ResolveDependencyAndQueueCommand
```

同步版本 `wait=true`；异步版本 `wait=memBlocking`（settings）。

## 5. 创建 user pool 并 trim

```text
muMemPoolCreate(&pool, createInfo);
  -> new Musa::MemoryPool
  -> Init(GeneralAlloc)                                  [src/musa/core/memoryPool.cpp:21-73]
       -> Hal::MemMgr::CreateUserPool                     [src/hal/m3d/memMgr.cpp:163-171]
            -> CreatePoolNoLock(usageFlags.userManaged=true)
                 -> m_UserPools.emplace_back(pool)
muMemPoolTrimTo(pool, N);
  -> muapiMemPoolTrimTo                                   [src/driver/mu_mempool.cpp:215-232]
       -> MemoryPool::TrimPoolToValue -> Hal TrimPool     [src/hal/m3d/memoryPool.cpp:502-532]
```

`TrimPool` 只释放完整空闲 chunk，含 busy segment 的 chunk 不会被 trim。

## 6. pool handle 的 IPC 导出与导入

```text
export:
  muMemPoolExportToShareableHandle(&fd, pool, handleType, 0);
    -> MemoryPool::Export                                [src/musa/core/memoryPool.cpp:528-562]
         -> CreateIpcMemPoolShmemIfNeed                   [src/musa/core/memoryPool.cpp:606-672]
              mkstemp / shm_open / ftruncate / mmap
              owners = 1
         -> dup(fd) 返回调用方
import:
  muMemPoolImportFromShareableHandle(&pool, fd, ...);
    -> MemoryPool::InitFromHandle                         [src/musa/core/memoryPool.cpp:564-604]
         dup(fd) -> mmap(metadata) -> owners += 1 -> m_IsImported = true
```

imported wrapper 没有 `m_pHalPool`，因此不能用于异步 pool allocation；`SetAttribute` 也会拒绝。[src/driver/mu_mempool.cpp:139-211]

## 7. 指针级 IPC 与外部内存

```text
muIpcGetMemHandle(&handle, dptr);
  -> muapiIpcGetMemHandle                                [src/driver/mu_memory.cpp:2139-2167]
       -> pMemory->ExportIpcHandle                        [src/musa/core/memory.cpp:107-130]
muIpcOpenMemHandle(&dptr, handle, flags);
  -> muapiIpcOpenMemHandle_v2                            [src/driver/mu_memory.cpp:2169-2211]
       -> memoryTypeIpcImport -> Memory::IpcImportAlloc   [src/musa/core/memory.cpp:871-914]
muImportExternalMemory(&extMem, memHandleDesc);
  -> muapiImportExternalMemory                          [src/driver/mu_external.cpp:146-165]
       -> Context::CreateExternalMemory
       -> GetMappedBuffer -> Memory::ExternalAlloc        [src/musa/core/memory.cpp:916-955]
```

## 8. 使用注意

- 任何 IPC `mmap` 结果都要按 `MAP_FAILED` 判断，不能按 `nullptr`。
- destroy pool 前确保没有 live allocation；当前 driver 入口没有显式检查 `m_MemoryAllocations`。
- 不要把 `RESERVED_MEM` 当作 `USED_MEM`，两者代表 chunk 总量与逻辑请求字节。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_memory.cpp`（8 处，例：[src/driver/mu_memory.cpp:271-304]）
- `src/musa/core/stream.cpp`（7 处，例：[src/musa/core/stream.cpp:577-587]）
- `src/musa/core/memory.cpp`（5 处，例：[src/musa/core/memory.cpp:595-641]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
