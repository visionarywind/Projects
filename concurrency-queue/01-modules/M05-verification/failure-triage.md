# M05 失败分诊

- 文档目的：把验证失败映射到最可能的层次，不把猜测写成根因。
- 证据状态：流程建议来自测试结构；具体失败需日志确认。
- 最后更新：2026-09-10
- 前置阅读：[测试矩阵](test-matrix.md)
- 后续阅读：[构建总览](../../00-overview/build-and-deploy.md)

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
