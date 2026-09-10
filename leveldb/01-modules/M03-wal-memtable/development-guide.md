# M03 开发指南与风险

- 文档目的：提供修改 WAL/MemTable 的安全步骤。
- 适用范围：M03。
- 证据状态：规则来自源码约束和测试结构。
- 最后更新：2026-09-10
- 前置阅读：[M03 line-level](line-level-analysis.md)
- 后续阅读：[M04 development](../M04-version-compaction/development-guide.md)

## 修改配方

1. 判断变更是内存布局、持久化编码、错误容忍还是排序语义。
2. 先补最小格式/行为测试；保存一份旧格式输入。
3. 同步修改 writer/reader、WriteBatch 或 comparator 的双端逻辑。
4. 运行 log、write_batch、dbformat、recovery 和全量 CTest。
5. 评估旧 DB 可打开性、sequence 可见性、Slice 生命周期和内存增长。

## 风险登记

| 风险 | 证据 | 后果 | 建议 |
|---|---|---|---|
| 改变 ValueType 值 | [db/dbformat.h:50-53](../../../../db/dbformat.h#L50-L53) | 旧表/WAL 误解析 | 禁止重排，补兼容测试 |
| Writer/Reader 分片不对称 | [db/log_writer.cc:37-77](../../../../db/log_writer.cc#L37-L77) | 恢复丢批次或报损坏 | 边界长度测试 |
| Slice 指向 Arena | [db/memtable.h:44-50](../../../../db/memtable.h#L44-L50) | UAF | 明确引用生命周期 |
| 忽略 batch corruption | [db/write_batch.cc:41-79](../../../../db/write_batch.cc#L41-L79) | 恢复错误数据 | Status 必须传播 |

## 相关文档

- [risks-and-debt](risks-and-debt.md)
- [M03 interfaces](interfaces.md)

## 源码证据摘要

见风险表。

## 未解决问题

长 key/value 的极限行为需专项测试。

## 下一步阅读建议

先改测试，再改编码逻辑。
