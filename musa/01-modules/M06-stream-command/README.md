# M06：Stream 与 Command 调度

- 源码范围：`src/driver/mu_stream.cpp`、`src/musa/core/stream.{h,cpp}`、`src/musa/core/context.cpp`（依赖解析与 capture 分支）、`src/musa/core/command/*`、`src/hal/m3d/queue.{h,cpp}`、`src/hal/m3d/cmdBuffer.{h,cpp}`
- 文档目的：stream command：总览：职责、结构、关键结论与文档导航。
- 适用范围：M06 模块的《M06：Stream 与 Command 调度》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：**静态源码已确认**；线程交错、硬件提交顺序、真实异常码**未验证**。
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 一句话职责

把 Stream/Event 相关 API 调用变成 `Command` 对象，解析 stream / context / barrier 之间的依赖，由每条 stream 的两个后台线程（提交线程、等待线程）把命令 Build 进 HAL cmd buffer、经 `Queue::Submit` 交给 M3D queue，并用 semaphore 和命令状态对外暴露完成/错误。

## 结论先行

| 环节 | 承担者 | 关键事实 |
|---|---|---|
| 句柄创建/查询/销毁 | `src/driver/mu_stream.cpp` | 所有入口先 `InitPlatform()`，再经 `Context::InfoStream` 解析句柄；销毁前先 `LockedWait()` |
| 依赖解析 | `Context::ResolveDependencyAndQueueCommand` | 默认流、barrier 流、普通 blocking 流三套规则，最后统一 `QueueCommand` |
| 入队 | `Stream::QueueCommand` | 受 `streamAsyncCapacity` 背压；写 `m_LastCommand`、状态置 `queued`、唤醒提交线程 |
| Build + 合批 | `Stream::AsyncSubmit` | 单线程按 `CanMergeTo` 合并，`Command::Build` 把依赖翻成 semaphore wait |
| 提交 | 各 `Command::Submit` → `Command::SubmitToQueue` → `Hal::M3d::Queue::Submit` | 边界停在 `IM3d::IQueue::Submit` |
| 完成/错误 | `Stream::AsyncWait` | host 侧等待 semaphore，sticky error 落到 `m_LastError`，状态置 `completed`/`error` |
| capture | `Stream::BeginCapture` / `EndCapture` / `CaptureNode` | capture 活跃时不入队，改写 Graph；失败置 `MU_STREAM_CAPTURE_STATUS_INVALIDATED` 并 sticky |

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)。
2. 再读 [`implementation.md`](implementation.md)、[`call-chains.md`](call-chains.md)、[`execution-flows.md`](execution-flows.md)。
3. 数据结构看 [`data-structures.md`](data-structures.md)，接口契约看 [`interfaces.md`](interfaces.md)。
4. 设计取舍看 [`design.md`](design.md)，图示看 [`diagrams.md`](diagrams.md)，用例看 [`examples.md`](examples.md)。
5. 涉及异步、并发、默认流、capture 与 HAL 边界的风险见 [`risks-and-debt.md`](risks-and-debt.md)。
6. 改代码前读 [`development-guide.md`](development-guide.md)，回归方案见 [`testing.md`](testing.md)。

## 边界

- **本模块拥有**：Stream 对象生命周期与两条后台线程、Command 从 `created` 到 `completed`/`error` 的状态机、依赖解析（默认流 / barrier 流 / blocking 流 / capture）、cmd buffer 池化与提交、capture 状态机。
- **本模块不拥有**：上游公共 ABI 契约（M02）、Graph 节点的构建语义（M08）、`IM3d::IQueue` 之后的内核驱动与 firmware 行为（M09 只覆盖到 M3D 接口层）。
- **证据边界**：以下行号来自静态阅读，不等同于目标机运行成功；`source/musa` 快照中没有 `unittest/` 目录，无法在本快照直接跑单测。

## 核心证据

