# M07 测试分析

- 文档目的：建立从局部回归到 CI/benchmark 的验证矩阵。
- 适用范围：M07。
- 源码版本：`main` / `7ee830d`。
- 证据状态：测试注册已确认；本轮命令执行状态未验证。
- 最后更新：2026-09-10
- 前置阅读：[M07 examples](examples.md)
- 后续阅读：[M07 development-guide](development-guide.md)

## 测试矩阵

| 变更主题 | 首选测试 |
|---|---|
| API/C API | `db/c_test.c`、`db/db_test.cc` |
| WAL/恢复 | `db/log_test.cc`、`db/recovery_test.cc` |
| Version/Manifest | `db/version_edit_test.cc`、`db/version_set_test.cc` |
| MemTable/内部 key | `db/dbformat_test.cc`、`db/skiplist_test.cc` |
| SSTable/block/filter | `table/table_test.cc`、`table/filter_block_test.cc` |
| Cache/Arena/coding | `util/cache_test.cc`、`util/arena_test.cc`、对应 util tests |
| Env/平台 | `util/env_posix_test.cc` 或 Windows 对应目标 |
| 故障/持久化 | `db/fault_injection_test.cc`（当前 CMake 中注释，需单独评估） |
| 性能 | `benchmarks/db_bench.cc` |

CMake 当前将 fault injection 和若干 issues 测试以注释形式排除，同时把核心测试加入 `leveldb_tests`。[CMakeLists.txt:312-350](../../../source/leveldb/CMakeLists.txt#L312-L350)

## 回归层级

1. 编译受影响 target。
2. 运行最小单测/CTest target。
3. 运行 `leveldb_tests` 全聚合。
4. 做重开、损坏、并发或资源测试。
5. 对性能改动运行固定 benchmark。
6. 在支持平台和 Debug/RelWithDebInfo 下复现 CI。

## 验收原则

测试通过不等于格式兼容或资源安全；要结合重开、Sanitizer、故障注入和平台矩阵。构建、测试、benchmark 的实际结果必须以命令输出为准。
