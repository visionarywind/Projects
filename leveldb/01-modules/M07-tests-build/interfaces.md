# M07 接口与工程契约

- 文档目的：记录构建选项、目标和验证入口。
- 适用范围：M07。
- 源码版本：`main` / `7ee830d`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M07 source-map](source-map.md)
- 后续阅读：[M07 data-structures](data-structures.md)

## CMake 选项

- `LEVELDB_BUILD_TESTS` 默认 ON。
- `LEVELDB_BUILD_BENCHMARKS` 默认 ON。
- `LEVELDB_INSTALL` 默认 ON。[CMakeLists.txt:32-34](../../../../CMakeLists.txt#L32-L34)

CMake 要求 C++17；C 可降级到 C89/C11 配置；非 MSVC 默认禁用 exceptions 和 RTTI。[CMakeLists.txt:8-22](../../../../CMakeLists.txt#L8-L22)、[CMakeLists.txt:55-77](../../../../CMakeLists.txt#L55-L77)

## 目标

- `leveldb`：核心库。
- `leveldbutil`：工具程序。
- `leveldb_tests`：主要聚合单元测试。
- `c_test`、`env_posix_test`/`env_windows_test`：独立 CTest 目标。
- `db_bench` 及可选 SQLite/KyotoCabinet benchmark。[CMakeLists.txt:287-291](../../../../CMakeLists.txt#L287-L291)、[CMakeLists.txt:312-403](../../../../CMakeLists.txt#L312-L403)、[CMakeLists.txt:412-467](../../../../CMakeLists.txt#L412-L467)

## 贡献契约

README 要求稳定 API、测试平台、伴随测试/充分解释，并遵循 Google C++ Style。[README.md:89-109](../../../../README.md#L89-L109)
