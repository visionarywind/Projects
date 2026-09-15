# M04 行级源码分析

- 文档目的：按逻辑块解释版本提交和压缩选择。
- 适用范围：`version_set.cc`、`version_edit.cc`。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M04 call-chains](call-chains.md)
- 后续阅读：[M04 testing](testing.md)
## 结论摘要

本页聚焦 01-modules/M04-version-compaction/line-level-analysis.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 代码块 | 意图 | 状态/风险 |
|---|---|---|
| `version_set.cc:760-775` | 更新 current 和版本链 | refs 必须正确 |
| `version_set.cc:777-859` | 写 MANIFEST、Sync、安装 | 锁外 IO/失败清理 |
| `version_set.cc:861-993` | 读取 CURRENT/MANIFEST | comparator/损坏传播 |
| `version_set.cc:1031-1067` | 计算 compaction score | 阈值影响后台压力 |
| `version_set.cc:1252-1304` | 选择 size/seek 输入 | level-0 重叠规则 |
| `version_set.cc:1385-1446` | 扩展输入、grandparents、pointer | 写放大/范围边界 |
| `version_set.cc:1499-1559` | trivial move、删除判断、停止输出 | snapshot/旧版本可见性 |

## 调试变量

记录 `current_->compaction_score_`、`compaction_level_`、`compact_pointer_`、输入文件号/范围、`last_sequence_` 和 `pending_outputs_`。

## 相关文档

- [risks](risks-and-debt.md)
- [global-data-flow](../../00-overview/global-data-flow.md)

## 源码证据摘要

见表格。

## 未解决问题

完整的 compaction drop 规则需结合 `DBImpl::DoCompactionWork` 展开。

## 下一步阅读建议

读 M02 的 `DoCompactionWork`。
