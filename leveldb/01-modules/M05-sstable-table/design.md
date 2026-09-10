# M05 设计：不可变表与分层读取

- 文档目的：说明 SSTable 的格式和读取设计取舍。
- 适用范围：`table_builder.cc`、`table.cc`、`format.*`、`block*`、`filter_block*`。
- 源码版本：`main` / `7ee830d`。
- 证据状态：格式和控制流已确认；历史动机部分为推断。
- 最后更新：2026-09-10
- 前置阅读：[M04 design](../M04-version-compaction/design.md)
- 后续阅读：[M05 call-chains](call-chains.md)

## 核心设计

### 1. 文件不可变，读取可并发

公共 `Table` 注释明确表是 immutable/persistent，允许多线程无外部同步访问；`Table::Open` 不接管 `RandomAccessFile`，而是要求文件在 Table 生命周期内保持有效。[include/leveldb/table.h:22-40](../../../../include/leveldb/table.h#L22-L40) 这使 compaction 可以先生成新文件，再通过 VersionEdit 原子地改变可见文件集合。

### 2. 多级索引降低随机读取成本

data block 保存实际键值，index block 保存“索引键 → BlockHandle”，footer 保存 metaindex/index 的位置。[table/table_builder.cc:242-265](../../../../table/table_builder.cc#L242-L265) 读取只需先读固定长度 footer 和 index，再按需加载目标 data block，而不是扫描整个文件。[table/table.cc:44-74](../../../../table/table.cc#L44-L74)

### 3. 块内前缀压缩与 restart

BlockBuilder 对相邻有序 key 保存共享前缀，按 `block_restart_interval` 插入完整 key restart point；读取器在 restart 数组上二分、在区间内线性扫描。[table/block_builder.cc:4-26](../../../../table/block_builder.cc#L4-L26)、[table/block.cc:163-218](../../../../table/block.cc#L163-L218) 这在压缩空间和可定位性之间折中。

### 4. 可选过滤器和缓存

filter block 按约 2KB 数据范围生成过滤器；命中“不可能存在”时跳过 data block，格式异常则按“可能命中”处理，避免假阴性。[table/filter_block.cc:13-25](../../../../table/filter_block.cc#L13-L25)、[table/filter_block.cc:89-103](../../../../table/filter_block.cc#L89-L103) data block 可由 block cache 复用，缓存句柄由 iterator cleanup 释放。[table/table.cc:150-204](../../../../table/table.cc#L150-L204)

## 明确边界

- M05 不决定哪些文件属于当前 Version；那是 M04。
- M05 不负责 WAL 或 sequence 可见性；那是 M03/M04。
- M05 依赖 `Env` 提供的 RandomAccessFile/WritableFile，但不实现操作系统文件语义。
- `TableBuilder::Add` 要求 comparator 顺序严格递增；排序责任在调用者。[table/table_builder.cc:93-116](../../../../table/table_builder.cc#L93-L116)

## 取舍与未知

- index key 使用 shortest separator/successor，减少索引空间；这是源码明确行为，节省量需 benchmark 确认。[table/table_builder.cc:101-107](../../../../table/table_builder.cc#L101-L107)、[table/table_builder.cc:242-249](../../../../table/table_builder.cc#L242-L249)
- Snappy/Zstd 只有在压缩后至少节省约八分之一时才采用，否则写原文；阈值的历史原因未从源码证明。[table/table_builder.cc:149-185](../../../../table/table_builder.cc#L149-L185)
