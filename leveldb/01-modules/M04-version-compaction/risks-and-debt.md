# M04 风险与技术债务

- 文档目的：登记版本和压缩的正确性/稳定性风险。
- 适用范围：M04。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：触发机制已确认，实际严重度待实验。
- 最后更新：2026-09-10
- 前置阅读：[M04 design](design.md)
- 后续阅读：[项目风险](../../99-roadmap/risk-register.md)
## 结论摘要

本页聚焦 01-modules/M04-version-compaction/risks-and-debt.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 风险 | 证据 | 影响 | 验证 |
|---|---|---|---|
| MANIFEST/Current 更新失败 | `version_set.cc:816-858` | 无法恢复或状态不一致 | 故障注入/重开 |
| Version/Compaction 引用错误 | `version_set.h:88-91` | UAF/泄漏 | ASan/iterator 压测 |
| 边界表漏选 | `version_set.cc:1346-1383` | 读结果错误 | boundary regression |
| 后台输出误删 | `db_impl.cc:233-278` | 数据丢失 | 并发 compaction |
| 旧删除标记清理错误 | `DBImpl::DoCompactionWork` | resurrect/误删 | snapshot 多版本测试 |

## 相关文档

- [development-guide](development-guide.md)
- [global-error-model](../../00-overview/global-error-model.md)

## 源码证据摘要

见表格。

## 未解决问题

需要历史提交和 fuzz/故障注入进一步确认边界。

## 下一步阅读建议

阅读 M02 compaction 执行细节。
