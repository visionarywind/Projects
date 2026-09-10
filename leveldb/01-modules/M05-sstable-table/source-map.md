# M05 源码地图

- 文档目的：按文件和逻辑块定位 SSTable 机制。
- 适用范围：M05。
- 源码版本：`main` / `7ee830d`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M05 README](README.md)
- 后续阅读：[M05 line-level-analysis](line-level-analysis.md)

| 文件 | 逻辑块 | 职责 |
|---|---|---|
| `table/table_builder.cc` | 20-62 | builder 状态、两个 BlockBuilder、filter 和 pending index |
| `table/table_builder.cc` | 93-138 | 加 entry、按 block size flush、filter key |
| `table/table_builder.cc` | 140-207 | 压缩、block trailer、CRC、文件 offset |
| `table/table_builder.cc` | 212-277 | filter/metaindex/index/footer、Finish/Abandon |
| `table/block_builder.cc` | 39-68 | restart 数组和最终编码 |
| `table/block_builder.cc` | 70-104 | 前缀压缩 entry 编码 |
| `table/block.cc` | 24-45 | 解析 block 尾部、所有权释放 |
| `table/block.cc` | 54-73 | 防越界 entry 解码 |
| `table/block.cc` | 137-218 | Next/Prev/Seek |
| `table/format.cc` | 15-29 | BlockHandle 编解码 |
| `table/format.cc` | 31-66 | Footer magic 和句柄编解码 |
| `table/format.cc` | 68-161 | 读取 trailer、CRC、解压和所有权 |
| `table/filter_block.cc` | 20-74 | 生成过滤器和 offset 数组 |
| `table/filter_block.cc` | 76-103 | 解析并判断 KeyMayMatch |
| `table/table.cc` | 37-131 | Open、footer/index、meta/filter |
| `table/table.cc` | 152-204 | block cache 与 block iterator |
| `table/table.cc` | 207-259 | Table iterator 和 InternalGet |
| `table/two_level_iterator.cc` | 81-159 | index/data 两级迭代状态机 |
| `db/table_cache.cc` | 40-117 | TableAndFile 缓存、打开、释放、驱逐 |
| `table/merger.cc` | 13-178 | 多子迭代器归并 |

## 依赖方向

`TableBuilder → BlockBuilder/FilterBlockBuilder/Format → WritableFile`；读取方向为 `TableCache → Table → TwoLevelIterator → Block → RandomAccessFile`。缓存的实际实现来自 M06 的 `Cache`，但 cleanup 回调在 M05 注册。

## 测试定位

`table/table_test.cc` 同时覆盖 BlockBuilder/Block、TableBuilder/Table 和自定义 comparator；`table/filter_block_test.cc` 覆盖过滤器编码；`db/table_cache` 的行为通过 DB/table 集成测试间接验证。

## 相关源码证据

- [公开 Table 契约](../../../../include/leveldb/table.h#L22-L76)
- [表格式定义](../../../../table/format.h#L20-L89)
- [两级迭代器接口](../../../../table/two_level_iterator.h#L13-L26)
