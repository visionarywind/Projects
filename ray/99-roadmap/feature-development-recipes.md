# 功能开发配方

- 对应源码版本：HEAD `cfe4725d23`。
- 证据状态：静态配方；命令结果未验证。

## 结论摘要

跨公共 API、任务选项、对象、RPC 或调度的功能必须先画影响图，再改源码和测试。

## 建议

1. 找稳定入口与真实副作用。
2. 更新 Python/C++/Java/proto 契约。
3. 增加正常、分支、错误、清理测试。
4. 更新文档/BUILD/CI 路由。
5. 运行最小相关测试并记录结果。

## 相关文档
[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`source/ray/README.rst:17-47`；`source/ray/python/ray/__init__.py:80-130`；`source/ray/python/ray/remote_function.py:355-574`；`source/ray/src/ray/core_worker/core_worker_process.cc:231-285`。

## 未解决问题
动态进程、网络、GPU、重试和性能结论需要构建/运行日志；当前只保留静态证据或明确标注推断。

## 下一步阅读建议
从 M01→M05 和 D01 继续补齐精确符号、测试和运行证据。
