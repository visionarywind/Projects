# M05 调用链

- 文档目的：从表生成、打开、单键读取和迭代器追踪控制流。
- 适用范围：M05。
- 源码版本：`main` / `7ee830d`。
- 证据状态：已确认；省略了通用 Env 实现细节。
- 最后更新：2026-09-10
- 前置阅读：[source-map](source-map.md)
- 后续阅读：[diagrams](diagrams.md)

## 生成 SSTable

```text
DBImpl::WriteLevel0Table / builder::BuildTable
  -> TableBuilder::Add
     -> FilterBlockBuilder::AddKey
     -> BlockBuilder::Add
     -> Flush when CurrentSizeEstimate >= block_size
        -> WriteBlock
           -> optional Snappy/Zstd
           -> WriteRawBlock + trailer/CRC
  -> Finish
     -> filter block
     -> metaindex block
     -> index block
     -> footer
```

`BuildTable` 负责迭代输入、Finish、Sync、Close 和失败删除；M05 负责 builder 内部格式。[db/builder.cc:16-79](../../../../db/builder.cc#L16-L79)

## TableCache 打开

```text
TableCache::FindTable
  -> Env::NewRandomAccessFile
  -> Table::Open
     -> file.Read(last Footer)
     -> Footer::DecodeFrom
     -> ReadBlock(index handle)
     -> new Block(index)
     -> ReadMeta -> ReadFilter (optional)
  -> cache.Insert(TableAndFile)
```

证据：[db/table_cache.cc:40-74](../../../../db/table_cache.cc#L40-L74)、[table/table.cc:37-107](../../../../table/table.cc#L37-L107)。

## 单键读取

```text
Version::Get
  -> TableCache::Get
     -> Table::InternalGet
        -> index_block iterator Seek(user/internal key)
        -> FilterBlockReader::KeyMayMatch
        -> Table::BlockReader
           -> block cache Lookup 或 ReadBlock
        -> data iterator Seek
        -> handle_result
```

过滤器只在能证明“不可能命中”时短路；否则仍读取 block。[table/table.cc:213-240](../../../../table/table.cc#L213-L240)

## Table 迭代器

```text
Table::NewIterator
  -> NewTwoLevelIterator(index iterator, BlockReader)
  -> Seek/Next/Prev
     -> InitDataBlock
     -> block iterator
     -> SkipEmptyDataBlocksForward/Backward
```

两级迭代器在 index block 和 data block 之间切换，并将 data iterator 的 status 汇总到外层。[table/two_level_iterator.cc:81-159](../../../../table/two_level_iterator.cc#L81-L159)

## 多表归并

Version 为多个文件构造子迭代器后，`NewMergingIterator` 逐个比较当前 child；实现使用简单数组而不是 heap，源码说明预期 child 数量较少。[table/merger.cc:13-20](../../../../table/merger.cc#L13-L20)、[table/merger.cc:134-175](../../../../table/merger.cc#L134-L175)
