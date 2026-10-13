# 设计原则

- 文档目的：设计原则与源码中的对应体现。
- 适用范围：00-overview《设计原则》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认与推断分开；未执行构建和硬件运行
- 最后更新：2026-10-07
- 前置阅读：[`project-overview.md`](project-overview.md)、[`../README.md`](../README.md)
- 后续阅读：[`architecture.md`](architecture.md)、[`../99-roadmap/reading-guide.md`](../99-roadmap/reading-guide.md)

## 结论

源码体现出五个稳定原则：C ABI 与 C++ 对象解耦、Context 作为资源归属边界、命令异步化、HAL 隔离硬件差异、旧 ABI 通过包装保持兼容。前四项由源码结构和调用点确认；“设计原则”这一表述是基于实现的归纳。

## 原则清单

1. **入口薄、核心厚**：`muapi*` 负责参数/句柄检查和结果转换，实际资源操作下沉到 `Platform`、`Context`、`Memory`、`Stream`。[src/driver/mu_memory.cpp:271-320] [src/musa/core/context.cpp:1037-1099]
2. **显式资源归属**：Context 的 `CriticalBase` 保存 stream、event、graph、memory、module 等集合，并在销毁时释放客户端资源。[src/musa/core/context.cpp:41-475]
3. **异步统一为 Command**：Kernel、copy、memset、graph launch 都经由 `Stream::Cmd*` 创建/排队，提交线程再调用各命令子类的 `Submit()`，并由 `Command::SubmitToQueue` 落到 HAL 队列。[src/musa/core/context.cpp:625-673] [src/musa/core/stream.cpp:1048-1220] [src/musa/core/command/command.cpp:828]
4. **硬件差异由 HAL 吸收**：core 只依赖 HAL 接口，`halM3d` 再链接 `m3d/scpc/util`。[src/musa/core/CMakeLists.txt:1-11] [src/hal/m3d/CMakeLists.txt:36-59]
5. **兼容性优先**：v1 API 多以 v2 API 的窄包装实现，导出表按领域分组。[src/driver/mu_context.cpp:184-221] [src/driver/mu_entry.cpp:1323-1660]

## 需要谨慎解释的地方

- 线程安全不是所有对象都由同一把锁保护；已看到 `ReadLockedAccessor`、`WriteLockedAccessor`、提交锁和 per-thread stream，具体锁序需要逐函数审计。[src/musa/core/context.cpp:552-613] [src/musa/core/stream.cpp:1048-1100]
- HAL 到内核驱动的最终副作用仍依赖私有 M3D 子模块，当前文档不把“调用成功”当作硬件验证。

## 相关文档

- [`project-overview.md`](project-overview.md)
- [`architecture.md`](architecture.md)
- [`../README.md`](../README.md)
- [`../01-modules/module-registry.md`](../01-modules/module-registry.md)
- [`../99-roadmap/reading-guide.md`](../99-roadmap/reading-guide.md)

## 源码证据摘要

- `src/musa/core/context.cpp`（4 处，例：[src/musa/core/context.cpp:1037-1099]）
- `src/musa/core/stream.cpp`（2 处，例：[src/musa/core/stream.cpp:1048-1220]）
- `src/driver/mu_memory.cpp`（1 处，例：[src/driver/mu_memory.cpp:271-320]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。

## 下一步阅读建议

阅读 [`architecture.md`](architecture.md)；阅读 [`../99-roadmap/reading-guide.md`](../99-roadmap/reading-guide.md)。
