# M02 调用链

- 文档目的：解释 01-modules/M02-runtime-context/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-runtime-context/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


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
