# M06 深度审计与实现追踪

- 文档目的：解释 Env 抽象如何在 POSIX 和 MemEnv 中落地，并追踪文件、调度、Cache/Arena 的真实资源边界。
- 适用范围：`include/leveldb/env.h`、`util/env_posix.cc`、`helpers/memenv/memenv.cc`、`util/cache*`、`util/arena*`。
- 源码版本：`main` / `7ee830d02b623e8ffe0b95d59a74db1e58da04c5`。
- 证据状态：核心接口和主要实现已静态确认；本轮未执行跨平台构建、故障注入和并发测试。
- 最后更新：2026-09-10
- 前置阅读：[M06 README](README.md)
- 后续阅读：[M06 examples](examples.md)

## 结论摘要

M06 通过虚接口把操作系统效果隔离出来，但不替上层承担所有权：`Env` 创建的文件对象通常以裸指针返回，调用者负责 `delete`；`RandomAccessFile` 允许并发读，`SequentialFile`/`WritableFile` 要求外部同步；`Schedule` 只承诺安排执行，不承诺线程或串行顺序。[`Env`](../../../../include/leveldb/env.h#L50-L217)

POSIX 实现中，资源限制会改变真实落地路径：常驻 fd 不足时随机读对象改成每次 `open`/`pread`/`close`；mmap 读取则返回指向映射区的借用 Slice，析构时 `munmap`。可写文件先将小 Append 放进 64 KiB buffer，`Sync` 对 MANIFEST 先同步目录、再刷新文件 buffer、最后同步文件 fd。[`PosixRandomAccessFile`](../../../../util/env_posix.cc#L171-L230)、[`PosixMmapReadableFile`](../../../../util/env_posix.cc#L238-L275)、[`PosixWritableFile`](../../../../util/env_posix.cc#L277-L418)

## 实现组件卡片

### `Env` / `EnvWrapper`

```text
组件：操作系统能力抽象
声明：include/leveldb/env.h:50-217
具体实现：PosixEnv、Windows Env、InMemoryEnv
创建与销毁：Default() 返回 LevelDB 所有且不可 delete；自定义 Env 由调用者管理
入口方法：New*File、LockFile、Schedule、StartThread
核心不变量：返回对象失败时为 nullptr；文件访问线程契约必须遵守
主要调用者：DBImpl、VersionSet、TableCache、测试 fixture
资源所有权：文件对象交给调用者；Schedule 的 arg 生命周期由调用者保证
真实副作用：文件/目录/锁/线程/时钟/日志
失败方式：Status；Schedule 本身为 void，异步任务错误由 DBImpl 保存
对应测试：util/env_test.cc、helpers/memenv/memenv_test.cc
```

`EnvWrapper` 的每个方法默认转发到 `target_`，这样测试或调用者只覆写一个操作即可注入行为。新增虚方法时必须同步更新具体 Env、Wrapper 和测试替身，否则会产生 ABI/行为不一致。[`EnvWrapper`](../../../../include/leveldb/env.h#L331-L402)

### `PosixRandomAccessFile`

构造函数拿走 fd 的所有权，并通过 `Limiter::Acquire` 决定是否保留 fd；析构关闭常驻 fd 并释放 limiter 配额。`Read` 使用 `pread`，不改变共享文件偏移，符合多线程并发读契约；降级模式每次读独立打开和关闭 fd。[`PosixRandomAccessFile`](../../../../util/env_posix.cc#L171-L230)

### `PosixWritableFile`

`Append` 维护用户态 buffer；buffer 满或数据很大时进入 `WriteUnbuffered`，对 EINTR 循环重试。`Close` 先刷 buffer 后 close，并将 fd 置为 -1；析构只对仍打开的 fd 做 best-effort Close。`Sync` 是显式 durable 边界，错误必须由调用者接收。[`PosixWritableFile::Append/Close/Sync`](../../../../util/env_posix.cc#L293-L351)

### `FileState` / `InMemoryEnv`

```text
组件：内存文件系统
声明/定义：helpers/memenv/memenv.cc:23-150, 221-384
创建者：NewMemEnv -> InMemoryEnv；NewWritableFile -> FileState
核心字段：file_map_、blocks_、size_、refs_
并发：file_map_ 受 InMemoryEnv::mutex_ 保护；blocks_/size_ 受 blocks_mutex_ 保护；refs_ 受 refs_mutex_ 保护
资源所有权：map 持一份 FileState 引用；每个打开文件对象再持一份
真实副作用：map/block 内存变化，不代表磁盘持久化
释放：map 删除或 InMemoryEnv 析构时 Unref；最后引用释放 FileState
```

`FileState::Read` 在锁下将 block 内容复制到调用者 scratch，并返回 `Slice(scratch, n)`；FileState 可在 map 删除后因打开文件仍存活，模拟“删除名称不立即销毁打开对象”的部分语义。[`FileState`](../../../../helpers/memenv/memenv.cc#L23-L150)、[`InMemoryEnv::RemoveFile`](../../../../helpers/memenv/memenv.cc#L311-L329)

## 公开入口到副作用的三条链

### 随机读

```text
TableCache::FindTable
  -> Env::NewRandomAccessFile
     -> PosixEnv::NewRandomAccessFile
        -> open / mmap / PosixRandomAccessFile
           -> RandomAccessFile::Read
              -> pread 或 mmap copy
                 -> Slice + Status
```

M06 的最后一个有实际副作用的节点是 `open`/`pread` 或 mmap 地址计算；TableCache 随后才把结果交给 `Table::Open`。`RandomAccessFile::Read` 的 Slice 可能指向 scratch 或 mmap，调用者必须在对应 buffer/文件对象仍存活时使用。[`Env::NewRandomAccessFile/RandomAccessFile::Read`](../../../../include/leveldb/env.h#L76-L85)、[`PosixRandomAccessFile::Read`](../../../../util/env_posix.cc#L199-L223)、[`PosixMmapReadableFile::Read`](../../../../util/env_posix.cc#L259-L267)

### MANIFEST 写入

```text
VersionSet::LogAndApply
  -> log::Writer::AddRecord
     -> WritableFile::Append
        -> PosixWritableFile::Append
           -> WriteUnbuffered -> write
  -> WritableFile::Sync
     -> SyncDirIfManifest -> fdatasync/fsync(directory)
     -> FlushBuffer -> write
     -> SyncFd -> fdatasync/fsync(MANIFEST)
  -> VersionSet::AppendVersion
```

`VersionSet::AppendVersion` 位于 M04；M06 只提供字节写入和同步语义，不决定版本是否可见。任何 Sync/Append 错误都以 Status 返回到 VersionSet，由上层决定不安装 Version、保存后台错误和清理输出。[`PosixWritableFile::Sync`](../../../../util/env_posix.cc#L334-L351)、[`VersionSet::LogAndApply`](../../../../db/version_set.cc#L792-L858)

### 后台调度

```text
DBImpl::MaybeScheduleCompaction
  -> Env::Schedule(function, arg)
     -> Env 的后台执行机制
        -> 未指定线程调用 function(arg)
           -> DBImpl::BackgroundCall
```

接口明确同一 Env 上的任务可能并发执行，所以 `DBImpl` 自己通过 mutex、scheduled 标志和条件变量维护状态；M06 不保证固定线程数、FIFO 或任务串行。[`Env::Schedule`](../../../../include/leveldb/env.h#L191-L201)、[`DBImpl::MaybeScheduleCompaction`](../../../../db/db_impl.cc#L668-L687)

## 配置/编译到实现选择

| 配置或能力 | 真实分支 | 结果 |
|---|---|---|
| `HAVE_O_CLOEXEC` | `kOpenBaseFlags` | POSIX open 是否带 close-on-exec |
| `HAVE_FDATASYNC` | `SyncFd` | 使用 fdatasync，否则 fsync |
| mmap limit | `g_mmap_limit`/Limiter | 是否允许 mmap 读取 |
| max open files | `g_open_read_only_file_limit`/fd Limiter | 常驻 fd 或每次读打开 |
| `Options::env` | DBImpl 保存的 Env 指针 | Default、MemEnv 或 EnvWrapper |
| `filter_policy`/`block_cache` | M05 Table 路径 | M06 资源接口被不同上层对象使用 |

这些宏由 CMake/平台配置决定；本页不将某一台 Linux 主机的探测结果推广成所有构建的保证。[`Limiter`](../../../../util/env_posix.cc#L43-L129)、[`PosixWritableFile::SyncFd`](../../../../util/env_posix.cc#L391-L418)

## 关键不变量和复杂度

- `RandomAccessFile::Read` 不应依赖可变共享文件偏移；POSIX `pread` 将一次读的 offset 作为参数，单次系统调用工作量与请求字节数相关。
- `FileState` 每次读在 blocks mutex 下复制数据，复制空间由调用者 scratch 提供；大读会跨多个 8 KiB block。
- `PosixWritableFile::Append` 可能复制一次进入 64 KiB buffer，超大写入进入直接写；精确吞吐和系统调用数量需 benchmark。
- Cache 的哈希、分片锁和 Handle 引用是 M05/M02 的性能与正确性边界，M06 不改变其 key/value 语义。

## 正常、失败、清理对照

| 资源 | 正常路径 | 失败路径 | 清理责任 |
|---|---|---|---|
| `SequentialFile*` | New 成功、Reader 消费 | NotFound/IOError | 上层 delete，Slice 不越过 scratch |
| `RandomAccessFile*` | fd/pread 或 mmap | open/pread/offset 错误 | TableAndFile deleter delete；析构 close/munmap |
| `WritableFile*` | Append → Sync → Close | write/Sync/Close Status | 上层按成功/失败路径 delete，必要时 RemoveFile |
| `FileLock*` | LockFile 成功 | 已锁或 OS 错误 | UnlockFile exactly once |
| Schedule task | 后台调用 function | DB shutdown 时跳过工作 | DBImpl 等 scheduled 标志归零 |
| Cache Handle | Lookup/Insert 后借用 value | eviction 仅删映射 | 每个外部 handle Release 一次 |
| Arena allocation | owner 批量持有 | 无单对象回滚 | owner 析构释放整个 arena |

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| Env 文件抽象 | 已完成 | 已完成 | 已完成 | 部分完成 | 已完成 | 已完成 | 已完成 | 已完成 | M06 examples 场景 A/B、链 1/4/6/8 | 真实故障注入未执行 |
| POSIX file | 已完成 | 已完成 | 已完成 | 部分完成 | 已完成 | 已完成 | 已完成 | 已完成 | M06 examples、TableCache 链 | Windows 分支未构建 |
| MemEnv | 已完成 | 已完成 | 已完成 | 部分完成 | 已完成 | 已完成 | 已完成 | 已完成 | M06 examples 场景 D | Sync/Lock 是简化语义 |
| Schedule boundary | 已完成 | 部分完成 | 已完成 | 部分完成 | 已完成 | 部分完成 | 部分完成 | 已完成 | 链 6/8 | 实际线程交错未知 |
| Cache/Arena contract | 已完成 | 已完成 | 已完成 | 部分完成 | 已完成 | 已完成 | 部分完成 | 已完成 | M06 examples 场景 E | 生命周期测试未运行 |

## 相关文档

- [M06 README](README.md)
- [M06 interfaces](interfaces.md)
- [M06 examples](examples.md)
- [M06 testing](testing.md)
- [M06 development guide](development-guide.md)
- [端到端深度链路](../../90-cross-module/end-to-end-traces.md)

## 源码证据摘要

- [`Env` 与文件线程契约](../../../../include/leveldb/env.h#L50-L289)
- [`EnvWrapper`](../../../../include/leveldb/env.h#L331-L402)
- [`Limiter` 与 POSIX 文件实现](../../../../util/env_posix.cc#L43-L275)
- [`PosixWritableFile`](../../../../util/env_posix.cc#L277-L418)
- [`MemEnv/FileState`](../../../../helpers/memenv/memenv.cc#L23-L150)
- [`InMemoryEnv`](../../../../helpers/memenv/memenv.cc#L221-L388)

## 未解决问题

- POSIX/Windows/MemEnv 的完整构建矩阵和真实运行差异尚未验证。
- fd/mmap 限制对性能的影响需要 benchmark，而不是从源码常数推断。
- 断电时目录 Sync、MANIFEST Sync 和 WAL Sync 的真实持久化窗口需要授权实验。

## 下一步阅读建议

先沿随机读链阅读 `Env::NewRandomAccessFile → PosixRandomAccessFile::Read`，再沿 MANIFEST 链阅读 `PosixWritableFile::Sync`，最后用 M06 examples 的 EnvWrapper 设计一个仅静态检查的失败路径回归。
