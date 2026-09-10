# 跨模块 ABI 与分配器

- 文档目的：说明 header-only 模板、C ABI、traits allocator 和安装 target 的交界。
- 证据状态：静态依赖已确认；二进制兼容性矩阵未执行。
- 最后更新：2026-09-10
- 前置阅读：[M04](../01-modules/M04-c-api/README.md)
- 后续阅读：[M07](../01-modules/M07-packaging-and-ci/README.md)

## 交界

- C++ 模板代码由使用方编译，traits 和宏会影响实例化行为。
- C ABI `.cpp` 固定实例化 `void*` queue，并通过导出函数给 C 调用者 opaque handle。
- CMake interface target 分发 headers；legacy Make 负责测试/benchmark 和 C API object。
- queue 的 allocator 负责 block/index 资源；C `value` 的指向对象所有权不由静态 cast 自动解决。

## 风险

跨编译单元宏不一致、不同编译器 ABI、错误 allocator 配对、C++ exception 穿过 C boundary、错误 handle destroy 都必须由调用约定和测试明确。不要把“能编译”解释为“跨语言/跨编译器 ABI 已兼容”。
