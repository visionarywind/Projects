# M08 调用链、数据结构与接口

- 文档目的：解释 01-modules/M08-tools-debug/call-chains.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/cuda HEAD 39d4a83（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 01-modules/M08-tools-debug/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


## 观测调用链

```text
cuiLaunch
 → cuiToolsNotifyLaunchBegin
 → cuiLaunchSetup_common
 → cuiToolsNotifyLaunchAfterSyscallSetup
 → cuiToolsNotifyLaunchAfterFunctionSetup
 → streamBeginPushWithFlags
 → cuiToolsNotifyLaunchPushbufferBegin
 → hal.launchFinalize / launchControl
 → cuiToolsNotifyLaunchAfterLaunchPushed
 → streamEndPush
 → cuiToolsNotifyLaunchAfterGridLaunched
 → cuiToolsNotifyLaunchEnd
```

## Debugger attach 链

```text
debugger client
 → CUDBG_* shared control variables / RPC or IPC
 → cudbgApiCheck
 → halo device/context state
 → cudbg module/function/grid maps
 → device control/read/write/report
```

## 失败与收尾链

```text
launch begin callback
 → optional pSkipInternalLaunch / blocking decision
 → memcheck table/error-entry preparation
 → HAL push + profiler/perfmon window
 → launch end callback
 → marker/stream completion
 → context destroy
    → tool-owned device buffers/table teardown
    → debugger session/control cleanup
```

其中 callback 的 begin/end 是 host-side 生命周期通知；GPU completion 仍由 marker 或同步路径给出。`pSkipInternalLaunch` 只改变是否进行内部提交，不取消工具资源与错误回滚（静态确认：[src/cui/cuilaunch.c:468-503,779-817]；[src/devtools/memcheck/memcheck.c:120-256]）。

- `g_callbackEnabled[domain][cbid]`：callback 快速开关表。
- `CUtoolsLaunchBegin` 等 callback 参数：携带 context、stream、module、function、grid id 和可修改控制字段。
- `CUDBG_*` 全局协议变量：attach、session、错误报告、blocking 和调试能力协商。
- `CCmemcheckRecData`：record 级 internal module 与 memcheck 状态。
- `MCrec.errorEntry/deviceTable`：device error entry 与 allocation range table。
- `globals.profilerObj`：profile mode、enable 状态和 requested 状态。

## 错误/所有权

Tools callback 返回值和主 launch status 的关系必须按具体 callback ABI 核对；已确认的 launch begin skip 是显式控制通道，不能推断所有 callback 错误都会中止 launch。memcheck allocation failure 会返回错误并要求调用者走 record 初始化回滚。debugger API 在调用前执行模块/TLS/device 状态验证，避免对已销毁 context 访问。

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
