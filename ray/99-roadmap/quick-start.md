# 快速上手

- 对应源码版本：HEAD `cfe4725d23`。
- 证据状态：静态配方；命令结果未验证。

## 结论摘要

从 `source/ray` 的开发文档和虚拟环境开始；本知识库根目录不是统一可构建产品。

## 建议

1. 阅读 `source/ray/AGENTS.md:64-92` 和开发文档。
2. 准备隔离 Python 环境。
3. 依据目标选择 wheel/Bazel/Python test 配方。
4. 先静态验证 D01，再在具备依赖时运行。
5. 记录真实命令和结果，未执行标为未验证。

## 相关文档
[项目入口](../README.md) · [分析状态](../00-overview/analysis-state.md)

## 源码证据摘要
`source/ray/README.rst:17-47`；`source/ray/python/ray/__init__.py:80-130`；`source/ray/python/ray/remote_function.py:355-574`；`source/ray/src/ray/core_worker/core_worker_process.cc:231-285`。

## 未解决问题
动态进程、网络、GPU、重试和性能结论需要构建/运行日志；当前只保留静态证据或明确标注推断。

## 下一步阅读建议
从 M01→M05 和 D01 继续补齐精确符号、测试和运行证据。
