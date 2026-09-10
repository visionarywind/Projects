# D01 调试走读

本页给出源码级断点和观察变量，不把调试器的某一次线程交错写成固定行为。建议在 Debug 构建中使用临时数据库和单个 gtest 过滤器。

## 1. 启动方式

```bash
./build/leveldb_tests --gtest_filter='DBTest.GetSnapshot'
# 或
lldb -- ./build/leveldb_tests --gtest_filter='DBTest.GetFromImmutableLayer'
# Linux gdb 等价：
gdb --args ./build/leveldb_tests --gtest_filter='DBTest.RecoverDuringMemtableCompaction'
```

如果测试二进制带有其他 GoogleTest 参数，先用 `--gtest_help` 和 `--gtest_list_tests` 确认。不要在生产目录上运行会调用 `DestroyDB` 的 fixture。

## 2. 断点组 A：首次打开

建议断点：

```text
leveldb::DBTest::TryReopen
leveldb::DB::Open
leveldb::DBImpl::Recover
leveldb::DBImpl::NewDB
leveldb::VersionSet::Recover
```

首次进入 `DB::Open` 时观察：

- `*dbptr == nullptr`；
- `impl->dbname_`；
- `impl->mutex_` 已 Lock；
- `db_lock_` 在 `Recover` 的 `LockFile` 后非空；
- 无 `CURRENT` 时进入 `NewDB`；
- 返回前 `mem_`、`logfile_`、`log_` 非空。

