# Stream 与 Command 调度：风险与技术债

- 排序依据：按"并发/生命周期 > 语义正确性 > 可维护性 > 构建复现"排列。
- 状态口径：**已确认** = 源码直接读到并给出引用；**推断** = 结构上必然但无运行证据；**未验证** = 需目标机或硬件。
- 通用前提：本页为静态分析，未验证。所有运行、并发、硬件行为都没有实测证据。
- 文档目的：stream command：风险、技术债及源码证据与修复/验证建议。
- 适用范围：M06 模块的《Stream 与 Command 调度：风险与技术债》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`call-chains.md`](call-chains.md)、[`implementation.md`](implementation.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 风险表

| # | 风险 | 成因 | 证据（引用） | 验证/修复建议 |
|---|---|---|---|---|
| 1 | sticky error 无可见清除路径 | `AsyncWait` 首次出错把 `commandStatus` 写进 `m_LastError`，全文件没有回写 `MUSA_SUCCESS` 的路径；`QueueCommand` 入口 `GetLastError()` 非 SUCCESS 即丢弃新命令 | [src/musa/core/stream.cpp:1511-1517] `m_LastError` | 补一条错误恢复专项用例；确认是否存在隐藏清除入口 |
| 2 | `m_AsyncCount` 检查与 `fetch_add` 非原子 | 背压判断用 `load()`、计数用 `fetch_add()`，中间有 yield 窗口，源码注释自陈可短暂超限 | [src/musa/core/stream.cpp:1062-1071] `m_AsyncCount` | 量化超限幅度；必要时改 CAS 循环 |
| 3 | 每流两条常驻线程 | `Stream::Init` 直接起 `AsyncSubmit` 与 `AsyncWait` 两条线程，流数量大时线程与内存开销线性增长 | [src/musa/core/stream.cpp:1026-1033] `m_SubmitThread` `m_WaitThread` | 统计最大并发流数；考虑线程池 |
| 4 | submit/merge/wait 三方锁序 | `m_SubmitMtx`、`m_MergingMtx`、`m_WaitMtx` 分属两条线程与 API 线程，锁序未做过审计 | [src/musa/core/stream.h:204-214] `m_SubmitMtx` `m_MergingMtx` `m_WaitMtx` | 画锁序图，标注三把锁与 Context 锁的先后 |
| 5 | `Command::Build` 用 `yield` 自旋等上游状态 | 依赖命令状态未推进时 `while(...) std::this_thread::yield()` 空转烧 CPU | [src/musa/core/command/command.cpp:175-183] `buildSemaphoreDependency` | 与 `m_ScheduleMode` 统一，改条件变量 |
| 6 | capture 失败只置 INVALIDATED 不回滚 | `CaptureNode`/`CaptureWaitEvent` 失败时只改 `m_CaptureStatus`，已加入 graph 的节点不移除 | [src/musa/core/stream.cpp:673-694] `CaptureNode` | 明确文档化；或失败时移除最后一个节点 |
| 7 | `CmdAttachMemory` 是空实现 | capture 分支下该函数体只有 `return MUSA_SUCCESS`，attach 语义丢失但仍报成功 | [src/musa/core/stream.cpp:502-504] `CmdAttachMemory` | 补实现或显式返回 `MUSA_ERROR_NOT_SUPPORTED` |
| 8 | `muapiStreamWaitEvent` 入口不校验 `Flags` | driver 层直接 `param.flags = Flags`；非 capture 路径由 `AsyncWaitEvent` 按事件 record 枚举拒绝非默认值，capture 路径由 `CaptureWaitEvent` 按默认/非默认走两套完全不同的实现 | [src/driver/mu_stream.cpp:356-389] `muapiStreamWaitEvent` | 把 Flags 校验上提到 driver 入口，并区分 wait 语义与 record 枚举 |
| 9 | 合批上限 32 为硬编码 | `stopMerging` 里 `static constexpr uint32_t mergingListMaxSize = 32`，大批次负载下吞吐固定不可调 | [src/musa/core/stream.cpp:1287-1303] `mergingListMaxSize` | 抽成平台设置项 |
| 10 | 引擎在途配额只覆盖 cdm/ce | `EngineSubmissionSchedule` 只对 `Device::Engine::cdm` 与 `ce` 计数，其它 engine 无在途限制 | [src/musa/core/stream.cpp:1932-1941] `EngineSubmissionSchedule` | 按 engine 补配额或在注释里写明设计意图 |
| 11 | barrier 流复用内部标志而非独立类型 | `EnableUserQueue` 最后用 `m_Flags != MU_STREAM_TYPE_INTERNAL_SYNC` 判定，内部同步流被永久排除在 user queue 之外 | [src/musa/core/stream.cpp:1911-1930] `EnableUserQueue` `MU_STREAM_TYPE_INTERNAL_SYNC` | 补注释或改用显式类型区分 |
| 12 | `Context::ResolveDependencyAndQueueCommand` 是全局热点 | 每次入队都要拿 `pfmSerialLock` 自旋，并按默认流/barrier 流两种模式扫同 context 全部流 | [src/musa/core/context.cpp:2012-2042] `ResolveDependencyAndQueueCommand` `GetPfmSerialLock` | 压测多流并发入队；考虑按流缓存依赖快照 |
| 13 | `M3D_BUILD_MT_TRACE_CAPTURE` 改变 `Build` 行为 | 同一段依赖收集在 `#if/#else` 下推入的 `m_WaitSemaphoreInfos` 集合不同，两配置编译出的语义不一致 | [src/musa/core/command/command.cpp:190-200] `M3D_BUILD_MT_TRACE_CAPTURE` | 两种配置都纳入 CI |
| 14 | HAL/firmware 行为不可见 | 错误码、提交顺序、超时行为在 `Queue::Submit` 更深层，静态读不出结论 | [src/hal/m3d/queue.cpp:182-372] `Queue::Submit` | 在目标源码与硬件上补证（M09 边界） |
| 15 | 快照无 `unittest/` 目录 | 根 `CMakeLists.txt` 有 `MUSA_BUILD_UT` 分支并 `add_subdirectory(unittest)`，但快照里没有该目录，本地无法开单测目标 | [CMakeLists.txt:167-169] `MUSA_BUILD_UT` `unittest` | 找到真实单测仓或补最小测试目录 |

## 与其它模块耦合的风险

- **M02 ABI**：`muDevicePrimaryCtxRetain` 等 loader 导出条目的顺序与 `MUstream` 布局变化会让外部程序加载失败，属于符号回归，不在本模块内解决。[src/loader/loader_api_list.inc:110-115] `muDevicePrimaryCtxRetain`
- **M05 memory**：`Stream::RequestSpillBase` 用 `std::call_once` 预留 64MB spill memory，生命周期与流绑定，流销毁时机错误会泄漏。[src/musa/core/stream.cpp:1895-1907] `RequestSpillBase`
- **M08 graph**：`m_LastCapturedNodes` 是 capture 与 Graph 之间的共享状态，任何在 capture 中插入的命令都会改变 leaf node 判定。[src/musa/core/stream.h:248-253] `m_LastCapturedNodes`
- **M10 instrumentation**：submission id 与时间戳字段被 MUpti 消费，字段改名或语义变更会静默破坏 trace。[src/musa/core/stream.cpp:1172-1197] `submissionId`

## 技术债清单（非风险但值得还）

- `Stream::AsyncSubmit` 单函数跨 180 行，内含 `submitMergingList`/`stopMerging`/主循环三个逻辑块，建议拆成员函数。[src/musa/core/stream.cpp:1159-1338] `AsyncSubmit`
- `Command::Build` 内的 `buildSemaphoreDependency` lambda 同时处理 `m_PrevCommand` 与执行依赖，条件分支与 `#if` 交织，可读性差。[src/musa/core/command/command.cpp:175-234] `Command::Build`
- 四个命令容器（`m_CommandList`/`m_MergingList`/`m_InflightList`/`m_WaitingList`）命名不体现生产者，建议在头文件补一段状态流转注释。[src/musa/core/stream.h:196-203] `m_MergingList`
- driver 层 `_ptsz` 变体是机械复制（默认流改 per-thread 后转调），可用宏收敛。[src/driver/mu_stream.cpp:915-920] `muapiStreamWaitEvent_ptsz`

## 文档维护说明

- 本页行号来自本地快照，源码演进后会漂移；改动后需重跑行号与符号校验。
- 未执行的测试不标为通过；没有源码引用的结论不进风险表。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（11 处，例：[src/musa/core/stream.cpp:1511-1517]）
- `src/musa/core/stream.h`（3 处，例：[src/musa/core/stream.h:204-214]）
- `src/musa/core/command/command.cpp`（3 处，例：[src/musa/core/command/command.cpp:175-183]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 上表风险的运行期复现与修复验证均未执行。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
