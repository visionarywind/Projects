# M04 开发指南与风险

- 文档目的：指导修改版本持久化和 compaction。
- 适用范围：M04。
- 证据状态：源码约束已确认。
- 最后更新：2026-09-10
- 前置阅读：[M04 line-level](line-level-analysis.md)
- 后续阅读：[跨模块影响](../../90-cross-module/change-impact-map.md)

## 修改步骤

1. 明确是否改变 MANIFEST 格式、文件集合、版本可见性或选压缩策略。
2. 先补单元测试，再补重开/损坏/并发测试。
3. 保持 LogAndApply 的锁外写和失败回滚；确保输入 Version Ref 和输出 pending 保护。
4. 检查 M02 的输出安装、M05 的表读取和 M06 的文件删除。
5. 运行全量 CTest 和 benchmark（若可用）。

## 风险

| 风险 | 证据 | 后果 |
|---|---|---|
| MANIFEST Sync 后安装失败 | `version_set.cc:816-858` | 旧/新状态处理错误 |
| Version refs 错误 | `version_set.cc:760-775` | iterator UAF |
| boundary file 漏选 | `version_set.cc:1346-1383` | 读到错误版本 |
| live file 集合不完整 | `db_impl.cc:224-278` | 数据丢失 |

## 相关文档

- [risks-and-debt](risks-and-debt.md)
- [M02 development](../M02-db-coordinator/development-guide.md)

## 源码证据摘要

见表格。

## 未解决问题

生产 compaction backlog 指标需要真实运行采集。

## 下一步阅读建议

先用 version_set_test 建立局部反馈。
