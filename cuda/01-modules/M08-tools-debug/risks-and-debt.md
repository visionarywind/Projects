# M08 风险与技术债

| 风险 | 静态依据 | 后果 |
|---|---|---|
| callback 递归/重入 | debugger 有 TLS callback inactive 检查 `[src/devtools/debugger/cudbgapi.c:487-495]` | 工具 callback 可能重入 driver |
| begin/end 不配对 | `cuiLaunch` 的显式契约 `[src/cui/cuilaunch.c:468-475,789-800]` | 工具看到错误的 launch 生命周期 |
| 工具改变 launch | `pSkipInternalLaunch` `[src/cui/cuilaunch.c:476-485,1044-1078]` | 主路径可能被跳过但仍需清理 |
| 额外 device 依赖 | memcheck error/table `[src/devtools/memcheck/memcheck.c:120-256]` | 工具资源可能早于 GPU 完成释放 |
| 外部协议不可见 | debugger RPC/IPC 变量 `[src/devtools/debugger/cudbgdriver.c:103-198]` | 无法静态确认 attach 与 RM 协议 |
| 工具资源与 context 销毁竞态 | memcheck device table/error entry `[src/devtools/memcheck/memcheck.c:120-256]` | 可能在异步 launch 完成前释放或遗留 device 资源 |
| profiler 全局模式与 launch counter 脱节 | mode mutex 与 push 内 perfmon 分属不同路径 `[src/profiler/profiler_common.c:21-96]`；`[src/cui/cuilaunch.c:635-710]` | host 状态完成不代表设备计数器完成 |

**深度等级：中→深（host-side）。** 主 launch callback、skip/blocking 控制、debugger 状态闸门、memcheck device 镜像和 profiler 插入点已覆盖；外部客户端协议、完整 memcheck patch/helper 生成链、DRS 运行时合并、具体 profiler backend 和设备端完成语义仍未闭合。
