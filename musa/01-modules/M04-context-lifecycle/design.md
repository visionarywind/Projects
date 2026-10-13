# Context 与资源生命周期：设计分析

- 文档目的：context lifecycle：设计意图、职责边界与关键取舍。
- 适用范围：M04 模块的《Context 与资源生命周期：设计分析》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


## 设计问题

本模块把 `Device::CreateContext -> Context::Init -> 资源登记 -> Synchronize/Dispose` 拆成稳定边界：

- 驱动入口（`muapiCtxCreate_v2`/`muapiCtxDestroy_v2`）负责参数校验、TLS 与 Platform 登记；
- `Context`/`Context::CriticalBase` 负责状态、资源集合与生命周期；
- `Device` 负责 1:N 持有 context 与 primary context 初始化；
- HAL 只负责具体资源的创建/提交/销毁。

这样减少了公共 ABI 对硬件实现的直接依赖，但也形成多层错误传播（`MUresult` 沿 `HalToMuResult` 向上）和生命周期耦合（context 析构必须级联清理所有登记资源）。[src/musa/core/device.cpp:635-675] [src/musa/core/context.cpp:1906-2005] [src/musa/core/internal_funcs.h:22-40]

## 设计取舍

| 取舍 | 好处 | 代价 | 证据状态 |
|---|---|---|---|
| `CriticalBase` 单点登记资源 | 统一查找/清理，`Synchronize`/`Dispose` 可全量遍历 | 所有集合裸指针，任何登记遗漏都会泄漏 | 已确认 |
| `ReadWriteLockedBase<std::shared_mutex>` 读写锁 | 读路径（查找/遍历）并发、写路径互斥 | 锁序和嵌套写锁风险（写锁内再取写锁需谨慎） | 已确认 |
| primary context 由 `Device` 以 `unique_ptr` 持有 | 生命周期与 device 绑定、`GetPrimaryContext` 简单 | 与用户 context 登记路径不一致，Reset 语义特殊 | 已确认 |
| 默认流 + barrier 流由 `Context` 直接持有 | 默认流语义（依赖所有非非阻塞流）实现简单 | barrier 流被 `RecordEvent`/`WaitEvent` 复用，职责重 | 已确认 |
| `RefCount`/`IsActivate` 管理 primary context | 匹配 `muDevicePrimaryCtxRetain/Release/Reset` 语义 | 用户 context 不使用该计数，两套激活逻辑并存 | 部分确认 |
| Array 池化（`m_ArrayPool`） | 减少分配/释放开销、`Retire` 复用 | 失败路径需正确 `Retire`，池中对象由析构统一删除 | 已确认 |

## 不变量

1. 公共句柄在使用前必须能映射到有效对象；`Platform::ValidateContext` 用 `LockedFindCtx` + `GetRefCount` 校验。[src/musa/core/platform.cpp:448-452]
2. `Context::Init` 成功前不会对外暴露 context：`Device::CreateContext` 在 `Init` 失败时 `delete ctx`，不登记。[src/musa/core/device.cpp:663-674]
3. `Context::Synchronize` 先检查 stream capture 状态，再逐流 `WaitFinish`，capture 冲突时返回 `MUSA_ERROR_STREAM_CAPTURE_UNSUPPORTED` 并 `InvalidateCaptureStatus`。[src/musa/core/context.cpp:997-1015]
4. 销毁路径必须先摘除登记（写锁）并同步，再释放对象；`Dispose` 对全部 stream `WaitFinish` 后才 `ReleaseClientResources`。[src/musa/core/context.cpp:1984-2005]
5. `Reset`（primary context）不是销毁而是重建：`LockedReset` 调 `Dispose` 后重建默认流与 barrier 流。[src/musa/core/context.cpp:2120-2144]

以上 1-4 由源码路径直接支持；第 5 项的重建路径是否与 HAL 状态完全对齐（例如 copy manager、L2 持久属性）仍需目标机验证。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（4 处，例：[src/musa/core/context.cpp:1906-2005]）
- `src/musa/core/device.cpp`（2 处，例：[src/musa/core/device.cpp:635-675]）
- `src/musa/core/internal_funcs.h`（1 处，例：[src/musa/core/internal_funcs.h:22-40]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
