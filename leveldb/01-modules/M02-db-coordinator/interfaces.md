# M02 接口

- 文档目的：记录 DBImpl 对 M01 的实现语义和内部协作接口。
- 适用范围：`DBImpl`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M02 design](design.md)
- 后续阅读：[M03 interfaces](../M03-wal-memtable/interfaces.md)

## 公共实现

`Put` 和 `Delete` 构造临时 WriteBatch 后调用 `Write`（[db/db_impl.cc:1198-1204](../../../../db/db_impl.cc#L1198-L1204)）；`Write` 返回日志、MemTable 和后台错误的组合 Status。`Get`/Iterator 的 snapshot 必须在 DB 生命周期内有效。

## 内部协作

- `Recover(VersionEdit*, bool*)` 要求持有 mutex，读取目录、MANIFEST 和日志。
- `WriteLevel0Table` 把 MemTable 转为表并填充 VersionEdit。
- `MaybeScheduleCompaction` 只负责安排一次背景工作；`BackgroundCall` 执行并恢复标志。
- `InstallCompactionResults` 安装新文件并删除旧文件的元数据。

这些前置条件写在 [db/db_impl.h:107-151](../../../../db/db_impl.h#L107-L151)。

## 错误语义

Open 失败时不返回 DB 指针；后台错误存入 `bg_error_`，后续路径检查；非 paranoid 模式下特定恢复错误可被记录后忽略。所有调用者都应检查 Status。

## 相关文档

- [global error model](../../00-overview/global-error-model.md)
- [call-chains](call-chains.md)

## 源码证据摘要

见正文引用。

## 未解决问题

完整的每个锁前置条件需要逐函数核对线程注解和实现。

## 下一步阅读建议

读 `db_impl.h` 的 `EXCLUSIVE_LOCKS_REQUIRED` 注释。
