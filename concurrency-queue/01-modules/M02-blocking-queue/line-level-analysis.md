# M02 行级审计

- 文档目的：修改 blocking wrapper 时的最小检查表。
- 证据状态：当前 HEAD 行区间静态整理。
- 最后更新：2026-09-10
- 前置阅读：[执行流程](execution-flows.md)
- 后续阅读：[M03](../M03-semaphore-and-platform/line-level-analysis.md)

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
