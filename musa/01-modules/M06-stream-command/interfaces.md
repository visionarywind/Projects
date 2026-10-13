# Stream 与 Command 调度：接口契约

- 覆盖范围：`muapiStream*`、`muapiEvent*` 与本模块内部 `IStream`/`Command`/`Hal` 三层契约；公共 ABI 符号本身归 M02。
- 文档目的：stream command：接口签名、前置条件、错误码与跨层契约。
- 适用范围：M06 模块的《Stream 与 Command 调度：接口契约》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：所有返回码、参数校验与前置条件均来自静态阅读对应函数体；目标机实测返回值**未验证**。
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 上游 → 本模块（driver 层 API）

所有入口的通用前置：先 `InitPlatform()`，平台未初始化直接返回初始化失败码；`TlsCtxTop()` 取当前线程 top context。

| 接口 | 输入 | 输出/副作用 | 前置条件 | 错误 | 证据 |
|---|---|---|---|---|---|
| `muapiStreamCreate` | `phStream`、`Flags` | 转发到带优先级版本 | 同下 | 同下 | [src/driver/mu_stream.cpp:17-19] |
| `muapiStreamCreateWithPriority` | `phStream`、`flags`、`priority` | 新建 Stream 并写回句柄 | 有 current context；flags 只能是 `MU_STREAM_DEFAULT` 或 `MU_STREAM_NON_BLOCKING` | `MUSA_ERROR_INVALID_CONTEXT`、`MUSA_ERROR_INVALID_VALUE` | [src/driver/mu_stream.cpp:21-42] |
| `muapiStreamGetPriority` | `hStream`、`priority` | 写出优先级 | 输出指针非空 | `MUSA_ERROR_INVALID_VALUE`、`MUSA_ERROR_STREAM_DETACHED`、`MUSA_ERROR_INVALID_HANDLE` | [src/driver/mu_stream.cpp:44-62] |
| `muapiStreamGetDevice` | `hStream`、`device` | 写出 device id | 输出指针非空 | 同上 | [src/driver/mu_stream.cpp:64-84] |
| `muapiStreamGetId` | `hStream`、`streamId` | 写出 `GetSeqID()` | 有 current context；两个指针均非空 | `MUSA_ERROR_INVALID_VALUE`、`MUSA_ERROR_INVALID_CONTEXT` | [src/driver/mu_stream.cpp:86-110] |
| `muapiStreamGetFlags` | `hStream`、`flags` | 写出 flags | 输出指针非空 | `MUSA_ERROR_INVALID_VALUE`、`MUSA_ERROR_STREAM_DETACHED`、`MUSA_ERROR_INVALID_CONTEXT` | [src/driver/mu_stream.cpp:112-130] |
| `muapiStreamGetCtx` | `hStream`、`pctx` | 写出所属 context | 输出指针非空 | 同上 | [src/driver/mu_stream.cpp:132-150] |
| `muapiStreamSynchronize` | `hStream` | 阻塞到本流（默认流则全 context 非 non-blocking 流）完成 | 句柄可解析 | `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED`（capture 中）、`MUSA_ERROR_STREAM_DETACHED`、`MUSA_ERROR_INVALID_HANDLE` | [src/driver/mu_stream.cpp:152-166] |
| `muapiStreamQuery` | `hStream` | 非阻塞查询 | 句柄可解析 | `MUSA_ERROR_NOT_READY`（未完成）、`MUSA_ERROR_STREAM_DETACHED`、`MUSA_ERROR_INVALID_HANDLE` | [src/driver/mu_stream.cpp:205-219] |
| `muapiStreamDestroy_v2` | `hStream` | 等待 context 完成后删除 Stream；detached 流走另一条删除分支 | 不能传 `nullptr`/`MU_STREAM_LEGACY`/`MU_STREAM_PER_THREAD` | `MUSA_ERROR_INVALID_HANDLE`、`MUSA_ERROR_INVALID_CONTEXT` | [src/driver/mu_stream.cpp:168-199] |
| `muapiStreamSetAttribute` | `hStream`、`attr`、`value` | 写属性 | `value` 非空 | `MUSA_ERROR_INVALID_VALUE`、`MUSA_ERROR_STREAM_DETACHED`、`MUSA_ERROR_INVALID_HANDLE` | [src/driver/mu_stream.cpp:221-239] |
| `muapiStreamGetAttribute` | `hStream`、`attr`、`value` | 读属性 | `value` 非空 | 同上 | [src/driver/mu_stream.cpp:241-261] |
| `muapiStreamCopyAttributes` | `dst`、`src` | 复制属性到目标流 | 有 current context；两流同 context | `MUSA_ERROR_INVALID_CONTEXT`、`MUSA_ERROR_INVALID_VALUE`、`MUSA_ERROR_INVALID_HANDLE` | [src/driver/mu_stream.cpp:263-292] |
| `muapiStreamAttachMemAsync` | `hStream`、`dptr`、`length`、`flags` | 异步登记地址区间 | — | 失败经 `Stream::CmdAttachMemory` 返回 | [src/driver/mu_stream.cpp:294-327] |
| `muapiStreamAddCallback` | `hStream`、`callback`、`userData`、`flags` | 入队 host 回调命令 | 回调指针有效 | 参数/句柄错误码 | [src/driver/mu_stream.cpp:329-354] |
| `muapiStreamWaitEvent` | `hStream`、`hEvent`、`Flags` | 入队 event 等待 | event/stream 可解析 | 参数/句柄错误码、capture 错误码 | [src/driver/mu_stream.cpp:356-389] |
| `muapiStreamBeginCapture_v2` | `hStream`、`mode` | 建 Graph 并把流切到 capture | `hStream` 非空 | `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED`；创建 Graph 失败则回滚 | [src/driver/mu_stream.cpp:391-421] |
| `muapiStreamEndCapture` | `hStream`、`phGraph` | 产出 Graph 并清理 capture 状态 | 流处于 ACTIVE 且 leaf node 已 join | `MUSA_ERROR_STREAM_CAPTURE_UNJOINED`、`MUSA_ERROR_STREAM_CAPTURE_INVALIDATED`、线程不匹配错误 | [src/driver/mu_stream.cpp:473-501] |
| `muapiStreamIsCapturing` | `hStream`、`captureStatus` | 写出 capture 状态 | 输出指针非空 | 参数/句柄错误码 | [src/driver/mu_stream.cpp:503-528] |
| `muapiThreadExchangeStreamCaptureMode` | `mode` | 交换当前线程 capture 模式，返回旧值 | — | 输出指针非空 | [src/driver/mu_stream.cpp:616-639] |
| `muapiEventCreate` | `phEvent`、`Flags` | 建 Event | `phEvent` 非空；flags 为 0/1/2/3/6/7 之一 | `MUSA_ERROR_INVALID_VALUE`、`MUSA_ERROR_INVALID_CONTEXT` | [src/driver/mu_event.cpp:62-99] |
| `muapiEventRecord` | `hEvent`、`hStream` | 转发到带 flags 版本 | — | 同下 | [src/driver/mu_event.cpp:101-103] |
| `muapiEventRecordWithFlags` | `hEvent`、`hStream`、`flags` | 转发到内部 `imuapiEventRecordWithFlags` | event/stream 同 context（green context 下允许对 primary ctx） | `MUSA_ERROR_INVALID_HANDLE`、`MUSA_ERROR_INVALID_CONTEXT` | [src/driver/mu_event.cpp:105-107] |
| `muapiEventSynchronize` | `hEvent` | 阻塞到 event 完成 | `hEvent` 非空、当前 context 有效 | `MUSA_ERROR_INVALID_HANDLE`、`MUSA_ERROR_INVALID_CONTEXT`、`MUSA_ERROR_CONTEXT_IS_DESTROYED` | [src/driver/mu_event.cpp:109-131] |
| `muapiEventQuery` | `hEvent` | 非阻塞查询 event | `hEvent` 非空、context 有效 | `MUSA_ERROR_NOT_READY` | [src/driver/mu_event.cpp:193-214] |

