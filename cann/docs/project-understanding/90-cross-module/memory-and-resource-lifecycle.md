# 跨模块：内存与资源生命周期

- 对应源码版本：GE `47020afc8`、ACL `d26308d3`、Runtime `dae460b78`、Driver `6e2914c`
- 证据状态：Runtime 用户态分配、Context/Stream 生命周期和 SOMA HAL 边界已确认；GE/ACL 具体 allocator 实现、设备完成语义和真实硬件行为未验证

## 1. 总体模型

```text
ACL/GE 输入、模型、workspace
  -> Runtime Context / Stream / Memory / Model
  -> Runtime 元数据 + Driver/HAL backing
  -> Queue/HDC/SVM/设备任务
  -> callback/event/synchronize 完成边界
  -> 分阶段释放 Runtime 对象、物理内存和 Driver session
```

核心规则是：**主机 API 返回成功、Runtime 元数据改变、Driver 接受请求和设备任务完成是四个不同时刻。** 尤其是普通异步任务、SOMA AICPU 操作和模型执行，不应将提交成功直接解释为设备已消费完 buffer。

## 2. 所有权分层

| 对象 | 创建/登记 | 使用者 | 释放/最终销毁 | 关键边界 |
|---|---|---|---|---|
| Runtime Context | `ContextCreate` 或 PrimaryContextRetain | 当前线程、Streams、Models | TearDown；非 Primary 且引用为 0 时 delete | Primary Context 不能显式 destroy |
| Stream | Context Setup 或 StreamFactory | Context、任务、SOMA sequence | 先 flush/teardown，再 delete 或延迟重试 | 未完成任务、owner stream 失败时恢复 |
| SOMA manager | `rtMemPoolCreate` + PoolRegistry ownership | `rtMemPool*`、地址查找、AICPU 参数 | Driver destroy 成功后 Registry remove + shared ownership reset | 查询 shared_ptr 延长 manager 生命周期 |
| SOMA Segment | 初始 pool segment / split | `allocedMap_`、free/cached sets | BUSY→CACHED 或 FREE；manager 析构删除 | CACHED 不等于当前重用已启用 |
| 普通 device memory | `rtMalloc` → HAL `halMemAlloc` | ACL/GE/Runtime task | `halMemFree` | flags、run mode、页策略影响 backing |
| Host memory | `rtMallocHost` / mmap + register | Host、设备 DMA | unregister → munmap/close 或 HostMemFree | Host pointer location 和 pin/map 状态必须一致 |
| Model/Kernel | Context/Program/Executor | Stream/task/model executor | teardown 后 delete | 设备仍运行时不得提前销毁 |

## 3. Context → Stream → Memory 顺序

### 3.1 创建

显式 Context 创建时，Runtime 校验 device ID，retain Device，构造 Context 并执行 `Context::Setup`。Setup 初始化 module allocator，申请 overflow address，主 Context 复用设备 Primary Stream，非主 Context 创建并 Setup default Stream，满足 feature/run-mode 时再初始化 online fast CQ，最后才把 Context 状态设为 ACTIVE `[runtime/src/runtime/api/impl/api_impl.cc:3508-3551]` `[runtime/src/runtime/core/src/context/context.cc:538-683]`。

`rtSetDevice` 不是简单写入 device ID：它 retain Primary Context，将引用对象写入线程局部容器，校验 Context/default Stream，设置 SatMode，清除显式 current Context 并发出设备状态回调 `[runtime/src/runtime/api/impl/api_impl.cc:2914-2940]`。因此 ACL/GE 在调用设备 API 后可能获得的是 Runtime 管理的 Primary Context 引用，而不是一个只存在于 wrapper 层的整数状态。

### 3.2 销毁

Context 销毁先进行有效性和访问模式检查，Primary Context 显式 destroy 被拒绝；显式 Context 要取得唯一 teardown 执行权并切换 ACTIVE→FINALIZING。TearDown 删除 Context 上的 Model，清理 owner Streams，清理 online/default Stream，最后清除线程绑定 `[runtime/src/runtime/api/impl/api_impl.cc:3553-3595]` `[runtime/src/runtime/core/src/context/context.cc:685-833]`。

