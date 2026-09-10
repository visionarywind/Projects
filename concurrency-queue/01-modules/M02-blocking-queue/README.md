# M02 阻塞队列

- 文档目的：解释 `BlockingConcurrentQueue<T>` 如何把核心队列和信号量组合为等待接口。
- 证据状态：静态主路径已确认；销毁竞态和平台行为需运行验证。
- 最后更新：2026-09-10
- 前置阅读：[模块注册表](../module-registry.md)
- 后续阅读：[执行流程](execution-flows.md)

## 职责

M02 是薄包装：拥有第一个成员 `inner` 核心队列和一个 `LightweightSemaphore`；成功 enqueue 后 signal，wait dequeue 先取得许可，再调用 inner dequeue。它不改变核心 queue 的跨 producer 顺序或生命周期契约。

## 关键约束

- `inner` 必须是第一个成员，源码用断言支持 underlying queue 转换。
- semaphore 创建失败抛 `std::bad_alloc`。
- 成功 signal 必须与成功入队配对；失败入队不能制造 permit。
- queue 析构前必须解除所有等待者。

## 主要 API

`try_dequeue`、`wait_dequeue`、`wait_dequeue_timed`、bulk wait，以及 token 转发 API 位于 `blockingconcurrentqueue.h:117-548`。

## 审计卡片

| 维度 | 结论 |
|---|---|
| 输入 | item、timeout、bulk count |
| 状态 | inner queue 状态 + semaphore count |
| 输出 | bool、移动后的 T、实际 bulk 数 |
| 错误 | inner bool/exception、timeout、semaphore allocation exception |
| 资源 | queue 持有 inner 与 semaphore |
| 并发 | enqueue signal 与 consumer wait 协作 |
| 未知 | 特定平台 wait 的精确行为 |

## 子页

- [execution-flows](execution-flows.md)
- [design](design.md)
- [line-level-analysis](line-level-analysis.md)
