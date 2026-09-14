# M03 Runtime 设计

- 证据状态：门面和构建边界已确认；内部调度实现部分未知

## 分层

```text
rt*/rts* C API
  -> Api::Instance() facade
  -> Device/Context/Stream/Memory/Event/Task objects
  -> scheduler / DFX / platform
  -> Driver HAL and device interface
```

`api_c_device.cc` 的设备函数获取 `Api::Instance()` 并统一转换错误 `[runtime/src/runtime/api/api_c_device.cc:49-119]`；通用 API 用宏完成对象解包和返回值处理 `[runtime/src/runtime/api/api_c.cc:118-153]`。

## 全局状态

ACL Runtime 实现用互斥锁保护 SoC 缓存和初始化引用计数，另有配置字符串 `[runtime/src/acl/aclrt_impl/acl_rt_impl_base.cpp:25-32]`。`InitSocVersion` 在锁内调用 `rtGetSocVersion` 并缓存结果 `[120-135]`。这减少重复查询，但使初始化、Finalize、设备切换和多线程调用必须遵循统一状态协议。

## 内存路径的设计边界

Runtime 至少维护三种不同的内存策略：

```text
KernelMemoryPool：Runtime 内部固定 2 MiB backing，小块链表 First-Fit
SOMA：公开 rtMemPool_t，Segment 状态机和 Stream-ordered AICPU 协议
ordinary memory：DevMalloc → NpuDriver → HAL；Driver 可能使用 V2/V3 cache
```

SOMA 的本地 Segment 状态变化不等于设备侧异步操作完成；当前源码中 `TryToReuse` 直接返回空指针，`MemPoolTrimImplicit` 直接成功返回，故设计接口与当前实际控制流必须分开记录。普通 `rtMalloc` 的 Driver cache 详见 M04 专题。

