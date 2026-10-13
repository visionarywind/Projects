# Stream 与 Command 调度：实现说明

- 文档目的：stream command：核心函数/流程的逐段实现解读。
- 适用范围：M06 模块的《Stream 与 Command 调度：实现说明》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：本文列出的函数均已逐段静态阅读；线程交错、HAL/firmware 侧行为、超时后的恢复**未验证**。
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 正常实现路径

```text
muapi* -> Context::InfoStream -> 建 Command -> ResolveDependencyAndQueueCommand
        -> Stream::QueueCommand -> AsyncSubmit/Build -> Command::Submit
        -> Command::SubmitToQueue -> Hal::IQueue::Submit
        -> AsyncWait -> completed/error -> ReleaseResources
```

骨架证据见 [src/musa/core/stream.cpp:1056-1549] 与 [src/musa/core/command/command.cpp:828-887]。

## Stream 构造与 Init

| 步骤 | 内容 | 证据 |
|---|---|---|
| 构造 | 记录 parent ctx、flags、priority，初始化 capture 状态为 `NONE`，`m_LastError = MUSA_SUCCESS` | [src/musa/core/stream.cpp:804-848] |
| 建 queue/pool | 按 `Device::Engine::count` 遍历，为每个 engine 建 `Hal::IQueue` 与 `Hal::ICmdPool`，`cmdBufferLists` 用 `s_CmdBufferListSize`（=0）初始化，即**不预分配** | [src/musa/core/stream.cpp:930-1033] |
| 建 semaphore | 建 timeline/hardware 两对 semaphore 与内部版本；`supportEngineSync` 为假时只建部分 | [src/musa/core/stream.cpp:930-1033] |
| 起线程 | `m_SubmitThread = std::thread(&Stream::AsyncSubmit, this)`、`m_WaitThread = std::thread(&Stream::AsyncWait, this)` | [src/musa/core/stream.cpp:930-1033] |

失败路径：任一 HAL 资源创建失败即返回错误码，由 `Context::CreateStream` 删除对象且不登记。[src/musa/core/context.cpp:1198-1217]

## 入队实现

`Stream::QueueCommand` 的顺序是固定的：背压检查 → sticky 检查 → 原子计数 +1 → 拿 `m_SubmitMtx` 更新 `m_LastCommand` → `SetStatus(queued)` → 记时间戳/通知 MUpti → `push_back` → 解锁后 `notify_one`。[src/musa/core/stream.cpp:1056-1116]

实现细节：

- 背压上限来自 `Settings::streamAsyncCapacity`（默认 1024，0 会 `MUSA_ASSERT`）。[src/musa/core/platformSettings.cpp:107]
- 背压用 `std::this_thread::yield()` 自旋，而不是 condition_variable 等待。
- `m_AsyncCount.fetch_add(1)` 在检查之后执行，源码注释明确说明"允许短暂超限"。
- capture 活跃时命令不会走到这里，`CaptureNode` 在同一把锁下直接改写 graph。
- 入队完成后立即 `m_SubmitCv.notify_one()`，提交线程的等待谓词是"队列非空 || stopMerging || stopToken"。[src/musa/core/stream.cpp:1159-1166]

## 提交线程实现

### buildCommand：从队头取出并合批

`buildCommand` 拿 `m_MergingMtx`，先 `FilterDependency()` 丢掉已 `completed` 的上游，再判断 `CanMergeTo`。不合批就先 `submitMergingList()` 把当前批次清空。之后为命令分配 signal timeline value（合批继承 primary，新批次 `++m_TimelineValue`），调用 `Command::Build`，把状态推到 `built` 并 push 进 `m_MergingList`。[src/musa/core/stream.cpp:1248-1285]

`Build` 失败只 `SetLastError`，**不中断提交线程**，命令继续留在批次里，等 `submitMergingList` 统一上报。

### submitMergingList：统一提交与错误传播

