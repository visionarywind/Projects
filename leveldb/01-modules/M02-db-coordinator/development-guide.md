# M02 开发指南与风险

- 文档目的：给出修改 DBImpl 的最短安全路径。
- 适用范围：M02。
- 证据状态：步骤基于源码和贡献规则；未执行。
- 最后更新：2026-09-10
- 前置阅读：[M02 line-level](line-level-analysis.md)
- 后续阅读：[跨模块影响](../../90-cross-module/change-impact-map.md)

## 修改步骤

1. 确定入口是恢复、写入、读取、后台还是析构。
2. 读取 `db_impl.h` 的字段注释和 lock annotation。
3. 画出资源/锁/Status 变化，先写回归测试。
4. 修改实现，保持 WAL→MemTable→VersionEdit 顺序和 pending output 保护。
5. 运行格式化、构建、CTest；若改持久化格式，再补重开/损坏测试。

## 风险表

| 风险 | 证据 | 影响 | 建议验证 |
|---|---|---|---|
| 后台关闭竞态 | [db/db_impl.cc:151-158](../../../source/leveldb/db/db_impl.cc#L151-L158) | UAF/死锁 | 重复关闭、TSan |
| 错误后删除活文件 | [db/db_impl.cc:224-231](../../../source/leveldb/db/db_impl.cc#L224-L231) | 数据损坏 | 故障注入 |
| 手工 MemTable 引用 | [db/memtable.h:19-38](../../../source/leveldb/db/memtable.h#L19-L38) | 泄漏/UAF | ASan/引用断言 |
| 改 writer 顺序 | [db/db_impl.cc:1206-1330](../../../source/leveldb/db/db_impl.cc#L1206-L1330) | 原子性/公平性变化 | 多线程回归 |

## 相关文档

- [testing](testing.md)
- [risks-and-debt](risks-and-debt.md)

## 源码证据摘要

见风险表。

## 未解决问题

性能回归基线需运行 benchmark 建立。

## 下一步阅读建议

修改前先阅读 M03 和 M04 的接口。
