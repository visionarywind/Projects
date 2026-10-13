# 性能关键路径

- 文档目的：《性能关键路径》的分析、证据与结论。
- 适用范围：90-cross-module《性能关键路径》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`../01-modules/module-registry.md`](../01-modules/module-registry.md)、[`system-wiring.md`](system-wiring.md)
- 后续阅读：[`interface-contracts.md`](interface-contracts.md)、[`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)


## 已能从源码定位的热点

1. `Platform::Init` 的设备发现和 `call_once` 初始化。[src/musa/core/platform.cpp:84-140]
2. `Context::ResolveDependencyAndQueueCommand` 的平台串行锁、依赖扫描和队列操作。[src/musa/core/context.cpp:1984-2038]
3. Stream 的 `QueueCommand`、合并列表和异步提交线程。[src/musa/core/stream.cpp:1048-1220]
4. MemoryTracker 地址区间查找、peer mapping 和多设备同步。[src/driver/mu_memory.cpp:105-125] [src/musa/core/memory.cpp:117-145]

## 测量建议

记录 API、queued、submitted、begin/end、completed 时间戳，并区分 CPU 等待、队列等待和 GPU 执行。`MUSA_TRACK_COMMAND_TIMESTAMP` 提供命令时间戳记录开关，`MUSA_INFLIGHT_SUBMISSION_LIMIT` 控制提交并发窗口；实际格式和开销需目标机验证。[src/musa/core/platformSettings.cpp:26-27]

不能凭源码行数声称性能提升；需基准、配置、设备型号和统计方法共同记录。

## 相关文档

- [`system-wiring.md`](system-wiring.md)
- [`interface-contracts.md`](interface-contracts.md)
- [`cross-module-call-chains.md`](cross-module-call-chains.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)

## 源码证据摘要

- `src/musa/core/platform.cpp`（1 处，例：[src/musa/core/platform.cpp:84-140]）
- `src/musa/core/context.cpp`（1 处，例：[src/musa/core/context.cpp:1984-2038]）
- `src/musa/core/stream.cpp`（1 处，例：[src/musa/core/stream.cpp:1048-1220]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。

## 下一步阅读建议

阅读 [`interface-contracts.md`](interface-contracts.md)；阅读 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。
