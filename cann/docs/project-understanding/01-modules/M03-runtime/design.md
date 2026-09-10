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

## 设计取舍

单例门面有利于统一 ABI、错误和资源调度，但隐藏了内部对象和动态依赖，调试时需从 C API、Api 实现、DFX 和 Driver 层逐级追踪。构建时将 DFX 与 runtime 组件拆分，有利于打包和可选能力；具体运行时装载策略待确认。
