# Memory 与 MemoryPool：源码地图

- 模块：M05 Memory / MemoryPool
- 文档目的：memory mempool：源码文件/目录到文档章节的映射。
- 适用范围：M05 模块的《Memory 与 MemoryPool：源码地图》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)
- 本文件列出本模块范围内文件的职责与关键行号锚点；行号用 `rg -n` 核对。

## 1. 文件职责总览

| 层 | 路径 | 职责 |
|---|---|---|
| Driver 入口 | `src/driver/mu_memory.cpp` | 同步/异步 alloc、free、memcpy、memset、host register、IPC handle、pointer attribute 等 API 参数校验与对象解析 |
| Core 对象 | `src/musa/core/memory.{h,cpp}` | `Musa::Memory` 生命周期、`Init`/`GeneralAlloc` 及各 memory type 分配路径、`Bind`/`Unbind`/`EnableAccess`、IPC/dma-buf export |
| Core pool | `src/musa/core/memoryPool.{h,cpp}` | `Musa::MemoryPool` 创建/属性/访问控制/统计/async CreateMemory/销毁/IPC metadata |
| HAL 接口 | `src/hal/halMemoryPool.h` | `Hal::IMemoryPool` 纯虚接口与 `MemoryPoolInfo`/`MemoryPoolCreateInfo` |
| HAL 实现 | `src/hal/m3d/memoryPool.{h,cpp}` | `Hal::M3d::MemoryPool`：chunk/`ResSegment`/free bucket/`m_SegmentTracker` 分配与回收 |
| HAL 实现 | `src/hal/m3d/memory.cpp` | `Hal::M3d::Memory`：CreateGpuMemory 适配、view 类型、export/peer 打开 |
| 支撑（跨模块） | `src/hal/m3d/memMgr.{h,cpp}`、`src/util/utilSplayTree.h` | 自动 pool registry、`MakeKey`、`SplayTree` 查找（含已登记缺陷） |
| 支撑（跨模块） | `src/musa/core/context.cpp`、`stream.cpp`、`device.cpp`、`platform.cpp`、`command/command.cpp` | `CreateMemory`/async 命令/device/host 池/`UpdateUserPools` 调用点 |

## 2. Driver 入口：`src/driver/mu_memory.cpp`

| 行号 | 函数 | 说明 |
|---|---|---|
| [src/driver/mu_memory.cpp:61-103] | `imuapiMemHostAlloc` | pinned host alloc 公共实现；flags 掩码校验、`memoryTypePinnedHost` |
| [src/driver/mu_memory.cpp:271-304] | `muapiMemAlloc_v2` | 同步普通 alloc；general + Virtual/DeviceMapped/SubAllocatable |
| [src/driver/mu_memory.cpp:310-347] | `muapiMemAllocAsync` | async alloc；解析 stream 后 `CmdMemAlloc` |
| [src/driver/mu_memory.cpp:349-397] | `muapiMemAllocFromPoolAsync` | 显式 Core pool 的 async alloc；拒绝 imported pool |
| [src/driver/mu_memory.cpp:399-456] | `muapiMemFreeAsync` | 按 memory type 分流；virtual+pool 走 `CmdMemFree` |
| [src/driver/mu_memory.cpp:514-546] | `muapiMemAllocManaged` | managed alloc；`MU_MEM_ATTACH_GLOBAL/HOST` 校验 |
| [src/driver/mu_memory.cpp:722-786] | `muapiMemFree_v2` | 同步 free；要求 allocation base（offset==0） |
| [src/driver/mu_memory.cpp:792-815] | `muapiMemFreeHost` | pinned host 同步 free |
| [src/driver/mu_memory.cpp:845-855] | `muapiMemcpyHtoD_v2` | memcpy H2D 同步入口 |
| [src/driver/mu_memory.cpp:877-887] | `muapiMemcpyDtoH_v2` | memcpy D2H 同步入口 |
| [src/driver/mu_memory.cpp:909-919] | `muapiMemcpyDtoD_v2` | memcpy D2D 同步入口 |
| [src/driver/mu_memory.cpp:1014-1024] | `muapiMemcpyDtoDAsync_v2` | memcpy D2D 异步入口 |
| [src/driver/mu_memory.cpp:1035-1057] | `muapiMemcpy`/`muapiMemcpyAsync` | UVA 版本，方向由指针推断 |
| [src/driver/mu_memory.cpp:1620-1629] | `muapiMemsetD8_v2` | memset D8 同步入口 |
| [src/driver/mu_memory.cpp:1678-1687] | `muapiMemsetD32_v2` | memset D32 同步入口 |
| [src/driver/mu_memory.cpp:1693-1702] | `muapiMemsetD32Async` | memset D32 异步入口 |
| [src/driver/mu_memory.cpp:1782-1841] | `muapiMemHostRegister_v2`/`muapiMemHostUnregister` | 用户 host 指针注册/注销 |
| [src/driver/mu_memory.cpp:2139-2167] | `muapiIpcGetMemHandle` | 指针级 IPC handle 导出 |
| [src/driver/mu_memory.cpp:2169-2211] | `muapiIpcOpenMemHandle_v2` | IPC handle 导入 |
| [src/driver/mu_memory.cpp:2746-2780] | `muapiMemGetHandleForAddressRange` | dma-buf fd 导出；要求 host page size 对齐 |

