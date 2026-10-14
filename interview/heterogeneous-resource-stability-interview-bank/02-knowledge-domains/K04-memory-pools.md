# K04：设备/主机内存、地址空间与内存池

## 边界

本知识域处理内存的物理与逻辑归属：设备内存与主机内存在不同地址域和一致性域中的分配、映射、属性、池化和释放。它回答“一块地址从哪来、属于哪一层、什么属性、何时可用、何时可回收”。

本域解决：

1. 设备/主机内存的地址域差异与 pin/map 语义；
2. 普通分配的属性解析路径（`rtMalloc` → HAL flags）；
3. 多层内存池的结构与选择策略（KernelMemoryPool、SOMA、Driver V2/V3 cache、GE、SHMEM）；
4. 别名、复用与释放顺序导致的故障。

本域不负责：

- 提交后的完成语义（K05）；
- 传输代价与拓扑（K07）；
- 容量规划与成本模型（K11）；
- 错误恢复流程（K09）。

与相邻域的接口：K03 决定内存对象的生命周期边界；K05 决定何时可以安全复用/释放；K07 消费地址域和属性做传输路径选择；K11 在本域的分层水位基础上做容量与碎片权衡。

## 核心机制

### 1. 设备内存与主机内存的地址域差异

设备内存（HBM/DDR）和主机内存位于不同的物理介质和地址域：设备通过自己的地址空间访问，主机通过 CPU 页表访问，两者之间要建立映射才能互相看到对方的内存。

关键路径：

- `rtMalloc` 先由 `ApiImpl::DevMalloc` 解析 memory type、policy、advise、module/device config，并按 32-byte 对齐；DVPP 走专用 Driver 分支，普通请求进入 `NpuDriver::DevMemAlloc`。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:80`。
- Driver 按 online/offline/AICPU-scheduler 选择路径，并把页类型、HBM/DDR/P2P、节点和 module ID 编进 HAL flags，最终调用 `halMemAlloc`。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:80`。
- Host allocation 同样按 32-byte 对齐；host register 还要在 pin/map 能力不足时维护 Runtime 软件表；host free/unregister 的顺序必须与 location、map、pin 状态相符。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:82`。

对比表：

| 维度 | 设备内存 | 主机内存 |
|---|---|---|
| 地址域 | 设备独立地址空间 | CPU 地址空间 |
| DMA 前提 | 映射到设备地址 | 常需 pin + 映射 |
| 分配入口 | `rtMalloc` → `DevMemAlloc` → `halMemAlloc` | `rtMallocHost` / mmap + register |
| 对齐 | 32-byte（文档记录） | 32-byte（文档记录） |
| 关键属性 | memory type、页类型、HBM/DDR/P2P、node、module | location、pin、map |
| 释放前置 | 完成同步、属性一致 | unregister 状态一致 |

### 2. 普通分配路径与属性解析

```text
rtMalloc
  -> ApiImpl::DevMalloc
  -> 当前 Context / policy + type + module/device config 校验
  -> 32-byte 对齐
  -> DevDvppMemAlloc（DVPP advise）或 NpuDriver::DevMemAlloc
  -> online/offline/AICPU-scheduler 分支
  -> huge/normal/1G huge policy
  -> halMemAlloc(dptr, size, drvFlag)
```

[已确认] 来源：`cann/01-modules/M03-runtime/call-chains.md:73-86`、`cann/01-modules/M03-runtime/memory-pool-analysis.md:155-168`。

online 大块请求先尝试 huge managed，失败再回退 managed；huge-only 和 1G huge-only 不采用同样的普通回退。[已确认] 来源：`cann/01-modules/M03-runtime/memory-pool-analysis.md:168`。最终 Driver 构造 flags 后调用 `halMemAlloc`，失败映射为 Runtime 错误并记录 device、size、policy、module 和 flag。

### 3. 多层池化：五类池，五种语义

CANN 不是一条单一 allocator 链，而是多种不同粒度的资源管理叠加。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:12-17`。

