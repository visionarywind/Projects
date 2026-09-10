# M03 行级源码分析

- 文档目的：覆盖 M03 最关键逻辑块、输入输出、复杂度和风险。
- 适用范围：`write_batch.cc`、`log_writer.cc`、`log_reader.cc`、`memtable.cc`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M03 call-chains](call-chains.md)
- 后续阅读：[M03 testing](testing.md)

| 代码块 | 意图 | 输入/输出 | 状态变化 | 风险 |
|---|---|---|---|---|
| `write_batch.cc:34-79` | 清空、解析和校验 batch | bytes → Handler/Status | 无外部状态 | malformed length/tag |
| `write_batch.cc:97-108` | 编码 Put/Delete | Slice → rep_ | count 增加 | 大 key/value 内存增长 |
| `write_batch.cc:131-136` | 批量插入 MemTable | batch → Status | sequence 递增 | 中途错误需保持语义 |
| `log_writer.cc:33-79` | 计算片段和 record type | Slice → physical records | block_offset 更新 | block 边界 |
| `log_writer.cc:81-107` | CRC/header/payload 写入 | fragment → file | offset/flush | IO 错误 |
| `log_reader.cc:90-170` | 重组/报错/EOF | fragments → logical record | scratch/in_fragmented | 截断与损坏区分 |
| `memtable.cc:75-99` | Arena 分配并编码 entry | key/value → node | SkipList 插入 | 生命周期/溢出 |
| `memtable.cc:101-135` | snapshot seek 和类型判断 | LookupKey → value/Status | 无修改 | comparator 必须正确 |

## 复杂度

Batch Iterate 是 O(batch bytes)；WAL 写入是 O(record bytes)；MemTable Add 的索引插入平均取决于 SkipList 高度，读取为 seek 加后续常数检查。具体 benchmark 不在本次静态分析中确认。

## 调试变量

观察 `WriteBatchInternal::Sequence/Count`、log `block_offset_`、Reader `in_fragmented_record`、MemTable iterator 的 internal key tag，以及 `LookupKey` 的 sequence。

## 相关文档

- [data-structures](data-structures.md)
- [risks](risks-and-debt.md)

## 源码证据摘要

见表格。

## 未解决问题

性能/分配量需使用 benchmark 或 allocator instrumentation 验证。

## 下一步阅读建议

阅读 M05 表构建如何消费 MemTable iterator。
