# 运行模型

- 文档目的：描述打开、运行、后台工作和关闭的生命周期。
- 适用范围：LevelDB `1.23.0`。
- 证据状态：主流程已确认；平台线程数为待验证细节。
- 最后更新：2026-09-10
- 前置阅读：[总体架构](architecture.md)
- 后续阅读：[端到端流程](../90-cross-module/end-to-end-flows.md)

## 结论摘要

`DB::Open` 创建 `DBImpl` 后执行恢复：创建/锁定目录，读取 CURRENT 指向的 MANIFEST，恢复 Version，重放符合条件的日志，必要时把恢复出的 MemTable 写为 Level-0 表，随后开始接收写入。运行中写线程与后台 compaction 通过 `mutex_`、条件变量、writer 队列和 Env 调度协调。析构先等待后台任务，再释放文件和版本资源。

## 生命周期图

```mermaid
stateDiagram-v2
  [*] --> Constructed: new DBImpl
  Constructed --> Recovering: DB::Open / Recover
  Recovering --> Running: lock + Version + WAL replay
  Running --> BackgroundWork: MemTable/compaction threshold
  BackgroundWork --> Running: install VersionEdit
  Running --> ShuttingDown: destructor
  ShuttingDown --> [*]: wait + unlock + release
```

## 启动与恢复

- 新库由 `NewDB` 写初始 MANIFEST、Sync/Close，再更新 CURRENT（[db/db_impl.cc:180-212](../../source/leveldb/db/db_impl.cc#L180-L212)）。
- `Recover` 创建目录、获取 `LOCK`，判断 create/error-if-exists，调用 `versions_->Recover`，再扫描并重放日志（[db/db_impl.cc:291-383](../../source/leveldb/db/db_impl.cc#L291-L383)）。
- `RecoverLogFile` 使用 `log::Reader::ReadRecord` 读取完整 WriteBatch，插入 MemTable；必要时写出 Level-0 表（[db/db_impl.cc:385-504](../../source/leveldb/db/db_impl.cc#L385-L504)）。

## 运行与后台

- 写入者按队列分组，批量共享一次日志写入；每个 writer 通过条件变量等待完成（`DBImpl::Writer`，[db/db_impl.cc:41-51](../../source/leveldb/db/db_impl.cc#L41-L51)）。
- MemTable 达到 `write_buffer_size` 后切换到 `imm_`；`MaybeScheduleCompaction` 通过 `Env::Schedule` 安排工作（[db/db_impl.cc:668-685](../../source/leveldb/db/db_impl.cc#L668-L685)）。
- `BackgroundCall` 获取锁执行后台压缩/flush，结束后更新调度状态并通知等待者（[db/db_impl.cc:689-707](../../source/leveldb/db/db_impl.cc#L689-L707)）。

## 关闭

析构把 `shutting_down_` 置为 true，等待 `background_compaction_scheduled_` 清零，然后解锁数据库、删除版本/内存表/日志/缓存，并释放由 DB 自己创建的 info log/block cache（[db/db_impl.cc:151-178](../../source/leveldb/db/db_impl.cc#L151-L178)）。调用者必须先删除 Iterator，再删除 DB（[include/leveldb/db.h:89-95](../../source/leveldb/include/leveldb/db.h#L89-L95)）。

## 相关文档

- [全局数据流](global-data-flow.md)
- [错误模型](global-error-model.md)
- [M02 模块](../01-modules/M02-db-coordinator/README.md)

## 源码证据摘要

见正文引用。

## 未解决问题

Env 的后台线程是否常驻、队列如何复用需结合平台实现和运行日志确认。

## 下一步阅读建议

沿 `Recover` 和 `Write` 两条链阅读 M02。
