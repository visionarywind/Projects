# 技术债

1. `mu_wrappers_generated.cpp` 的生成来源和 export 生成链需要专门说明。
2. HAL/M3D 的 queue、cmdBuffer、memory manager 仍缺完整实现证据。
3. Graph 拓扑、update、conditional、child graph 尚未形成完整 Demo 轨迹。
4. `mu_error.cpp` 和异步错误映射尚未统一索引。
5. 测试文件已登记但尚未逐个映射到 CTest/CI。
6. 当前文档使用静态源码证据，缺目标机运行日志和性能基线。
