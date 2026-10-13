# 跨模块错误边界

- 文档目的：错误边界：层间错误转换与传播契约。
- 适用范围：90-cross-module《跨模块错误边界》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-08
- 前置阅读：[`../01-modules/module-registry.md`](../01-modules/module-registry.md)、[`system-wiring.md`](system-wiring.md)
- 后续阅读：[`interface-contracts.md`](interface-contracts.md)、[`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)


```text
API参数错误 -> Driver API
对象/所有权错误 -> Platform/Context/Memory
底层资源错误 -> HALToMuResult边界
异步执行错误 -> Stream Query/Synchronize/robust info
工具输出错误 -> checkMuErrors/CLI
```

每一层都应保留原始上下文（device、context、stream、correlation id）。异步 enqueue 返回成功不等于 GPU 工作成功；这是由 `AsyncSubmit` 与 `GetEngineLastError` 的分离可见的。[src/musa/core/stream.cpp:1113-1146] [src/musa/core/stream.cpp:1154-1220]

## 层间错误转换锚点

与上面的分类一一对应，每条都能在源码里逐行核对；错误是否在目标机真实触发未验证。

| 层 | 转换/传播点 | 行级锚点 |
|---|---|---|
| API 参数 | flags 非 0 直接置 `MUSA_ERROR_INVALID_VALUE`，不进初始化 | [src/driver/mu_context.cpp:121-133] `muapiInit` |
| 平台初始化 | HAL 结果经 `HalToMuResult` 转公共错误码；可见设备为空置 `MUSA_ERROR_NO_DEVICE` | [src/musa/core/platform.cpp:97-117] `HalToMuResult` |
| 设备句柄 | ordinal 越界时 `*device = -1` 并返回 `MUSA_ERROR_INVALID_DEVICE` | [src/driver/mu_device.cpp:22-33] `MUSA_ERROR_INVALID_DEVICE` |
| 当前 context | TLS 无当前 context 时返回 `MUSA_ERROR_INVALID_CONTEXT`，查询不回落到默认 context | [src/driver/mu_memory.cpp:542-560] `muapiMemGetInfo_v2` |
| capture 状态 | 发现 ACTIVE/INVALIDATED capture 即失败并 `InvalidateCaptureStatus` | [src/musa/core/context.cpp:997-1015] `Context::Synchronize` |
| HAL → 公共错误码 | `HalToMuResult` 的 switch 把 `Hal::Result` 映射为 `MUresult`，未知值默认 `MUSA_ERROR_UNKNOWN` | [src/musa/core/internal_funcs.h:22-24] `HalToMuResult` |
| 工具输出 | `__checkMusaErrors` 打印文件/行号后 `exit(EXIT_FAILURE)`，首个错误即终止 | [src/tools/muInfo.cpp:9-20] `__checkMusaErrors` |

## 边界处的已知模糊点

- `Stream::WaitFinish` 把 `MUSA_ERROR_NOT_READY` 与真正失败区分开，但随后的 sticky error 日志只在非 SUCCESS/非 NOT_READY 时打印；调用方拿到的 `MUresult` 是否已含 HAL 侧错误需运行确认。[src/musa/core/stream.cpp:1118-1127]
- `Stream::GetEngineLastError` 先查 robust info、再查 queue、最后回退到 device；三层都成功后若仍有异常类型，就把结果写进 `pLastErr`，否则维持 `MUSA_SUCCESS`。这条回退链在无硬件时不可实测。[src/musa/core/stream.cpp:1134-1157]

## 相关文档

- [`system-wiring.md`](system-wiring.md)
- [`interface-contracts.md`](interface-contracts.md)
- [`cross-module-call-chains.md`](cross-module-call-chains.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（4 处，例：[src/musa/core/stream.cpp:1113-1146] `GetEngineLastError`）
- `src/driver/mu_context.cpp`（1 处，例：[src/driver/mu_context.cpp:121-133] `muapiInit`）
- `src/musa/core/platform.cpp`（1 处，例：[src/musa/core/platform.cpp:97-117] `HalToMuResult`）
- `src/driver/mu_device.cpp`（1 处，例：[src/driver/mu_device.cpp:22-33] `MUSA_ERROR_INVALID_DEVICE`）
- `src/driver/mu_memory.cpp`（1 处，例：[src/driver/mu_memory.cpp:542-560] `muapiMemGetInfo_v2`）
- `src/musa/core/context.cpp`（1 处，例：[src/musa/core/context.cpp:997-1015] `Context::Synchronize`）
- `src/musa/core/internal_funcs.h`（1 处，例：[src/musa/core/internal_funcs.h:22-24] `HalToMuResult`）
- `src/tools/muInfo.cpp`（1 处，例：[src/tools/muInfo.cpp:9-20] `__checkMusaErrors`）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。

## 下一步阅读建议

阅读 [`interface-contracts.md`](interface-contracts.md)；阅读 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。
