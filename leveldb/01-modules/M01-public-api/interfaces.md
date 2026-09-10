# M01 接口与协议

- 文档目的：明确公共函数的参数、返回值、前置/后置条件和同步要求。
- 适用范围：`include/leveldb`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M01 README](README.md)
- 后续阅读：[M02 接口](../M02-db-coordinator/interfaces.md)

## DB 生命周期

`DB::Open(options, name, &dbptr)` 成功时写入堆分配 DB，失败时写入 nullptr；调用者负责 delete。`DB` 不可复制。`NewIterator` 返回堆对象，必须在 DB 删除前销毁。[include/leveldb/db.h:42-60](../../../../include/leveldb/db.h#L42-L60)、[include/leveldb/db.h:89-105](../../../../include/leveldb/db.h#L89-L105)

## 写接口

`Put`、`Delete` 修改单个 key；`Write` 原子应用 WriteBatch。`WriteOptions::sync=true` 要求把文件同步到持久存储，false 允许机器崩溃丢失近期写入。[include/leveldb/db.h:62-77](../../../../include/leveldb/db.h#L62-L77)、[include/leveldb/options.h:166-184](../../../../include/leveldb/options.h#L166-L184)

## 读接口

`Get` 成功写 value；不存在时保留原 value 并返回 `IsNotFound()` 的 Status。`ReadOptions::snapshot` 必须属于此 DB 且未释放；为空时使用本次读开始时的隐式快照。[include/leveldb/db.h:79-87](../../../../include/leveldb/db.h#L79-L87)、[include/leveldb/options.h:149-164](../../../../include/leveldb/options.h#L149-L164)

## 批量与迭代

WriteBatch 的操作按添加顺序应用，`Append` 是 O(source size)；非 const 与 const 并发混用需要外部同步。[include/leveldb/write_batch.h:4-18](../../../../include/leveldb/write_batch.h#L4-L18)、[include/leveldb/write_batch.h:49-72](../../../../include/leveldb/write_batch.h#L49-L72)

## 外部接口 Env

Env 的文件对象、LockFile/UnlockFile、Schedule/StartThread 都是虚接口；所有 Env 实现需支持并发访问，但单个 Sequential/WritableFile 的同步要求由接口注释规定。[include/leveldb/env.h:50-110](../../../../include/leveldb/env.h#L50-L110)、[include/leveldb/env.h:170-217](../../../../include/leveldb/env.h#L170-L217)

## 边界条件

- Slice 不以 `\0` 结尾，允许嵌入零字节；它不拥有数据。
- Comparator 重启数据库时必须名称一致且排序完全一致。
- Snapshot 必须 Release；否则旧状态可能持续存活。
- 公开枚举/持久化编码值不可任意调整。

## 相关文档

- [数据结构](data-structures.md)
- [测试](testing.md)

## 源码证据摘要

见各段引用。

## 未解决问题

ABI 的跨编译器细节超出头文件契约范围。

## 下一步阅读建议

追踪 `DB::Open` 的实现和 `Status` 在 M02 的传播。