## 3. Core `Memory`：`src/musa/core/memory.{h,cpp}`

| 行号 | 函数/结构 | 说明 |
|---|---|---|
| [src/musa/core/memory.h:26-46] | `IpcHandleInternal` | IPC handle 内部布局（hal handle + pciBusId + allocOffset/size + pid + serial） |
| [src/musa/core/memory.h:48-59] | `GraphAllocState`/`GraphAlloc` | graph allocation 状态机与 owner 释放标记 |
| [src/musa/core/memory.h:61-164] | `class Memory` | Core 用户可见内存对象；`m_pHalMemory`+`m_Offset` 表示 chunk 内偏移 |
| [src/musa/core/memory.cpp:15-66] | `Memory::ValidateCreateInfo` | memory type 的 deviceMapped 判定 + capture 状态检查 |
| [src/musa/core/memory.cpp:170-199] | `Memory::Synchronize` | virtual 遍历 `m_PhysTracker`；非 virtual 同步 context 与 peer context |
| [src/musa/core/memory.cpp:277-339] | `Memory::Bind` | virtual 绑定 physical；重叠检查；unix 下 fd 重映射 |
| [src/musa/core/memory.cpp:341-390] | `Memory::Unbind` | PROT_NONE paging + `DestroyPeerMemory` 后解除物理属性 |
| [src/musa/core/memory.cpp:392-463] | `Memory::EnableAccess` | peer 打开 + paging 组装；granularity 校验 |
| [src/musa/core/memory.cpp:485-505] | `Memory::~Memory` | 按 suballocatable/`m_pPool` 分流到 MemMgr/pool Free 或直接 Destroy |
| [src/musa/core/memory.cpp:507-554] | `Memory::Init` | 按 memory type 分派到各 `*Alloc` 路径 |
| [src/musa/core/memory.cpp:556-586] | `Memory::InitFromPool` | pool 分配前按 device minimum granularity 对齐 size，再 `FullAllocate` |
| [src/musa/core/memory.cpp:595-641] | `Memory::GeneralAlloc` | 组装 `Hal::MemoryCreateInfo`；SubAllocatable → `MemMgr::Allocate` |
| [src/musa/core/memory.cpp:688-765] | `Memory::PinnedHostAlloc` | pinned host：property 按 flags 组合；MAP_FAILED 时 heap 降级 general |
| [src/musa/core/memory.cpp:767-827] | `Memory::PinnedHostRegister` | 用户指针注册：overlap 检查 + view 创建 |
| [src/musa/core/memory.cpp:829-869] | `Memory::ManagedAlloc` | managed：host/device 分配类型选择；成功后 `GetHostPointer` |
| [src/musa/core/memory.cpp:871-914] | `Memory::IpcImportAlloc` | IPC handle 导入为 view；同进程且非 mempool 导出时拒绝 |
| [src/musa/core/memory.cpp:916-955] | `Memory::ExternalAlloc` | 外部 fd/fabric 导入为 view |
| [src/musa/core/memory.cpp:961-981] | `Memory::VirtualAlloc` | 纯虚拟地址区间保留 |

