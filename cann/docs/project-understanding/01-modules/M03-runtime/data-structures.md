# M03 Runtime 数据结构与资源

- 证据状态：Device/Context/Stream/SOMA/KernelMemoryPool 关键字段和所有权已确认；完整全量布局仍待补

## 资源对象

| 对象 | 作用 | 关键不变量 |
|---|---|---|
| Runtime `Api` singleton | C API 到内部实现的门面 | 初始化前不能使用内部能力 |
| Device | 设备选择、属性和 reset | device ID 必须有效；Context 通过 Device 访问 Driver |
| Context | 当前执行环境和资源 owner | Stream/Event/Model/任务需匹配 Context；状态必须可访问 |
| Stream | 异步任务队列和顺序 | 提交后保持有效；销毁前 flush/teardown |
| Event | 完成和跨队列依赖 | 等待关系和 Registry sequence 记录需先于释放维护 |
| KernelMemoryPool | Runtime 内部 Kernel/Program/Binary 小块分配 | 固定 2 MiB backing；按 read-only 分池；非 SOMA |
| SOMA `SegmentManager` | `rtMemPool_t` 的区间、Segment 和统计管理 | `allocedMap_`、`cachedSegs_`、`freeSegs_` 与双向链一致 |
| SOMA `Segment` | 单个虚拟地址区间状态 | FREE/BUSY/CACHED 状态和 stream/seq 元数据一致 |
| Device memory | 输入、输出、workspace 和内部 buffer | 地址/容量/对齐满足 API 和模型约束 |
| Model/Kernel handle | 可执行对象 | 释放前不可有未完成任务 |
| Queue/HDC handle | 向 Driver 提交工作 | Driver session 生命周期先于 queue 使用 |

## SOMA 关键字段

`SegmentManager` 使用 `allocedMap_` 记录 BUSY 基地址，`cachedSegs_` 记录逻辑释放段，`freeSegs_` 记录可立即分配段；`busySize_` 是已分配大小，`reserveSize_` 是已分配加缓存大小。`state_` 保存 Stream/Event 依赖开关和 watermark，`canDelete_`、`isIPCPool_` 影响销毁/分配边界 `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:107-171]`。

`PoolRegistry` 以按池基址排序的 `entries_` 定位池，以 `poolOwnership_` 保存 shared ownership，并维护 Event/Stream sequence map。查询返回 `shared_ptr`，因此查询期间 manager 不会仅因 Registry 移除而立刻析构 `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:180-230]`。

## 全局缓存

SoC 版本和平台资源信息由 ACL Runtime 基础实现缓存，包含 AICore、Vector Core、Cube Core 数量 `[runtime/src/acl/aclrt_impl/acl_rt_impl_base.cpp:41-151]`。

## 句柄安全

C API 不直接暴露内部 C++ 对象，而通过 `RT_VALIDATE_AND_UNWRAP_OBJECT` 等宏校验和解包 `[runtime/src/runtime/api/api_c.cc:118-153]`。SOMA 的 `rtMemPool_t` 在当前实现中承载 `SegmentManager*`，但调用者仍必须通过公开 API 使用，不能依赖内部布局 `[runtime/src/runtime/feature/soma/soma.cc:245-264]`。

## 锁与生命周期

KernelMemoryPoolManager 以 `shared_mutex` 保护池集合；分配/释放使用写锁，查询使用读锁。SegmentManager 自身以 mutex 保护 Segment 集合和计数。Context teardown 通过原子 teardown 状态和线程引用计数防止重复销毁或过早 delete `[runtime/src/runtime/core/src/pool/memory_pool_manager.cc:26-41,69-141,174-234]` `[runtime/src/runtime/core/src/context/context.cc:1887-1923]`。
