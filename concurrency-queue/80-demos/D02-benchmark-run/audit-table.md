# D02 深度审计表

- 文档目的：检查性能 Demo 的输入、测量和产物是否可复现。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：静态确认；没有运行数据。
- 最后更新：2026-09-10
- 前置阅读：[执行轨迹](execution-trace.md)
- 后续阅读：[结果格式](../../01-modules/M06-benchmarks/result-format.md)
## 结论摘要

本页聚焦 80-demos/D02-benchmark-run/audit-table.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 维度 | 必须记录 | 当前状态 |
|---|---|---|
| 版本 | commit、编译器、flags | 待运行 |
| 硬件 | CPU、核数、OS | 待运行 |
| 场景 | producer/consumer/batch | 需从输出确认 |
| 被测实现 | adapter 和 traits | 源码已确认 |
| 原始数据 | benchmark log | 未生成 |
| 转换 | script、CSV schema | 脚本存在，格式待确认 |
| 结论 | 置信区间/重复次数 | 未知 |

禁止在缺少上述信息时提交性能排名或回归阈值。

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