#### 3.1 KernelMemoryPool（Runtime 内部固定 backing 池）

`MemoryPool::Init` 从 Driver 一次申请 2 MiB HBM backing，创建 `MemoryList` 并登记整块空闲区。`Allocate` 从链表取满足大小的块，必要时只向前移动地址和减少剩余 size；`Release` 把释放块重新挂回链表并更新 `usedSize_`。析构时先调用 Driver `DevMemFree` 再删除 `MemoryList`。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:37`。

`MemoryList::GetBlock` 是 first-fit，整块命中时删除节点，部分命中时切割前缀；源码未显示释放相邻块的合并，因此碎片会随请求顺序累积。空闲池超过 5 个时管理器释放整池。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:39`、`cann/01-modules/M03-runtime/memory-pool-analysis.md:49-57`。

超过 2 MiB 或不满足条件的请求进入普通 Runtime→Driver/HAL 路径；不能用固定池的 `usedSize_` 推断全局 HBM 可用量。[已确认]

#### 3.2 SOMA（stream-ordered segment pool）

SOMA 创建产生三层资源：Runtime `SegmentManager` + Registry ownership、Driver/HAL pool 与 VA/backing、异步 malloc/free 使用的 `SomaMemMng` AICPU 命令。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:68-72`。

`Segment` 记录 base/size、prev/next、streamId、graphId、eventId、seqId 和 `FREE/CACHED/BUSY` 状态；`SegmentManager` 维护 `allocedMap_`、按 size 排序的 `cachedSegs_` 和 `freeSegs_`，并记录 busy/reserved/high-watermark。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:55`。

分配路径：

```text
校验 SegmentManager / IPC pool
  -> 从 freeSegs_.lower_bound(request) 取最小满足块（Best-Fit, 同尺寸取低地址）
  -> 左切割
  -> 标记 BUSY、写 allocedMap_、记录 streamId
  -> 更新 reserveSize_ / busySize_ / high-water mark
```

[已确认] 来源：`cann/01-modules/M03-runtime/memory-pool-analysis.md:95-107`。

释放路径：`SegmentFree(forceFree=false)` 默认进入 CACHED，按 stream sequence 记录依赖并与相邻同状态/同 stream/同 graph/event/seq 的 segment 合并；`forceFree=true` 才直接进入 FREE 集合。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:61`。

当前实现事实：`TryToReuse` 直接返回 `nullptr`，所以 cached 段的候选重用算法虽存在，当前控制流仍走 `AllocFromFreeSegs`；`MemPoolTrimImplicit` 是 no-op 直接返回成功。[已确认] 来源：`cann/01-modules/M03-runtime/memory-pool-analysis.md:27-29`。这意味着“cached 段会被跨 stream 复用”不能写成已启用行为。

#### 3.3 Driver 普通内存 cache（V2/V3）

普通 `rtMalloc` 经 Runtime policy 和 `halMemAlloc` 后可能进入 Driver ordinary cache。产品构建决定后端：`ascend910B`/`ascend910_93` 使用 V2 heap + 多棵树，`ascend950` 使用 V3 cache allocator + range/area。[已确认] 来源：`cann/90-cross-module/memory-and-resource-lifecycle.md:86`。

```text
V2：heap -> alloced/idle_va/idle_size/idle_mapped 多棵树 -> heap operation
V3：cache_allocator -> ga_inst -> 多个 ga_range -> ga_area
```

- V3 `cache_allocator` 按 `(devid, flag)` 建全局表；`ga_area` 同时挂在 range 地址树和全局 size tree，分配用 exact/upper-bound，释放时按地址找左右邻居并合并，只有整个 range 完全空闲才 shrink。[已确认] 来源：`cann/01-modules/M04-driver/driver-memory-pool-analysis.md:145-295,519-660`。
- V2 小请求优先复用已映射 cache node，其次 idle size node，最后新建 heap；同样采用 exact/upper-bound，只有完整 mapped node（`total == size`）才适合作为独立 shrink 单位。[已确认] 来源：`cann/01-modules/M04-driver/driver-memory-pool-analysis.md:744-960`。
- 底层返回 `DRV_ERROR_BUSY` 时进入延迟 recycle，而不是立即丢弃。[已确认] 来源：`cann/01-modules/M04-driver/driver-memory-pool-analysis.md:662-690`。

#### 3.4 BufferAllocator（固定槽位池）

`BufferAllocator` 用 Bitmap 保存 item id 到连续 backing 数组的占用状态。`LINEAR` 按 `initCount` 增长，`EXPONENTIAL` 按 2 倍增长；策略只决定扩容节奏，不是地址区间的 best-fit。`FreeById/FreeByItem` 归还 bitmap id。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:45-49`。