不要把 `new DBImpl` 当成 Open 完成点；`Recover`、初始 WAL 和必要的 VersionEdit 都成功后才把指针交给测试。[`DB::Open`](../../../../db/db_impl.cc#L1503-L1543)

## 3. 断点组 B：写入、WAL 和 MemTable

建议断点：

```text
leveldb::DBImpl::Write
leveldb::DBImpl::BuildBatchGroup
leveldb::DBImpl::MakeRoomForWrite
leveldb::log::Writer::AddRecord
leveldb::WriteBatchInternal::InsertInto
leveldb::MemTable::Add
```

单步时重点看：

1. `writers_.front()` 是否是当前栈上的 `Writer w`；
2. `WriteBatchInternal::Sequence/Count/Contents` 是否一致；
3. `mutex_` 在 `AddRecord` 前暂时释放，插入 MemTable 后重新取得；
4. `mem_->ApproximateMemoryUsage()` 和 `options_.write_buffer_size`；
5. `tmp_batch_` 是否因为 batch group 包含多个 writer 而使用；
6. `status` 失败时是否跳过 `InsertInto`。

可在 gdb 中观察 `versions_->LastSequence()`、`last_sequence` 和 `WriteBatchInternal::Count(write_batch)`，但不要直接改这些状态继续运行，因为这会破坏测试的不变量。

## 4. 断点组 C：Snapshot 和 Get

建议断点：

```text
leveldb::DBImpl::GetSnapshot
leveldb::DBImpl::Get
leveldb::MemTable::Get
leveldb::Version::Get
leveldb::TableCache::Get
leveldb::Table::InternalGet
```

`DBImpl::Get` 的锁边界是本场景关键：

```text
MutexLock l(&mutex_)
  -> Ref mem/imm/current
  -> mutex_.Unlock()
  -> LookupKey + MemTable/Version 读取
  -> mutex_.Lock()
  -> UpdateStats + Unref
```

在断点中分别打印 snapshot sequence、`mem`/`imm`/`current` 地址，以及 `LookupKey::internal_key()`。如果 mem 命中，`Version::Get` 不会被调用；先执行 `TEST_CompactMemTable` 再 Get 才能走表读取路径。[`DBImpl::Get`](../../../../db/db_impl.cc#L1121-L1166)

## 5. 断点组 D：写满切换和恢复

使用：

```bash
./build/leveldb_tests --gtest_filter='DBTest.GetFromImmutableLayer:DBTest.RecoverDuringMemtableCompaction'
```

建议断点：

```text
leveldb::DBImpl::MakeRoomForWrite
leveldb::DBImpl::MaybeScheduleCompaction
leveldb::DBImpl::BackgroundCall
leveldb::DBImpl::CompactMemTable
leveldb::DBImpl::WriteLevel0Table
leveldb::DBImpl::RecoverLogFile
```

观察状态序列：

```text
mem_ != nullptr, imm_ == nullptr
  -> 新 log number
  -> old mem_ 移入 imm_
  -> new mem_ 建立
  -> has_imm_ release-store
  -> Schedule
  -> BuildTable 在 mutex_ 外执行
  -> LogAndApply
  -> imm_ Unref
```

`GetFromImmutableLayer` 用 `delay_data_sync_` 阻塞输出文件 Sync；阻塞时可看到后台任务存在但新 Get 仍可以从 mem/imm 找到值。`Env::Schedule` 不承诺具体线程，断点时间顺序可能与一次运行不同。[`SpecialEnv::DataFile::Sync`](../../../../db/db_test.cc#L184-L191)

## 6. 断点组 E：MANIFEST 提交

建议断点：

```text
leveldb::VersionSet::LogAndApply
leveldb::log::Writer::AddRecord
leveldb::PosixWritableFile::Sync
leveldb::VersionSet::AppendVersion
```

依次观察：

- VersionEdit 的 added/deleted files；
- `descriptor_log_`/MANIFEST 写入；
- Sync 返回前 current Version 不变；
- Sync 成功后 `AppendVersion` 安装新 Version；
- `pending_outputs_` 在 BuildTable 到安装前保护新 file number。

如果使用 `ManifestWriteError`，可在 `SpecialEnv::ManifestFile::Append/Sync` 断点观察错误返回，再确认 `RecordBackgroundError` 和重开时旧 Version 的恢复。[`SpecialEnv::ManifestFile`](../../../../db/db_test.cc#L194-L218)、[`ManifestWriteError`](../../../../db/db_test.cc#L1849-L1887)

## 7. 断点组 F：Iterator cleanup

建议断点：

```text
leveldb::DBImpl::NewInternalIterator
leveldb::CleanupIteratorState
leveldb::Iterator::~Iterator
leveldb::DBImpl::~DBImpl
```

创建 Iterator 后记录 `mem`、`imm`、`Version` 的引用状态；写入/compaction 改变 DB 当前指针后，Iterator 仍使用捕获对象。删除 Iterator 时 cleanup 重新取得 DB mutex 并 Unref。调用方应先 `delete iter`，再 `delete db`，否则 cleanup 访问的 DB 状态可能已经销毁。[`DBImpl::NewInternalIterator/CleanupIteratorState`](../../../../db/db_impl.cc#L1059-L1107)、[`db.h` Iterator 规则](../../../../include/leveldb/db.h#L90-L96)

## 8. 调试器中容易误读的现象

| 现象 | 正确解释 |
|---|---|
| `TableBuilder` 已写出文件但 Get 看不到 | MANIFEST/AppendVersion 尚未提交，或读仍命中 MemTable/旧 Version |
| 后台断点没有立即命中 | `Schedule` 是异步入口，不保证立即执行或固定线程 |
| `Slice.data()` 指针变化 | Slice 借用 scratch/block/iterator 存储，下一次 iterator 修改可能失效 |
| `bg_error_` 是 OK 但本次 Put 失败 | 当前同步 IO Status 可能先返回；只有特定错误（如 Sync）才记录后台错误 |
| 重开后 log 内容与预期不同 | 需要区分 MANIFEST 注册状态、WAL 尾部、合法截断和文件系统持久化；不能从一次正常 Close 推导断电语义 |

## 9. 观察记录模板

```text
filter:
breakpoint:
thread id / call stack:
mutex held?:
sequence / file number:
mem / imm / current addresses:
Status:
external side effect:
cleanup observed:
what remains unverified:
```
