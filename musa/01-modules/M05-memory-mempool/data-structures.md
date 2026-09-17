# M05：MemoryPool 数据结构与所有权

## 1. Core `Musa::MemoryPool`

`src/musa/core/memoryPool.h:85-152` 的关键字段：

| 字段 | 类型/含义 | 生命周期/并发 |
|---|---|---|
| `m_pHalPool` | 对应 HAL `IMemoryPool*` | Core pool 析构时 release/destroy |
| `m_pDevice` | 所属 Core device | 不拥有 device |
| `m_pStream` | mapping/paging 使用的 stream | `AsyncMemAlloc` 会写入；pool 级字段 |
| `m_ReuseAllowance` | reuse 总开关字段 | 当前已读实现未见参与分配决策 |
| `m_MinBytesToKeep` | Core 字段 | 当前已读实现未见参与 trim |
| 三个 `m_DisableReuse*` | API reuse attribute 的反值 | Set/Get 可见，具体复用策略尚未接入证据 |
| `m_LocationAccessMap` | device id → access flags | 独立 mutex；影响新/现有 virtual allocation 的 paging |
| `m_MemoryAllocations` | `unordered_set<Memory*>` | 记录该 Core pool 的 virtual Memory；独立 mutex |
| `m_RequestedBytes` | 当前 logical bytes | 按 `Memory::GetSize()` 统计，不是 HAL reserved bytes |
| `m_RequestedBytesHigh` | logical high watermark | 创建时更新，attribute 置零时重置 |
| `m_Type` | Default/Graph/Internal | graph allocation 判定依赖它 |
| `m_IpcMemPoolData` | POSIX shared-memory IPC 元数据 | export/import 另有 mutex/once flag |

Core pool 不保存 free segment。真正的空闲区间和 chunk 由 `m_pHalPool` 指向的 HAL pool 保存。[`src/musa/core/memoryPool.h:121-149`]

## 2. HAL `Hal::M3d::MemoryPool`

### 2.1 `ResSegment`

```text
ResSegment
  pChunkMem       -> 所属真实 HAL chunk
  lazyFreeCount   -> 完整 chunk 被归还的次数
  pPrev/pNextSegment -> chunk/segment 空间链表
  pPrev/pNextFree    -> size bucket 空闲链
  isLeftMost/isRightMost -> 是否覆盖 chunk 左/右边界
  busy            -> 是否已从 free list 取出
  base,size      -> 当前 segment 地址和长度
  chunkBase       -> 所属 chunk 起始地址
  tag             -> pool key
```

字段定义见 [`src/hal/m3d/memoryPool.h:48-85`]。同一个 chunk 的 segment 同时存在于：

- 一个空间顺序的 segment 链表；
- 空闲时的一个 size bucket 链；
- virtual pool 的 busy interval map（只对占用 segment插入）。

### 2.2 Pool 容器

```text
m_FreeBuckets[s_FreeTableLimit] : ResSegment*
  └─ 每个 bucket 是 pNextFree/pPrevFree 双向链
m_EltMappingHash : uint64_t
  └─ 第 index 位表示 m_FreeBuckets[index] 非空
m_pHeadSegment : ResSegment*
  └─ 所有 segment 的 pNextSegment/pPrevSegment 链
m_SegmentTracker : std::map<MemoryRange, ResSegment*>
  └─ virtual property 下索引 busy allocation
```

`MemoryRange` 使用 `[base, base+size-1]` 的闭区间端点并通过 `operator<` 实现不重叠区间排序。[`src/hal/m3d/memoryPool.h:87-98`]

### 2.3 水位与策略

- `m_ChunkAlignment`：从对应 heap 的 `largestPageSize` 获取。
- `m_ChunkAllocSize`：chunk 增长量下限/量化值，由 createInfo 传入。
- `m_FreeSize`：当前所有 free segment 的字节总和。
- `m_TotalSize`：所有 chunk 总字节数。
- `m_MemUsedSizeHigh`：`Total-Free` 的峰值。
- `m_TotalSizeHigh`：reserved chunk 总量峰值。
- `m_Policy` 默认构造为 `{fast, assuredFit, false}`。[`src/hal/m3d/memoryPool.cpp:42-61`]

## 3. MemMgr registry

`Hal::M3d::MemMgr` 保存三种引用：

```text
m_InternalPoolRefs[array<list<MemoryPool*>>]
  -> internal pool；按 InternalMemoryPoolType 分组
m_PoolRefs[SplayTree<Key, MemoryPool*>]
  -> 非 user-managed 的自动 pool；Key = MakeKey(poolInfo)
m_UserPools[list<MemoryPool*>]
  -> user-managed pool；供 Validate/UpdateUserPools
```

定义见 [`src/hal/m3d/memMgr.h:37-39,121-129`]。`CreatePoolNoLock` 的注册规则是：user-managed 放入 `m_UserPools`（由 `CreateUserPool` 完成）；非 user-managed 的 internal 放入 internal list，其他放入 splay tree。[`src/hal/m3d/memMgr.cpp:163-197`]

## 4. Pool key

`MakeKey` 将下列字段压进一个 `DevSize`：

```text
property       -> low 32 bits（但先与 s_PropertyMask）
viewCapability -> bits 32..37
allocation type-> bit 38
heap           -> bits 39..41
numaId         -> bits 42..45（低 4 bit）
```

