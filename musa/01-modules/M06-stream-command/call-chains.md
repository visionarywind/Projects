# Stream 与 Command 调度：调用链

- 模块：M06 Stream / Command
- 文档目的：stream command：关键调用链：从公共入口到真实副作用的追踪。
- 适用范围：M06 模块的《Stream 与 Command 调度：调用链》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：API 进入 Context/Stream、Command 入队、异步提交线程到 HAL queue submit 的骨架已静态确认；每类 Command 的 Build/Submit 细节见 [`implementation.md`](implementation.md)。
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)
- 行号均以 `rg -n` 对本地快照 `source/musa` 核对为准。

## CHAIN-M06-CREATE：`muapiStreamCreate` 到 Stream 初始化

```text
muapiStreamCreate(phStream, Flags)                         [src/driver/mu_stream.cpp:17-19]
  -> muapiStreamCreateWithPriority(phStream, Flags, 0)     [src/driver/mu_stream.cpp:21-42]
     ├─ InitPlatform()
     ├─ TlsCtxTop() 为空 -> MUSA_ERROR_INVALID_CONTEXT
     ├─ phStream 为空 -> MUSA_ERROR_INVALID_VALUE
     ├─ flags 既非 MU_STREAM_DEFAULT 也非 MU_STREAM_NON_BLOCKING -> MUSA_ERROR_INVALID_VALUE
     └─ Context::CreateStream(&pStream, flags, priority)   [src/musa/core/context.cpp:1198-1217]
        ├─ priority 先 clamp 到 [Stream::Priority::high, Stream::Priority::low]
        ├─ new Stream(this, flags, priority)               [src/musa/core/stream.cpp:804-848]
        ├─ stream->Init()                                  [src/musa/core/stream.cpp:930-1033]
        │   ├─ 按 engine 建 Hal::IQueue / Hal::ICmdPool（cmd buffer 池先建空表）
        │   ├─ 建 Timeline semaphore 两对；supportEngineSync 时再建 Hardware semaphore
        │   └─ 起两条线程：AsyncSubmit、AsyncWait
        ├─ 成功：ctxCrit->AddStream + IssueStreamCreatedResourceCallback
        └─ 失败：delete stream，不登记
```

`Context::Init` 也会用同一入口建默认流和 barrier 流，二者都走 `Stream::Init`。[src/musa/core/context.cpp:1906-1947]

## CHAIN-M06-COPY-ASYNC：`muMemcpyAsync` 到 copy command 入队

```text
muapiMemcpyAsync(dst, src, bytes, hStream)
  -> muapiMemcpyAsync                                       [src/driver/mu_memory.cpp:1047-1055]
     ├─ InitPlatform()
     ├─ GetMemcpy3DFrom1D(..., memcpy_default)
     └─ Context::GeneralMemcpy(TlsCtxTop(), hStream, copy3D, memBlocking)
        -> Context::GeneralMemcpy                           [src/musa/core/context.cpp:702-734]
           ├─ InfoStream(ctx, hStream)                      [src/musa/core/context.cpp:558-600]
           ├─ Context::CreateMemcpyNode(...)                [src/musa/core/context.cpp:2154-2172]
           │  -> GraphMemcpyNode::Init -> MemcpyLegalize    [src/musa/core/node/graphMemcpyNode.cpp:47-58]
           │  -> CopyManagerSelectPass                      [src/musa/core/node/graphMemcpyNode.cpp:439-525]
           ├─ capture ACTIVE  -> Stream::CaptureNode        [src/musa/core/context.cpp:722-730]
           ├─ capture INVALID -> MUSA_ERROR_STREAM_CAPTURE_INVALIDATED
           └─ 非 capture -> Stream::CmdCopyMemory           [src/musa/core/stream.cpp:696-752]
              -> 按 node->IsAsyncMemcpyCmd() 造 AsyncMemcpyCommand 或 SyncMemcpyCommand
              -> Context::ResolveDependencyAndQueueCommand  [src/musa/core/context.cpp:2012-2084]
```

同步 `muMemcpy` 与异步入口的差别只在 driver 层传入的 `wait`：同步版传 `true`，异步版传 `Platform::Get().GetSettings().memBlocking`（默认 0）。[src/driver/mu_memory.cpp:1035-1045] [src/musa/core/platformSettings.cpp:72-73]

## CHAIN-M06-KERNEL：kernel 提交

