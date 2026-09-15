# M01 设计原则

- 文档目的：解释核心算法为何采用 producer 分片、block 和 optimistic dequeue。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：行为已确认，性能收益是设计推断，未执行基准不能量化。
- 最后更新：2026-09-10
- 前置阅读：[全局设计原则](../../00-overview/design-principles.md)
- 后续阅读：[实现](implementation.md)
## 结论摘要

本页聚焦 01-modules/M01-core-queue/design.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## Producer 分片

每个 producer 有自己的 head/tail 和 block 链。入队通常不需要和其他 producer 争夺同一个元素槽位；consumer 通过 producer list 找工作。这牺牲了独立 producer 之间的全局顺序，README 明确说明该语义边界。[../../../../README.md:65-85](../../../source/concurrency-queue/README.md#L65-L85)

## Block 与索引

连续 block 减少每个元素一次分配，并使 block 可回收；BlockIndex 将逻辑 index 与具体 block 分离，使 producer 可以跨 block 增长。代价是 index 扩展、回收和空状态协议复杂。[`concurrentqueue.h:1466-1714`](../../../source/concurrency-queue/concurrentqueue.h#L1466-L1714)

## Token

ProducerToken 缓存 explicit producer，ConsumerToken 缓存 consumer 遍历位置和消费配额。token 是调用方状态，不是可并发共享锁；它减少查找但增加生命周期责任。

## Optimistic dequeue

消费者先用 optimistic count 领取候选位置，再以 tail/head 和 overcommit 修正判断是否真的可取。这样避免全局锁，但读者不能把瞬时 `false` 或 `size_approx` 当成严格全局事实。

## 分配模式

`CanAlloc` 路径允许 queue 增长；`CannotAlloc` 用于 `try_enqueue`，预留资源耗尽时返回 false。调用方如果需要无分配实时路径，必须预先提供 capacity 并处理容量不足。

## 修改警戒线

任何改变以下内容的 patch 都需要同时复核：release tail、acquire head/index、overcommit 恢复、异常 Guard、block empty 到 free list 的条件。

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
