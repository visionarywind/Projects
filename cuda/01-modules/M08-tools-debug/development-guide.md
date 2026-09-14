# M08 开发指南

1. 新 hook 同步 callback ABI、enable table、参数 packing 和编译宏。
2. 在 launch 中加入 callback 时保证 begin/end、push begin/end 和 error cleanup 配对。
3. debugger 修改必须更新 TLS recursion、device suspended/status 和 context map 检查。
4. profiler 修改必须核对 mutex、concurrency、wait-for-idle 和 HAL perfmon。
5. memcheck 修改必须覆盖额外 memory 的 tracking、context destroy 和架构 interop。