如果 Stream 清理失败且仍有合法 child handle，代码会恢复 owner stream 以支持重试。最终删除还需要非 Primary、delete 标志和线程引用计数为 0，随后切换 DEINITIALIZING、从 Registry 移除并 `delete this` `[runtime/src/runtime/core/src/context/context.cc:759-785,1887-1923]`。这构成了资源顺序：

```text
停止新访问 -> 清理 Model -> flush/清理 Stream -> 清除线程绑定
-> 等待引用耗尽 -> 移除 Context Registry -> 删除 Context
```

## 4. 三条内存生命周期

### 4.1 KernelMemoryPool

Runtime 内部 Kernel/Program/Binary 请求若满足单次不超过 2 MiB，可从 fixed-size HBM backing pool 的空闲块分配。池用 `MemoryList` 链表从头扫描首个满足大小的块，即 First-Fit；释放块尾插，管理器按 read-only 分池，并在空闲池超过 5 个时释放整池 `[runtime/src/runtime/core/src/pool/memory_pool.cc:40-105]` `[runtime/src/runtime/core/src/pool/memory_pool_manager.cc:69-201]` `[runtime/src/runtime/core/src/pool/memory_list.cc:61-80]`。

它与 SOMA 的策略不同：KernelMemoryPool 的空闲块没有按大小排序，不能描述为 Best-Fit；源码也未显示 `MemoryList` 释放时执行相邻块合并。其 backing 生命周期依赖 Device/Driver，但没有公开 Stream-ordered handle。不要用 Kernel pool 的 used size 推导 SOMA 的 busy/reserved size。

### 4.2 SOMA

SOMA 创建同时产生三层资源：

1. Runtime `SegmentManager` 和 Registry ownership；
2. Driver/HAL pool、虚拟地址和设备 backing 管理；
3. 异步 malloc/free 使用的 `SomaMemMng` AICPU 命令。

Runtime 普通 async free 会把 Segment 从 `allocedMap_` 移到 CACHED；force free 才进入 FREE。当前 `TryToReuse` 直接返回 nullptr，所以 cached 段的候选重用算法虽然存在，当前控制流仍走 `AllocFromFreeSegs`。`freeSegs_` 和 `cachedSegs_` 使用按 `(size, basePtr)` 升序的 `SegmentComparator`；因此 `AllocFromFreeSegs` 的 `lower_bound` 选择的是最小满足请求的 FREE Segment，即 Best-Fit（同尺寸按低地址），而不是 First-Fit `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:67-75,154-157]` `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:345-361]`。

销毁顺序是 Driver `StreamMemPoolDestroy` 成功、Registry 移除、shared ownership reset；使用中对象和设备侧未完成任务能否被拒绝或等待，需要设备验证。显式 trim 则先问 Runtime 统计、调用 Driver trim，再更新本地 Segment 状态 `[runtime/src/runtime/feature/soma/soma.cc:287-325]`。

### 4.3 普通 device/Host memory

`rtMalloc` 先由 `ApiImpl::DevMalloc` 解析 memory type、policy、advise、module/device config 并 32-byte 对齐；DVPP 走专用 Driver 分支，普通请求进入 `NpuDriver::DevMemAlloc` `[runtime/src/runtime/api/impl/api_impl_memory.cc:765-842]`。Driver 按 online/offline/AICPU-scheduler 选择路径，并将页类型、HBM/DDR/P2P、节点和 module ID 编进 HAL flags，最终调用 `halMemAlloc` `[runtime/src/runtime/driver/npu_driver_mem.cc:1026-1114,1194-1215,1302-1326]`。

Host allocation 同样按 32-byte 对齐，host register 还要在 pin/map 能力不足时维护 Runtime 软件表。Host free/unregister 的顺序必须与 location、map、pin 状态相符 `[runtime/src/runtime/api/impl/api_impl_memory.cc:47-158,160-247]`。

## 5. 异步完成边界

### 5.1 普通任务

