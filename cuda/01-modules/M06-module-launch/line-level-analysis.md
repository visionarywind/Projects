# M06 行级分析

- `[src/cui/cuilaunch.c:164-218]`：收集 function、syscall、context、const bank、module/function-scope memobj 的 launch tracking。
- `[src/cui/cuilaunch.c:231-285]`：local memory、syscall callback、HAL launchCheck、const bank/QMD 和 tools setup。
- `[src/cui/cuilaunch.c:318-334]`：架构 ABI const-bank/grid 编码和 launchInitialize。
- `[src/cui/cuilaunch.c:419-503]`：context lock、launchCount、gridid、tools begin 和 setup。
- `[src/cui/cuilaunch.c:582-817]`：参数 finalize、push、HAL control、同步/错误回滚和 launch end。
- `[src/cui/cuimod.c:1835-1891]`：ELF image copy、shared ELF hash/refcount。
- `[src/cui/cuimod.c:2363-2548]`：JIT、module load transaction、UVM/syscall/CNP 顺序和失败卸载。
- `[src/cui/cuimod.c:2767-2943]`：module unload reverse path、shared ELF/syscall/UVM/tool 清理。
- `[src/cui/cuigraph.c:3304-3492]`：graph instantiate 的 clone/flatten/context lock/QMD pool registration。
- `[src/cui/cuigraph.c:4056-4162]`：graph launch 的动态 context lock、marker、UVM running 和 rollback。
- `[src/cui/cuistream.c:2180-2268]`：capture graph 创建、TLS/global mode 计数、unjoined/invalidation 检查和失败销毁。
- `[src/cui/cuigraph.c:1835-1933]`：per-context QMD、constant-bank、HAL staging 和 scheduler host/device backing。
- `[src/cui/cuigraph.c:1621-1809,2054-2141]`：kernel node resource slice、internal stream/marker 和 instantiate traversal。
- `[src/cui/cuigraph.c:3495-3575,3741-3880]`：kernel group memory tracking 与拓扑 node launch。
- `[src/cui/cuigraph.c:1035-1064,1093-1205]`：exec resource teardown、stream detach、QMD/constant-bank/marker/memobj 释放。
- `[src/api/apigraph.c:1332-1420]`、`[src/cui/cuigraph.c:4619-4915]`：graph exec update 的 topology 和参数约束。
- `[src/cui/cuigraph.c:3457-3492,4116-4157]`：instantiate 锁数组、launch API stream 恢复的静态风险点。
