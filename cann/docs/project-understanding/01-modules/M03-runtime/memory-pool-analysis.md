# M03 Runtime：内存池源码级分析

- 对应源码版本：Runtime `dae460b78`
- 证据状态：关键用户态路径已确认；设备侧完成语义和未执行测试为“未验证”
- 范围：KernelMemoryPool、SOMA stream-ordered memory pool、普通设备/Host 内存三条路径

## 1. 结论先行

Runtime 中至少存在三种不能合并描述的“池/内存策略”：

1. **KernelMemoryPool**：Runtime 内部 Kernel/Program/Binary 使用的固定 2 MiB backing pool。它在主机侧维护空闲块链表，底层 backing 通过 `DevMemAlloc` 一次申请；它不是公开的 `rtMemPool_t`，没有 Stream-ordered free 语义。
2. **SOMA**：`rtMemPool*` 对应的用户可见流序内存池。Runtime 管理虚拟地址区间、Segment 状态、归属和统计，Driver/HAL 管理 backing 与设备侧内存池协议；异步分配/释放还会向 `SomaMemMng` 提交 AICPU 操作。
3. **普通内存路径**：`rtMalloc`、`rtMallocHost`、host register 等接口直接按内存类型、页策略、设备运行模式和 feature flag 选择 Driver 分支，不经过前两种池。

最重要的当前实现事实：

- `SegmentManager::TryToReuse` 当前直接返回 `nullptr`，所以虽然 `SingleStreamReuse`、`StreamEventReuse` 和 `StreamInternalReuse` 辅助实现及依赖元数据存在，`SegmentAlloc` 当前实际会继续走 `AllocFromFreeSegs`；不能把这些策略写成已启用行为 `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:140-147,150-195]`。
- SOMA 的 `freeSegs_` 和 `cachedSegs_` 都按 `(size, basePtr)` 升序排列。`AllocFromFreeSegs` 对请求构造同样大小的临时 Segment 并调用 `freeSegs_.lower_bound`，因此选择的是**最小的满足 `segment.size >= request` 的 Segment**，即按行为属于 Best-Fit；相同大小时选择较低地址。三个 cached 重用辅助函数也从同一按 size 升序的 `cachedSegs_` 下界开始扫描，因此在其各自过滤条件内同样优先较小的可用 Segment `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:67-75,154-157]` `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:245-333,345-361]`。
- `SomaApi::MemPoolTrimImplicit` 当前是临时 no-op，直接返回成功；调用点存在不等于隐式 trim 已回收设备或本地缓存 `[runtime/src/runtime/feature/soma/soma.cc:328-333]`。

## 2. 三类路径对照

| 路径 | 用户可见句柄 | Runtime 元数据 | backing/外部副作用 | 释放语义 |
|---|---|---|---|---|
| KernelMemoryPool | 否 | `MemoryPoolManager` → `deque<MemoryPool*>` → `MemoryList` | 每个池一次 `DevMemAlloc(..., 2 MiB, RT_MEMORY_HBM)` | 归还空闲块；空闲池超过 5 个时删池 |
| SOMA | `rtMemPool_t`（实为 `SegmentManager*`） | `allocedMap_`、`cachedSegs_`、`freeSegs_`、双向 Segment 链、Registry ownership | 保留 VA、HAL pool、AICPU `SomaMemMng`、HAL async config/trim | 普通 free → CACHED；force/sync free → FREE；trim 可释放 backing |
| 普通设备内存 | `void*` | 主要由 Driver/设备侧维护 | `halMemAlloc` / `halMemFree`，按 flags 选择 HBM/DDR/P2P/huge 等 | 通常直接 Driver free；同步 wrapper 可先 device synchronize |

“池”这一词只表示复用或分配管理结构，不能据此推导三者共享地址空间、统计或生命周期。

## 3. KernelMemoryPool

### 3.1 创建与 backing

`MemoryPool::Init` 固定申请 `POOL_SIZE_2M`，创建 `MemoryList`，并把整个 backing 作为一个空闲块加入链表。backing 的 `MemoryPool::AllocDevMem` 使用 `RT_MEMORY_HBM`、`MODULEID_RUNTIME` 和 `DevMemAlloc`；设备不支持 Kernel data read-only feature 时才把池的 read-only 属性传入 Driver `[runtime/src/runtime/core/src/pool/memory_pool.cc:40-50,91-105]`。

`MemoryPoolManager::Init` 按初始数量创建 read-only pool；后续分配只接受 `0 < size <= 2 MiB`，按 read-only 属性遍历现有池，找不到时在写锁内追加一个池 `[runtime/src/runtime/core/src/pool/memory_pool_manager.cc:43-66,69-95]`。

