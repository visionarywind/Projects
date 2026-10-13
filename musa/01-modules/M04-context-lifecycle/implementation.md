# Context 与资源生命周期：实现说明

- 文档目的：context lifecycle：核心函数/流程的逐段实现解读。
- 适用范围：M04 模块的《Context 与资源生命周期：实现说明》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 正常实现路径

```text
Device::CreateContext -> Context 构造 -> Context::Init（CriticalBase + 默认流 + barrier 流 + 库）
-> 资源登记（AddStream/AddEvent/AddMemory/RecordModule/AddGraph/AddGraphExec）
-> Context::Synchronize -> Context::Dispose/析构
```

源码证据：[src/musa/core/device.cpp:635-675] [src/musa/core/context.cpp:1906-1965] [src/musa/core/context.cpp:1967-2005]

## 关键实现观察

- `Device::CreateContext` 用 `ReadLockedAccessor<Device::CriticalBase>` 读 `ctxs()` 数量决定调度模式，然后 `new Context` + `Init`，失败即 `delete ctx`，不登记。[src/musa/core/device.cpp:635-674]
- `Context::Init` 的失败点（默认流/barrier 流创建失败）直接返回 `MUSA_ERROR_OUT_OF_MEMORY`，此时 `CreateContext` 会 `delete ctx` 回滚。[src/musa/core/context.cpp:1917-1953]
- 资源登记采用“写锁 + 集合插入 + 日志”，例如 `AddStream` 同时分配 `m_SeqStreamNum` 作为流序号。[src/musa/core/context.cpp:97-108]
- `Context::Synchronize` 先检查所有流的 capture 状态，再 `LockedWait`；capture 冲突会把流标记为 `InvalidateCaptureStatus`。[src/musa/core/context.cpp:997-1015]
- `Context::Dispose` 分三步：通知库卸载 → 摘除 GraphExec 并同步全部流 → `ReleaseClientResources` 强制清理。[src/musa/core/context.cpp:1984-2005]
- `Context::CriticalBase::ReleaseClientResources` 对 texture/surface/graphics resource/mipmapped array/event/stream/memory/external 资源逐个检查并打 WARNING 清理。[src/musa/core/context.cpp:293-377]

## 分支矩阵

| 分支 | 触发 | 结果 | 当前证据 |
|---|---|---|---|
| 参数/句柄无效 | `pctx` 为空、device 无效、flags 越界 | `MUSA_ERROR_INVALID_VALUE`/`MUSA_ERROR_INVALID_DEVICE` | 已确认于 `muapiCtxCreate_v2` |
| 初始化失败 | `Context::Init` 内流创建失败 | 不登记、`delete ctx` 回滚 | 已确认于 `Device::CreateContext` |
| context 已销毁 | `DestroyContext` 找不到 `FindCtx` | `MUSA_ERROR_CONTEXT_IS_DESTROYED` | 已确认于 `Device::DestroyContext` |
| capture 模式 | stream 处于 ACTIVE(严格) 或 INVALIDATED | `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED` | 已确认于 `Context::Synchronize` |
| 单资源销毁 | `DestroyGraph`/`DestroyGraphExec` | 摘除后 `LockedWait` 再 `delete` | 已确认 |
| primary 专属 | `Retain`/`Release`/`Reset` | 改 `m_RefCount`/`m_IsActivate`/重建 | 已确认于 `Context::Release` 等 |
| 跨设备/特殊类型 | peer、managed、IPC、external | `MapToPeers` 选专门 HAL 路径 | 部分确认于 `MapToPeers` |

## 尚未证明

- 所有错误分支是否都释放中间对象（例如 `CreateTexture` 各 Init 阶段失败都 `delete pTexture`，但这类分支的完整覆盖未逐一验证）。
- `Context::Reset` 重建路径与 HAL 状态（copy manager、L2 持久属性）是否完全对齐，需目标机验证。
- 底层错误映射 `HalToMuResult` 的完整表项未在本模块逐一核对。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（7 处，例：[src/musa/core/context.cpp:1906-1965]）
- `src/musa/core/device.cpp`（2 处，例：[src/musa/core/device.cpp:635-675]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