### 通用错误映射

`Context::InfoStream` 找不到句柄时返回空指针，driver 层统一把它翻译成两种错误：

- 句柄是 detached stream → `MUSA_ERROR_STREAM_DETACHED`
- 否则 → `MUSA_ERROR_INVALID_HANDLE`（个别 getter 返回 `MUSA_ERROR_INVALID_CONTEXT`）

判别由 `Stream::IsDetachedStream` 完成，它排除 `nullptr`/`MU_STREAM_LEGACY`/`MU_STREAM_PER_THREAD` 后查 Platform 的 detached 注册表。[src/musa/core/stream.h:78-83]

## 本模块 → Core（IStream 覆写）

`Stream` 必须实现 `IStream` 的全部方法，这是 Context、driver 层和 Graph 执行器看到的唯一形状。[src/musa/core/stream.h:31-68]

| 分组 | 方法 | 约定 |
|---|---|---|
| capture | `BeginCapture`、`EndCapture`、`ExchangeCaptureMode`、`GetCaptureStatus`、`GetCaptureInfo`、`AddLastCapturedNodes`、`SetLastCapturedNodes` | 状态错误直接返回 capture 类错误码 |
| 属性 | `GetAttribute`、`SetAttribute`、`CopyAttributes`、`GetPriority`、`GetFlags` | 属性只在非 capture 时允许修改 |
| 生命周期 | `GetContext`、`Query`、`Synchronize`、`GetSeqID` | `Synchronize` 在 capture 中返回 `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED` |
| 命令提交 | `CmdLaunchRays`、`CmdSetEvent`、`CmdWaitEvent`、`CmdExecCallback`、`CmdAttachMemory`、`CmdLaunchGraph` 等 | capture 中走 `CaptureXxx`，否则走 `AsyncXxx` + 入队 |

