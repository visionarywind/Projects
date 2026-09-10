# M06 结果与提取

- 文档目的：说明 benchmark 日志如何进入图表数据。
- 证据状态：脚本职责已确认，具体列格式需从实际输出确认。
- 最后更新：2026-09-10
- 前置阅读：[M06 README](README.md)
- 后续阅读：[D02](../../80-demos/D02-benchmark-run/README.md)

`benchmarks/extract_graph_data.py` 从 `benchmarks.log`（也支持显式日志路径）读取结果并生成 CSV。除非已执行 benchmark 并检查输出，不应断言日志必然存在、列顺序或具体数值。

建议记录：commit、编译器、编译 flags、CPU/线程数、场景、原始日志、脚本参数和生成 CSV。图表只作为观测产物，不替代 unit/model test。
