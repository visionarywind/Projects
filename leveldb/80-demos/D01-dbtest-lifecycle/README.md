# D01：DBTest 生命周期贯穿场景

- 文档目的：用仓库中真实的 `db/db_test.cc` fixture，把公共 API、WAL/MemTable、flush、Snapshot、Iterator、reopen 和错误验证串成一条可定位的学习路径。
- 性质：真实测试源码解剖；不是新增 demo 程序，也不是本轮另行编写并运行的独立可执行文件。
- 源码版本：`main` / `7ee830d02b623e8ffe0b95d59a74db1e58da04c5`。
- 证据状态：源码和构建目标已确认；本页场景的独立复现命令与逐测试运行结果需按 `build-and-run.md` 重新执行。
- 前置阅读：[端到端深度链路](../../90-cross-module/end-to-end-traces.md)
- 后续阅读：[执行轨迹](execution-trace.md)

## 为什么选择这个“Demo”

仓库没有独立的 `demo/` 程序，但 `DBTest` 是真实的 GoogleTest fixture，直接调用公开 `DB` API，并通过测试专用 `DBImpl` 方法观察内部状态。它比虚构一个示例程序更适合学习：fixture 的 `Reopen`、`Put`、`Get`、`Contents` 和 `MakeTables` 复用生产入口；`SpecialEnv` 又能在不修改 LevelDB 源码的情况下阻塞 Sync、制造写入/同步/Manifest 错误。[`DBTest` fixture](../../../source/leveldb/db/db_test.cc#L260-L352)、[`SpecialEnv`](../../../source/leveldb/db/db_test.cc#L83-L258)

## 场景覆盖

| 阶段 | 真实测试 | 观察点 | 详细页 |
|---|---|---|---|
| fixture 创建与首次 Open | `DBTest` 构造函数 | 临时目录、`DestroyDB`、`DB::Open` | [执行轨迹](execution-trace.md#阶段-1fixture-创建与首次-open) |
| Put/Get/Delete | `PutDeleteGet`、`ReadWrite` | WriteBatch、WAL、MemTable、Status | [数据状态](data-and-state-trace.md#阶段-2写入和删除) |
| Snapshot | `GetSnapshot`、`Snapshot` | sequence 上限和旧值可见性 | [执行轨迹](execution-trace.md#阶段-4snapshot-固定可见序列) |
| Iterator | `IterMulti`、`IteratorPinsRef` | 合并迭代、Slice 借用、引用保护 | [数据状态](data-and-state-trace.md#阶段-5iterator-和引用生命周期) |
| immutable flush/reopen | `GetFromImmutableLayer`、`Recover`、`RecoverDuringMemtableCompaction` | `imm_`、L0 表、WAL 重放 | [调试走读](debug-walkthrough.md#断点组-b写满切换和恢复) |
| compaction | `CompactionsGenerateMultipleFiles`、`DeletionMarkers1/2` | VersionEdit、删除标记和层级 | [失败路径](failure-paths.md#compaction-和清理) |
| Env 故障 | `WriteSyncError`、`ManifestWriteError`、`LogCloseError` | `bg_error_` 与保守清理 | [失败路径](failure-paths.md#io-故障) |

## 推荐阅读顺序

1. 先看 [构建与运行](build-and-run.md)，确认真实目标由 CMake 注册为 `leveldb_tests`。
2. 再看 [执行轨迹](execution-trace.md)，按测试代码顺序跟进控制流。
3. 对照 [数据与状态轨迹](data-and-state-trace.md)，区分 user key、InternalKey、WAL 和文件版本。
4. 需要定位运行时问题时读 [调试走读](debug-walkthrough.md)。
5. 最后读 [失败路径](failure-paths.md) 和 [修改练习](modification-exercises.md)。

## 相关模块

- [M01 公共 API](../../01-modules/M01-public-api/README.md)
- [M02 DB 协调器](../../01-modules/M02-db-coordinator/README.md)
- [M03 WAL/MemTable](../../01-modules/M03-wal-memtable/README.md)
- [M04 Version/Compaction](../../01-modules/M04-version-compaction/README.md)
- [M05 SSTable/Table](../../01-modules/M05-sstable-table/README.md)
- [M06 Env](../../01-modules/M06-env-platform/README.md)
- [M07 测试与构建](../../01-modules/M07-tests-build/README.md)

## 未验证边界

- 本目录不声称已独立编译或运行一个新的 demo target。
- `DelayMilliseconds` 只在现有测试中用于等待条件，不能证明后台任务的固定执行时序。
- 真实断电、文件系统缓存和跨平台 Env 行为仍需单独实验。
