# M01 风险与技术债务

- 文档目的：登记公共契约的错误使用和兼容风险。
- 适用范围：M01。
- 源码版本：`main` / `7ee830d`。
- 证据状态：触发机制已确认，严重度需测试。
- 最后更新：2026-09-10
- 前置阅读：[M01 design](design.md)
- 后续阅读：[项目风险](../../99-roadmap/risk-register.md)

| 风险 | 证据 | 影响 | 验证 |
|---|---|---|---|
| Slice 借用内存过早释放 | `doc/index.md:201-229` | UAF/错误数据 | ASan/生命周期测试 |
| Snapshot 未释放 | `db.h:97-105` | 旧版本无法回收 | compaction 长压 |
| Iterator 与 DB 生命周期脱节 | `db.h:89-95` | UAF | 关闭顺序测试 |
| 虚函数/头文件 ABI 变化 | `db.h`、`export.h` | 客户端重编译/运行失败 | 多编译器 ABI 检查 |
| Options 指针对象不存活 | `options.h` | comparator/env/cache UAF | 自定义对象生命周期 |
| Status 被忽略 | `status.h`、API 文档 | 数据/错误静默丢失 | 错误注入 |
| API 默认值变化 | `options.h` | 性能/格式行为变化 | 配置矩阵 |

## 未解决问题

发布版本间 ABI 兼容范围需要结合官方发布策略和下游实践确认；本知识库只把源码公开契约作为证据。
