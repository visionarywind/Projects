# 共享数据与类型

- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：静态代表路径；运行/性能未验证。

## 结论摘要

ObjectRef/ID、task options、resource maps、function descriptors 和 protobuf message 是跨模块共享类型；它们有语言/ABI/序列化边界。

## 关系/流程

| 类型 | 跨越 | 证据 |
|---|---|---|
| ObjectRef/IDs | Python↔C++↔store | `__init__.py:87-104` |
| descriptor/pickle | Driver↔Worker/GCS | `remote_function.py:391-415` |
| task options | API↔Core/Raylet | `remote_function.py:421-556` |

## 相关文档
[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`source/ray/README.rst:17-47`；`source/ray/python/ray/__init__.py:80-130`；`source/ray/python/ray/remote_function.py:355-574`；`source/ray/src/ray/core_worker/core_worker_process.cc:231-285`。

## 未解决问题
动态进程、网络、GPU、重试和性能结论需要构建/运行日志；当前只保留静态证据或明确标注推断。

## 下一步阅读建议
从 M01→M05 和 D01 继续补齐精确符号、测试和运行证据。
