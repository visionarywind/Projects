# M06 Benchmark 审计

- 文档目的：修改 benchmark adapter 或结果解析时的检查点。
- 证据状态：静态确认。
- 最后更新：2026-09-10
- 前置阅读：[M06 README](README.md)
- 后续阅读：[结果格式](result-format.md)

| 检查点 | 代码入口 | 关注项 |
|---|---|---|
| adapter API | `benchmarks/benchmarks.cpp` | 不同 queue 语义是否可比 |
| dedicated consumer | `DedicatedConsumerBenchmark` | 线程模型和场景定义 |
| result type | `BenchmarkResult` | 单位、聚合和打印 |
| third-party links | `build/makefile` | TBB/其他依赖可用性 |
| log extraction | `extract_graph_data.py` | 输入路径和列格式 |
| main options | benchmark main 约 `1963` | 默认运行时长/场景 |

没有执行程序时，不填写样本吞吐、延迟或排名。
