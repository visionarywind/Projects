# D01 修改练习与回归边界

- 文档目的：解释 80-demos/D01-dbtest-lifecycle/modification-exercises.md 的职责、证据和维护边界。
- 适用范围：本页及其直接关联的源码、测试和配置；第三方、生成物与动态结果仅在有证据时纳入。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：部分完成；静态证据优先，构建、运行和硬件行为未在本轮验证。
- 最后更新：2026-09-15
- 前置阅读：[项目入口](../../README.md)。
- 后续阅读：[分析状态](../../00-overview/analysis-state.md)。
## 结论摘要

本页聚焦 80-demos/D01-dbtest-lifecycle/modification-exercises.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


这些练习的目的不是要求现在修改源码，而是把端到端链路转成可执行的工程问题。每项都列出应先阅读的入口、可能受影响的模块和最小回归集合。

## 练习 1：增加一个可选的同步写测试

**任务**：在 `DBTest` 中使用 `WriteBatch` 同时写两个 key，设置 `WriteOptions::sync=true`，重开后验证两项都存在。

**先读**：`DBImpl::Write`、`BuildBatchGroup`、`WriteBatch::Iterate`、`RecoverLogFile`。[`DBImpl::Write`](../../../source/leveldb/db/db_impl.cc#L1206-L1277)、[`WriteBatch`](../../../source/leveldb/db/write_batch.cc#L42-L79)

**必须考虑**：

- sequence 从 batch 第一条开始连续递增；
- WAL AddRecord/Sync 成功后才插入 MemTable；
- 重开时 MANIFEST 的 last sequence 与 WAL 最大 sequence 取最大值；
- 测试必须检查返回 Status，不能只检查当前进程 Get。

**回归**：`DBTest.WriteSyncError`、`DBTest.Recover`、`db/write_batch_test.cc`、`db/log_test.cc`。

## 练习 2：把 Get 改成新的临时缓存

**任务**：为单键读取增加统计或临时缓存，但不改变 Snapshot 语义。

**先读**：`DBImpl::Get` 的 Ref/Unlock/Lock/Unref 顺序、`Version::Get`、`TableCache::Get`。[`DBImpl::Get`](../../../source/leveldb/db/db_impl.cc#L1121-L1166)

**必须保持**：

- 解锁前抓住 mem/imm/current 的引用；
- 临时缓存不得保存跨 owner 生命周期的 Slice；
- 读取错误仍从 Table/Version 向上传递；
- Snapshot sequence 不能由 latest sequence 替代。

**回归**：`DBTest.GetSnapshot`、`DBTest.Snapshot`、`DBTest.IteratorPinsRef`、`DBTest.GetFromVersions`、`table/table_test.cc`。

## 练习 3：修改 MemTable flush 阈值

**任务**：改变默认或测试 Options 的 `write_buffer_size`，观察 imm 切换和恢复成本。

**先读**：`MakeRoomForWrite`、`WriteLevel0Table`、`CompactMemTable`、`RecoverLogFile`。[`MakeRoomForWrite`](../../../source/leveldb/db/db_impl.cc#L1331-L1405)

**必须评估**：

- 最多两个 write buffer 的内存占用；
- imm 未完成时 writer 的 CondVar 等待；
- pending output 与 VersionEdit 的提交顺序；
- 大 WAL 重开时中途生成多个 L0 表的行为。

**回归**：`GetFromImmutableLayer`、`RecoverDuringMemtableCompaction`、`RecoverWithLargeLog`、`MinorCompactionsHappen`。

## 练习 4：替换 FilterPolicy

**任务**：使用自定义 FilterPolicy 或改变 Bloom bits-per-key，并验证命中与 miss。

**先读**：`TableBuilder` filter block、`Table::InternalGet`、`FilterBlockReader::KeyMayMatch` 和 `SpecialEnv` 的 random-read 计数。[M05 实现](../../01-modules/M05-sstable-table/implementation.md)

**必须保持**：

- filter false 只能表示“不可能命中”；
- filter 缺失/未知/读取错误不能制造 false negative；
- `ReadOptions::verify_checksums` 和 `fill_cache` 语义不被意外改变；
- Cache Handle 在 direct Get 和 iterator 两条路径都正确释放。

**回归**：`DBTest.BloomFilter`、`table/filter_block_test.cc`、`table/table_test.cc`、`db/table_cache_test.cc`。

## 练习 5：新增 Env 故障注入

**任务**：在 `SpecialEnv` 中加入一种只让某类文件 `Close` 或 `Sync` 失败的开关。

**先读**：`Env` 文件线程契约、`EnvWrapper` 转发、POSIX WritableFile、`RecordBackgroundError`。[`Env`](../../../source/leveldb/include/leveldb/env.h#L50-L217)、[`EnvWrapper`](../../../source/leveldb/include/leveldb/env.h#L331-L402)

**必须记录**：

- 失败发生在 Append、Flush、Sync、Close 的哪一步；
- 当前 API 是同步返回还是进入 `bg_error_`；
- 输出文件是否被 Abandon/删除；
- 重开从哪个 Version/WAL 恢复。

**回归**：`WriteSyncError`、`LogCloseError`、`ManifestWriteError`、`MissingSSTFile`。

## 练习 6：修改公共 API 或头文件

**任务**：改变 `Options`、`DB` 或 `Iterator` 公共接口前，建立 ABI/安装/实现/测试检查表。

**必须查看**：

- `include/leveldb/*.h` 的所有权、线程和 Status 契约；
- CMake 的安装头文件清单和 `LEVELDB_EXPORT`；
- `db/c.cc` 的 C API 边界；
- M01 文档和 CTest 的 `c_test`。

**回归**：Debug/Release 至少各一次（若修改 ABI），`leveldb_tests`、`c_test`、`env_posix_test`，以及本地 prefix install 后用外部小程序编译头文件。

## 练习 7：改动 compaction 裁剪规则

**任务**：改变旧版本或 deletion marker 的保留判断。

**先读**：`DoCompactionWork` 的 smallest snapshot/base-level 条件和 VersionSet 输入扩展。[`DoCompactionWork`](../../../source/leveldb/db/db_impl.cc#L898-L1057)、[`SetupOtherInputs`](../../../source/leveldb/db/version_set.cc#L1385-L1446)

**必须证明**：

- 活跃 Snapshot 仍能读到旧版本；
- 没有 Snapshot 时可安全裁剪覆盖版本；
- deletion 只在 base-level 条件满足时丢弃；
- L0 overlap 和 boundary files 不会漏输入。

**回归**：`Snapshot`、`HiddenValuesAreRemoved`、`DeletionMarkers1`、`DeletionMarkers2`、`OverlapInLevel0`、`SparseMerge`。

## 改动前后的报告模板

```text
变更意图:
受影响模块: M01 ... M07
控制流变化:
数据/文件格式变化:
锁/线程变化:
owner/Ref/Unref/Handle/Slice 变化:
Status/错误变化:
测试命令:
实际结果:
未验证边界:
```

## 禁止的捷径

- 不要只改 `db/db_impl.cc` 而跳过公共头文件、Env、Version 和测试影响分析。
- 不要用固定 `sleep` 证明后台 compaction 已完成；优先使用现有测试专用入口、属性或条件变量可观察状态。
- 不要把 Debug benchmark 当成性能回归结论。
- 不要把 MemEnv 的简化 `Sync`/Lock 语义直接推广到 POSIX 断电语义。
- 不要把 `Slice`、Cache Handle、Snapshot 或 Iterator 当作 owning 对象。

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
