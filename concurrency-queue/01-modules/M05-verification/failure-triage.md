# M05 失败分诊

- 文档目的：把验证失败映射到最可能的层次，不把猜测写成根因。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：流程建议来自测试结构；具体失败需日志确认。
- 最后更新：2026-09-10
- 前置阅读：[测试矩阵](test-matrix.md)
- 后续阅读：[构建总览](../../00-overview/build-and-deploy.md)
## 结论摘要

本页聚焦 01-modules/M05-verification/failure-triage.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 分诊顺序

1. 先记录命令、编译器、commit、退出码和完整 stderr。
2. 编译失败：区分头文件/API、C API 对象文件、第三方 benchmark 依赖和平台宏。
3. 单线程失败：优先检查对象构造/移动/析构和 block/index 基本路径。
4. threaded 失败：检查 producer list、head/tail、optimistic overcommit 和测试 shutdown。
5. 仅 blocking 失败：检查 signal/wait count、timeout 和 waiter 生命周期。
6. 仅 C API 失败：检查 handle 类型、输出指针和异常边界。
7. 仅 RISC-V 失败：先确认交叉编译器/QEMU/运行时 sysroot，再归因于算法。

## 证据规则

在没有重现日志前只写“待定位”；不要把偶发 hang 自动归因于 semaphore，也不要把 benchmark 波动当作正确性失败。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
