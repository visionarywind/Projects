# M07 测试、基准与工程入口

- 文档目的：解释 CMake、测试注册、CI、benchmark 和安装入口。
- 适用范围：`CMakeLists.txt`、`.github/workflows/build.yml`、测试和 `benchmarks/`。
- 源码版本：`main`，HEAD `7ee830d`。
- 证据状态：构建配置和 CI 已确认；本机执行状态需以实际命令结果更新。
- 最后更新：2026-09-10
- 前置阅读：[M06 Env](../M06-env-platform/README.md)
- 后续阅读：[实践层 quick-start](../../99-roadmap/quick-start.md)

## 结论摘要

项目以 CMake 为唯一主要工程入口：C++17、C11、POSIX/Windows 平台选择、可选 crc32c/Snappy/Zstd/tcmalloc；默认构建静态库、测试、benchmark 和安装规则。GoogleTest 测试多数聚合到 `leveldb_tests`，另有 C API 和平台 Env 独立测试；CI 覆盖 Ubuntu/macOS/Windows 与 clang/gcc/msvc 的 Debug/RelWithDebInfo 组合。

## 关键入口

- 版本和语言：[CMakeLists.txt:4-22](../../../source/leveldb/CMakeLists.txt#L4-L22)
- 核心库源文件：[CMakeLists.txt:119-231](../../../source/leveldb/CMakeLists.txt#L119-L231)
- 测试注册：[CMakeLists.txt:292-404](../../../source/leveldb/CMakeLists.txt#L292-L404)
- benchmark：[CMakeLists.txt:406-468](../../../source/leveldb/CMakeLists.txt#L406-L468)
- 安装：[CMakeLists.txt:470-518](../../../source/leveldb/CMakeLists.txt#L470-L518)
- CI：[.github/workflows/build.yml:10-102](../../../source/leveldb/.github/workflows/build.yml#L10-L102)

## 相关文档

- [design](design.md)
- [source-map](source-map.md)
- [interfaces](interfaces.md)
- [data-structures](data-structures.md)
- [call-chains](call-chains.md)
- [diagrams](diagrams.md)
- [line-level-analysis](line-level-analysis.md)
- [examples](examples.md)
- [testing](testing.md)
- [development-guide](development-guide.md)
- [risks-and-debt](risks-and-debt.md)
