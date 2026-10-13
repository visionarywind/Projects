# 阅读指南

- 文档目的：按角色和目标给出阅读路线，每条路线同时指向知识库文档与对应源码入口。
- 适用范围：`source/musa` 快照（5.3.0 develop）建立的整套知识库。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）。
- 证据状态：已确认（源码入口行号经静态核对）；阅读耗时为估计值。
- 最后更新：2026-10-07
- 前置阅读：[`../README.md`](../README.md)
- 后续阅读：[`quick-start.md`](quick-start.md)

## 结论摘要

这套知识库是"总 → 分 → 关联 → 实践"结构：总览层讲清分层与数据流，模块层按 M01–M13 展开实现，关联层用一条主线把它们串起来，实践层给配方与风险。**任何路线都要在看到 wrapper 时继续追到 Core，看到 Core 时继续追到 HAL**，否则得到的只是目录说明。

---

## 路线一：30 分钟快速了解

1. [`../README.md`](../README.md)：项目一句话、13 个模块摘要、三条主流程。
2. [`../00-overview/project-overview.md`](../00-overview/project-overview.md)：定位、目录职责、边界。
3. [`../00-overview/architecture.md`](../00-overview/architecture.md)：分层与依赖方向。
4. [`../01-modules/module-registry.md`](../01-modules/module-registry.md)：M01–M13 稳定 ID 与依赖图。
5. [`../80-demos/D01-muinfo/README.md`](../80-demos/D01-muinfo/README.md)：主 Demo 概览。

源码入口：初始化从 `muapiInit` 进入并落到 `Platform::Init` [src/driver/mu_context.cpp:121-133]、[src/musa/core/platform.cpp:84-140]。看完这条就知道"驱动启动到底做了什么"。

---

## 路线二：半天理解核心流程

按"入口 → 对象 → 状态 → 下游副作用"逐模块推进，每模块先读其 `README.md` 再读 `implementation.md`：

| 顺序 | 模块 | 源码入口 | 知识库文档 |
|---|---|---|---|
| 1 | M02 Driver API | `muapiMemAlloc_v2` [src/driver/mu_memory.cpp:271-304] | `../01-modules/M02-driver-api/` |
| 2 | M03 平台与设备 | `Platform::Init` [src/musa/core/platform.cpp:84-140] | `../01-modules/M03-platform-device/` |
| 3 | M04 Context | `Context::CreateMemory` [src/musa/core/context.cpp:1044-1099] | `../01-modules/M04-context-lifecycle/` |
| 4 | M05 Memory/Mempool | `IMemMgr::Allocate` 契约 [src/hal/halMemMgr.h:17-40] | `../01-modules/M05-memory-mempool/` |
| 5 | M06 Stream/Command | `Stream::QueueCommand` [src/musa/core/stream.cpp:1056-1104] | `../01-modules/M06-stream-command/` |
| 6 | M07 Module/Kernel | `muapiLaunchKernel` [src/driver/mu_module.cpp:231-285] | `../01-modules/M07-module-kernel/` |
| 7 | M08 Graph | `muapiGraphAddNode` 分发 [src/driver/mu_graph.cpp:1012-1069] | `../01-modules/M08-graph/` |
| 8 | M09 HAL/M3D | `IMemMgr` 接口边界 [src/hal/halMemMgr.h:17-40] | `../01-modules/M09-hal-m3d/` |
| 9 | M10 工具与调试 | `muapiGetExportTable` [src/driver/mu_entry.cpp:1255-1270] | `../01-modules/M10-tools-debug/` |
| 10 | M11 Loader | 转发清单展开 [src/loader/loader.cpp:119-123] | `../01-modules/M11-loader/` |
| 11 | M12 Util | `SplayTree::Search` 缺陷点 [src/util/utilSplayTree.h:216-226] | `../01-modules/M12-util/` |
| 12 | M13 头文件层 | `MUSA_VERSION` 契约 [src/musa_shared_include/musa.h:210-215] | `../01-modules/M13-musa-shared-include/` |

最后用 [`../90-cross-module/system-wiring.md`](../90-cross-module/system-wiring.md) 把 12 步串成一条运行主线，再对照 [`../90-cross-module/end-to-end-flows.md`](../90-cross-module/end-to-end-flows.md)。

---

## 路线三：一天准备首次修改

1. 精读一条完整链：分配 `muapiMemAlloc_v2` → `Context::CreateMemory` [src/musa/core/context.cpp:1044-1099] → HAL `IMemMgr` [src/hal/halMemMgr.h:17-40]。
2. 精读一条提交链：`Context::ResolveDependencyAndQueueCommand` [src/musa/core/context.cpp:2012-2076] → `Stream::QueueCommand` [src/musa/core/stream.cpp:1056-1104] → `AsyncSubmit` [src/musa/core/stream.cpp:1159-1310]。
3. 读 [`feature-development-recipes.md`](feature-development-recipes.md) 选定你要加的东西属于哪类配方。
4. 做 [`../80-demos/D01-muinfo/modification-exercises.md`](../80-demos/D01-muinfo/modification-exercises.md) 里的一道低风险练习。
5. 读 [`testing-recipes.md`](testing-recipes.md) 明确本快照**没有** `unittest/`、`tests/`，验证只能落到工具运行与静态检查。
6. 改动前过一遍 [`risk-register.md`](risk-register.md)，确认不踩 `SplayTree` / `MemMgr` 那两条严重项。

