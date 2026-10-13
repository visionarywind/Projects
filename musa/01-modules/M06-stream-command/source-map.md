# Stream 与 Command 调度：源码地图

- 源码根：`source/musa`（本地快照，无 Git 元数据）
- 文档目的：stream command：源码文件/目录到文档章节的映射。
- 适用范围：M06 模块的《Stream 与 Command 调度：源码地图》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：下列路径与区间均已用 `rg -n`/`sed -n` 在本快照核对；区间是"阅读过的范围"，不等于该文件的全部内容。
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 分层地图

| 层 | 路径 | 关注点 | 关键区间 |
|---|---|---|---|
| Driver API | `src/driver/mu_stream.cpp` | Stream/Event/capture 的 C ABI 入口、参数校验、句柄解析 | 创建 17-42；同步/查询 152-219；属性 221-292；callback/event 329-389；capture 391-639；batch mem op 641-892；`_ptsz` 变体 894-962 |
| Driver API | `src/driver/mu_event.cpp` | event create/record/sync/query/IPC | 内部 record 实现 9-57；对外入口 62-214；IPC 216-273 |
| Driver API | `src/driver/mu_memory.cpp` | `muapiMemcpy*` 入口，转 `Context::GeneralMemcpy` | 同步/异步 memcpy 1035-1055 |
| Driver API | `src/driver/mu_module.cpp` | `muapiLaunchKernel*` 入口，转 `Context::GeneralLaunchKernel` | 231-268 |
| Core | `src/musa/core/stream.h` | `Stream` 类、字段、常量、`IStream` 覆写声明 | 类 28-260；常量 183-186；队列字段 195-203；线程/锁 205-214 |
| Core | `src/musa/core/stream.cpp` | capture 状态机、入队、两条后台线程、cmd buffer 池、引擎配额 | capture 39-156；查询/同步 210-239；event 命令 416-504；入队 1056-1127；`AsyncSubmit` 1159-1338；`AsyncWait` 1340-1549；kernel/copy 1568-1582；event 路径 1627-1808；池与配额 1810-1968 |
| Core | `src/musa/core/context.cpp` | 句柄解析、默认流/barrier 流、依赖解析、capture 分支、event 走 barrier 流 | `InfoStream` 558-600；kernel 631-677；memcpy 702-734；同步 997-1017；流创建/销毁 1198-1232；per-thread 默认流 1238-1277；graph capture 2424-2457；`Init` 1906-1960；`Dispose` 1984-2010；依赖解析 2012-2084；默认流同步/等待/重置 2086-2144；event 2706-2734 |
| Core | `src/musa/core/command/command.h` | `Command` 基类、`Status`/`MergeLevel`/`Type`、依赖与 semaphore 字段 | 类 33-266；状态 35-42；类型 57-75；合批条件 110-115；字段 216-245 |
| Core | `src/musa/core/command/command.cpp` | 状态推进、`Build`/`Wait`/`Submit`、semaphore 展开、时间戳与 MUpti | 构造 34-81；`SetStatus` 92-120；依赖记录 132-166；`Build` 175-234；`Wait` 236-245；host 同步 265-299；submit wait/signal 303-378；`GetHalCmdBuffer` 745-765；`SubmitToQueue` 828-887 |
| Core | `src/musa/core/command/` 子类 | 各类命令的 `Build`/`Submit`/`Postprocess` | `callbackCommand.cpp` 18-44；`SyncMemcpyCommand.cpp` 34-76；`AsyncMemcpyCommand.cpp` 129-200；`dispatchCommand.cpp` 302-347；`barrierCommand.cpp` 8-145；`recordCommand.cpp` 9-131；`graphCommand.cpp` 69-78、433 起 |
| Core | `src/musa/core/platformSettings.cpp` | 调度相关默认值 | `launchBlocking`/`memBlocking` 默认 0 于 72-73；`streamAsyncCapacity` 默认 1024 于 107；环境变量读取 370-371 |
| HAL 接口 | `src/hal/halQueue.h`、`halCmdBuffer.h`、`halSemaphore.h` | `IQueue`/`ICmdBuffer`/`ISemaphore` 抽象与 `QueueSubmitInfo` | 接口头文件，未逐行展开 |
| HAL 实现 | `src/hal/m3d/queue.cpp`、`queue.h` | M3D 提交、wait/signal 拆分、鲁棒性查询 | 构造 40-45；`Init` 94-180；`Submit` 182-372；`Paging` 374-418；`WaitIdle` 420-422；`QueryRobustInfo` 424-545 |
| HAL 实现 | `src/hal/m3d/cmdBuffer.cpp`、`cmdBuffer.h` | cmd buffer 状态机与录制 | 构造 53-65；`Init` 75-143；`Begin` 145-197；`End` 199-211；`Reset` 213-228；`CmdDispatch` 347 起 |
| 观测 | `src/driver/mupti`、`src/driver/mu_entry.cpp` | 命令提交/完成的 accessor、submission 关联 | 命令内调用 `MUpti::RegisterStreamWaitEvent` 等，未逐行展开 |

## 证据锚点

- 入队与两条线程的完整骨架：[src/musa/core/stream.cpp:1056-1549]
- 依赖解析的三条规则与 capture 注入：[src/musa/core/context.cpp:2012-2084]
- 命令状态机与提交边界：[src/musa/core/command/command.cpp:92-120] [src/musa/core/command/command.cpp:828-887]
- HAL 提交：[src/hal/m3d/queue.cpp:182-372]
- Stream 析构时的资源释放顺序：[src/musa/core/stream.cpp:850-912]

## 与其它模块的交界

| 相邻模块 | 交界点 |
|---|---|
| M02 公共 ABI | `muapiStream*`/`muapiEvent*` 导出符号与 `MUstream` 句柄布局 |
| M05 memory | `Stream::CmdMemAlloc`/`CmdMemFree`、`RequestSpillBase` 预留的 spill memory |
| M08 graph/capture | `Stream::CaptureNode` 写入的 `m_CaptureGraph`、`GraphCommand` |
| M09 HAL/kernel | `Hal::IQueue::Submit` 之后的驱动/firmware 行为 |
| M10 instrumentation | `MUpti::RegisterStreamWaitEvent`、submission id 关联、错误快照 |

## 搜索建议

```bash
rg -n "AsyncSubmit|AsyncWait|QueueCommand" src/musa/core
rg -n "ResolveDependencyAndQueueCommand|InfoStream" src/musa/core/context.cpp
rg -n "SubmitToQueue|Build\(" src/musa/core/command
rg -n "^MUresult muapiStream" src/driver/mu_stream.cpp
```

上述命令是阅读建议，不代表已在本地或远端执行。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（2 处，例：[src/musa/core/stream.cpp:1056-1549]）
- `src/musa/core/command/command.cpp`（2 处，例：[src/musa/core/command/command.cpp:92-120]）
- `src/musa/core/context.cpp`（1 处，例：[src/musa/core/context.cpp:2012-2084]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
