# Stream 与 Command 调度：数据结构与所有权

- 结构基线：`Stream` 定义见 `src/musa/core/stream.h`，`Command` 及其子类见 `src/musa/core/command/*.h`，提交容器 `QueueSubmitInfo` 见 `src/hal/halQueue.h`。
- 文档目的：stream command：核心数据结构、字段含义与持有关系。
- 适用范围：M06 模块的《Stream 与 Command 调度：数据结构与所有权》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：类/字段/枚举位置已静态确认；字段在目标机上的实际对齐、原子时序**未验证**。
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 结构关系

```text
Public handle -> Core object -> optional shared_ptr/unique_ptr -> HAL object
                                      └-> tracker/Context/Stream collection

MUstream (void*)  ──cast──>  Musa::Stream*  ──owns──>  Hal::IQueue / Hal::ICmdPool / Hal::ICmdBuffer
                                                    └> Timeline+Hardware semaphores
                                    │
                                    └─queue─> std::shared_ptr<Musa::Command>
                                                ├─ m_ExecutionDependencies / m_PrevCommand
                                                ├─ m_WaitSemaphoreInfos / m_pSignalTimelineSemaphore
                                                └─ m_pHalCmdBuffers
```

## Stream：类骨架与字段分组

`class Stream : public IStream` 整体位于 `src/musa/core/stream.h`。[src/musa/core/stream.h:28-260]

| 分组 | 字段/常量 | 作用 |
|---|---|---|
| 身份 | `m_ParentCtx`、`m_Flags`、`m_Priority`、`m_SeqID`、`m_Detached` | 归属 context、创建 flags、优先级、序号（`m_SeqID` 为 0 即默认流）、是否从 context detach |
| 背压 | `m_AsyncCount`（`std::atomic<uint32_t>`） | 每入队一条命令 `fetch_add`，完成一条 `fetch_sub`；上限取平台设置 `streamAsyncCapacity` |
| 命令队列 | `m_CommandList`（deque）、`m_MergingList`、`m_InflightList`、`m_WaitingList`（list）、`m_LastCommand` | API 线程 → 提交线程 → 等待线程之间的五级容器 |
| 依赖注入 | `m_CurrentDependencies` | capture/event/默认流场景下由 Context 注入给下一条命令 |
| 线程 | `m_SubmitThread` + `m_SubmitMtx`/`m_MergingMtx`/`m_SubmitCv`/`m_SubmitStopToken`；`m_WaitThread` + `m_WaitMtx`/`m_WaitCv`/`m_WaitStopToken` | 两条后台线程及其同步原语 |
| 错误 | `m_Attr`、`m_LastError` | stream 属性与 sticky 错误；`GetLastError` 直接返回该字段 |
| 命令间同步 | `m_TimelineSemaphore`、`m_HardwareSemaphore`、`m_TimelineValue` | 跨命令/跨 stream 的 wait/signal |
| 命令内同步 | `m_InternalTimelineSemaphore`、`m_InternalHardwareSemaphore`、`m_InternalTimelineValue` | 同一条命令内部的 semaphore |
| 引擎 | `m_EngineResources`（`std::array<EngineResource, count>`）、`m_InflightSubmissionCounts`、`m_UserQueueSubmissionIds` | 每个 engine 的 queue/cmdPool/cmdBufferLists 与在途提交计数 |
| capture | `m_CaptureGraph`、`m_CaptureStatus`、`m_IsOriginStream`、`m_LastCapturedNodes`、`m_PrimaryCaptureStream`、`m_CapturedEvents`、`m_BeginThreadId` | capture 状态机字段 |
| 静态/线程局部 | `s_BeginCaptureNum`、`t_BeginCaptureNum`、`t_CaptureMode` | capture 计数与模式（thread_local） |

字段分组的源码位置：命令队列与背压在 [src/musa/core/stream.h:195-203]，线程与锁在 [src/musa/core/stream.h:205-214]，semaphore 在 [src/musa/core/stream.h:219-227]，capture 字段在 [src/musa/core/stream.h:248-255]。

