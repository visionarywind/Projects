# M05 端到端示例：一次 Get 如何走到 Block

- 文档目的：把一个 SSTable 的构造、缓存、过滤、索引定位和读取错误串成完整场景。
- 适用范围：M05 与 M02/M04/M06 的交界。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：正常控制流和资源边界来自源码；命令未在本轮执行。
- 最后更新：2026-09-10
- 前置阅读：[M05 interfaces](interfaces.md)
- 后续阅读：[M05 testing](testing.md)
## 结论摘要

本页聚焦 01-modules/M05-sstable-table/examples.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 场景设定

假设 M04 已经把文件 `N.ldb` 放入某个 Version，调用者执行 `db->Get(ReadOptions(), "k", &value)`。本例从 M02 的 `Version::Get` 开始，追到 M05 的 `TableCache`、footer/index/filter/data block，再回到 `Version::Get` 的 `Saver`。

## 完整链路

```text
DBImpl::Get
  -> Version::Get(LookupKey("k", snapshot))
     -> ForEachOverlapping 找到 FileMetaData(N)
        -> TableCache::Get
           -> FindTable(N)
              -> cache Lookup(file number)
              -> Env::NewRandomAccessFile
              -> Table::Open
                 -> ReadFooter
                 -> Read index block
                 -> Read filter metadata（可选）
              -> cache.Insert(TableAndFile)
           -> Table::InternalGet
              -> index iterator Seek
              -> filter KeyMayMatch
              -> BlockReader/data block
              -> data iterator Seek
              -> SaveValue
        -> Saver state = kFound / kDeleted / kNotFound
  -> return value or Status
```

对应入口：[`Version::Get`](../../../source/leveldb/db/version_set.cc#L324-L400)、[`TableCache::FindTable/Get`](../../../source/leveldb/db/table_cache.cc#L40-L111)、[`Table::Open/InternalGet`](../../../source/leveldb/table/table.cc#L37-L240)。

## 逐阶段状态

| 阶段 | 输入 | 输出 | 资源/所有权 | 错误 |
|---|---|---|---|---|
| Version 候选 | user key + InternalKey | FileMetaData | Version 由 Get 临时 Ref | 不命中继续其他文件 |
| FindTable cache hit | file number | cache handle | handle 必须由 caller Release | 无 |
| FindTable cache miss | file number/size | TableAndFile + handle | cache entry 拥有 file/table | open/open format error，不缓存 |
| InternalGet index | LookupKey | block handle | Table 借用 cache entry | index/block corruption |
| Filter | key + block offset | may-match bool | filter 属 Table | 异常应按可能命中 |
| Data block | handle | iterator | block cache handle 由 iterator cleanup 释放 | CRC/decompress/read error |
| Saver callback | user key/value | state/value string | value 复制到 caller string | deletion/not-found |
| TableCache return | Status | Version state | 释放 handle | 向上返回 |

## 为什么 cache handle 不能提前释放

`TableCache::Get` 在 `FindTable` 成功后从 handle 取得 Table，调用 `InternalGet`，直到调用结束才 `cache_->Release(handle)`。如果在 `InternalGet` 前释放，cache eviction 可能删除 `TableAndFile`，Table 和 file 指针立即悬空。Iterator 路径不同：`TableCache::NewIterator` 把 Release 注册到 iterator cleanup，因此 iterator 活着时 cache entry 继续活着。[`TableCache::NewIterator/Get`](../../../source/leveldb/db/table_cache.cc#L77-L111)、[`DeleteEntry`](../../../source/leveldb/db/table_cache.cc#L13-L29)

## 最小构造/读取示例

```cpp
// 概念示例；TableBuilder 需要已排序的 InternalKey/Comparator 输入。
StringSink sink;
leveldb::Options options;
leveldb::TableBuilder builder(options, &sink);
builder.Add("a", "one");
builder.Add("k", "value");
leveldb::Status s = builder.Finish();
// 真实 DB 路径还需要 Table::Open 的 file size、RandomAccessFile 和 TableCache。
```

Builder 的 `Add` 在 block 达到阈值时 Flush，Finish 会写 metaindex、index 和 footer；数据的读写格式必须由同一个 comparator 和对应的 format reader 解码。[`TableBuilder::Add/Finish`](../../../source/leveldb/table/table_builder.cc#L93-L277)、[`BlockBuilder`](../../../source/leveldb/table/block_builder.cc#L39-L104)、[`Footer/ReadBlock`](../../../source/leveldb/table/format.cc#L15-L161)

## 三个边界测试

1. **Filter false**：key 不在 filter 时可以跳过 data block，但错误/未知格式不得 false-negative；应检查尚未读取 data block 的路径。
2. **CRC/压缩错误**：破坏 block trailer 或压缩 payload，检查 iterator/status 返回 corruption/IO，不要只检查 `Valid()`。
3. **Cache eviction**：保持 Table iterator 后调用 `Evict`，iterator 仍应凭 handle 读取；删除 iterator 后才允许 deleter 回收 Table/file。

这些测试命令当前未执行；测试入口见 `table/table_test.cc`、`table/filter_block_test.cc` 和 `db/table_cache_test.cc`。

## 相关文档

- [M05 调用链](call-chains.md)
- [M05 数据结构](data-structures.md)
- [端到端单键读取](../../90-cross-module/end-to-end-traces.md#链-4单键读取从快照序号到-table-block)

## 源码证据摘要

- [`TableCache`](../../../source/leveldb/db/table_cache.cc#L13-L117)
- [`Table::InternalGet`](../../../source/leveldb/table/table.cc#L152-L240)
- [`TableBuilder`](../../../source/leveldb/table/table_builder.cc#L93-L277)
- [`BlockBuilder`](../../../source/leveldb/table/block_builder.cc#L39-L104)

## 未解决问题

实际缓存命中率、压缩选择和文件系统读取次数必须用 benchmark/运行时统计测量；本文不提供性能数字。

## 下一步阅读建议

先在 `table/table.cc` 标出 index/filter/data 三次边界，再回到 `Version::Get` 查看多个候选文件如何决定最终 `Saver` 状态。
