# M03 接口与协议

- 文档目的：说明 M03 的内部协议和调用者责任。
- 适用范围：WAL、WriteBatch、MemTable。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M03 design](design.md)
- 后续阅读：[M04 interfaces](../M04-version-compaction/interfaces.md)
## 结论摘要

本页聚焦 01-modules/M03-wal-memtable/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## WriteBatch 协议

`rep_` 的前 12 字节是 sequence/count，随后是 `kTypeValue` 的 key/value 或 `kTypeDeletion` 的 key。`Iterate` 会验证 header、tag、长度和 count；非法输入返回 Corruption。[db/write_batch.cc:41-79](../../../source/leveldb/db/write_batch.cc#L41-L79)

## WAL 协议

Writer 接受一个逻辑 Slice，按 32KiB block 分片，每个物理片段写 7 字节 header、payload，并 Flush；Reader 负责 CRC、片段顺序和逻辑记录组装。[db/log_writer.cc:33-107](../../../source/leveldb/db/log_writer.cc#L33-L107)、[db/log_reader.cc:90-170](../../../source/leveldb/db/log_reader.cc#L90-L170)

## MemTable 协议

`Add(seq,type,key,value)` 要求 key/value Slice 在调用期间有效；`Get` 在 `LookupKey` 的 sequence 上限下返回 value、NotFound 或 false。Iterator 使用时底层 MemTable 必须保持 live。[db/memtable.h:40-62](../../../source/leveldb/db/memtable.h#L40-L62)

## 兼容性

不能改变 log type、block/header 常量、WriteBatch header 或 ValueType 持久化值而不更新兼容策略；坏记录和尾截断的处理是恢复语义的一部分。

## 相关文档

- [data-structures](data-structures.md)
- [global error model](../../00-overview/global-error-model.md)

## 源码证据摘要

见正文引用。

## 未解决问题

日志文件的旧版本兼容案例需结合 recovery tests 和历史提交补充。

## 下一步阅读建议

阅读 `doc/log_format.md` 与 `db/log_test.cc`。
