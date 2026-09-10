# D01 执行轨迹：从 fixture 到重开

本页不是把测试名重新列一遍，而是说明每个测试动作如何进入生产代码、改变什么状态，以及下一步为何能观察到该结果。

## 阶段 1：fixture 创建与首次 Open

### 测试代码

`DBTest` 构造函数建立 `SpecialEnv`，把数据库名设为 GoogleTest 临时目录下的 `db_test`，先调用 `DestroyDB`，再调用 `Reopen`。`TryReopen` 复制当前 Options，打开 `create_if_missing=true`，最后调用公共 `DB::Open` 并把结果存入 `db_`。[`DBTest` 构造/`TryReopen`](../../../../db/db_test.cc#L260-L345)

### 生产路径

```text
DBTest::Reopen
  -> DBTest::TryReopen
     -> DB::Open
        -> new DBImpl
        -> impl->mutex_.Lock()
        -> DBImpl::Recover
           -> CreateDir / LockFile
           -> NewDB（若无 CURRENT）
           -> VersionSet::Recover
        -> NewWritableFile(log)
        -> new log::Writer
        -> new MemTable + Ref
        -> 可选 VersionSet::LogAndApply
        -> RemoveObsoleteFiles / MaybeScheduleCompaction
```

`DB::Open` 在失败前把 `*dbptr` 置空；成功才交出 `DBImpl*`，失败则删除半初始化对象。[`DB::Open`](../../../../db/db_impl.cc#L1503-L1543)

### 锁和资源

- `DBImpl::mutex_` 在 `Recover`、初始日志和版本安装阶段保护数据库状态。
- `LOCK` 的 `FileLock*` 由 `DBImpl` 持有，析构时释放。
- `mem_` 建立后立即 `Ref`；新 WAL 的 `WritableFile*` 和 `log::Writer*` 归 `DBImpl`。
- `Options::env` 指向 `SpecialEnv`，文件系统调用通过 `EnvWrapper` 转发，不绕过 Env。

## 阶段 2：写入和删除

### 测试代码

`PutDeleteGet` 依次写 `foo=v1`、`foo=v2`、删除 `foo`，每次调用 helper `Get` 把 `NotFound` 转成测试字符串 `NOT_FOUND`。[`PutDeleteGet`](../../../../db/db_test.cc#L609-L618)、[`DBTest::Put/Get`](../../../../db/db_test.cc#L348-L365)

### 生产路径

```text
DBTest::Put / DB::Put
  -> DBImpl::Put / DB::Put convenience wrapper
     -> 临时 WriteBatch::Put
     -> DBImpl::Write
        -> Writer 入 writers_
        -> MakeRoomForWrite
        -> BuildBatchGroup
        -> SetSequence
        -> log::Writer::AddRecord
        -> WriteBatchInternal::InsertInto
           -> MemTableInserter::Put/Delete
              -> MemTable::Add
                 -> Arena::Allocate + SkipList::Insert
```

`WriteBatch` 的前 12 字节是 sequence/count，记录带 value/deletion tag；同一批 bytes 先进入 WAL，成功后由 `InsertInto` 解释为 MemTable 条目。[`WriteBatch` 布局和插入](../../../../db/write_batch.cc#L5-L14)、[`InsertInto`](../../../../db/write_batch.cc#L115-L137)

### 重要分支

- WAL `AddRecord` 失败：当前 batch 不进入 MemTable，`Status` 返回给测试。
- `WriteOptions::sync=true`：在 MemTable 插入前调用 `WritableFile::Sync`；失败时记录 `bg_error_`，后续写会失败。[`DBImpl::Write`](../../../../db/db_impl.cc#L1230-L1257)
- MemTable 接近阈值：`MakeRoomForWrite` 可能等待 `imm_`、等待 L0，或切换 WAL/MemTable 后调度后台 flush。[`MakeRoomForWrite`](../../../../db/db_impl.cc#L1331-L1405)

## 阶段 3：先从内存读，再进入版本

`Get` 测试 helper 先建立 `ReadOptions`，可选携带 Snapshot，然后调用 `db_->Get`。DBImpl 在锁下读取 snapshot sequence，Ref `mem_`/`imm_`/current Version，解锁后按 mem → imm → Version 顺序访问，最后重新加锁更新 seek 统计并 Unref。[`DBImpl::Get`](../../../../db/db_impl.cc#L1121-L1166)

当数据还在 MemTable，测试无需等待 SSTable；当 `TEST_CompactMemTable` 已完成，Version/TableCache/Table/Block 路径仍返回相同 user value。`GetFromVersions` 正是用一次强制 flush 验证这两个读层次。[`GetFromVersions`](../../../../db/db_test.cc#L640-L646)

## 阶段 4：Snapshot 固定可见序列

`GetSnapshot` 在 mutex 下调用 `snapshots_.New(versions_->LastSequence())`，只保存 sequence，不复制每个 key/value。[`DBImpl::GetSnapshot`](../../../../db/db_impl.cc#L1187-L1195)

`GetSnapshot` 测试的真实步骤：

```text
Put(key, v1)       -> seq 1
GetSnapshot()      -> S1.sequence = 1
Put(key, v2)       -> seq 2
Get(key)           -> v2
Get(key, S1)       -> LookupKey(key, 1) -> v1
TEST_CompactMemTable()
Get(key, S1)       -> 仍为 v1
ReleaseSnapshot(S1)
```

测试同时使用短 key 和 200 字节 key，验证的是 sequence/编码路径而不是某个固定 key 长度。[`GetSnapshot`](../../../../db/db_test.cc#L659-L675)

`Snapshot` 测试创建 S1/S2/S3 后继续写 v4，并分别断言 v1/v2/v3/v4；释放顺序说明 Snapshot 句柄独立于 DB 当前值，但必须由同一个 DB 显式释放。[`Snapshot`](../../../../db/db_test.cc#L1351-L1377)

## 阶段 5：Iterator 和引用生命周期

`IterMulti` 先写 a/b/c，创建 Iterator，执行 Seek、Next、Prev；随后在 Iterator 已存在时写入新版本和删除 b，再次遍历仍看到创建时的 a/b/c 视图。这验证 `NewInternalIterator` 对 mem/imm/current Version 的引用保护以及 DB iterator 的 snapshot sequence。[`IterMulti`](../../../../db/db_test.cc#L859-L940)

`IteratorPinsRef` 更明确地在创建 iterator 后写入大量 100 KiB value，迫使后台 compaction 工作，再检查 iterator 仍返回旧 `foo=hello`。Iterator cleanup 最终释放捕获的 MemTable/Version 引用；调用方必须先删 Iterator，再删 DB。[`IteratorPinsRef`](../../../../db/db_test.cc#L1328-L1349)、[`NewInternalIterator/CleanupIteratorState`](../../../../db/db_impl.cc#L1059-L1107)

## 阶段 6：写满、immutable flush 与 Level-0

`GetFromImmutableLayer` 使用 `write_buffer_size=100000`，让两个大 value 填满 MemTable；通过 `SpecialEnv::delay_data_sync_` 阻塞数据文件 Sync，在后台 flush 未完成时仍读取 `foo=v1`。[`GetFromImmutableLayer`](../../../../db/db_test.cc#L620-L638)

状态转换：

```text
mem_ = M1, WAL = L1
  -> MakeRoomForWrite
  -> 新建 L2 和 M2
  -> imm_ = M1, mem_ = M2, has_imm_ = true
  -> Env::Schedule(BGWork)
  -> CompactMemTable
  -> BuildTable(M1) -> L0 table file
  -> VersionEdit -> MANIFEST AddRecord + Sync
  -> AppendVersion
  -> imm_->Unref(), imm_ = nullptr
```

`WriteLevel0Table` 把输出 file number 放入 `pending_outputs_`，锁外构建表，重新加锁后才从 pending 集合移除并把 metadata 加入 VersionEdit；因此尚未安装的文件不会被 obsolete GC 误删。[`WriteLevel0Table`](../../../../db/db_impl.cc#L505-L546)

## 阶段 7：关闭、重开与 WAL 恢复

`Recover` 测试写入 foo/baz，`Reopen`，再写 bar/foo，第二次 `Reopen` 后验证最新值。fixture 的 `Reopen` 先 `delete db_`，这会等待后台工作、释放 LOCK，然后再次调用 `DB::Open`。[`Recover`](../../../../db/db_test.cc#L1017-L1037)、[`DBTest::Reopen`](../../../../db/db_test.cc#L317-L346)

重开控制流：

```text
DBImpl::~DBImpl
  -> shutting_down_ = true
  -> 等 background_compaction_scheduled_ 归零
  -> 解锁 LOCK、释放 Version/mem/imm/log/TableCache
DB::Open -> Recover
  -> CURRENT -> MANIFEST -> VersionSet::Recover
  -> GetChildren + AddLiveFiles
  -> 按 file number 筛选新 WAL
  -> log::Reader::ReadRecord
  -> WriteBatchInternal::InsertInto 临时 MemTable
  -> 必要时 WriteLevel0Table
  -> 更新 LastSequence
```

若最后一个 WAL 可以复用且没有中途 compaction，`reuse_logs` 分支可重新打开 appendable file；否则临时 MemTable 被写为 L0 表。[`RecoverLogFile`](../../../../db/db_impl.cc#L385-L503)

`RecoverDuringMemtableCompaction` 故意在长 flush 期间写入新 WAL，然后 Reopen，验证 foo、bar 和两个大 value 都恢复。这不是对断电的完整模拟，而是对“旧 imm 正在落盘、新 WAL 已接收写入”状态边界的测试。[`RecoverDuringMemtableCompaction`](../../../../db/db_test.cc#L1051-L1074)

## 阶段 8：错误注入和可见结果

`SpecialEnv` 将 `.log`/`.ldb` 包装成 `DataFile`，将 MANIFEST 包装成 `ManifestFile`；原子开关控制 Append、Sync、Close 和新文件创建的错误。[`SpecialEnv::NewWritableFile`](../../../../db/db_test.cc#L155-L232)

- `WriteSyncError`：非 sync 写入可进入内存；sync 写 `k2` 失败且不出现在 MemTable；之后即使关闭故障开关，`bg_error_` 仍使 `k3` 写入失败。[`WriteSyncError`](../../../../db/db_test.cc#L1818-L1847)
- `ManifestWriteError`：生成表后 MANIFEST 写或 Sync 失败，重开仍能读 `foo=bar`；测试防止 GC 删除仍被旧 Version 需要的文件。[`ManifestWriteError`](../../../../db/db_test.cc#L1849-L1887)
- `MissingSSTFile`：关闭后删除表文件，以 paranoid checks 重开应失败；`StillReadSST` 把 `.ldb` 改名为旧 `.sst` 后验证兼容读取。[`MissingSSTFile/StillReadSST`](../../../../db/db_test.cc#L1889-L1920)

## 轨迹总结

D01 的核心不变量不是“每次 Put 立刻生成 SSTable”，而是：

1. 公共 API 只交出 Status/DB/Iterator/Snapshot 契约；
2. WAL payload 与 MemTable 应用使用同一 WriteBatch 表示；
3. 读路径先固定 sequence 并保护对象，再在锁外读取；
4. SSTable 只有在 VersionEdit 经 MANIFEST 提交后成为 current Version 的可见文件；
5. 后台/Env 错误通过 Status 或 `bg_error_` 改变后续行为；
6. 所有裸指针、Cache Handle、Slice 和 Snapshot 都有明确的拥有者和释放边界。

## 证据边界

上述控制流和状态来自源码与测试断言。测试中用到的等待、随机值和 `SpecialEnv` 是验证工具，不应被解释为生产环境固定线程数、固定 compaction 延迟或断电持久性证明。

## 相关源码

- [`DB::Open`](../../../../db/db_impl.cc#L1503-L1543)
- [`DBImpl::Write/Get`](../../../../db/db_impl.cc#L1121-L1277)
- [`DBImpl::Recover/RecoverLogFile`](../../../../db/db_impl.cc#L292-L503)
- [`DBTest` fixture helpers](../../../../db/db_test.cc#L260-L492)
- [`WriteBatch` 编码](../../../../db/write_batch.cc#L5-L148)
