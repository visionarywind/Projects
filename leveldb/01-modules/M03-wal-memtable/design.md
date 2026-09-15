# M03 设计

- 文档目的：说明 WAL、批次和 MemTable 的协作及取舍。
- 适用范围：M03。
- 证据状态：机制已确认，性能动机部分推断。
- 最后更新：2026-09-10
- 前置阅读：[M03 README](README.md)
- 后续阅读：[M03 调用链](call-chains.md)

## 结论摘要

同一 WriteBatch 同时是原子编辑集合、WAL 载荷和 MemTable 插入来源。sequence 为每个编辑建立可见性顺序；InternalKey 将 user key 与序列号/类型绑定，因而读和 compaction 可在合并多个版本时判断可见记录。

## 设计约束

1. WriteBatch header 固定为 8 字节 sequence + 4 字节 count；记录带 tag 和 length-prefixed bytes。[db/write_batch.cc:4-26](../../../source/leveldb/db/write_batch.cc#L4-L26)
2. WAL 物理块固定 32768 字节，header 为 CRC(4)+length(2)+type(1)，大记录用 First/Middle/Last 分片。[db/log_format.h:13-29](../../../source/leveldb/db/log_format.h#L13-L29)
3. InternalKey 的 `ValueType` 枚举值嵌入持久化结构，不能重排；序列号按降序比较。[db/dbformat.h:50-116](../../../source/leveldb/db/dbformat.h#L50-L116)
4. MemTable 的 Arena 负责条目存储，SkipList 负责有序索引；引用计数由调用者管理。[db/memtable.h:19-82](../../../source/leveldb/db/memtable.h#L19-L82)

## 取舍

- WAL 顺序追加和批处理降低随机 IO，但 recovery 需要重放，sync=false 有机器崩溃窗口。
- Arena 避免每条记录单独分配，代价是 MemTable 生命周期内内存不能逐条回收。
- InternalKey 允许多版本共存，compaction 才清理旧值/删除标记。
- Reader 对尾部半条记录容忍，能处理进程在写入中崩溃；CRC/坏类型则报告损坏。

## 相关文档

- [interfaces](interfaces.md)
- [data-structures](data-structures.md)
- [risks](risks-and-debt.md)

## 源码证据摘要

见约束列表。

## 未解决问题

批量大小、写放大和恢复时间需要 benchmark 量化。

## 下一步阅读建议

阅读 `dbformat.h` 的比较规则，然后读 log reader 的错误分支。
