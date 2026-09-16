# 调试指南

## 先分类

- 库加载/符号：M01/M02/export
- 初始化/无设备：M03
- 无 current context/句柄错误：M02/M04
- 地址、泄漏、异步 free：M05
- 卡住/顺序/完成错误：M06/M09
- 参数/模块加载：M07
- capture/update/拓扑：M08

## 证据采集

保留 commit、配置、设备 ordinal、context/stream id、API/correlation/submission id、MUresult 和同步点。不要用未执行命令的预期输出替代日志。

## 静态入口

`mu_entry.cpp` 暴露了 profiler/debugger/accessor 表，可用于关联 Command、Memory、GraphNode 和时间戳。[src/driver/mu_entry.cpp:1085-1249]
