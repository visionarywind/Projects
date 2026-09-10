# 项目概览

- 文档目的：确认项目定位、边界、版本和组成。
- 适用范围：LevelDB `1.23.0` 源码。
- 证据状态：已确认为主，设计动机部分为推断。
- 最后更新：2026-09-10
- 前置阅读：[知识库入口](../README.md)
- 后续阅读：[总体架构](architecture.md)、[运行模型](runtime-model.md)

## 结论摘要

LevelDB 是嵌入式、有序、持久化键值库，不是 SQL 服务，也不提供客户端/服务端协议。一个数据库目录由日志、MemTable、分层 SSTable、MANIFEST、CURRENT、锁和日志文件组成。单进程内同一个 `DB` 可被多线程共享，但同一数据库由操作系统锁限制为单进程打开。

证据：[README.md:1-31](../../../README.md#L1-L31)、[doc/index.md:130-141](../../../doc/index.md#L130-L141)、[doc/impl.md:7-62](../../../doc/impl.md#L7-L62)。

## 输入、输出与边界

| 维度 | 事实 |
|---|---|
| 输入 | 任意字节数组 key/value；可选 comparator、filter policy、Env、压缩和缓存选项。 |
| 输出 | `Status`、字符串值、Iterator、Snapshot、属性/空间估算；持久化文件。 |
| 进程模型 | 库嵌入调用方；一个数据库目录同时只允许一个进程持有锁。 |
| 平台 | CMake 配置 POSIX 或 Windows；C++17，C11 兼容 C API。 |
| 外部依赖 | 可选 crc32c、Snappy、Zstd、tcmalloc、SQLite/Kyoto benchmark；测试依赖 GoogleTest。 |
| 非目标 | SQL、索引、网络服务、跨进程并发访问。 |

证据：[CMakeLists.txt:4-22](../../../CMakeLists.txt#L4-L22)、[CMakeLists.txt:39-53](../../../CMakeLists.txt#L39-L53)、[README.md:29-31](../../../README.md#L29-L31)。

## 仓库组成

- `include/leveldb/`：公开 ABI/API；调用方不应依赖其他头文件。
- `db/`：数据库协调器、格式、日志、恢复、版本、压缩、C API 辅助程序。
- `table/`：Block、SSTable、过滤器、表构建器和迭代器。
- `util/`：Env 实现、缓存、编码、状态、日志、内存等基础设施。
- `port/`：平台能力和可选压缩封装。
- `helpers/memenv/`：内存文件系统，主要用于测试。
- `benchmarks/`、`issues/`：性能入口与回归样例；`third_party/` 为子模块。

证据：[CMakeLists.txt:119-231](../../../CMakeLists.txt#L119-L231)。

## 设计思想（确认与推断分开）

- **已确认**：写入通过日志先行和 MemTable，日志达到阈值后切换并由后台生成 Level-0 表（[doc/impl.md:64-75](../../../doc/impl.md#L64-L75)）。
- **已确认**：Level-0 可重叠，Level-1 及以上通常按 key range 不重叠，compaction 向更高层迁移数据（[doc/impl.md:27-41](../../../doc/impl.md#L27-L41)）。
- **推断**：这种结构把随机写转换为顺序日志和批量合并，代价是后台 IO、读时合并和恢复时间；源码直接证明机制，但“为什么”是架构推断。

## 当前版本和维护约束

目标提交为 `7ee830d`，最近提交信息是 “Bump third_party/ dependencies.”；README 明确说明项目处于非常有限维护状态，主要接受严重 bug 修复和内部支持客户端所需变更（[README.md:1-5](../../../README.md#L1-L5)）。新功能应优先证明兼容性、测试覆盖和必要性。

## 相关文档

- [架构](architecture.md)
- [设计原则](design-principles.md)
- [模块注册表](../01-modules/module-registry.md)

## 源码证据摘要

见正文引用；完整汇总见 [evidence-index.md](evidence-index.md)。

## 未解决问题

本机是否能链接可选压缩库、是否具备所有子模块内容尚未以构建命令验证。

## 下一步阅读建议

阅读 [architecture.md](architecture.md)，再阅读目标模块的 README。
