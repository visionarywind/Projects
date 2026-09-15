# M03 WAL、WriteBatch 与 MemTable

- 文档目的：解释写入记录如何编码、持久化、恢复并进入内存表。
- 适用范围：`db/log_*`、`write_batch*`、`memtable*`、`dbformat*`、`skiplist.h`。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：核心机制已确认。
- 最后更新：2026-09-10
- 前置阅读：[全局数据流](../../00-overview/global-data-flow.md)
- 后续阅读：[M04 Version/Compaction](../M04-version-compaction/README.md)
## 结论摘要

M03 把用户批次转换为带 sequence/type 的 InternalKey，WAL 以可校验的物理记录持久化同一批次，MemTable 使用 Arena + SkipList 保存可排序的内存副本。恢复通过 Reader 重放完整 WriteBatch；MemTable 满后交给 M04 生成 SSTable。

## 边界

- 负责：WriteBatch 编码、日志分片/校验、内部键、内存表、日志恢复输入。
- 不负责：writer 排队和数据库锁（M02）、MANIFEST/层级 compaction（M04）、磁盘表块格式（M05）。

## 关键入口

| 符号 | 位置 | 作用 |
|---|---|---|
| `WriteBatch::Put/Delete/Iterate` | [db/write_batch.cc:34-108](../../../source/leveldb/db/write_batch.cc#L34-L108) | 编辑编码/解析。 |
| `WriteBatchInternal::InsertInto` | [db/write_batch.cc:131-136](../../../source/leveldb/db/write_batch.cc#L131-L136) | 按序写 MemTable。 |
| `log::Writer::AddRecord` | [db/log_writer.cc:33-79](../../../source/leveldb/db/log_writer.cc#L33-L79) | 记录分片。 |
| `log::Reader::ReadRecord` | [db/log_reader.cc:55-173](../../../source/leveldb/db/log_reader.cc#L55-L173) | 组装/校验逻辑记录。 |
| `MemTable::Add/Get` | [db/memtable.cc:75-135](../../../source/leveldb/db/memtable.cc#L75-L135) | 写入/按快照查找。 |
| `LookupKey` | [db/dbformat.h:182-219](../../../source/leveldb/db/dbformat.h#L182-L219) | 构造 memtable/internal 查询键。 |

## 相关文档

- [design](design.md)
- [data-structures](data-structures.md)
- [call-chains](call-chains.md)
- [line-level-analysis](line-level-analysis.md)
- [testing](testing.md)

## 源码证据摘要

见正文及 [M03 source-map](source-map.md)。

## 未解决问题

日志在不同 Env 上的 mmap/append 特性需结合平台实现确认。

## 下一步阅读建议

先读 [data-structures.md](data-structures.md)，再追踪 WriteBatch → WAL → MemTable。

## 文档元数据（规范补充）

- 文档目的：说明 `01-modules/M03-wal-memtable/README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `01-modules/M03-wal-memtable/README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |
