# M07 设计：工程入口与反馈环

- 文档目的：说明构建、测试、benchmark 和 CI 如何约束代码变化。
- 适用范围：M07。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：配置事实已确认；维护策略来自 README。
- 最后更新：2026-09-10
- 前置阅读：[M07 README](README.md)
- 后续阅读：[M07 call-chains](call-chains.md)
## 结论摘要

本页聚焦 01-modules/M07-tests-build/design.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 构建分层

CMake 先探测平台和可选库，再生成 `port_config.h`，构建 `leveldb` 库；测试和 benchmark 以库为依赖。[CMakeLists.txt:36-109](../../../source/leveldb/CMakeLists.txt#L36-L109)、[CMakeLists.txt:119-285](../../../source/leveldb/CMakeLists.txt#L119-L285) 这使同一源码可在依赖存在/缺失时编译，但压缩和 CRC 行为可能不同。

## 测试分层

核心 GoogleTest 聚合目标 `leveldb_tests` 包含 DB、WAL、Version、Table、util 和 MemEnv 测试；C API 与平台 Env 测试通过 `leveldb_test` 函数单独注册 CTest。[CMakeLists.txt:312-403](../../../source/leveldb/CMakeLists.txt#L312-L403) 共享库构建时部分内部测试被排除，因测试依赖内部符号/实现布局。

## CI 作为支持矩阵

CI 对 Ubuntu/macOS/Windows 以及 clang/gcc/msvc 组合执行 configure、build、CTest、benchmark 和 install。[.github/workflows/build.yml:20-47](../../../source/leveldb/.github/workflows/build.yml#L20-L47)、[.github/workflows/build.yml:74-102](../../../source/leveldb/.github/workflows/build.yml#L74-L102) README 又要求只修改已测试平台，保持稳定 API，并为代码修改提供测试。[README.md:89-115](../../../source/leveldb/README.md#L89-L115)

## 维护取舍

README 明确项目处于有限维护，重点是严重 bug 和内部支持客户端所需的兼容修复。[README.md:2-6](../../../source/leveldb/README.md#L2-L6) 因此新功能不仅要能构建，还要评估 ABI、平台矩阵、持久化兼容和长期维护成本。

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
