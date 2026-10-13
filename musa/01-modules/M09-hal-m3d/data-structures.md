# HAL/M3D 适配：数据结构与所有权

- 文档目的：hal m3d：核心数据结构、字段含义与持有关系。
- 适用范围：M09 模块的《HAL/M3D 适配：数据结构与所有权》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 结构关系

```text
Hal::IPlatform
  └─ Hal::M3d::Platform
       ├─ OwnPtr<IM3d::IPlatform> m_M3dPlatform
       ├─ std::vector<Device*>    m_Devices
       └─ vector<vector<PlatformDeviceP2pCapability>> m_DevicesP2pCapabilities

Hal::IDevice
  └─ Hal::M3d::Device
       ├─ ::M3d::IDevice*  m_M3dDevice
       ├─ MemMgr           m_MemMgr
       └─ IMemoryPool*     m_pInternalMemoryPool

Hal::IMemoryPool
  └─ Hal::M3d::MemoryPool
       ├─ ResSegment*  m_pHeadSegment
       ├─ ResSegment*  m_FreeBuckets[]
       ├─ MemSegTree   m_SegmentTracker
       └─ OwnPtr<IM3d::IGpuMemory>（经 ChunkAllocate 持有）

Hal::IQueue
  └─ Hal::M3d::Queue
       └─ OwnPtr<IM3d::IQueue> m_M3dQueue
```

## 所有权要点

- `Platform` 拥有 `m_Devices`（`std::vector<Device*>`），析构时逐个 `delete` [src/hal/m3d/platform.h:40]、[src/hal/m3d/platform.cpp:96-100]。
- `Device` 内嵌 `MemMgr m_MemMgr`（值成员），构造函数以 `this` 初始化 [src/hal/m3d/device.h:119]、[src/hal/m3d/device.cpp:350]。
- `MemoryPool` 通过 `OwnPtr<IM3d::IGpuMemory>` 持有底层 M3D 显存；虚拟属性段登记进 `m_SegmentTracker`（`std::map<MemoryRange, ResSegment*>`）[src/hal/m3d/memoryPool.h:98]、[src/hal/m3d/memoryPool.h:152]。
- `Memory` 保存 `Device&`、尺寸/堆/属性，`m_M3dGpuMemory` 持有顶层 M3D 内存，`m_M3dSourceGpuMemory` 在 prealloc view 时指向父内存 [src/hal/m3d/memory.h:59-70]。
- `Queue`/`CmdBuffer` 均以 `OwnPtr` 持有对应 M3D 对象 [src/hal/m3d/queue.h:35]、[src/hal/m3d/cmdBuffer.h:142]。

## `MemMgr` 成员（池注册表）

| 成员 | 类型 | 职责 | 位置 |
|---|---|---|---|
| `m_InternalPoolRefs` | `std::array<MemPoolList, InternalMemoryPoolType::Count>` | 内部池，按 `internalType` 下标组织，构造后常驻 | [src/hal/m3d/memMgr.h:123] |
| `m_PoolRefs` | `MemPoolTree`（`::Util::SplayTree<Key, MemoryPool*>`） | 外部自动池注册表；键由 `MakeKey` 生成 | [src/hal/m3d/memMgr.h:125]、[src/hal/m3d/memMgr.h:37] |
| `m_UserPools` | `std::list<MemoryPool*>` | 用户托管池列表 | [src/hal/m3d/memMgr.h:127] |
| `m_Lock` | `std::mutex` | 全部注册表访问的互斥锁 | [src/hal/m3d/memMgr.h:128] |

`MakeKey` 把 `property`、`viewCapability`、`type`、`heap`、`numaId` 五个字段按位移打包进一个 `DevSize` [src/hal/m3d/memMgr.h:98-104]；`s_InternalInfos` 提供 5 种内置池模板 [src/hal/m3d/memMgr.h:88-94]。SplayTree 的缺陷链（`Search` 方向反转、`Find` 返回哨兵）直接影响 `m_PoolRefs` 的查找，详见 [`risks-and-debt.md`](risks-and-debt.md)。

