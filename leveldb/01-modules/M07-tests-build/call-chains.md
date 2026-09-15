# M07 调用链

- 文档目的：从配置到构建、测试、benchmark 和安装追踪工程控制流。
- 适用范围：M07。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M07 data-structures](data-structures.md)
- 后续阅读：[M07 diagrams](diagrams.md)
## 结论摘要

本页聚焦 01-modules/M07-tests-build/call-chains.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

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
