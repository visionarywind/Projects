# M06 调用链、接口与数据结构

- 文档目的：解释 01-modules/M06-module-launch/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M06-module-launch/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## Module / ELF / JIT

```text
cuiModuleLoadInternalAndLink
 → cuiModuleLoadDataEx
 → cuiDeviceCodeObtainCubin
 → cuiModInit
 → cuiModuleLoadBinary
 → cuiElfModuleLoad (ELF32/ELF64 dispatch)
 → cuiModuleProcessFunctions / cuiFuncOnModuleLoad
 → cuiSyscallOnModuleLoad
 → external text relocation patch
 → cuiFuncBulkDownload
 → optional binary deduplication
```

共享 ELF 由 `globals.binload_cs` 和 `CUsharedelf.ref_count` 管理；卸载同时撤销 function/ref/symbol/memobj/syscall/UVM/tool 状态，最后释放 module（静态确认：[src/cui/cuimod.c:1835-1891,2767-2943]）。

## Graph exec

```text
cuiGraphInstantiate
 → cycle + conditional validation
 → clone/flatten/memset conversion
 → context assignment + scheduling
 → per-context resource counting
 → lock all context internalsMutex
 → cuiGraphInstantiate_UnderLock
 → QMD semaphore-pool registration

cuiGraphLaunch
 → lock graph contexts (+ launch-only context)
 → first-launch per-context setup
 → wait prior completion/texture updates
 → cuiUvmDagSetRunning
 → topological cuiGraphNodeLaunch loop
 → wait current completion markers
 → UVM rollback on failure
 → unlock + tools end notification
```

静态确认：[src/cui/cuigraph.c:3304-3492,4056-4162]。

## Graph 资源分配与释放

```text
cuiGraphInstantiate
 → per-context resource counts
 → const-bank acquire + QMD array
 → scheduler host backing + driver memobj（有 device nodes）
 → internal stream + completion marker per node
 → launch setup / HAL staging

cuiGraphExecDestroy
 → propagate completion QMD（已 launch）
 → lock all graph contexts
 → destroy nodes and completion markers
 → detach internal streams
 → qmdDeallocate + const-bank release
 → memobjFree(scheduler backing) + host/HAL staging free
 → release locks and clone links
```

资源分配集中在 `allocateExecutionResourcesFunctor`，反向清理由 `destroyCtxDataFunctor` 与 `cuiGraphDestroy` 完成（静态确认：[src/cui/cuigraph.c:1835-1933,1035-1064,1144-1205]）。

## 普通 launch

```text
cuLaunchKernel
 → cuapiLaunchKernel
 → cuapiLaunchKernelCommon
 → cuiFuncInitCheck + cuiStreamInitCheckWithFlags
 → cuiLaunchKernel_nonreentrant
 → cuiFuncValidateParams
 → cuiLaunchSetBlockShape / SetSharedSize
 → cuiProfilerLaunch
 → cuiLaunchSetup_common
 → hal.launchCheck
 → const-bank + QMD + ABI encode
 → stream/channel push (M05)
```

## Capture launch

```text
cuLaunchKernel
 → common checks
 → cuiStreamIsCapturing
 → graph lock
 → cuiGraphNodeValidateKernelParams
 → cuiGraphCreateKernelNode
 → cuiStreamUpdateCaptureInfo
```

## 数据结构

- `CUmod`：module 链表和 function 列表的所有者。
- `CUfunc`：function 镜像、grid id、参数元数据、launchCount、持久 launch 状态。
- `CUIlaunchData`：grid/block/shared-memory/params/QMD 的通用中间表示。
- `CUIkernelParams`：unpacked/packed buffer、size、packed 标志。
- `CUIgraphNode`/`CUIgraph`：capture 节点、依赖和 graph 生命周期。

前向声明见：[src/cuda_types.h:45-52,150-163]。

## 接口契约

- `cuiFuncInitCheck` 必须验证 function handle 和 context；不能只用非空指针。
- `cuiLaunchSetup_common` 的调用者需要持有 context lock（具体断言/调用约束需继续查全）。
- launch memory tracking 的引用必须覆盖到异步完成边界；否则 free/stream detach 可能早于 GPU 使用。
- graph exec destroy 不能只释放节点结构；必须按 QMD/constant-bank/stream/marker/scheduler memobj 的反向顺序清理，并保留 context lock 保护。
- HAL 只提供架构特化操作，不能由接口名称推断 QMD 字段或 ABI 编码完全相同。

## Module unload reverse path

```text
cuiModuleUnloadEx
 → remove from context module list
 → syscall synchronous flush / CNP QMD cache cleanup
 → destroy function and module-scope resource lists
 → release shared ELF under binload_cs
 → decrement syscall import refcounts
 → unregister UVM managed namespace / atomics
 → unregister timeout ISR and deactivate trap when unused
 → tools unload notification
 → free CUmod
```

正常卸载与 load-failure 卸载共用该 reverse path，但 `wasLoadedSuccessfully` 会影响 tools 通知。ELF image 只有 shared refcount 归零时才真正释放；syscall import refcount 归零时才清除 active bit（静态确认：[src/cui/cuimod.c:2767-2943]）。

## Graph capture / instantiate / launch 链

```text
cuStreamBeginCapture
 → cuapiStreamBeginCaptureCommon
 → cuiStreamBeginCapture
 → cuiGraphCreate
 → stream.capture.graph = graph

cuLaunchKernel (capturing)
 → cuiGraphLockForCapture
 → cuiGraphNodeValidateKernelParams
 → cuiGraphCreateKernelNode(deps)
 → cuiStreamUpdateCaptureInfo
 → unlock

cuStreamEndCapture
 → origin/thread/invalidation/join validation
 → remove participating streams/events
 → graph handle or graph destroy

cuGraphInstantiate
 → cuiGraphInstantiate
 → cycle + conditional validation
 → cuiGraphCloneExec
 → cuiGraphFlatten + cuiGraphConvertMemsetNodes
 → context registration + cuiGraphSetupScheduling
 → cuiGraphInstantiate_UnderLock
 → qmdRegisterSemaphorePoolIfNeeded

cuGraphLaunch
 → cuapiGraphLaunchCommon
 → cuiGraphLaunch
 → wait previous per-context markers
 → cuiUvmDagSetRunning
 → cuiGraphNodeLaunch in topological list
 → marker waits / pushbuffer / memcpy / callback / scheduler
 → wait current graph completion markers
```

capture 中的 event wait 不会把另一个 graph 直接拼接进来：跨 graph capture 会使相关 capture invalidated；同一 graph 的 captured event dependencies 被放入 stream 的 `nextPushDeps`，供下一个操作创建节点时消费（静态确认：[src/api/apistream.c:480-520,src/api/apistream.c:538-667]；[src/cui/cuistream.c:1223-1272]）。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
