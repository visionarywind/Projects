# M03 行级分析

- `[src/cui/devmgr.c:39-72]`：建立 identity remap table。
- `[src/cui/devmgr.c:75-121]`：验证 filter/range/duplicate，决定可见设备。
- `[src/cui/devmgr.c:201-237]`：以 TPC、SM、架构 multiplier 等计算排序速度。
- `[src/cui/hal/hal.c:35-79]`：编译条件包围的 `ccArch` switch 调用具体 init。
- `[src/cui/channel.c:43-173]`：DMAL 初始化、同步对象、manager 插入和失败清理。
