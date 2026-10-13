# 跨模块运行时轨迹

- 文档目的：《跨模块运行时轨迹》的分析、证据与结论。
- 适用范围：90-cross-module《跨模块运行时轨迹》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`../01-modules/module-registry.md`](../01-modules/module-registry.md)、[`system-wiring.md`](system-wiring.md)
- 后续阅读：[`interface-contracts.md`](interface-contracts.md)、[`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)


## 初始化/查询

```text
muInfo::main
 -> muInit(0)
 -> muapiInit
 -> Platform::Init (call_once)
 -> Hal::CreatePlatform
 -> visible HAL devices
 -> Musa::Device
 -> muDeviceGetCount/Get/Attribute
 -> stdout
```

证据：[src/tools/muInfo.cpp:372-388] [src/driver/mu_context.cpp:121-133] [src/musa/core/platform.cpp:84-140]。

## 内存

```text
muapiMemAlloc_v2
 -> TlsCtxTop
 -> Context::CreateMemory
 -> Memory::Init
 -> GeneralAlloc
 -> Hal::IDevice::GetMemMgr()->Allocate/CreateMemory
 -> MemoryTracker::TrackMemory
 -> MUdeviceptr
```

证据：[src/driver/mu_memory.cpp:271-320] [src/musa/core/context.cpp:1037-1081] [src/musa/core/memory.cpp:470-513]。

## Kernel/异步

```text
muapiLaunchKernel
 -> Context::GeneralLaunchKernel
 -> Stream::CmdLaunchKernel
 -> ResolveDependencyAndQueueCommand
 -> Stream::QueueCommand
 -> AsyncSubmit
 -> Command::Submit
```

证据：[src/driver/mu_module.cpp:232-285] [src/musa/core/context.cpp:625-673] [src/musa/core/stream.cpp:1048-1220]。

## 相关文档

- [`system-wiring.md`](system-wiring.md)
- [`interface-contracts.md`](interface-contracts.md)
- [`cross-module-call-chains.md`](cross-module-call-chains.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（2 处，例：[src/musa/core/context.cpp:1037-1081]）
- `src/tools/muInfo.cpp`（1 处，例：[src/tools/muInfo.cpp:372-388]）
- `src/driver/mu_context.cpp`（1 处，例：[src/driver/mu_context.cpp:121-133]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。

## 下一步阅读建议

阅读 [`interface-contracts.md`](interface-contracts.md)；阅读 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。
