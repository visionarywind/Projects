# M07 设计：工程入口与反馈环

- 文档目的：说明构建、测试、benchmark 和 CI 如何约束代码变化。
- 适用范围：M07。
- 源码版本：`main` / `7ee830d`。
- 证据状态：配置事实已确认；维护策略来自 README。
- 最后更新：2026-09-10
- 前置阅读：[M07 README](README.md)
- 后续阅读：[M07 call-chains](call-chains.md)

## 构建分层

CMake 先探测平台和可选库，再生成 `port_config.h`，构建 `leveldb` 库；测试和 benchmark 以库为依赖。[CMakeLists.txt:36-109](../../../../CMakeLists.txt#L36-L109)、[CMakeLists.txt:119-285](../../../../CMakeLists.txt#L119-L285) 这使同一源码可在依赖存在/缺失时编译，但压缩和 CRC 行为可能不同。

## 测试分层

核心 GoogleTest 聚合目标 `leveldb_tests` 包含 DB、WAL、Version、Table、util 和 MemEnv 测试；C API 与平台 Env 测试通过 `leveldb_test` 函数单独注册 CTest。[CMakeLists.txt:312-403](../../../../CMakeLists.txt#L312-L403) 共享库构建时部分内部测试被排除，因测试依赖内部符号/实现布局。

## CI 作为支持矩阵

CI 对 Ubuntu/macOS/Windows 以及 clang/gcc/msvc 组合执行 configure、build、CTest、benchmark 和 install。[.github/workflows/build.yml:20-47](../../../../.github/workflows/build.yml#L20-L47)、[.github/workflows/build.yml:74-102](../../../../.github/workflows/build.yml#L74-L102) README 又要求只修改已测试平台，保持稳定 API，并为代码修改提供测试。[README.md:89-115](../../../../README.md#L89-L115)

## 维护取舍

README 明确项目处于有限维护，重点是严重 bug 和内部支持客户端所需的兼容修复。[README.md:2-6](../../../../README.md#L2-L6) 因此新功能不仅要能构建，还要评估 ABI、平台矩阵、持久化兼容和长期维护成本。
