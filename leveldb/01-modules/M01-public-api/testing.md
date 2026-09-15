# M01 测试分析补充

- 文档目的：验证公共 API 的语义、所有权和错误边界。
- 适用范围：M01。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：测试入口已确认；执行未验证。
- 最后更新：2026-09-10
- 前置阅读：[M01 examples](examples.md)
- 后续阅读：[M07 测试](../M07-tests-build/testing.md)
## 结论摘要

本页聚焦 01-modules/M01-public-api/testing.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 测试矩阵

| 主题 | 入口 |
|---|---|
| Open/Put/Get/Delete/Iterator/Snapshot | `db/db_test.cc` |
| C API | `db/c_test.c` |
| WriteBatch | `db/write_batch_test.cc` |
| Status/Slice 基础 | `util/status_test.cc`、各 DB 测试 |
| Destroy/Repair/重开 | `db/recovery_test.cc`、`db/corruption_test.cc` |

## 回归要求

API 改动至少覆盖成功和错误 Status；涉及 Snapshot/Iterator 必须覆盖释放顺序；涉及 Options 默认值必须覆盖显式配置和默认配置；涉及 C ABI 必须构建独立 `c_test`。当前 CMake/CTest 是否成功需实际运行确认。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