`Stream::Synchronize` 的分支：capture 中先报错并把状态改成 `MU_STREAM_CAPTURE_STATUS_INVALIDATED`；默认流则委托给 `Context::LockedSyncDefaultStream`，其余流委托给 `Stream::WaitFinish`。[src/musa/core/stream.cpp:223-239]

## 本模块 → Core（Context 提供）

| 接口 | 作用 | 证据 |
|---|---|---|
| `Context::InfoStream` | 把 `MUstream` 解析成 `IStream*`，含默认流/每线程默认流的重定向与跨 context 兜底 | [src/musa/core/context.cpp:558-600] |
| `Context::CreateStream` / `DestroyStream` | 建流（含 `Init`）与销毁 | [src/musa/core/context.cpp:1198-1228] |
| `Context::ResolveDependencyAndQueueCommand` | 依赖解析 + `QueueCommand` + 可选 `Wait` | [src/musa/core/context.cpp:2012-2084] |
| `Context::GeneralLaunchKernel` / `GeneralMemcpy` | kernel/copy 的提交入口，含 capture 分支 | [src/musa/core/context.cpp:631-734] |
| `Context::RecordEvent` / `Context::WaitEvent` | 走 barrier 流的 event 语义 | [src/musa/core/context.cpp:2706-2734] |

## 本模块 → HAL

| 接口 | 用途 | 证据 |
|---|---|---|
| `Hal::IQueue::Submit` | 唯一提交边界，参数为 `Hal::QueueSubmitInfo` | [src/musa/core/command/command.cpp:828-887] |
| `Hal::ICmdBuffer::Begin` / `End` / `Reset` | cmd buffer 状态机 | [src/hal/m3d/cmdBuffer.cpp:145-228] |
| `Hal::ISemaphore::Wait` / `Signal` | 完成与依赖通知 | [src/musa/core/command/command.cpp:265-299] |
| `Stream::RequestCmdBuffer` / `ReleaseCmdBuffer` | cmd buffer 池的取还 | [src/musa/core/stream.cpp:1810-1893] |

HAL 返回码经 `HalToMuResult` 类转换边界回到 `MUresult`；本页不展开具体映射表。

## 调用方约束（开发约束，非源码事实）

- 公共 ABI 兼容性是 M02 的硬约束：其他模块不得直接改变 `MUstream` 句柄布局和导出表顺序。
- `MU_STREAM_LEGACY`/`MU_STREAM_PER_THREAD` 是保留句柄，任何取指针、销毁操作都必须先被 `InfoStream` 消费掉。
- capture 活跃时调用 `Synchronize`/`Query` 属于未定义用法，源码会报错并把流打成 `MU_STREAM_CAPTURE_STATUS_INVALIDATED`。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_stream.cpp`（20 处，例：[src/driver/mu_stream.cpp:17-19]）
- `src/driver/mu_event.cpp`（5 处，例：[src/driver/mu_event.cpp:62-99]）
- `src/musa/core/context.cpp`（5 处，例：[src/musa/core/context.cpp:558-600]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
