# M01 核心无锁队列

- 文档目的：解释 `ConcurrentQueue<T>` 的数据结构、原子协议和资源回收。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：主实现已静态确认；形式化正确性和性能未验证。
- 最后更新：2026-09-10
- 前置阅读：[模块注册表](../module-registry.md)
- 后续阅读：[实现细节](implementation.md)
## 结论摘要

本页聚焦 01-modules/M01-core-queue/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

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

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M01-core-queue/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| concurrency-queue/01-modules/M01-core-queue/README.md | 已定位 | 已追踪代表路径 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已标注 | 已引用或待补 | 已映射或无专用 Demo | 部分完成：动态构建、运行和硬件边界仍未验证 |

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
