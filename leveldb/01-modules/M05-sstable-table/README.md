# M05 SSTable、Block 与迭代器

- 文档目的：解释不可变表文件的生成、布局、校验和读取路径。
- 适用范围：`table/`、`include/leveldb/table.h`、`db/table_cache.*`。
- 源码版本：`main`，HEAD `7ee830d`（LevelDB 1.23 系列）。
- 证据状态：核心机制已确认；性能结论需基准验证。
- 最后更新：2026-09-10
- 前置阅读：[M04 Version/Compaction](../M04-version-compaction/README.md)
- 后续阅读：[M06 Env](../M06-env-platform/README.md)

## 结论摘要

M05 将有序键值序列编码为不可变 SSTable：`TableBuilder` 依次写 data block、filter/metaindex、index block 和 footer；`Table::Open` 先验证 footer 并加载 index；读取时先定位 index，再按 `BlockHandle` 读取 data block，必要时使用 filter 和 block cache。表对象及其迭代器通过 cleanup 回调释放文件、缓存句柄和 block。

## 关键入口

- 生成：[table/table_builder.cc:93-122](../../../source/leveldb/table/table_builder.cc#L93-L122)、[table/table_builder.cc:212-277](../../../source/leveldb/table/table_builder.cc#L212-L277)
- 打开：[table/table.cc:37-78](../../../source/leveldb/table/table.cc#L37-L78)
- 单键读取：[table/table.cc:213-241](../../../source/leveldb/table/table.cc#L213-L241)
- 迭代器：[table/table.cc:207-211](../../../source/leveldb/table/table.cc#L207-L211)
- 缓存入口：[db/table_cache.cc:40-111](../../../source/leveldb/db/table_cache.cc#L40-L111)

## 阅读顺序

1. `format.h/.cc`：BlockHandle、Footer、块 trailer 和校验。
2. `block_builder.cc` 与 `block.cc`：前缀压缩和 restart 搜索。
3. `table_builder.cc`：把各类 block 组装成文件。
4. `table.cc`、`two_level_iterator.cc`：按索引惰性读取。
5. `table_cache.cc`：文件和 Table 的缓存生命周期。

## 相关文档

- [implementation](implementation.md)
- [source-map](source-map.md)
- [interfaces](interfaces.md)
- [data-structures](data-structures.md)
- [call-chains](call-chains.md)
- [diagrams](diagrams.md)
- [line-level-analysis](line-level-analysis.md)
- [examples](examples.md)
- [testing](testing.md)
- [development-guide](development-guide.md)
- [risks-and-debt](risks-and-debt.md)

## 未解决问题

不同压缩算法、block size、filter policy 对实际读放大和缓存命中率的影响尚未在本机 benchmark 中验证。

## 下一步阅读建议

先用 `table/table_test.cc` 观察 Block/Table 构造夹具，再回到 `Version::Get` 理解表文件如何进入全局读路径。
