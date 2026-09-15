# M06 端到端示例：Env 文件、调度和资源生命周期

- 文档目的：把 DBImpl 的一次表文件读取和一次后台任务，落到 Env 的具体文件对象、锁、调度与释放动作。
- 适用范围：M06 与 M02/M04/M05 的交界；同时说明 MemEnv、EnvWrapper 和 Cache Handle 的测试用法。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：Env 接口、MemEnv 实现和 POSIX 文件实现已由源码确认；本页命令、跨平台构建和故障注入均未执行。
- 最后更新：2026-09-10
- 前置阅读：[M06 interfaces](interfaces.md)
- 后续阅读：[M06 testing](testing.md)

- 源码版本：`main` / `7ee830d02b623e8ffe0b95d59a74db1e58da04c5`。

## 结论摘要

M06 不是一个独立的“文件工具函数集合”，而是 DBImpl、VersionSet、TableCache 和测试替身共同依赖的运行时边界。上层只看到 `Env::NewRandomAccessFile`、`NewWritableFile`、`Schedule` 等虚函数；POSIX 下具体落地为 `open`/`pread`/`write`/`fdatasync`/`fsync`/`close`，MemEnv 下则落地为受 mutex 保护的 `FileState` block map。返回的文件对象和 Cache Handle 都是裸指针/句柄，调用者必须负责按契约释放。

## 场景 A：TableCache 读取一个 SSTable

假设 `TableCache::FindTable` 要打开 `demo/000007.ldb`，并把它交给 `Table::Open`。

### A.1 声明到具体实现

```text
TableCache::FindTable
  -> options_.env->NewRandomAccessFile("demo/000007.ldb", &file)
     -> Env 虚分派
        -> PosixEnv::NewRandomAccessFile（真实磁盘 Env）
           -> open(filename, O_RDONLY | O_CLOEXEC)
           -> PosixRandomAccessFile(fd, fd_limiter)
              -> fd_limiter.Acquire() 或关闭 fd，改为每次 Read 时 open
  -> Table::Open(options, file, file_size, &table)
     -> file->Read(offset, n, scratch)
        -> PosixRandomAccessFile::Read
           -> pread(fd, scratch, n, offset)
           -> Slice 指向 scratch
```

