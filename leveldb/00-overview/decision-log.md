# 设计决策记录

- 文档目的：记录源码明确的约束、取舍和需要实验验证的判断。
- 适用范围：目标提交 `7ee830d`。
- 证据状态：机制已确认，设计动机部分标为推断。
- 最后更新：2026-09-10
- 前置阅读：[设计原则](design-principles.md)
- 后续阅读：[风险登记](../99-roadmap/risk-register.md)

| ID | 决策/约束 | 状态 | 证据 | 影响 |
|---|---|---|---|---|
| D01 | 用 WAL + MemTable 接收新写入，再异步生成有序表 | 已确认 | [doc/impl.md:12-19](../../../doc/impl.md#L12-L19) | 写入顺序化，恢复依赖 WAL |
| D02 | 用 MANIFEST/CURRENT 持久化表集合和当前版本 | 已确认 | [doc/impl.md:43-55](../../../doc/impl.md#L43-L55) | 打开需先恢复 descriptor |
| D03 | Level-0 可重叠，高层按范围组织并 compaction | 已确认 | [doc/impl.md:27-41](../../../doc/impl.md#L27-L41) | 读路径合并；压缩减少读放大 |
| D04 | Env 隔离文件、锁和调度 | 已确认 | [include/leveldb/env.h:50-64](../../../include/leveldb/env.h#L50-L64) | 支持 POSIX/Windows/测试替身 |
| D05 | 禁用 C++ exception 和 RTTI | 已确认 | [CMakeLists.txt:55-77](../../../CMakeLists.txt#L55-L77) | 必须使用 Status/显式生命周期 |
| D06 | InternalKey 的枚举值属于磁盘格式，不能随意调整 | 已确认 | [db/dbformat.h:50-66](../../../db/dbformat.h#L50-L66) | 格式变更需兼容策略 |
| D07 | 当前项目有限维护 | 已确认 | [README.md:1-5](../../../README.md#L1-L5) | 新功能需证明必要性和兼容性 |
| D08 | 具体性能收益与每个常数的历史动机 | 未知/推断 | 当前源码和文档不足 | 需 benchmark/历史提交确认 |

## 相关文档

- [风险和债务](../99-roadmap/risk-register.md)
- [分析状态](analysis-state.md)

## 源码证据摘要

见表格。

## 未解决问题

没有将推断的“写放大/读放大取舍”冒充作者明确决策。

## 下一步阅读建议

修改格式或并发模型前先检查 D02、D03、D06。
