# M03 Runtime：模块入口

- 对应源码版本：`dae460b78`
- 证据状态：C API、构建分层和 ACL Runtime 映射已确认

Runtime 是设备运行时和维测基础设施，负责 Device、Context、Stream、Event、内存、任务调度、模型/Kernel 执行以及 log、profiling、dump、error manager 等 `[runtime/README.md:7-14]`。

## 关键入口

- `rtSetDevice`、`rtGetDeviceCount`、`rtDeviceReset`、`rtDeviceSynchronize`：`src/runtime/api/api_c_device.cc` `[49-119,208-215]`。
- 通用 C API 门面：`src/runtime/api/api_c.cc`，通过 `Api::Instance()`、句柄校验宏和错误宏委托内部对象 `[118-153]`。
- `src/acl/aclrt/acl_rt.cpp`：ACL Runtime 导出映射和 Hook `[runtime/src/acl/aclrt/acl_rt.cpp:17-104]`。
- `src/acl/aclrt_impl/acl_rt_impl_base.cpp`：SoC、平台信息和 ACL 初始化全局状态 `[25-199]`。
- [内存池源码级分析](memory-pool-analysis.md)：KernelMemoryPool、SOMA 与普通设备/Host 内存的分层、状态、锁、异步 ABI 和验证边界。

## 已确认的深层边界

- `rtSetDevice` 并非只更新一个线程变量：`ApiImpl::SetDevice` 取得 `PrimaryContextRetain` 返回的引用对象，写入线程局部 current reference，校验默认 Stream，设置设备 SatMode，清除显式 current Context，并发出设备状态回调 `[runtime/src/runtime/api/impl/api_impl.cc:2914-2940]`。
- 普通 `rtMalloc` 按 policy、module/device 配置和 32-byte 对齐后进入 `NpuDriver::DevMemAlloc`；该 Driver 再按 online/offline/AICPU-scheduler 选择 `halMemAlloc` flags `[runtime/src/runtime/api/impl/api_impl_memory.cc:765-842]` `[runtime/src/runtime/driver/npu_driver_mem.cc:1026-1114,1302-1326]`。
- `rtMemPool*` 的 Segment 复用选择入口当前是 no-op；显式 trim 有 Driver→本地两阶段，而隐式 trim 当前直接成功返回。详见专题，不将设计意图写成当前行为。

## 进一步阅读：内存分层

普通 `rtMalloc` 不等于 Runtime 内部池分配：KernelMemoryPool 服务 Kernel/Program/Binary，SOMA 服务 `rtMemPool*` Stream-ordered API，普通设备内存经 `NpuDriver::DevMemAlloc` 后还可能进入 Driver V2/V3 ordinary cache。三条路径的对象、锁、回收和异步语义不同，详见 [memory-pool-analysis.md](memory-pool-analysis.md)。

已确认的实现差异：KernelMemoryPool 是 2 MiB backing 上的 `MemoryList` First-Fit；SOMA FREE Segment 使用 `lower_bound` 的按大小 Best-Fit，但当前 `TryToReuse` 和隐式 trim 为 no-op；Driver ordinary cache 的 V2/V3 选择由 SoC 产品构建决定，而非本文档层面的运行时猜测。