### EngineResource

每个 engine 一个槽位，包含 `HalPtr<Hal::IQueue> queue`、`HalPtr<Hal::ICmdPool> cmdPool`、`std::vector<std::list<HalPtr<Hal::ICmdBuffer>>> cmdBufferLists`、条件编译的 `cmdBufferFreeLists`（`M3D_BUILD_MT_TRACE_CAPTURE`）以及一把 `mtx`。[src/musa/core/stream.h:232-240]

### 关键常量

| 常量 | 值 | 含义 |
|---|---|---|
| `s_CmdBufferListSize` | 0 | 初始化时不预分配任何 cmd buffer |
| `s_SurfaceObjLimit` | 8 | surface 环形容器容量 |
| `s_InflightSubmissionLimit` | 3 | user queue 在途提交上限 |
| `s_NonUserQueueInflightSubmissionLimit` | 2 | 非 user queue 在途提交上限 |
| `reserveSpillSize` | 1ULL << 26 | 首次调用 `RequestSpillBase` 时预留的 spill memory |

常量定义位于 [src/musa/core/stream.h:183-186]，`reserveSpillSize` 位于 [src/musa/core/stream.h:137-137]。

### 只读访问器

`GetCommandList` / `GetMergingList` / `GetInflightList` / `GetWaitingList` 以 const 引用返回四个容器，是对外观测的固定入口。[src/musa/core/stream.h:148-151]

## Command：状态、类型与依赖

`class Command` 整体位于 [src/musa/core/command/command.h:33-266]。

### Status 状态机

```text
created(4) -> queued(3) -> built(2) -> submitted(1) -> completed(0)
                  |            |            |
                  +------------+------------+--> error(-1)
```

枚举值按"越大越早"排布，便于用数值比较判断先后；`error` 为 -1，因此 `Status > completed` 通常表示"还没完成"。[src/musa/core/command/command.h:35-42]

状态推进由 `Command::SetStatus` 用 `std::atomic` CAS 完成，只能沿合法边移动。[src/musa/core/command/command.cpp:92-120]

### MergeLevel 与 Type

- `MergeLevel { primary, secondary }` 决定合批时谁持有 signal timeline value。[src/musa/core/command/command.h:44-47]
- `Type` 枚举包含 `Dispatch`、`Barrier`、`Callback`、`Memcpy`、`AsyncMemcpy`、`Memset`、`Graph`、`Record`、`Paging`、`AccelBuild`、`DispatchRay` 等；注释要求**新类型只能追加在末尾**，因为枚举同时被 MUpti 使用。[src/musa/core/command/command.h:57-75]

### 依赖与 semaphore 字段

| 字段 | 作用 |
|---|---|
| `m_ExecutionDependencies` | 本命令必须等其**完成**的上游命令，`Command::Build` 把它翻成 wait semaphore |
| `m_SubmitDependencies` | 只需上游**已提交**即可的依赖，用于不能等完成的场景 |
| `m_PrevCommand` | 紧邻上一条命令（`Status::built` 之后才等待） |
| `m_WaitSemaphoreInfos` | Build 阶段展开出来的待等 semaphore（指针 + timeline 值） |
| `m_SubWaitSemaphoreInfos` / `m_SubSignalSemaphoreInfo` | Submit 阶段额外的 wait/signal |
| `m_pSignalTimelineSemaphore` / `m_pSignalHardwareSemaphore` / `m_SignalSemaphoreValue` | 本命令完成后 signal 的对象与值 |
| `m_Status`、`m_LastError` | 原子状态与 sticky 错误 |

字段位置见 [src/musa/core/command/command.h:216-245]；`SemaphoreInfo` 是 `std::pair<Hal::ISemaphore*, uint64_t>`。[src/musa/core/command/command.h:27]

### 默认合批条件

`Command::CanMergeTo` 的基类默认实现要求：`SupportMerge()`、执行依赖为空、合批列表非空、列表尾也支持合批、且 engine 相同；否则不允许并入。[src/musa/core/command/command.h:110-115]

