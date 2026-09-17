# 调试指南

- 对应源码版本：HEAD `cfe4725d23`。
- 证据状态：静态配方；命令结果未验证。

## 结论摘要

优先把问题归类为 API/序列化、CoreWorker、Raylet、对象、GCS/RPC、worker、环境或观测边界。

## 建议

- 记录 driver/worker/raylet/gcs PID、task/object/node ID。
- 在 `RemoteFunction._remote` 和 `worker.get` 观察输入/错误。
- 再查 CoreWorker、Raylet 和 Object Manager 日志。
- 遵守 token 与 runtime_env 脱敏规则。

## 相关文档
[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`source/ray/README.rst:17-47`；`source/ray/python/ray/__init__.py:80-130`；`source/ray/python/ray/remote_function.py:355-574`；`source/ray/src/ray/core_worker/core_worker_process.cc:231-285`。

## 未解决问题
动态进程、网络、GPU、重试和性能结论需要构建/运行日志；当前只保留静态证据或明确标注推断。

## 下一步阅读建议
从 M01→M05 和 D01 继续补齐精确符号、测试和运行证据。
