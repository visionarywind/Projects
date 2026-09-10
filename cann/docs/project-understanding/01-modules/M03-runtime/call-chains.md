# M03 Runtime 调用链

- 证据状态：`rtSetDevice`、Context 创建/销毁、普通内存和 SOMA 关键控制流已确认；HAL/SDK-driver/设备完成语义仍部分未验证

## 设备选择：`rtSetDevice`

```text
rtSetDevice(devId)
  -> GLOBAL_STATE_WAIT_IF_LOCKED
  -> Api::Instance()->SetDevice
  -> ApiDecorator::SetDevice
  -> ApiImpl::SetDevice
  -> GetDrvSentinelMode
  -> Runtime::PrimaryContextRetain(devId)
  -> InnerThreadLocalContainer::SetCurRef(primary-context RefObject)
  -> 校验 Context / DefaultStream
  -> Device::SetSatMode(Runtime::GetSatMode)
  -> 清除显式 current Context
  -> DeviceStateCallbackManager::Notify(SET_POST)
```

C API 等待全局状态锁并委托门面 `[runtime/src/runtime/api/api_c_device.cc:74-84]`；Decorator 只是转发 `[runtime/src/runtime/api/impl/api_decorator.cc:727-727]`，实际副作用在 `ApiImpl::SetDevice`：保留 Primary Context、更新线程局部引用、应用 SatMode 和发送设备状态回调 `[runtime/src/runtime/api/impl/api_impl.cc:2914-2940]`。`PrimaryContextRetain` 的创建/重用、设备 retain 和失败回滚由 Runtime 实现继续承接；本文件不把回调通知等同于设备任务完成。

## Context 创建与销毁

```text
rtsCtxCreate
  -> Api::Instance / ApiImpl::ContextCreate
  -> Driver::GetDeviceCount 校验 devId
  -> Runtime::DeviceRetain(deviceId, tsId)
  -> new Context(dev, false)
  -> Context::Setup
       -> Init (module allocator)
       -> SetOverflowAddr (DevMemAlloc)
       -> 创建/取得 default Stream
       -> Stream::Setup
       -> OnlineStreamInit（条件满足时 fast CQ）
       -> state = ACTIVE
  -> ContextManage::InsertContext
  -> ContextSetCurrent
```

`ContextCreate` 的设备计数校验、`NewContext`、Setup 失败时 `TearDown`/delete、Registry 插入和绑定当前线程均在 `[runtime/src/runtime/api/impl/api_impl.cc:3508-3551]`；Setup 的 allocator、溢出地址、default stream、同步和 ACTIVE 状态在 `[runtime/src/runtime/core/src/context/context.cc:538-683]`。

```text
rtsCtxDestroy(ctx)
  -> CheckContextIsValid(USER)
  -> 拒绝 Primary Context
  -> TearDownIsCanExecute
  -> Context::TearDown
       -> 删除 Context 上 Model
       -> 清理 owner Streams（失败可恢复）
       -> 清理 online/default Stream
       -> 清除线程绑定
  -> Context::TryDeleteIfNeeded
       -> 非 Primary 且线程引用数为 0
       -> 标记 DEINITIALIZING
       -> 从 ContextManage 集合移除
       -> delete Context
```

销毁时 Primary Context 不能显式销毁；显式 Context 的 TearDown 失败会让对象保持可重试状态。最终删除还受线程引用计数保护 `[runtime/src/runtime/api/impl/api_impl.cc:3553-3595]` `[runtime/src/runtime/core/src/context/context.cc:685-741,759-785,1887-1923]`；`ContextManage` 的有效性检查在读锁下执行，并在必要时把无效线程绑定移出读锁范围 `[runtime/src/runtime/core/src/context/context_manage.cc:72-103]`。

## 普通设备内存

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

入口和 policy 映射见 `[runtime/src/runtime/api/impl/api_impl_memory.cc:765-842]`；Driver 运行模式分派见 `[runtime/src/runtime/driver/npu_driver_mem.cc:1302-1326]`，online 大块 huge-first fallback、huge-only 和 1G huge 分支见 `[runtime/src/runtime/driver/npu_driver_mem.cc:1026-1114]`。HAL 到更下层的动态实现和设备行为仍需匹配 Driver/固件验证。

## SOMA 异步内存

```text
rtMemPoolMallocAsync
  -> Api C wrapper / ApiImplSoma
  -> SomaApi::AllocFromMemPool
  -> SegmentManager::SegmentAlloc
       -> TryToReuse（当前无条件 nullptr）
       -> AllocFromFreeSegs / FREE segment split
       -> BUSY + allocedMap_ + counters
  -> StreamMemPoolAsyncConfig
  -> AicpuPoolCtxArgs
  -> StreamLaunchCpuKernel("SomaMemMng")

rtMemPoolFreeAsync
  -> SOMA 地址区间查询
  -> SegmentFree（BUSY -> CACHED）
  -> StreamMemPoolAsyncConfig / SomaMemMng FREE
  -> 非 SOMA 指针则 LaunchHostFunc -> DevFreeStatic
```

Segment 分配/释放事实见 `[runtime/src/runtime/feature/soma/stream_mem_pool.cc:140-218]`；SOMA API 对齐、指针定位和 trim 见 `[runtime/src/runtime/feature/soma/soma.cc:235-325]`；AICPU 和非 SOMA callback 路径见 `[runtime/src/runtime/api/impl/api_impl_soma.cc:64-195,197-267]`。当前 `MemPoolTrimImplicit` 是 no-op `[runtime/src/runtime/feature/soma/soma.cc:328-333]`。

## 设备同步与释放边界

`rtFreeWithDevSync` 和 `rtFreeHostWithDevSync` 显式先调用 `rtDeviceSynchronize`，成功后才释放 `[runtime/src/runtime/api/api_c_memory.cc:173-192]`。这与普通 `rtFree`/异步 SOMA free 的提交语义不同；调用方必须保留仍被设备任务引用的 buffer、Stream、Event 和 callback 数据，设备侧完成与错误传播不能从 API 提交成功推出。

## 设备/Driver 边界

Runtime `NpuDriver` 将内存、SOMA、资源和同步操作转成 weak HAL 函数指针或 `halMemAlloc`/`halMemFree` 调用；SOMA V3 HAL 已能静态确认 pool client、VMM segment 和 SOMA ioctl 的边界，但 SDK-driver→内核→固件的完整动态路径、不同后端选择和设备完成语义仍标记为“未验证”。
