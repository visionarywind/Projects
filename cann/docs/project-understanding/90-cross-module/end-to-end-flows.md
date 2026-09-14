# 跨模块：端到端流程

- 证据状态：主要链路已确认；Runtime→Driver 深层调用和真实设备行为为推断/未验证

## 1. 图编译与执行

```text
Framework/model file
  -> GE API / Parser
  -> Graph MetaDef / AscendIR
  -> Compiler Pass / Engine / planner
  -> OM / ExecuteGraph
  -> GE V1/V2 Executor
  -> Runtime rt*/aclrt*
  -> Driver HAL / queue / HDC
  -> NPU
```

GE 的前端、IR、Compiler 和 Executor 分工见 `[ge/docs/zh/design/architecture.md:11-74]`；V2 Load/Execute/Unload 的状态和参数路径见 `[ge/runtime/v2/core/model_v2_executor.cc:201-318]`。

## 2. ACL 设备调用

```text
Application
  -> aclrtSetDevice
  -> aclrtSetDeviceImpl
  -> rtSetDevice
  -> ApiImpl::SetDevice
  -> PrimaryContextRetain
  -> thread-local primary Context reference
  -> Device::SetSatMode + state callback
```

ACL 包装和 Runtime C API 入口分别由 `[acl/runtime/device.cpp:47-59]`、`[runtime/src/runtime/api/api_c_device.cc:74-84]` 确认；真正的 Runtime 设备选择副作用在 `[runtime/src/runtime/api/impl/api_impl.cc:2914-2940]`。这条路径不应在 `Api::Instance` 或 wrapper 处终止。

## 3. 队列任务

```text
ACL/GE execution data
  -> Runtime Context/Stream/Event/task
  -> Driver queue/esched/HDC
  -> device scheduler
  -> completion event/callback/synchronize
```

Driver 的 Queue context、HDC session 和调度 ioctl ABI 已确认 `[driver/src/sdk_driver/queue/host/queue_fops.c:62-175]`、`[driver/src/sdk_driver/esched/command/ioctl/esched_ioctl.h]`；精确命令映射和设备完成语义待补。

## 4. 内存资源路径

```text
ACL/GE model/input/workspace
  -> ordinary rtMalloc or Runtime internal KernelMemoryPool
  -> Runtime DevMemAlloc / halMemAlloc
  -> Driver memory backing

ACL/GE stream-ordered allocation
  -> rtMemPoolMallocAsync
  -> SegmentManager (FREE/BUSY/CACHED)
  -> AICPU SomaMemMng + HAL async config
  -> Driver SVM pool / device backing
```

三类路径的 ownership、失败窗口和显式/隐式 trim 差异见 [memory-and-resource-lifecycle.md](memory-and-resource-lifecycle.md) 和 [../01-modules/M03-runtime/memory-pool-analysis.md](../01-modules/M03-runtime/memory-pool-analysis.md)。当前 `TryToReuse` 和隐式 trim 的源码状态不能由 API 名称推导。

## 4.1 普通 rtMalloc 的 Driver cache 分支

```text
rtMalloc
  -> Runtime DevMalloc / policy + alignment
  -> NpuDriver::DevMemAlloc
  -> halMemAlloc
  -> 产品选择的 Driver SVM
       -> V2: heap / mapped tree / idle size tree
       -> V3: cache_allocator / range / area
       -> normal allocator（不满足 cache 条件）
  -> 返回地址

rtFree
  -> cache free：合并并暂存 backing
  -> shrink 条件满足：释放完整 node/range
```

V2/V3 ordinary cache 不等同于 SOMA；其命中还受 flag、size、align、NUMA、设备能力和产品构建影响。详细证据见 [M04 Driver cache 专题](../01-modules/M04-driver/driver-memory-pool-analysis.md)。


- 初始化：GE/ACL/Runtime/Driver 必须按版本兼容组合启动。
- 提交：检查状态、句柄、shape、地址、容量和 stream 归属。
- 异步：保持 buffer、model、stream、event、callback 存活；SOMA 还要区分本地 Segment 与设备侧操作完成。
- 释放：先等待/关闭任务和 session，再释放上层对象与底层 client；Context teardown 失败时保留可重试状态。

## 未验证项

当前没有匹配 NPU、固件和闭源 Toolkit，未执行构建、样例或端到端运行。
