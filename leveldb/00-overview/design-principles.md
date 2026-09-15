# 设计原则与取舍

- 文档目的：区分源码确认的设计约束、推断的架构目标和修改代价。
- 适用范围：LevelDB 1.23.0。
- 证据状态：约束已确认；目标/动机部分为推断。
- 最后更新：2026-09-10
- 前置阅读：[总体架构](architecture.md)
- 后续阅读：[模块设计](../01-modules/M04-version-compaction/design.md)

## 结论摘要

LevelDB 以稳定的公共 API、顺序日志、不可变/有序表、版本编辑和后台合并为核心。它牺牲了 SQL 能力、跨进程服务和部分即时一致的空间回收，换取嵌入式部署、顺序写、快照视图和可恢复的持久化状态。

## 原则表

| 原则 | 源码事实 | 代价/影响 |
|---|---|---|
| 公共 API 与内部实现分离 | 公开头文件集中在 `include/leveldb`；内部头文件不保证兼容。 | 修改内部实现通常无需 API 变更，但必须保持磁盘格式和测试。 |
| 日志先行 | `DBImpl::Write` 先 `log_->AddRecord`，成功后插入 MemTable。 | sync=false 的机器崩溃可能丢近期写入；sync=true 增加延迟。 |
| 内部键版本化 | `ValueType` 与序列号编码在 64 位 tag 中，枚举值不可改变。 | 键比较、格式和 compaction 必须保持兼容。 |
| 不可变 SSTable + 分层 compaction | Version 按层保存表，Level-0 允许重叠。 | 后台 IO、写放大和读时合并成本。 |
| Env 抽象外部世界 | 文件、锁、Schedule、线程等均通过虚接口。 | 接口兼容性和平台差异需维护。 |
| 无异常/无 RTTI构建 | CMake 对非 MSVC 禁用 exceptions/RTTI。 | 错误必须通过 Status 和显式资源管理传播。 |

证据：[CMakeLists.txt:55-77](../../source/leveldb/CMakeLists.txt#L55-L77)、[db/dbformat.h:50-66](../../source/leveldb/db/dbformat.h#L50-L66)、[include/leveldb/options.h:166-184](../../source/leveldb/include/leveldb/options.h#L166-L184)。

## 设计取舍说明

- **同步写**：公共文档明确说明 sync=false 类似写入操作，sync=true 类似写入后 fsync；因此可靠性/延迟由调用方选择（[doc/index.md:94-123](../../source/leveldb/doc/index.md#L94-L123)）。
- **快照**：Snapshot 是不可变句柄，允许一致读；但未释放会阻止旧版本/旧数据尽早丢弃（[include/leveldb/db.h:97-105](../../source/leveldb/include/leveldb/db.h#L97-L105)）。
- **过滤器和缓存**：Bloom/filter 可减少磁盘读，block cache 可复用解压后的块；代价是内存占用和 cache 一致性管理。
- **手工生命周期**：MemTable/Version/文件对象多数是裸指针配合引用计数或显式 delete；这是当前 ABI/性能实现，不等同于缺陷，修改时必须遵守所有权注释。

## 修改原则

1. 先判断是否改变公开 API、磁盘格式、线程/锁契约或错误语义。
2. 若枚举值、InternalKey、日志或表格式变化，必须同步更新兼容性测试和格式文档。
3. 功能修改必须伴随测试；项目贡献要求也明确要求测试或解释无需测试（[README.md:89-101](../../source/leveldb/README.md#L89-L101)）。
4. 不把实现中的常量直接写成通用性能保证；用“当前源码默认值”描述。

## 相关文档

- [全局错误模型](global-error-model.md)
- [决策记录](decision-log.md)
- [风险登记](../99-roadmap/risk-register.md)

## 源码证据摘要

见正文各表。

## 未解决问题

作者当年的具体性能目标和每项常数的完整设计历史不在当前源码中；相关解释标为推断。

## 下一步阅读建议

阅读 M03 的内部键与 WAL，再阅读 M04 的 compaction。