### 3.2 分配算法与碎片

`MemoryList::GetBlock` 从链表头查找首个足够大的块；精确大小时移除该块，否则从块左侧切割并保留右半空闲块。释放时 `AddBlock` 直接把块追加到链表尾部。源码未显示按地址排序或相邻空闲块合并，因此这是“链表 first-fit + 释放尾插”，不是通用 coalescing allocator `[runtime/src/runtime/core/src/pool/memory_list.cc:14-122]`。

这意味着：

- 分配顺序受链表历史影响；
- 释放后相邻块不会由 `MemoryList` 自动合并（是否由其他调用约束避免问题，需额外验证）；
- 大于 2 MiB 的单次请求直接绕过该池，返回空指针给上层选择其他路径 `[runtime/src/runtime/core/src/pool/memory_pool_manager.cc:69-95]`。

### 3.3 释放、查找和并发

`Release` 和 `TryRelease` 在 `MemoryPoolManager` 写锁内遍历池；`TryRelease` 将 `Contains` 与 `Release` 保持在同一写锁范围，避免池被并发删除。`GetPoolMemInfo`、`Contains` 和地址/advise mutex 查询使用 shared lock `[runtime/src/runtime/core/src/pool/memory_pool_manager.cc:98-141,163-218]`。

当释放后空闲池数量大于 `maxFreePools_`（5）时，管理器从 deque 前端删除空闲池；`MemoryPool` 析构释放整块 backing 和 `MemoryList` `[runtime/src/runtime/core/src/pool/memory_pool_manager.cc:174-201]`。析构管理器持有 unique lock 后删除所有池 `[runtime/src/runtime/core/src/pool/memory_pool_manager.cc:26-41]`。

### 3.4 边界

KernelMemoryPool 的 `usedSize_` 和链表地址判断只服务 Runtime 内部池；它没有 `rtMemPoolSetAttr`、Stream/Event sequence 或 AICPU 配置。不能用 KernelMemoryPool 的“空闲块”解释 SOMA 的 `reserved`、`cached` 或设备侧 backing。

## 3.5 其他固定槽位/资源池（不属于 Best-Fit 变量大小分配）

Runtime 还使用 `BufferAllocator` 管理 SPM、SQ address、Event、copy buffer 等固定大小 item。它不是按空闲块大小选择，而是以 `Bitmap` 分配 item ID，再由 `pool_[poolIdx] + itemOffset * itemSize_` 计算地址；释放时通过 bitmap 释放 ID。因此这类池应称为 bitmap/slot allocator，而不是 First-Fit 或 Best-Fit。`BufferAllocator::LINEAR` 与 `EXPONENTIAL` 只控制池容量扩展（增加 `initCount_` 或按 2 倍扩展），不表示内存块选择算法 `[runtime/src/runtime/core/src/pool/buffer_allocator.hpp:23-54,81-125]` `[runtime/src/runtime/core/src/pool/buffer_allocator.cc:93-151,154-197]`。

例如 SQ address pool 按 32 KiB 至 2 MiB 的 item size 分成多个独立 allocator，并使用 `BufferAllocator::LINEAR`；SPM pool 也使用固定 item size 和 LINEAR 扩容 `[runtime/src/runtime/core/src/device/sq_addr_memory_pool.cc:101-127]` `[runtime/src/runtime/core/src/pool/spm_pool.cc:39-75]`。它们不能与 KernelMemoryPool 或 SOMA 的可变大小区间分配混为一谈。


### 4.1 SegmentManager

`SegmentManager` 保存：

- `allocedMap_`：基地址 → 当前 BUSY Segment；
- `cachedSegs_`：逻辑 free 但受 Stream/Event 依赖约束的缓存段；
- `freeSegs_`：立即可分配的 FREE 段；
- `tail_`、`base_`、`size_`：双向 Segment 链和池区间；
- `busySize_`、`reserveSize_`、两个 high-water mark；
- `deviceId_`、`graphId_`、`canDelete_`、`isIPCPool_` `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:122-171]`。

Segment 携带 `basePtr`、`size`、`prev/next`、`streamId`、`graphId`、`eventId`、`seqId` 和 `SegmentState`。初始状态是 FREE；分配后是 BUSY；普通异步 free 后是 CACHED；force free 后转 FREE `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:22-44]`。

### 4.2 分配

