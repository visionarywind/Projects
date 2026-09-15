# 跨模块调用链（展开版）

- 文档目的：把公共入口与内部函数之间的关键边界展开到锁、数据、所有权和错误出口，而不是只列符号箭头。
- 适用范围：M01–M07 的核心调用关系。
- 源码版本：`main` / `7ee830d`。
- 证据状态：静态调用边界和源码明确的锁/引用动作已确认；Env 的实际线程交错需实验。
- 最后更新：2026-09-10
- 前置阅读：[端到端深度链路](end-to-end-traces.md)
- 后续阅读：[共享数据与类型](shared-data-and-types.md)

完整叙述和最小场景见 [`end-to-end-traces.md`](end-to-end-traces.md)。本页按调用链快速定位“谁调用谁、在哪里解锁、什么对象必须活着”。

## 1. Open → Recover → Version/WAL

```text
DB::Open
  └─ new DBImpl
      └─ mutex_.Lock
          └─ Recover
              ├─ Env::CreateDir + LockFile
              ├─ NewDB（缺 CURRENT 且 create_if_missing）
              │   └─ VersionEdit -> log::Writer -> MANIFEST Sync/Close -> CURRENT
              ├─ VersionSet::Recover
              │   └─ CURRENT -> MANIFEST Reader -> VersionEdit -> Builder -> current Version
              └─ RecoverLogFile
                  └─ WAL Reader -> WriteBatchInternal::InsertInto -> MemTable
                      └─ WriteLevel0Table（超出内存阈值时）
```

