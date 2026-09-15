# 模块注册表

- 文档目的：定义知识库稳定的 M01–M07 模块边界、入口和证据。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：文件边界已确认；模块名称是文档层抽象。
- 最后更新：2026-09-10
- 前置阅读：[总入口](../README.md)
- 后续阅读：[M01](M01-core-queue/README.md)
## 结论摘要

本页聚焦 01-modules/module-registry.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| ID | 模块 | 主要源码 | 真实入口 | 主要输出/副作用 |
|---|---|---|---|---|
| M01 | core-queue | `concurrentqueue.h` | `ConcurrentQueue::enqueue`, `try_dequeue` | 构造/移动/析构 T，更新原子索引，分配/回收 block |
| M02 | blocking-queue | `blockingconcurrentqueue.h` | `BlockingConcurrentQueue::wait_dequeue` | 等待 permit，委托核心 queue |
| M03 | semaphore-and-platform | `lightweightsemaphore.h` | `waitWithPartialSpinning`, `signal` | 原子计数和平台 wait/signal |
| M04 | c-api | `c_api/*` | `moodycamel_*` functions | C ABI handle/value 转发 |
| M05 | verification | `tests/*` | test main、fuzz、Relacy、CDSChecker | 进程退出码、断言、模型检查结果 |
| M06 | benchmarks | `benchmarks/*` | benchmark main | 计时、统计和日志/CSV |
| M07 | packaging-and-ci | CMake、Make、CI | build targets/workflows | 编译、链接、安装、跨架构运行 |

## 阅读约定

每个模块页按“契约 → 数据结构 → 真实调用链 → 状态/所有权 → 错误/清理 → 验证缺口”组织。源码区间是当前 HEAD 的定位提示；变更后以 [证据索引](../00-overview/evidence-index.md) 为准。

## 依赖方向

M01 是运行时核心；M02 依赖 M01，M03 为 M02 提供等待原语；M04 分别落到 M01/M02；M05/M06 使用多个运行时模块；M07 构建和交付这些模块但不参与运行时调度。

## 覆盖状态

静态主路径已覆盖，运行数据和性能结论尚未覆盖。任何文档中的“未验证”不得被解释为失败，也不得被解释为成功。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
