# Stream 与 Command 调度：例子

- 文档目的：stream command：用法示例与边界/错误输入说明。
- 适用范围：M06 模块的《Stream 与 Command 调度：例子》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


- 说明：以下都是**源码阅读模型**，不是已运行的示例；行号来自本快照静态核对。
- 可用 D01 `muInfo` 覆盖查询类路径，用 D02/D04 覆盖异步与 Graph 路径（需目标机 + GPU，本知识库未声称执行成功）。

## 例 1：创建一条非阻塞流

```text
muapiStreamCreate(&s, MU_STREAM_NON_BLOCKING)
  -> muapiStreamCreateWithPriority(&s, MU_STREAM_NON_BLOCKING, 0)   [src/driver/mu_stream.cpp:21-42]
     校验: phStream 非空 / 有 current context / flags 在白名单
  -> Context::CreateStream(&stream, flags, priority)                [src/musa/core/context.cpp:1198-1217]
     new Stream(ctx, flags, low)                                    [src/musa/core/stream.cpp:804-848]
     Stream::Init()                                                 [src/musa/core/stream.cpp:930-1033]
        建 queue/pool/semaphore -> 起 AsyncSubmit / AsyncWait 两条线程
     登记到 ctxCrit + IssueStreamCreatedResourceCallback
返回: MUSA_SUCCESS，*phStream 指向新 Stream
```

关键点：`priority` 在 `Context::CreateStream` 里被 clamp 到 `[Priority::high, Priority::low]`，driver 层不校验非法优先级。[src/musa/core/context.cpp:1198-1217]

## 例 2：异步拷贝，观察四个队列

```text
muapiMemcpyAsync(dst, src, n, hStream)
  -> Context::GeneralMemcpy(..., memBlocking=0)                     [src/musa/core/context.cpp:702-734]
  -> Stream::CmdCopyMemory(node, blocking=false)                    [src/musa/core/stream.cpp:696-752]
  -> Context::ResolveDependencyAndQueueCommand(cmd, stream, false)  [src/musa/core/context.cpp:2012-2084]
  -> Stream::QueueCommand(cmd)                                      [src/musa/core/stream.cpp:1056-1116]
        cmd 状态: created -> queued
        m_AsyncCount: 0 -> 1
        m_CommandList: [cmd]  m_LastCommand = cmd

AsyncSubmit 线程:
  buildCommand -> cmd 状态 queued -> built，进 m_MergingList        [src/musa/core/stream.cpp:1248-1285]
  stopMerging  -> submitMergingList -> cmd->Submit()                [src/musa/core/stream.cpp:1160-1246]
        cmd 状态: built -> submitted；m_InflightList: [cmd]

AsyncWait 线程:
  Wait(signalValue) -> 等到 -> cmd 状态: submitted -> completed     [src/musa/core/stream.cpp:1340-1549]
  m_AsyncCount: 1 -> 0；m_WaitingList: [cmd] -> ReleaseResources
```

观察点：`m_AsyncCount` 是背压计数器，若连续提交超过 1024 条未完成，`QueueCommand` 会进入 `yield` 自旋。[src/musa/core/stream.cpp:1056-1116] [src/musa/core/platformSettings.cpp:107]

## 例 3：默认流上的 kernel 与非默认流的同步语义

```text
// 语义差异来自三条依赖规则                              [src/musa/core/context.cpp:2012-2084]
默认流发命令      -> 依赖本 context 内所有非默认/非 barrier/非 non-blocking 流
barrier 流发命令  -> 依赖其它所有流
普通 non-blocking 流发命令 -> 不被默认流当作依赖方

muapiStreamSynchronize(默认流)
  -> Stream::Synchronize() -> Context::LockedSyncDefaultStream()   [src/musa/core/context.cpp:2086-2101]
     遍历所有流，跳过 IsNonBlocking()，逐个 WaitFinish()

muapiStreamSynchronize(非默认流)
  -> Stream::Synchronize() -> Stream::WaitFinish()                 [src/musa/core/stream.cpp:1118-1127]
     LastCommand()->Wait()  // Schedule 自旋到 completed/error
```

## 例 4：event record 与 wait

