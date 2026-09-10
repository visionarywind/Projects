# 模块注册表

- 文档目的：定义知识库稳定的 M01–M07 模块边界、入口和证据。
- 证据状态：文件边界已确认；模块名称是文档层抽象。
- 最后更新：2026-09-10
- 前置阅读：[总入口](../README.md)
- 后续阅读：[M01](M01-core-queue/README.md)

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
