# 风险登记

- 对应源码版本：HEAD `cfe4725d23`。
- 证据状态：静态配方；命令结果未验证。

## 结论摘要

最高风险集中在分布式生命周期、跨语言 ABI、对象所有权、RPC 安全、资源调度和未验证运行环境。

## 建议

| 风险 | 等级 | 缓解 |
|---|---|---|
| ObjectRef/ownership | 高 | M03 + fault tests |
| Core/Raylet协议 | 高 | proto/integration tests |
| worker/node故障 | 高 | retry/recovery tests |
| auth/runtime_env泄露 | 高 | token/redaction review |

## 相关文档
[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`source/ray/README.rst:17-47`；`source/ray/python/ray/__init__.py:80-130`；`source/ray/python/ray/remote_function.py:355-574`；`source/ray/src/ray/core_worker/core_worker_process.cc:231-285`。

## 未解决问题
动态进程、网络、GPU、重试和性能结论需要构建/运行日志；当前只保留静态证据或明确标注推断。

## 下一步阅读建议
从 M01→M05 和 D01 继续补齐精确符号、测试和运行证据。
