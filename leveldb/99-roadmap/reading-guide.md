# 阅读指南

- 文档目的：按目标给出 LevelDB 的渐进阅读路径，并把模块知识连接到真实的端到端执行轨迹。
- 适用范围：知识库全部文档。
- 源码版本：`main` / `7ee830d`。
- 证据状态：路径根据模块依赖、源码入口和端到端深度链路整理；运行时调度和构建结果未验证。
- 最后更新：2026-09-10
- 前置阅读：[快速开始](quick-start.md)
- 后续阅读：[调试指南](debugging-guide.md)

## 30 分钟路径

1. [项目概览](../00-overview/project-overview.md)
2. [总体架构](../00-overview/architecture.md)
3. [端到端深度链路的贯穿示例](../90-cross-module/end-to-end-traces.md#一条贯穿示例k-从写入到重开)
4. [M01 README](../01-modules/M01-public-api/README.md)
5. [M02 call-chains](../01-modules/M02-db-coordinator/call-chains.md)
6. [M03 design](../01-modules/M03-wal-memtable/design.md)
7. [M04 README](../01-modules/M04-version-compaction/README.md)
8. [M05 call-chains](../01-modules/M05-sstable-table/call-chains.md)

## 半天：完整控制/数据/错误/资源链

按以下顺序阅读深度页，而不是只看箭头摘要：

1. [链 1：首次打开与创建](../90-cross-module/end-to-end-traces.md#链-1首次打开与创建数据库)：理解 `DB::Open`、LOCK、CURRENT、MANIFEST、WAL 和 MemTable 初始化。
2. [链 2：已有数据库重开与崩溃恢复](../90-cross-module/end-to-end-traces.md#链-2已有数据库重开与崩溃恢复)：理解 VersionEdit 重建、WAL 重放和 sequence 恢复。
3. [链 3：并发批量写入](../90-cross-module/end-to-end-traces.md#链-3并发批量写入限流与日志先行)：理解 writer queue、batch group、sync 和日志先行。
4. [链 4：单键读取](../90-cross-module/end-to-end-traces.md#链-4单键读取从快照序号到-table-block)：理解 Snapshot、MemTable、Version、TableCache 和 Block。
5. [链 5：Iterator 与快照](../90-cross-module/end-to-end-traces.md#链-5iterator-与快照的长生命周期读取)：理解 merging iterator、Slice 借用和 cleanup。
6. [链 6–7：flush 与 compaction](../90-cross-module/end-to-end-traces.md#链-6memtable-切换immutable-flush-与-level-0-安装)：理解文件生成与 Version 安装不是同一时刻。
7. [链 8：错误、关闭和资源](../90-cross-module/end-to-end-traces.md#链-8错误关闭和资源生命周期)：把 Status、`bg_error_`、后台 CondVar、引用和句柄串起来。

每一条链都应同时打开对应的 M02/M03/M04/M05/M06 `implementation`、`call-chains`、`examples` 和 `testing` 文档；端到端页保留全局视角，模块页解释局部实现。

## 存储引擎路径

M03 WAL/InternalKey/MemTable → M04 Version/Compaction → M05 SSTable/Block → M02 DBImpl 协调。适合修改恢复、读路径、压缩和文件格式；建议先读[链 3–7](../90-cross-module/end-to-end-traces.md#链-3并发批量写入限流与日志先行)。

## 平台路径

M06 Env/interfaces → POSIX/Windows → Cache/Arena → M02 调度与锁 → M07 平台测试。适合文件系统、线程、故障注入和资源问题；重点对照[链 4、6、8](../90-cross-module/end-to-end-traces.md#链-4单键读取从快照序号到-table-block)和 [M06 examples](../01-modules/M06-env-platform/examples.md)。

## API/应用路径

M01 `db.h/options.h` → `doc/index.md` → M02 `DBImpl::Open/Write/Get` → M07 `db_test`/`c_test`。再用[端到端贯穿示例](../90-cross-module/end-to-end-traces.md#一条贯穿示例k-从写入到重开)查看一次完整生命周期。

## 测试/工程路径

M07 README → CMake targets → CTest 注册 → 对应模块 testing → `db_bench` → CI。Linux Debug configure/build、CTest 和有限 benchmark 已实际验证；D01 filter 独立记录、Release/Sanitizer、fault injection、跨平台和生产性能基线仍未验证。不要把下列命令之外的实验当作已运行结果。

## 一天准备首次修改

1. 阅读 [分析状态](../00-overview/analysis-state.md) 和 [证据索引](../00-overview/evidence-index.md)。
2. 选一条端到端链，画出输入、输出、锁、所有权和 Status 边界。
3. 进入目标模块的 `line-level-analysis.md` 与 `development-guide.md`。
4. 反向查对应测试入口和 `change-impact-map.md`。
5. 修改前先补失败/清理路径的测试设计；构建依赖未准备好时仅做静态检查。

## 十个源码入口

1. `include/leveldb/db.h:DB::Open`
2. `db/db_impl.cc:DBImpl::Recover`
3. `db/db_impl.cc:DBImpl::Write`
4. `db/db_impl.cc:DBImpl::Get`
5. `db/log_writer.cc:Writer::AddRecord`
6. `db/memtable.cc:MemTable::Add`
7. `db/version_set.cc:VersionSet::LogAndApply`
8. `db/version_set.cc:VersionSet::PickCompaction`
9. `table/table_builder.cc:TableBuilder::Finish`
10. `table/table.cc:Table::Open`

## 读代码方法

每次追踪都记录：调用线程/锁、输入数据表示、输出所有权、Status 错误、持久化副作用和测试入口。源码明确的机制与历史动机推断分开；遇到 `Env::Schedule` 时只记录“异步排队”，不要假定固定线程数或执行顺序。

## 相关文档

- [端到端流程总览](../90-cross-module/end-to-end-flows.md)
- [端到端深度链路](../90-cross-module/end-to-end-traces.md)
- [证据索引](../00-overview/evidence-index.md)
- [修改影响地图](../90-cross-module/change-impact-map.md)

## 源码证据摘要

- [`DBImpl::Open/Write/Get`](../../../db/db_impl.cc#L1121-L1276)
- [`VersionSet::Recover/LogAndApply/PickCompaction`](../../../db/version_set.cc#L777-L991)
- [`Table::Open/InternalGet`](../../../table/table.cc#L37-L240)

## 未解决问题

- Linux Debug configure/build、CTest、有限 benchmark 和本地 prefix install 已有执行记录；Release/Sanitizer/fault injection/跨平台实验仍应明确标记为未验证。
- 后台任务实际线程交错、断电持久化语义和平台差异需实验。

## 下一步阅读建议

从端到端深度页的贯穿示例开始，选择一条与当前修改相关的链，再回到模块实现和测试文档。
