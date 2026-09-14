# M08 设计与实现

## Callback 设计

`toolsCallbackEnabled` 读取每个 domain/cbid 的 volatile enable flag，关闭时避免进入通用 dispatch；打开后由 `toolsIssueCallback` 广播给已注册 callback（静态确认：[src/etbl/tools/tools_callbacks_internal.h:22-41]）。driver 用私有结构翻译 memory flags 为稳定的 tools descriptor，避免 tools 直接依赖内部 `nvtypes`（静态确认：[src/etbl/tools/tools_callbacks_internal.h:45-219]）。

## Launch 插入点

`cuiLaunch` 在 launch begin 后允许工具跳过内部提交；普通情况下先做 syscall/HAL/QMD/setup，再进入 pushbuffer begin、HAL finalize/control、pushbuffer end 和 stream end push；最终始终调用 launch end（静态确认：[src/cui/cuilaunch.c:468-503,582-624,666-730,779-817]）。这形成严格的 begin/end 配对契约，即使中途失败也需经过 `EndPushAndExit`/`Exit`。

## Debugger

debugger API 维护 module/context/function/grid 的映射；其 API check 将依赖标志逐级展开：device、status、suspended、SM、warp、lane、TLS callback 和 initialized 检查按层叠加（静态确认：[src/devtools/debugger/cudbgapi.c:401-557]）。attach 相关共享变量同时承担 debugger 与应用进程之间的控制协议，例如 launch blocking、integrated memcheck、preemption debugging 和 session id（静态确认：[src/devtools/debugger/cudbgdriver.c:103-198]）。

## Memcheck

memcheck 为 error entry 分配 pinned host/device 可见内存；全局 allocation table 在 host 填充后通过 inline HtoD copy 下载到 device，失败分别映射为 host/device memory 错误（静态确认：[src/devtools/memcheck/memcheck.c:120-256]）。因此 memcheck 的资源也有 context、stream 和 device memory 生命周期，不能只看成日志功能。

## Profiler

全局 profiler mode 由 mutex 保护；profile mode 允许“同模式或 none”兼容，初始化可读取 `CUPTI_PROFILE_MODE`（静态确认：[src/profiler/profiler_common.c:21-56,85-96]）。launch 中的 perfmon begin/end、wait-for-idle、PM trigger 和 counter start/stop 位于 HAL push 期间（静态确认：[src/cui/cuilaunch.c:635-710]）。
