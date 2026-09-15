# M06 结果与提取

- 文档目的：说明 benchmark 日志如何进入图表数据。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：脚本职责已确认，具体列格式需从实际输出确认。
- 最后更新：2026-09-10
- 前置阅读：[M06 README](README.md)
- 后续阅读：[D02](../../80-demos/D02-benchmark-run/README.md)
## 结论摘要

本页聚焦 01-modules/M06-benchmarks/result-format.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


`benchmarks/extract_graph_data.py` 从 `benchmarks.log`（也支持显式日志路径）读取结果并生成 CSV。除非已执行 benchmark 并检查输出，不应断言日志必然存在、列顺序或具体数值。

建议记录：commit、编译器、编译 flags、CPU/线程数、场景、原始日志、脚本参数和生成 CSV。图表只作为观测产物，不替代 unit/model test。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
