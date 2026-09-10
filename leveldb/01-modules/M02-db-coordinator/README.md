# M02 DB 生命周期与协调器

- 文档目的：解释 `DBImpl` 如何协调打开、恢复、读写、快照、后台工作和关闭。
- 适用范围：`db/db_impl.{h,cc}`、相关 DB 辅助。
- 证据状态：已确认；线程调度数量仍需平台验证。
- 最后更新：2026-09-10
- 前置阅读：[M01 公共 API](../M01-public-api/README.md)
- 后续阅读：[M03 WAL/MemTable](../M03-wal-memtable/README.md)、[M04 Version](../M04-version-compaction/README.md)

## 一句话说明

M02 是整个数据库实例的控制平面：它持有所有长期资源和锁，把公共 API 调用转换为 WAL/MemTable 操作，把后台任务转换为表构建和 Version 安装。

## 入口与关键字段

`DBImpl` 实现全部 DB 虚接口（[db/db_impl.h:28-71](../../../../db/db_impl.h#L28-L71)）。关键状态包括 `mem_`/`imm_`、`log_`/`logfile_`、writer 队列、snapshots、pending outputs、后台调度标志和 `versions_`（[db/db_impl.h:169-204](../../../../db/db_impl.h#L169-L204)）。

## 核心路径

- 打开：`DB::Open` → 构造 DBImpl → `Recover`。[db/db_impl.cc:1503-1546](../../../../db/db_impl.cc#L1503-L1546)
- 写入：`Write` 分组 writer、写 WAL、插入 MemTable。[db/db_impl.cc:1206-1330](../../../../db/db_impl.cc#L1206-L1330)
- 读取：`Get` 查 MemTable/immutable/Version。[db/db_impl.cc:1121-1166](../../../../db/db_impl.cc#L1121-L1166)
- 后台：`MaybeScheduleCompaction` → `BackgroundCall` → flush/compaction。[db/db_impl.cc:668-1079](../../../../db/db_impl.cc#L668-L1079)
- 关闭：等待后台工作、解锁、释放资源。[db/db_impl.cc:151-178](../../../../db/db_impl.cc#L151-L178)

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
