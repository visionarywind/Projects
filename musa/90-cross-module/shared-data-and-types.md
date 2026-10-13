# 共享数据与类型

- 文档目的：跨模块共享数据与类型的定义与使用方。
- 适用范围：90-cross-module《共享数据与类型》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：部分推断（引用较少，证据密度不足）
- 最后更新：2026-10-07
- 前置阅读：[`../01-modules/module-registry.md`](../01-modules/module-registry.md)、[`system-wiring.md`](system-wiring.md)
- 后续阅读：[`interface-contracts.md`](interface-contracts.md)、[`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)


| 类型/数据 | 生产者 | 消费者 | 生命周期/风险 |
|---|---|---|---|
| `MUdevice` | Platform/Device | API/Context | ordinal 与可见设备映射 |
| `MUcontext` | Device | TLS/API/资源 | current context 栈 |
| `MUdeviceptr` | Memory/HAL | pointer/copy/kernel | tracker 区间和 offset |
| `MemoryCreateInfo` | Driver/API | Memory/HAL | type/property/view 决定分支 |
| `Command` | Stream/Context | submit/wait/MUPTI | shared_ptr 保活 |
| `GraphNode` | Context/capture | GraphExec/Stream | 拓扑、依赖、资源引用 |
| correlation/submission id | ThreadInfo/Stream | profiler/debugger | 异步排障关键 |

来源：[src/musa/core/context.cpp:1037-1081] [src/musa/core/stream.cpp:1048-1220] [src/driver/mu_entry.cpp:121-198]。

## 相关文档

- [`system-wiring.md`](system-wiring.md)
- [`interface-contracts.md`](interface-contracts.md)
- [`cross-module-call-chains.md`](cross-module-call-chains.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（1 处，例：[src/musa/core/context.cpp:1037-1081]）
- `src/musa/core/stream.cpp`（1 处，例：[src/musa/core/stream.cpp:1048-1220]）
- `src/driver/mu_entry.cpp`（1 处，例：[src/driver/mu_entry.cpp:121-198]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。

## 下一步阅读建议

阅读 [`interface-contracts.md`](interface-contracts.md)；阅读 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。
