# 源码证据索引

- 文档目的：集中维护关键结论的可追溯源码证据
- 适用范围：当前已分析路径
- 对应源码版本：见 `analysis-state.md`
- 证据状态：已确认静态证据；未做运行时验证
- 最后更新：2026-09-14
- 前置阅读：project-overview.md
- 后续阅读：对应模块 source-map

| 主题 | 结论 | 证据 |
|---|---|---|
| GE 定位 | GE 是图编译器和执行器，支持图优化、多流、内存复用、模型下沉 | `[ge/README.md:10-13]` |
| GE 分包 | 可构建 `ge-compiler`、`ge-executor`、`dflow-executor` | `[ge/CMakeLists.txt:30-47]` |
| GE 初始化 | `GEInitialize` 调用 V2 初始化、异构时初始化 DFlow，并创建 SessionManager | `[ge/api/session/client/ge_api.cc:199-239]` |
| GE Session 生命周期 | 构造 InnerSession、Initialize、注册 Registry；析构 Unregister/Finalize | `[ge/api/session/session/ge_session_impl.cc:34-71]` |
| GE V2 加载 | 初始化图先 Load/Execute/UnLoad，再加载主图并变为 Loaded | `[ge/runtime/v2/core/model_v2_executor.cc:201-230]` |
| GE V2 执行 | 校验状态、输入输出数量/指针、绑定资源限制、执行图并解除绑定 | `[ge/runtime/v2/core/model_v2_executor.cc:260-305]` |
| ACL 定位 | 提供设备、Context、Stream、内存、模型、算子、媒体 API | `[acl/README.md:20-41]` |
| ACL runtime 包装 | `aclrtSetDeviceImpl` 调用 `rtSetDevice` 并映射错误码 | `[acl/runtime/device.cpp:47-59]` |
| ACL 构建依赖 | 查找 runtime、ascend_hal、metadef、adump、msprof 等包 | `[acl/CMakeLists.txt:110-126]` |
| Runtime 定位 | 设备、流、Event、内存、任务调度及维测 | `[runtime/README.md:7-14]` |
| Fixed-slot resource pools | SPM/SQ address/copy/event 等固定 item 使用 `BufferAllocator` + `Bitmap` 分配 ID；`LINEAR/EXPONENTIAL` 仅控制扩容，不是 Best-Fit/First-Fit 块选择 | `[runtime/src/runtime/core/src/pool/buffer_allocator.hpp:23-54,81-125]` `[runtime/src/runtime/core/src/pool/buffer_allocator.cc:93-197]` |
| KernelMemoryPool | Runtime 内部池固定申请 2 MiB backing；`MemoryList` 使用 first-fit、分割空闲块，源码未显示邻接合并 | `[runtime/src/runtime/core/src/pool/memory_pool.cc:15-106]` `[runtime/src/runtime/core/src/pool/memory_list.cc:14-122]` |
| KernelMemoryPool manager | 按 read-only 分池，单次请求超过 2 MiB 不走池；最多保留 5 个空闲池；Contains/Release 在写锁内完成 | `[runtime/src/runtime/core/src/pool/memory_pool_manager.cc:20-233]` `[runtime/src/runtime/core/src/pool/memory_pool_manager.hpp:23-72]` |
| SOMA FREE segment strategy | `freeSegs_` 按 `(size, basePtr)` 升序排列，`lower_bound` 选择最小满足请求的 Segment，实际是 Best-Fit（同尺寸按低地址） | `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:67-75,154-157]` `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:345-361]` |
| SOMA Segment allocation | `SegmentAlloc` 当前从 `TryToReuse` 继续到 `AllocFromFreeSegs`，FREE 段左切割并更新 BUSY/统计 | `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:140-195,345-361]` |
| SOMA reuse status | `TryToReuse` 当前直接返回 `nullptr`；same-stream、Event 和 internal 辅助算法虽存在但未由统一入口启用；这些候选函数按大小下界扫描 | `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:140-147,245-334]` |
| SOMA implicit trim | `MemPoolTrimImplicit` 当前忽略参数并直接返回成功，不执行真实 trim | `[runtime/src/runtime/feature/soma/soma.cc:328-333]` |
| SOMA lifecycle | create 对齐/保留 VA/Driver pool/初始 Segment/Registry 注册；destroy 受 `CanDelete` 和 Driver destroy 顺序约束 | `[runtime/src/runtime/feature/soma/soma.cc:31-121,223-233]` |
| SOMA async ABI | `rtMemPoolMallocAsync/freeAsync` 配置 HAL 并提交 `SomaMemMng` AICPU kernel；非 SOMA free 走 HostFunc | `[runtime/src/runtime/api/impl/api_impl_soma.cc:64-195,197-267]` |
| SOMA Registry | `poolOwnership_` 保存 shared ownership；查询返回 `shared_ptr`；Stream/Event callback 维护 sequence metadata | `[runtime/src/runtime/feature/soma/stream_mem_pool.hpp:180-230]` `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:527-775]` |
| SOMA Driver V3 | async alloc 失败删除 VMM segment；async free 先删 VMM segment，free ioctl 失败明确不回滚 | `[driver/src/ascend_hal/svm/v3/api/master/svm_soma.c:744-821,862-901]` |
| Ordinary memory | `DevMalloc` 解析 HBM/huge/P2P/module/alignment policy，再进入 `NpuDriver::DevMemAlloc` 和 HAL | `[runtime/src/runtime/api/impl/api_impl_memory.cc:765-842]` `[runtime/src/runtime/driver/npu_driver_mem.cc:1026-1114,1302-1326]` |
| `rtFreeWithDevSync` | 同步释放先执行 `rtDeviceSynchronize`，与普通 free 的异步契约不同 | `[runtime/src/runtime/api/api_c_memory.cc:173-192]` |
| `rtSetDevice` side effects | `ApiImpl::SetDevice` retain Primary Context、写 TLS current reference、设置 SatMode 并通知设备状态 callback | `[runtime/src/runtime/api/impl/api_impl.cc:2914-2940]` |
| Driver ordinary cache | 普通 `rtMalloc` 进入 Driver 后可能使用 ordinary cache；V2/V3 具体实现按产品编译选择 | `[driver/src/ascend_hal/svm/CMakeLists.txt:11-15]` `[driver/build.sh:46-65]` `[../01-modules/M04-driver/driver-memory-pool-analysis.md]` |
| Driver V2 cache | heap + alloced/idle VA/idle size/idle mapped 多树；小请求先 mapped cache，按大小 exact/upper-bound，split/merge/shrink | `[driver/src/ascend_hal/svm/v2/devmm/devmm_virt_com_heap.c:1036-1137,1151-1391]` `[driver/src/ascend_hal/svm/v2/devmm/devmm_rbtree/devmm_rbtree.c:104-179]` |
| Driver V3 cache | cache_allocator + ga_range/ga_area；size tree exact/upper-bound，normal backing 扩展，完整 idle range 回收，BUSY 延迟 recycle | `[driver/src/ascend_hal/svm/v3/assign/cache_malloc/cache_malloc.c:133-275]` `[driver/src/ascend_hal/svm/v3/assign/gen_allocator/gen_allocator.c:163-177,388-422,600-668]` |
| Driver 构建 | HAL 和 SDK-driver 按组件/产品分支加入构建 | `[driver/src/CMakeLists.txt:9-30]` |
| Driver Queue | 设备表、open/release context、HDC 初始化和 IOCTL 参数检查 | `[driver/src/sdk_driver/queue/host/queue_fops.c:35-143,149-175]` |
| Driver HAL | `halGetSocVersion` 校验参数、查询设备信息并复制 SoC 版本 | `[driver/src/ascend_hal/dms/dc/devdrv_manager_adapter.c:16-52]` |
| Driver HDC | Client 创建分配 session 数组，Destroy 检查活动 session 后释放 | `[driver/src/ascend_hal/hdc/common/hdc_client.c:20-111,130-179]` |

## 相关文档

- [../01-modules/module-registry.md](../01-modules/module-registry.md)

## 源码证据摘要

本索引中的路径和行号均来自当前四个仓库 HEAD 的读取结果。

## 未解决问题

动态链接库和闭源 Driver 实现的符号映射需要目标构建产物补证。

## 下一步阅读建议

功能修改前先在此索引找到入口，再进入对应模块的 line-level-analysis。
