# D01 数据与状态轨迹

## 1. 对象、字节和文件的对应关系

| 时刻 | 用户层对象 | 内存表示 | 持久化/版本状态 | owner 与借用边界 |
|---|---|---|---|---|
| `Put(k,v)` 进入 | `Slice` key/value、临时 `WriteBatch` | `rep_ = sequence(8)+count(4)+tag+varstring...` | 尚未写 WAL | `Slice` 借用调用者字节；batch 自有 `std::string` |
| WAL 成功后 | 同一 WriteBatch payload | `log::Writer` 分片成 32 KiB block record | log 文件追加记录 | `WritableFile`/Writer 由 DBImpl 持有 |
| MemTable 插入后 | InternalKey | varint key length + user key + `(seq<<8)|type` + value | WAL 与 MemTable 均含 batch 语义 | entry 在 MemTable Arena 中，不能单独 delete |
| Snapshot 建立 | `Snapshot*` | 一个 sequence number | 无新文件 | DB 的 SnapshotList owner；调用方只持句柄 |
| flush 后 | immutable entries | `TableBuilder` block | L0 SSTable + MANIFEST VersionEdit | `pending_outputs_` 保护未安装文件 |
| Version 安装后 | current Version | FileMetaData ranges | MANIFEST 指向的 Version 可见 | Version 引用计数保护表元数据 |
| Iterator 创建后 | user iterator + child iterators | merging iterator / block cache | 读取某个稳定视图 | cleanup 释放 MemTable/Version/Cache Handle |
| reopen | 新 DBImpl | CURRENT/MANIFEST + 新 MemTable | 新进程状态由 WAL/manifest 恢复 | 旧 DB、Snapshot、Iterator 不跨 DB 实例 |

## 2. WriteBatch 到 InternalKey

`WriteBatch` 的内部编码由注释和实现共同定义：

```text
rep_
├── fixed64 sequence
├── fixed32 count
└── count records
    ├── kTypeValue + varstring key + varstring value
    └── kTypeDeletion + varstring key
```