```text
muapiLaunchKernel(...)                                      [src/driver/mu_module.cpp:231-268]
  -> Context::GeneralLaunchKernel(ctx, hStream, nodeParams, wait) [src/musa/core/context.cpp:631-677]
     ├─ InfoStream 解析句柄，跨 context 使用需 capture ACTIVE 或 IsGreen
     ├─ Context::CreateKernelNodeOnStream(...)
     ├─ capture ACTIVE  -> Stream::CaptureNode              [src/musa/core/stream.cpp:673-694]
     ├─ capture INVALID -> MUSA_ERROR_STREAM_CAPTURE_INVALIDATED
     └─ 非 capture -> Stream::CmdLaunchKernel               [src/musa/core/stream.cpp:1568-1582]
        -> CreatePooledDispatchCommand(...)
        -> Context::ResolveDependencyAndQueueCommand        [src/musa/core/context.cpp:2012-2084]
```

blocking 参数来源是平台设置 `launchBlocking`，默认 0，即默认异步提交。[src/musa/core/platformSettings.cpp:72] [src/musa/core/platformSettings.cpp:370]

## CHAIN-M06-QUEUE：依赖解析和入队

```text
Context::ResolveDependencyAndQueueCommand(command, stream, blocking)
  [src/musa/core/context.cpp:2012-2084]
  ├─ pfmSerialMode 自旋抢 PfmSerialLock；blockedByPfm 自旋等 pfm context 清空
  ├─ 若是默认流：依赖本 context 中所有非默认、非 barrier、非 non-blocking 流的 LastCommand
  │                                                          [src/musa/core/context.cpp:2032-2042]
  ├─ 若是 barrier 流：依赖其它所有流的 LastCommand           [src/musa/core/context.cpp:2043-2052]
  ├─ 若是普通 blocking 流：依赖默认流 LastCommand            [src/musa/core/context.cpp:2053-2056]
  ├─ 依赖 barrier 流 LastCommand（仅当其状态 > completed）   [src/musa/core/context.cpp:2058-2062]
  ├─ 合并 pStream->GetCurrentDependencies 并清空             [src/musa/core/context.cpp:2064-2067]
  ├─ pStream->QueueCommand(command)                          [src/musa/core/context.cpp:2069]
  └─ blocking 或 pfmSerialMode 时 commandRef->Wait()         [src/musa/core/context.cpp:2071-2073]
```

```text
Stream::QueueCommand(command)                                [src/musa/core/stream.cpp:1056-1116]
  ├─ asyncCapacity = Settings.streamAsyncCapacity（默认 1024），为 0 会断言
  ├─ m_AsyncCount >= asyncCapacity 时 std::this_thread::yield() 自旋背压
  ├─ GetLastError() 非 SUCCESS 直接返回 sticky error，命令不入队
  ├─ m_AsyncCount.fetch_add(1)（与检查非原子，允许短暂超限，源码注释已说明）
  ├─ 锁 m_SubmitMtx：ChoosePerfEngine / SetPrevCommand / m_LastCommand = command
  ├─ command 状态 created -> queued（Command::SetStatus 用 CAS 保证单向）
  ├─ Graph/Dispatch 记录 queued timestamp；Dispatch 在 !supportEngineSync 时通知 MUpti
  ├─ m_CommandList.push_back(command)
  └─ 解锁后 m_SubmitCv.notify_one() 唤醒提交线程
```

## CHAIN-M06-ASYNC-SUBMIT：提交线程

```text
Stream::AsyncSubmit()                                        [src/musa/core/stream.cpp:1159-1338]
  ├─ 等待谓词：m_CommandList 非空 || stopMerging() || m_SubmitStopToken
  ├─ stopMerging()：合并列表非空 且（EngineSubmissionReady 或列表 >= 32 或命令队列已空）
  │                                                          [src/musa/core/stream.cpp:1287-1306]
  ├─ submitMergingList()                                     [src/musa/core/stream.cpp:1160-1246]
  │   ├─ EngineSubmissionSchedule(engine) 限制 cdm/ce 在途提交数
  │   ├─ 非 Memcpy/Graph 命令统一分配 submission id 并做 MUpti 关联
  │   ├─ status = m_MergingList.front()->GetLastError()
  │   ├─ status == SUCCESS -> m_MergingList.front()->Submit()
  │   ├─ 结果传播给同批所有命令：SetLastError + SetStatus(submitted)
  │   └─ m_MergingList splice 到 m_InflightList，notify m_WaitCv
  └─ buildCommand(frontCommand)                              [src/musa/core/stream.cpp:1248-1285]
      ├─ 锁 m_MergingMtx；FilterDependency() 去掉已完成依赖
      ├─ CanMergeTo() 决定是否并入当前批次；不合批则先 submitMergingList()
      ├─ 分配 signal timeline value：合批继承 primary，否则 ++m_TimelineValue
      ├─ command->Build(m_MergingList)；失败只 SetLastError，不中断线程
      ├─ 状态 queued -> built
      └─ push_back 进 m_MergingList
```

