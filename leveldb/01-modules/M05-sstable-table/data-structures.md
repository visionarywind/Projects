# M05 数据结构与生命周期

- 文档目的：说明表文件内外的主要对象、编码和所有权。
- 适用范围：M05。
- 源码版本：`main` / `7ee830d`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[interfaces](interfaces.md)
- 后续阅读：[call-chains](call-chains.md)

## 文件布局

```text
[SSTable]
  data block 0 ... data block N
  filter block              (可选)
  metaindex block
  index block
  footer: metaindex handle + index handle + magic
```

每个 block 后紧跟 1 字节 compression type 和 4 字节 masked CRC；`BlockHandle` 的 size 只覆盖 block contents，不覆盖 trailer。[table/table_builder.cc:140-207](../../../../table/table_builder.cc#L140-L207)、[table/format.h:45-89](../../../../table/format.h#L45-L89)

## Block entry

单条 data entry 为：

```text
shared_bytes   varint32
unshared_bytes varint32
value_length   varint32
key_delta      bytes
value          bytes
```

restart 数组位于 block 尾部，最后一个 fixed32 是 restart 数量。[table/block_builder.cc:15-26](../../../../table/block_builder.cc#L15-L26) `Block::Iter` 用 `key_` 重建完整 key；`value_` 直接指向 block 数据。[table/block.cc:76-88](../../../../table/block.cc#L76-L88)

## 过滤器

FilterBlockBuilder 将多个数据范围的 key 展平交给 `FilterPolicy`，在末尾保存每个 filter 的 offset 数组、数组起点和 `base_lg`。[table/filter_block.cc:50-74](../../../../table/filter_block.cc#L50-L74) Reader 只保存指针，不复制内容；因此 Table 的 filter bytes 必须一直有效。[table/filter_block.h:51-63](../../../../table/filter_block.h#L51-L63)

## 对象所有权

| 对象 | 创建者 | 释放者/时机 |
|---|---|---|
| `Table::Rep` | `Table::Open` | `Table::~Table` |
| `index_block` | `Table::Open` | `Rep` 析构 |
| `RandomAccessFile` | `TableCache::FindTable` | `DeleteEntry` 与 Table 一起释放 |
| data `Block` | `Table::BlockReader` 或 cache | iterator cleanup 或 cache eviction |
| cache handle | `TableCache`/`BlockReader` | `TableCache::Get` 或 iterator cleanup |
| `Table` | TableCache 插入的 `TableAndFile` | cache entry 删除 |

证据：[table/table.cc:19-35](../../../../table/table.cc#L19-L35)、[table/table.cc:135-148](../../../../table/table.cc#L135-L148)、[db/table_cache.cc:13-29](../../../../db/table_cache.cc#L13-L29)。

## 关键不变量

- builder 输入必须严格按 comparator 排序。
- block 至少包含合法 restart 尾部；否则 `Block` 置为错误标记。[table/block.cc:24-39](../../../../table/block.cc#L24-L39)
- filter 只能减少读取，不能把潜在命中误报为“不存在”。
- `Table` 不拥有传给 `Table::Open` 的文件；在 TableCache 中则由 `TableAndFile` 统一拥有。
