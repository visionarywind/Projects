# M01 行级分析

- 文档目的：定位公共 API 契约和修改敏感区。
- 适用范围：`include/leveldb/*.h`。
- 源码版本：`main` / `7ee830d`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M01 source-map](source-map.md)
- 后续阅读：[M01 examples](examples.md)

| 代码块 | 语义 | 修改注意 |
|---|---|---|
| `db.h:42-60` | DB 生命周期与 Open | 返回对象/错误必须保持兼容 |
| `db.h:62-87` | 写入和读取 | WriteBatch 原子语义、Status |
| `db.h:89-105` | Iterator/Snapshot | 释放、线程和快照边界 |
| `db.h:107-162` | 属性、压缩、Destroy/Repair | 跨模块副作用 |
| `options.h:32-147` | Options 默认值和借用配置 | comparator/env/cache 生命周期 |
| `options.h:149-184` | Read/WriteOptions | sync/snapshot/filter 行为 |
| `write_batch.h:4-78` | Handler、Append、Iterate | batch 编码与并发契约 |
| `env.h:50-217` | Env 文件/调度边界 | 平台实现需同步 |
| `status.h` | 错误分类和字符串化 | 不要用异常替代 Status |

## 调试重点

记录传入的 `Options`、comparator 名称、snapshot sequence、ReadOptions/WriteOptions、返回 Status，以及 Iterator/Snapshot 的释放点。公共 API 的问题通常需要继续追到 M02 的 `DBImpl`，而不是只看头文件。
