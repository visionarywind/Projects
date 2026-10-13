# Context 与资源生命周期：调用链

- 文档目的：context lifecycle：关键调用链：从公共入口到真实副作用的追踪。
- 适用范围：M04 模块的《Context 与资源生命周期：调用链》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 创建链

```text
muapiCtxCreate_v2
 -> InitPlatform / Platform::Get().GetDevice(dev)
 -> Device::CreateContext
 -> new Context（构造函数初始化 m_CriticalData、m_DefaultStream=nullptr）
 -> Context::Init
    -> Context::CriticalBase::Init（创建 Array 池、m_Peers.resize）
    -> 创建 m_DefaultStream（Stream + Init + AddStream）
    -> 库集合 LoadToContext
    -> 创建 m_BarrierStream（Stream + Init + AddStream）
 -> LockedAddContext / Platform::LockedAddCtx / TlsCtxPush
 -> MUresult
```

- 驱动入口 `muapiCtxCreate_v2` 校验句柄、device 与 flags，然后调 `pDevice->CreateContext`，成功后把新 context 压入 TLS 并登记到 Platform。[src/driver/mu_context.cpp:156-182]
- `Device::CreateContext` 依据 `MU_CTX_SCHED_*` 选 `Util::ScheduleMode`，创建并 `Init` 后 `LockedAddContext` + `SetWaitMode` + `Retain`；失败时 `delete ctx`。[src/musa/core/device.cpp:635-675]
- `Context::Init` 先初始化 `CriticalBase`，再按需创建默认流，然后加载库、创建 barrier 流，成功才发 `IssueContextCreatedResourceCallback`。[src/musa/core/context.cpp:1906-1965]

## 同步链

```text
muapiCtxSynchronize
 -> Context::Synchronize
    -> 读锁遍历流检查 capture 状态
    -> Context::LockedWait（逐流 WaitFinish）
 -> Device::LockedSynchronizeGreenContexts（当当前 context 是 primary）
```

- `Context::Synchronize` 只读遍历 `Streams()`，发现 ACTIVE（需严格检查）或 INVALIDATED 的 capture 状态即失败并 `InvalidateCaptureStatus`，否则转 `LockedWait`。[src/musa/core/context.cpp:997-1015]
- `LockedWait` 逐流调用 `WaitFinish`，包一层 MUpti context-synchronize 观测区间。[src/musa/core/context.cpp:2103-2118]
- 若当前 context 是 primary，驱动层还会同步该设备上的所有 green context。[src/driver/mu_context.cpp:135-154]

## 销毁链

```text
muapiCtxDestroy_v2
 -> Platform::LockedRemoveCtx
 -> Device::DestroyContext
 -> delete Context -> ~Context -> Context::Dispose
    -> 库集合 UnloadFromCtx
    -> 摘除并删除 GraphExec
    -> 同步所有流（WaitFinish）
    -> Context::CriticalBase::ReleaseClientResources
 -> MUresult
```

- `Device::DestroyContext` 先在 device 集合中 `FindCtx`，再 `LockedRemoveContext` 并 `delete`。[src/musa/core/device.cpp:677-688]
- `Context::Dispose` 通知库卸载、在写锁下摘走 `graphExecs` 并同步所有流、清理遗留 GraphExec，最后 `ReleaseClientResources`。[src/musa/core/context.cpp:1967-2005]
- `ReleaseClientResources` 按 texture/surface/graphics resource/mipmapped array/event/stream/memory/external semaphore/external memory 顺序强制清理。[src/musa/core/context.cpp:293-377]

## 单资源销毁链

- stream：`DestroyStream` → `RemoveStream`（写锁）→ `delete`。[src/musa/core/context.cpp:1219-1228]
- event：`DestroyEvent` → `RemoveEvent`（写锁）→ `delete`。[src/musa/core/context.cpp:1334-1347]
- memory：`DestroyMemory` → `RemoveMemory`（写锁，取 parent ctx）→ `UntrackMemory`。[src/musa/core/context.cpp:1080-1088]
- module：`DestroyModule` → 逐符号清理 texture/surface 引用 → `RemoveModule` → `delete`。[src/musa/core/context.cpp:1288-1313]
- graph/graphExec：先 `RemoveGraph`/`RemoveGraphExec`，再 `LockedWait` 后 `delete`。[src/musa/core/context.cpp:1618-1659] [src/musa/core/context.cpp:1690-1738]

## 链路缺口

- `Stream::WaitFinish`、`Stream::Init` 与 `Memory::Init` 的内部实现属于 M06/M05 范围，本模块只确认它们在 context 生命周期中的调用点。
- HAL 侧 `Hal::IDevice`/queue 的实际创建副作用不可见（M09 范围），需在目标源码补证。
- 若涉及动态导出或回调，需同时检查 `mu_entry.cpp` 和生成脚本。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（11 处，例：[src/musa/core/context.cpp:1906-1965]）
- `src/driver/mu_context.cpp`（2 处，例：[src/driver/mu_context.cpp:156-182]）
- `src/musa/core/device.cpp`（2 处，例：[src/musa/core/device.cpp:635-675]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
