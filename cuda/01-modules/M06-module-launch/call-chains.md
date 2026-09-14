# M06 调用链、接口与数据结构

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
