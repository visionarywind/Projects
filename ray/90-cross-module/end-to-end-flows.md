# 端到端流程

- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：静态代表路径；运行/性能未验证。

## 结论摘要

任务端到端流程可由 D01 代表：装饰、提交、调度、执行、对象化、获取。Data/Train/Serve 另有业务流程，尚未深审。

## 关系/流程

1. 启动：`ray.init` 连接/启动 runtime。
2. 提交：`RemoteFunction._remote` export + `submit_task`。
3. 执行：Raylet 选择 worker（待补）。
4. 获取：`ray.get` → `get_objects` → value/error。

## 相关文档
[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`source/ray/README.rst:17-47`；`source/ray/python/ray/__init__.py:80-130`；`source/ray/python/ray/remote_function.py:355-574`；`source/ray/src/ray/core_worker/core_worker_process.cc:231-285`。

## 未解决问题
动态进程、网络、GPU、重试和性能结论需要构建/运行日志；当前只保留静态证据或明确标注推断。

## 下一步阅读建议
从 M01→M05 和 D01 继续补齐精确符号、测试和运行证据。