## 4. Core `MemoryPool`：`src/musa/core/memoryPool.{h,cpp}`

| 行号 | 函数/结构 | 说明 |
|---|---|---|
| [src/musa/core/memoryPool.h:37-75] | `MemoryPoolCreateInfo` | General/External/Internal 三种初始化类型 |
| [src/musa/core/memoryPool.h:77-82] | `IpcMemPoolShmem_t` | IPC 共享 metadata：`owners` + allocationType/location/maxSize |
| [src/musa/core/memoryPool.h:84-99] | `IpcMemPoolData_t` | 进程内 IPC fd/shmem 句柄与 imported 标记 |
| [src/musa/core/memoryPool.h:101-183] | `class MemoryPool` | Core pool 句柄：HAL pool、access map、allocation set、统计 |
| [src/musa/core/memoryPool.cpp:21-73] | `MemoryPool::Init` | GeneralAlloc→`CreateUserPool`；ExternalAlloc→`InitFromHandle`；InternalAlloc→`GetInternalPool` |
| [src/musa/core/memoryPool.cpp:97-112] | `MemoryPool::~MemoryPool` | internal 走 `ReleaseInternalPool`，其他走 `DestroyUserPool` |
| [src/musa/core/memoryPool.cpp:114-168] | `SetAttribute` | reuse 三开关、release threshold、high-water 重置 |
| [src/musa/core/memoryPool.cpp:170-242] | `GetAttribute` | USED/RESERVED/MAX_POOL_SIZE 等读取 |
| [src/musa/core/memoryPool.cpp:244-324] | `SetAccess` | 校验 descriptors、更新 location map、对已有 allocation 逐个 `ModifyAccess` |
| [src/musa/core/memoryPool.cpp:344-389] | `ModifyAccess` | 跨设备 `OpenPeerMemory` + 组装 paging + `CmdPaging` |
| [src/musa/core/memoryPool.cpp:391-420] | `DisableAccess` | 对所有 location 组装 PROT_NONE paging |
| [src/musa/core/memoryPool.cpp:423-458] | `CreateMemory` | `InitFromPool` + tracker/set 登记 + requested bytes 统计 |
| [src/musa/core/memoryPool.cpp:460-475] | `DestroyMemory` | 从 set/tracker 移除；Detached 且空时允许销毁 pool |
| [src/musa/core/memoryPool.cpp:528-562] | `Export` | POSIX fd dup；`CreateIpcMemPoolShmemIfNeed` |
| [src/musa/core/memoryPool.cpp:564-604] | `InitFromHandle` | 只 dup fd + mmap metadata + owners+1，不建 HAL pool |
| [src/musa/core/memoryPool.cpp:606-672] | `CreateIpcMemPoolShmemIfNeed` | mkstemp/shm_open/ftruncate/mmap 初始化共享 metadata |
| [src/musa/core/memoryPool.cpp:674-709] | `DestroyIpcMemPoolShmemIfNeed` | owners-1/munmap/close；非 imported 且 owners==0 时 unlink |

## 5. HAL 接口：`src/hal/halMemoryPool.h`

