# Memory 与 MemoryPool：接口契约

- 文档目的：memory mempool：接口签名、前置条件、错误码与跨层契约。
- 适用范围：M05 模块的《Memory 与 MemoryPool：接口契约》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


- 模块：M05 Memory / MemoryPool
- 本文件描述 Driver API → Core → HAL 三层接口的输入、输出、前置条件与错误边界；所有行号用 `rg -n` 核对。

## 1. 分层接口一览

| 层 | 接口 | 定义位置 | 语义 |
|---|---|---|---|
| Driver → Core | `muapiMemAlloc_v2`/`muMemAlloc` | [src/driver/mu_memory.cpp:271-304] | 同步普通 device 分配，返回 chunk VA+offset |
| Driver → Core | `muapiMemAllocAsync` | [src/driver/mu_memory.cpp:310-347] | stream-ordered 分配；地址在 API 内保留，mapping 排队 |
| Driver → Core | `muapiMemFree_v2` | [src/driver/mu_memory.cpp:722-786] | 同步释放；要求 `offset == 0` |
| Driver → Core | `muapiMemFreeAsync` | [src/driver/mu_memory.cpp:399-456] | 按 memory type 分流；virtual 走 `CmdMemFree` |
| Driver → Core | memcpy/memset 系列 | [src/driver/mu_memory.cpp:845-919]、[src/driver/mu_memory.cpp:1620-1780] | 收敛到 `Context::GeneralMemcpy/GeneralMemset` |
| Core → HAL | `IMemory` | [src/hal/halMemory.h:291-407] | `GetDeviceVirtualAddress`/`Map`/`ExportExternalHandle`/`OpenPeerMemory` 等 |
| Core → HAL | `IMemoryPool` | [src/hal/halMemoryPool.h:62-103] | 分配/释放/统计/trim 纯虚接口 |
| Core → HAL | `IMemMgr` | `src/hal/m3d/memMgr.{h,cpp}` | 自动 pool registry 与 `CreateUserPool` |

## 2. 内存类型契约

`MemoryType` 枚举定义于 [src/musa/musaMemory.h:24-46]：

| 类型 | 创建入口 | 语义 |
|---|---|---|
| `memoryTypeGeneral` | `muapiMemAlloc_v2` | 普通 device memory；可 suballocate |
| `memoryTypePitchedGeneral` | `muapiMemAllocPitch_v2` | pitch 对齐的 device memory |
| `memoryTypePinnedHost` | `imuapiMemHostAlloc` | page-locked host，host/device 共享 VA |
| `memoryTypeRegisteredPinnedHost` | `muapiMemHostRegister_v2` | 用户 host 指针注册 |
| `memoryTypeManaged` | `muapiMemAllocManaged` | managed memory（当前等价 page-locked） |
| `memoryTypeIpcImport` | `muapiIpcOpenMemHandle_v2` | 从 IPC handle 导入 |
| `memoryTypeExternal` | `muImportExternalMemory`（`src/driver/mu_external.cpp`） | 从外部 handle 导入 |
| `memoryTypePrealloc` | internal/预分配路径 | 生命周期由外部管理 |
| `memoryTypeVirtual` | async/graph 保留的纯虚拟地址 | 不绑定物理 |

## 3. `MemoryCreateInfo` 到 HAL `MemoryAllocInfo`

Core 侧 `Memory::Init` 按类型分派（[src/musa/core/memory.cpp:507-554]）。`GeneralAlloc` 把 flags 转成 `Hal::MemoryCreateInfo` 的 alloc 字段（[src/musa/core/memory.cpp:595-641]）：

- `type = memoryAllocTypeDeviceLocal`、`heap = largePage`；
- property 恒加 `Physical | SharedVirtualAddress`；flags 含 `Virtual` 时再加 DeviceVisible/HostVisible/HostCoherent/DeviceWriteable/DeviceCached；
- viewCapability 默认 `Exportable`，flags 含 `DeviceMapped` 时加 `PeerAccessible | IpcExportable`；
- 含 `SubAllocatable` 时走 `MemMgr::Allocate`，否则走 `Hal().CreateMemory`。

HAL 侧校验在 [src/hal/m3d/memory.cpp:15-36]：size==0、overflow、超过 `totalGlobalMem`、不支持 device coherency 都会拒绝。

## 4. `IMemoryPool` 纯虚接口

[src/hal/halMemoryPool.h:62-103] 定义的分配与回收入口：

