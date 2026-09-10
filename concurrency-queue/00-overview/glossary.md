# 术语表

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