- 先 `EngineSubmissionSchedule(engine)` 占用在途配额，拿不到就 yield 等待。[src/musa/core/stream.cpp:1160-1164]
- 非 Memcpy/Graph 命令统一分配 `submissionId` 并做 MUpti 关联。[src/musa/core/stream.cpp:1170-1215]
- 批次状态取 `front()->GetLastError()`；为 `SUCCESS` 才调 `front()->Submit()`。[src/musa/core/stream.cpp:1217-1225]
- 结果 `SetLastError` + `SetStatus(submitted)` 推给同批所有命令。[src/musa/core/stream.cpp:1227-1234]
- 锁 `m_WaitMtx` 后 `splice` 把 `m_MergingList` 整体挪进 `m_InflightList`，再 `m_WaitCv.notify_one()`。[src/musa/core/stream.cpp:1236-1244]

### stopMerging：合批终止条件

三个条件之一成立就停止合批：`EngineSubmissionReady` 返回假（在途配额用尽）、批次长度 >= 32、命令队列已空。[src/musa/core/stream.cpp:1287-1306]

## 等待线程实现

`AsyncWait` 分三段：

1. **提交等待**：`front` 是 primary 时按 preferred semaphore 类型 `Wait(signalValue)`，带 `executionTimeout`；hardware semaphore 场景补一个 `Signal(m_TimelineSemaphore)`。[src/musa/core/stream.cpp:1370-1440]
2. **引擎反馈**：`EngineSubmissionFeedback(engine)` 归还在途配额。[src/musa/core/stream.cpp:1441-1455]
3. **错误与完成**：`GetEngineLastError` 逐条查鲁棒性错误；首次出错对四个队列做 `AbortedCommandSnapshot`；逐条 `OutputPrintBuffer`/`SetLastError`/`ErrorHandler`/`Postprocess`；`SetStatus(completed|error)` 并 `m_AsyncCount.fetch_sub(1)`；最后把 `m_WaitingList` 交给 Platform executor 异步 `ReleaseResources`。[src/musa/core/stream.cpp:1496-1545]

`Stream::Query` 不等待：capture 处于 ACTIVE 直接返回 `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED`；否则 `LastCommand()` 为空或状态等于 `completed` 返回 `MUSA_SUCCESS`，其它情况返回 `MUSA_ERROR_NOT_READY`。[src/musa/core/stream.cpp:210-221]

## Command::Build 实现

`Build` 内部是一个 `buildSemaphoreDependency` lambda：对每个依赖 `while (dependant->GetStatus() > waitStage) yield`，等它到指定阶段；`GetLastError() != SUCCESS` 就把错误透传给本命令；否则按"双方都是 hardware 且同 device"选择 hardware 或 timeline semaphore，push 进 `m_WaitSemaphoreInfos`。[src/musa/core/command/command.cpp:175-234]

`m_PrevCommand` 只在三种情况下才需要单独等：IP major < 4、本流未开 user queue、或前后命令 engine 不同。[src/musa/core/command/command.cpp:218-226]

执行依赖用 `Status::submitted` 作为等待目标，`m_PrevCommand` 用 `Status::built`。[src/musa/core/command/command.cpp:175-234]

## Command::Wait / 提交实现

- `Wait()` 用 `Schedule`（按 `m_ScheduleMode` 选 spin/yield/sleep）自旋到 `completed` 或 `error`，期间每次未完成都会调 `UpdateUserPools()`，最后返回 `m_LastError`。[src/musa/core/command/command.cpp:236-245]
- `HostWaitSemaphores()` 在 host 侧逐个 `Wait` 并清空 `m_WaitSemaphoreInfos`。[src/musa/core/command/command.cpp:271-286]
- `HostSignalFinish()` 按 preferred 类型 signal。[src/musa/core/command/command.cpp:265-269]
- `GetPreferredSemaphoreType()` 由 `m_ParentStream->EnableUserQueue(m_Engine)` 决定 hardware 还是 timeline。[src/musa/core/command/command.cpp:288-291]
- `SubmitToQueue` 先展开 wait/signal semaphore 数组，超出上限的拆成独立调用，再调 `pQueue->Submit`。[src/musa/core/command/command.cpp:828-887]