- 创建路径：`muapiStreamCreate` 仅转发到 `muapiStreamCreateWithPriority`，由 `Context::CreateStream` 分配并 `Init()`。[src/driver/mu_stream.cpp:17-42] [src/musa/core/context.cpp:1198-1217]
- 依赖解析：`Context::ResolveDependencyAndQueueCommand` 集中处理默认流、barrier 流、blocking 流和 capture 注入的 current dependency，最后 `QueueCommand` 并按需 `Wait()`。[src/musa/core/context.cpp:2012-2084]
- 入队：`Stream::QueueCommand` 在 `m_SubmitMtx` 下更新 `m_LastCommand`、置 `queued`、push 进 `m_CommandList` 并 `notify_one`。[src/musa/core/stream.cpp:1056-1116]
- 提交线程：`Stream::AsyncSubmit` 负责合批、分配 submission id、调用 `Submit()`，再把命令从 `m_MergingList` 挪进 `m_InflightList`。[src/musa/core/stream.cpp:1159-1338]
- 等待线程：`Stream::AsyncWait` 等 semaphore、查引擎错误、置 `completed`/`error` 并递减 `m_AsyncCount`。[src/musa/core/stream.cpp:1340-1549]
- 命令构建与等待：`Command::Build` 把依赖转成 `m_WaitSemaphoreInfos`，`Command::Wait` 自旋到 `completed`/`error`。[src/musa/core/command/command.cpp:175-245]
- HAL 提交：`Command::SubmitToQueue` 组装 wait/signal semaphore 后调用 `Hal::IQueue::Submit`。[src/musa/core/command/command.cpp:828-887] [src/hal/m3d/queue.cpp:182-372]
- capture 状态机：`BeginCapture` 登记到 Platform 的 captured set，`EndCapture` 校验线程/leaf node 后清理。[src/musa/core/stream.cpp:45-139]

## 同步与默认流语义（源码事实）

- `muapiStreamSynchronize` 解析句柄后直接调用 `Stream::Synchronize`；capture 状态非 `NONE` 时返回 `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED` 并把状态改成 `MU_STREAM_CAPTURE_STATUS_INVALIDATED`。[src/driver/mu_stream.cpp:152-166] [src/musa/core/stream.cpp:223-239]
- 同步默认流时走 `Context::LockedSyncDefaultStream`，等待 context 内所有 **非** non-blocking 流；其它流走 `WaitFinish`。[src/musa/core/stream.cpp:223-239] [src/musa/core/context.cpp:2086-2101]
- `Context::InfoStream` 把 `nullptr`/`MU_STREAM_LEGACY` 解析成默认流，把 `MU_STREAM_PER_THREAD` 解析成线程私有默认流，其余按 context 注册表查找，找不到再跨 device/context 兜底查找。[src/musa/core/context.cpp:558-600]
- 默认流创建时带 `musaStreamDefault` 标志且优先级为 `low`；barrier 流用内部标志 `MU_STREAM_TYPE_INTERNAL_SYNC` 创建，因此 `EnableUserQueue` 恒为 false。[src/musa/core/context.cpp:1906-1947] [src/musa/core/stream.cpp:1911-1930]

## 深度状态

已建立：创建 → 依赖解析 → 入队 → Build/merge → Submit → AsyncWait 完成 → 析构的正常路径，以及 capture、event wait/record、同步/异步 memcpy、kernel 提交的分支入口。

未验证：两线程与 API 线程的真实交错、M3D/firmware 侧提交顺序、错误注入后的恢复行为、`MU_STREAM_TYPE_INTERNAL_SYNC` 等内部标志的完整影响面。凡标记「推断/未验证」的条目，均需在目标源码或硬件环境中补证。

## Demo 覆盖

- **真实经过本模块的 Demo 步骤**：无被 trace 单列的步骤。D01 的设备循环只有 retain/set current/属性打印（见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md) §1–§2），没有命令入队或提交；primary context 初始化隐式创建默认流与 barrier 流（`Context::Init` [src/musa/core/context.cpp:1906-1965]），但 trace 未把它列为独立步骤。
- **候选但未覆盖**：D02–D07（`tests/*.cu`）——本快照无 `tests/` 目录，入口未核实；候选表把 D02/D03（异步 memcpy）与 D04/D05（graph 提交）的 M06 列为计划覆盖，D06/D07 为“可能”。
- **尚无 Demo 覆盖的重要实现**：
  1. 入队 `Stream::QueueCommand` [src/musa/core/stream.cpp:1056-1116]；
  2. 提交线程 `Stream::AsyncSubmit` [src/musa/core/stream.cpp:1159-1338]；
  3. capture 状态机 `Stream::BeginCapture` [src/musa/core/stream.cpp:45-139]；
  4. HAL 提交 `Queue::Submit` [src/hal/m3d/queue.cpp:182-357]；依赖解析与线程模型见 [`call-chains.md`](call-chains.md)、[`execution-flows.md`](execution-flows.md)。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（7 处，例：[src/musa/core/stream.cpp:1056-1116]）
- `src/musa/core/context.cpp`（5 处，例：[src/musa/core/context.cpp:1198-1217]）
- `src/driver/mu_stream.cpp`（2 处，例：[src/driver/mu_stream.cpp:17-42]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
