# M08 行级分析

- `[src/etbl/tools/tools_callbacks_internal.h:33-41]`：callback enable 快速路径与统一 dispatch。
- `[src/cui/cuilaunch.c:468-503]`：launch begin 后 tools 可跳过内部 launch。
- `[src/cui/cuilaunch.c:582-730]`：pushbuffer/HAL 前后 callback 和 perfmon 插入点。
- `[src/cui/cuilaunch.c:779-817]`：错误回滚、launch end 和 profiler 收尾。
- `[src/devtools/memcheck/memcheck.c:120-256]`：error entry、allocation table、host/device 镜像拷贝及失败释放。
- `[src/devtools/debugger/cudbgapi.c:401-557]`：module/context/function/grid 查询前的状态检查与 TLS callback inactive 闸门。
- `[src/devtools/debugger/cudbgdriver.c:103-210]`：attach/session、launch blocking、integrated memcheck、preemption debugging 的共享控制状态。
- `[src/profiler/profiler_common.c:21-56,85-96]`：profile mode mutex、兼容检查和 `CUPTI_PROFILE_MODE`。
- `[src/cui/cuilaunch.c:635-710]`：wait-for-idle、perfmon begin/end、PM trigger 及 counter 操作所在 push 区间。

这些行号支持 host-side 生命周期和错误分支；RM、设备 helper、客户端握手以及最终计数器结果仍是未知/未验证。