| 行号 | 内容 | 说明 |
|---|---|---|
| [src/hal/halMemoryPool.h:8-14] | `MemoryPoolInfo` | type/heap/property/viewCapability/numaId |
| [src/hal/halMemoryPool.h:16-23] | `MemoryPoolUsageFlags` | userManaged/internal 位域 |
| [src/hal/halMemoryPool.h:25-35] | `MemoryPoolCreateInfo` | info + usage + 各 chunk 策略参数 |
| [src/hal/halMemoryPool.h:37-43] | `InsertPolicy` | fast/optimal 插入策略 |
| [src/hal/halMemoryPool.h:45-60] | `SelectPolicy` | assuredFit/bestFit 选择策略 |
| [src/hal/halMemoryPool.h:62-103] | `IMemoryPool` | `SubAllocate`/`ChunkAllocate`/`FullAllocate`/`Free`/统计查询/`TrimPool`/`SetReleaseThreshold` |

## 6. HAL M3D pool：`src/hal/m3d/memoryPool.{h,cpp}`

| 行号 | 函数/结构 | 说明 |
|---|---|---|
| [src/hal/m3d/memoryPool.h:43-47] | `Policy` | insertion + selection + noSplit |
| [src/hal/m3d/memoryPool.h:50-86] | `ResSegment` | chunk 内区间；空间链 + free 链 + lazyFreeCount + leftmost/rightmost |
| [src/hal/m3d/memoryPool.h:88-96] | `MemoryRange` | 闭区间 `[base, base+size-1]` 排序键 |
| [src/hal/m3d/memoryPool.h:100-102] | `s_FreeTableLimit`/`s_DefaultChunkAllocSize` | bucket 数上限；HAL 默认 2 MiB chunk quantum |
| [src/hal/m3d/memoryPool.cpp:16-42] | `FindBucket` | 顺序遍历 free list，AlignUp + 容量检查，受 tryLimit 限制 |
| [src/hal/m3d/memoryPool.cpp:85-98] | `FullAllocate` | SubAllocate → errorNotFound → ChunkAllocate → 再 SubAllocate 一次 |
| [src/hal/m3d/memoryPool.cpp:100-154] | `SubAllocate` | Log2 bucket + 位图；assuredFit/bestFit 两种选择 |
| [src/hal/m3d/memoryPool.cpp:156-223] | `ChunkAllocate` | 对齐/量化后创建 virtual 或 physical chunk |
| [src/hal/m3d/memoryPool.cpp:225-270] | `Free` | tracker 精确删除 + 左右合并 + `ResourceRemove`/free-list |
| [src/hal/m3d/memoryPool.cpp:340-354] | `ResourceRemove` | 完整 chunk 的 lazyFreeCount 与 reuse limit 判定 |
| [src/hal/m3d/memoryPool.cpp:380-435] | `ResourceSplit` | prefix/suffix 双切分；virtual busy range 入 tracker |
| [src/hal/m3d/memoryPool.cpp:438-500] | `FreeListInsert`/`FreeListRemove` | Log2 size bucket + bitmap 维护 |
| [src/hal/m3d/memoryPool.cpp:502-532] | `TrimPool` | 只释放完整空闲 chunk，直到 total 不超过阈值 |

## 7. HAL M3D memory：`src/hal/m3d/memory.cpp`

| 行号 | 函数 | 说明 |
|---|---|---|
| [src/hal/m3d/memory.cpp:127-191] | `Memory::Init` | alloc/view 两类创建入口分派 |
| [src/hal/m3d/memory.cpp:193-274] | `ExportExternalHandle` | dma-buf/kernelManagedGlobal/fabric 导出 |
| [src/hal/m3d/memory.cpp:276-392] | `OpenPeerMemory` | kernelManagedGlobal/dmaBuf 两条 peer 打开路径，topo 选择 |
| [src/hal/m3d/memory.cpp:394-404] | `DestroyPeerMemory` | 销毁 peer memory 并置空 |
| [src/hal/m3d/memory.cpp:406-467] | `InitGeneralDeviceMemory` | device-local：GetGpuMemorySize + CreateGpuMemory；svm/heap/物理分配标志 |
| [src/hal/m3d/memory.cpp:469-570] | `InitGeneralHostMemory` | host：svm（SharedVirtualAddress）或普通 Gart heap |
| [src/hal/m3d/memory.cpp:572-626] | `InitPreallocMemory` | device/host 预分配 view |
| [src/hal/m3d/memory.cpp:628-681] | `InitLockedMemory` | host register：pinned GPU memory |
| [src/hal/m3d/memory.cpp:683-720] | `InitMigratedSharedVirtualMemory` | SVM 迁移 |
| [src/hal/m3d/memory.cpp:722-743] | `InitSharedMemory` | 打开共享 GPU memory |
| [src/hal/m3d/memory.cpp:745-777] | `InitPeerMemory` | 打开 peer GPU memory |
| [src/hal/m3d/memory.cpp:779-908] | `InitExternalMemory` | dma-buf/global/fabric/mmio 外部导入 |

