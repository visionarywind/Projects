# M09 行级分析

- `[src/icd_rel/icd.c:43-51]`：全局 vendor list 和一次性初始化入口。
- `[src/icd_rel/icd.c:63-208]`：动态库、extension、platform IDs、suffix、vendor node 和临时资源清理。
- `[src/icd_rel/icd_dispatch.c:43-90]`：platform IDs 参数校验、输出清零和 vendor 枚举。
- `[src/cl/cliglobals.c:58-118]`：CUI 初始化、配置、dispatch/platform 创建和失败清零。
- `[src/cl/cliicd.c:24-144,146-235]`：固定 dispatch table 与版本/OS 条件入口。
