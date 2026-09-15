# M07 接口与工程契约

- 文档目的：记录构建选项、目标和验证入口。
- 适用范围：M07。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M07 source-map](source-map.md)
- 后续阅读：[M07 data-structures](data-structures.md)
## 结论摘要

本页聚焦 01-modules/M07-tests-build/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## CMake 选项

- `LEVELDB_BUILD_TESTS` 默认 ON。
- `LEVELDB_BUILD_BENCHMARKS` 默认 ON。
- `LEVELDB_INSTALL` 默认 ON。[CMakeLists.txt:32-34](../../../source/leveldb/CMakeLists.txt#L32-L34)

CMake 要求 C++17；C 可降级到 C89/C11 配置；非 MSVC 默认禁用 exceptions 和 RTTI。[CMakeLists.txt:8-22](../../../source/leveldb/CMakeLists.txt#L8-L22)、[CMakeLists.txt:55-77](../../../source/leveldb/CMakeLists.txt#L55-L77)

## 目标

- `leveldb`：核心库。
- `leveldbutil`：工具程序。
- `leveldb_tests`：主要聚合单元测试。
- `c_test`、`env_posix_test`/`env_windows_test`：独立 CTest 目标。
- `db_bench` 及可选 SQLite/KyotoCabinet benchmark。[CMakeLists.txt:287-291](../../../source/leveldb/CMakeLists.txt#L287-L291)、[CMakeLists.txt:312-403](../../../source/leveldb/CMakeLists.txt#L312-L403)、[CMakeLists.txt:412-467](../../../source/leveldb/CMakeLists.txt#L412-L467)

## 贡献契约

README 要求稳定 API、测试平台、伴随测试/充分解释，并遵循 Google C++ Style。[README.md:89-109](../../../source/leveldb/README.md#L89-L109)

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