## 8. 自动 pool registry（跨模块证据）

- [src/hal/m3d/memMgr.h:98-115]：`MakeKey` 把 property/view/type/heap/NUMA 压成整数 key。
- [src/hal/m3d/memMgr.cpp:81-147]：`Allocate` 校验后按 key 查找/创建自动 pool。
- [src/hal/m3d/memMgr.cpp:149-161]：`Free` 用 `Get` 找回自动 pool 后 `Free`。
- [src/util/utilSplayTree.h:215-226]：`Search` 分支方向与 BST 直觉相反，是已登记缺陷，见 [`../../99-roadmap/rca-splaytree-memmgr.md`](../../99-roadmap/rca-splaytree-memmgr.md)。

## 9. 支撑调用点（跨模块）

- [src/musa/core/context.cpp:1044-1079]：`Context::CreateMemory`（先 `ValidateCreateInfo`，再 `Init`，成功后登记 context/tracker）。
- [src/musa/core/context.cpp:702-734]：`Context::GeneralMemcpy`（memcpy 链公共收敛点）。
- [src/musa/core/context.cpp:862-895]：`Context::GeneralMemset`（memset 链公共收敛点）。
- [src/musa/core/stream.cpp:529-575]：`AsyncMemAlloc`；[src/musa/core/stream.cpp:608-653]：`AsyncMemFree`。
- [src/musa/core/device.cpp:451-497]：`GetDefaultMemoryPool`；[src/musa/core/device.cpp:499-522]：current 池三方法；[src/musa/core/device.cpp:524-561]：`GetGraphMemoryPool`；[src/musa/core/device.cpp:1155-1172]：`GetInternalPool`；[src/musa/core/device.cpp:1174-1203]：`AllocateInternalMem`；[src/musa/core/device.cpp:1205-1217]：`FreeInternalMem`。
- [src/musa/core/platform.cpp:658-689]：`CreateHostMemoryPoolInternal`；[src/musa/core/platform.cpp:736-750]：`GetDefaultHostMemoryPool`；[src/musa/core/platform.cpp:752-763]：`GetCurrentHostMemoryPool`；[src/musa/core/platform.cpp:765-768]：`SetCurrentHostMemoryPool`；[src/musa/core/platform.cpp:797-802]：`ReleaseMemoryPoolAllocation`。

## 10. 证据锚点与阅读顺序

- 先读 [`design.md`](design.md) 理解三层 pool 划分，再读 [`data-structures.md`](data-structures.md) 认识 `ResSegment`/registry，最后用本地图定位函数后读 [`implementation.md`](implementation.md)。
- 搜索建议：

```bash
grep -R "memoryType" src/musa src/driver
grep -R "MemoryPool" src/hal src/musa
grep -R "FullAllocate" src/hal
```

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_memory.cpp`（20 处，例：[src/driver/mu_memory.cpp:61-103]）
- `src/musa/core/memory.cpp`（15 处，例：[src/musa/core/memory.cpp:15-66]）
- `src/musa/core/memoryPool.cpp`（13 处，例：[src/musa/core/memoryPool.cpp:21-73]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
