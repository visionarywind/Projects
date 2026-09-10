# M01 核心无锁队列

- 文档目的：解释 `ConcurrentQueue<T>` 的数据结构、原子协议和资源回收。
- 证据状态：主实现已静态确认；形式化正确性和性能未验证。
- 最后更新：2026-09-10
- 前置阅读：[模块注册表](../module-registry.md)
- 后续阅读：[实现细节](implementation.md)

## 职责

M01 将公开 enqueue/dequeue API 映射到每个 producer 的 block 子队列。producer 负责发布 tail；consumer 在多个 producer 间选择并通过 head/optimistic 协议领取元素。它还拥有 producer list、implicit producer hash、初始 block pool 和 global free list。

## 入口与边界

- API：`ConcurrentQueue<T>`、`ProducerToken`、`ConsumerToken`。
- 核心实现：`concurrentqueue.h:786-3708`。
- 不负责：线程创建、业务 shutdown、跨 producer 全局顺序、阻塞等待。

## 关键不变量

1. 元素构造完成后才能 release-store tail。
2. 消费者获得槽位后必须 move-assign、析构内部对象并更新 empty 状态。
3. token 不可并发共享；queue 析构必须没有并发访问。
4. `try_enqueue` 不因单次调用隐式扩容；`size_approx` 不是严格快照。

## 子页

- [data-structures](data-structures.md)
- [design](design.md)
- [implementation](implementation.md)
- [execution-flows](execution-flows.md)
- [call-chains](call-chains.md)
- [line-level-analysis](line-level-analysis.md)

## 审计卡片

| 维度 | 结论 | 证据/状态 |
|---|---|---|
| 输入 | `T`、capacity、traits、token | 已确认 |
| 状态 | head/tail、optimistic/overcommit、block index | 已确认 |
| 输出 | bool、移动后的 T、近似 size | 已确认 |
| 所有权 | queue 持有 producer/hash/block；调用方持有 token | 已确认 |
| 并发 | producer 写自己的流；多 consumer 领取 | 已确认；内存模型需测试 |
| 异常 | 构造和 move assignment 有清理路径 | 静态确认 |
| 清理 | producer、未消费 T、hash、free list、pool | 静态确认 |
| 缺口 | 目标架构和全 interleaving | 未知 |
