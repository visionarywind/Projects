# Stream 与 Command 调度：行级分析

- 文档目的：stream command：关键代码位置的行级证据与解读。
- 适用范围：M06 模块的《Stream 与 Command 调度：行级分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


- 方法：用 `rg -n` 定位函数签名，再 `sed -n` 读取完整函数体；只记录读到的内容。
- 结论状态：**已确认** = 行号与代码逐行读过；**推断** = 由代码结构推出但没有运行证据；**未验证** = 需目标机或硬件补证。

## 证据表

| 符号 | 行号区间 | 观察 | 结论状态 |
|---|---|---|---|
| `muapiStreamCreate` | [src/driver/mu_stream.cpp:17-19] | 纯转发，无校验 | 已确认 |
| `muapiStreamCreateWithPriority` | [src/driver/mu_stream.cpp:21-42] | flags 白名单 + priority 未在此校验，交 `Context::CreateStream` clamp | 已确认 |
| `muapiStreamSynchronize` | [src/driver/mu_stream.cpp:152-166] | 解析句柄后直接 `pStream->Synchronize()`，无额外等待 | 已确认 |
| `muapiStreamDestroy_v2` | [src/driver/mu_stream.cpp:168-199] | 普通流走 `LockedWait` + `DestroyStream`；detached 流走 Platform 删除分支 | 已确认 |
| `muapiStreamQuery` | [src/driver/mu_stream.cpp:205-219] | 只做句柄解析，语义在 `Stream::Query` | 已确认 |
| `muapiStreamWaitEvent` | [src/driver/mu_stream.cpp:356-389] | 校验 `hEvent`/context/句柄/`ValidateEvent`，`Flags` 原样写进 `param.flags` 交给 `CmdWaitEvent`，函数本身不检查 Flags | 已确认 |
| `muapiStreamBeginCapture_v2` | [src/driver/mu_stream.cpp:391-421] | 建 Graph 失败由调用方 `Context::DestroyGraph` 回滚 | 已确认 |
| `muapiStreamEndCapture` | [src/driver/mu_stream.cpp:473-501] | 只有 `EndCapture` 返回 SUCCESS 且 `phGraph` 非空时才写回；`hStream` 为空返回 `MUSA_ERROR_ILLEGAL_STATE` | 已确认 |
| `muapiThreadExchangeStreamCaptureMode` | [src/driver/mu_stream.cpp:616-639] | 交换线程局部 capture 模式，返回旧值 | 已确认 |
| `Stream::BeginCapture` | [src/musa/core/stream.cpp:45-73] | 三个失败分支依次是"已有 capture""无 Graph""模式非法" | 已确认 |
| `Stream::EndCapture` | [src/musa/core/stream.cpp:75-139] | 校验顺序：origin 流 → 线程 → 状态 → leaf node join | 已确认 |
| `Stream::NeedStrictlyCheck` | [src/musa/core/stream.cpp:141-156] | 由 capture mode 决定是否严格校验 | 已确认 |
| `Stream::Query` | [src/musa/core/stream.cpp:210-221] | capture ACTIVE 返回 UNSUPPORTED；否则无命令或最后一条等于 `completed` 才成功 | 已确认 |
| `Stream::Synchronize` | [src/musa/core/stream.cpp:223-239] | capture 分支会顺手把状态打成 `MU_STREAM_CAPTURE_STATUS_INVALIDATED` | 已确认 |
| `Stream::CaptureNode` | [src/musa/core/stream.cpp:673-694] | 失败时直接把 `m_CaptureStatus` 置为 INVALIDATED，已成功加入的节点不回滚 | 已确认 |
| `Stream::CmdCopyMemory` | [src/musa/core/stream.cpp:696-752] | 按 `IsAsyncMemcpyCmd()` 选择 Async/Sync 两种命令 | 已确认 |
| `Stream::CmdAttachMemory` | [src/musa/core/stream.cpp:502-504] | capture 分支是空实现，直接返回 `MUSA_SUCCESS` | 已确认 |
| `Stream` 构造 | [src/musa/core/stream.cpp:804-848] | capture 字段与 `m_LastError` 在此初始化 | 已确认 |
| `Stream` 析构 | [src/musa/core/stream.cpp:850-912] | 先停线程再释放资源，最后析构 `HalPtr` | 已确认 |
| `Stream::Init` | [src/musa/core/stream.cpp:930-1033] | 建 queue/pool/semaphore 后才起两条线程 | 已确认 |
| `Stream::QueueCommand` | [src/musa/core/stream.cpp:1056-1116] | 背压、sticky、计数、锁内改 `m_LastCommand`、锁外 notify | 已确认 |
| `Stream::WaitFinish` | [src/musa/core/stream.cpp:1118-1127] | 取最后一条命令 `Wait()`，之后刷新 user pools | 已确认 |
| `Stream::AsyncSubmit` | [src/musa/core/stream.cpp:1159-1338] | 三个内嵌函数：`submitMergingList`、`buildCommand`、`stopMerging` | 已确认 |
| `Stream::AsyncWait` | [src/musa/core/stream.cpp:1340-1549] | 等 semaphore → 查引擎错误 → 快照 → 状态推进 → 异步释放 | 已确认 |
| `Stream::CmdLaunchKernel` | [src/musa/core/stream.cpp:1568-1582] | 建 `DispatchCommand` 后交 Context 解析依赖 | 已确认 |
| `Stream::AsyncSetEvent` | [src/musa/core/stream.cpp:1653-1714] | IPC event 与普通 event 分成两条路径 | 已确认 |
| `Stream::AsyncWaitEvent` | [src/musa/core/stream.cpp:1769-1808] | IPC 走 `CallbackCommand`，普通走 `BarrierCommand` | 已确认 |
| `Stream::RequestCmdBuffer` | [src/musa/core/stream.cpp:1810-1862] | 池为空时 `cmdPool->Alloc`，`engineMask` 决定取哪条 list | 已确认 |
| `Stream::ReleaseCmdBuffer` | [src/musa/core/stream.cpp:1864-1893] | 归还后唤醒等待线程 | 已确认 |
| `Stream::EnableUserQueue` | [src/musa/core/stream.cpp:1911-1930] | 按 engine 位掩码读 `enableUserQueue` | 已确认 |
| `Stream::EngineSubmissionSchedule` | [src/musa/core/stream.cpp:1932-1941] | 只对 cdm/ce 生效 | 已确认 |
| `Context::InfoStream` | [src/musa/core/context.cpp:558-600] | 默认流/每线程默认流重定向 + 注册表 + 跨 context 兜底 | 已确认 |
| `Context::GeneralLaunchKernel` | [src/musa/core/context.cpp:631-677] | capture 三态分支（ACTIVE/INVALIDATED/none） | 已确认 |
| `Context::GeneralMemcpy` | [src/musa/core/context.cpp:702-734] | 同上三分支，非 capture 时调 `Stream::CmdCopyMemory` | 已确认 |
| `Context::Synchronize` | [src/musa/core/context.cpp:997-1017] | 同步本 context 全部流 | 已确认 |
| `Context::CreateStream` | [src/musa/core/context.cpp:1198-1217] | `new` + `Init`，失败即 `delete` 且不登记 | 已确认 |
| `Context::GetPerThreadDefaultStream` | [src/musa/core/context.cpp:1238-1277] | 惰性建流并挂到线程局部 | 已确认 |
| `Context::Init` | [src/musa/core/context.cpp:1906-1960] | 默认流在 1918 建，barrier 流在 1944 建 | 已确认 |
| `Context::Dispose` | [src/musa/core/context.cpp:1984-2010] | 先 `WaitFinish` 全部流，再 `DetachClientStreams` | 已确认 |
| `Context::ResolveDependencyAndQueueCommand` | [src/musa/core/context.cpp:2012-2084] | 三条依赖规则 + capture 注入 + 可选 `Wait` | 已确认 |
| `Context::LockedSyncDefaultStream` | [src/musa/core/context.cpp:2086-2101] | 跳过 non-blocking 流 | 已确认 |
| `Context::LockedReset` | [src/musa/core/context.cpp:2120-2144] | 2127 处理默认流，2135 处理 barrier 流 | 已确认 |
| `Context::RecordEvent` | [src/musa/core/context.cpp:2706-2719] | `RecordCommand` 挂到 barrier 流 | 已确认 |
| `Context::WaitEvent` | [src/musa/core/context.cpp:2721-2734] | `BarrierCommand` 挂到 barrier 流 | 已确认 |
| `Command` 构造 | [src/musa/core/command/command.cpp:34-81] | 初始化状态 `created`、engine、merge 标志、时间戳占位 | 已确认 |
| `Command::SetStatus` | [src/musa/core/command/command.cpp:92-120] | CAS 推进，非法转移返回 false | 已确认 |
| `Command::RecordDependency` | [src/musa/core/command/command.cpp:132-154] | 按上游状态选择 `m_ExecutionDependencies` 或 `m_SubmitDependencies` | 已确认 |
| `Command::FilterDependency` | [src/musa/core/command/command.cpp:156-166] | 丢掉已完成的依赖，减少无效 semaphore wait | 已确认 |
| `Command::Build` | [src/musa/core/command/command.cpp:175-234] | 先 `m_PrevCommand` 后执行依赖；错误透传 | 已确认 |
| `Command::Wait` | [src/musa/core/command/command.cpp:236-245] | `Schedule` 自旋 + 刷新 user pools | 已确认 |
| `Command::Postprocess` | [src/musa/core/command/command.cpp:247-252] | 归还全部 cmd buffer | 已确认 |
| `Command::ReleaseResources` | [src/musa/core/command/command.cpp:254-263] | 清理 timestamp memory | 已确认 |
| `Command::SubmitToQueue` | [src/musa/core/command/command.cpp:828-887] | memcpy 每次重分配 submission id | 已确认 |
| `Hal::M3d::Queue::Submit` | [src/hal/m3d/queue.cpp:182-372] | 超出 semaphore 上限时拆成多次 HAL 调用 | 已确认 |
| `Hal::M3d::CmdBuffer::Begin`/`End`/`Reset` | [src/hal/m3d/cmdBuffer.cpp:145-228] | 状态机 `initial → recording → executable → pending → initial` | 已确认 |

