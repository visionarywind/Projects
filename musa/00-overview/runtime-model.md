# 运行时模型

- 文档目的：运行时对象模型：进程、上下文、线程与句柄。
- 适用范围：00-overview《运行时模型》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`project-overview.md`](project-overview.md)、[`../README.md`](../README.md)
- 后续阅读：[`architecture.md`](architecture.md)、[`../99-roadmap/reading-guide.md`](../99-roadmap/reading-guide.md)


## 对象层次

```text
Platform(singleton)
  └─ Device[ordinal]
      ├─ primary Context
      ├─ optional user Context / green Context
      ├─ MemoryPool / copy managers
      └─ HAL device
Context
  ├─ default/barrier/per-thread Stream
  ├─ Memory / Module / Event / Graph / Node
  └─ CriticalBase + dependency state
Stream
  ├─ engine queues/cmd pools/semaphores
  ├─ waiting/inflight/command lists
  └─ submit/wait threads
```

## 初始化与延迟初始化

`muapiInit` 触发 `Platform::Init`；Platform 创建可见 Device。Device 的若干重资源在 `GetDevice` 时通过 `LaterInit` 按 `call_once` 完成，包括 finalize、engine 映射、primary context 和 copy manager。[src/driver/mu_context.cpp:121-133] [src/musa/core/platform.cpp:84-140] [src/musa/core/device.cpp:856-1085]

## 当前上下文模型

Driver API 通过 `TlsCtxTop()` 读取线程局部当前 context；显式句柄经过 `Platform::ValidateContext` 后再转型。[src/driver/mu_context.cpp:15-29] 这意味着 API 的许多参数合法性依赖调用线程的 context 栈，而非仅依赖全局 device。

## 命令状态模型

源码至少出现 `queued`、`submitted`、`completed` 等状态；Stream 将命令放入 `m_CommandList`，异步提交线程取出并调用 `Submit`。[src/musa/core/stream.cpp:1048-1220] 完整状态转移和错误回收需结合 `command.cpp` 逐行确认。

## 生命周期边界

- Platform：进程内单例，析构时关闭 debugger、释放未释放 memory/resource descriptor、销毁 HAL platform。[src/musa/core/platform.cpp:478-541]
- Context：创建默认流和 barrier stream；`Dispose`/析构前触发资源回调。[src/musa/core/context.cpp:1875-1984]
- Stream：创建 HAL queue/cmd pool/cmd buffer/semaphore，并拥有 submit/wait 线程；析构时停止并 join 线程。[src/musa/core/stream.cpp:804-1020] [src/musa/core/stream.cpp:849-908]

## 相关文档

- [`project-overview.md`](project-overview.md)
- [`architecture.md`](architecture.md)
- [`../README.md`](../README.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- [`../99-roadmap/reading-guide.md`](../99-roadmap/reading-guide.md)

## 源码证据摘要

- `src/musa/core/stream.cpp`（3 处，例：[src/musa/core/stream.cpp:1048-1220]）
- `src/driver/mu_context.cpp`（2 处，例：[src/driver/mu_context.cpp:121-133]）
- `src/musa/core/platform.cpp`（2 处，例：[src/musa/core/platform.cpp:84-140]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。

## 下一步阅读建议

阅读 [`architecture.md`](architecture.md)；阅读 [`../99-roadmap/reading-guide.md`](../99-roadmap/reading-guide.md)。
