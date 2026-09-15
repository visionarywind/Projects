# 项目概览

- 文档目的：说明项目定位、边界、能力和真实仓库规模。
- 适用范围：当前提交 `683b9e31ea15eb69f1b81cc1defc7850d5f20b71`。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：已确认；设计意图部分来自 README，为推断时已标注。
- 最后更新：2026-09-10
- 前置阅读：[知识库入口](../README.md)
- 后续阅读：[总体架构](architecture.md)、[设计原则](design-principles.md)
## 结论摘要

这是一个不依赖运行时服务的 C++11 并发容器库。它把多生产者/多消费者队列实现为“每个 producer 一个内部流 + 一个全局 producer 链表”的组合，并通过连续 block、原子索引和回收 free list 避免每个元素一次分配。阻塞版和 C API 是薄的适配边界；测试、模糊测试、模型检查和基准是验证与观测工具，而不是生产运行时模块。

## 项目解决的问题

调用方需要在多个线程之间传递任意可移动/可析构 C++ 对象，同时尽量减少锁、分配和同步开销。核心 API 支持单项、bulk、显式 token、隐式 producer、预分配和近似大小；阻塞 API 为消费者提供等待和超时。README 明确列出 C++11、线程安全、bulk 操作和 exception safety 等能力。[../../../README.md:6-18](../../source/concurrency-queue/README.md#L6-L18)

## 系统边界

**边界内**：模板队列算法、对象构造/析构、块内存池、原子协议、阻塞信号量包装、C ABI、测试/benchmark 构建和 CMake 安装导出。

**边界外**：操作系统线程调度和底层 semaphore 实现细节、第三方 Boost/dlib/TBB/Relacy/CDSChecker 内部实现、应用如何同步队列对象的构造和销毁、部署服务。仓库没有服务进程、配置文件驱动的 daemon 或数据库。

## 主要交付物

| 交付物 | 入口 | 运行形态 |
|---|---|---|
| 非阻塞队列 | `concurrentqueue.h` | header-only 模板 |
| 阻塞队列 | `blockingconcurrentqueue.h` + `lightweightsemaphore.h` | header-only 模板 |
| C ABI | `c_api/concurrentqueue.h` + 两个 `.cpp` | 编译进调用方或测试程序 |
| 测试 | `tests/unittests`、`tests/fuzztests` | `build/bin/*` 可执行文件 |
| 基准 | `benchmarks/benchmarks.cpp` | `build/bin/benchmarks` |
| 安装包 | `CMakeLists.txt` | CMake interface/export/package |

## 事实、推断和未知

- **已确认**：根 CMake 目标为 `INTERFACE`；测试由 legacy Makefile 构建。[../../../CMakeLists.txt:1-16](../../source/concurrency-queue/CMakeLists.txt#L1-L16)、[../../../build/makefile:28-50](../../source/concurrency-queue/build/makefile#L28-L50)
- **已确认**：README 说明队列不是 linearizable/ sequentially consistent，并要求调用方处理生命周期。[../../../README.md:47-63](../../source/concurrency-queue/README.md#L47-L63)、[../../../README.md:122-137](../../source/concurrency-queue/README.md#L122-L137)
- **推断**：按 M01–M07 划分主要是为了分别隔离算法、阻塞同步、ABI、验证和交付边界；源码没有显式 module system。
- **未知**：不同编译器/架构的原子是否全部 lock-free，只能通过 `ConcurrentQueue::is_lock_free()` 或目标平台实验确认。

## 相关文档

- [总体架构](architecture.md)
- [依赖地图](dependency-map.md)
- [构建总览](build-and-deploy.md)

## 源码证据摘要

[../../../README.md:65-85](../../source/concurrency-queue/README.md#L65-L85) 解释每个 producer 子队列、块存储和跨 producer 无序语义；[../../../README.md:490-516](../../source/concurrency-queue/README.md#L490-L516) 按源码顺序说明 free list、block、explicit/implicit producer 和 block pool。

## 未解决问题

仓库没有独立的版本宏说明文档；本页版本以 Git HEAD 为准。部署/服务启动/网络协议不适用。

## 下一步阅读建议

阅读 [runtime-model.md](runtime-model.md) 了解对象与资源生命周期，再进入 M01。
