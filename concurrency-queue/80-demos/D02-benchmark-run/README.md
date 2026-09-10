# D02 Benchmark Run

- 文档目的：说明 benchmark 从编译到日志提取的真实路径。
- 证据状态：源码和 Make 入口已确认；本批未运行。
- 最后更新：2026-09-10
- 前置阅读：[Demo 注册表](../demo-registry.md)
- 后续阅读：[执行轨迹](execution-trace.md)

## 命令路径

```sh
cd build && make bin/benchmarks
./build/bin/benchmarks
python3 benchmarks/extract_graph_data.py benchmarks.log
```

命令是否需要额外 TBB/第三方库由 `build/makefile` 和当前环境决定；这里只记录仓库入口，不声称成功。

## 测量边界

benchmark main 选择场景和 adapter，启动生产/消费工作，收集 `BenchmarkResult`，输出日志；提取脚本将日志转为 CSV。吞吐/延迟、排名和稳定性必须来自实际日志，不能从源码推导。

## 审计卡片

| 维度 | 状态 |
|---|---|
| 输入 | benchmark options/默认场景 |
| 被测物 | concurrentqueue 和对比队列 adapter |
| 外部依赖 | TBB、Boost、dlib 等按构建配置 |
| 输出 | stdout/log/CSV |
| 正确性 | 不是 unit/model proof |
| 当前执行 | 未验证 |
