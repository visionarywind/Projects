# M02 实现：初始化与 Context 生命周期

- 文档目的：解释 01-modules/M02-runtime-context/implementation.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M02-runtime-context/implementation.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 全局初始化

`cuiInitInternal` 先保证 TLS（Windows 由 DllMain 初始化，其他平台在此处初始化），再初始化性能状态、globals、portable memory manager、平台属性、UVM、UVA、user VA、第三方 P2P、host VA heap、stream/graph ID 和 primary memory managers（静态确认：[src/cui/cuiinit.c:3060-3199]）。任何步骤失败跳到统一 `Error`，按 primary memmgr → user VA → UVA → UVM → globals 的反向顺序销毁（静态确认：[src/cui/cuiinit.c:3201-3208]）。

`cuiInit` 在 `globals.initMutex` 下做 already-initialized 快速路径；这说明初始化幂等性由 mutex 和状态共同保证（静态确认：[src/cui/cuiinit.c:3225-3246]）。

## 初始化检查

`cuiInitCheckEx` 首先拒绝禁止 API 调用的线程，再区分 `DESTROYED`、`NOT_INITIALIZED`；若调用者要求 context，则从 TLS 取 current context，调用 `cuiInitCheckCtx`，最后把 context 返回给调用者（静态确认：[src/cui/cuiinit.c:2910-2961]）。`cuiInitCheckCtx` 检查 active state、amode1 特例、context API 版本，并在非豁免模式读取 sticky error（静态确认：[src/cui/cuiinit.c:2979-3039]）。

## Context 创建/销毁

创建参数初始化会验证 flags/scheduling flags，并安装 memcpy、memblock、function memory-range、local-memory 回调，同时合并 tools 强制 flags（静态确认：[src/cui/cuictx.c:240-286]）。`cuiCtxCreate` 分配 context、设置 attachRefCount，必要时 retain primary context，再初始化；失败把输出置 NULL（静态确认：[src/cui/cuictx.c:294-359]）。销毁路径的调用者必须先处理 TLS pop，CUI 自身完成 finalize/deinitialize 和 primary release（静态确认：[src/cui/cuictx.c:362-385]）。

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
