# M08 调用链、数据结构与接口

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