`DB::Open` 成功才把 `DBImpl*` 写回调用方；失败路径 delete impl。Recover 期间 `mutex_` 持有，文件读取/写入虽是同步调用，版本恢复失败立即返回，不把半恢复状态暴露给 API。[`DB::Open`](../../source/leveldb/db/db_impl.cc#L1503-L1543)、[`DBImpl::Recover`](../../source/leveldb/db/db_impl.cc#L292-L383)、[`VersionSet::Recover`](../../source/leveldb/db/version_set.cc#L861-L991)

## 2. Put → WriteBatch → writer queue → WAL → MemTable

```text
DB::Put
  └─ local WriteBatch::Put
      └─ DBImpl::Write
          ├─ stack Writer 入 writers_
          ├─ 非队首：Writer::cv.Wait
          ├─ 队首：MakeRoomForWrite
          ├─ BuildBatchGroup（可能使用 tmp_batch_）
          ├─ 分配 sequence
          ├─ 解锁 mutex_
          │   ├─ log::Writer::AddRecord
          │   ├─ optional logfile_->Sync
          │   └─ WriteBatchInternal::InsertInto(mem_)
          └─ 加锁：更新 last sequence、完成并唤醒 writers_
```

WAL 和 MemTable 写入之间的顺序是源码明确的持久化边界。`WriteBatchInternal::InsertInto` 使用同一批次的 sequence/type 生成 InternalKey；`tmp_batch_` 只用于合并多个 writer，不能让首个调用者的 batch 被悄悄改写。[`DB::Put`](../../source/leveldb/db/db_impl.cc#L1487-L1493)、[`DBImpl::Write`](../../source/leveldb/db/db_impl.cc#L1206-L1276)、[`DBImpl::BuildBatchGroup`](../../source/leveldb/db/db_impl.cc#L1279-L1327)

## 3. Write → MakeRoom → immutable flush

```text
Write
  └─ MakeRoomForWrite
      ├─ bg_error_ != OK -> return saved Status
      ├─ L0 slowdown -> unlock + Sleep(1ms) + retry
      ├─ mem usage <= write_buffer_size -> proceed
      ├─ imm_ != nullptr -> CondVar.Wait
      ├─ L0 stop trigger -> CondVar.Wait
      └─ new WAL + old log close + imm_=old mem_ + new mem_ + Schedule
          └─ BGWork -> BackgroundCall -> CompactMemTable
              └─ WriteLevel0Table -> BuildTable
                  └─ VersionSet::LogAndApply
```

切换时 `has_imm_` 使用 release store，长时间 BuildTable 在锁外执行；旧 MemTable、新 MemTable 和 output file 分别由引用计数、DBImpl 字段和 `pending_outputs_` 保护。写线程等待的是状态条件，不是固定时间。[`DBImpl::MakeRoomForWrite`](../../source/leveldb/db/db_impl.cc#L1331-L1405)、[`DBImpl::CompactMemTable`](../../source/leveldb/db/db_impl.cc#L549-L580)

## 4. Get → LookupKey → MemTable → Version → TableCache

```text
DBImpl::Get
  ├─ snapshot/no snapshot -> SequenceNumber
  ├─ Ref mem_, imm_, current Version
  ├─ unlock mutex_
  │   ├─ MemTable::Get(mem_)
  │   ├─ MemTable::Get(imm_)
  │   └─ Version::Get
  │       └─ ForEachOverlapping
  │           └─ TableCache::Get
  │               └─ Table::InternalGet
  │                   └─ index/filter/data block
  ├─ lock mutex_
  ├─ UpdateStats -> MaybeScheduleCompaction（必要时）
  └─ Unref mem_/imm_/Version
```

引用先于解锁取得；因此读取期间 MemTable/Version 可在 DB 状态字段中切换，但本次读取持有的对象仍有效。Get 命中 MemTable 时不会打开 SSTable；只有前两层 miss 才进入 Version。[`DBImpl::Get`](../../source/leveldb/db/db_impl.cc#L1121-L1165)、[`Version::Get`](../../source/leveldb/db/version_set.cc#L324-L400)、[`TableCache::Get`](../../source/leveldb/db/table_cache.cc#L99-L111)

## 5. NewIterator → child iterators → merge → cleanup

```text
DBImpl::NewIterator
  └─ NewInternalIterator
      ├─ lock mutex_
      ├─ mem_->NewIterator + mem_->Ref
      ├─ imm_->NewIterator + imm_->Ref（若有）
      ├─ current->AddIterators
      ├─ NewMergingIterator(internal comparator)
      ├─ current->Ref
      ├─ RegisterCleanup(CleanupIteratorState)
      └─ unlock mutex_
          └─ NewDBIterator(user comparator/snapshot)
              └─ delete -> cleanup lock -> Unref all captured state
```

Table child iterator 的 cache handle 也在 iterator cleanup 中 Release。迭代器返回的 key/value 是 `Slice` 借用，不是自动拥有的 string；调用方必须遵守下一次移动和 owner 的生命周期。[`DBImpl::NewInternalIterator`](../../source/leveldb/db/db_impl.cc#L1083-L1107)、[`CleanupIteratorState`](../../source/leveldb/db/db_impl.cc#L1071-L1079)、[`TableCache::NewIterator`](../../source/leveldb/db/table_cache.cc#L77-L97)

## 6. Automatic compaction → input merge → output Version

```text
MaybeScheduleCompaction
  └─ Env::Schedule(BGWork)
      └─ BackgroundCall
          └─ BackgroundCompaction
              ├─ PickCompaction（score 优先 seek）
              ├─ trivial move -> RemoveFile + AddFile + LogAndApply
              └─ DoCompactionWork
                  ├─ smallest_snapshot
                  ├─ MakeInputIterator
                  ├─ unlock mutex_，merge/裁剪/BuildTable
                  ├─ FinishOutput：Finish -> Sync -> Close -> reopen verify
                  └─ lock mutex_ -> InstallCompactionResults -> LogAndApply
```

`LogAndApply` 在 MANIFEST record 和 Sync 期间也释放传入 mutex，成功后才 AppendVersion；因此“文件已写完”与“新文件对读取可见”是两个阶段。[`DBImpl::BackgroundCompaction`](../../source/leveldb/db/db_impl.cc#L708-L787)、[`DBImpl::DoCompactionWork`](../../source/leveldb/db/db_impl.cc#L898-L1057)、[`VersionSet::LogAndApply`](../../source/leveldb/db/version_set.cc#L777-L858)

## 7. Version::Get → TableCache → Table → Block

```text
Version::Get
  └─ ForEachOverlapping
      └─ TableCache::Get
          ├─ FindTable(file number)
          │   ├─ cache Lookup
          │   ├─ Env::NewRandomAccessFile
          │   ├─ Table::Open(footer/index/filter)
          │   └─ cache Insert(TableAndFile)
          └─ Table::InternalGet
              ├─ index Seek
              ├─ FilterBlockReader::KeyMayMatch
              ├─ ReadBlock / CRC / decompress
              └─ data iterator Seek + handle_result
```

TableCache cache miss 的 `TableAndFile` 同时拥有 RandomAccessFile 和 Table；cache handle 在直接 Get 返回后 Release，在 iterator 上注册 cleanup。打开失败不缓存，避免 transient error 永久污染。[`TableCache::FindTable`](../../source/leveldb/db/table_cache.cc#L40-L75)、[`Table::InternalGet`](../../source/leveldb/table/table.cc#L152-L240)

## 8. MANIFEST edit → durable metadata → current Version

```text
VersionSet::LogAndApply
  ├─ Builder(current + edit) -> new Version
  ├─ Finalize(compaction score)
  ├─ 首次：NewWritableFile + WriteSnapshot
  ├─ mutex unlock
  │   └─ descriptor_log_->AddRecord(edit) -> descriptor_file_->Sync
  │       └─ SetCurrentFile（首次新 manifest）
  ├─ mutex lock
  └─ success -> AppendVersion / log numbers update
```

失败时不安装 `v`；若刚创建新 MANIFEST，还关闭/删除 descriptor objects 和临时 manifest。新 SSTable 先通过 `pending_outputs_` 保持存活，只有 edit 提交后才进入 Version 的 live file 集合。[`VersionSet::LogAndApply`](../../source/leveldb/db/version_set.cc#L792-L858)、[`DBImpl::RemoveObsoleteFiles`](../../source/leveldb/db/db_impl.cc#L225-L290)

## 9. 错误和关闭出口

- Open：Recover/NewDB/初始 WAL/LogAndApply 任一失败，解锁后 delete impl，`*dbptr` 保持 null。[`DB::Open`](../../source/leveldb/db/db_impl.cc#L1503-L1543)
- Write：AddRecord 或 Sync 失败不插入 MemTable；Sync 错误额外写入 `bg_error_`，未来 writer 在 MakeRoom 起点失败。[`DBImpl::Write`](../../source/leveldb/db/db_impl.cc#L1235-L1257)、[`DBImpl::MakeRoomForWrite`](../../source/leveldb/db/db_impl.cc#L1331-L1341)
- Background：compaction 错误由 `RecordBackgroundError` 保留；CleanupCompaction 放弃 builder、删除 outfile、移除 pending output。[`DBImpl::CleanupCompaction`](../../source/leveldb/db/db_impl.cc#L789-L804)
- Shutdown：析构 release-store `shutting_down_` 并等待 scheduled 标志归零，再释放 DB 共享对象。[`DBImpl::~DBImpl`](../../source/leveldb/db/db_impl.cc#L152-L178)

## 跨链数据与锁摘要

| 边界 | 输入 | 输出 | 锁/线程 | 主要资源 |
|---|---|---|---|---|
| API → DBImpl | Slice/Options/WriteBatch | Status/DB/Iterator | 调用线程 | caller-owned bytes/batch |
| Write → WAL | WriteBatch contents | physical log records | queue head；IO 时解锁 | WritableFile/log writer |
| WAL → MemTable | encoded batch | InternalKey entries | 恢复或 writer owner | MemTable Ref/Arena |
| Get → Table | LookupKey | value/Status | Get IO 时解锁 | Version/Table cache handles |
| flush → Version | FileMetaData/VersionEdit | current Version | manifest IO 时解锁 | pending output/manifest |
| compaction → output | merged InternalKeys | SSTable files | work loop 解锁 | builder/outfile/input refs |

## 相关文档

- [端到端深度链路](end-to-end-traces.md)
- [共享数据与类型](shared-data-and-types.md)
- [错误边界](error-boundaries.md)
- [修改影响地图](change-impact-map.md)

## 源码证据摘要

见各节链接；推荐从 `DBImpl::Write`、`DBImpl::Get`、`DBImpl::DoCompactionWork` 三个锁边界开始。

## 未解决问题

静态源码不能确定 Env::Schedule 的实际调度交错、断电时 Sync 的持久化可见性和生产性能；需要实验。

## 下一步阅读建议

用本页定位函数后，回到端到端深度链路阅读对应正常/错误示例。
