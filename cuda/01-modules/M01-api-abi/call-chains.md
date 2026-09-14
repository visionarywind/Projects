# M01 调用链与证据

## 链 1：初始化

```text
cuInit
  → cuapiInit                         [src/api/apiinit.c:19-47]
  → cuiGlobalMutexInitOnce            [src/cui/cuiinit.c:2814-2870]
  → cuiInit(CUI_API_CUDA)             [src/cui/cuiinit.c:3225-3313]
  → cuiInitInternal                   [src/cui/cuiinit.c:3060-3208]
  → globals/UVM/UVA/primary memmgr
```

正常返回 `CUDA_SUCCESS`；非零 Flags 在进入 CUI 前返回 `CUDA_ERROR_INVALID_VALUE`。

## 链 2：设备内存

```text
cuMemAlloc_v2
  → cuapiMemAlloc_v2                  [src/api/apimem.c:129-136]
  → cuapiMemAlloc_common              [src/api/apimem.c:52-118]
  → cuiInitCheckEx / cuiCheckStreamCaptureInteraction
  → memobjAlloc(ctx->memmgr, CUmemdesc)
  → memglobalsRegisterMemobj
  → cuiMemToolsNotifyAlloc
  → memobjGetDevicePtr
```

`memglobalsRegisterMemobj` 失败时执行 `memobjFree` 回滚；这是已确认的异常路径。

## 链 3：Kernel launch

```text
cuLaunchKernel
  → cuapiLaunchKernel                  [src/api/apilaunch.c:304-323]
  → cuapiLaunchKernelCommon            [src/api/apilaunch.c:212-301]
  → cuiFuncInitCheck / cuiStreamInitCheckWithFlags
  → capture: cuiGraphCreateKernelNode
  → normal: cuiLaunchKernel_nonreentrant
  → cuiProfilerLaunch
  → cuiLaunchSetup_common / stream push
```

`cuiLaunchSetup_common` 后续 HAL/DMAL 动态分派仍需按目标配置继续核对；当前证据只能确认达到 `launchCheck`、ABI 编码和资源提交准备。

## 链 4：Stream

```text
cuStreamCreate
  → cuapiStreamCreate                  [src/api/apistream.c:113-136]
  → cuapiStreamCreate_common            [src/api/apistream.c:66-109]
  → cuiStreamCreate
  → cuiStreamCreate_UnderLock
  → stream pool/QMD/semaphore/UVM register
```

QMD semaphore pool 的 portable 登记放在释放 context lock 后执行，失败则 detach（静态确认：[src/api/apistream.c:83-109]）。