Stream 只是提交顺序和依赖的 Runtime 对象；真正设备消费在 Driver queue/HDC/esched 和设备侧调度链中发生。除非调用 `rtDeviceSynchronize`、Stream synchronize、Event wait 或已确认 callback 完成，否则应保留被任务引用的 memory、model、stream、event 和 callback data。

### 5.2 SOMA

SOMA async malloc 的本地 Segment 分配发生在 AICPU kernel 提交之前；提交失败时 Runtime 尝试 free，但默认非 force free，可能进入 CACHED。Driver V3 malloc 配置先建立本地 VMM segment，再发送 `SVM_SOMA_MEM_ALLOC`；free 配置先删除 segment，再发送 `SVM_SOMA_MEM_FREE`，源码明确标注 ioctl 失败“不回滚” `[runtime/src/runtime/api/impl/api_impl_soma.cc:64-150]` `[runtime/src/ascend_hal/svm/v3/api/master/svm_soma.c:744-821,862-901]`。

这形成一个必须测试的故障窗口：上层 Runtime 元数据、Driver 用户态 VMM segment、远端设备 pool client 和 AICPU 操作可能短暂处于不同状态。

### 5.3 同步释放

`rtFreeWithDevSync` 和 `rtFreeHostWithDevSync` 显式先同步设备再释放；它们不是普通 free 的别名 `[runtime/src/runtime/api/api_c_memory.cc:173-192]`。该差异是跨模块 API 契约的一部分。

## 6. 错误回滚矩阵

| 故障点 | 已确认的清理/回滚 | 需验证的残余状态 |
|---|---|---|
| Context Setup 失败 | `NewContext` 调用 `TearDown` 后 delete；部分初始化分支有局部清理 | Device retain 引用和所有 feature-specific 资源 |
| Stream Setup 失败 | 删除 newly created default Stream；Context 创建失败返回 | Driver SQ/CQ 资源是否完全回收 |
| Kernel pool Init 失败 | 删除新池及已创建池 | HAL 分配失败日志和重试 |
| SOMA HAL create 失败 | Standard SoC 释放已保留 VA；V3 create 失败释放 global VA/client 中间资源 | 各后端 destroy 语义 |
| SOMA AICPU launch 失败 | 尝试本地 `FreeToMemPool` | cached/设备配置/sequence 是否一致 |
| SOMA V3 free ioctl 失败 | 已删除 VMM segment，源码标注不回滚 | 后续重试、泄漏和地址可见性 |
| ordinary `halMemAlloc` 失败 | Runtime 映射 drv error 并记录 flags/module/device | Driver 内部部分分配是否留下资源 |
| Context/Stream teardown 失败 | 恢复 owner stream 或保留对象以便重试 | 设备 pending task 和 callback 具体状态 |

## 7. 跨模块变更影响

- 修改 `rtMemPool_t` 属性、Segment 状态或 AICPU 参数：同步检查 Runtime C API、ApiImpl、SomaApi、SegmentManager、PoolRegistry、NpuDriver、HAL、SDK-driver/ioctl、ACL/GE allocator 和测试。
- 修改普通 `rtMalloc` policy/flags：检查 ACL/GE 对内存类型的假设、Runtime Driver flags、HAL ABI 和不同 run mode；不应假设会影响 SOMA。
- 修改 Context/Stream teardown：检查模型执行、异步 callback、SOMA Stream/Event sequence、DFX/profiling 和设备 reset。
- 修改公共错误码：同步 Driver→Runtime→ACL/GE 映射、扩展错误上下文和失败回滚测试。
- 修改 KernelMemoryPool：检查 Kernel/Program/Binary 所有调用方和池回收阈值；SOMA 测试不能替代该回归。

## 8. 验证状态

已确认：Runtime 用户态关键控制流、资源对象字段、锁范围、SOMA V3 HAL 的主要 ioctl 前后顺序。

推断：Runtime queue/HDC/esched 是异步设备任务的下游承载；跨仓 ABI 需要版本配套。

未验证：真实 NPU 上的设备完成时间、busy destroy 拒绝/等待、AICPU kernel 行为、不同 SoC 后端差异、HAL 到固件的实际错误码、性能收益。

本环境未执行构建、UT、样例、设备操作或依赖安装。