| 方法 | 行号 | 语义 |
|---|---|---|
| `SubAllocate` | 73 | 从现有 free segment 切分 |
| `ChunkAllocate` | 75 | 向底层申请新 chunk |
| `FullAllocate` | 77 | SubAllocate 失败后扩一个 chunk 再试一次 |
| `Free` | 79 | 归还区间（segment 合并/复用/销毁） |
| `GetInfo`/`GetUsage` | 86/88 | pool 属性与用途 |
| `GetFreeSize`/`GetTotalSize`/`GetMaxSize` | 90/91/92 | 统计查询 |
| `GetMemUsedSizeHigh`/`GetTotalSizeHigh` | 93/94 | high watermark |
| `TrimPool` | 97 | 按阈值释放完整空闲 chunk |
| `SetReleaseThreshold`/`GetReleaseThreshold` | 98/99 | release threshold 读写 |

## 5. `IMemory` 关键契约

[src/hal/halMemory.h:291-407]：

- `GetDeviceVirtualAddress()`（298）：HAL memory 的 device VA，不含 Core `m_Offset`；Core `Memory::GetDevicePointer()` 返回 `m_pHalMemory->GetDeviceVirtualAddress() + m_Offset`。[src/musa/core/memory.h:67]
- `Map/Unmap`（326/333）：host 映射；`Memory::GetHostPointer` 在首次访问时懒映射并叠加 `m_Offset`。[src/musa/core/memory.cpp:72-91]
- `ExportExternalHandle`（370）：dma-buf/global/fabric。
- `OpenPeerMemory`/`DestroyPeerMemory`/`GetPeerMemory`（405/406/407）：peer access 三件套。

## 6. pool 属性契约（`MUmemPool_attribute`）

`SetAttribute`/`GetAttribute` 在 [src/musa/core/memoryPool.cpp:114-242]：

| 属性 | 写入 | 读取 | 备注 |
|---|---|---|---|
| `REUSE_FOLLOW_EVENT_DEPENDENCIES` | 置 `m_DisableReuseViaEventDependencies` | 取反返回 | 当前只保存布尔值 |
| `REUSE_ALLOW_OPPORTUNISTIC` | 置 `m_DisableReuseOpportunistic` | 取反返回 | 当前只保存布尔值 |
| `REUSE_ALLOW_INTERNAL_DEPENDENCIES` | 置 `m_DisableReuseViaFalseDependencies` | 取反返回 | 当前只保存布尔值 |
| `RELEASE_THRESHOLD` | 写 HAL pool threshold | 读 HAL pool | 参与 `TrimPool` |
| `RESERVED_MEM_CURRENT/HIGH` | HIGH 支持置 0 重置 | 读 HAL total/high | HAL chunk 总量 |
| `USED_MEM_CURRENT/HIGH` | HIGH 支持置 0 重置 | 读 `m_RequestedBytes` | Core 逻辑请求字节 |
| `MAX_POOL_SIZE` | 只读 | HAL `GetMaxSize()`；imported 读共享 metadata | 创建期参数 |

## 7. 错误与生命周期边界

- HAL 错误经 `HalToMuResult`/`M3dToHalResult` 转换后向上传播；`mu_memory.cpp` 各入口对参数错误统一返回 `MUSA_ERROR_INVALID_VALUE`，对 context/stream 缺失返回 `MUSA_ERROR_INVALID_CONTEXT/HANDLE`。
- imported pool 是 metadata-only wrapper：`SetAttribute` 拒绝 imported pool，异步 pool allocation 也拒绝 imported pool。[src/driver/mu_mempool.cpp:139-211]、[src/driver/mu_memory.cpp:349-397]
- `Memory::~Memory` 的释放契约：suballocatable 且 `m_pPool == nullptr` 时经 MemMgr 自动池 `Free`；`m_pPool != nullptr` 时按 device minimum granularity 对齐后走该 Core pool 的 HAL `Free`。[src/musa/core/memory.cpp:485-505]
- `IMemoryPool::Free` 以 `{base,size}` 精确命中 `m_SegmentTracker`；调用方必须传入与分配一致的 base 和按 granularity 对齐的 size。[src/hal/m3d/memoryPool.cpp:225-270]

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_memory.cpp`（7 处，例：[src/driver/mu_memory.cpp:271-304]）
- `src/musa/core/memory.cpp`（4 处，例：[src/musa/core/memory.cpp:507-554]）
- `src/hal/halMemory.h`（2 处，例：[src/hal/halMemory.h:291-407]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