## `MemoryPool` 内部结构

| 结构 | 类型 | 说明 | 位置 |
|---|---|---|---|
| `ResSegment` | struct | chunk 内的一段内存：`base/size/chunkBase/tag`、双向 segment 链、双向 free 链、`isLeftMost/isRightMost/busy`、`lazyFreeCount` | [src/hal/m3d/memoryPool.h:50-86] |
| `MemoryRange` | struct | `std::map` 的键，区间不重叠语义 | [src/hal/m3d/memoryPool.h:88-96] |
| `m_FreeBuckets` | `ResSegment*[s_FreeTableLimit]` | 按 `Log2(size)` 分类的空闲段链 | [src/hal/m3d/memoryPool.h:146] |
| `m_EltMappingHash` | `DevSize` | 非空 bucket 位图 | [src/hal/m3d/memoryPool.h:148] |
| `m_pHeadSegment` | `ResSegment*` | 全部段的双向链表头 | [src/hal/m3d/memoryPool.h:150] |
| `m_SegmentTracker` | `MemSegTree` | busy 段精确索引 | [src/hal/m3d/memoryPool.h:152] |
| `m_ChunkAlignment`/`m_ChunkAllocSize` | `DevSize` | chunk 对齐与步长 | [src/hal/m3d/memoryPool.h:124-125] |
| `m_ReuseCountLimit`/`m_ReuseSizeLimit` | `DevSize` | lazy-free 复用阈值 | [src/hal/m3d/memoryPool.h:141-142] |

`ResSegment` 通过 `pNextSegment`/`pPrevSegment` 挂在 segment 双向链表，通过 `pNextFree`/`pPrevFree` 挂在 free 链，`busy` 标记该段是否被占用 [src/hal/m3d/memoryPool.h:50-69]。

池级统计字段 `m_FreeSize`、`m_TotalSize` 与水线值 `m_MemUsedSizeHigh`、`m_TotalSizeHigh` 位于成员区 [src/hal/m3d/memoryPool.h:128-132]。

## 常量

| 常量 | 值 | 位置 |
|---|---|---|
| `MemoryPool::s_DefaultChunkAllocSize` | 2 MiB | [src/hal/m3d/memoryPool.h:102] |
| `MemoryPool::s_DefaultLazyFreeThreshold` | `UINT64_MAX` | [src/hal/m3d/memoryPool.h:103] |
| `MemoryPool::s_FreeTableLimit` | `Device::s_MtgpuMaxGpuMmuBits`（48） | [src/hal/m3d/memoryPool.h:100]、[src/hal/m3d/device.h:84] |
| `Device::s_MtgpuMaxGpuMmuBits` | 48U | [src/hal/m3d/device.h:84] |
| `MemMgr::s_PropertyShift` | 32ULL | [src/hal/m3d/memMgr.h:40] |

## 所有权风险

- `m_PoolRefs.Delete(key, true)` 在 `DestroyPoolNoLock` 中释放 `m_Value` 指向的池对象 [src/hal/m3d/memMgr.cpp:208]，与析构批量清理 [src/hal/m3d/memMgr.cpp:25-33] 并存，两条删除路径需避免重复释放。
- `m_InternalPoolRefs` 以 `internalType` 直接作下标写 [src/hal/m3d/memMgr.cpp:186]、[src/hal/m3d/memMgr.cpp:206]，依赖 `InternalMemoryPoolType` 值域与 `std::array` 容量一致（见 risks-and-debt.md 风险 4）。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/hal/m3d/memoryPool.h`（15 处，例：[src/hal/m3d/memoryPool.h:98]）
- `src/hal/m3d/memMgr.h`（8 处，例：[src/hal/m3d/memMgr.h:123]）
- `src/hal/m3d/memMgr.cpp`（4 处，例：[src/hal/m3d/memMgr.cpp:208]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
