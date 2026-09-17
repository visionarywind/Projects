# 跨模块调用链

- 文档目的：按真实源码函数和资源状态审计 CUDA Driver 的跨模块调用链。
- 适用范围：`source/cuda` 当前源码快照；引用路径相对于 `source/cuda`。
- 证据状态：调用关系、host-side 状态和回滚为静态源码确认；RM/NVRM、firmware、GPU 消费和运行测试未验证。
- 前置阅读：[项目入口](../README.md)、[分析状态](../00-overview/analysis-state.md)、[Kernel Launch 端到端调用链](end-to-end-kernel-launch.md)。

## 1. 统一主线

```text
cuInit
  → cuapiInit
  → cuiInit / cuiInitInternal
  → globals / TLS / UVM / UVA / primary memmgr

cuCtxCreate_v2
  → cuapiCtxCreate_common
  → cuiCtxCreate
  → cuiCtxAllocate / cuiCtxInitialize
  → TLS current Context

cuMemAlloc_v2
  → cuapiMemAlloc_common
  → CUmemdesc
  → memobjAlloc
  → memmgr/suballocator/DMAL backing

cuStreamCreate
  → cuapiStreamCreate_common
  → cuiStreamCreate
  → stream pool / QMD / semaphore / marker

cuModuleLoad + cuModuleGetFunction
  → ELF/JIT/module registry
  → CUfunc metadata / syscall imports

cuLaunchKernel
  → cuapiLaunchKernelCommon
  → cuiValidateLaunch
  → capture Graph node OR cuiLaunchKernel_nonreentrant
  → cuiLaunch
  → cuiLaunchSetup_common
  → syscall + HAL + QMD + memory tracking
  → stream push / GPFIFO / DMAL backend
  → marker completion / reclaim
```

## 2. 跨模块审计表

| 链路 | 入口 | 正常路径 | 关键分支 | 异常/回滚 | 完成/清理 | 证据 |
|---|---|---|---|---|---|---|
| 初始化 | `cuapiInit` | `cuiInit`→`cuiInitInternal` | 已初始化早退、tools injection | 逆序销毁 globals/managers | atfork/全局 teardown | `src/api/apiinit.c`; `src/cui/cuiinit.c` |
| Context | `cuapiCtxCreate_common` | allocate→initialize→TLS push | primary context 路径 | `cuiCtxDestroy` | finalize/deinitialize/release | `src/api/apictx.c`; `src/cui/cuictx.c` |
| Device/HAL | device init | `InitDeviceArch`→`CUdev->hal` | 架构和 compute class | feature/arch unsupported | device/platform teardown | `src/cui/hal/hal.c`; `src/cui/hal/*` |
| Memory | `cuMemAlloc_v2` | desc→memobj→memblock→DMAL | pool/suballocation/UVA/UVM | registration/backing rollback | tracking 后释放/coalesce | `src/api/apimem.c`; `src/cui/memobj.c`, `memblock.c` |
| Stream | `cuStreamCreate` | pool→QMD→semaphore→marker | null/barrier/capture/priority | detach partial state | marker 完成后 reclaim | `src/cui/cuistream.c` |
| Channel | Context/stream init | `channelInit`→DMAL Init | RM/MRM/WDDM/MPS | backend deinit/semaphore cleanup | flush completion | `src/cui/channel.c`; `src/cui/dmal/*` |
| Module | module load | ELF/JIT→module/function | PTX/JIT/shared ELF | unregister/release | outstanding launch 后卸载 | `src/cui/cuimod.c`, `cuifunc.c` |
| Launch | `cuLaunchKernel` | setup→push→GPFIFO | graph capture、tools skip/block | push/UVM/tracking rollback | QMD/memory reclaim | `src/api/apilaunch.c`; `src/cui/cuilaunch.c` |
| Graph | graph instantiate/launch | clone/flatten/schedule→group push | conditional/device scheduler | destroy partial exec | graph marker/stream teardown | `src/cui/cuigraph.c` |
| Syscall | module/launch | init→imports→OnKernelLaunch | per-arch generated code | refcount/trampoline cleanup | reason buffer/trampoline destroy | `src/cui/cuisyscall.c`; `cudaSyscalls.nvmk` |
| Completion | synchronize/reclaim | marker/semaphore→get/reclaim | sync/async、timeout | sticky/device error | detached→free | `src/cui/cuistream.c`; `gpfifo.c` |

