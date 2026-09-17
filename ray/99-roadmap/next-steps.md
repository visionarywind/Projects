# 后续路线

- 对应源码版本：HEAD `cfe4725d23`。
- 证据状态：静态配方；命令结果未验证。

## 结论摘要

按 project-prompt 的批次协议推进，优先 Core 与 D01。

## 建议

- **今天**：完成 M01-M05 符号定位和 D01 环境检查。
- **一周**：补任务/对象/调度异常与测试映射，完成 D01 动态证据。
- **一个月**：深化 M06-M14，选择 D02/D03，完成深度审计。

## 相关文档
[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`source/ray/README.rst:17-47`；`source/ray/python/ray/__init__.py:80-130`；`source/ray/python/ray/remote_function.py:355-574`；`source/ray/src/ray/core_worker/core_worker_process.cc:231-285`。

## 未解决问题
动态进程、网络、GPU、重试和性能结论需要构建/运行日志；当前只保留静态证据或明确标注推断。

## 下一步阅读建议
从 M01→M05 和 D01 继续补齐精确符号、测试和运行证据。
