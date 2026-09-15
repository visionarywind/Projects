# M05 深度审计与实现追踪

- 文档目的：把 SSTable 从构造字节到 TableCache 命中、Block 读取和迭代器清理的实现链落到具体代码。
- 适用范围：`table/`、`db/table_cache.*` 与 M02/M04/M06 的调用边界。
- 源码版本：`main` / `7ee830d02b623e8ffe0b95d59a74db1e58da04c5`。
- 证据状态：主要函数、数据格式和句柄生命周期已静态确认；构造/损坏/压缩测试本轮未执行。
- 最后更新：2026-09-10
- 前置阅读：[M05 README](README.md)
- 后续阅读：[端到端单键读取](../../90-cross-module/end-to-end-traces.md#链-4单键读取从快照序号到-table-block)

## 结论摘要

M05 的真实工作不是“读一个表”这么简单，而是三个彼此嵌套的协议：

1. `TableBuilder` 将排序的 InternalKey/value 流编码为多个 block，并在文件尾部写入可定位的 footer；
2. `Table::Open` 先从 footer 得到 metaindex/index 的 `BlockHandle`，再把 index 和 filter 元数据载入 Table；
3. `Table::InternalGet` 用 index iterator 找到候选 data block，filter 只负责安全地跳过“不可能命中”的 block，最后 `ReadBlock` 校验/解压并由 data iterator 调用回调。

所有返回的 `Slice` 都是借用：可能指向读调用的 scratch、mmap 区域、block cache 或 iterator 临时存储。TableCache handle 必须覆盖整个 `Table::InternalGet`，Table iterator 则通过 cleanup 延长 entry 生命周期。

## 组件实现表

| 实现组件 | 声明 | 定义 | 创建者 | 主要调用者 | 核心状态 | 真实副作用 |
|---|---|---|---|---|---|---|
| `BlockBuilder` | `table/block_builder.h` | `table/block_builder.cc` | `TableBuilder` | `TableBuilder::Add/Flush` | entries、restarts、buffer | 生成前缀压缩 block bytes |
| `TableBuilder` | `table/table_builder.h` | `table/table_builder.cc` | `BuildTable/compaction` | `Add`, `Finish` | data/index/meta blocks、file offset | Append SSTable bytes |
| `Footer`/`BlockHandle` | `table/format.h` | `table/format.cc` | builder/reader | `ReadFooter`, `ReadBlock` | offset、size、magic | 校验文件格式和 block trailer |
| `Table` | `table/table.h` | `table/table.cc` | `Table::Open` | `InternalGet`, `NewIterator` | index、filter、options、file | 查询和创建二级 iterator |
| `TableCache` | `db/table_cache.h` | `db/table_cache.cc` | `DBImpl` | `Version::Get`, Version iterator | cache entry、handle | 管理 `TableAndFile` owner |

## 入口落地链

### 1. 构造链：InternalKey 到 SSTable bytes

```text
BuildTable / compaction iterator
  -> TableBuilder::Add(key, value)
     -> BlockBuilder::Add
        -> FindShortestSeparator / comparator（index key）
        -> Flush（达到 block_size）
           -> WriteBlock
              -> RawWriteBlock
                 -> WritableFile::Append(block contents + type/crc trailer)
  -> TableBuilder::Finish()
     -> filter block
     -> metaindex block
     -> index block
     -> footer.EncodeTo + file Append
```

`TableBuilder::Add` 只接受按 comparator 排序的输入；重复/逆序输入会破坏 builder 的前置条件。每次数据 block flush 后保存 pending handle，下一条 key 到来时才可生成对应 index entry，因为 index value 必须包含已写 block 的 offset/size。[`TableBuilder::Add/Flush`](../../../source/leveldb/table/table_builder.cc#L93-L185)、[`BlockBuilder::Add`](../../../source/leveldb/table/block_builder.cc#L39-L104)

`Finish` 先结束最后的数据 block，再写 filter/metaindex/index，最后编码 footer；因此 footer 是读者找到前面元数据的固定入口。Finish 后 builder 不应再 Add，调用者仍需处理文件 Sync/Close。[`TableBuilder::Finish`](../../../source/leveldb/table/table_builder.cc#L212-L277)、[`Footer::EncodeTo`](../../../source/leveldb/table/format.cc#L15-L89)

### 2. 打开链：footer 到 Table 对象

```text
TableCache::FindTable
  -> Env::NewRandomAccessFile
  -> Table::Open(options, file, file_size, &table)
     -> ReadFooter(file, file_size)
        -> Read(end - kEncodedLength, footer bytes)
        -> DecodeFrom + magic check
     -> ReadBlock(index handle)
        -> VerifyChecksum / DecompressBlock
     -> new Table(index, filter metadata, file)
```

`Table::Open` 的输出 `Table*` 只拥有/借用它被设计持有的 index/filter 资源，文件对象由 `TableCache::TableAndFile` 一起保存；打开失败必须 delete 已分配对象并返回 Status，TableCache 不缓存失败结果。[`Table::Open`](../../../source/leveldb/table/table.cc#L37-L78)、[`ReadFooter/ReadBlock`](../../../source/leveldb/table/format.cc#L90-L161)、[`TableCache::FindTable`](../../../source/leveldb/db/table_cache.cc#L40-L75)

### 3. 查询链：index → filter → data block → callback

```text
Version::Get
  -> TableCache::Get
     -> Table::InternalGet
        -> index_block->NewIterator
        -> index_iter->Seek(lookup key)
        -> filter->KeyMayMatch(user key, block offset)
        -> ReadBlock(data handle)
        -> data_iter->Seek(lookup key)
        -> handle_result(arg, data_iter->key(), data_iter->value())
     -> delete iterator / Release cache handle
```

index iterator 首先把查询映射到一个候选 data block；filter 返回 false 时可以避免读取数据块，但 filter 缺失、未知格式或读取异常不能制造 false negative，读取路径应继续按可能命中处理。data block iterator 的 `status()` 必须在清理前由上层检查。[`Table::InternalGet`](../../../source/leveldb/table/table.cc#L152-L240)、[`FilterBlockReader::KeyMayMatch`](../../../source/leveldb/table/filter_block.cc#L50-L103)、[`TableCache::Get`](../../../source/leveldb/db/table_cache.cc#L99-L111)

## 核心算法伪代码

### `TableBuilder::Add`

```text
assert key >= previous_key
if pending_index_entry:
    emit index(previous_key, pending_data_handle)
if data_block would exceed target and data_block has entries:
    Flush data block
add key/value to data_block
if filter: add key to filter builder
remember key as previous_key
```

实际代码还处理 `block_restart_interval`、分隔 key、压缩类型和 pending handle；伪代码只保留控制不变量。[`TableBuilder::Add`](../../../source/leveldb/table/table_builder.cc#L93-L122)

### `Table::InternalGet`

```text
index_iter.Seek(lookup_internal_key)
if !index_iter.Valid(): return
handle = decode(index_iter.value())
if filter exists and !filter.KeyMayMatch(handle.offset, user_key): return
block_iter = BlockReader(handle)
block_iter.Seek(lookup_internal_key)
if block_iter.Valid(): callback(user_key, block_iter.key(), block_iter.value())
return block_iter.status()
```

复杂度取决于 block 数、index iterator 和 block 内 restart 数；源码没有为当前配置给出固定性能数字。读取数据块的内存和缓存占用由 `ReadBlock`、block cache 和 iterator cleanup 共同决定。[`Table::InternalGet`](../../../source/leveldb/table/table.cc#L152-L240)、[`BlockIter`](../../../source/leveldb/table/block.cc#L24-L218)

## 状态和所有权表

| 对象 | 创建者 | 借用者 | 所有权/释放 |
|---|---|---|---|
| `TableBuilder` | flush/compaction | input iterator | `Finish`/`Abandon` 后由调用者 delete |
| `WritableFile` | Env | builder | `Sync`/`Close` 后 delete |
| data/index block bytes | builder/ReadBlock | writer/iterator | builder buffer 或 cache handle 管理 |
| `Table` | `Table::Open` | TableCache entry | cache deleter delete |
| `RandomAccessFile` | Env | Table | `TableAndFile` deleter delete |
| TableCache `Handle` | Cache | TableCache Get/iterator | Get 返回后或 iterator cleanup `Release` |
| `Slice key/value` | block/MemTable/scratch | callback/caller | 不复制、不拥有；按 owner 生命周期使用 |

## 正常与异常路径差异

| 阶段 | 正常 | 异常/提前退出 | 必须保持的动作 |
|---|---|---|---|
| Add | 按序写入 data block | comparator/Append 状态异常 | 不继续伪造 index；交给 builder Status |
| Finish | 写 meta/index/footer | builder/Append 失败 | `Abandon` 或 delete，避免使用不完整 footer |
| Open | footer/index 校验成功 | 短读、magic/CRC/压缩错误 | delete 临时 Table/block，并向 TableCache 返回错误 |
| InternalGet | callback 找到 value | filter/data block/iterator error | 返回 Status；释放 block iterator 与 cache handle |
| iterator | cache handle 注册 cleanup | 调用者提前 delete DB | 属于公共生命周期违规；先 delete iterator |

## 关键配置到行为映射

| 配置 | 影响位置 | 行为 |
|---|---|---|
| `block_size`/`block_restart_interval` | TableBuilder/BlockBuilder | 数据块大小和 restart 密度 |
| `compression` | `WriteBlock`/`ReadBlock` | block trailer 类型及解压分支 |
| `filter_policy` | builder/Table::Open/InternalGet | 是否生成和查询 filter block |
| `block_cache` | Table/BlockReader | data/index block 的缓存与 handle 生命周期 |
| `verify_checksums` | ReadBlock/compaction iterator | 是否校验 block checksum |
| `paranoid_checks` | 上层读取/compaction | 读取或后台错误的严格程度 |

## Demo 与测试映射

- 端到端读路径：[链 4](../../90-cross-module/end-to-end-traces.md#链-4单键读取从快照序号到-table-block) 的 `Version::Get → TableCache::Get → Table::InternalGet`。
- flush/compaction 输出：[链 6](../../90-cross-module/end-to-end-traces.md#链-6memtable-切换immutable-flush-与-level-0-安装) 和[链 7](../../90-cross-module/end-to-end-traces.md#链-7分层-compaction版本裁剪与输出安装)。
- 模块示例：[M05 examples](examples.md)；测试入口 `table/table_test.cc`、`table/filter_block_test.cc`、`db/table_cache_test.cc`。
- 尚无本轮已运行的 Demo；CRC、压缩和 eviction 观察项标记为未验证。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| TableBuilder/Block | 已完成 | 已完成 | 已完成 | 已完成 | 已完成 | 已完成 | 单线程 builder；已确认 | 已完成 | 链 6/7、M05 examples | 测试未运行 |
| Table::Open/InternalGet | 已完成 | 已完成 | 已完成 | 已完成 | 已完成 | 已完成 | 读调用线程；block cache 交界已说明 | 已完成 | 链 4/5、M05 examples | 具体 cache 命中率未测量 |
| TableCache | 已完成 | 已完成 | 已完成 | 已完成 | 已完成 | 已完成 | 多读线程契约来自 Env | 已完成 | 链 4/5 | eviction 回归未运行 |

## 相关文档

- [M05 README](README.md)
- [M05 call-chains](call-chains.md)
- [M05 data structures](data-structures.md)
- [M05 examples](examples.md)
- [M05 testing](testing.md)
- [M06 Env examples](../M06-env-platform/examples.md)
- [端到端深度链路](../../90-cross-module/end-to-end-traces.md)

## 源码证据摘要

- [`TableBuilder::Add/Finish`](../../../source/leveldb/table/table_builder.cc#L93-L277)
- [`BlockBuilder::Add`](../../../source/leveldb/table/block_builder.cc#L39-L104)
- [`Table::Open/InternalGet`](../../../source/leveldb/table/table.cc#L37-L240)
- [`ReadFooter/ReadBlock`](../../../source/leveldb/table/format.cc#L90-L161)
- [`TableCache::FindTable/Get`](../../../source/leveldb/db/table_cache.cc#L40-L111)

## 未解决问题

- 当前配置下 block cache 命中率、压缩收益和读放大未通过 benchmark 测量。
- 损坏 block、未知压缩类型、filter 读取失败和 iterator cleanup 需要执行测试进一步确认。

## 下一步阅读建议

先沿“构造链”阅读 `TableBuilder::Add/Finish`，再沿“查询链”阅读 `Table::InternalGet`，最后结合 TableCache 句柄和 M06 Env 的 `RandomAccessFile` 生命周期检查每个错误出口。
