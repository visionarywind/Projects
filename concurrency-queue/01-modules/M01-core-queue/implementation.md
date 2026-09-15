# M01 实现解剖

- 文档目的：越过公开 API，定位真正改变队列状态和对象生命周期的代码。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：主路径静态确认。
- 最后更新：2026-09-10
- 前置阅读：[M01 design](design.md)
- 后续阅读：[执行流程](execution-flows.md)
## 结论摘要

本页聚焦 01-modules/M01-core-queue/implementation.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 构造与销毁

`ConcurrentQueue` 构造函数初始化 producer/hash/free-list 相关成员，建立 implicit producer hash 和初始 block pool。析构函数遍历 producer、使 token 失效、释放 hash、free list 和 pool；它要求调用方先停止所有访问。[`concurrentqueue.h:833-919`](../../../source/concurrency-queue/concurrentqueue.h#L833-L919)

## 入队实际路径

```text
public enqueue
  -> inner_enqueue<CanAlloc>
  -> explicit token producer 或 get_or_add_implicit_producer
  -> ExplicitProducer/ImplicitProducer::enqueue
  -> block boundary/index capacity check
  -> requisition_block (initial pool -> free list -> allocator)
  -> reset empty state
  -> placement-new T
  -> release-store tailIndex
```

实现区间：显式 producer `1877-1981`，隐式 producer `2515-2577`，block 请求 `3068-3143`。如果 T 构造抛异常，路径恢复 index/block 状态，不发布未完成元素。

## 出队实际路径

```text
try_dequeue
  -> scan producerListTail
  -> size_approx heuristic
  -> ProducerBase::dequeue
  -> optimistic count/overcommit
  -> acquire tail and acq_rel head claim
  -> BlockIndex lookup
  -> move assignment to caller output
  -> T destructor
  -> empty counter/flags
  -> block index removal and free list recycle when empty
```

实现区间：公共选择 `1149-1185`，显式 `1983-2081`，隐式 `2579-2647`。输出赋值抛异常时 Guard 仍完成内部清理。

## 正常/异常/清理路径

| 路径 | 状态变化 | 结果 |
|---|---|---|
| 正常 enqueue | 构造、tail 发布 | true |
| 不允许分配 | 无可用 block/index | false |
| 构造异常 | 回滚 index/block | 异常传播 |
| 正常 dequeue | claim、move、析构、empty | true |
| 空/竞争失败 | optimistic 纠正或候选回退 | false/继续扫描 |
| producer 析构 | 析构剩余 T、释放 block/index | queue 清理继续 |

## 证据约束

“lock-free”“高吞吐”是项目定位或设计目标；静态本文不提供特定机器的性能数字，也不把未运行测试写成证明。

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
