# M04 Version、Manifest 与 Compaction

- 文档目的：解释持久化版本集合、MANIFEST 增量和分层压缩。
- 适用范围：`version_set.*`、`version_edit.*`、`builder.*`、`table_cache.*`。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：核心机制已确认。
- 最后更新：2026-09-10
- 前置阅读：[M03 WAL/MemTable](../M03-wal-memtable/README.md)
- 后续阅读：[M05 SSTable](../M05-sstable-table/README.md)
## 结论摘要

M04 管理“当前数据库状态”而不是单个文件：Version 按 level 保存表元数据，VersionSet 维护版本链和 MANIFEST，VersionEdit 记录增量；Compaction 选择输入、生成输出并以一次编辑安装。旧 Version 由引用计数保护，旧表由 live-file 集合和 pending outputs 防止误删。

## 关键入口

- `Version::Get`：[db/version_set.cc:324-401](../../../source/leveldb/db/version_set.cc#L324-L401)
- `VersionSet::LogAndApply`：[db/version_set.cc:777-859](../../../source/leveldb/db/version_set.cc#L777-L859)
- `VersionSet::Recover`：[db/version_set.cc:861-993](../../../source/leveldb/db/version_set.cc#L861-L993)
- `VersionSet::PickCompaction`：[db/version_set.cc:1252-1304](../../../source/leveldb/db/version_set.cc#L1252-L1304)
- `SetupOtherInputs`：[db/version_set.cc:1385-1446](../../../source/leveldb/db/version_set.cc#L1385-L1446)

## 相关文档

- [design](design.md)
- [source-map](source-map.md)
- [interfaces](interfaces.md)
- [data-structures](data-structures.md)
- [call-chains](call-chains.md)
- [testing](testing.md)

## 源码证据摘要

见正文链接。

## 未解决问题

压缩阈值的完整历史动机和生产负载影响需 benchmark/历史记录确认。

## 下一步阅读建议

先读 `VersionEdit`，再读 `LogAndApply` 和 `PickCompaction`。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M04-version-compaction/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M04-version-compaction/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |
