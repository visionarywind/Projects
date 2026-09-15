# D01 失败路径

- 文档目的：解释 80-demos/D01-dbtest-lifecycle/failure-paths.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-dbtest-lifecycle/failure-paths.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


D01 中的错误测试依赖 `SpecialEnv`，其作用是让现有生产代码收到可控的 `Status`，再验证错误传播和清理，而不是模拟任意操作系统故障。

## 1. IO 故障

### 1.1 WAL Sync 失败

`WriteSyncError` 先让普通写 `k1` 成功（不调用 Sync），再把 `data_sync_error_` 设为 true，并以 `sync=true` 写 `k2`。`DBImpl::Write` 的 WAL AddRecord 可能成功，但 Sync 返回错误，因此 `k2` 不插入 MemTable，调用返回非 OK；代码将 `bg_error_` 设为该错误，之后即使关闭注入，`k3` 仍被 `MakeRoomForWrite` 拒绝。[`WriteSyncError`](../../../source/leveldb/db/db_test.cc#L1818-L1847)、[`DBImpl::Write`](../../../source/leveldb/db/db_impl.cc#L1234-L1253)

```text
k1: AddRecord -> InsertInto -> Get(k1)=v1
k2: AddRecord -> Sync error -> skip InsertInto -> Get(k2)=NOT_FOUND
future k3: MakeRoomForWrite sees bg_error_ -> fail
```

不要把“Sync 失败后 WAL 一定没有记录”写成结论；源码明确说日志状态可能不确定，重开可见性需要专门实验。

### 1.2 WAL Close 失败

`LogCloseError` 在 MemTable 切换期间让 log file `Close` 返回错误。测试确认当前写入失败，且未来写入继续失败，覆盖了切换旧 WAL 时不能吞掉 Close 错误的回归边界。[`LogCloseError`](../../../source/leveldb/db/db_test.cc#L1980-L2005)

### 1.3 新文件不可写

`NonWritableFileSystem` 让 `NewWritableFile` 失败，先写入一个基准值，再用小 write buffer 反复写大 value，断言至少出现错误。这里验证的是 `Status` 传递和后台状态反应，不验证某个固定错误次数。[`NonWritableFileSystem`](../../../source/leveldb/db/db_test.cc#L1797-L1816)、[`SpecialEnv::NewWritableFile`](../../../source/leveldb/db/db_test.cc#L220-L232)

## 2. MANIFEST 提交失败

`ManifestWriteError` 有两轮：第一轮令 MANIFEST `Sync` 失败，第二轮令 MANIFEST `Append` 失败。每轮先把 `foo=bar` flush 到较深 level，再让合并 compaction 失败，恢复注入后 Reopen，最后断言仍读到 `bar`。[`ManifestWriteError`](../../../source/leveldb/db/db_test.cc#L1849-L1887)

关键状态边界：

```text
输出 SSTable 已生成
  -> VersionEdit 准备提交
  -> MANIFEST Append/Sync 失败
  -> 新 Version 不应被当成已提交
  -> bg_error_ 记录错误，未确认提交的输出不能被随意 GC
  -> 重开从旧 current Version/WAL 恢复数据
```

该测试针对历史上“MANIFEST 记录写入/Sync 失败后过早删除输出，导致重开引用缺失文件”的危险窗口。它证明现有 fixture 下数据不丢，不证明所有文件系统断电窗口。

## 3. SSTable 丢失、旧扩展名和恢复

- `MissingSSTFile` flush 后关闭 DB，删除一个表文件，以 `paranoid_checks=true` 重开，断言 Open 失败且错误包含 missing 信息。[`MissingSSTFile`](../../../source/leveldb/db/db_test.cc#L1889-L1904)
- `StillReadSST` 把 `.ldb` 改为 `.sst`，以 paranoid checks 重开并读取成功，覆盖旧表名兼容分支。[`StillReadSST`](../../../source/leveldb/db/db_test.cc#L1906-L1920)

这两个测试要一起看：缺少已声明文件是 Corruption；合法的旧扩展名不是错误。不能只根据目录中是否存在任意表文件判断恢复是否安全。

## 4. WAL 尾部和恢复错误

`RecoverLogFile` 使用 `log::Reader` 强制校验 checksum；Reader 对完整逻辑记录、FIRST/MIDDLE/LAST、CRC、错误长度和尾部半记录分别处理。`DBImpl` 再根据 `paranoid_checks` 决定是否把 Reporter 的第一个错误作为 Open 失败。[`RecoverLogFile`](../../../source/leveldb/db/db_impl.cc#L385-L503)、[`Reader::ReadRecord`](../../../source/leveldb/db/log_reader.cc#L56-L173)

D01 的 `Recover`、`RecoverWithLargeLog` 和 `RecoverDuringMemtableCompaction` 验证正常 WAL 重放、一个日志内的多次恢复 flush，以及旧 imm 与新 log 并存；它们没有执行破坏性截断或断电注入。因此以下项目仍是未验证：

- 真实进程在任意物理 record 中止时的文件内容；
- 目录和文件系统缓存的持久化顺序；
- 非 paranoid 模式下所有损坏组合的可见结果。

## 5. Compaction 失败和清理

Compaction 可能在输出打开、TableBuilder、输入 iterator、Sync、Close、验证或 `LogAndApply` 阶段失败。`BackgroundCompaction`/`CleanupCompaction` 需要放弃未完成 builder、删除未提交 outfile、移除 `pending_outputs_`、释放 CompactionState/Version 引用，并把首个错误保存到 `bg_error_`；后台错误存在时 obsolete GC 保守退出。[`DBImpl::CleanupCompaction`](../../../source/leveldb/db/db_impl.cc#L789-L804)、[`DBImpl::BackgroundCompaction`](../../../source/leveldb/db/db_impl.cc#L708-L787)、[`DBImpl::RemoveObsoleteFiles`](../../../source/leveldb/db/db_impl.cc#L225-L290)

`ManifestWriteError` 是 D01 中直接可运行的提交失败样例；输出表构建错误和 checksum/压缩错误分别由 M05 测试入口覆盖，不能把它们冒充为本 D01 已独立执行的结果。

## 6. 生命周期违规

以下不是 LevelDB 应自动修复的错误，而是调用方违反公共契约：

- `delete db` 前仍持有 Iterator；
- ReleaseSnapshot 后继续用 Snapshot；
- 让 `Iterator::key()/value()` 的 Slice 跨下一次 iterator 修改或 owner 销毁；
- 把一个 DB 的 Snapshot 交给另一个 DB；
- 多线程同时修改同一个 WriteBatch/Iterator 而没有外部同步。

`DBTest` 的 `Contents`、`IterStatus` 和 Snapshot 测试展示了正确顺序；公共头文件给出了调用方责任。[`DBTest::Contents/IterStatus`](../../../source/leveldb/db/db_test.cc#L367-L392)、[`db.h`](../../../source/leveldb/include/leveldb/db.h#L90-L106)、[`iterator.h`](../../../source/leveldb/include/leveldb/iterator.h#L60-L81)

## 7. 错误处理检查表

| 问题 | 应检查 | 不应假设 |
|---|---|---|
| 写入失败 | 返回 `Status`、后续 `bg_error_`、Get 是否看到 batch | AddRecord 成功就等于 Sync 持久化 |
| 表构建失败 | builder Status、outfile、pending_outputs_ | 文件已出现就等于 Version 可见 |
| MANIFEST 失败 | old current Version、重开结果、GC 是否保守 | 可以立即删除所有旧/新表 |
| Open 失败 | `dbptr == nullptr`、Status 类型和 message | 返回了半初始化 DB |
| Iterator 错误 | `Valid()` 后仍调用 `status()` | `!Valid()` 必然等于正常结束 |
| Snapshot | Release 次数和 DB 所属 | Snapshot 复制了 value 或跨 DB 永久有效 |

## 相关页面

- [执行轨迹](execution-trace.md)
- [数据与状态轨迹](data-and-state-trace.md)
- [全局错误模型](../../00-overview/global-error-model.md)
- [错误边界](../../90-cross-module/error-boundaries.md)
- [M06 Env 实现](../../01-modules/M06-env-platform/implementation.md)

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
