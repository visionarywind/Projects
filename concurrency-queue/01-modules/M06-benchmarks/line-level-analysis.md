# M06 Benchmark 审计

- 文档目的：修改 benchmark adapter 或结果解析时的检查点。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：静态确认。
- 最后更新：2026-09-10
- 前置阅读：[M06 README](README.md)
- 后续阅读：[结果格式](result-format.md)
## 结论摘要

本页聚焦 01-modules/M06-benchmarks/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 检查点 | 代码入口 | 关注项 |
|---|---|---|
| adapter API | `benchmarks/benchmarks.cpp` | 不同 queue 语义是否可比 |
| dedicated consumer | `DedicatedConsumerBenchmark` | 线程模型和场景定义 |
| result type | `BenchmarkResult` | 单位、聚合和打印 |
| third-party links | `build/makefile` | TBB/其他依赖可用性 |
| log extraction | `extract_graph_data.py` | 输入路径和列格式 |
| main options | benchmark main 约 `1963` | 默认运行时长/场景 |

没有执行程序时，不填写样本吞吐、延迟或排名。

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
