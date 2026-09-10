# M03 示例

- 文档目的：用最小场景把 WriteBatch、WAL、恢复和 MemTable 连接起来。
- 适用范围：M03。
- 源码版本：`main` / `7ee830d`。
- 证据状态：核心调用入口和数据流来自源码；Linux Debug 聚合构建与 CTest 已通过，但本页的大 value WAL 截断/损坏实验和独立 filter 尚未执行。
- 最后更新：2026-09-10
- 前置阅读：[M03 README](README.md)
- 后续阅读：[M03 testing](testing.md)

## 批量写入与可见性

创建 `WriteBatch`，加入多个 `Put`/`Delete`，通过 `DB::Write` 提交；数据库先将批次编码到 WAL，再由 `WriteBatchInternal::InsertInto` 按同一 sequence 范围写入 MemTable。对应实现入口为 [`DBImpl::Write`](../../../../db/db_impl.cc#L1206-L1330)、[`WriteBatchInternal::InsertInto`](../../../../db/write_batch.cc#L131-L136) 和 [`MemTable::Add`](../../../../db/memtable.cc#L75-L135)。

验证时应同时检查批次中多个 key 的结果、删除标记的可见性以及返回的 `Status`，不要只断言单个最终值。

## WAL 边界与恢复

使用足够大的 value 跨越 32 KiB 日志块，验证 Writer 产生 fragmented record，Reader 在重开时重新组装完整批次；可结合故障注入或截断尾部检查恢复对不完整尾记录的处理。对应入口为 [`log::Writer::AddRecord`](../../../../db/log_writer.cc#L33-L107)、[`log::Reader::ReadRecord`](../../../../db/log_reader.cc#L55-L255) 和 [`DBImpl::RecoverLogFile`](../../../../db/db_impl.cc#L385-L504)。

测试应区分合法的尾部截断与中间损坏：前者可能被当作不完整尾记录处理，后者应传播 corruption 或相应恢复错误。

## 快照读取

在写入旧值后取得 Snapshot，再写入新值，使用 snapshot 创建 iterator 或执行 `Get`，验证 `LookupKey` 使用的 sequence 限制读到旧版本；释放 Snapshot 后再检查当前值。关键编码入口为 [`LookupKey`](../../../../db/dbformat.h#L182-L219) 和 [`MemTable::Get`](../../../../db/memtable.cc#L75-L135)。

Snapshot 的生命周期由 DB 管理：调用者使用完毕后必须通过 `ReleaseSnapshot` 归还，不能把借用的 Slice 脱离其底层对象长期保存。

## 资源边界

用 MemTable iterator 逐步读取并在 owner 释放前完成访问；若修改 Arena 或引用计数，加入 iterator、reopen 和 ASan 场景，确保 MemTable 的 `Ref/Unref` 与 Arena 批量释放保持匹配。
