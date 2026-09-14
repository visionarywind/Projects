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

## 资源回滚与销毁

工具资源不是独立于 context 的日志缓存。memcheck 为 error entry 和 allocation table 建立 host/device 对，先在 host 填充，再通过 inline HtoD copy 同步 device 镜像；任一阶段失败都要释放已建立的 entry/table 并返回对应内存错误（静态确认：[src/devtools/memcheck/memcheck.c:120-180,206-256]）。因此 context destroy、allocation teardown 和 launch failure 必须检查工具侧仍持有的 device-visible 资源。

debugger 的 shared control variables 连接 attach/session 与 launch blocking 等协议状态；API check 通过 TLS callback inactive 条件防止 callback 重入。静态源码能确认状态闸门和字段用途，但不能确认另一进程何时更新这些变量、RPC/IPC 如何完成握手（静态确认：[src/devtools/debugger/cudbgapi.c:401-557]；[src/devtools/debugger/cudbgdriver.c:103-210]）。

profiler 的全局模式与 launch 内 perfmon 操作由不同层次管理：前者决定是否接受 profile client，后者插入具体提交窗口。故 callback end 或 host API 返回不等价于 counter 已停止，最终完成仍依赖 stream/marker 路径；这部分设备端时序未知。
