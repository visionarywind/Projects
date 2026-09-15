# M04 设计

- 文档目的：说明 Version/Manifest/compaction 如何实现持久化状态转换。
- 适用范围：M04。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：机制已确认；目标解释部分推断。
- 最后更新：2026-09-10
- 前置阅读：[M04 README](README.md)
- 后续阅读：[M04 数据结构](data-structures.md)
## 结论摘要

Version 是不可变的逻辑视图；VersionEdit 描述新增/删除文件和序列元数据；LogAndApply 先构造新 Version，再锁外写 MANIFEST，成功后安装 current。这把昂贵文件 IO 与短暂元数据锁分开，并让失败时旧 Version 继续服务。

## 分层规则

Level-0 文件可能重叠；高层文件通常范围不重叠。`PickCompaction` 优先 size score，其次 seek score；Level-0 收集全部重叠输入，随后选择下一层和 grandparents。[db/version_set.cc:1252-1303](../../../source/leveldb/db/version_set.cc#L1252-L1303)、[doc/impl.md:76-105](../../../source/leveldb/doc/impl.md#L76-L105)

## MANIFEST 原子性

`LogAndApply` 写 VersionEdit 并 Sync，首次建立 descriptor 后更新 CURRENT；成功才 `AppendVersion`，失败删除临时 manifest 并保留旧状态。[db/version_set.cc:800-858](../../../source/leveldb/db/version_set.cc#L800-L858)

## 取舍

版本链保护迭代器的一致性，但旧 Version/文件不能立刻回收；compaction 降低读放大，却引入写放大、后台 IO 和输出安装错误路径。

## 相关文档

- [interfaces](interfaces.md)
- [risks](risks-and-debt.md)

## 源码证据摘要

见正文引用。

## 未解决问题

MANIFEST reuse_logs 的所有运行分支需结合 Options 和测试补充。

## 下一步阅读建议

阅读 `VersionSet::Recover` 的 Builder.Apply。