---

## 路线四：按角色

### 4.1 模块开发者

[`../01-modules/module-registry.md`](../01-modules/module-registry.md) → 目标模块 `design.md` / `implementation.md` / `call-chains.md` → [`../90-cross-module/interface-contracts.md`](../90-cross-module/interface-contracts.md) → [`../90-cross-module/change-impact-map.md`](../90-cross-module/change-impact-map.md)。源码上先找该模块的 `mu_*` 入口文件，再向 `src/musa/core` 追一层。

### 4.2 故障定位

[`debugging-guide.md`](debugging-guide.md) → [`../00-overview/global-error-model.md`](../00-overview/global-error-model.md) → [`../90-cross-module/error-boundaries.md`](../90-cross-module/error-boundaries.md) → [`../80-demos/D01-muinfo/failure-paths.md`](../80-demos/D01-muinfo/failure-paths.md)。源码起点是 `muapiGetProcAddress_v2` 之上的错误返回与 `mu_error.cpp` 的错误映射 [src/driver/mu_entry.cpp:1226-1250]。

### 4.3 性能优化

[`performance-guide.md`](performance-guide.md) → [`../90-cross-module/performance-critical-paths.md`](../90-cross-module/performance-critical-paths.md) → 按表回到 `MemMgr::Allocate` [src/hal/m3d/memMgr.cpp:81-147] 与 `Stream::AsyncSubmit` [src/musa/core/stream.cpp:1159-1310]。**先测后改**，本快照没有基线数据。

### 4.4 测试与质量

[`testing-recipes.md`](testing-recipes.md) → [`../00-overview/build-and-deploy.md`](../00-overview/build-and-deploy.md) → 各模块 `testing.md`。当前缺口（`unittest/` 缺失、`MUSA_BUILD_UT` 指向不存在目录）在 [`technical-debt.md`](technical-debt.md) 有登记。

### 4.5 工具与观测开发者

[`../80-demos/D01-muinfo/README.md`](../80-demos/D01-muinfo/README.md) → [`../01-modules/M10-tools-debug/call-chains.md`](../01-modules/M10-tools-debug/call-chains.md) → `muapiGetExportTable` 的分表与 `FillAccessors` [src/driver/mu_entry.cpp:2146-2195]。工具入口本体是 [`printDeviceProp`](../80-demos/D01-muinfo/execution-trace.md)，源码在 [src/tools/muInfo.cpp:283-370]。

---

## 阅读技巧

1. 每读一条链都记六件事：**入口、对象查找、状态变更、下游副作用、错误、清理**。
2. 只看到 `muapi*` wrapper 时继续往下追到 Core 对象；只看到 Core 调用时继续追到 HAL 接口；`Hal::` 之下的私有子模块是明确的证据边界，越界处标注"未知"。
3. 同一事实只在权威文档展开一次，其他地方用相对链接引用（见 [`../00-overview/evidence-index.md`](../00-overview/evidence-index.md)）。
4. 行号会随源码漂移，引用旁同时记符号全名，例如 [src/driver/mu_entry.cpp:1226-1250] `muapiGetProcAddress_v2`。
5. 遇到"已确认 / 推断 / 未知"三种标记时不要跨级理解，尤其不要把静态推断当运行结论。

## 相关文档

- [`quick-start.md`](quick-start.md)
- [`../00-overview/evidence-index.md`](../00-overview/evidence-index.md)
- [`../00-overview/glossary.md`](../00-overview/glossary.md)
- [`next-steps.md`](next-steps.md)

## 源码证据摘要

- 初始化：[src/driver/mu_context.cpp:121-133]、[src/musa/core/platform.cpp:84-140]
- 分配：[src/driver/mu_memory.cpp:271-304]、[src/musa/core/context.cpp:1044-1099]、[src/hal/halMemMgr.h:17-40]
- 提交：[src/musa/core/context.cpp:2012-2076]、[src/musa/core/stream.cpp:1056-1104]、[src/musa/core/stream.cpp:1159-1310]
- 边界层：[src/loader/loader.cpp:119-123]、[src/musa_shared_include/musa.h:210-215]、[src/driver/mu_entry.cpp:1255-1270]

## 未解决问题

1. 各模块 `README.md` 的深度不一致（M11/M12/M13 已完成，M08/M09/M10 部分完成），路线二中后三个模块的阅读深度有限。
2. 无运行验证，路线中的"看输出""跑 Demo"类步骤需在目标环境执行。

## 下一步阅读建议

按角色选一条路线执行到底；执行中发现断链或引用漂移，记录到 [`../00-overview/analysis-state.md`](../00-overview/analysis-state.md)。
