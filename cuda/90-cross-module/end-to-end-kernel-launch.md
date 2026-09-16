# CUDA Driver Kernel Launch 端到端流程

- 文档目的：以源码可追踪的 host-side 路径说明 CUDA Driver 从初始化到一次普通 kernel launch、异步完成和资源回收的完整生命周期。
- 适用范围：`source/cuda` 源码快照；引用相对于 `source/cuda`，行号以当前源码快照为准。
- 证据状态：调用关系、对象生命周期和错误回滚为静态源码确认；RM 内核驱动、firmware、GPU 消费、compiler 生成物和实际运行结果未验证。
- 对应源码版本：见 [`00-overview/analysis-state.md`](../00-overview/analysis-state.md)。
- 前置阅读：[总体架构](../00-overview/architecture.md)、[模块注册表](../01-modules/module-registry.md)。

## 1. 结论摘要

一次普通 kernel launch 不是单个函数调用，而是以下阶段的组合：

```text
应用
  → API wrapper 参数/句柄检查
  → TLS current Context
  → Function/Stream/Context 一致性检查
  → launchData 与参数 packing
  → syscall 与 memory tracking
  → 架构 HAL 检查和 QMD/ABI 编码
  → Stream/Channel 获取 pushbuffer
  → QMD、launch-control 和 tracking 命令
  → GPFIFO entry 发布
  → DMAL backend / RM 边界
  → tracking semaphore / marker
  → completion 查询、pushbuffer 和对象 reclaim
```

源码能确认到 `hal.writeGPPut` 以及 backend 的提交准备；不能从当前 checkout 单独证明 RM 内核驱动如何消费 GPPut、firmware 如何调度 QMD，或 GPU 已经执行完成。

## 2. 前置资源建立

### 2.1 全局初始化

```text
cuInit
  → src/api/apiinit.c: cuapiInit
  → src/cui/cuiinit.c: cuiInit
  → cuiGlobalMutexInitOnce / globals.initMutex
  → cuiInitInternal
      → CUDA 版本检查
      → cuiTlsInitialize
      → cuiPerformanceInit
      → globalsInit
      → memglobalsCreate
      → cuiPlatformInit
      → uvmManagerCreate
      → uvaManagerCreate
      → userdVaManagerCreate
      → thirdPartyP2PInit
      → host VA heap
      → primary memory managers
```

`cuiInit` 以全局锁保证幂等初始化，并在失败时释放已经创建的全局对象。这里建立的是 host-side driver 状态，不等于一次 kernel 已经提交。

### 2.2 Context

```text
cuCtxCreate_v2
  → cuapiCtxCreate_common
  → cuiCtxCreateParamsInit
  → cuiCtxCreate
      → cuiCtxAllocate
      → primary context retain
      → cuiCtxInitialize
          → Context memory/stream/channel state
          → semaphore/QMD/constant-bank state
          → UVM/syscall/tool state
  → TLS push current Context
```

失败路径从 `cuiCtxCreate` 返回到 `cuiCtxDestroy`，再反向执行 `cuiCtxFinalize`、`cuiCtxDeinitialize` 和 primary-context release。Context 是 Context 级 manager、锁和异步对象的所有权边界。

### 2.3 Device/HAL

设备初始化经过：

```text
device enumeration/remap/filter/sort
  → CUdev
  → InitDeviceArch
  → CUdev->hal
  → CUdev->dmal
```

`src/cui/hal/hal.c` 按 `ccArch` 选择 Kepler、Maxwell、Pascal、Volta、Turing 或 Ampere 初始化函数；可用架构由 `cuda.nvmk` 中的 `NVCFG_GLOBAL_ARCH_*` 条件决定。HAL 表提供 `launchCheck`、`launchInitialize`、`launchFinalize`、`launchControl`、`encodeQmd`、`encodeAbiConstBankGridParams`、`setGpfifoEntry` 等架构契约。

### 2.4 Memory

```text
cuMemAlloc_v2
  → cuapiMemAlloc_common
  → CUmemdesc
  → memobjAlloc
  → memmgr / heap / suballocator
  → CUmemobj / CUmemblock
  → DMAL backing and mapping
  → device pointer
```

释放时，逻辑对象可以先从用户可查找结构移除，但物理 backing 是否立即回收取决于异步 tracking marker。不能把 API 返回成功解释成 backing 已经独立于后续 launch。

### 2.5 Stream 和 Channel

Stream 创建路径：