`Env` 的接口规定：随机访问文件创建失败时把 `*result` 置为 `nullptr` 并返回非 OK；返回对象可以被多个线程并发读。顺序文件和可写文件则由调用方按单线程契约使用。[`Env::NewRandomAccessFile`](../../../source/leveldb/include/leveldb/env.h#L76-L85)、[`RandomAccessFile::Read`](../../../source/leveldb/include/leveldb/env.h#L250-L270)、[`TableCache::FindTable`](../../../source/leveldb/db/table_cache.cc#L40-L75)

POSIX 实现的具体选择由资源限制改变：`PosixRandomAccessFile` 构造时尝试占用 fd limiter；若额度耗尽，关闭初始 fd，之后每一次 `Read` 临时 `open`，读完立即 `close`。因此“打开了 RandomAccessFile”不一定意味着对象整个生命周期都持有一个 fd。[`Limiter::Acquire/Release`](../../../source/leveldb/util/env_posix.cc#L68-L129)、[`PosixRandomAccessFile`](../../../source/leveldb/util/env_posix.cc#L171-L230)

### A.2 资源和 Slice 时间线

| 时刻 | 对象/资源 | 所有者 | 状态变化 |
|---|---|---|---|
| T0 | `RandomAccessFile* file` | `TableCache` 的打开路径暂时持有 | Env 成功返回具体 POSIX 对象 |
| T1 | fd 或 fd limiter 配额 | `PosixRandomAccessFile` | 常驻 fd，或降级为每次读临时 fd |
| T2 | `Slice result` | `Read` 调用者借用 | 可能指向调用者 `scratch`；不能脱离 scratch |
| T3 | `TableAndFile` | TableCache entry | 同时拥有 `file` 和 `Table` |
| T4 | Cache Handle | TableCache/cache | `FindTable` 返回外部引用 |
| T5 | Get 完成 | `TableCache::Get` | 先结束 `InternalGet`，再 `cache_->Release(handle)` |
| T6 | cache eviction 且无外部引用 | cache deleter | `delete table`、`delete file`、`delete TableAndFile` |

`TableCache` 的直接 Get 在 `InternalGet` 返回后释放 handle；Table iterator 则把释放注册到 iterator cleanup。这样 `Table` 使用 `RandomAccessFile` 时 entry 不会因中途 eviction 被回收。[`TableCache::Get/NewIterator`](../../../source/leveldb/db/table_cache.cc#L77-L111)、[`DeleteEntry`](../../../source/leveldb/db/table_cache.cc#L13-L29)

### A.3 失败路径

- `open` 失败：POSIX `PosixError` 将 `ENOENT` 转为 `NotFound`，其他 errno 转为 `IOError`；TableCache 不把失败结果插入 cache，因此之后仍可重试。[`PosixError`](../../../source/leveldb/util/env_posix.cc#L61-L67)、[`TableCache::FindTable`](../../../source/leveldb/db/table_cache.cc#L48-L74)
- `pread` 失败：返回非 OK，结果 Slice 为空；`Table::ReadBlock`/iterator status 向上报告，调用者仍需释放 TableCache handle。[`PosixRandomAccessFile::Read`](../../../source/leveldb/util/env_posix.cc#L199-L223)
- offset 超过 mmap 区域：`PosixMmapReadableFile::Read` 返回 `IOError`，不返回越界 Slice。[`PosixMmapReadableFile::Read`](../../../source/leveldb/util/env_posix.cc#L259-L267)
- fd/mmap 配额耗尽：不直接报错，进入降级实现；这可能增加每次读取的系统调用和延迟，但具体性能必须测量。

## 场景 B：写 MANIFEST 时的 Sync 顺序

### B.1 从 VersionEdit 到 POSIX 持久化调用

```text
VersionSet::LogAndApply
  -> descriptor_log_->AddRecord(edit.EncodeTo)
     -> WritableFile::Append
        -> PosixWritableFile::Append
           -> 先写 64 KiB 用户态 buffer
           -> buffer 满时 WriteUnbuffered
              -> write(fd, data, size)，EINTR 重试
  -> descriptor_file_->Sync
     -> PosixWritableFile::Sync
        -> MANIFEST：SyncDirIfManifest -> open(dirname) -> fdatasync/fsync(dir)
        -> FlushBuffer -> write(fd, ...)
        -> fdatasync/fsync(MANIFEST fd)
  -> 成功后 VersionSet::AppendVersion
```

POSIX `WritableFile::Sync` 明确先同步 MANIFEST 所在目录，再刷写文件缓冲区，最后同步 MANIFEST fd。这样源码层面建立了“新文件目录项/数据先于 manifest 引用同步”的顺序；断电后每个文件系统的实际可见窗口仍需实验，不能把调用顺序等同于所有介质上的绝对持久化保证。[`PosixWritableFile::Sync`](../../../source/leveldb/util/env_posix.cc#L322-L351)、[`PosixWritableFile::SyncDirIfManifest`](../../../source/leveldb/util/env_posix.cc#L375-L388)、[`VersionSet::LogAndApply`](../../../source/leveldb/db/version_set.cc#L777-L858)

`Append` 不保证每次调用都立即触发系统 `write`：小片段先积累在 65536 字节 buffer 中；`Close` 会 FlushBuffer 后关闭 fd，即使析构函数兜底调用 Close，也会忽略析构阶段潜在错误。因此需要在正常路径显式检查 `Sync`/`Close` 的 Status。[`PosixWritableFile::Append/Close`](../../../source/leveldb/util/env_posix.cc#L277-L330)

### B.2 失败和清理

| 失败点 | 已发生状态 | 上层处理重点 |
|---|---|---|
| Append/write | MANIFEST 可能只有部分物理内容 | `LogAndApply` 不安装新 Version，返回 Status |
| 目录 Sync | 不能确认新文件引用关系已持久化 | 不应把 VersionEdit 当作已提交 |
| MANIFEST fd Sync | record 不可确认 durable | 保留旧 current，记录后台错误/返回错误 |
| Close | fd 关闭失败或 Flush 失败 | 必须检查返回 Status；compaction 输出走清理路径 |
| 任意失败 | 临时输出可能存在 | `pending_outputs_`、CleanupCompaction 和 obsolete 策略保护文件 |

这些是跨模块的 Status 传播边界，不是 M06 自行回滚 Version。VersionSet/DBImpl 决定是否 AppendVersion、记录 `bg_error_` 和清理临时输出。[`VersionSet::LogAndApply`](../../../source/leveldb/db/version_set.cc#L792-L858)、[`DBImpl::CleanupCompaction`](../../../source/leveldb/db/db_impl.cc#L789-L804)

## 场景 C：Env::Schedule 的后台边界

```text
DBImpl::MaybeScheduleCompaction
  -> env_->Schedule(&DBImpl::BGWork, this)
     -> Env 实现把函数和 arg 排入后台执行机制
        -> 某个未指定线程调用 BGWork(arg)
           -> BackgroundCall
              -> 取得 DBImpl::mutex_
              -> 检查 shutting_down_ / bg_error_
              -> BackgroundCompaction 或退出
```

公共契约只保证任务被安排一次执行，不保证使用哪个线程；同一个 Env 上的多个函数可能在不同线程并发。因此 DBImpl 用 `background_compaction_scheduled_`、mutex 和条件变量维护自己的状态，不把 Env 当成串行队列。[`Env::Schedule`](../../../source/leveldb/include/leveldb/env.h#L191-L201)、[`DBImpl::MaybeScheduleCompaction/BackgroundCall`](../../../source/leveldb/db/db_impl.cc#L668-L706)

关闭时，DBImpl 设置 shutdown 标志并等待 scheduled 标志归零；后台任务看到标志后清除状态并广播。这里可以静态确认等待和通知配对，不能静态确认 POSIX 环境实际使用的线程数量或任务先后顺序。[`DBImpl::~DBImpl`](../../../source/leveldb/db/db_impl.cc#L152-L178)

## 场景 D：用 MemEnv 替换磁盘 Env

### D.1 创建和文件状态

```cpp
leveldb::Env* env = leveldb::NewMemEnv(leveldb::Env::Default());
leveldb::Options options;
options.env = env;
// 通过 DB::Open/测试 fixture 使用 options；这是概念示例，未运行。
```

`NewMemEnv` 创建 `InMemoryEnv`，它继承 `EnvWrapper`，用 `std::map<std::string, FileState*> file_map_` 表示文件系统。map 操作由 Env 自己的 mutex 保护；每个 `FileState` 又用独立的 `blocks_mutex_` 保护数据和 size，并用引用计数让打开的 Sequential/Random/Writable file 在 map 删除后仍可继续访问。[`NewMemEnv/InMemoryEnv`](../../../source/leveldb/helpers/memenv/memenv.cc#L221-L240)、[`FileState`](../../../source/leveldb/helpers/memenv/memenv.cc#L23-L150)

创建文件时，map 持有初始 FileState 引用；`NewWritableFile` 对已有文件执行 `Truncate`，对新文件建立 FileState 并插入 map，再创建 `WritableFileImpl` 增加引用。删除文件时先对 map 中对象 `Unref`，擦除映射；仍被文件对象引用时，数据不会立刻释放。[`InMemoryEnv::NewWritableFile/RemoveFile`](../../../source/leveldb/helpers/memenv/memenv.cc#L256-L329)

读取时 `FileState::Read` 在 blocks mutex 下把数据复制到调用者 scratch 并返回 `Slice(scratch, n)`；因此 MemEnv 与 POSIX 的 mmap 路径不同，返回 Slice 仍只是借用 scratch，不是永久字符串。[`FileState::Read`](../../../source/leveldb/helpers/memenv/memenv.cc#L71-L106)、[`RandomAccessFileImpl::Read`](../../../source/leveldb/helpers/memenv/memenv.cc#L185-L198)

### D.2 MemEnv 的边界和不应假定的语义

- `CreateDir`、`RemoveDir`、`Flush`、`Sync` 和 `Close` 是简化/空操作，不能用 MemEnv 证明 POSIX 的持久化语义。[`InMemoryEnv`](../../../source/leveldb/helpers/memenv/memenv.cc#L200-L214)、[`InMemoryEnv::CreateDir/RemoveDir`](../../../source/leveldb/helpers/memenv/memenv.cc#L331-L334)
- `LockFile` 直接分配一个 `FileLock`，不模拟跨进程排他性；不能用它证明两个进程同时打开同一 DB 的真实行为。[`InMemoryEnv::LockFile/UnlockFile`](../../../source/leveldb/helpers/memenv/memenv.cc#L358-L366)
- `NewMemEnv` 的 base Env 主要用于 EnvWrapper 转发未覆写的方法；测试替身的覆盖范围必须对照实际实现，不要默认它等价于 POSIX。[`EnvWrapper`](../../../source/leveldb/include/leveldb/env.h#L331-L402)

## 场景 E：Cache Handle 和 Arena 的联合生命周期

```text
TableCache::FindTable -> Cache::Insert/Lookup -> Handle
  -> Table::InternalGet / child iterator 借用 Value(Handle)
  -> direct Get: Release(Handle)
  -> iterator: RegisterCleanup(Release)
  -> no external refs: deleter(TableAndFile)

MemTable::Add -> Arena::Allocate -> entry pointer
  -> SkipList stores pointer
  -> MemTable Unref 到 0
  -> Arena 析构批量释放 block
```

Cache 的 `Erase` 只删除 key 到 entry 的映射；只要外部 Handle 仍存在，value/deleter 不应提前执行。Arena 的分配指针没有独立释放接口，必须跟随 MemTable owner。修改任一释放顺序都要同时检查 M02 Get/Iterator、M03 MemTable、M05 TableCache 和 M07 生命周期测试。[`Cache`](../../../source/leveldb/include/leveldb/cache.h#L33-L97)、[`Arena`](../../../source/leveldb/util/arena.h#L15-L66)、[`TableCache`](../../../source/leveldb/db/table_cache.cc#L13-L117)

## EnvWrapper 故障注入示例（静态设计）

继承 `EnvWrapper`，只覆写目标操作即可保留其他行为转发：

```cpp
class FailingEnv : public leveldb::EnvWrapper {
 public:
  explicit FailingEnv(leveldb::Env* target) : EnvWrapper(target) {}
  leveldb::Status NewWritableFile(const std::string& name,
                                  leveldb::WritableFile** result) override {
    *result = nullptr;
    return leveldb::Status::IOError(name, "injected failure");
  }
};
```

这是基于真实接口的测试设计示例，未加入源码、未编译、未运行。实际 fault injection 应使用仓库现有 fixture 和授权的测试环境，验证失败后 `DBImpl` 是否移除 pending output、是否保存 `bg_error_`、是否释放文件对象；不要为了演示而破坏工作目录。[`EnvWrapper`](../../../source/leveldb/include/leveldb/env.h#L331-L402)、[`DBImpl::RecordBackgroundError`](../../../source/leveldb/db/db_impl.cc#L660-L666)

## 观测和验证矩阵

| 目标 | 静态证据 | 推荐测试入口 | 本轮状态 |
|---|---|---|---|
| RandomAccessFile 并发读 | `RandomAccessFile::Read` 契约、POSIX `pread` | `util/env_test.cc`、`util/env_posix_test.cc` | 未执行 |
| MANIFEST Sync 顺序 | `PosixWritableFile::Sync` | `db/version_set_test.cc`、故障注入 fixture | 未执行 |
| Schedule/关闭 | `Env::Schedule`、`BackgroundCall` | `db/db_test.cc`、`db/fault_injection_test.cc` | 未执行 |
| MemEnv 文件引用 | `FileState::Ref/Unref`、RemoveFile | `helpers/memenv/memenv_test.cc` | 未执行 |
| Cache eviction | Cache handle/deleter | `util/cache_test.cc`、`db/table_cache_test.cc` | 未执行 |
| fd/mmap 降级 | `Limiter`、POSIX helper | `util/env_posix_test.cc` | 未执行 |

## 相关文档

- [M06 interfaces](interfaces.md)
- [M06 call-chains](call-chains.md)
- [M06 testing](testing.md)
- [M06 development guide](development-guide.md)
- [端到端深度链路](../../90-cross-module/end-to-end-traces.md)
- [跨模块错误边界](../../90-cross-module/error-boundaries.md)

## 源码证据摘要

- [`Env`/文件线程契约](../../../source/leveldb/include/leveldb/env.h#L50-L289)
- [`PosixRandomAccessFile`](../../../source/leveldb/util/env_posix.cc#L171-L275)
- [`PosixWritableFile`](../../../source/leveldb/util/env_posix.cc#L277-L418)
- [`MemEnv`/`FileState`](../../../source/leveldb/helpers/memenv/memenv.cc#L23-L150)
- [`EnvWrapper`](../../../source/leveldb/include/leveldb/env.h#L331-L402)

## 未解决问题

- POSIX、Windows、MemEnv 的真实构建组合尚未验证。
- Sync/Close 与断电后的实际持久化窗口需要授权的故障注入或文件系统实验。
- Env::Schedule 的实际线程数、公平性和交错顺序不能由公共接口单独确定。

## 下一步阅读建议

先读 [场景 A](#场景-a-tablecache-读取一个-sstable) 的 `FindTable → RandomAccessFile → Table::Open`，再读 [场景 B](#场景-b写-manifest-时的-sync-顺序) 的 `Append → Sync → LogAndApply`，最后对照 M06 测试文档设计一个不破坏工作目录的 EnvWrapper 回归测试。
