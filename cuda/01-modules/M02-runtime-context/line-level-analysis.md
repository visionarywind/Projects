# M02 行级分析

- `[src/cui/cuiinit.c:2910-2940]`：先检查线程能否调用 API，再检查 globals initialized。
- `[src/cui/cuiinit.c:2950-3039]`：获取 TLS current context，检查 active/API version/sticky error。
- `[src/cui/cuiinit.c:3060-3208]`：初始化资源按依赖顺序创建，Error 标签执行逆序销毁。
- `[src/cui/cuictx.c:240-305]`：context 参数初始化合并 flags、device 和 API version。
- `[src/cui/cuictx.c:306-405]`：创建失败清理，destroy 释放 context 与 primary TSG。