```text
cuStreamCreate
  → cuapiStreamCreate_common
  → cuiStreamCreate
      → lock streamPool->mutex
      → cuiStreamCreate_UnderLock
          → public handle
          → stream-pool object
          → optional QMD allocation
          → optional UVM prefetch semaphore
          → CPU semaphore
          → marker mutex and stream flags
          → priority and streamID
          → uvmStreamRegister
          → tools notification
      → unlock streamPool->mutex
```

`CUIstream` 保存 QMD、semaphore、marker、capture 状态和最后使用的 channel 关系。Channel 则是提交资源：

```text
channelInit
  → device->dmal.channelInitDmal
  → channel->dmal.Init
  → tracking/cross-engine semaphore
  → channel manager
```

## 3. Module 和 Function

```text
cuModuleLoad/cuModuleLoadData
  → module object
  → ELF parsing / optional PTX JIT
  → code/constant allocation
  → syscall import registration
  → function metadata

cuModuleGetFunction
  → module symbol lookup
  → CUfunc
  → parameter/register/shared-memory metadata
```

Function、code、constant bank 和 syscall 资源可能被异步 launch 使用。Module unload 必须服从 tracking 和 Context teardown 语义，不能简化成立即释放 code backing。

## 4. API 入口和分叉

```text
cuLaunchKernel
  → cuapiLaunchKernelCommon
  → cuiValidateLaunch
  → current Context / function / stream checks
  → cuiStreamIsCapturing(stream)
```

分支：

```text
capturing
  → 创建 Graph kernel node
  → 保存 function、参数、grid、stream、依赖
  → 不立即发布 GPFIFO

non-capturing
  → cuiLaunchKernel_nonreentrant
  → cuiLaunch
```

验证失败发生在提交前，因此正常情况下不会产生新的有效 GPFIFO 发布；capture 分支保存的是 graph 描述，不是已执行的设备命令。

## 5. 普通 launch 详细顺序

### 5.1 建立 launch 状态和引用

```text
cuiLaunchKernel_nonreentrant
  → Context/stream 约束下取得 launch state
  → 参数 packing / launchData
  → function 与 stream 资源引用
  → cuiLaunch
```

当 backend 要求 memory tracking 时，`doLaunchMemTracking` 构造 `CUmemTrackList`，可覆盖：

- function memory；
- Context texture header/sampler；
- local memory；
- vidmem heap；
- per-launch constant bank；
- syscall memory。

之后通过 `channelTrackMem` 将这组引用绑定到待提交的 channel/flush unit。

### 5.2 Setup 阶段

`cuiLaunchSetup_common` 的源码顺序是：

```text
local memory configuration
  → cuiSyscallOnKernelLaunch
  → tools launch callback
  → hal.launchCheck
  → constant-bank parameter setup
  → QMD setup
  → lmem state
  → cooperative-launch handling
  → hal.encodeAbiConstBankGridParams
  → hal.launchInitialize
```

这里的边界分工为：

| 层 | 负责内容 |
|---|---|
| CUI | 参数、Context、资源引用、工具回调、回滚 |
| Syscall | per-launch syscall 资源、imports、trampoline 关联 |
| HAL | 架构相关 ABI、QMD 字段和 launch packet |
| Stream/Channel | push 空间、tracking 和提交顺序 |

### 5.3 Pushbuffer 和 QMD

```text
streamBeginPushWithFlags
  → CUnvCurrent
  → CUnvchannel
  → pushbuffer space
  → pre-launch commands
  → QMD / launch state
```

随后：

```text
hal.launchFinalize
  → QMD semaphore initialization
  → profiler/perfmon patch if enabled
  → hal.launchControl
  → channelTrackQMD
  → streamEndPush
```

QMD 是设备执行描述；其分配和复用由 stream/graph 资源池管理，具体字段解释和 ABI 由架构 HAL 决定。Ampere 还会按 compute class 选择不同 QMD 编码版本，不能把架构名称直接当作 QMD ABI 名称。

### 5.4 GPFIFO 发布

通用路径：

```text
streamEndPush
  → gpfifoFlushGpfifoEntry
  → device->hal.setGpfifoEntry
  → channelFlushUnitFlushGPFIFO
  → channel->dmal.GpfifoAdvanceGpuPut
```

RM backend 的静态路径为：

```text
channelGpfifoAdvanceGpuPutRM
  → 遍历 pending GPFIFO entries
  → flush host pushbuffer cache
  → flush sysmem GPFIFO entry cache
  → optional IBM NPU generation sync
  → store fence
  → 计算 GPPut
  → device->hal.writeGPPut(channel, gpput)
  → 第二次 store fence
```