## cmd buffer 池

| 方法 | 行为 | 证据 |
|---|---|---|
| `Stream::RequestCmdBuffer` | 从 `m_EngineResources[engine].cmdBufferLists` 取头元素，没有就 `cmdPool->Alloc`；按 `engineMask` 决定取哪条 list | [src/musa/core/stream.cpp:1810-1862] |
| `Stream::ReleaseCmdBuffer` | 归还到对应 list，`m_WaitCv.notify_all()` | [src/musa/core/stream.cpp:1864-1893] |
| `Command::GetHalCmdBuffer` | `fromPool` 为真时申请并记进 `m_pHalCmdBuffers`，为假时复用上一个 | [src/musa/core/command/command.cpp:745-765] |
| 释放时机 | `Command::Postprocess` 里把 `m_pHalCmdBuffers` 全部 `ReleaseCmdBuffer` | [src/musa/core/command/command.cpp:247-252] |

常量 `s_CmdBufferListSize` 的值是 0，意味着初始化阶段一个 cmd buffer 都不分配，全部延迟到首次 `RequestCmdBuffer`。[src/musa/core/stream.h:183-183] [src/musa/core/stream.cpp:1810-1862]

## 引擎在途配额

| 方法 | 作用 | 证据 |
|---|---|---|
| `EngineSubmissionSchedule` | 提交前占用配额：cdm 与 ce 按 `s_InflightSubmissionLimit`(3)/`s_NonUserQueueInflightSubmissionLimit`(2) 限制，其它 engine 直接放行 | [src/musa/core/stream.cpp:1932-1941] |
| `EngineSubmissionFeedback` | 等待线程归还配额 | [src/musa/core/stream.cpp:1943-1953] |
| `EngineSubmissionReady` | 判断配额是否已满 | [src/musa/core/stream.cpp:1955-1968] |
| `EnableUserQueue` | `Settings::enableUserQueue` 按 engine 位掩码判断；barrier 流因 `MU_STREAM_TYPE_INTERNAL_SYNC` 恒为 false | [src/musa/core/stream.cpp:1911-1930] |

## capture 实现

- 全局计数 `s_BeginCaptureNum` 与线程局部 `t_BeginCaptureNum`/`t_CaptureMode` 保存 capture 上下文。[src/musa/core/stream.cpp:39-43]
- `BeginCapture` 校验"已有 capture/无 Graph/模式非法"三种失败，成功后写 `m_CaptureGraph`、`m_CaptureStatus=ACTIVE`、`m_BeginThreadId`，并把流加进 Platform 的 captured set。[src/musa/core/stream.cpp:45-73]
- `CaptureNode` 调 `m_CaptureGraph->AddGraphNode`，成功才把节点放进 `m_LastCapturedNodes`；任何一步失败立刻 `m_CaptureStatus = INVALIDATED`。[src/musa/core/stream.cpp:673-694]
- `EndCapture` 校验 origin 流、线程一致、状态 ACTIVE、leaf node 已 join，然后输出 graph 并清理。[src/musa/core/stream.cpp:75-139]
- `NeedStrictlyCheck` 决定 capture 中是否严格校验依赖。[src/musa/core/stream.cpp:141-156]
- `CmdAttachMemory` 在 capture 分支是空实现，直接返回 `MUSA_SUCCESS`。[src/musa/core/stream.cpp:502-504]

## 依赖解析实现

`Context::ResolveDependencyAndQueueCommand` 按"**谁在发命令**"决定依赖集合：

