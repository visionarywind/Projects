# Stream 与 Command 调度：设计分析

- 文档目的：stream command：设计意图、职责边界与关键取舍。
- 适用范围：M06 模块的《Stream 与 Command 调度：设计分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：设计取舍由源码结构直接支持；"好处/代价"部分是对实现的解读，不是运行测量结果。
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 设计问题

模块要同时满足四件事：保持 `MUstream` 公共 ABI 稳定、允许同进程多线程并发提交、支持 capture 与 event 的特殊语义、并把上层对象与 M3D 队列解耦。源码给出的答案是把 `Cmd* -> dependency -> queue -> submit/wait` 拆成四层稳定边界。

## 设计取舍

| 取舍 | 好处 | 代价 | 证据状态 |
|---|---|---|---|
| 每条 Stream 起 2 条常驻线程（提交 + 等待） | API 线程只做入队，返回快；完成处理不阻塞提交 | 每条流 2 个线程 + 2 把锁 + 2 个 cv，流数量大时线程数翻倍 | 已确认，代价为推断 |
| 提交与等待用 4 个命令容器分段（CommandList/Merging/Inflight/Waiting） | 每段锁范围小，合批与完成可并行 | 状态要在 5 个地方流转，排查需跨容器 | 已确认 |
| 提交线程内合批（`CanMergeTo`，批次上限 32） | 减少 HAL 调用次数与 submission 数量 | 合批失败时要先 flush 再重排，增加分支 | 已确认；性能收益**未验证** |
| 依赖解析集中在 `Context::ResolveDependencyAndQueueCommand` | 默认流/barrier 流语义只写一处 | 该函数是全局热点，所有流的每次入队都过它 | 已确认 |
| semaphore 双栈：per-stream timeline + hardware | user queue 场景用 hardware semaphore 少一次 host 等待 | `Build` 里要按"双方类型 + 同 device"选择，条件复杂 | 已确认 |
| sticky error 放在 `Stream::m_LastError` | 错误不会被后续命令覆盖，`QueueCommand` 直接短路 | 源码中未见清除路径，可能需要重建流 | 已确认路径，恢复策略**未验证** |
| `Command::Status` 用越大越早的整数编码 | `Status > waitStage` 即"还没到目标阶段"，一个比较搞定 | 阅读时容易误解 -1/0 的语义 | 已确认 |
| cmd buffer 池 `s_CmdBufferListSize = 0`，惰性分配 | 启动零开销，不浪费 | 首次提交承担 `Alloc` 开销与失败风险 | 已确认 |
| capture 不入队，直接改写 Graph | capture 期无异步副作用，语义清晰 | capture 与正常路径在同一组 `Cmd*` 函数里分叉，分支多 | 已确认 |
| barrier 流用内部标志 `MU_STREAM_TYPE_INTERNAL_SYNC` | event 语义复用同一套入队/提交机制 | barrier 流被 `EnableUserQueue` 恒判为 false，与普通流行为不同 | 已确认；是否有意为之**未验证** |
| 分层 + HAL 接口抽象 | 可替换 HAL、集中管理对象与错误转换 | 调用链长、排查成本高 | 已确认结构 |

## 关键不变量

1. **句柄可解析**：`muapi*` 入口必须能通过 `Context::InfoStream` 把 `MUstream` 解析成 `IStream*`，否则统一翻译成 `MUSA_ERROR_STREAM_DETACHED`（detached 流）或 `MUSA_ERROR_INVALID_HANDLE`（真句柄错误）。[src/musa/core/context.cpp:558-600] [src/driver/mu_stream.cpp:152-166]
2. **半初始化对象不外泄**：`Context::CreateStream` 在 `Stream::Init()` 失败时 `delete` 且不登记。[src/musa/core/context.cpp:1198-1217]
3. **状态单向推进**：`Command::SetStatus` 用 CAS 保证 `created -> queued -> built -> submitted -> completed`，错误只能进入 `error`。[src/musa/core/command/command.cpp:92-120]
4. **资源活到命令完成**：`shared_ptr<Command>` 在四个队列间转移，`m_LastCommand` 保存最后一条；`Stream` 析构先 join 线程再释放。[src/musa/core/stream.cpp:850-912]
5. **销毁先等待**：`muapiStreamDestroy_v2` 调 `Context::LockedWait()`，`Context::Dispose` 对所有流 `WaitFinish()`。[src/driver/mu_stream.cpp:168-199]
6. **背压上限**：`m_AsyncCount` 不超过 `streamAsyncCapacity`（允许短暂超限，源码注释已声明）。[src/musa/core/stream.cpp:1056-1116]

以上第 1、2、3、4、5 项由代码直接支持；第 6 项的"短暂超限"窗口大小需运行验证。

## 边界决策

| 决策 | 选择 | 源码依据 |
|---|---|---|
| 依赖由谁解析 | Context 而非 Stream | Stream 不知道"其它流"，Context 持有全部流注册表 |
| 默认流怎么识别 | `m_SeqID == 0` + `Context::GetDefaultStream()` 比较 | `Stream::Synchronize` 里用 `this == GetDefaultStream()` 分支 |
| barrier 流怎么识别 | 内部标志 `MU_STREAM_TYPE_INTERNAL_SYNC` 而非单独类型 | `Stream::EnableUserQueue` 依赖该标志 |
| capture 失败怎么办 | 状态置 `MU_STREAM_CAPTURE_STATUS_INVALIDATED`，后续命令全部报错 | `CaptureNode` 与 `Synchronize` 都这么做 |
| 完成通知给谁 | 等待线程统一处理，`Query`/`Wait` 只读状态 | `AsyncWait` 独占状态推进与错误快照 |

## 与上下游的设计耦合

- 上游：`muapi*` 层只做参数校验和错误翻译，业务全在 Core，因此 M02 的 ABI 变更不会波及调度逻辑。
- 下游：`Command::SubmitToQueue` 是唯一往 HAL 推的点，HAL 换实现只需改 `src/hal/m3d`。
- 横向：capture 复用同一组 `Cmd*`，Graph 模块（M08）通过 `Stream::CaptureNode` 与 `m_LastCapturedNodes` 喂数据；MUpti（M10）通过 submission id 与时间戳字段挂接。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（2 处，例：[src/musa/core/context.cpp:558-600]）
- `src/driver/mu_stream.cpp`（2 处，例：[src/driver/mu_stream.cpp:152-166]）
- `src/musa/core/stream.cpp`（2 处，例：[src/musa/core/stream.cpp:850-912]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
