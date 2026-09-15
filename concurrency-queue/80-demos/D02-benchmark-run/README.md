# D02 Benchmark Run

- 文档目的：说明 benchmark 从编译到日志提取的真实路径。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：源码和 Make 入口已确认；本批未运行。
- 最后更新：2026-09-10
- 前置阅读：[Demo 注册表](../demo-registry.md)
- 后续阅读：[执行轨迹](execution-trace.md)
## 结论摘要

本页聚焦 80-demos/D02-benchmark-run/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

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

## 文档元数据（规范补充）

- 文档目的：说明 `80-demos/D02-benchmark-run/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| concurrency-queue/80-demos/D02-benchmark-run/README.md | 已定位 | 静态入口已定位，运行未验证 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已标注 | 已引用或待补 | 主 Demo | 部分完成：动态构建、运行和硬件边界仍未验证 |

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