cache flush 和 fence 的对象是 CPU 可见的 pushbuffer/GPFIFO 内存；`writeGPPut` 是发布新的生产者位置，而非 GPU completion 证明。

## 6. 完成、同步和 reclaim

异步完成的抽象链：

```text
RM/firmware/GPU boundary
  → tracking semaphore / completion marker update
  → gpfifoAdvanceGpuGet
  → reclaim completed pushbuffer
  → release tracked memory
  → stream marker becomes completed
```

Stream 销毁状态机：

```text
ACTIVE
  → cuiStreamDetach_UnderLock
      → end/remove capture if necessary
      → uvmStreamUnregister
      → free public handle
      → release constant bank
      → abandon semaphores
      → qmdDeallocate
      → DETACHED
  → cuiStreamPoolMoveDetachedToFree
      → ctxMarkerGetStatus
      → CU_CTX_MARKER_COMPLETED_BY_GPU
      → cuiStreamReclaimDetached
      → FREE
```

因此 `cuStreamDestroy` 或 Context teardown 的 host-side detach 不代表 GPU 已经停止使用对应 QMD、pushbuffer 或 tracking memory。

## 7. 错误和回滚

| 阶段 | 失败含义 | 必须保留的语义 |
|---|---|---|
| API/handle validation | 尚未提交 | 不产生有效新命令 |
| syscall setup | launch 资源未完备 | 释放临时引用，不发布 launch |
| `hal.launchCheck` | ABI/硬件约束不满足 | 回滚 setup 状态 |
| `streamBeginPush` | 没有可用提交空间 | 不发布 GPFIFO |
| finalize/control | 已开始构造但未完成 | 关闭 push、释放临时资源 |
| GPFIFO/DMAL | host 提交失败 | 不伪造 completion，保留错误状态 |
| device execution | 已提交后的执行错误 | 通过 marker/trap/timeout 传播 |
| synchronize | 等待或设备错误 | 不提前 reclaim 未完成资源 |

`cuiLaunch` 的失败路径还需要处理 `streamEndPush`、UVM DAG rollback、memory tracking、profiler/perfmon 临时状态和 tools launch-end 通知。具体设备错误如何从 trap/firmware 传播到 API error，当前源码边界不足以完全证明。

## 8. Graph 分支

Graph capture 只保存节点。`cuiGraphInstantiate` 会执行：

```text
cycle detection
  → conditional validation
  → clone and flatten
  → memset node conversion
  → Context assignment
  → scheduling setup
  → count kernel/conditional/device nodes
  → collect and lock Contexts
  → cuiGraphInstantiate_UnderLock
  → register QMD semaphore pools
```

Graph launch 可以把 scheduling group 中的多个 kernel 节点合并到一个 push sequence；`cuiGraphLaunchKernelGroup` 仍然在 launch 时建立 memory tracking，因为 Context 中的可追踪对象可能在 instantiate 后发生变化。

Graph destroy 必须反向释放 graph stream、QMD、constant bank、scheduler backing、marker、tracking 和 Context lock data，并服从尚未完成的设备工作。

## 9. Syscall 边界

Host-side 可追踪流程：

```text
cuiSyscallInit
  → reason buffer
  → trampoline
  → module/function imports
  → cuiSyscallOnKernelLaunch
  → per-launch resource aggregation
  → cuiSyscallDestroy
```

以下行为属于当前 checkout 之外或无法独立验证的边界：

```text
cudaSyscalls.nvmk
  → compiler/device-code generation
  → bin2c/generated cubin
  → device trampoline entry
  → CNP queue
  → trap/firmware/reason-buffer consumption
```

## 10. 证据等级和验证计划

### 静态确认

- API → CUI 调用关系；
- 初始化、Context、Stream、Graph 的 host-side 状态变化；
- memory tracking、QMD、pushbuffer 和 GPFIFO 的调用顺序；
- HAL 函数表和架构选择；
- RM backend 的 cache flush、fence 和 GPPut 发布。

### 未验证/外部边界

- nvmake 真实构建；
- compiler/gpgpucomp/cubin 生成；
- RM 内核驱动和 firmware；
- GPU 消费 GPFIFO/QMD；
- UVM fault/migration；
- DVS、GPU、OpenCL、debugger、profiler 运行结果。

在拥有完整 NVIDIA 源码树、nvmake、目标 GPU 和权限后，才应补做构建和硬件验证；当前文档不把构建配方当作成功结果。
