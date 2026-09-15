# M03 测试分析

- 文档目的：说明协议和内存表的测试边界。
- 适用范围：M03。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：测试文件和 CMake 注册已确认，执行未验证。
- 最后更新：2026-09-10
- 前置阅读：[M03 diagrams](diagrams.md)
- 后续阅读：[M07 testing](../M07-tests-build/testing.md)
## 结论摘要

本页聚焦 01-modules/M03-wal-memtable/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 现有测试

| 主题 | 文件 | 重点 |
|---|---|---|
| log physical format | `db/log_test.cc` | block boundary、fragment、CRC、offset |
| WriteBatch | `db/write_batch_test.cc` | 编码、count、sequence、append、插入 |
| internal format | `db/dbformat_test.cc` | comparator、解析、LookupKey |
| MemTable ordering | `db/skiplist_test.cc`、`db/db_test.cc` | 查找与排序 |
| recovery | `db/recovery_test.cc` | 重启、日志重放 |

## 新测试配方

格式改动：先写旧/新 bytes 编解码测试，再写 reopen/recovery；并发改动：复用 DBTest 和 Env，断言最终值/Status，不以 sleep 作为唯一同步；内存改动：用 MemTable iterator、ASan 和引用计数断言。

## 相关文档

- [M03 risks](risks-and-debt.md)
- [项目 testing recipes](../../99-roadmap/testing-recipes.md)

## 源码证据摘要

[CMakeLists.txt:333-341](../../../source/leveldb/CMakeLists.txt#L333-L341)。

## 未解决问题

覆盖率和 flaky test 需实际运行统计。

## 下一步阅读建议

重点先跑 `log_test` 和 `write_batch_test`。
