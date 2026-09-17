# 技术债务

- 对应源码版本：HEAD `cfe4725d23`。
- 证据状态：静态配方；命令结果未验证。

## 结论摘要

当前知识库技术债务是 Core 精确链、动态运行证据和测试映射不足；不把未知伪装成已完成。

## 建议

1. 补 `_raylet` 到 C++ 符号链。
2. 补 M03-M07 状态/所有权/错误/清理。
3. 为 Data/Train/Serve 选择真实 Demo。
4. 运行最小单机验证并记录日志。

## 相关文档
[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`source/ray/README.rst:17-47`；`source/ray/python/ray/__init__.py:80-130`；`source/ray/python/ray/remote_function.py:355-574`；`source/ray/src/ray/core_worker/core_worker_process.cc:231-285`。

## 未解决问题
动态进程、网络、GPU、重试和性能结论需要构建/运行日志；当前只保留静态证据或明确标注推断。

## 下一步阅读建议
从 M01→M05 和 D01 继续补齐精确符号、测试和运行证据。