## 阅读方法

1. 先看参数检查和 early return：driver 层每个 `muapi*` 开头都是 `InitPlatform()` + `TlsCtxTop()` + 指针非空校验。
2. 再看对象创建、登记、状态推进：`Context::CreateStream`、`Stream::QueueCommand`、`Command::SetStatus`。
3. 最后看 HAL 调用、异步入队、错误回滚和析构：`Command::SubmitToQueue`、`Hal::M3d::Queue::Submit`、`Stream::~Stream`。

## 需要注意的行级事实

- `Stream::CaptureNode` 失败只把状态改成 `MU_STREAM_CAPTURE_STATUS_INVALIDATED`，已经 `AddGraphNode` 成功的节点不会回滚，`EndCapture` 会因此返回错误。[src/musa/core/stream.cpp:673-694]
- `Stream::CmdAttachMemory` 在 capture 分支什么都不做就返回成功，属于**空实现**。[src/musa/core/stream.cpp:502-504]
- `Command::Build` 在依赖上游已经 `error` 时把 `GetLastError` 复制给自己，不返回失败；真正的失败上报发生在 `submitMergingList`。[src/musa/core/command/command.cpp:175-234]
- `Command::Wait` 在等待期间每次循环都会调 `UpdateUserPools()`，等待越久池刷新越频繁。[src/musa/core/command/command.cpp:236-245]

本页不把没有逐行读取的代码扩写成确定行为。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（24 处，例：[src/musa/core/stream.cpp:45-73]）
- `src/musa/core/context.cpp`（13 处，例：[src/musa/core/context.cpp:558-600]）
- `src/musa/core/command/command.cpp`（11 处，例：[src/musa/core/command/command.cpp:34-81]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
