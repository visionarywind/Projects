# 证据索引

- 文档目的：集中列出最常引用的源码证据，便于行号校验和增量维护。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../README.md)。
- 后续阅读：[分析状态](analysis-state.md)。
## 结论摘要

本页聚焦 00-overview/evidence-index.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 主题 | 证据 |
|---|---|
| queue 构造/析构 | `concurrentqueue.h:833-919` |
| token 生命周期 | `concurrentqueue.h:671-733` |
| 公开 enqueue/dequeue | `concurrentqueue.h:1010-1361` |
| enqueue dispatch | `concurrentqueue.h:1395-1418` |
| free list | `concurrentqueue.h:1466-1579` |
| Block 空状态 | `concurrentqueue.h:1588-1714` |
| producer 基础索引 | `concurrentqueue.h:1727-1791` |
| explicit enqueue/dequeue | `concurrentqueue.h:1877-2081` |
| implicit enqueue/dequeue | `concurrentqueue.h:2515-2648` |
| block requisition | `concurrentqueue.h:3068-3143` |
| producer list | `concurrentqueue.h:3256-3306` |
| implicit hash | `concurrentqueue.h:3323-3509` |
| queue fields | `concurrentqueue.h:3678-3708` |
| blocking construction/enqueue | `blockingconcurrentqueue.h:23-129` |
| blocking waits | `blockingconcurrentqueue.h:146-548` |
| semaphore spinning/wait | `lightweightsemaphore.h:280-360` |
| semaphore signal | `lightweightsemaphore.h:421-430` |
| C ABI declarations | `c_api/concurrentqueue.h` |
| C ABI implementation | `c_api/concurrentqueue.cpp`, `c_api/blockingconcurrentqueue.cpp` |
| CMake interface target | `CMakeLists.txt:1-16` |
| Make targets | `build/makefile:28-50` |
| CI native/RISC-V | `.github/workflows/ci.yml` |
| README contract | `README.md:47-180` |
| allocator 与动态块回收策略 | `source/concurrency-queue/concurrentqueue.h:396-430` |
| initial pool 构造与 queue 析构 | `source/concurrency-queue/concurrentqueue.h:823-919` |
| block pool 请求/归还优先级 | `source/concurrency-queue/concurrentqueue.h:3068-3143` |
| producer 复用与线程退出 | `source/concurrency-queue/concurrentqueue.h:3256-3306,3509-3599` |
| block 分配/回收测试 | `source/concurrency-queue/tests/unittests/unittests.cpp:1010-1202` |
| producer/free-list 测试 | `source/concurrency-queue/tests/unittests/unittests.cpp:1275-1460,4852-4925` |
| Graph 边界 | `source/concurrency-queue/concurrentqueue.h:1-3`; `source/concurrency-queue/benchmarks/tbb/flow_graph.h` | 核心队列无 Graph runtime，benchmark 第三方 graph 不纳入资源模型 |

## 行号维护规则

源码变更后，先更新本表和受影响文档的证据范围，再做链接和证据一致性检查。行号是当前 HEAD 参考，不应在未重新读取源码时声称仍精确。

## 相关文档
- [项目入口](../README.md)
- [分析状态](analysis-state.md)
- [源码证据索引](evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
