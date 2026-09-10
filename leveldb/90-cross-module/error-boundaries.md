# 跨模块错误边界（展开版）

- 文档目的：按端到端阶段说明错误的产生点、Status 传播、后台错误固化和资源清理。
- 适用范围：Open/recovery、Write、Get/Iterator、flush、compaction、shutdown。
- 源码版本：`main` / `7ee830d`。
- 证据状态：主要错误分支和清理动作已由源码确认；具体文件系统断电语义需实验。
- 最后更新：2026-09-10
- 前置阅读：[端到端深度链路](end-to-end-traces.md)
- 后续阅读：[风险登记](../99-roadmap/risk-register.md)

## 统一错误模型

```text
系统 errno / malformed bytes / CRC / compression / comparator mismatch
  -> Env、log、table、VersionSet 生成 Status
       ├─ Open/Recover：同步返回，DBImpl 不交付给调用方
       ├─ Get/Iterator：同步返回 Status 或 iterator.status()
       ├─ Write：当前调用返回；Sync 不确定性还写入 bg_error_
       └─ Background：RecordBackgroundError 保存首个错误并唤醒等待者
```

LevelDB 不依赖 C++ 异常跨越模块；Status 是错误载荷，锁、裸指针和临时文件仍须在每个出口保持可析构状态。[`Status`](../../../include/leveldb/status.h#L20-L115)

## 1. Open/创建错误

### 正常阶段

`DB::Open` 初始化 `*dbptr=nullptr`，创建 DBImpl 并在锁下调用 Recover。Recover 创建目录、锁定 LOCK，按 `CURRENT` 判断是创建还是已有数据库；创建路径的 NewDB 写初始 MANIFEST、Sync/Close，再写 CURRENT。之后 Open 创建新 WAL/MemTable，必要时 LogAndApply，最后才把 DB 指针交给调用方。[`DB::Open`](../../../db/db_impl.cc#L1503-L1543)、[`DBImpl::Recover`](../../../db/db_impl.cc#L292-L326)、[`DBImpl::NewDB`](../../../db/db_impl.cc#L181-L213)

### 错误出口

| 失败点 | 直接 Status | 后续动作 |
|---|---|---|
| LockFile | Env IO/其他错误 | Recover 返回；DBImpl 由 Open delete |
| `create_if_missing=false` 且无 CURRENT | InvalidArgument | 不创建数据库 |
| NewDB MANIFEST 写/Sync/Close | IO Status | 删除临时 MANIFEST（失败路径），Open delete impl |
| CURRENT 不存在/格式错误 | NotFound/Corruption | 不扫描 WAL，不交付 DB |
| CURRENT 指向缺失 MANIFEST | Corruption | `VersionSet::Recover` 返回 |
| comparator 不匹配 | InvalidArgument | 不安装 recovered Version |
| 缺少 MANIFEST 必需字段 | Corruption | 不交付 DB |

Open 失败时 `DB::Open` 解锁 mutex 后 delete impl；因此调用者应只检查 Status 和 `*dbptr`，不能使用失败路径的 DB 指针。[`DB::Open`](../../../db/db_impl.cc#L1536-L1543)、[`VersionSet::Recover`](../../../db/version_set.cc#L869-L991)

## 2. WAL 恢复错误

`RecoverLogFile` 通过 Env 打开 SequentialFile，Reader 强制做 checksum。`LogReporter` 总是记录损坏；`paranoid_checks=true` 时把第一个错误写入 Status，false 时允许丢弃坏记录继续恢复。小于 12 字节的 record、WriteBatch 解析失败或 MemTable 插入失败都进入相同 Status 路径。[`DBImpl::RecoverLogFile`](../../../db/db_impl.cc#L385-L467)

当临时 MemTable 超阈值时，恢复线程同步调用 WriteLevel0Table；生成失败会立即终止 Open，而不是把尚未落盘的数据留在一个未返回的 DB 中。恢复结束后若不能复用最后 WAL，剩余 mem 也会被 flush 并由 `save_manifest` 驱动版本 edit。[`DBImpl::RecoverLogFile`](../../../db/db_impl.cc#L455-L502)

**边界说明**：源码能确认 Reader/Reporter 的处理接口和 `paranoid_checks` 分支；某种断电产生的具体尾部字节是否被 Reader 视为可忽略截断，应以 `log_reader.cc` 的完整判定和测试为准，不能仅凭“WAL 有 CRC”推断。

## 3. Write 错误

### AddRecord/Sync/InsertInto

写入队首 writer 建立 sequence，释放 mutex 后按顺序执行 AddRecord、可选 logfile Sync、InsertInto。AddRecord 失败或 Sync 失败时不会执行 InsertInto；成功后才重新加锁更新 last sequence，并向本批次覆盖的 writer 返回 Status。[`DBImpl::Write`](../../../db/db_impl.cc#L1221-L1276)

| 失败点 | 当前调用返回 | DB 全局状态 |
|---|---|---|
| `MakeRoomForWrite` 发现既有 `bg_error_` | 保存的错误 | 不再接受新写 |
| 新 WAL `NewWritableFile` | IO Status | 旧状态保留；file number 复用 |
| 旧 logfile Close | 错误记入 `bg_error_`，仍切日志 | 未来写停止，旧 log 状态不确定 |
| WAL AddRecord | 错误 | 当前 batch 不进 MemTable；是否已有部分物理写入需 Env 语义确认 |
| WAL Sync | 错误 | 当前 batch 不进 MemTable；记录 `bg_error_`，未来写失败 |
| InsertInto | 错误 | 当前调用返回；需按实现检查批次/内存状态 |

Sync 失败的特殊处理是因为“log record 是否在重开时出现”不确定；代码宁可把 DB 置于不可继续写的状态，也不把潜在分叉的日志与 MemTable 当成正常状态。[`DBImpl::Write`](../../../db/db_impl.cc#L1235-L1257)、[`DBImpl::RecordBackgroundError`](../../../db/db_impl.cc#L660-L666)

## 4. 后台错误与写入背压

后台 flush/compaction、输出文件验证、MANIFEST LogAndApply 或文件删除会产生 Status。后台函数调用 `RecordBackgroundError` 只保留第一个错误，并 `SignalAll`；之后 `MakeRoomForWrite` 在循环第一项读取 `bg_error_` 并返回它。`MaybeScheduleCompaction` 看到错误后不再排队新的后台工作。[`DBImpl::RecordBackgroundError`](../../../db/db_impl.cc#L660-L682)、[`DBImpl::MakeRoomForWrite`](../../../db/db_impl.cc#L1331-L1341)

`RemoveObsoleteFiles` 在 `bg_error_` 非 OK 时直接返回：版本提交状态不明时不做可能不可逆的垃圾回收。该策略可能留下文件，但优先避免把错误扩大为数据丢失。[`DBImpl::RemoveObsoleteFiles`](../../../db/db_impl.cc#L225-L237)

## 5. Get/Iterator 错误

Get 先 Ref 住 mem/imm/current，解锁进行读取；MemTable miss 才进入 Version。Version 对每个候选文件调用 TableCache；TableCache cache miss 时打开文件、Table::Open，失败不缓存错误；成功 handle 在 direct Get 结束时 Release。Table 返回 IO/Corruption 时 `Version::Get` 停止搜索并向 Get 返回 Status。[`DBImpl::Get`](../../../db/db_impl.cc#L1121-L1165)、[`Version::Get`](../../../db/version_set.cc#L324-L400)、[`TableCache::FindTable/Get`](../../../db/table_cache.cc#L40-L111)

Iterator 错误通常延迟到 `Iterator::status()`：child iterator 可能在 Seek/Next 中遇到 block 读取错误；cleanup 仍必须释放 cache handle、Version 和 MemTable 引用。调用方即使 `Valid()` 为 false，也应检查 status 后再 delete iterator。[`include/leveldb/iterator.h`](../../../include/leveldb/iterator.h#L20-L66)、[`CleanupIteratorState`](../../../db/db_impl.cc#L1071-L1079)

## 6. Flush/compaction 错误和临时输出

`WriteLevel0Table` 为新表号加入 `pending_outputs_`，锁外 BuildTable，完成后重新加锁、删除 input iterator、移除 pending 标记；成功的非空文件加入 VersionEdit。`CompactMemTable` 只有 LogAndApply 成功才释放 imm 并执行 obsolete 清理；失败调用 RecordBackgroundError，旧 imm 不能被当作已提交删除。[`DBImpl::WriteLevel0Table`](../../../db/db_impl.cc#L505-L547)、[`DBImpl::CompactMemTable`](../../../db/db_impl.cc#L549-L580)

普通 compaction 的输出有三层检查：builder Finish/Abandon、outfile Sync/Close、TableCache reopen 验证。任何一步失败，CleanupCompaction 放弃未完成 builder、删除 outfile 并移除 pending output；输入 Version 引用随后释放，但 obsolete 清理因 bg_error 保守停止。[`DBImpl::FinishCompactionOutputFile`](../../../db/db_impl.cc#L831-L877)、[`DBImpl::CleanupCompaction`](../../../db/db_impl.cc#L789-L804)

MANIFEST 的 `LogAndApply` 先构造临时 Version；写 record 和 Sync 失败时不 AppendVersion，若是新 manifest 还删除临时文件和 descriptor objects。**文件已生成不等于版本已提交**：输入表不能因为输出生成就立即删除。[`VersionSet::LogAndApply`](../../../db/version_set.cc#L792-L858)

## 7. Shutdown 错误边界

析构先锁 DB mutex、发布 `shutting_down_`，等待 `background_compaction_scheduled_` 归零。后台看到 shutdown 后跳过新工作，清除 scheduled 并广播；正在执行的 DoCompactionWork 检查 shutdown 并返回“Deleting DB during compaction”的 IOError，随后清理其 builder/outfile。[`DBImpl::~DBImpl`](../../../db/db_impl.cc#L152-L159)、[`DBImpl::BackgroundCall`](../../../db/db_impl.cc#L689-L706)、[`DBImpl::DoCompactionWork`](../../../db/db_impl.cc#L1022-L1057)

等待结束后才解锁 db_lock、删除 VersionSet、Unref mem/imm、删除 log/file/TableCache。公共 API 要求调用方先释放 Iterator/Snapshot；否则 cleanup 可能访问已经释放的 DB 状态。[`DBImpl::~DBImpl`](../../../db/db_impl.cc#L161-L178)、[`include/leveldb/db.h`](../../../include/leveldb/db.h#L83-L105)

## 8. 错误演练矩阵

| 场景 | 入口 | 预期观察 | 不能假定 |
|---|---|---|---|
| MANIFEST 缺字段 | reopen | Open 返回 Corruption | 自动修复且数据完整 |
| WAL 尾部截断 | reopen | 以 Reader 规则处理；检查 Status/日志 | 所有截断都安全 |
| WAL Sync 失败 | Write(sync=true) | 当前 Write 失败，后续写失败 | 该 record 一定存在或一定不存在 |
| SSTable 读取损坏 | Get/Iterator | Corruption/iterator status | 继续返回旧值 |
| compaction output Sync 失败 | 后台 | bg_error、输出清理、文件保守保留 | 输入已可删除 |
| delete DB during compaction | DB destructor | 等待后台，临时输出清理 | 固定 sleep 足够同步 |

这些场景的独立故障注入命令和逐项结果当前未执行。Linux Debug configure/build 已成功，CTest 的三个聚合目标已通过；聚合测试不能替代 WAL 尾部、MANIFEST 损坏、输出 Sync 失败和真实断电窗口的专门实验。

## 相关文档

- [端到端深度链路](end-to-end-traces.md)
- [共享数据与类型](shared-data-and-types.md)
- [调试指南](../99-roadmap/debugging-guide.md)
- [M07 测试](../01-modules/M07-tests-build/testing.md)

## 源码证据摘要

见各节；核心边界包括 [`DBImpl::Write`](../../../db/db_impl.cc#L1206-L1276)、[`DBImpl::RecoverLogFile`](../../../db/db_impl.cc#L385-L503)、[`VersionSet::LogAndApply`](../../../db/version_set.cc#L777-L858) 和 [`DBImpl::~DBImpl`](../../../db/db_impl.cc#L152-L178)。

## 未解决问题

- 断电时文件系统对 Sync/Close 的真实持久化语义未验证。
- fault injection 测试当前尚未作为独立实验运行；Linux Debug 聚合测试已通过，但不能替代专门故障注入和断电语义验证。
- 某些 Env 对尾部读取和 append 的差异需平台测试。

## 下一步阅读建议

先沿表格选一个错误场景，反向阅读其直接函数和 cleanup，再用 `db/fault_injection_test.cc` 设计可执行回归。