`DBImpl::Write` 先给整个合并 batch 分配起始 sequence；`MemTableInserter` 每处理一个 record 就递增 sequence。`MemTable::Add` 再把 user key 和 `(sequence << 8) | ValueType` 编入 InternalKey。[`WriteBatch`](../../../source/leveldb/db/write_batch.cc#L5-L14)、[`DBImpl::Write`](../../../source/leveldb/db/db_impl.cc#L1221-L1228)、[`MemTable::Add`](../../../source/leveldb/db/memtable.cc#L76-L100)

这解释了以下观察：同一 key 多次更新不会覆盖掉旧 bytes，而是形成按 sequence 排序的多个 InternalKey；读路径选择对 snapshot 可见的第一条；compaction 在确定旧版本和 deletion 安全后才裁剪。

## 3. WAL 物理状态

`log::Writer::AddRecord` 将一个逻辑 WriteBatch 按 32 KiB block 和 7 字节 header 分片：

```text
kFullType                 一块内完整记录
kFirstType + kMiddleType* + kLastType   跨块记录
```

header 保存 masked CRC、payload 长度和 type；每个 fragment Append 后 Flush。Reader 遇到 FIRST/MIDDLE/LAST 才组合成一个逻辑 `Slice`，尾部未完成的记录按 writer 在中途退出处理并忽略；CRC 错误通过 Reporter 记录。[`Writer::AddRecord`](../../../source/leveldb/db/log_writer.cc#L34-L79)、[`Reader::ReadRecord`](../../../source/leveldb/db/log_reader.cc#L56-L173)

因此“大 value 跨物理块”不会改变 WriteBatch 语义，只增加 Reader 重组阶段。D01 未把某个固定 value 大小作为已运行的 WAL 碎片实验结果；源码能确认分片规则。

## 4. MemTable 与 Arena

MemTable 构造 `Arena` 和 `SkipList`，`Add` 从 Arena 一次分配完整 entry 并插入 SkipList。`Get` 用 `LookupKey` Seek 到不大于 snapshot sequence 的位置，再检查 user key 和 type；value 复制到调用者 `std::string`，deletion 返回 `NotFound`。[`MemTable` 构造/析构](../../../source/leveldb/db/memtable.cc#L21-L26)、[`MemTable::Get`](../../../source/leveldb/db/memtable.cc#L102-L136)

```text
DBImpl::mem_ owner Ref
  ├─ Get 临时 Ref -> 解锁读取 -> Unref
  ├─ Iterator 捕获 Ref -> cleanup 时 Unref
  └─ imm_ transition / flush Ref
        -> Version 安装后 Unref
              -> refs_ == 0 -> MemTable/Arena 销毁
```

Arena 没有单 entry delete；如果把 `MemTable::Iterator::key()` 返回的 Slice 保存到 MemTable owner 之外，会越过借用边界。

## 5. Snapshot 和可见性

Snapshot 不复制数据库：

```text
seq=1: foo -> v1
S1.sequence = 1
seq=2: foo -> v2
seq=3: foo deletion
```

- latest Get 使用 `LastSequence=3`，看到 deletion，返回 NotFound；
- `Get(ReadOptions{snapshot=S1})` 构造 LookupKey `(foo,1)`，看到 v1；
- flush/compaction 只改变物理文件层次，不改变 InternalKey 的 sequence 过滤；
- ReleaseSnapshot 只删除句柄，不能再使用该 Snapshot。

公共契约要求 Snapshot 属于创建它的 DB，且 Iterator 必须在 DB 删除前销毁。[`db.h:98-106`](../../../source/leveldb/include/leveldb/db.h#L98-L106)、[`options.h:150-165`](../../../source/leveldb/include/leveldb/options.h#L150-L165)

## 6. Iterator 的数据借用

`Iterator::key()` 和 `value()` 返回 Slice，公共契约只保证到 iterator 下一次修改；其底层可能分别来自：

- MemTable Arena；
- block cache 中的 block；
- RandomAccessFile read scratch；
- iterator 自己的临时编码空间。

因此 D01 中 `IterStatus` 立即调用 `ToString()` 把借用 Slice 复制成测试字符串，避免把 Slice 保存到下一次 `Next/Prev` 之后。[`DBTest::IterStatus`](../../../source/leveldb/db/db_test.cc#L521-L529)、[`Iterator` 契约](../../../source/leveldb/include/leveldb/iterator.h#L60-L73)

`IteratorPinsRef` 在后台写入和 compaction 后仍读到旧值，不是因为 Slice 永久有效，而是因为 Iterator 的 child resources 和 Version/MemTable 引用仍被 cleanup 保护。[`IteratorPinsRef`](../../../source/leveldb/db/db_test.cc#L1328-L1349)

## 7. SSTable、Version 和 MANIFEST

flush 的文件状态按以下顺序变化：

```text
pending_outputs_ += N
  -> BuildTable 追加 data/filter/meta/index/footer
  -> WritableFile Sync/Close
  -> VersionEdit.AddFile(level,N,...)
  -> MANIFEST AddRecord + Sync
  -> VersionSet::AppendVersion
  -> pending_outputs_ -= N / obsolete scan
```

在 `AppendVersion` 前，表文件已经存在但尚未对 current Version 可见。若 MANIFEST 记录或 Sync 失败，不能假定输出和输入的 GC 安全关系已经提交；测试 `ManifestWriteError` 通过重开验证数据不丢。[`WriteLevel0Table`](../../../source/leveldb/db/db_impl.cc#L505-L546)、[`ManifestWriteError`](../../../source/leveldb/db/db_test.cc#L1849-L1887)

## 8. 错误状态变化

| 注入点 | 当前调用结果 | 内存可见性 | 后续行为 | 恢复观察 |
|---|---|---|---|---|
| `DataFile::Sync` | sync Put 返回错误 | 当前 batch 不进 MemTable | `bg_error_` 使后续写失败 | 未确认该 record 是否持久存在，必须重开实测 |
| `ManifestFile::Append` | compaction/LogAndApply 失败 | 旧 Version 仍提供读 | 后台记录错误，保守不删文件 | 重开应读旧 Version 数据 |
| `ManifestFile::Sync` | 版本提交失败 | 新表未成为 current | 不可把文件删除当成安全 | `ManifestWriteError` 验证重开不丢数据 |
| 缺失 SSTable | Open 在 paranoid 下返回 Corruption | 不交出 DB 指针 | 调用方需处理失败 | `MissingSSTFile` 明确断言失败 |
| log Close | 切换 WAL 失败 | 旧/新状态受错误保护 | 后续 Put 失败 | `LogCloseError` 检查未来写入拒绝 |

错误类型通过 `Status` 表示；`Status` 的 error state 自有复制的 message storage，OK 由空 state 表示。[`Status`](../../../source/leveldb/include/leveldb/status.h#L24-L100)

## 9. 重新打开的持久化边界

`DBImpl::Recover` 从 CURRENT 找到 MANIFEST，`VersionSet::Recover` 先恢复 file number/log number/last sequence，再扫描目录中不低于 manifest log number 的 WAL。恢复得到的 `max_sequence` 与 MANIFEST 的 `LastSequence` 取最大值。[`Recover`](../../../source/leveldb/db/db_impl.cc#L292-L382)

测试 `RecoverWithLargeLog` 用较大 batch 写入，再以更小 `write_buffer_size` 重开，验证恢复过程可以在一个 WAL 内多次写出 L0 表，同时保留所有值。这证明的是源码定义的恢复路径，不是对任意进程崩溃时文件系统持久性的承诺。[`RecoverWithLargeLog`](../../../source/leveldb/db/db_test.cc#L1107-L1129)

## 相关页面

- [执行轨迹](execution-trace.md)
- [失败路径](failure-paths.md)
- [M03 数据结构](../../01-modules/M03-wal-memtable/data-structures.md)
- [共享数据与类型](../../90-cross-module/shared-data-and-types.md)
- [M05 实现](../../01-modules/M05-sstable-table/implementation.md)
