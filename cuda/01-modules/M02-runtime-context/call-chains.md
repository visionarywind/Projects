# M02 调用链

## 初始化链

```text
cuapiInit [src/api/apiinit.c:19-47]
 → cuiGlobalMutexInitOnce
 → cuiInit [src/cui/cuiinit.c:3225-3313]
 → globals.initMutex / cuiInitInternal
 → cuiTlsInitialize
 → globalsInit → memglobalsCreate → uvmManagerCreate
 → uvaManagerCreate → userdVaManagerCreate
 → cuheapCreate → memglobalsCreatePrimaryMemmgrs
```

## Context 链

```text
cuapiCtxCreate_v2
 → cuapiCtxCreate_common [src/api/apictx.c:37-95]
 → cuiTlsGetCurrentThreadData / cuiInitCheckEx
 → cuiCtxCreateParamsInit [src/cui/cuictx.c:240-286]
 → cuiCtxCreate [src/cui/cuictx.c:294-359]
 → cuiCtxInitialize
 → cuiTlsPushContext
```

销毁链为 `cuapiCtxDestroy_common` → `cuiCtxDestroy` → finalize → deinitialize → primary release；legacy/v2 attach 引用规则由 wrapper 额外检查（静态确认：[src/api/apictx.c:97-157]）。

## 错误链

```text
异步 channel/context error
 → ctx sticky error
 → 下一次 cuiInitCheckCtx
 → cuiCtxCheckError(CUI_CTX_CHECK_STICKY_ONLY)
 → CUresult 返回 API
```

这一链的“错误产生点”在当前树中分散于 channel/DMAL；本页确认的是观察点，不声称定位了所有产生点。