`SomaApi::AllocFromMemPool` 校验输出指针、pool handle 和 Stream ID，并按 `DEVICE_POOL_MIN_BLOCK_SIZE` 对齐；随后调用 `SegmentAlloc`，成功时返回 Segment 的虚拟地址 `[runtime/src/runtime/feature/soma/soma.cc:235-255]`。

`SegmentAlloc` 在 mutex 内先调用 `TryToReuse`。当前该函数无条件返回空指针，因此实际分配路径是：

```text
校验 SegmentManager / IPC pool
  -> lock SegmentManager
  -> TryToReuse（当前 nullptr）
  -> AllocFromFreeSegs
  -> freeSegs_.lower_bound(request)
  -> 选择最小的满足 size >= request 的 FREE Segment（Best-Fit）
  -> 从该 Segment 左侧切割
  -> 标记 BUSY、写 allocedMap_、记录 streamId
  -> 更新 reserveSize_ / busySize_ / high-water mark
```

`freeSegs_` 是 `std::set<Segment*, SegmentComparator>`，比较器先按 Segment `size` 升序、再按 `basePtr` 升序；`AllocFromFreeSegs` 用请求大小构造临时 Segment 并调用 `lower_bound`，所以这里不是链表 First-Fit，而是按大小排序的 Best-Fit（同尺寸按地址作稳定的次级选择） `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:67-75,154-157]` `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:345-361]`。对应完整控制流见 `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:150-195]`。

### 4.3 释放和合并

`SegmentFree` 在 mutex 内从 `allocedMap_` 查找并删除，递减 `busySize_`。force free 直接标记 FREE 并并入 `freeSegs_`；普通 free 用当前 Stream sequence 写入 `seqId`，标记 CACHED 并尝试与满足相同 stream/graph/event/seq 规则的邻接 cached 段合并 `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:198-244,336-344]`。

因此“free 返回成功”只表示 Runtime 元数据已接受释放请求；对于普通异步 free，物理 backing 和设备侧配置仍由后续异步协议/Driver 处理。设备侧完成时间不能从该函数的同步返回值推出。

### 4.4 依赖策略的实现状态

头文件声明了 `singleDependencies`、`eventDependencies`、`internalDependencies`、`opportunistic` 和 watermark；也存在：

- `SingleStreamReuse`：从按 `(size, basePtr)` 升序排列的 cached Segment 中，从请求大小的 lower bound 开始寻找同 Stream 且大小足够的 Segment；
- `StreamEventReuse`：从同一大小下界开始，依据 Registry sequence map 判断 Event 依赖已满足；
- `StreamInternalReuse`：从同一大小下界开始寻找其他 Stream 的 cached Segment `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:67-75,107-120,140-143]` `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:245-334]`。

因此，这些候选函数如果被 `TryToReuse` 调用，会按大小选择最小满足条件的候选，即各自过滤条件下的 Best-Fit；但是调用它们的 `TryToReuse` 当前直接 `return nullptr`。准确表述是：**SOMA 的 FREE Segment 实际分配路径已经实现按大小 Best-Fit；cached Segment 的 Best-Fit 候选函数存在，但当前统一选择入口未启用这些依赖重用算法。**

## 5. SOMA 创建、异步 ABI 与销毁

### 5.1 创建

`rtMemPoolCreate` 经过 C API、`ApiImplSoma` 和 `SomaApi` 后，执行设备号转换、查询 HBM 总量/空闲量、获取 allocation granularity；`maxSize == 0` 时按总量向下对齐，指定值则向上对齐，超过总量失败。随后创建 `SegmentManager`、调用 Driver `StreamMemPoolCreate`、建立初始 VA Segment，并把 manager 注册到 `PoolRegistry` `[runtime/src/runtime/feature/soma/soma.cc:31-121]`。

Runtime Standard SoC Driver 在创建时先保留 VA，构造 `soma_mem_pool_t{poolId, deviceId}` 和 HBM/huge-page/ACL module 属性，再调用 weak HAL `halMemPoolCreate`；HAL 失败会释放已保留 VA `[runtime/src/runtime/driver/npu_driver_standard_soc.cc:658-699]`。Driver V3 HAL 随后分配 global VA，并通过 `mem_pool_client_create` 建立远端 pool client `[driver/src/ascend_hal/svm/v3/api/master/svm_soma.c:531-552]`。

### 5.2 异步 AICPU 配置

`AicpuPoolCtxArgs` 的 ABI 字段为 `size`、`va`、`mempoolId`、`deviceId`、`memAsyncOpType`、`memAsyncSubCMD`；操作类型为 MALLOC/FREE，子命令用于区分策略 `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:87-105]`。

