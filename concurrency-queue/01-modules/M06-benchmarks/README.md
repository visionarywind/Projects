# M06 Benchmark 模块

- 文档目的：说明 benchmark 可执行文件的构成、测量对象和边界。
- 证据状态：源码入口和适配器已确认；性能数值未执行、未记录。
- 最后更新：2026-09-10
- 前置阅读：[模块注册表](../module-registry.md)
- 后续阅读：[运行 Demo](../../80-demos/D02-benchmark-run/README.md)

## 组成

`benchmarks/benchmarks.cpp`（约 2311 行）定义 queue adapters、traits、`BenchmarkResult`、`DedicatedConsumerBenchmark` 和 main（约 1963 行）。它比较 concurrentqueue、Boost、TBB、dlib 和 lock-based queue 等实现；第三方内部不在本知识库展开。

## 数据流

```text
main/options
  -> benchmark scenario/adapter
  -> producer/consumer threads
  -> timing and result aggregation
  -> benchmark log
  -> extract_graph_data.py
  -> CSV/graph input
```

## 边界

benchmark 是性能观测工具，不是 queue 语义证明。结果依赖机器、编译选项、线程调度、第三方版本和运行时间；本批没有运行，因此没有任何性能结论。

## 子页

- [line-level-analysis](line-level-analysis.md)
- [result-format](result-format.md)
