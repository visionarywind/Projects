# 跨模块端到端流程总览

- 文档目的：提供从公共 API 到持久化文件和后台任务的导航，并把详细链路分流到端到端追踪页。
- 适用范围：M01–M07 的运行时、恢复、测试和资源边界。
- 源码版本：`main` / `7ee830d`。
- 证据状态：总览入口和关键函数由源码确认；本机 Linux Debug build/CTest/有限 benchmark 已验证，实际线程时序、断电语义和更广泛实验仍未验证。
- 最后更新：2026-09-10
- 前置阅读：[总览架构](../00-overview/architecture.md)
- 后续阅读：[端到端深度链路](end-to-end-traces.md)

## 阅读入口

原先的调用链列表适合定位符号，但不足以解释锁、引用计数、Status 和物理文件何时变化。请以 [`end-to-end-traces.md`](end-to-end-traces.md) 为主线阅读；该页把每条链拆成阶段、分支、状态转移、资源和示例。

| 目标 | 详细链路 |
|---|---|
| 首次创建、CURRENT、MANIFEST、初始 WAL | [链 1：首次打开与创建](end-to-end-traces.md#链-1首次打开与创建数据库) |
| 重开、MANIFEST、WAL 重放、sequence 恢复 | [链 2：已有数据库重开与崩溃恢复](end-to-end-traces.md#链-2已有数据库重开与崩溃恢复) |
| 多线程写、batch group、sync、限流 | [链 3：并发批量写入](end-to-end-traces.md#链-3并发批量写入限流与日志先行) |
| Get、快照、MemTable、TableCache、Block | [链 4：单键读取](end-to-end-traces.md#链-4单键读取从快照序号到-table-block) |
| Iterator、merging iterator、cleanup | [链 5：Iterator 与快照读取](end-to-end-traces.md#链-5iterator-与快照的长生命周期读取) |
| immutable MemTable 到 Level-0 SSTable | [链 6：MemTable flush](end-to-end-traces.md#链-6memtable-切换immutable-flush-与-level-0-安装) |
| compaction 选输入、裁剪、输出和安装 | [链 7：分层 compaction](end-to-end-traces.md#链-7分层-compaction版本裁剪与输出安装) |
| 错误传播、关闭和所有权 | [链 8：错误、关闭和资源生命周期](end-to-end-traces.md#链-8错误关闭和资源生命周期) |

## 总体数据流

```text
公共 API
  ├─ Open ──> LOCK + CURRENT/MANIFEST + WAL + MemTable
  ├─ Write ─> WriteBatch bytes ─> WAL ─> InternalKey/MemTable
  ├─ Get ───> LookupKey ─> mem/imm ─> Version ─> TableCache ─> Block
  └─ Iterator -> mem/imm + Version table iterators -> merge -> user iterator

写满 MemTable
  -> 新 WAL/MemTable + imm
  -> Env::Schedule(BGWork)
  -> BuildTable(SSTable)
  -> VersionEdit -> MANIFEST Sync -> AppendVersion
  -> obsolete file cleanup
```

## 核心链摘要

### 打开/恢复

`DB::Open` 在 `DBImpl::Recover` 中获得目录锁，读取 CURRENT/MANIFEST 重建 Version，再扫描较新的 WAL；WAL 中的 WriteBatch 重放到 MemTable，必要时生成 Level-0 表，最后以 VersionEdit 更新 MANIFEST。[`DB::Open`](../../../db/db_impl.cc#L1503-L1544)、[`DBImpl::Recover`](../../../db/db_impl.cc#L292-L383)、[`VersionSet::Recover`](../../../db/version_set.cc#L861-L991)

### 写入

`DBImpl::Write` 通过 `writers_` 串行化队首 writer；`MakeRoomForWrite` 负责等待或切换内存表；写入阶段先 `log_->AddRecord`，sync 写再 Sync，成功后才插入 MemTable。[`DBImpl::Write`](../../../db/db_impl.cc#L1206-L1276)、[`DBImpl::MakeRoomForWrite`](../../../db/db_impl.cc#L1331-L1405)

### 读取

`DBImpl::Get` 固定 snapshot sequence 并暂时 Ref 住 mem/imm/current，解锁后按 mem → imm → Version 顺序读取；Version 通过 TableCache 访问表，Table 再访问 index/filter/data block。[`DBImpl::Get`](../../../db/db_impl.cc#L1121-L1165)、[`Version::Get`](../../../db/version_set.cc#L324-L400)、[`TableCache`](../../../db/table_cache.cc#L40-L111)

### 后台 flush/compaction

调度入口只排队 `BGWork`，不保证立即执行。后台优先 flush imm；普通 compaction 由 score 或 seek 触发，完成 SSTable 后把输入删除和输出添加封装进一次 MANIFEST edit。[`DBImpl::MaybeScheduleCompaction`](../../../db/db_impl.cc#L668-L706)、[`DBImpl::DoCompactionWork`](../../../db/db_impl.cc#L898-L1057)

### 关闭

析构先发布 shutdown 标志并等待 scheduled 后台工作归零，再解锁 DB、释放 Version/MemTable/日志/TableCache。调用方必须先释放 Iterator、Snapshot 等公共对象。[`DBImpl::~DBImpl`](../../../db/db_impl.cc#L152-L178)

## 证据和边界

以上摘要只保留入口；锁释放点、引用保护、文件编号和错误分支在 [端到端深度链路](end-to-end-traces.md) 展开。源码能证明机制，不能单独证明固定线程数量、断电后的每种文件系统结果或性能保证。本机 Linux Debug build、CTest 和有限 benchmark 已完成；独立故障注入、跨平台和生产性能验证仍未完成。

## 相关文档

- [端到端深度链路](end-to-end-traces.md)
- [跨模块调用链](cross-module-call-chains.md)
- [共享数据与类型](shared-data-and-types.md)
- [错误边界](error-boundaries.md)
- [配置影响地图](configuration-impact-map.md)
- [性能关键路径](performance-critical-paths.md)

## 源码证据摘要

- [`DBImpl`](../../../db/db_impl.cc#L126-L178)
- [`DBImpl::Recover`](../../../db/db_impl.cc#L292-L503)
- [`DBImpl::Write`](../../../db/db_impl.cc#L1206-L1405)
- [`VersionSet::LogAndApply`](../../../db/version_set.cc#L777-L858)
- [`VersionSet::PickCompaction`](../../../db/version_set.cc#L1252-L1446)

## 未解决问题

- 运行时后台调度的实际线程交错需 Env/实验确认。
- 断电、文件系统缓存和故障注入的精确结果需执行测试；Debug CTest 已通过，但不能替代这些边界实验。

## 下一步阅读建议

从端到端深度链路的贯穿示例开始，然后选择对应模块的 `call-chains.md` 和 `examples.md` 对照源码。
