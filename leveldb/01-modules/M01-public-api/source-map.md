# M01 源码地图

- 文档目的：映射公开头文件、实现入口与测试。
- 适用范围：M01。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M01 README](README.md)
- 后续阅读：[M02 source-map](../M02-db-coordinator/source-map.md)

| 源码 | 类型 | 作用 | 重要性 |
|---|---|---|---|
| `include/leveldb/db.h` | API | DB、Snapshot、Range、Destroy/Repair | 高 |
| `include/leveldb/options.h` | 配置 | Options/ReadOptions/WriteOptions | 高 |
| `include/leveldb/env.h` | 抽象 | 文件、锁、线程、调度 | 高 |
| `include/leveldb/write_batch.h` | API | 原子编辑集合 | 高 |
| `include/leveldb/iterator.h` | API | 有序遍历 | 高 |
| `include/leveldb/status.h` | 值类型 | 错误语义 | 高 |
| `include/leveldb/slice.h` | 值类型 | 借用字节视图 | 高 |
| `db/c.cc` | C 适配 | C API 到 C++ API | 中 |
| `db/db_test.cc` | 测试 | 主要行为覆盖 | 高 |

## 相关文档

- [interfaces](interfaces.md)
- [testing](testing.md)

## 源码证据摘要

[CMakeLists.txt:193-210](../../../source/leveldb/CMakeLists.txt#L193-L210) 列出安装的公开头文件。

## 未解决问题

C API 的每一项映射需在 M02/C API 文档中细化。

## 下一步阅读建议

阅读 `include/leveldb/db.h` 后追踪 `db/db_impl.cc`。
