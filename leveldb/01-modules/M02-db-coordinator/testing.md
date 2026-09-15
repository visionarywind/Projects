# M02 测试分析

- 文档目的：说明 DB 协调器的测试边界和如何补回归。
- 适用范围：DBImpl 相关测试。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：测试清单已确认，执行未验证。
- 最后更新：2026-09-10
- 前置阅读：[M02 README](README.md)
- 后续阅读：[M07](../M07-tests-build/README.md)
## 结论摘要

本页聚焦 01-modules/M02-db-coordinator/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 测试矩阵

| 路径 | 测试 |
|---|---|
| Open/create/options | `db/db_test.cc` |
| Put/Get/Delete/iterator/snapshot | `db/db_test.cc` |
| WAL replay/reopen | `db/recovery_test.cc` |
| corruption | `db/corruption_test.cc` |
| automatic/manual compaction | `db/autocompact_test.cc`、`db/db_test.cc` |
| injected IO failures | `db/fault_injection_test.cc`（当前 CMake 注释） |

## 新回归测试

使用已有 DBTest fixture 和 MemEnv；先构造最小输入，再断言 Status、值、文件属性或 compaction 结果。涉及并发时控制 Env/CondVar 并避免时间睡眠作为唯一同步手段。

## 相关文档

- [examples](examples.md)
- [development-guide](development-guide.md)

## 源码证据摘要

[CMakeLists.txt:313-352](../../../source/leveldb/CMakeLists.txt#L313-L352)。

## 未解决问题

实际覆盖率、flaky 情况和 fault injection 的当前可运行状态需执行 CTest。

## 下一步阅读建议

从 `DBTest::Reopen` 和恢复测试开始。