源码为 [`src/hal/m3d/memMgr.h:41-115`]。因此：

- key 是资源兼容性分类，不包含请求 size/alignment；同一 pool 可服务多个大小请求。
- pool property 中不在 `s_PropertyMask` 的位不会区分 key。
- NUMA ID 只保留低 4 位；这是当前实现的编码事实，不代表 NUMA 总数被限制为 16（调用端仍可能另行限制）。
- `MemMgr::Allocate` 使用 key 查找，并再次比较 type/heap/property/view capability；该显式比较没有单列 numaId，但查找 key 已包含 numaId。[`src/hal/m3d/memMgr.cpp:98-133`]

## 5. Core Memory 与 pool 的所有权

```text
Core Context/Platform MemoryTracker
       owns shared_ptr<Memory>
                 │
                 ├─ Memory::m_pHalMemory -> HAL chunk/physical memory
                 └─ Memory::m_pPool -> Core pool（仅 pool allocation）
                                      │
                                      └─ m_pHalPool -> HAL pool
```

- Core `MemoryPool::CreateMemory` 建立 `shared_ptr<Memory>`，交给 `MemoryTracker`，并把 raw pointer 放入 `m_MemoryAllocations`。[`src/musa/core/memoryPool.cpp:380-410`]
- `DestroyMemory` 从 set 和 tracker 中移除；shared ownership 消失后，`Memory::~Memory` 才向 HAL pool `Free`。[`src/musa/core/memoryPool.cpp:413-427`、`src/musa/core/memory.cpp:360-379`]
- internal allocation 例外：`Device::AllocateInternalMem` 直接 new Core `Memory` 并 `InitPrealloc`；`FreeInternalMem` 先 delete，再调用 HAL pool free。[`src/musa/core/device.cpp:1110-1152`]
- graph allocation 的 `AllocParameter` 持有 virtual memory shared pointer；graph resource 析构时显式 `DestroyPhysMemories` 和 `pPool->DestroyMemory`。[`src/musa/core/graph.cpp:22-29`]

## 7. IPC pool metadata 与 SplayTree registry 的边界

### 7.1 IPC pool metadata

Core pool 的 IPC handle 不是可在接收进程直接复用的 HAL pool 指针，而是 POSIX shared-memory metadata 的 ownership wrapper：

- export 侧通过 `mkstemp`、`shm_open`、`ftruncate`、`mmap` 建立共享元数据，设置 `owners = 1`，再 `dup` fd 返回。[`src/musa/core/memoryPool.cpp:512-573`]
- import 侧只 `dup`/`mmap` 传入 fd，递增共享 `owners` 并设置 `m_IsImported`；没有设置 `m_pHalPool`。[`src/musa/core/memoryPool.cpp:473-510`]
- 因此 imported wrapper 可作为句柄查询/销毁，但当前 driver 明确拒绝对它设置属性或从它执行异步 pool allocation。[`src/driver/mu_mempool.cpp:153-169,349-390`]
- 析构时会递减 `owners` 并 `munmap`；源码的 `close(fd)` 位于 `m_IpcName` 非空分支内，而 imported wrapper 没有设置该名称，不能笼统地描述为所有 import 路径都会关闭 dup fd。[`src/musa/core/memoryPool.cpp:575-609`]

`mmap` 失败值是 `MAP_FAILED`，源码却与 `nullptr` 比较；这是静态审计风险，尚未通过故障注入复现。跨进程 `owners` 也只由进程内 mutex 包围，不能据此证明跨进程原子性。

### 7.2 SplayTree registry

自动 pool registry 使用 `Util::SplayTree<Key, MemoryPool*>`：

- `Get` 会 splay 并返回根节点，未命中时可能是最近节点；`MemMgr::Allocate` 因此还要做属性兼容性比较。[`src/util/utilSplayTree.h:61-100`、`src/hal/m3d/memMgr.cpp:116-133`]
- `Insert` 遇到已有 key 不替换 value；自动 registry 的 key 不能对应两个 pool。[`src/util/utilSplayTree.h:144-179`]
- `Delete(key, true)` 会同时释放节点 value；`MemMgr` 的自动 pool teardown 依赖这一 ownership 语义。[`src/util/utilSplayTree.h:182-212`、`src/hal/m3d/memMgr.cpp:20-34`]
- `Search` 的比较分支方向与通常升序 BST 直觉相反；这只是静态实现风险，真实树形下的 `FindPool` 行为仍需单测验证。[`src/util/utilSplayTree.h:215-226`]

## 8. 两套统计不可混淆

| API 观察 | 来源 | 代表什么 |
|---|---|---|
| `USED_MEM_CURRENT/HIGH` | `m_RequestedBytes` | Core virtual allocation 的逻辑请求字节 |
| `RESERVED_MEM_CURRENT/HIGH` | HAL `m_TotalSize/m_TotalSizeHigh` | 已向下层取得的 chunk 总量 |
| HAL free size | `m_FreeSize` | chunk 内可再次切分的空闲字节 |
| segment size | `ResSegment::size` | 某个连续区间，可能包含对齐 padding 后的实际范围 |

这几类数字在大 alignment、chunk 保留和 graph physical backing 延迟存在时可以明显不同。
