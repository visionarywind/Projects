# M01 关键调用链与逐行定位

- 文档目的：防止分析停在 wrapper，给出到状态改变点的完整路径。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：路径已由源码区间确认。
- 最后更新：2026-09-10
- 前置阅读：[M01 执行流程](execution-flows.md)
- 后续阅读：[M05 测试矩阵](../M05-verification/test-matrix.md)
## 结论摘要

本页聚焦 01-modules/M01-core-queue/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 隐式 enqueue

1. `ConcurrentQueue::enqueue`（约 `1010` 起）选择可分配路径。
2. `inner_enqueue`（`1395-1418`）调用 `get_or_add_implicit_producer`。
3. implicit producer 通过 thread-id hash 找到或建立 producer（`3323-3509`）。
4. `ImplicitProducer::enqueue`（`2515-2577`）检查 block/index，获取 block，placement-new 构造 T。
5. 成功后 release 发布 `tailIndex`；失败路径撤销 index tail 并归还 block。

## dequeue

1. `ConcurrentQueue::try_dequeue`（`1149-1185`）扫描 producer list 并按近似 size 选候选。
2. producer dequeue（`1983-2081` 或 `2579-2647`）用 optimistic count、overcommit 和 tail/head 原子协议竞争。
3. 成功 claim 后通过 BlockIndex 找槽位，向调用方输出对象执行 move assignment。
4. 显式析构槽内 T，更新 empty counter/flags；block 全空时回收。

## blocking

`BlockingConcurrentQueue::enqueue`（`117-129`）只有核心 enqueue 成功才 `sema->signal`；`wait_dequeue`（`146` 起）进入 `LightweightSemaphore::wait`，取得 permit 后调用核心 `try_dequeue`。信号量的真实等待在 `waitWithPartialSpinning`（`290-323`）和平台 `Semaphore` 分支。

## 维护检查点

- wrapper 是否仍只在成功后 signal？
- tail release 是否仍位于 T 构造之后？
- claim 失败是否正确修正 overcommit？
- move assignment 异常是否仍触发 Guard？
- empty block 是否只在所有槽位销毁后入 free list？

## 不能从静态调用链得出的结论

不能仅凭链路声称特定平台 lock-free、无饥饿、性能提升或所有 interleaving 都满足某个线性化定义；这些需要测试/模型证据。

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
