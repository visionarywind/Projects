# M02 阻塞队列

- 文档目的：解释 `BlockingConcurrentQueue<T>` 如何把核心队列和信号量组合为等待接口。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：静态主路径已确认；销毁竞态和平台行为需运行验证。
- 最后更新：2026-09-10
- 前置阅读：[模块注册表](../module-registry.md)
- 后续阅读：[执行流程](execution-flows.md)
## 结论摘要

本页聚焦 01-modules/M02-blocking-queue/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

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

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M02-blocking-queue/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| concurrency-queue/01-modules/M02-blocking-queue/README.md | 已定位 | 已追踪代表路径 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已标注 | 已引用或待补 | 已映射或无专用 Demo | 部分完成：动态构建、运行和硬件边界仍未验证 |

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
