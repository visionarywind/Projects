# M01 测试分析补充

- 文档目的：验证公共 API 的语义、所有权和错误边界。
- 适用范围：M01。
- 源码版本：`main` / `7ee830d`。
- 证据状态：测试入口已确认；执行未验证。
- 最后更新：2026-09-10
- 前置阅读：[M01 examples](examples.md)
- 后续阅读：[M07 测试](../M07-tests-build/testing.md)

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
