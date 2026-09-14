# M02 实现：初始化与 Context 生命周期

## 全局初始化

`cuiInitInternal` 先保证 TLS（Windows 由 DllMain 初始化，其他平台在此处初始化），再初始化性能状态、globals、portable memory manager、平台属性、UVM、UVA、user VA、第三方 P2P、host VA heap、stream/graph ID 和 primary memory managers（静态确认：[src/cui/cuiinit.c:3060-3199]）。任何步骤失败跳到统一 `Error`，按 primary memmgr → user VA → UVA → UVM → globals 的反向顺序销毁（静态确认：[src/cui/cuiinit.c:3201-3208]）。

`cuiInit` 在 `globals.initMutex` 下做 already-initialized 快速路径；这说明初始化幂等性由 mutex 和状态共同保证（静态确认：[src/cui/cuiinit.c:3225-3246]）。

## 初始化检查

`cuiInitCheckEx` 首先拒绝禁止 API 调用的线程，再区分 `DESTROYED`、`NOT_INITIALIZED`；若调用者要求 context，则从 TLS 取 current context，调用 `cuiInitCheckCtx`，最后把 context 返回给调用者（静态确认：[src/cui/cuiinit.c:2910-2961]）。`cuiInitCheckCtx` 检查 active state、amode1 特例、context API 版本，并在非豁免模式读取 sticky error（静态确认：[src/cui/cuiinit.c:2979-3039]）。

## Context 创建/销毁

创建参数初始化会验证 flags/scheduling flags，并安装 memcpy、memblock、function memory-range、local-memory 回调，同时合并 tools 强制 flags（静态确认：[src/cui/cuictx.c:240-286]）。`cuiCtxCreate` 分配 context、设置 attachRefCount，必要时 retain primary context，再初始化；失败把输出置 NULL（静态确认：[src/cui/cuictx.c:294-359]）。销毁路径的调用者必须先处理 TLS pop，CUI 自身完成 finalize/deinitialize 和 primary release（静态确认：[src/cui/cuictx.c:362-385]）。