## Command 子类

| 子类 | 头文件 | 提交语义 |
|---|---|---|
| `CallbackCommand` | `src/musa/core/command/callbackCommand.h` | host 回调，`Submit` 后由 stream 执行 `muapiLaunchHostFunc` |
| `DispatchCommand` | `src/musa/core/command/dispatchCommand.h` | kernel dispatch，走 `GetHalCmdBuffer` + `CmdDispatch` + `Queue::Submit` |
| `MemsetCommand` | `src/musa/core/command/memsetCommand.h` | memset |
| `MemcpyCommand` | `src/musa/core/command/memcpyCommand.h` | copy 基类，`SyncMemcpyCommand` 与 `AsyncMemcpyCommand` 继承它 |
| `AsyncMemcpyCommand` | `src/musa/core/command/memcpyCommand.h` | 异步 copy，可合并，`WaitWithinCmd` |
| `BarrierCommand` | `src/musa/core/command/barrierCommand.h` | event wait/signal 之间的空命令 |
| `RecordCommand` | `src/musa/core/command/recordCommand.h` | `muapiEventRecord` 产生的信号命令 |
| `GraphCommand` | `src/musa/core/command/graphCommand.h` | graph execute，`Submit` 后在 stream 上展开 `Execute()` |
| `PagingCommand` | `src/musa/core/command/pagingCommand.h` | 显存分页 |

类声明位置：`MemcpyCommand` 在 [src/musa/core/command/memcpyCommand.h:21-54]，`SyncMemcpyCommand`/`AsyncMemcpyCommand` 在 [src/musa/core/command/memcpyCommand.h:56-83]，`DispatchCommand` 在 [src/musa/core/command/dispatchCommand.h:23-96]，`GraphCommand` 在 [src/musa/core/command/graphCommand.h:17-62]。

## 所有权

- `Context` 持有全部 `Stream*`（裸指针注册表）；销毁时由 `Context::Dispose` 先 `WaitFinish` 再逐个 destroy。[src/musa/core/context.cpp:1984-2010]
- `Stream` 通过 `std::shared_ptr` 持有 `Command`；四个队列 + `m_LastCommand` 共享所有权，保证异步完成前对象存活。[src/musa/core/stream.cpp:1056-1116]
- `Command` 通过 `HalPtr<Hal::ICmdBuffer>` 持有 cmd buffer；`ReleaseResources` 在等待线程处理完后调用。[src/musa/core/command/command.h:139-139]
- 流的 HAL 资源（queue/pool/semaphore）由 `HalPtr` 持有，随 `Stream` 析构释放。[src/musa/core/stream.cpp:850-912]
- `Event`/`Graph` 由各自注册表持有；`m_CapturedEvents` 只是 capture 期间的引用，`EndCapture` 会清空。[src/musa/core/stream.cpp:75-139]

## 所有权问题

- `m_LastCommand` 在 `m_SubmitMtx` 下更新，`LastCommand()` 也在同一把锁里读；越过这把锁直接读会拿到过期指针。[src/musa/core/stream.cpp:1056-1116]
- `m_CurrentDependencies` 由 `ResolveDependencyAndQueueCommand` 在 `QueueCommand` 之后清空；若在两步之间插入新命令，依赖会重复注入。[src/musa/core/context.cpp:2064-2069]
- `Command::m_pSignalTimelineSemaphore` 等是裸指针，指向 `Stream` 持有的 semaphore；`Stream` 必须活到其全部命令完成，否则等待线程会解引用悬空指针。[src/musa/core/stream.cpp:850-912]
- 本页未列出的字段（如 `m_pSpillMemory`、`m_pSurfaceList`）以头文件原文为准，本文不臆测未读字段。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/stream.h`（9 处，例：[src/musa/core/stream.h:28-260]）
- `src/musa/core/command/command.h`（8 处，例：[src/musa/core/command/command.h:33-266]）
- `src/musa/core/stream.cpp`（5 处，例：[src/musa/core/stream.cpp:1056-1116]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
