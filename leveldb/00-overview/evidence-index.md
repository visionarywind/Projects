# 证据索引

- 文档目的：集中列出关键结论的源码证据。
- 适用范围：目标提交 `7ee830d`。
- 证据状态：已建立第一版，后续模块分析继续补充。
- 最后更新：2026-09-10
- 前置阅读：[项目概览](project-overview.md)
- 后续阅读：[分析状态](analysis-state.md)

| 结论 | 证据 |
|---|---|
| 项目是嵌入式有序键值库，非 SQL/服务 | [README.md:1-31](../../source/leveldb/README.md#L1-L31) |
| 公共 API 集中在 include/leveldb | [README.md:207-214](../../source/leveldb/README.md#L207-L214) |
| 项目版本 1.23.0，要求 C++17 | [CMakeLists.txt:4-22](../../source/leveldb/CMakeLists.txt#L4-L22) |
| 默认打开测试/benchmark/install | [CMakeLists.txt:32-34](../../source/leveldb/CMakeLists.txt#L32-L34) |
| 核心库源文件组成 | [CMakeLists.txt:119-231](../../source/leveldb/CMakeLists.txt#L119-L231) |
| 测试目标和 CTest 注册 | [CMakeLists.txt:292-405](../../source/leveldb/CMakeLists.txt#L292-L405) |
| benchmark 入口 | [CMakeLists.txt:407-469](../../source/leveldb/CMakeLists.txt#L407-L469) |
| WAL、SSTable、Manifest、CURRENT 文件模型 | [doc/impl.md:7-62](../../source/leveldb/doc/impl.md#L7-L62) |
| Level-0/分层 compaction | [doc/impl.md:64-105](../../source/leveldb/doc/impl.md#L64-L105) |
| DB 公共接口与所有权 | [include/leveldb/db.h:42-162](../../source/leveldb/include/leveldb/db.h#L42-L162) |
| DBImpl 运行状态字段 | [db/db_impl.h:157-204](../../source/leveldb/db/db_impl.h#L157-L204) |
| 打开与恢复 | [db/db_impl.cc:180-504](../../source/leveldb/db/db_impl.cc#L180-L504) |
| 后台工作与 compaction | [db/db_impl.cc:668-1079](../../source/leveldb/db/db_impl.cc#L668-L1079) |
| 写入与写者队列 | [db/db_impl.cc:1206-1330](../../source/leveldb/db/db_impl.cc#L1206-L1330) |
| InternalKey 编码约束 | [db/dbformat.h:50-180](../../source/leveldb/db/dbformat.h#L50-L180) |
| WAL 物理记录 | [db/log_writer.cc:33-107](../../source/leveldb/db/log_writer.cc#L33-L107) |
| WAL 恢复/损坏处理 | [db/log_reader.cc:55-255](../../source/leveldb/db/log_reader.cc#L55-L255) |
| MemTable 引用计数和 API | [db/memtable.h:19-82](../../source/leveldb/db/memtable.h#L19-L82) |
| Version/VersionSet | [db/version_set.h:59-315](../../source/leveldb/db/version_set.h#L59-L315) |
| VersionEdit 文件元数据 | [db/version_edit.h:17-101](../../source/leveldb/db/version_edit.h#L17-L101) |
| TableBuilder 块/过滤器/footer | [table/table_builder.cc:93-267](../../source/leveldb/table/table_builder.cc#L93-L267) |
| BlockHandle/Footer/table 读取格式 | [table/format.h:20-89](../../source/leveldb/table/format.h#L20-L89) |
| Env 文件/锁/调度边界 | [include/leveldb/env.h:50-217](../../source/leveldb/include/leveldb/env.h#L50-L217) |
| Table/Block/Filter 读取实现 | [table/table.cc:37-240](../../source/leveldb/table/table.cc#L37-L240)、[table/block.cc:24-218](../../source/leveldb/table/block.cc#L24-L218)、[table/filter_block.cc:20-103](../../source/leveldb/table/filter_block.cc#L20-L103) |
| Cache handle 与 Arena 资源契约 | [include/leveldb/cache.h:33-97](../../source/leveldb/include/leveldb/cache.h#L33-L97)、[util/arena.h:15-66](../../source/leveldb/util/arena.h#L15-L66) |
| Arena 分块与批量释放 | `source/leveldb/util/arena.h:16-65`、`source/leveldb/util/arena.cc:9-64` |
| LRU handle/淘汰/分片 | `source/leveldb/util/cache.cc:17-399` |
| TableCache file/Table 所有权 | `source/leveldb/db/table_cache.cc:13-117` |
| block cache 与 iterator cleanup | `source/leveldb/table/table.cc:145-203` |
| DB 默认 cache 与析构顺序 | `source/leveldb/db/db_impl.cc:100-178` |
| pending output 防删除协议 | `source/leveldb/db/db_impl.cc:230-285,501-535,789-835` |
| Get/Iterator 引用闭环 | `source/leveldb/db/db_impl.cc:1063-1166` |
| CI 矩阵和命令 | [.github/workflows/build.yml:10-102](../../source/leveldb/.github/workflows/build.yml#L10-L102) |
| 测试、benchmark、install 构建边界 | [CMakeLists.txt:292-519](../../source/leveldb/CMakeLists.txt#L292-L519) |
| 本机 Linux Debug 构建与测试证据 | [D01 构建与运行](../80-demos/D01-dbtest-lifecycle/build-and-run.md) |
| D01 真实测试入口与覆盖矩阵 | [D01 注册表](../80-demos/demo-registry.md) |
| 跨模块端到端流程总览 | [90-cross-module/end-to-end-flows.md](../90-cross-module/end-to-end-flows.md) |
| 八条端到端深度链路（Open/恢复、Write、Get、Iterator、flush、compaction、错误/关闭） | [90-cross-module/end-to-end-traces.md](../90-cross-module/end-to-end-traces.md) |
| 配置影响、错误边界和性能路径 | [90-cross-module/configuration-impact-map.md](../90-cross-module/configuration-impact-map.md)、[90-cross-module/error-boundaries.md](../90-cross-module/error-boundaries.md)、[90-cross-module/performance-critical-paths.md](../90-cross-module/performance-critical-paths.md) |
| 构建/调试/测试实践入口 | [99-roadmap/quick-start.md](../99-roadmap/quick-start.md)、[99-roadmap/debugging-guide.md](../99-roadmap/debugging-guide.md)、[99-roadmap/testing-recipes.md](../99-roadmap/testing-recipes.md) |

## 相关文档

- [总体架构](architecture.md)
- [各模块 source-map](../01-modules/)

## 源码证据摘要

表格即摘要；模块文档补充函数级证据。

## 未解决问题

证据行号会随源码变更漂移；更新目标提交时必须重新核对。

## 下一步阅读建议

用本表从结论反查源码，再进入对应模块。