| 发起流 | 依赖谁 | 证据 |
|---|---|---|
| 默认流 | 本 context 内所有非默认、非 barrier、非 non-blocking 流的 `LastCommand` | [src/musa/core/context.cpp:2032-2042] |
| barrier 流 | 其它所有流的 `LastCommand` | [src/musa/core/context.cpp:2043-2052] |
| 普通 blocking 流 | 默认流的 `LastCommand` | [src/musa/core/context.cpp:2053-2056] |
| 任何流 | barrier 流的 `LastCommand`（仅当状态 `> completed`） | [src/musa/core/context.cpp:2058-2062] |
| 任何流 | `pStream->GetCurrentDependencies()` 合并后清空 | [src/musa/core/context.cpp:2064-2067] |

最后统一 `pStream->QueueCommand(command)`，若 `blocking` 或处于 `pfmSerialMode` 再 `commandRef->Wait()`。[src/musa/core/context.cpp:2069-2084]

## 分支矩阵

| 分支 | 触发 | 结果 | 当前证据 |
|---|---|---|---|
| 句柄无法解析 | `InfoStream` 返回空 | `MUSA_ERROR_STREAM_DETACHED` 或 `MUSA_ERROR_INVALID_HANDLE` | 已确认于 driver 层 |
| capture ACTIVE | 发命令时 `m_CaptureStatus == ACTIVE` | `CaptureNode` 追加节点，不入队 | 已确认 |
| capture INVALIDATED | 状态被打断 | 返回 `MUSA_ERROR_STREAM_CAPTURE_INVALIDATED` | 已确认 |
| capture 中调用同步 | `Synchronize`/`Query` | 返回 `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED` 并把状态置 `MU_STREAM_CAPTURE_STATUS_INVALIDATED` | 已确认 |
| 背压满 | `m_AsyncCount >= streamAsyncCapacity` | `yield` 自旋直到有命令完成 | 已确认 |
| sticky error | `m_LastError != SUCCESS` | `QueueCommand` 直接返回，新命令不入队 | 已确认 |
| 配额不足 | cdm/ce 在途达上限 | `EngineSubmissionSchedule` 内 yield | 已确认 |
| 合批终止 | 批次 >= 32 / 队列空 / 配额满 | `submitMergingList` 立即提交 | 已确认 |
| Build 失败 | 依赖上游已 error | 透传 `GetLastError`，提交线程继续 | 已确认 |
| HAL Submit 失败 | `pQueue->Submit` 返回非 SUCCESS | 批次统一置 `error`，等待线程触发 `ErrorHandler` | 已确认路径，硬件行为未验证 |
| 默认流同步 | 本流 `SeqID == 0` 且非 detached | 走 `LockedSyncDefaultStream` 扫全部非 non-blocking 流 | 已确认 |
| barrier 流内部 | `MU_STREAM_TYPE_INTERNAL_SYNC` | `EnableUserQueue` 恒 false，禁用 user queue doorbell | 已确认 |

## 尚未证明

- `AsyncSubmit`/`AsyncWait` 两个线程与 API 线程在真实负载下的交错顺序与锁竞争代价。
- `Hal::IQueue::Submit` 之后到内核驱动/firmware 的行为与真实错误码。
- 错误注入后 sticky error 的恢复条件（源码未见清除 `m_LastError` 的公开路径，**未验证**是否需要重建 stream）。
- `M3D_BUILD_MT_TRACE_CAPTURE` 关闭时 `Build` 少 push 一个 hardware 依赖的具体影响。
- 超时（`executionTimeout`）触发后 `Wait` 返回什么、`AsyncWait` 是否把命令置 `error`。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（31 处，例：[src/musa/core/stream.cpp:1056-1549]）
- `src/musa/core/command/command.cpp`（11 处，例：[src/musa/core/command/command.cpp:828-887]）
- `src/musa/core/context.cpp`（7 处，例：[src/musa/core/context.cpp:1198-1217]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
