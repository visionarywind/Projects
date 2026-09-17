# 统一运行轨迹

- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：静态代表路径；运行/性能未验证。

## 结论摘要

统一主线为 init→register→remote submit→schedule→execute→object ready→get→cleanup。入口和首尾已静态确认，中间 RPC 需补符号。

## 关系/流程

```text
ray.init
  → CoreWorkerProcess/Raylet registration
  → RemoteFunction._remote
  → CoreWorker.submit_task
  → Raylet/Worker
  → Object Store
  → worker.get_objects
  → Python value/error
```

## 相关文档
[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`source/ray/README.rst:17-47`；`source/ray/python/ray/__init__.py:80-130`；`source/ray/python/ray/remote_function.py:355-574`；`source/ray/src/ray/core_worker/core_worker_process.cc:231-285`。

## 未解决问题
动态进程、网络、GPU、重试和性能结论需要构建/运行日志；当前只保留静态证据或明确标注推断。

## 下一步阅读建议
从 M01→M05 和 D01 继续补齐精确符号、测试和运行证据。
