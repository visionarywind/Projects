# M03 调用链

- 文档目的：给出写入、恢复、查询和落盘准备的调用链。
- 适用范围：M03。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M03 README](README.md)
- 后续阅读：[M04 调用链](../M04-version-compaction/call-chains.md)

## 写入链

```text
DBImpl::Write
  -> WriteBatchInternal::SetSequence
  -> log::Writer::AddRecord(Contents(batch))
  -> WriteBatchInternal::InsertInto
    -> WriteBatch::Iterate
      -> MemTableInserter::Put/Delete
        -> MemTable::Add
          -> SkipList::Insert
```

证据：[db/write_batch.cc:114-136](../../../../db/write_batch.cc#L114-L136)、[db/db_impl.cc:1227-1246](../../../../db/db_impl.cc#L1227-L1246)。

## 恢复链

```text
DBImpl::RecoverLogFile
  -> log::Reader::ReadRecord
  -> WriteBatchInternal::SetContents
  -> WriteBatchInternal::InsertInto
  -> maybe BuildTable/WriteLevel0Table
```

证据：[db/db_impl.cc:385-504](../../../../db/db_impl.cc#L385-L504)。

## 查询链

```text
DBImpl::Get
  -> LookupKey
  -> MemTable::Get(mem_)
  -> MemTable::Get(imm_)
  -> Version::Get (M04)
```

## 相关文档

- [diagrams](diagrams.md)
- [line-level-analysis](line-level-analysis.md)

## 源码证据摘要

见各链。

## 未解决问题

跨线程写入序列化细节属于 M02。

## 下一步阅读建议

把每个节点的输入 Slice 生命周期与 sequence 记录在调试笔记中。
