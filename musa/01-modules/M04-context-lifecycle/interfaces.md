# Context 与资源生命周期：接口契约

- 文档目的：context lifecycle：接口签名、前置条件、错误码与跨层契约。
- 适用范围：M04 模块的《Context 与资源生命周期：接口契约》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 接口方向表

| 接口方向 | 输入 | 输出/副作用 | 前置条件 | 错误 |
|---|---|---|---|---|
| 驱动 -> `Device::CreateContext` | `IContext**`、flags | 新 context、登记、TLS 压栈 | device 有效、flags 合法 | `MUSA_ERROR_INVALID_VALUE` |
| 驱动 -> `Device::DestroyContext` | `MUcontext` | 从 device 集合移除并析构 | context 在 `FindCtx` 集合中 | `MUSA_ERROR_CONTEXT_IS_DESTROYED` |
| 本模块 -> `Context::Init` | createDefaultStream 标志 | CriticalBase、内部流、库加载 | 构造完成 | 传播/`MUSA_ERROR_OUT_OF_MEMORY` |
| 本模块 -> `Context::Synchronize` | 无 | 等待全部流完成 | context 有效 | `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED` |
| 本模块 -> `Context::Dispose` | 无 | 清理全部登记资源 | 析构/Reset 路径 | 无返回（void） |
| 本模块 -> `Context::CriticalBase` | 资源指针 | 登记/查找/摘除 | 已取得读写锁 | 无返回或 bool |

## 入口契约

- `muapiCtxCreate_v2`：`pctx` 非空、`dev` 有效、`flags` 在 `MU_CTX_FLAGS_MASK` 内，成功后把新 context 压入 TLS 并登记到 Platform。[src/driver/mu_context.cpp:156-182]
- `muapiCtxDestroy_v2`：禁止销毁 primary context（返回 `MUSA_ERROR_INVALID_CONTEXT`），当前 context 需先 `TlsCtxPop`，再 `LockedRemoveCtx` 与 `DestroyContext`。[src/driver/mu_context.cpp:188-218]
- `Context::Release`/`Retain` 只用于 primary context；`Release` 在 `m_RefCount==0` 时返回 `MUSA_ERROR_INVALID_CONTEXT`，归零后置 `m_IsActivate=false`。[src/musa/core/context.cpp:926-949]
- `Context::SetFlags` 把 `MU_CTX_SCHED_*` 映射为 `Util::ScheduleMode`（AUTO 按 context 数量在 Yield/Spin 间选择），非法 flag 返回 `MUSA_ERROR_INVALID_VALUE`。[src/musa/core/context.cpp:951-982]
- 错误码沿 `MUresult` 传播；HAL 错误经 `HalToMuResult` 转换。[src/musa/core/internal_funcs.h:22-40]

## 观测方向

| 观测 -> 本模块 | accessor/query 请求 | ID、时间戳、metadata | 对象仍存活 | 访问失败/空值 |

公共 ABI 兼容性是 M02 的硬约束；其他模块应避免直接改变句柄布局和导出表顺序。此处的“应”是开发约束，不是源码事实。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/driver/mu_context.cpp`（2 处，例：[src/driver/mu_context.cpp:156-182]）
- `src/musa/core/context.cpp`（2 处，例：[src/musa/core/context.cpp:926-949]）
- `src/musa/core/internal_funcs.h`（1 处，例：[src/musa/core/internal_funcs.h:22-40]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
