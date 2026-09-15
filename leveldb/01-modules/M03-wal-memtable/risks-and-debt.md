# M03 风险与技术债务

- 文档目的：列出 WAL/MemTable 的正确性、兼容和生命周期风险。
- 适用范围：M03。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：触发条件已确认，严重程度需实验。
- 最后更新：2026-09-10
- 前置阅读：[M03 design](design.md)
- 后续阅读：[项目风险登记](../../99-roadmap/risk-register.md)
## 结论摘要

本页聚焦 01-modules/M03-wal-memtable/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 风险 | 源码证据 | 影响 | 验证 |
|---|---|---|---|
| 持久化常量被改动 | `db/log_format.h:13-29`、`db/dbformat.h:50-66` | 旧数据不可恢复 | 旧格式回归 |
| WAL 尾部处理误改为 corruption | `db/log_reader.cc:143-150` | 进程崩溃后无法打开 | 截断日志测试 |
| Arena 生命周期错误 | `db/memtable.h:40-50` | UAF/泄漏 | ASan |
| sequence/类型排序错误 | `db/dbformat.h:99-116` | snapshot 读错值 | 多版本测试 |
| Batch count 与 records 不一致 | `db/write_batch.cc:74-79` | Corruption 或错误恢复 | malformed batch 测试 |

## 相关文档

- [development-guide](development-guide.md)
- [global-error-model](../../00-overview/global-error-model.md)

## 源码证据摘要

见表格。

## 未解决问题

真实线上故障概率和性能债务需运行数据。

## 下一步阅读建议

进入 M04，观察内部键如何进入 Version/Compaction。
