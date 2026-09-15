# M03 源码地图

- 文档目的：建立 WAL、批次、内部键和内存表的文件/符号索引。
- 适用范围：M03。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M03 README](README.md)
- 后续阅读：[M03 行级分析](line-level-analysis.md)

| 源码 | 关键内容 | 调用者/下游 | 重要性 |
|---|---|---|---|
| `db/write_batch.cc` | batch 编解码、Handler、InsertInto | M02、测试 | 高 |
| `db/write_batch_internal.h` | 私有 rep 操作 | M02/M03 | 高 |
| `db/log_format.h` | 物理格式常量 | reader/writer | 高 |
| `db/log_writer.cc` | 分片、CRC、Flush | M02 | 高 |
| `db/log_reader.cc` | 重组、校验、corruption | recovery/repair | 高 |
| `db/dbformat.h/.cc` | InternalKey、LookupKey、filter wrapper | M02/M04/M05 | 高 |
| `db/memtable.h/.cc` | Arena + SkipList、Get | M02/M04 | 高 |
| `db/skiplist.h` | lock-free-ish读、插入结构 | MemTable | 中高 |
| `db/builder.cc` | MemTable iterator → TableBuilder | M04/M05 | 高 |
| `db/log_test.cc` | physical/logical record 测试 | M07 | 高 |
| `db/write_batch_test.cc` | batch 格式和插入测试 | M07 | 高 |
| `db/skiplist_test.cc` | SkipList 测试 | M07 | 中 |

## 相关文档

- [call-chains](call-chains.md)
- [testing](testing.md)

## 源码证据摘要

核心源码由 [CMakeLists.txt:135-152](../../../source/leveldb/CMakeLists.txt#L135-L152) 纳入库。

## 未解决问题

SkipList 的完整内存排序不变量需按模板实现逐段阅读。

## 下一步阅读建议

从 `WriteBatch::Iterate` 和 `MemTable::Add` 开始。
