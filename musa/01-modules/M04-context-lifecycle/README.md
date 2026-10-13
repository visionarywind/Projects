# Context 与资源生命周期（M04）

- 源码范围：`src/musa/core/context.cpp、context.h、musaContext.h、device.cpp（CreateContext 相关）`
- 对应版本：`5.3.0 develop`
- 文档目的：context lifecycle：总览：职责、结构、关键结论与文档导航。
- 适用范围：M04 模块的《Context 与资源生命周期（M04）》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：静态源码已确认；硬件行为、性能和完整异常覆盖未验证
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)

## 一句话职责

Context 生命周期负责 `Device::CreateContext -> Context::Init（含 CriticalBase 初始化）-> 资源集合登记与销毁 -> Context::Synchronize -> Context::Dispose/析构`。

## 阅读入口

1. 先读本页和 [`source-map.md`](source-map.md)。
2. 再读 [`implementation.md`](implementation.md)、[`call-chains.md`](call-chains.md)。
3. 涉及资源所有权、锁、同步或清理时，结合 [`risks-and-debt.md`](risks-and-debt.md) 和 [`design.md`](design.md)。

## 边界

- **本模块拥有**：`Context` 及 `Context::CriticalBase` 的状态与生命周期、资源集合登记/查找/销毁、默认流与 barrier 流的创建和释放。
- **本模块不拥有**：上游 API 的公共 ABI 契约（M02）和下游 GPU/内核驱动的最终实现（M09）。
- **证据边界**：以下结论来自源码行号，不等同于目标机运行成功。

## 核心证据

[src/musa/core/context.cpp:41-475] [src/musa/core/context.cpp:997-1015] [src/musa/core/context.cpp:1906-2005] [src/musa/core/device.cpp:635-688] [src/musa/musaContext.h:42-100]

## 深度状态

正常路径已建立：`Context` 构造（1879-1899）→ `Context::Init`（1906-1965）→ 资源登记（93-475 各 Add 方法）→ `Synchronize`/`LockedWait`（997-1015、2103-2118）→ `Dispose`/析构（1901-1904、1967-2005）。分支、失败回滚、并发和清理见子文档。凡标记“推断/未知”的项目，均需在目标源码或硬件环境中补证。

## Demo 覆盖

- **真实经过本模块的 Demo 步骤**：只到 retain/set current 边界（见 [`../../80-demos/D01-muinfo/execution-trace.md`](../../80-demos/D01-muinfo/execution-trace.md) §1 与 [`../../80-demos/D01-muinfo/data-and-state-trace.md`](../../80-demos/D01-muinfo/data-and-state-trace.md) 主状态转移）：
  1. D01-S005 的 `muapiDevicePrimaryCtxRetain`：取 primary context 并把引用计数 +1 [src/driver/mu_context.cpp:472-488]；
  2. D01-S005 的 `muapiCtxSetCurrent`：TLS context 栈 push/pop [src/driver/mu_context.cpp:280-295]；
  3. primary context 的惰性初始化在 `Device::LaterInit` 内完成 [src/musa/core/device.cpp:911-1017]（trace §1 记为“初始化 primary Context”）。
- **候选但未覆盖**：D02–D07（`tests/*.cu`）——本快照无 `tests/` 目录，入口未核实；候选表中 D02/D03/D04/D05/D06 计划覆盖 M04，D07 只覆盖 M02、M08。
- **尚无 Demo 覆盖的重要实现**：
  1. 用户 context 创建/销毁 `muapiCtxCreate`、`muapiCtxDestroy_v2` [src/driver/mu_context.cpp:184-188]；
  2. primary context 释放 `muapiDevicePrimaryCtxRelease_v2`（D01 到进程结束也不显式释放）[src/driver/mu_context.cpp:532-547]；
  3. `Context::Synchronize` 全流同步 [src/musa/core/context.cpp:997-1015]；
  4. `Context::Dispose` 资源回收 [src/musa/core/context.cpp:1967-2005]；细节见 [`call-chains.md`](call-chains.md)。

## 相关文档

- [`README.md`](README.md)
- [`design.md`](design.md)
- [`call-chains.md`](call-chains.md)
- [`implementation.md`](implementation.md)
- [`risks-and-debt.md`](risks-and-debt.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（3 处，例：[src/musa/core/context.cpp:41-475]）
- `src/musa/core/device.cpp`（1 处，例：[src/musa/core/device.cpp:635-688]）
- `src/musa/musaContext.h`（1 处，例：[src/musa/musaContext.h:42-100]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
