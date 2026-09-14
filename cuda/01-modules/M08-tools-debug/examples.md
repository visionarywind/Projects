# M08 代表性例子

- tools 在 `LAUNCH_BEGIN` 设置 skip：driver 不提交内部 launch，但仍必须经过 launch-end 观察和状态清理。
- debugger 设置 launch blocking：异步 launch 可能转成同步等待，错误观察点改变。
- memcheck 为 allocation table 分配 device memory：工具本身增加 M04/M05 资源依赖。
