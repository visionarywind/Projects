# 配置影响地图

- 对应源码版本：HEAD `cfe4725d23`（2026-09-17）。
- 证据状态：静态代表路径；运行/性能未验证。

## 结论摘要

配置从 `ray.init`、环境变量、remote options、runtime_env、资源和认证模式投影到启动、调度、worker 和观测。

## 关系/流程

| 配置 | 影响 | 证据/状态 |
|---|---|---|
| address/RAY_ADDRESS | 连接 vs 本地启动 | `worker.py:1439-1505` 已确认 |
| `RAY_TASK_MAX_RETRIES` | task 默认重试 | `remote_function.py:421-430` 已确认 |
| resources/placement | Raylet 调度 | `remote_function.py:482-516` 已确认 |
| runtime_env | worker/job 依赖 | `remote_function.py:137-155` 已确认 |
| token auth | RPC/HTTP 安全 | 需深审 |

## 相关文档
[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`source/ray/README.rst:17-47`；`source/ray/python/ray/__init__.py:80-130`；`source/ray/python/ray/remote_function.py:355-574`；`source/ray/src/ray/core_worker/core_worker_process.cc:231-285`。

## 未解决问题
动态进程、网络、GPU、重试和性能结论需要构建/运行日志；当前只保留静态证据或明确标注推断。

## 下一步阅读建议
从 M01→M05 和 D01 继续补齐精确符号、测试和运行证据。