这类池是 ID/对象槽复用，而不是动态字节内存回收，属于 K06 的固定槽资源。

#### 3.5 GE 编译期 offset 复用与 SHMEM

- GE `MemReuseStrategy` 根据 thread scope、stream id、引用节点和跨 stream 拓扑分析是否可复用；跨 stream 的复用必须由 event/internal dependency 证明，不能只按 tensor size 判断。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:83`。
- SHMEM VMM 实现按 `rankCnt * aligned segment size` 保留全局 VA，每个 PE 获得固定窗口；`AllocLocalMemory` 调用 `HalMemCreate`，创建 `MemSlice`、导出 fabric handle 并 `HalMemMap` 到本地 VA；释放按 Unmap → HalMemRelease → erase slice 顺序。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:91`。

### 4. 属性一致性与 alloc/free key

一个可核对的不变量：属性必须端到端一致。分配时写入 HAL flags 的页类型、节点和 module，必须在释放时以同样的键回收，否则会落到错误的 cache 或 heap。

```text
alloc_key = (device, memory_type, policy, page_type, node, module, align)
free_key  == alloc_key   // 否则可能无法命中同一 backing
```

### 5. 碎片行为对比

| 池 | 粒度 | 复用特征 |
|---|---|---|
| KernelMemoryPool | 2 MiB backing 上的小块 | first-fit，未见邻接合并，碎片随请求顺序累积 |
| SOMA FREE Segment | stream-ordered Segment | Best-Fit（`lower_bound`），cached 重用入口当前 no-op |
| Driver V3 area | range 内空闲区间 | exact/upper-bound，相邻 area 合并，完整 idle range 才 shrink |
| Driver V2 node | heap 内多棵树 | exact/upper-bound，小请求先 mapped cache，完整 mapped node 才 shrink |

“池”不是同义词；选错层会得到不同的碎片和复用行为。

### 6. 别名、复用与释放顺序

别名和复用本质是“同一块内存被多个逻辑对象或时间点共享”：

- **空间别名**：多个句柄/offset 指向同一 backing，一处写入影响另一处；
- **时间复用**：同一地址在不同时刻给不同用途，必须由依赖（stream/event）证明前者已不再使用。

复用条件：

```text
reuse(prev, next) allowed iff
    (same stream and ordered) or
    (event/internal dependency proves prev completed before next starts)
```

释放前置：

```text
free(buf) only when pending_tasks(buf) == 0 and refs(buf) == 0
```

SOMA 的 `SegmentFree` 默认 CACHED 并与邻接同状态段合并；`forceFree=true` 才进 FREE 集合。[已确认] 来源：`cann/90-cross-module/pooling-and-resource-management.md:61,57`。

## 常见失效条件

