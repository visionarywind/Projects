# Context 与资源生命周期：数据结构与所有权

- 文档目的：context lifecycle：核心数据结构、字段含义与持有关系。
- 适用范围：M04 模块的《Context 与资源生命周期：数据结构与所有权》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 结构关系

```text
IContext（musaContext.h:42，抽象接口）
  └-> Context（context.h:43）
       ├-> m_Device -> Device（父设备）
       ├-> m_CriticalData : CriticalBase（context.h:185，ReadWriteLockedBase<std::shared_mutex>）
       │    ├-> m_Streams / m_Events / m_Textures / m_Surfaces
       │    ├-> m_Memories / m_LoadedModules / m_Graphs / m_GraphExecs
       │    ├-> m_ArrayPool / m_MipmappedArrays / m_GraphicsResources
       │    ├-> m_UserObjectMap / m_ConditioalHandle
       │    └-> m_ExternalSemaphores / m_ExternalMemories / m_Peers
       ├-> m_DefaultStream / m_BarrierStream（内部流）
       └-> m_RefCount / m_IsActivate / m_WaitMode / m_SeqID
```

`CriticalBase` 的集合字段全部是裸指针（`unordered_set<T*>`），`Context` 本体用 `unique_ptr` 管理 `m_PerfExperiment` 等少数对象。[src/musa/core/context.h:257-363]

## 关键字段

| 字段 | 声明位置 | 作用 |
|---|---|---|
| `m_CriticalData` | context.h:350 | context 级资源登记表的锁容器 |
| `m_Streams`/`m_Events`/`m_Textures` | context.h:262-268 | stream/event/texture 登记 |
| `m_Memories` | context.h:278 | memory 登记 |
| `m_LoadedModules` | context.h:272 | 已加载 module 登记 |
| `m_Graphs`/`m_GraphExecs` | context.h:280-282 | graph/graphExec 登记 |
| `m_DefaultStream`/`m_BarrierStream` | context.h:347-349 | 默认流与 barrier 流 |
| `m_Peers` | context.h:293 | 按 device 序号的 peer 映射 flag |
| `m_RefCount`/`m_IsActivate` | context.h:351-352 | primary context 引用/激活状态 |

上表行号来自 [src/musa/core/context.h:262-352]。

## 所有权问题

- `Context::CriticalBase` 是 context 全部资源的登记表；字段 `m_Streams` 等均为裸指针集合，析构时由 `ReleaseClientResources` 强制清理。[src/musa/core/context.h:185-293]
- `m_DefaultStream` 与 `m_BarrierStream` 由 `Context` 直接持有（成员裸指针），在 `ReleaseClientResources` 中最后删除（先普通流、再 barrier、最后 default）。[src/musa/core/context.cpp:293-354]
- `Memory` 同时被 context（`m_Memories`）与 Platform MemoryTracker 追踪；`DestroyMemory` 只移除 context 登记并 `UntrackMemory`。[src/musa/core/context.cpp:1080-1088]
- `Device` 与 `Context` 是 1:N：device 持有一个 `m_PrimaryCtx`（unique_ptr）+ `ctxs()` 集合；每个 context 只保存一个父 `m_Device` 指针。[src/musa/core/device.h:137-138] [src/musa/core/device.h:210-211] [src/musa/core/device.h:247] [src/musa/core/context.h:343-363]
- `m_RefCount` 与 `m_IsActivate` 控制 primary context 的引用/激活状态（`Context::Release`/`Retain`/`Reset`）。[src/musa/core/context.h:346-352]
- `Module` 由 `m_LoadedModules` 登记；context 析构会强制 `delete` 用户忘卸的 module 并打 WARNING。[src/musa/core/context.cpp:63-74]
- Array 使用池化：`CriticalBase::Init` 预建 `MUSA_INIT_ARRAY_POOL` 个 `Array`，`CreateArray` 从池中激活、`DestroyArray` 回池。[src/musa/core/context.cpp:76-91] [src/musa/core/context.cpp:401-421] [src/musa/core/context.cpp:1126-1129]

具体模块的字段布局以对应头文件为准；本文不臆测未读字段。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（6 处，例：[src/musa/core/context.cpp:293-354]）
- `src/musa/core/context.h`（5 处，例：[src/musa/core/context.h:257-363]）
- `src/musa/core/device.h`（3 处，例：[src/musa/core/device.h:137-138]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
