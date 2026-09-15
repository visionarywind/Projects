# M01 设计

- 文档目的：解释公共 API 的边界与稳定性取舍。
- 适用范围：`include/leveldb`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M01 README](README.md)
- 后续阅读：[M02 设计](../M02-db-coordinator/design.md)

## 结论摘要

M01 以抽象类和轻量值类型隐藏存储实现，同时把必须由调用方负责的生命周期、线程同步和持久化兼容条件写在头文件注释中。错误通过 Status 显式返回，适配无异常构建。

## 关键取舍

- 虚接口让内部实现可替换，但 API/ABI 更难随意变更。
- Slice 避免拷贝，但借用外部内存，需要调用方保证存活。
- Snapshot 给出稳定视图，但长期持有增加旧版本保留压力。
- Env 把平台能力注入 Options，便于测试和平台移植，但接口契约必须长期维护。

证据：[include/leveldb/env.h:4-10](../../../source/leveldb/include/leveldb/env.h#L4-L10)、[include/leveldb/slice.h](../../../source/leveldb/include/leveldb/slice.h)。

## 相关文档

- [接口](interfaces.md)
- [数据结构](data-structures.md)

## 源码证据摘要

见正文和 M01 README。

## 未解决问题

历史设计动机部分只能从注释和行为推断。

## 下一步阅读建议

进入 M02 查看这些契约如何落地。