- **把总显存当可分配量。** 碎片、对齐、属性和其他租户会改变实际可用量。
- **跨层外推。** 用固定池 `usedSize_` 代表全局 HBM，或用 SOMA 的 busy/reserved 代表 Driver cache。
- **alloc/free key 不一致。** 页类型、节点或 module 不同导致回收落到错误路径。
- **跨 stream 按大小复用。** 无 event/依赖证明时写覆盖。
- **释放后继续提交使用。** 悬空访问，错误随机难查。
- **忽略 pin/map 状态。** host unregister 顺序错误留下映射或 pin 泄漏。
- **把 `DRV_ERROR_OUT_OF_MEMORY` 当设备物理内存耗尽。** V3 首次 `svm_ga_alloc` 的该错误可能表示 cache range 无适配 area，随后会尝试扩展。[已确认] 来源：`cann/00-overview/global-error-model.md:27`。
- **把 cached 当作已启用复用。** 当前 `TryToReuse` 是 no-op，cached 段仍走 FREE 分配。
- **忽略切分/合并后的地址偏移。** 计算错位导致越界。

## 关键不变量

1. **属性端到端一致**：`free_key == alloc_key`。
2. **完成先于释放**：`pending_tasks == 0 ∧ refs == 0` 才可 free。
3. **复用需证明**：跨 stream 复用必须有 event/internal dependency。
4. **原地回收边界**：V3 只有 `idle_area_size == range->size` 的完整 idle range 才从 gen allocator 移除；V2 只有完整 mapped node 才 shrink。[已确认]
5. **层间不可外推**：不同层的池统计互不代表。
6. **BUSY 延迟回收**：底层 `DRV_ERROR_BUSY` 时进入 recycle，不当作成功也不丢弃。[已确认]
7. **SHMEM 释放顺序**：Unmap → HalMemRelease → erase slice。[已确认]

## 与其他知识域的连接

- ← **K01**：资源安全与正确性维度在此量化为分层水位与属性一致性。
- ← **K03**：内存对象被任务引用，释放受生命周期逆拓扑序约束。
- → **K05**：`pending_tasks == 0` 的判定依赖 K05 的完成证据。
- → **K06**：`BufferAllocator` 固定槽与 SOMA stream-ordered 语义参与并发与隔离。
- → **K07**：地址域、pin/map 和 HAL flags 属性直接决定传输路径。
- → **K08**：SHMEM 每 PE 窗口与 fabric handle 是集合通信内存的基础。
- → **K09**：V3 free ioctl “不回滚”形成必须测试的故障窗口。
- → **K11**：碎片率、shrink 阈值和 resident 内存来自本域的池行为。

## 题目索引

主域题目（`01-question-bank` 中 `知识域：K04`）：

- 入门：`HRS-B005`（设备/主机内存分配、地址空间与内存池差异）、`HRS-B014`（内存别名、复用和释放顺序故障）
- 进阶：`HRS-I004`（三条内存路径与地址空间/池映射）
- 高级：`HRS-A004`（内存池碎片量化与容量规划）
- 专家：`HRS-E004`（内存子系统的资源安全证明）

## 证据入口

- `cann/90-cross-module/memory-and-resource-lifecycle.md:58-101`：三条内存生命周期、属性解析、V3 ioctl 顺序。
- `cann/90-cross-module/pooling-and-resource-management.md:35-103`：五类池结构、所有权矩阵、失败路径。
- `cann/01-modules/M03-runtime/memory-pool-analysis.md:17-73,155-195`：KernelMemoryPool、SOMA、普通内存与诊断矩阵。
- `cann/01-modules/M04-driver/driver-memory-pool-analysis.md:17-53,145-295,519-690,744-993`：V2/V3 cache 结构、切分/合并/shrink/recycle。
- `cann/01-modules/M03-runtime/data-structures.md:30-53`：SOMA 字段、内存池选择矩阵。
- `cann/00-overview/global-error-model.md:25-27`：Driver cache 错误边界。
- `cann/90-cross-module/performance-critical-paths.md:25-33`：碎片与 SOMA 潜在开销。

## 学习提示

不要背“显存 vs 内存”，要背三个维度：地址域、分配属性、池化层次。读每个池时问四个问题：粒度多大、用什么算法选块、何时合并、何时真正归还 backing。做容量判断时永远先问“这是哪一层的哪个属性的水位”，再用 K05 的完成边界决定何时释放。
