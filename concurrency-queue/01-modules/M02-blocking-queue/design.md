# M02 设计与权衡

- 文档目的：说明阻塞 API 的最小适配边界。
- 证据状态：行为已确认；公平性和调度延迟未量化。
- 最后更新：2026-09-10
- 前置阅读：[M02 README](README.md)
- 后续阅读：[行级审计](line-level-analysis.md)

## 设计

Blocking queue 不在核心 queue 中嵌入阻塞逻辑，而是以 semaphore 计数表示“已成功入队、尚未被等待消费者领取”的许可。这样非阻塞 API 保持独立，阻塞调用方可以选择自旋/平台等待。

## 权衡

- 收益：等待消费者无需主动轮询，bulk wait 可以一次取得多个许可。
- 代价：元素队列和 semaphore count 必须保持协议配对；异常、超时和销毁顺序更复杂。
- 边界：permit 不等于某个特定 producer 的元素，也不提供跨 producer 顺序保证。

## 不能推断

静态代码不能证明平台 semaphore 的公平性、唤醒顺序或 timeout 精度。
