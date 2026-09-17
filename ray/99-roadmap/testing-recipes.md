# 测试配方

- 对应源码版本：HEAD `cfe4725d23`。
- 证据状态：静态配方；命令结果未验证。

## 结论摘要

测试命令必须来自 Ray 的目标模块文档、pytest.ini、BUILD 或 CI；本轮未执行。

## 建议

- Python：从 `python/ray/<module>/tests` 选择 focused pytest。
- C++：从 `src/ray/<module>/BUILD.bazel` 选择 target。
- 跨模块：先 unit，再 integration/cluster。
- 文档、GPU、云、多节点测试分别标明前提。

## 相关文档
[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`source/ray/README.rst:17-47`；`source/ray/python/ray/__init__.py:80-130`；`source/ray/python/ray/remote_function.py:355-574`；`source/ray/src/ray/core_worker/core_worker_process.cc:231-285`。

## 未解决问题
动态进程、网络、GPU、重试和性能结论需要构建/运行日志；当前只保留静态证据或明确标注推断。

## 下一步阅读建议
从 M01→M05 和 D01 继续补齐精确符号、测试和运行证据。