## 3. 一次普通 launch 的跨模块数据流

```text
用户参数
  → CUIlaunchData
  → CUfunc 参数布局/constant bank
  → CUmemTrackList
  → HAL 编码 QMD 和 launch packet
  → CUnvCurrent / pushbuffer
  → CUgpfifoEntry
  → DMAL flush item
  → GPPut
  → tracking marker
```

对象引用关系：

```text
Context
 ├─ Memory manager → CUmemobj → CUmemblock/backing
 ├─ Stream manager → CUIstream → CUqmd/semaphore/marker
 ├─ Module registry → module → CUfunc/code/constant/syscall
 └─ Channel manager → CUnvchannel → GPFIFO/pushbuffer/flush unit
```

## 4. 入口、锁和执行上下文

- API wrapper：用户线程；负责参数、句柄、版本和 current TLS Context 检查。
- CUI Context：通常在 Context lock 约束下修改共享对象；不是 GPU 执行线程。
- Stream pool：`streamPool->mutex` 保护 `FREE/ACTIVE/DETACHED` 状态。
- Graph：capture/instantiate 可能需要 graph lock 和多个 Context lock。
- HAL：同步执行 host-side 编码函数，不拥有上层对象。
- DMAL/RM：执行平台提交和 cache/fence 操作；设备消费在源码外部。

## 5. 关键分支

### Capture

```text
cuiValidateLaunch
  → cuiStreamIsCapturing
  → 创建 Graph node
  → 保存参数和依赖
  → 不进入 pushbuffer/GPFIFO
```

### Tools

launch callback 可以引入 skip、blocking、profiler 或 memcheck 相关旁路；这些是 CUI launch 状态的一部分，不应被误写为普通 launch 必经路径。

### Backend

`channelFlushUnitFlushGPFIFO` 通过 DMAL 函数表进入 RM、MRM、WDDM 等不同实现。某些同步或 external semaphore 能力由 backend 明确返回 not supported，不能用 RM 的路径推断所有平台。

## 6. 异常和资源清理

```text
validation error
  → 不产生有效提交

setup/HAL error
  → 释放临时 QMD/constant/tracking state

push error
  → 关闭或回滚 push，不能发布 completion

DMAL/RM error
  → 保存 sticky error，不伪造 GPU 完成

device/trap/timeout
  → 通过 marker/trap/reason-buffer 边界传播

stream destroy
  → detach → marker 查询 → reclaim → FREE
```

## 7. 证据边界

静态源码能闭合到：

- API→CUI 的函数调用；
- CUI 对象之间的资源引用；
- HAL 函数表和架构选择；
- pushbuffer/GPFIFO/DMAL 的 host-side 提交；
- marker 触发的 deferred reclaim。

不能由当前树单独闭合：

- RM 内核驱动如何处理 GPPut；
- firmware/GPU 如何消费 QMD；
- compiler/assembler/cubin 的实际生成；
- GPU 上的 fault、timeout、completion latency；
- DVS、GPU、OpenCL、debugger、profiler 实际运行结果。

## 8. 关联文档

- [Kernel Launch 端到端调用链](end-to-end-kernel-launch.md)
- [具体架构设计](architecture-design.md)
- [跨模块接口契约](interface-contracts.md)
- [运行时轨迹](runtime-trace.md)
- [源码证据索引](../00-overview/evidence-index.md)
