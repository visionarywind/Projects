# 跨模块 ABI 与分配器

- 文档目的：说明 header-only 模板、C ABI、traits allocator 和安装 target 的交界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/concurrency-queue HEAD 683b9e3（2026-09-15 只读确认）。
- 证据状态：静态依赖已确认；二进制兼容性矩阵未执行。
- 最后更新：2026-09-10
- 前置阅读：[M04](../01-modules/M04-c-api/README.md)
- 后续阅读：[M07](../01-modules/M07-packaging-and-ci/README.md)
## 结论摘要

本页聚焦 90-cross-module/abi-and-allocation.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 交界

- C++ 模板代码由使用方编译，traits 和宏会影响实例化行为。
- C ABI `.cpp` 固定实例化 `void*` queue，并通过导出函数给 C 调用者 opaque handle。
- CMake interface target 分发 headers；legacy Make 负责测试/benchmark 和 C API object。
- queue 的 allocator 负责 block/index 资源；C `value` 的指向对象所有权不由静态 cast 自动解决。

## 风险

跨编译单元宏不一致、不同编译器 ABI、错误 allocator 配对、C++ exception 穿过 C boundary、错误 handle destroy 都必须由调用约定和测试明确。不要把“能编译”解释为“跨语言/跨编译器 ABI 已兼容”。

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