Runtime 异步 malloc 先在本地 SegmentManager 分配，再组织参数并通过 `StreamLaunchCpuKernel("SomaMemMng", ...)` 提交。启动失败时尝试 `FreeToMemPool`；该补偿调用默认不是 force free，可能再次进入 CACHED，而不是必然回到 FREE。该失败路径是否还需要回滚设备配置、backing 或 sequence 状态，需专项测试 `[runtime/src/runtime/api/impl/api_impl_soma.cc:64-109,197-267]`。

异步 free 对 SOMA 指针先定位 pool 和 allocation size，调用 `StreamMemPoolAsyncConfig(..., true)`，再把本地段放入 cached 并提交 FREE 操作；非 SOMA 指针则注册 HostFunc，回调执行 `DevFreeStatic` `[runtime/src/runtime/api/impl/api_impl_soma.cc:111-195]`。

Driver V3 的 `halMemPoolAsyncConfig` 校验 pool、地址、大小和 2 MiB 对齐，malloc 路径先增加 VMM segment 再发 `SVM_SOMA_MEM_ALLOC` ioctl；ioctl 失败会删除 segment。free 路径先删除 segment，再发 `SVM_SOMA_MEM_FREE`；源码明确标注失败时“不回滚”，所以 Driver 用户态本地状态可能已先改变 `[driver/src/ascend_hal/svm/v3/api/master/svm_soma.c:744-821,862-901,928-979]`。

### 5.3 trim 与隐式 trim

显式 `MemPoolTrimTo` 读取 busy/reserved，计算 free size，先调用 Driver trim，再调用本地 `SegmentManager::TrimTo`；本地 trim 裁剪 cached 段并并回 FREE 段 `[runtime/src/runtime/feature/soma/soma.cc:287-325]`。

相反，`MemPoolTrimImplicit` 是临时 workaround，忽略 `includeGraphPool` 并立即返回成功 `[runtime/src/runtime/feature/soma/soma.cc:328-333]`。所以“分配失败会隐式 trim 后重试”目前只能作为设计意图或调用点假设，不能作为当前实现事实。

### 5.4 销毁与 ownership

`PoolRegistry` 用 `call_once` 建立单例；用按基地址排序的 `entries_` 查找池，用 `poolOwnership_` 持有 `shared_ptr<SegmentManager>`。查询返回 shared pointer，可保证查询期间 manager 不会被立即析构 `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:180-230]`。

销毁时先从 Registry 取得 shared ownership，检查 `CanDelete`，再调用 Driver `StreamMemPoolDestroy`，成功后移除 Registry，最后 reset ownership 触发 manager 析构 `[runtime/src/runtime/feature/soma/soma.cc:223-233]`。使用中指针、未完成异步任务或设备侧拒绝销毁的实际语义不能仅由用户态静态代码确认。

## 6. 普通设备/Host 内存

### 6.1 `rtMalloc` → `DevMalloc`

C API 的 `rtMalloc` 经过全局状态等待和 `Api::Instance()` 后调用 `DevMalloc`，错误通过扩展错误码返回 `[runtime/src/runtime/api/api_c_memory.cc:63-78]`。`ApiImpl::DevMalloc`：

1. 要求当前 Context 有效；
2. 从 policy 高位解析 DDR/HBM/默认和 read-only；
3. 把 huge/normal/P2P/1G huge policy 映射到 Runtime memory type；
4. 解析 module/device config，并强制 config device 与当前 Context 设备一致；
5. 按 32-byte 对齐；
6. DVPP advise 走 `DevDvppMemAlloc`，其余走 `Driver::DevMemAlloc` `[runtime/src/runtime/api/impl/api_impl_memory.cc:765-842]`。

`NpuDriver::DevMemAlloc` 再按运行模式选择 online、offline 或 AICPU scheduler 路径 `[runtime/src/runtime/driver/npu_driver_mem.cc:1302-1326]`。online 默认/huge-first 对大于临界值的请求先尝试 huge managed，失败再回退 managed；huge-only 和 1G huge-only 则不采用同样的普通回退 `[runtime/src/runtime/driver/npu_driver_mem.cc:1026-1114]`。最终 Driver 构造 flags 后调用 `halMemAlloc`，失败映射为 Runtime 错误并记录 device、size、policy、module 和 flag `[runtime/src/runtime/driver/npu_driver_mem.cc:1194-1215]`。

### 6.2 Host 内存

