# 运行时轨迹

- 文档目的：用状态阶段描述 CUDA Driver 从 host 初始化到命令完成和资源回收的运行时轨迹。
- 适用范围：`source/cuda` host-side 静态路径；设备消费和动态结果未验证。
- 证据状态：源码状态转移和调用顺序为静态确认；未运行 nvmake、DVS、GPU、OpenCL、debugger 或 profiler。
- 前置阅读：[Kernel Launch 端到端调用链](end-to-end-kernel-launch.md)、[具体架构设计](architecture-design.md)。

## 1. 轨迹总览

```text
UNINITIALIZED
  → GLOBALS_READY
  → CONTEXT_CURRENT
  → RESOURCES_READY
  → LAUNCH_PREPARED
  → PUSH_OPEN
  → SUBMITTED
  → DEVICE_PENDING
  → COMPLETED
  → RECLAIMED
  → DETACHED/FREE
```

这些是分析状态，不是源码中统一的 enum；每个阶段对应不同对象的实际状态和源码函数。

## 2. 阶段和状态

| 阶段 | 主要动作 | 关键对象 | 可确认的状态 |
|---|---|---|---|
| `UNINITIALIZED` | API 首次进入 | globals/TLS | `globals.initialized` 未设置 |
| `GLOBALS_READY` | `cuiInitInternal` | UVM/UVA/heap/memmgr | 全局 managers 建立 |
| `CONTEXT_CURRENT` | Context 创建/推入 TLS | `CUctx` | current Context 可用 |
| `RESOURCES_READY` | alloc stream/module/memory | memobj/stream/QMD/function | launch 输入资源可引用 |
| `LAUNCH_PREPARED` | validation/setup/tracking | launchData/memTrack/QMD | 尚未必发布 GPFIFO |
| `PUSH_OPEN` | `streamBeginPush*` | CUnvCurrent/channel | pushbuffer 正在构造 |
| `SUBMITTED` | GPFIFO/GPPut | entry/flush unit | host-side 发布完成 |
| `DEVICE_PENDING` | 外部 RM/GPU 消费 | marker/semaphore | 设备完成未知 |
| `COMPLETED` | marker 状态完成 | marker/QMD | 可安全推进相关 reclaim |
| `RECLAIMED` | get/reclaim | pushbuffer/memory | 资源可复用 |
| `DETACHED/FREE` | stream teardown | CUIstream | deferred stream 回池 |

## 3. 初始化轨迹

```text
cuInit
  [API validation]
  → cuiInit
  [global mutex]
  → cuiInitInternal
  [TLS, globals, platform, UVM, UVA, heap, primary memmgr]
  → GLOBALS_READY
```

初始化失败时不会进入 `GLOBALS_READY`；已建立的对象必须逆序清理。源码提供 host-side rollback，不能据此推断外部设备初始化已经成功。

## 4. Context 和资源轨迹

```text
cuCtxCreate_v2
  → cuiCtxAllocate
  → cuiCtxInitialize
  → TLS current
  → CONTEXT_CURRENT

cuMemAlloc_v2
  → CUmemdesc
  → memobj/memblock/backing
  → device pointer

cuStreamCreate
  → stream pool
  → QMD/semaphore/marker
  → ACTIVE stream

cuModuleLoad/GetFunction
  → code/constant/function/syscall metadata
  → RESOURCES_READY
```

资源建立成功不代表它们已经和某次 launch 绑定；绑定发生在 launch setup 和 tracking 阶段。

## 5. 普通 launch 轨迹

### 5.1 Validation

```text
cuLaunchKernel
  → cuapiLaunchKernelCommon
  → cuiValidateLaunch
```

检查 current Context、Function、Stream、参数和 launch 配置。失败状态停留在 `RESOURCES_READY`，不应进入 `PUSH_OPEN`。

### 5.2 Capture 分支

```text
RESOURCES_READY
  → stream is capturing
  → Graph kernel node
  → 保存 launchData/依赖
  → 回到 host graph state
```