`Command` 基类负责把依赖翻译成 HAL semaphore wait：`Command::Build` 先按 `Status::built` 等待 `m_PrevCommand`，再按 `Status::submitted` 等待 `m_ExecutionDependencies`。[src/musa/core/command/command.cpp:175-234]

## CHAIN-M06-ASYNC-WAIT：完成线程

```text
Stream::AsyncWait()                                          [src/musa/core/stream.cpp:1340-1549]
  ├─ 等待谓词：m_InflightList 非空 || m_WaitStopToken
  ├─ 把 InflightList 中到首个 primary 为止的命令 splice 进 m_WaitingList
  ├─ front 是 primary 时：
  │   ├─ 按 preferred semaphore 类型 Wait(signalValue)，带 executionTimeout
  │   ├─ hardware semaphore 场景额外 Signal m_TimelineSemaphore
  │   └─ EngineSubmissionFeedback(engine) 归还在途配额
  ├─ Dispatch 额外 QueryGpuCopyJobQueueLastError，避免串扰错误归属
  ├─ GetEngineLastError(engine) 逐条查询鲁棒性错误
  ├─ 首次出错时对四个列表做 AbortedCommandSnapshot 快照
  ├─ 逐条 OutputPrintBuffer / SetLastError / ErrorHandler / Postprocess
  ├─ SetStatus(completed 或 error)；m_AsyncCount.fetch_sub(1)
  └─ 把 WaitingList 交给 Platform executor 异步 ReleaseResources
```

## CHAIN-M06-SYNC：同步入口

```text
muapiStreamSynchronize(hStream)                              [src/driver/mu_stream.cpp:152-166]
  -> Stream::Synchronize()                                   [src/musa/core/stream.cpp:223-239]
     ├─ capture 状态 != NONE -> 返回 UNSUPPORTED 并置 INVALIDATED
     ├─ 本流是默认流 -> Context::LockedSyncDefaultStream()   [src/musa/core/context.cpp:2086-2101]
     │                  等待 context 内所有非 non-blocking 流
     └─ 其它流 -> Stream::WaitFinish()                       [src/musa/core/stream.cpp:1118-1127]
                    -> LastCommand() 在 m_SubmitMtx 下取 m_LastCommand
                    -> command->Wait() 自旋到 completed/error
                    -> Hal().GetMemMgr()->UpdateUserPools()
```

`muapiStreamQuery` 不阻塞：无最后一条命令或最后一条已 `completed` 返回成功，否则 `MUSA_ERROR_NOT_READY`。[src/musa/core/stream.cpp:210-221]

## CHAIN-M06-EVENT：event record / wait

```text
muapiEventRecord(hEvent, hStream)
  -> imuapiEventRecordWithFlags                              [src/driver/mu_event.cpp:10-57]
     ├─ 校验 context、stream、event，并要求 stream/event 同 context
     └─ Stream::CmdSetEvent(param, blocking=false)           [src/musa/core/stream.cpp:416-426]
        ├─ capture ACTIVE  -> CaptureSetEvent                [src/musa/core/stream.cpp:1627-1651]
        ├─ capture INVALID -> MUSA_ERROR_STREAM_CAPTURE_INVALIDATED
        └─ 否则 -> AsyncSetEvent                             [src/musa/core/stream.cpp:1653-1714]
           ├─ MU_EVENT_INTERPROCESS -> CallbackCommand（host 侧自旋推进 IPC signal）
           └─ 普通 event -> RecordCommand + event->SetRecordCommand

muapiStreamWaitEvent(hStream, hEvent, Flags)                 [src/driver/mu_stream.cpp:356-389]
  -> Context::ValidateEvent + Stream::CmdWaitEvent           [src/musa/core/stream.cpp:428-449]
     ├─ 本流未 capture 但 event 是 capture event：
     │   先 Synchronize()，再把本流切到 ACTIVE 并登记到 captured set
     ├─ capture ACTIVE  -> CaptureWaitEvent                  [src/musa/core/stream.cpp:1716-1767]
     └─ 否则 -> AsyncWaitEvent                               [src/musa/core/stream.cpp:1769-1808]
        ├─ IPC event -> CallbackCommand（host 侧等待 read_index/signal）
        └─ 普通 event -> BarrierCommand
```

