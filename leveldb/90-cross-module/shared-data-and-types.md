# 跨模块共享数据、类型与生命周期（展开版）

- 文档目的：解释同一份数据如何从 API 表示转换为日志、InternalKey、MemTable、SSTable 和 MANIFEST 元数据，并标出所有权边界。
- 适用范围：M01–M06。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：主要编码、字段和引用动作已由源码确认；跨平台实现差异和运行时悬空错误需实验/工具验证。
- 最后更新：2026-09-10
- 前置阅读：[展开版跨模块调用链](cross-module-call-chains.md)
- 后续阅读：[配置影响地图](configuration-impact-map.md)
## 结论摘要

本页聚焦 90-cross-module/shared-data-and-types.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 一张跨阶段数据图

```text
用户 Slice(key,value)
  └─ WriteBatch record(tag + length + bytes)
      └─ WAL physical record(header + fragments + CRC)
          └─ WriteBatchInternal::InsertInto
              └─ InternalKey(user key + (sequence,type) tag)
                  ├─ MemTable entry（Arena + SkipList）
                  └─ TableBuilder -> data/index/filter/footer block

VersionEdit（文件增删、log/sequence、compact pointer）
  └─ MANIFEST log record
      └─ VersionSet::Builder
          └─ Version（FileMetaData ranges + refs）
              └─ Version::Get / iterator -> TableCache -> Table/Block
```

## 1. `Slice`：借用视图，不是所有权

