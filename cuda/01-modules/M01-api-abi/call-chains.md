# M01 调用链与证据

- 文档目的：解释 01-modules/M01-api-abi/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M01-api-abi/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
