# M02 风险与技术债务

- 文档目的：列出 DB 协调器中可由源码证明的高影响风险。
- 适用范围：M02。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：风险触发机制已确认，实际发生需实验。
- 最后更新：2026-09-10
- 前置阅读：[M02 design](design.md)
- 后续阅读：[项目风险登记](../../99-roadmap/risk-register.md)
## 结论摘要

本页聚焦 01-modules/M02-db-coordinator/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 风险 | 源码证据 | 影响 | 触发 | 验证/建议 |
|---|---|---|---|---|
| 后台任务与析构交错 | [db/db_impl.cc:151-158](../../../source/leveldb/db/db_impl.cc#L151-L158) | 死锁或 UAF | 关闭时有 compaction | TSan、反复 close |
| 锁外删除文件 | [db/db_impl.cc:281-289](../../../source/leveldb/db/db_impl.cc#L281-L289) | 文件状态竞态 | 并发读/写 | 故障注入 |
| bg_error 阻塞回收 | [db/db_impl.cc:224-231](../../../source/leveldb/db/db_impl.cc#L224-L231) | 磁盘空间增长 | 后台 IO 错误 | 长压/磁盘满测试 |
| writer 队列等待 | [db/db_impl.cc:41-51](../../../source/leveldb/db/db_impl.cc#L41-L51) | 延迟尖峰 | 写入受限/compaction | 并发基准 |
| 裸指针和引用计数 | [db/db_impl.h:176-204](../../../source/leveldb/db/db_impl.h#L176-L204) | 生命周期错误 | 异常路径 | ASan/代码审查 |

## 相关文档

- [development-guide](development-guide.md)
- [global error model](../../00-overview/global-error-model.md)

## 源码证据摘要

见表格。

## 未解决问题

严重程度需结合生产负载和实际故障注入量化。

## 下一步阅读建议

检查最新历史提交中的并发 bug 修复是否改变这些结论。
