# M08 风险与技术债

| 风险 | 静态依据 | 后果 |
|---|---|---|
| callback 递归/重入 | debugger 有 TLS callback inactive 检查 `[src/devtools/debugger/cudbgapi.c:487-495]` | 工具 callback 可能重入 driver |
| begin/end 不配对 | `cuiLaunch` 的显式契约 `[src/cui/cuilaunch.c:468-475,789-800]` | 工具看到错误的 launch 生命周期 |
| 工具改变 launch | `pSkipInternalLaunch` `[src/cui/cuilaunch.c:476-485,1044-1078]` | 主路径可能被跳过但仍需清理 |
| 额外 device 依赖 | memcheck error/table `[src/devtools/memcheck/memcheck.c:120-256]` | 工具资源可能早于 GPU 完成释放 |
| 外部协议不可见 | debugger RPC/IPC 变量 `[src/devtools/debugger/cudbgdriver.c:103-198]` | 无法静态确认 attach 与 RM 协议 |

**深度等级：中等。** 主 launch callback 和工具资源边界已覆盖；外部客户端协议、完整 memcheck patch 生成链、DRS 运行时合并和具体 profiler backend 未闭合。