此分支不进入 `SUBMITTED`；实际设备提交推迟到 Graph launch。

### 5.3 Non-capture 分支

```text
RESOURCES_READY
  → cuiLaunchKernel_nonreentrant
  → doLaunchMemTracking
  → cuiSyscallOnKernelLaunch
  → hal.launchCheck
  → constant bank/QMD/ABI setup
  → hal.launchInitialize
  → LAUNCH_PREPARED
```

### 5.4 Push 和提交

```text
LAUNCH_PREPARED
  → streamBeginPushWithFlags
  → PUSH_OPEN
  → hal.launchFinalize
  → hal.launchControl
  → channelTrackQMD
  → streamEndPush
  → gpfifoFlushGpfifoEntry
  → channelFlushUnitFlushGPFIFO
  → DMAL GpfifoAdvanceGpuPut
  → RM cache flush/fence/writeGPPut
  → SUBMITTED
```

`SUBMITTED` 仅代表 host-side 提交位置已发布。`DEVICE_PENDING` 的实际进入依赖外部 RM/firmware/GPU 行为。

## 6. 完成轨迹

```text
SUBMITTED
  → DEVICE_PENDING
  → tracking semaphore / completion marker
  → gpfifoAdvanceGpuGet
  → pushbuffer reclaim
  → release tracked memory/QMD references
  → COMPLETED
  → RECLAIMED
```

同步 API 主要等待或推动这条状态链；异步 API 返回时通常仍处于 `SUBMITTED` 或 `DEVICE_PENDING`。

## 7. Stream 状态轨迹

```text
FREE
  → ACTIVE
  → DETACHED
  → FREE
```

`cuiStreamDetach_UnderLock` 会移除 public handle、注销 UVM stream、abandon semaphore、deallocate QMD，并把 stream 放入 detached list。只有：

```text
ctxMarkerGetStatus
  → CU_CTX_MARKER_COMPLETED_BY_GPU
```

才由 `cuiStreamReclaimDetached` 清空 marker、重置字段并放回 free list。

## 8. Graph 轨迹

```text
capture
  → graph nodes
  → instantiate: validate/clone/flatten/schedule
  → QMD/constant-bank/stream/marker/scheduler resources
  → graph launch
  → scheduling group push
  → marker completion
  → graph exec destroy/reclaim
```

Graph instantiate 后的资源计划是可重复执行的 host-side execution plan；Graph launch 仍需创建当前 memory tracking，因为 Context 资源可能已变化。

## 9. 错误轨迹

```text
validation error
  → RESOURCES_READY

setup/HAL error
  → rollback launch temporary state
  → no SUBMITTED

push construction error
  → close/rollback PUSH_OPEN
  → preserve error

DMAL/RM submit error
  → submission uncertain or failed
  → do not mark COMPLETED

device trap/timeout
  → external error propagation
  → marker/reason-buffer/error state

wait timeout/error
  → remain pending/error
  → do not reclaim uncompleted objects
```

对于“提交是否已经到达设备”无法从某些 host-side失败返回值直接推断时，文档应记录为不确定状态，而不是把资源标记为已完成。

## 10. 观测点和测试映射

源码和测试可以分别观察：

- API 返回值、Context 和 stream handle；
- `beginPushCount`、launch count、channel 选择；
- marker/semaphore 状态；
- DVS runner 的 pass/failure/timeout/no-result 聚合。

这些 host/test 观察点不能替代 GPU 设备执行证据。D01 可作为基础 memory/stream/launch 入口；Graph、syscall、UVM migration、工具和 backend 专用路径仍缺乏当前 checkout 中的运行验证。

## 11. 关联文档

- [Kernel Launch 端到端调用链](end-to-end-kernel-launch.md)
- [具体架构设计](architecture-design.md)
- [跨模块调用链审计](cross-module-call-chains.md)
- [跨模块接口契约](interface-contracts.md)
- [D01 执行轨迹](../80-demos/D01-cuda-test-memory-stream/execution-trace.md)
