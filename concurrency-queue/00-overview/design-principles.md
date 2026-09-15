# 设计原则与取舍

- 文档目的：解释核心架构为什么包含 producer 子队列、block、token、弱内存序和阻塞包装。
- 适用范围：M01–M03 的算法设计，以及对调用方的契约影响。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：实现行为已确认；“为什么”部分以 README/代码注释为依据，未明确处标为推断。
- 最后更新：2026-09-10
- 前置阅读：[总体架构](architecture.md)
- 后续阅读：[M01 design](../01-modules/M01-core-queue/design.md)
## 结论摘要

核心原则是把高频操作的共享竞争拆散：producer 主要访问自己的子队列，consumer 通过原子 head 领取工作；连续 block 让对象批量驻留并可复用；token 把搜索和状态缓存到调用方。代价是全局顺序、严格实时大小和简单可证明性被让渡给吞吐与低分配开销。

## 原则表

| 原则 | 代码体现 | 收益 | 代价/边界 |
|---|---|---|---|
| producer 分片 | `producerListTail` + Explicit/ImplicitProducer | 降低多个 producer 对同一写入点的竞争 | 独立 producer 无全局顺序 |
| block 存储 | `Block::elements`、`BLOCK_SIZE` | 连续存储、批量和复用 | block index 与回收复杂 |
| token 快路径 | `ProducerToken`、`ConsumerToken` | 保存 producer/consumer 局部状态 | token 不可并发共享 |
| optimistic dequeue | `dequeueOptimisticCount`/`dequeueOvercommit` | 多 consumer 无锁领取 | 可能产生暂时 overcommit，需恢复 |
| 弱内存序 | release tail、acquire head/index | 减少屏障成本 | 不等价于 sequential consistency |
| 分配模式 | `CanAlloc`/`CannotAlloc` | `enqueue` 可成长，`try_enqueue` 可预分配失败 | 调用方必须处理 false |
| 阻塞适配 | enqueue 后 `sema->signal` | 等待消费者低开销 | queue 销毁时不可有等待者 |

## 稳定契约与实现细节

**稳定契约（已确认）**：C++11 API 形状、单 producer 内顺序、blocking wait 接口、`try_enqueue` 可能失败、对象构造/销毁同步责任。[../../../README.md:112-137](../../source/concurrency-queue/README.md#L112-L137)

**实现细节（修改需谨慎）**：`BLOCK_SIZE` 默认值、producer 链表方向、hash 的 probing 和 block index 的历史链，除非 traits/API 明确暴露，不应让调用方依赖。

## 替代方案及评价

- 全局锁队列：更容易推理，但违背项目的 lock-free 性能目标；未在当前实现中使用。
- 单一 MPMC ring buffer：可能更直接，但所有 producer 共享槽位，且对象构造/容量扩展与本项目需求不同；这是架构推断，非仓库实验结论。
- 只提供 SPSC：性能可更高，但不满足项目的多 producer/consumer 定位；README 也将 SPSC 作为另一个项目。[../../../README.md:4-5](../../source/concurrency-queue/README.md#L4-L5)

## 相关文档

- [M01 数据结构](../01-modules/M01-core-queue/data-structures.md)
- [全局错误模型](global-error-model.md)

## 源码证据摘要

块存储和 empty 状态在 [../../../concurrentqueue.h:1588-1714](../../source/concurrency-queue/concurrentqueue.h#L1588-L1714)；optimistic dequeue 的原子协议在 [../../../concurrentqueue.h:1983-2081](../../source/concurrency-queue/concurrentqueue.h#L1983-L2081)。

## 未解决问题

README 没有给出形式化线性化证明；任何调整 memory order 的提议都需要模型检查、TSAN/压力测试和目标架构验证。

## 下一步阅读建议

阅读 [M01 line-level analysis](../01-modules/M01-core-queue/line-level-analysis.md)。
