# M04 源码地图

- 文档目的：映射版本和压缩的主要文件。
- 适用范围：M04。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M04 README](README.md)
- 后续阅读：[M04 call-chains](call-chains.md)
## 结论摘要

本页聚焦 01-modules/M04-version-compaction/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 源码 | 作用 | 重要性 |
|---|---|---|
| `db/version_set.h/.cc` | Version、VersionSet、Compaction、读和选压缩 | 高 |
| `db/version_edit.h/.cc` | FileMetaData、增量编解码 | 高 |
| `db/builder.h/.cc` | Iterator → SSTable、填充元数据 | 高 |
| `db/table_cache.h/.cc` | 文件号到 Table/Iterator/Get | 高 |
| `db/filename.h/.cc` | 数据库文件命名/解析 | 中高 |
| `db/snapshot.h` | SnapshotList | 中高 |
| `db/db_impl.cc:505-1079` | flush/compaction 执行者 | 高 |
| `db/version_set_test.cc` | 版本/压缩输入测试 | 高 |
| `db/version_edit_test.cc` | VersionEdit 编码测试 | 中高 |

## 相关文档

- [line-level-analysis](line-level-analysis.md)
- [testing](testing.md)

## 源码证据摘要

[CMakeLists.txt:145-152](../../../source/leveldb/CMakeLists.txt#L145-L152) 列出主要源文件。

## 未解决问题

TableCache 的缓存淘汰细节在 M05/M06 交叉说明。

## 下一步阅读建议

先看 `version_set.h` 字段和 lock 要求。
