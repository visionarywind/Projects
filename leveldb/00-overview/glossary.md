# 术语表

- 文档目的：统一知识库中的存储引擎术语。
- 适用范围：LevelDB 1.23.0。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：已确认术语定义，个别动机为推断。
- 最后更新：2026-09-10
- 前置阅读：[项目概览](project-overview.md)
- 后续阅读：[模块注册表](../01-modules/module-registry.md)
## 结论摘要

本页聚焦 00-overview/glossary.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 术语 | 含义 |
|---|---|
| WAL / log | 追加写的预写日志，保存最近更新并用于恢复。 |
| MemTable | 内存中的按 InternalKey 排序表；通常由 SkipList 支撑。 |
| immutable MemTable | 已切换、不再接收新写入、等待后台落盘的 MemTable。 |
| SSTable / Table | 按 key 排序的不可变磁盘表文件。 |
| InternalKey | user key 加序列号和 ValueType 的内部编码。 |
| SequenceNumber | 单调递增的写入版本号，用于 snapshot 可见性。 |
| Version | 某一时刻各 level 的表文件集合。 |
| VersionSet | 管理 Version 链、MANIFEST 元数据和 compaction 选择的对象。 |
| VersionEdit | 对版本/文件/日志编号的增量描述。 |
| MANIFEST | 以日志形式持久化 VersionEdit 的 descriptor 文件。 |
| CURRENT | 指向当前 MANIFEST 文件名的文本文件。 |
| Level-0 | 新落盘表所在层，文件 key range 可能重叠。 |
| Compaction | 合并多层表、减少旧版本/删除标记并生成新表的过程。 |
| TableCache | 按文件号缓存打开的 Table。 |
| Block cache | 缓存读取后的块，减少文件读取和解压。 |
| Env | 文件系统、锁、时间和后台调度的抽象。 |
| Snapshot | 不可变的一致读视图句柄。 |
| Slice | 指向外部字节数组的指针+长度，不拥有内存。 |
| Status | 以类型和消息表达成功/NotFound/IO/Corruption 等结果的对象。 |

## 相关文档

- [全局数据流](global-data-flow.md)
- [共享数据与类型](../90-cross-module/shared-data-and-types.md)

## 源码证据摘要

- [doc/impl.md:7-49](../../source/leveldb/doc/impl.md#L7-L49)
- [db/dbformat.h:50-66](../../source/leveldb/db/dbformat.h#L50-L66)
- [include/leveldb/slice.h](../../source/leveldb/include/leveldb/slice.h)

## 未解决问题

不同文章对 table/ldb/sstable 的历史命名可能不同；以当前 `filename` 解析规则为准。

## 下一步阅读建议

把术语与 M03/M04 的数据结构文档对照阅读。
