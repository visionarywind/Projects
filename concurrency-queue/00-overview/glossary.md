# 术语表

- 文档目的：解释 00-overview/glossary.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/glossary.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- **producer 子队列**：每个 producer 自有的逻辑 FIFO 流；不同 producer 之间没有全局顺序。
- **ExplicitProducer**：由 `ProducerToken` 绑定、调用方显式管理的 producer。
- **ImplicitProducer**：按线程 ID 查找/创建、由队列维护的 producer。
- **Block**：连续存储多个 `T` 槽位并记录空状态的存储单元。
- **BlockIndex**：producer 用来按逻辑 index 找到 block 的索引结构。
- **FreeList**：回收空 block 并供 producer 重新请求的无锁链表。
- **optimistic dequeue**：消费者先乐观领取候选工作，再用 head/tail 协议确认是否实际有元素。
- **overcommit**：多个消费者乐观领取超过当前可见元素数时记录的暂时超额。
- **CanAlloc/CannotAlloc**：允许或禁止执行路径新增 block/index 的分配模式。
- **token**：缓存 producer 或 consumer 访问状态的调用方对象；不应并发共享。
- **LightweightSemaphore**：以原子计数、有限自旋和平台等待组合的轻量信号量。
- **近似大小**：遍历原子索引得到的估算值，不是严格线性化快照。
- **opaque handle**：C ABI 中表示 C++ queue 对象的不透明 `void*`。
- **证据状态**：已确认表示源码/配置直接支持；推断表示根据结构解释；未知表示需运行或额外证据。

## 相关文档

- [设计原则](design-principles.md)
- [运行时模型](runtime-model.md)
- [模块注册表](../01-modules/module-registry.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