`Context::RecordEvent` / `Context::WaitEvent` 走 barrier 流，因此会拿到 barrier 流的全量依赖。[src/musa/core/context.cpp:2706-2734]

## CHAIN-M06-CAPTURE：capture 生命周期

```text
muapiStreamBeginCapture_v2(hStream, mode)                    [src/driver/mu_stream.cpp:391-421]
  ├─ hStream 为空 -> MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED
  ├─ Context::CreateGraph(&pGraph)
  ├─ Stream::BeginCapture(pGraph, mode, ...)                 [src/musa/core/stream.cpp:45-73]
  │   └─ 失败 -> Context::DestroyGraph(pGraph) 回滚
  └─ 成功后：状态 ACTIVE、记录 begin 线程、Platform::LockedAddCpaturedStream

Stream::CaptureNode(pGraphNode)                              [src/musa/core/stream.cpp:673-694]
  ├─ 无 m_CaptureGraph -> MUSA_ERROR_INVALID_VALUE
  ├─ m_CaptureGraph->AddGraphNode(...)，把新节点写进 m_LastCapturedNodes
  └─ 任一步失败 -> m_CaptureStatus = INVALIDATED

muapiStreamEndCapture(hStream, phGraph)                      [src/driver/mu_stream.cpp:473-501]
  -> Stream::EndCapture(ppGraph)                             [src/musa/core/stream.cpp:75-139]
     ├─ 非 origin 流 / 线程不匹配 / INVALIDATED / 非 ACTIVE 各自返回对应错误
     ├─ leaf node 不在 m_LastCapturedNodes 中 -> STREAM_CAPTURE_UNJOINED
     └─ 成功：输出 graph、清理 capture 字段、复位 captured event、
        从 Platform captured set 中摘除本流与跟随流
```

## CHAIN-M06-HAL-QUEUE：HAL/M3D submit 边界

```text
Command 子类 Submit()
  -> Command::SubmitToQueue(Hal::IQueue*, QueueSubmitInfo)   [src/musa/core/command/command.cpp:828-887]
     ├─ wait semaphore：preferred=Hardware 且对端是 Timeline 时改成 host 侧 Wait
     ├─ signal semaphore 直接展开成数组
     ├─ Memcpy 类型每次提交前重新分配 submission id 并做 MUpti 关联
     └─ pQueue->Submit(submitInfo)
  -> Hal::M3d::Queue::Submit                                 [src/hal/m3d/queue.cpp:182-372]
     ├─ user queue + pFence 直接 errorInvalidValue
     ├─ 每次提交最多 maxWaitSemaphoreCount 个 wait、maxSemaphoreCount-1 个 signal
     ├─ 超出上限的 wait/signal 拆成独立 WaitQueueSemaphore/SignalQueueSemaphore 调用
     ├─ 转换 Hal::ICmdBuffer* 为 IM3d::ICmdBuffer*
     ├─ 组装 IM3d::PerSubQueueSubmitInfo / MultiSubmitInfo，submissionHash[0] = submissionId
     ├─ 设置 submQueueSerial / freeSchedule / mpExceptionSettings
     └─ m_M3dQueue->Submit(m3dSubmitInfo)                    [src/hal/m3d/queue.cpp:353-353]
```

HAL 层录制侧：`Hal::M3d::CmdBuffer::Begin` 状态从 `initial` 转 `recording`，`End` 转 `executable`，`Reset` 在非 `pending` 时回到 `initial` 并清理内部内存。[src/hal/m3d/cmdBuffer.cpp:145-228]

## 缺口

- `AsyncMemcpyCommand` / `SyncMemcpyCommand` 的 Build/Submit 已展开，但 copy 细分路径（image/array 方向）的硬件行为未验证。[src/musa/core/command/AsyncMemcpyCommand.cpp:129-200] [src/musa/core/command/SyncMemcpyCommand.cpp:34-76]
- `EngineSubmissionSchedule` 的配额只覆盖 cdm 与 ce，其它 engine 无在途限制。[src/musa/core/stream.cpp:1932-1968]
- M3D `IQueue::Submit` 之后到内核驱动/firmware 的路径尚未验证，属于 M09 边界。
- `GraphCommand::Submit` 内部走 `Execute()`，graph 子任务展开属于 M08。[src/musa/core/command/graphCommand.cpp:69-78]

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（24 处，例：[src/musa/core/stream.cpp:804-848]）
- `src/musa/core/context.cpp`（19 处，例：[src/musa/core/context.cpp:1198-1217]）
- `src/driver/mu_stream.cpp`（6 处，例：[src/driver/mu_stream.cpp:17-19]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
