# Context 与资源生命周期：例子

- 文档目的：context lifecycle：用法示例与边界/错误输入说明。
- 适用范围：M04 模块的《Context 与资源生命周期：例子》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 静态最小例子（阅读模型）

```text
例子 1：context 创建/同步/销毁
  muInit(0)
  muDeviceGet(&dev, 0)
  muCtxCreate_v2(&ctx, MU_CTX_SCHED_AUTO, dev)
    -> Device::CreateContext -> new Context -> Context::Init
    -> LockedAddContext + LockedAddCtx + TlsCtxPush
  muCtxSynchronize()
    -> Context::Synchronize -> LockedWait（逐流 WaitFinish）
  muCtxDestroy_v2(ctx)
    -> Platform::LockedRemoveCtx -> Device::DestroyContext
    -> ~Context -> Context::Dispose -> ReleaseClientResources
```

```text
例子 2：context 内资源登记模式
  CreateStream -> new Stream(this, flags, priority) -> stream->Init()
               -> AddStream（写锁，分配 m_SeqStreamNum）
  DestroyStream -> RemoveStream（写锁）-> delete stream
  CreateEvent  -> new Event(this, flags) -> event->Init()
               -> AddEvent（写锁）
  DestroyEvent -> RemoveEvent（写锁）-> delete event
```

```text
例子 3：primary context Reset（重建而非销毁）
  muDevicePrimaryCtxReset_v2(dev)
    -> Context::Reset -> LockedReset
    -> Dispose（清理旧资源）-> 重建 m_DefaultStream + m_BarrierStream
```

```text
例子 4：流句柄哨兵解析（legacy / per-thread）
  Context::InfoStream(ctx, inStream)
    inStream == nullptr || MU_STREAM_LEGACY   -> GetDefaultStream()
    inStream == MU_STREAM_PER_THREAD          -> GetPerThreadDefaultStream()
                                                 （thread_local map 惰性 new Stream + Init + AddStream）
    其它                                      -> FindStream 命中才返回该流，否则保持空指针
```

## 关键行为示例

- 默认流语义：`ResolveDependencyAndQueueCommand` 中，命令入默认流会记录所有非非阻塞流的 `LastCommand` 依赖。[src/musa/core/context.cpp:2032-2042]
- 逐流同步：`Context::LockedWait` 对 `Streams()` 逐个 `WaitFinish`，任一失败即返回。[src/musa/core/context.cpp:2103-2118]
- 池化 Array：`ActivateArray` 从 `m_ArrayPool` 取空闲 `Array`，无空闲则 `new` 并插入池。[src/musa/core/context.cpp:401-421]
- 哨兵解析：`Context::InfoStream` 把 `MU_STREAM_PER_THREAD` 分派到 `GetPerThreadDefaultStream`，`nullptr`/legacy 分支取 `GetDefaultStream`，其余走 `FindStream` 校验。[src/musa/core/context.cpp:567-576]
- per-thread 默认流：`GetPerThreadDefaultStream` 用 thread_local map 按 `device id << 32 | ctx id` 惰性创建；源码注释写明该流不随线程退出销毁，而是在对应 context 销毁前才被同步与销毁。[src/musa/core/context.cpp:1238-1277]
- 销毁顺序：`ReleaseClientResources` 先逐个 `delete` 除 `pDefaultStream`/barrier 之外的流（per-thread 默认流包含在内），再按固定顺序删 barrier 与默认流。[src/musa/core/context.cpp:343-354]
- 析构期同步：`Stream::~Stream` 在非 detached 时先让默认流与 barrier 流 `WaitFinish`，随后才停 submit/wait 线程并回收队列资源。[src/musa/core/stream.cpp:850-877]
- 引用计数示例：`Context::Release` 把计数减到 0 时只把 `m_IsActivate` 置 false，`Context::Reset` 则走 `LockedReset` 重建内部流；两者都不直接 `Dispose` 资源。[src/musa/core/context.cpp:926-943]
- 边界输入：`muapiCtxPopCurrent_v2` 只做 `TlsCtxPop` 弹栈不做引用计数，栈为空时返回 `MUSA_ERROR_INVALID_CONTEXT`，`pctx` 为空也视为合法。[src/driver/mu_context.cpp:224-241]
- 销毁流输入：`Context::DestroyStream` 先回调 `IssueStreamDestroyStartingResourceCallback`，写锁内 `RemoveStream` 后直接 `delete stream`，不等待该流未完成命令。[src/musa/core/context.cpp:1219-1228]

## 验证说明

以上是源码阅读模型，不是已运行的示例。可用 D01 `muInfo` 验证查询类路径，用 D02/D04 验证异步/Graph 路径。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（8 处，例：[src/musa/core/context.cpp:1238-1277] `GetPerThreadDefaultStream`）
- `src/musa/core/stream.cpp`（1 处，例：[src/musa/core/stream.cpp:850-877] `Stream::~Stream`）
- `src/driver/mu_context.cpp`（1 处，例：[src/driver/mu_context.cpp:224-241] `muapiCtxPopCurrent_v2`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
