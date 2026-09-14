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

## 数据结构

- `g_callbackEnabled[domain][cbid]`：callback 快速开关表。
- `CUtoolsLaunchBegin` 等 callback 参数：携带 context、stream、module、function、grid id 和可修改控制字段。
- `CUDBG_*` 全局协议变量：attach、session、错误报告、blocking 和调试能力协商。
- `CCmemcheckRecData`：record 级 internal module 与 memcheck 状态。
- `MCrec.errorEntry/deviceTable`：device error entry 与 allocation range table。
- `globals.profilerObj`：profile mode、enable 状态和 requested 状态。

## 错误/所有权

Tools callback 返回值和主 launch status 的关系必须按具体 callback ABI 核对；已确认的 launch begin skip 是显式控制通道，不能推断所有 callback 错误都会中止 launch。memcheck allocation failure 会返回错误并要求调用者走 record 初始化回滚。debugger API 在调用前执行模块/TLS/device 状态验证，避免对已销毁 context 访问。
