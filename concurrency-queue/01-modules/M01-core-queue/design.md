# M01 设计原则

- 文档目的：解释核心算法为何采用 producer 分片、block 和 optimistic dequeue。
- 证据状态：行为已确认，性能收益是设计推断，未执行基准不能量化。
- 最后更新：2026-09-10
- 前置阅读：[全局设计原则](../../00-overview/design-principles.md)
- 后续阅读：[实现](implementation.md)

## Producer 分片

每个 producer 有自己的 head/tail 和 block 链。入队通常不需要和其他 producer 争夺同一个元素槽位；consumer 通过 producer list 找工作。这牺牲了独立 producer 之间的全局顺序，README 明确说明该语义边界。[../../../../README.md:65-85](../../../../README.md#L65-L85)

## Block 与索引

连续 block 减少每个元素一次分配，并使 block 可回收；BlockIndex 将逻辑 index 与具体 block 分离，使 producer 可以跨 block 增长。代价是 index 扩展、回收和空状态协议复杂。[`concurrentqueue.h:1466-1714`](../../../../concurrentqueue.h#L1466-L1714)

## Token

ProducerToken 缓存 explicit producer，ConsumerToken 缓存 consumer 遍历位置和消费配额。token 是调用方状态，不是可并发共享锁；它减少查找但增加生命周期责任。

## Optimistic dequeue

消费者先用 optimistic count 领取候选位置，再以 tail/head 和 overcommit 修正判断是否真的可取。这样避免全局锁，但读者不能把瞬时 `false` 或 `size_approx` 当成严格全局事实。

## 分配模式

`CanAlloc` 路径允许 queue 增长；`CannotAlloc` 用于 `try_enqueue`，预留资源耗尽时返回 false。调用方如果需要无分配实时路径，必须预先提供 capacity 并处理容量不足。

## 修改警戒线

任何改变以下内容的 patch 都需要同时复核：release tail、acquire head/index、overcommit 恢复、异常 Guard、block empty 到 free list 的条件。
