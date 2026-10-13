# HAL/M3D 适配：调用链

- 文档目的：hal m3d：关键调用链：从公共入口到真实副作用的追踪。
- 适用范围：M09 模块的《HAL/M3D 适配：调用链》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 链 1：`MemMgr::Allocate` → `MemoryPool::FullAllocate`

```
MemMgr::Allocate                src/hal/m3d/memMgr.cpp:81
  -> 校验 size==0 / 溢出 / 超 totalGlobalMem   src/hal/m3d/memMgr.cpp:85-96
  -> ppMemoryPool 已指定: MakeKey 比对         src/hal/m3d/memMgr.cpp:99-105
  -> 未指定: MakeKey(poolInfo) 查 m_PoolRefs   src/hal/m3d/memMgr.cpp:116
     -> Get(key)->m_Value 为空/错池时 CreatePoolNoLock  src/hal/m3d/memMgr.cpp:117-132
        -> new MemoryPool + Init + 登记          src/hal/m3d/memMgr.cpp:173-198
  -> pPool->FullAllocate(allocInfo, ...)       src/hal/m3d/memMgr.cpp:144
     -> SubAllocate(…, errorNotFound 时) ChunkAllocate 再 SubAllocate  src/hal/m3d/memoryPool.cpp:85-98
        -> FindBucket -> ResourceSplit         src/hal/m3d/memoryPool.cpp:108-141
        -> ChunkAllocate -> CreateMemory        src/hal/m3d/memoryPool.cpp:184-202
```

分配规模校验发生在 `MemMgr::Allocate` [src/hal/m3d/memMgr.cpp:85-96]；池的键一致性校验在 [src/hal/m3d/memMgr.cpp:103]、[src/hal/m3d/memMgr.cpp:116-121]。`SplayTree::Get` 对缺失键返回「最近节点」而不是空指针，因此 [src/hal/m3d/memMgr.cpp:117] 的 `if (!pPool)` 只能拦空树情况，错池依赖五字段比对兜底——这是 `risks-and-debt.md` 风险 1/2 的输入路径。

## 链 2：`MemMgr::Free`

```
MemMgr::Free                    src/hal/m3d/memMgr.cpp:149
  -> 由 IMemory 重建 MemoryPoolInfo   src/hal/m3d/memMgr.cpp:150-155
     -> IsHost / GetHeap / GetProperties / GetViewCapabilities / GetNumaId
  -> m_PoolRefs.Get(MakeKey(poolInfo))  src/hal/m3d/memMgr.cpp:158
  -> MUSA_ASSERT(pMemoryPool && 键相等) src/hal/m3d/memMgr.cpp:159
  -> pMemoryPool->Free(pMemory, base, size)  src/hal/m3d/memMgr.cpp:160
     -> m_SegmentTracker 查 busy 段   src/hal/m3d/memoryPool.cpp:230-231
     -> 左/右邻居合并                   src/hal/m3d/memoryPool.cpp:240-260
     -> ResourceRemove / FreeListInsert src/hal/m3d/memoryPool.cpp:262-264
```

`Free` 无错误码出口；release 构建下 `MUSA_ASSERT` 被裁剪后，`Get` 返回错池或空值时直接落到 `pMemoryPool->Free`（[src/hal/m3d/memMgr.cpp:160]），即 risks-and-debt.md 风险 3。

## 链 3：`Device::CreateMemory` → `Memory::Init`

```
Device::CreateMemory             src/hal/m3d/device.cpp:172
  -> new Memory(*this)           src/hal/m3d/device.cpp:174
  -> Memory::Init(createInfo)    src/hal/m3d/memory.cpp:127
     -> ValidateCreateInfo       src/hal/m3d/memory.cpp:140
     -> memoryTypeAlloc:
        -> InitGeneralDeviceMemory / InitGeneralHostMemory  src/hal/m3d/memory.cpp:143-147
     -> memoryTypeView:
        -> InitPreallocMemory / InitLockedMemory / InitMigratedSharedVirtualMemory
           / InitSharedMemory / InitPeerMemory / InitExternalMemory   src/hal/m3d/memory.cpp:155-177
  -> 失败则 delete pMemory       src/hal/m3d/device.cpp:178-180
```

`Memory` 保存 `Device&`、HAL 尺寸/堆/属性，并用 `OwnPtr<IM3d::IGpuMemory>` 持有 M3D 底层内存对象 [src/hal/m3d/memory.h:58-73]。`GetRawGpuMemory` 在 prealloc view 场景返回父内存 [src/hal/m3d/memory.cpp:117-125]。

## 链 4：`Device::CreateQueue` → `Queue::Init`

