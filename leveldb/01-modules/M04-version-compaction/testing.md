# M04 测试分析

- 文档目的：列出 Version/Manifest/Compaction 的验证策略。
- 适用范围：M04。
- 证据状态：测试存在已确认，执行未验证。
- 最后更新：2026-09-10
- 前置阅读：[M04 diagrams](diagrams.md)
- 后续阅读：[M07 testing](../M07-tests-build/testing.md)

## 测试矩阵

| 主题 | 文件 |
|---|---|
| VersionEdit encode/decode | `db/version_edit_test.cc` |
| file overlap/compaction selection | `db/version_set_test.cc` |
| automatic compaction | `db/autocompact_test.cc` |
| recovery and manifest | `db/recovery_test.cc` |
| output files and reads | `db/db_test.cc`、`table/table_test.cc` |

## 回归要求

所有影响文件集合的改动至少要测试重开；影响 snapshot/drop 的改动要有多版本读取；影响边界的改动要覆盖相邻 user key 和 boundary file。

## 相关文档

- [development-guide](development-guide.md)
- [risk-register](../../99-roadmap/risk-register.md)

## 源码证据摘要

[CMakeLists.txt:337-340](../../../../CMakeLists.txt#L337-L340)。

## 未解决问题

覆盖率和失败注入状态需构建后确认。

## 下一步阅读建议

先运行 version_set 相关测试，再做 DB 长压。
