# M02 DB 生命周期与协调器

- 文档目的：解释 `DBImpl` 如何协调打开、恢复、读写、快照、后台工作和关闭。
- 适用范围：`db/db_impl.{h,cc}`、相关 DB 辅助。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：已确认；线程调度数量仍需平台验证。
- 最后更新：2026-09-10
- 前置阅读：[M01 公共 API](../M01-public-api/README.md)
- 后续阅读：[M03 WAL/MemTable](../M03-wal-memtable/README.md)、[M04 Version](../M04-version-compaction/README.md)
## 结论摘要

本页聚焦 01-modules/M02-db-coordinator/README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 一句话说明

M02 是整个数据库实例的控制平面：它持有所有长期资源和锁，把公共 API 调用转换为 WAL/MemTable 操作，把后台任务转换为表构建和 Version 安装。

## 入口与关键字段

`DBImpl` 实现全部 DB 虚接口（[db/db_impl.h:28-71](../../../source/leveldb/db/db_impl.h#L28-L71)）。关键状态包括 `mem_`/`imm_`、`log_`/`logfile_`、writer 队列、snapshots、pending outputs、后台调度标志和 `versions_`（[db/db_impl.h:169-204](../../../source/leveldb/db/db_impl.h#L169-L204)）。

## 核心路径

- 打开：`DB::Open` → 构造 DBImpl → `Recover`。[db/db_impl.cc:1503-1546](../../../source/leveldb/db/db_impl.cc#L1503-L1546)
- 写入：`Write` 分组 writer、写 WAL、插入 MemTable。[db/db_impl.cc:1206-1330](../../../source/leveldb/db/db_impl.cc#L1206-L1330)
- 读取：`Get` 查 MemTable/immutable/Version。[db/db_impl.cc:1121-1166](../../../source/leveldb/db/db_impl.cc#L1121-L1166)
- 后台：`MaybeScheduleCompaction` → `BackgroundCall` → flush/compaction。[db/db_impl.cc:668-1079](../../../source/leveldb/db/db_impl.cc#L668-L1079)
- 关闭：等待后台工作、解锁、释放资源。[db/db_impl.cc:151-178](../../../source/leveldb/db/db_impl.cc#L151-L178)

## 相关文档

- [design](design.md)
- [source-map](source-map.md)
- [call-chains](call-chains.md)
- [line-level-analysis](line-level-analysis.md)
- [testing](testing.md)

## 源码证据摘要

见正文链接。

## 未解决问题

不同 Env 的 Schedule 是否并行执行及精确线程数量需平台实现确认。

## 下一步阅读建议

从 `DB::Open` 和 `DBImpl::Write` 开始。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M02-db-coordinator/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M02-db-coordinator/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |
