# M08 行级分析

- `[src/etbl/tools/tools_callbacks_internal.h:33-41]`：callback enable 快速路径与统一 dispatch。
- `[src/cui/cuilaunch.c:468-503]`：launch begin 后 tools 可跳过内部 launch。
- `[src/cui/cuilaunch.c:582-730]`：pushbuffer/HAL 前后 callback 和 perfmon 插入点。
- `[src/cui/cuilaunch.c:779-817]`：错误回滚、launch end 和 profiler 收尾。
- `[src/devtools/memcheck/memcheck.c:120-256]`：error entry、allocation table 与 HtoD 下载。