```text
muapiEventRecord(e, s)
  -> imuapiEventRecordWithFlags(e, s, MU_EVENT_RECORD_DEFAULT)      [src/driver/mu_event.cpp:10-57]
     校验 event/stream 可解析、同 context（green context 允许 primary ctx）
  -> Stream::CmdSetEvent(param, blocking=false)                     [src/musa/core/stream.cpp:416-426]
     capture ACTIVE -> CaptureSetEvent                              [src/musa/core/stream.cpp:1627-1651]
     否则           -> AsyncSetEvent                               [src/musa/core/stream.cpp:1653-1714]
        IPC event   -> CallbackCommand（host 侧自旋推进 IPC signal）
        普通 event  -> RecordCommand + event->SetRecordCommand

muapiStreamWaitEvent(s, e, 0)
  -> Stream::CmdWaitEvent(param, blocking=false)                   [src/musa/core/stream.cpp:428-449]
     capture 未激活但 event 是 capture event: 先 Synchronize，再把本流切到 ACTIVE
     否则 -> AsyncWaitEvent                                        [src/musa/core/stream.cpp:1769-1808]
        IPC -> CallbackCommand；普通 -> BarrierCommand
```

`muapiEventSynchronize` / `muapiEventQuery` 走 event 自身状态，不经过 stream。[src/driver/mu_event.cpp:109-131]

## 例 5：capture 一段 graph

```text
muapiStreamBeginCapture_v2(s, mode)                                 [src/driver/mu_stream.cpp:391-421]
  -> Context::CreateGraph(&pGraph)
  -> Stream::BeginCapture(pGraph, mode, ...)                        [src/musa/core/stream.cpp:45-73]
     记 m_CaptureGraph / m_CaptureStatus=ACTIVE / m_BeginThreadId
     Platform::LockedAddCpaturedStream(this)
     失败 -> Context::DestroyGraph 回滚

// capture 中发 kernel
Context::GeneralLaunchKernel -> capture ACTIVE -> Stream::CaptureNode [src/musa/core/stream.cpp:673-694]
  -> m_CaptureGraph->AddGraphNode(...)
  -> 成功: SetLastCapturedNodes(newNode)
  -> 失败: m_CaptureStatus = INVALIDATED（已加节点不回滚）

muapiStreamEndCapture(s, &g)                                       [src/driver/mu_stream.cpp:473-501]
  -> Stream::EndCapture(&pGraph)                                   [src/musa/core/stream.cpp:75-139]
     校验 origin 流 / 线程一致 / ACTIVE / leaf node 已 join
     成功: 输出 graph、清空 capture 字段、复位 captured event、
           从 Platform captured set 摘除本流与跟随流
```

## 例 6：带错误的批次

```text
某条命令 Build 时上游已 error
  -> Command::Build: dependant->GetLastError() != SUCCESS
  -> 把上游错误写进本命令 m_LastError，但返回值仍是 SUCCESS   [src/musa/core/command/command.cpp:175-234]

submitMergingList
  -> status = front()->GetLastError() != SUCCESS -> 跳过 Submit()
  -> 同批所有命令 SetLastError(status) + SetStatus(error)   [src/musa/core/stream.cpp:1160-1246]

Stream::QueueCommand 的下一次调用
  -> GetLastError() != SUCCESS -> 直接返回 sticky error，新命令不入队 [src/musa/core/stream.cpp:1056-1116]
```

**未验证**：sticky error 后是否必须重建 stream，源码中未见清除 `m_LastError` 的公开路径。

## 例 7：销毁

```text
muapiStreamDestroy_v2(hStream)                                     [src/driver/mu_stream.cpp:168-199]
  ├─ hStream 是 nullptr / MU_STREAM_LEGACY / MU_STREAM_PER_THREAD -> MUSA_ERROR_INVALID_HANDLE
  ├─ detached 流: Platform 注册表移除 -> delete detachedStream -> 必要时 delete parentCtx
  └─ 普通流:
     pStream->GetContext()->LockedWait()          // 先等 context 内所有流
     Context::DestroyStream(pStream)              [src/musa/core/context.cpp:1219-1228]
       -> ~Stream()                               [src/musa/core/stream.cpp:850-912]
          置 stop token -> notify -> join 两条线程
          清空四个命令队列 -> 释放 spill/surface -> HalPtr 归零
```

## 例 8：观测一个命令

```text
读时间戳: command->GetQueuedTimestamp() / GetSubmittedTimestamp() / GetTimestamp(i)
读 submission: command->GetSubId()（memcpy 每次提交前重新分配）
读状态:     command->GetStatus() -> ToString<Command::Status>
读错误:     command->GetLastError()
读四队列:   stream->GetCommandList() / GetMergingList() / GetInflightList() / GetWaitingList()
错误快照:   stream->TakeAbortedCommandSnapshots()（首次出错时填充）
```

字段与访问器声明位置见 [src/musa/core/command/command.h:88-174] 与 [src/musa/core/stream.h:148-151]。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（20 处，例：[src/musa/core/stream.cpp:804-848]）
- `src/musa/core/context.cpp`（7 处，例：[src/musa/core/context.cpp:1198-1217]）
- `src/driver/mu_stream.cpp`（4 处，例：[src/driver/mu_stream.cpp:21-42]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
