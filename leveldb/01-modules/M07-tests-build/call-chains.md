# M07 调用链

- 文档目的：从配置到构建、测试、benchmark 和安装追踪工程控制流。
- 适用范围：M07。
- 源码版本：`main` / `7ee830d`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M07 data-structures](data-structures.md)
- 后续阅读：[M07 diagrams](diagrams.md)

## Configure/build

```text
cmake -S . -B build
  -> compiler/platform checks
  -> optional library checks
  -> port_config.h
  -> target leveldb
cmake --build build
  -> compile db/table/util/port/env
  -> link leveldb
```

## CTest

```text
CMake LEVELDB_BUILD_TESTS=ON
  -> add_subdirectory(third_party/googletest)
  -> target leveldb_tests
  -> add_test(leveldb_tests)
  -> leveldb_test(c_test / env_*_test)
  -> ctest
```

证据：[CMakeLists.txt:292-404](../../../source/leveldb/CMakeLists.txt#L292-L404)。

## CI

```text
checkout --submodules
  -> Linux optional dependencies
  -> cmake configure + install prefix
  -> cmake build
  -> ctest --verbose
  -> db_bench
  -> optional SQLite/Kyoto benchmark
  -> install target
```

证据：[.github/workflows/build.yml:61-102](../../../source/leveldb/.github/workflows/build.yml#L61-L102)。

## 修改到反馈

```text
源码改动
 -> 受影响模块测试
 -> leveldb_tests / 独立 CTest
 -> db_bench 性能基线
 -> CI 多平台/配置
 -> install/package ABI 检查
```