`HostMalloc` 把大小按 32 bytes 对齐后调用 `HostMemAlloc`；带 config 的 `HostMallocWithCfg` 支持 module ID 和 VA flag，其他属性立即报无效值 `[runtime/src/runtime/api/impl/api_impl_memory.cc:47-130]`。`rtFreeHost` 通过当前 Context 或全局 NPU Driver 释放，并在非 PC 平台先检查指针 location 必须是 HOST/UNREGISTERED `[runtime/src/runtime/api/impl/api_impl_memory.cc:133-158]`。

Host register 在支持 Driver pin register 时直接下发；不支持时 Runtime 维护 mapped/pinned 软件表，并在 unregister 时分别清理。重复注册的 Driver BUSY 会映射为 `RT_ERROR_HOST_MEMORY_ALREADY_REGISTERED` `[runtime/src/runtime/api/impl/api_impl_memory.cc:160-247]`。

### 6.3 同步和异步边界

`rtFreeWithDevSync` / `rtFreeHostWithDevSync` 显式先调用 `rtDeviceSynchronize`，成功后再 free `[runtime/src/runtime/api/api_c_memory.cc:173-192]`。这说明普通 `rtFree` 与“先确认设备任务完成再释放”是两个不同契约；不能把普通 free 的成功理解为所有异步消费者已经完成。

## 7. 错误、锁与观测矩阵

| 场景 | 静态确认行为 | 仍需验证 |
|---|---|---|
| Kernel pool 请求 >2 MiB | manager 不从该池分配 | 上层是否可靠切换到其他 allocator |
| Kernel pool backing 申请失败 | Init/新增池失败并清理对象 | 设备内存压力下的重试策略 |
| SOMA pool 内地址越界 | Segment/Driver 参数校验拒绝 | 跨进程或多后端差异 |
| SOMA 普通 free | 本地 BUSY→CACHED | 设备任务完成与 backing 释放时刻 |
| SOMA AICPU launch 失败 | 尝试本地 free，默认可能 cached | 本地/设备状态是否完全回滚 |
| Driver V3 SOMA free ioctl 失败 | 删除 segment 后“不回滚” | 后续重试和泄漏处理 |
| 显式 trim | Driver 成功后本地 TrimTo | Driver 返回目标大小和真实物理回收 |
| 隐式 trim | 当前直接成功 no-op | 后续版本恢复实现的行为 |
| 普通 malloc huge-first | online 大块先 huge，失败回退 managed | 各 SoC 的临界值和 Driver 行为 |
| 普通 free | 直接 HAL free | 未完成任务访问已释放地址的设备结果 |

建议日志至少关联 API、device ID、Context/Stream、pool ID、VA、size、Segment state、原始 `drvError_t` 和映射后的 `rtError_t`；当前代码已经在多个层面记录其中一部分，但尚未形成统一字段规范。

## 8. 验证矩阵与变更影响

### 无设备可执行的静态检查（未执行）

- Markdown 链接和源码引用存在性；
- 头文件布局、枚举、函数签名和 CMake target 检查；
- Runtime 单元测试的 mock Driver 覆盖：空 pool、越界、重复 free、trim 失败、AICPU launch failure；
- TSAN/ASAN 检查 Registry shared ownership、SegmentManager mutex 和 KernelMemoryPool shared mutex。

### 设备依赖验证（未验证）

- `rtMemPoolCreate` 在不同 SoC、run mode 和 granularity 下的实际值；
- AICPU `SomaMemMng` 的提交/完成顺序及设备侧拒绝；
- stream/event dependency 是否会在后续版本或编译配置中启用；
- Driver trim 的物理 backing 回收和 pool destroy 的 busy 资源语义；
- 普通 malloc flags 在 HBM、DDR、P2P、huge/1G huge 分支的实际映射。

### 变更影响

修改公开 SOMA 属性、AICPU 参数或 HAL `soma_mem_pool_*` ABI 时，必须同时检查 Runtime C API、ApiImpl、SegmentManager、PoolRegistry、NpuDriver、Driver HAL、SDK-driver/ioctl、ACL/GE allocator 以及对应测试。修改 KernelMemoryPool 只看 `rtMemPool*` 测试是不充分的；修改普通 `rtMalloc` policy 也不能假定会影响 SOMA。

## 9. 证据边界

本文将源码控制流标为“已确认”，将跨仓未完整追踪、设备侧行为和未执行构建/测试标为“推断/未验证”。特别是 `TryToReuse` 和 `MemPoolTrimImplicit` 的当前 no-op 事实优先于旧文档中的抽象策略描述。
