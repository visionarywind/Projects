# M08 测试与验证

## 可静态核对的路径

- launch callback 的 begin/end 配对及 skip 行为可由 `cuilaunch.c` 分支核对（静态确认：[src/cui/cuilaunch.c:468-503,779-817]）。
- profiler mode 与 `CUPTI_PROFILE_MODE` 环境变量读取可由 profiler 源码核对（静态确认：[src/profiler/profiler_common.c:21-56,85-96]）。
- memcheck 的 device error entry/table 分配、下载和失败清理可由 `memcheck.c` 核对（静态确认：[src/devtools/memcheck/memcheck.c:120-180,206-256]）。

## 运行状态

debugger attach、integrated memcheck、profiler counter 和工具注入都需要外部客户端、驱动权限或 GPU；本项目未执行，结果统一为未验证。
