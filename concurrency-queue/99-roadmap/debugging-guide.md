# 调试指南

- 文档目的：按症状把问题定位到核心协议、阻塞适配、ABI 或构建环境。
- 证据状态：症状分类是诊断建议；根因必须由重现和源码证据确认。
- 最后更新：2026-09-10
- 前置阅读：[错误边界](../90-cross-module/error-boundaries.md)
- 后续阅读：[风险与技术债务](risks-and-debt.md)

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
