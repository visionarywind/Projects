# M02 行级审计

- 文档目的：修改 blocking wrapper 时的最小检查表。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：当前 HEAD 行区间静态整理。
- 最后更新：2026-09-10
- 前置阅读：[执行流程](execution-flows.md)
- 后续阅读：[M03](../M03-semaphore-and-platform/line-level-analysis.md)
## 结论摘要

本页聚焦 01-modules/M02-blocking-queue/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 检查点 | 证据 | 关注后果 |
|---|---|---|
| inner 为首成员 | `blockingconcurrentqueue.h:23-70` | reinterpret cast 失效 |
| enqueue 仅成功后 signal | `117-129` | 虚假 permit/消费者取空 |
| bulk signal 与实际 count 配对 | `blockingconcurrentqueue.h` bulk methods | permit 泄漏或欠账 |
| timeout 委托 semaphore | `146-548` | 超时误报成功 |
| inner exception 不被静默吞掉 | wrapper methods | 状态与返回值不一致 |
| 析构前停止 waiter | README caveat + destructor | use-after-free |

## 必要验证

运行 blocking 单测、timed/bulk 测试和至少一个多生产者/多消费者场景；平台变更还需在目标系统执行。

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
