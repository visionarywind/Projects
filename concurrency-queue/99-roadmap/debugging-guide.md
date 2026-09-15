# 调试指南

- 文档目的：按症状把问题定位到核心协议、阻塞适配、ABI 或构建环境。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：症状分类是诊断建议；根因必须由重现和源码证据确认。
- 最后更新：2026-09-10
- 前置阅读：[错误边界](../90-cross-module/error-boundaries.md)
- 后续阅读：[风险与技术债务](risks-and-debt.md)
## 结论摘要

本页聚焦 99-roadmap/debugging-guide.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 空/漏元素

先区分 `try_dequeue` 的瞬时 false、跨 producer 无序和真实丢失；记录 producer/consumer 数、是否 token、是否提前退出。检查 head/tail、overcommit、block index 和测试 shutdown。

## Hang/超时

确认是 queue empty、semaphore wait、未 signal、worker 未 join 还是析构竞态。blocking queue 不能在 waiter 仍存在时销毁；保留超时参数和线程堆栈。

## 崩溃/UB

检查 handle 是否有效、queue 是否已析构、block 是否过早回收、value 输出指针是否有效、C++ exception 是否越过 C ABI。

## 只在某架构失败

先复核预处理宏、原子 lock-free 属性、对齐、编译器和 sysroot；再用 native/RISC-V 对应测试或模型检查缩小范围。

## 性能回退

固定 commit、编译 flags、CPU、线程模型和场景后运行 benchmark；不要从单次 noisy 结果推断算法正确性或普遍性能。

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