`Slice` 只保存指针和长度，常见来源是调用者 string、WriteBatch 编码缓冲区、MemTable Arena、block 缓冲区或 iterator 临时 key。它在 API 调用期间便于避免拷贝，但不能脱离来源对象长期保存。公共 API 的 Iterator、Comparator、Env 等接口都以 Slice 传递字节；调用方若需要长期保存必须复制到 `std::string`。[`include/leveldb/slice.h`](../../source/leveldb/include/leveldb/slice.h#L15-L72)、[`include/leveldb/iterator.h`](../../source/leveldb/include/leveldb/iterator.h#L20-L66)

贯穿链中的典型边界：`DBImpl::Get` 用用户 key 构造 `LookupKey`；`Version::Get` 的 `saver.user_key` 指向 LookupKey 的存储；Table data iterator 返回的 key/value 只在其 iterator/cache handle 生命周期内有效。任何把这些 Slice 放入异步任务的改动都必须复制数据或延长 owner。

## 2. `WriteBatch` bytes：API 原子批次到 WAL 的共同载荷

WriteBatch 的 bytes 同时是 API 的批次状态和 WAL 的 record payload。`DBImpl::Write` 在设置首个 sequence 后调用 `WriteBatchInternal::Contents(write_batch)` 传给 log writer；日志成功后对同一 batch 调 `InsertInto`，而不是重新解析用户参数。[`DBImpl::Write`](../../source/leveldb/db/db_impl.cc#L1221-L1247)、[`db/write_batch.cc`](../../source/leveldb/db/write_batch.cc#L4-L147)

这产生一个重要不变量：WAL 中成功持久化的 batch 与 MemTable 中应用的 batch 必须有相同的 count、sequence 和操作顺序。修改 WriteBatch 编码时必须同时更新 Writer/Reader、`InsertInto`、恢复测试和旧 bytes 兼容性测试；只改 WAL header 不会覆盖 WriteBatch payload 兼容性。

## 3. `SequenceNumber` 与 InternalKey：可见性契约

内部 tag 把 sequence 和 `ValueType` 压进 64 位尾部；InternalKey comparator 先按 user comparator，再按 tag 逆序处理 sequence。Write 队列为一个 batch 分配连续 sequence，Snapshot 保存某个 last sequence，LookupKey 用该 sequence 构造上限；MemTable、Version 和 compaction 都依赖同一规则。[`db/dbformat.h`](../../source/leveldb/db/dbformat.h#L50-L219)、[`DBImpl::Write`](../../source/leveldb/db/db_impl.cc#L1221-L1228)、[`DBImpl::Get`](../../source/leveldb/db/db_impl.cc#L1121-L1148)

| 阶段 | sequence 的意义 | 失败/生命周期注意 |
|---|---|---|
| Write | 为整个 batch 分配连续区间 | WAL/插入失败时不能把部分成功当作完整 batch |
| Snapshot | 固定读取上限 | Snapshot 必须 Release；不复制 value |
| LookupKey | 把 user key 转成内部查找 key | 内部缓冲区必须覆盖 MemTable lookup |
| MemTable | 同 user key 的版本排序 | entry 属于 Arena，不单独 delete |
| Compaction | 判断旧版本/删除标记能否丢弃 | 最老活动 Snapshot 决定保留下限 |
| Recover | 取 MANIFEST last sequence 与 WAL max 的最大值 | 防止重开时 sequence 倒退 |

## 4. MemTable entry、Arena 与 SkipList

MemTable 把 internal key 和 value 编码为 entry，使用 Arena 分配连续内存，再把 entry 指针放进 SkipList。MemTable 的 owner 是 DBImpl（`mem_`/`imm_`）或恢复临时变量；读取或 iterator 在释放 DB mutex 前先 Ref，结束后 Unref。Arena 析构时批量释放 block，entry 指针不能被单独 `delete`。[`db/memtable.cc`](../../source/leveldb/db/memtable.cc#L20-L135)、[`db/memtable.h`](../../source/leveldb/db/memtable.h#L19-L82)、[`util/arena.h`](../../source/leveldb/util/arena.h#L15-L66)

```text
DBImpl::mem_ Ref count
  ├─ DBImpl owner reference
  ├─ Get temporary reference（锁内取得，IO/lookup 锁外使用）
  ├─ Iterator cleanup reference
  └─ imm_ transition reference
        └─ Unref 到 0 -> Arena/SkipList/MemTable 析构
```

## 5. `VersionEdit`、`FileMetaData` 与 `Version`

`VersionEdit` 是增量：记录 added/deleted files、log number、prev log、next file、last sequence、comparator 和 compact pointer。`VersionSet::LogAndApply` 先用 Builder 把 edit 合并到 current Version，只有 MANIFEST record 写入并 Sync 成功后才 AppendVersion。`FileMetaData` 包含 file number、size、smallest/largest InternalKey 和 compaction seek 计数；新文件在 Builder 中建立引用，Version 链和 compaction 都可能继续持有它。[`db/version_edit.h`](../../source/leveldb/db/version_edit.h#L17-L101)、[`VersionSet::LogAndApply`](../../source/leveldb/db/version_set.cc#L777-L858)、[`VersionSet::Builder`](../../source/leveldb/db/version_set.cc#L566-L731)

| 对象 | 生产者 | 消费者 | owner/释放 |
|---|---|---|---|
| `VersionEdit` | flush/compaction/recovery | MANIFEST/Builder | 通常栈对象；编码后立即使用 |
| `FileMetaData` | VersionEdit/Builder | Version、Compaction | refs 归零时删除 |
| `Version` | VersionSet/Builder | Get、Iterator、Compaction | current 和版本链 + 临时 Ref |
| `pending_outputs_` number | DBImpl output 分配 | obsolete scan | 安装/失败清理时移除 |

`RemoveObsoleteFiles` 把 `pending_outputs_` 与所有 Version 的 live files 合并；后台错误时直接停止垃圾回收，因为无法确定 MANIFEST 是否已经提交。[`DBImpl::RemoveObsoleteFiles`](../../source/leveldb/db/db_impl.cc#L225-L290)

## 6. Cache Handle、TableAndFile 与 Block

TableCache 用 file number 的固定 64 位编码作为 cache key。cache entry 的 value 是 `TableAndFile`，同时拥有 `RandomAccessFile*` 和 `Table*`；cache deleter 删除二者。`FindTable` 返回外部 handle：直接 `Get` 在调用后 Release，iterator 将 Release 注册到 iterator cleanup。`Cache::Erase` 只删除映射，仍有外部 handle 时 value 不会立即被 deleter 释放。[`db/table_cache.cc`](../../source/leveldb/db/table_cache.cc#L13-L29)、[`db/table_cache.cc`](../../source/leveldb/db/table_cache.cc#L40-L117)、[`include/leveldb/cache.h`](../../source/leveldb/include/leveldb/cache.h#L33-L97)

Block iterator 还可能持有 block cache handle；因此返回的 Slice 不能越过 iterator/handle 规定的生命周期。读路径应先完成 `Table::InternalGet` 的 callback，再释放 TableCache handle；改变释放顺序可能导致 cache eviction 后访问悬空 block。

## 7. `Options` 和指针边界

`Options` 从调用方进入 `DBImpl`，`SanitizeOptions` 生成内部副本。comparator、Env、filter policy、info log、block cache 等多数是调用方或共享设施的指针；DBImpl 只在明确标记 owns 的情况下析构 info log/cache。改变 Options 默认值可能同时影响 WAL durability、MemTable flush、Table format、cache 和线程行为。[`DBImpl::DBImpl`](../../source/leveldb/db/db_impl.cc#L126-L150)、[`DBImpl::~DBImpl`](../../source/leveldb/db/db_impl.cc#L161-L178)、[`include/leveldb/options.h`](../../source/leveldb/include/leveldb/options.h#L32-L184)

## 8. `Status`：跨模块错误载荷

Status 不依赖异常跨越边界。Env 将系统错误映射为 Status，log/table/parser 添加 corruption 或 IO 状态，DBImpl 在 Open 路径立即返回，在后台路径把首个错误保存到 `bg_error_`，下一次写入从 `MakeRoomForWrite` 读出。[`DBImpl::RecordBackgroundError`](../../source/leveldb/db/db_impl.cc#L660-L666)、[`DBImpl::MakeRoomForWrite`](../../source/leveldb/db/db_impl.cc#L1331-L1341)

错误对象本身通常按值返回；但 `Status` 中的消息可能引用/复制字符串，不能把“函数返回 OK”误解为后台任务已经完成。后台调度必须通过状态字段、条件变量或测试 fixture 观察。

## 9. 从一个 key 看所有共享数据

```text
Put("k","v1")
  -> caller Slice
  -> WriteBatch bytes(tag=Put, key/value)
  -> WAL record(sequence=1)
  -> MemTable entry InternalKey("k",1,Value)
  -> flush Table data block + index/filter/footer
  -> FileMetaData(#N, smallest/largest)
  -> VersionEdit(AddFile)
  -> MANIFEST record
  -> Version current
  -> Get("k") LookupKey(sequence=latest)
  -> mem/imm miss
  -> Version overlap candidate
  -> TableCache handle
  -> Table index/filter/data block
  -> value copied to caller std::string
```

这里唯一明确拥有最终返回值的是 Get 调用者提供的 `std::string* value`；中间 key/value 大量是 Slice 视图。Snapshot `S1` 不保存一份 v1，而是通过 sequence 让读取沿同一 InternalKey 顺序选择旧版本。

## 相关文档

- [端到端深度链路](end-to-end-traces.md)
- [错误边界](error-boundaries.md)
- [修改影响地图](change-impact-map.md)
- [M03 数据结构](../01-modules/M03-wal-memtable/data-structures.md)
- [M04 数据结构](../01-modules/M04-version-compaction/data-structures.md)
- [M05 数据结构](../01-modules/M05-sstable-table/data-structures.md)

## 源码证据摘要

- [InternalKey/LookupKey](../../source/leveldb/db/dbformat.h#L50-L219)
- [WriteBatch 与 MemTable](../../source/leveldb/db/write_batch.cc#L4-L147)、[../../../db/memtable.cc#L20-L135)
- [VersionEdit/Builder](../../source/leveldb/db/version_edit.h#L17-L101)、[../../../db/version_set.cc#L566-L731)
- [TableCache ownership](../../source/leveldb/db/table_cache.cc#L13-L117)
- [Cache contract](../../source/leveldb/include/leveldb/cache.h#L33-L97)

## 未解决问题

- 不同 Env 上 Slice 返回缓冲区和 append/read 的精确稳定范围需分别查平台实现。
- 完整 C++ 静态分析、ASan/TSan 尚未运行；当前只根据源码描述 owner/ref 关系。

## 下一步阅读建议

先沿第 9 节的单 key 图逐项打开源码，再对照端到端链 3、4、6、7 的锁和错误分支。
