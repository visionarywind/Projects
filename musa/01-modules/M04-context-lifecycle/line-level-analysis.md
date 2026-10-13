# Context 与资源生命周期：行级分析

- 文档目的：把 M04 的关键代码逐行落到可核对的引用上，避免把"读过"扩写成"已确认"。
- 适用范围：M04 模块的《Context 与资源生命周期：行级分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态分析，未验证。所有行号来自本地快照 `source/musa`，无运行、无硬件、无测试。
- 最后更新：2026-10-07
- 前置阅读：[`call-chains.md`](call-chains.md)、[`implementation.md`](implementation.md)、[`risks-and-debt.md`](risks-and-debt.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 证据表：driver 层入口

| 行号 | 代码意图 | 前置条件与输入 | 输出与状态变化 | 调用关系 | 并发与所有权 | 风险 |
|---|---|---|---|---|---|---|
| [src/driver/mu_context.cpp:135-154] `muapiCtxSynchronize` | 同步当前上下文 | `InitPlatform()` 成功且 `TlsCtxTop()` 非空 | 成功时 primary context 再同步全部 green context | `TlsCtxTop` → `Context::Synchronize` → `LockedSynchronizeGreenContexts` | 只读当前线程 TLS | 多线程各自持不同 ctx 时结果不同 |
| [src/driver/mu_context.cpp:156-182] `muapiCtxCreate_v2` | 创建并压入用户 context | flags 掩码合法 | 新 context 入栈并设为当前 | 参数校验 → `Device::CreateContext` → `CtxPush` | 调用线程私有 TLS 栈 | 失败时是否已出栈需逐行核对 |
| [src/driver/mu_context.cpp:188-218] `muapiCtxDestroy_v2` | 销毁当前 context | `TlsCtxTop()` 命中待删 context | 先 `LockedRemoveCtx` 再 `DestroyContext`，随后才从 TLS 栈弹出 | `Platform::LockedRemoveCtx` → `Device::DestroyContext` | 只在调用线程栈顶匹配时 `TlsCtxPop` | 其它线程栈内残留裸指针 |
| [src/driver/mu_context.cpp:224-241] `muapiCtxPopCurrent_v2` | 弹出当前 context | `TlsCtxTop()` 非空 | 可选写出旧值后 `TlsCtxPop` | `TlsCtxTop` → `TlsCtxPop` | 不做引用计数，只动栈 | 弹栈不等于销毁，引用仍由 `Retain`/`Release` 管 |
| [src/driver/mu_context.cpp:396-429] `muapiCtxDetach` | 释放外部持有的一份引用 | ctx 合法 | `Release()` 可能把引用计数减到 0 | `Context::Release` | 跨线程共享计数 | 与 `muapiCtxDestroy_v2` 并发时语义需实测 |
| [src/driver/mu_context.cpp:472-489] `muapiDevicePrimaryCtxRetain` | 取得并保留 primary context | 设备存在且 `pctx` 非空 | 写出 primary 指针并 `Retain()` | `IDevice::GetPrimaryContext` → `Context::Retain` | 计数非 CAS | 与 Release 并发存在竞态 |

## 证据表：device 层

| 行号 | 代码意图 | 前置条件与输入 | 输出与状态变化 | 调用关系 | 并发与所有权 | 风险 |
|---|---|---|---|---|---|---|
| [src/musa/core/device.cpp:635-675] `Device::CreateContext` | 新建 context 并登记 | 无 | 成功：`LockedAddContext` + `SetWaitMode` + `Retain`；失败：`delete ctx` | `Context::Context` → `Context::Init` → `LockedAddContext` | 读锁统计 ctx 数量后再写入集合 | `delete ctx` 走析构链，依赖 `Dispose` 覆盖不完整初始化 |
| [src/musa/core/device.cpp:642-661] `Util::ScheduleMode` | 按调度 flags 选等待模式 | `flags & MU_CTX_SCHED_MASK` | 得到 `Spin`/`Yield`/`Sleep`，非法值返回 `INVALID_VALUE` | `switch(schedFlags)` | 无锁，纯参数推导 | `AUTO` 分支依赖 ctx 总数，结果随并发变化 |
| [src/musa/core/device.cpp:677-688] `Device::DestroyContext` | 从集合摘除并删除 context | `FindCtx` 命中 | 先 `LockedRemoveContext` 再 `delete` | `CriticalBase::FindCtx` → `LockedRemoveContext` → `Context::~Context` | 读锁检查、写锁摘除 | 检查与摘除不是同一临界区 |
| [src/musa/core/device.cpp:718-748] `Device::~Device` | 按序清理用户 context 与 primary | 设备销毁 | 集合内用户 context 先删，`m_PrimaryCtx` 后 `reset` | 遍历 `ctxs()` → `DestroyContext` | 设备级写锁 | 顺序反转会留下悬垂引用 |

## 证据表：context 核心

| 行号 | 代码意图 | 前置条件与输入 | 输出与状态变化 | 调用关系 | 并发与所有权 | 风险 |
|---|---|---|---|---|---|---|
| [src/musa/core/context.cpp:41-74] `Context::CriticalBase` | 构造/析构全部资源集合 | 无 | 构造清空集合，析构清理未卸载 module | 直接操作 `m_Streams` 等容器 | 受 `ReadLockedAccessor` 保护 | 析构顺序未在文档中固化 |
| [src/musa/core/context.cpp:76-91] `CriticalBase::Init` | 预建 Array 池与 peer 表 | 设备信息可用 | 生成 `MUSA_INIT_ARRAY_POOL` 个 Array，`m_Peers` resize | `Context::Init` 调用 | 初始化期独占 | 失败时已建对象的回收路径需逐行核对 |
| [src/musa/core/context.cpp:93-126] `CriticalBase::AddStream` | 登记 stream 并分配序号 | 无 | `m_SeqStreamNum++` 后插入 `m_Streams` | `FindStream`/`AddStream`/`RemoveStream` 对称 | 所有集合操作都必须持写锁 | 登记与创建不是原子的，中间失败即泄漏登记 |
| [src/musa/core/context.cpp:293-377] `ReleaseClientResources` | 按类型强制清理并打 WARNING | context 析构期 | 逐类 `delete` 后清空容器 | `Dispose` 调用 | 持有 context 写锁 | 无条件删除内部流，即使 `Init` 未完成 |
| [src/musa/core/context.cpp:343-354] `pDefaultStream` | 先删 barrier 流再删默认流 | `m_BarrierStream`/`m_DefaultStream` 非空 | 两个内部流被释放 | `Context::Dispose` → `ReleaseClientResources` | 写锁内同步 | `Init` 中途失败时这两个指针可能处于半初始化 |
| [src/musa/core/context.cpp:926-949] `Context::Release` | 引用计数递减 | 无 | 归零只把 `m_IsActivate` 置 false，不释放资源 | `Context::Retain` 对称 | 计数是普通成员而非原子 | 与 `Retain`/`Destroy` 并发存在竞态 |
| [src/musa/core/context.cpp:997-1015] `Context::Synchronize` | 同步本 context 全部流 | capture 检查通过 | 作用域结束释放读锁后再调 `LockedWait` | `Context::LockedWait` | 读锁与 `LockedWait` 是先后两次取锁，不嵌套 | 两次取锁之间状态可能变化 |
| [src/musa/core/context.cpp:1198-1217] `CreateStream` | 创建并登记 stream | 参数合法 | `Init` 失败 `delete stream` 不登记，成功才 `AddStream` | `Context::AddStream` | 写锁内登记 | 与 `DestroyStream` 并发需实测 |
| [src/musa/core/context.cpp:1219-1236] `DestroyStream` | 摘除并删除 stream | `FindStream` 命中 | 直接 `delete stream` | `Context::RemoveStream` | 不等待流上未完成命令 | 未排空命令即释放 |
| [src/musa/core/context.cpp:1238-1278] `GetPerThreadDefaultStream` | 惰性建 per-thread 默认流 | thread_local map 命中即返回 | 首次调用插入 map 并登记到 context | `Context::CreateStream` | thread_local，不跨线程 | 线程退出不清理，生命周期随 context |
| [src/musa/core/context.cpp:1288-1313] `DestroyModule` | 卸载 module 并按符号清理资源 | `FindModule` 命中 | `IterateSymbols` 回调里逐个 `fnRemoveRef` | `Module::IterateSymbols` | 回调内取 context 写锁 | 回调内嵌取锁，锁序需审计 |
| [src/musa/core/context.cpp:1334-1347] `DestroyEvent` | 摘除并删除 event | `FindEvent` 命中 | 直接 `delete event`，源码留有 `FIXME: not sync yet?` | `Context::RemoveEvent` | 无同步点 | 事件尚未完成即释放 |
| [src/musa/core/context.cpp:1879-1899] `Context::Context` | 初始化成员默认值 | 无 | `m_SeqID=UINT32_MAX`，内部流指针置空 | 无 | 构造期独占 | 无 |
| [src/musa/core/context.cpp:1906-1965] `Context::Init` | 建默认流、载库、建 barrier 流 | 任一步失败即 `break` | 已建对象留给析构链回收 | `CreateStream` → `LoadToContext` → 回调 | 未完成初始化即可能被 `delete ctx` | 中途失败的回滚完整性未验证 |
| [src/musa/core/context.cpp:1967-2005] `Context::Dispose` | 卸载库、摘 GraphExec、同步、清理 | 无 | 逐项释放后调 `ReleaseClientResources` | `LockedWait` → `ReleaseClientResources` | 持 context 写锁，期间阻塞等待流完成 | 写锁内长时间阻塞 |
| [src/musa/core/context.cpp:2012-2084] `ResolveDependencyAndQueueCommand` | 为新命令解析依赖并入队 | 命令与流参数合法 | 持 `pfmSerialLock` 遍历同 context 全部流 | `Context::QueueCommand` | 全局序列锁 + 默认流/barrier 流两套遍历 | 每次入队都是全局扫描，热点 |
| [src/musa/core/context.cpp:2103-2118] `Context::LockedWait` | 逐流等待完成 | 持读锁 | 每条流 `WaitFinish` | `Stream::WaitFinish` | 读锁内阻塞 | 阻塞时长由 HAL 决定，不可静态证明 |
| [src/musa/core/context.cpp:2120-2144] `Context::LockedReset` | 重建内部流与 L2 属性 | 持写锁 | 重建默认流/barrier 流并 `SetLLCPersistingProperties` | `Context::Dispose` → 重建 | 写锁内完成 | HAL 级状态是否完整对齐未验证 |

## 证据表：登记与 TLS

| 行号 | 代码意图 | 前置条件与输入 | 输出与状态变化 | 调用关系 | 并发与所有权 | 风险 |
|---|---|---|---|---|---|---|
| [src/musa/core/platform.cpp:430-452] `Platform::ValidateContext` | 判定 context 是否仍可用 | 无 | 要求 `GetRefCount()!=0` 且在 `ctxs` 集合内 | `LockedRemoveCtx` → `LockedFindCtx` → `ValidateContext` | 平台级锁 | 计数归零与摘除不同步时判据互相矛盾 |
| [src/musa/core/context.h:257-294] `m_Streams` | 声明全部裸指针容器 | 无 | 无 | `CriticalBase` 成员 | 全靠外层读写锁 | 所有权必须成对登记/摘除 |
| [src/driver/internal.h:232-234] `TlsCtxTop` | 读当前线程 context 栈顶 | TLS 数据存在 | 返回栈顶或空 | `TlsCtxPush`/`TlsCtxPop` | 线程局部 | 栈内是裸指针，跨线程悬垂 |
| [src/musa/core/context.cpp:1280-1286] `CreateModule` `RecordModule` | 新建 module 并登记进 context | 无参数校验 | `new Module` 后写锁内 `RecordModule` | `CriticalBase::RecordModule` | 写锁内登记 | 登记与装载不同步，装载失败时对象已登记 |

## 下游范围（不在本页逐行展开）

- HAL 层：[src/hal/m3d/queue.cpp:182-372] `Queue::Submit`（`Result Queue::Submit`）——构建/接口已确认，硬件副作用未知。
- 流等待：[src/musa/core/context.cpp:2103-2118] `WaitFinish` 的内部实现与超时行为。
- 内存初始化：`Memory::Init` 属 M05 范围，本页不引用其行号。

## 阅读方法

1. 先看参数检查和 early return（`muapiCtxCreate_v2` 的 NULL/掩码检查、`Device::DestroyContext` 的 `FindCtx`）。
2. 再看对象创建、登记、引用计数（`Retain`/`Release`、`m_RefCount`）和集合更新。
3. 最后看 HAL 调用（`HalToMuResult`）、异步入队（`QueueCommand`）、错误回滚（`delete ctx`）和析构（`Dispose`/`ReleaseClientResources`）。
4. 每遇到"成功才登记"的分支，同时找它的失败分支，确认半初始化对象不会进入集合。

本页不把没有逐行读取的代码扩写成确定行为；行号随源码演进会漂移，改动后需重跑行号校验。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（20 处，例：[src/musa/core/context.cpp:41-74]）
- `src/driver/mu_context.cpp`（6 处，例：[src/driver/mu_context.cpp:135-154]）
- `src/musa/core/device.cpp`（4 处，例：[src/musa/core/device.cpp:635-675]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