```
Device::CreateQueue              src/hal/m3d/device.cpp:77
  -> new Queue(*this, createInfo)  src/hal/m3d/device.cpp:80
  -> Queue::Init()               src/hal/m3d/queue.cpp:94
     -> 按 family 名映射 queueType/engineType  src/hal/m3d/queue.cpp:101-151
        -> CDM/TCE/TDM/CE/ACE/DMA/UNIVERSAL/MMU
     -> GetQueueSize -> CreateQueue   src/hal/m3d/queue.cpp:163-176
        -> user queue 无 write signal 时回退   src/hal/m3d/queue.cpp:172-176
  -> 失败则 delete pQueue        src/hal/m3d/device.cpp:86
```

`Queue` 类声明在 [src/hal/m3d/queue.h:11-37]，成员 `m_M3dQueue` 以 `OwnPtr<IM3d::IQueue>` 持有底层队列，家族名是 HAL→M3D 引擎类型的唯一转换依据。

## 链 5：`Queue::Submit`

```
Queue::Submit                    src/hal/m3d/queue.cpp:182
  -> user queue + fence 拒绝      src/hal/m3d/queue.cpp:185-188
  -> wait/signal 信号量装配(限 32 wait / 8 signal)  src/hal/m3d/queue.cpp:199-225
  -> 超出上限的 wait 单独 WaitQueueSemaphore  src/hal/m3d/queue.cpp:229-237
  -> CmdBuffer -> GetM3dCmdbuffer 转换  src/hal/m3d/queue.cpp:281-286
  -> IM3d::IQueue->Submit(m3dSubmitInfo)  src/hal/m3d/queue.cpp:353
  -> 超出上限的 signal 单独 SignalQueueSemaphore  src/hal/m3d/queue.cpp:360-369
```

## 链 6：`CmdBuffer` 录制生命周期

```
Device::CreateCmdBuffer          src/hal/m3d/device.cpp:105
  -> CmdBuffer::Init             src/hal/m3d/cmdBuffer.cpp:75
     -> 按 family 名映射 engine  src/hal/m3d/cmdBuffer.cpp:84-129
     -> GetCmdBufferSize -> CreateCmdBuffer  src/hal/m3d/cmdBuffer.cpp:133-141
  -> CmdBuffer::Begin            src/hal/m3d/cmdBuffer.cpp:145
     -> m_M3dCmdBuffer->Begin    src/hal/m3d/cmdBuffer.cpp:185
     -> 状态 Initial->Recording  src/hal/m3d/cmdBuffer.cpp:186
  -> CmdBuffer::End              src/hal/m3d/cmdBuffer.cpp:199
     -> m_M3dCmdBuffer->End      src/hal/m3d/cmdBuffer.cpp:206
     -> 状态 Recording->Executable，回收外部分配器  src/hal/m3d/cmdBuffer.cpp:207-208
  -> CmdBuffer::Reset            src/hal/m3d/cmdBuffer.cpp:213
     -> pending 拒绝 / m_M3dCmdBuffer->Reset  src/hal/m3d/cmdBuffer.cpp:215-219
```

## 链 7：`Platform::CreateDevices`

```
Platform::Init                   src/hal/m3d/platform.cpp:102
  -> CreatePlatform              src/hal/m3d/platform.cpp:116
     -> IM3d::CreatePlatform     src/hal/m3d/platform.cpp:125
  -> CreateDevices               src/hal/m3d/platform.cpp:128
     -> EnumerateDevices         src/hal/m3d/platform.cpp:131
     -> new Device(*this, m3dDevices[i], i) -> Device::Init  src/hal/m3d/platform.cpp:181-184
        -> CommitSettingsAndInit -> InitProps -> ConstructHeapInfos -> ConstructQueueFamilies  src/hal/m3d/device.cpp:366-388
     -> CreateDevicesP2pTopoMatrix  src/hal/m3d/platform.cpp:193-194
```

## 链路缺口

- 链 1、链 2 进入 `MemoryPool::ChunkAllocate` 后调用 `m_pDevice->GetPlatform().CreateMemory` 或 `m_pDevice->CreateMemory`，再进入 `IM3d::IDevice` 的 `CreateGpuMemory` 等私有接口 [src/hal/m3d/memoryPool.cpp:193-201]；`src/hal/m3d/m3d` 子模块内部不可见，本文只确认构建边界 [src/hal/m3d/CMakeLists.txt:22-23]。
- 链 5、链 6 的最终提交与命令编码在 `IM3d::IQueue`/`IM3d::ICmdBuffer` 内部完成，未逐跳展开。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/hal/m3d/memMgr.cpp`（5 处，例：[src/hal/m3d/memMgr.cpp:85-96]）
- `src/hal/m3d/memory.h`（1 处，例：[src/hal/m3d/memory.h:58-73]）
- `src/hal/m3d/memory.cpp`（1 处，例：[src/hal/m3d/memory.cpp:117-125]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
