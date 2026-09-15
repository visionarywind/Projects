# LevelDB 项目理解知识库

- 文档目的：为未参与过 LevelDB 的开发者提供可追溯的架构、代码、构建、调试和扩展入口。
- 适用范围：仓库 `main` 在提交 `7ee830d02b623e8ffe0b95d59a74db1e58da04c5` 的源码；项目版本 `1.23.0`。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：核心源码和 Linux Debug 验证已确认；未执行的 Release、Sanitizer、跨平台和故障注入实验明确标注。
- 最后更新：2026-09-10
- 前置阅读：无
- 后续阅读：[5 分钟项目概览](00-overview/project-overview.md)、[阅读指南](99-roadmap/reading-guide.md)
## 结论摘要

本页聚焦 README.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。

## 一句话介绍

LevelDB 是一个嵌入式、进程内、持久化且按键有序的字符串键值库。它把最近写入先落到 WAL 和内存表，再异步转换为分层 SSTable，通过版本清单和后台 compaction 在崩溃恢复、写入吞吐与读取放大之间取得平衡。

## 5 分钟快速理解

1. 外部程序调用 `leveldb::DB::Open` 以及 `Put/Get/Delete/Write/NewIterator`；公共契约在 `include/leveldb/`。
2. `DBImpl` 串联锁、写入者队列、日志、MemTable、快照、VersionSet 与后台压缩。
3. 每个写批次先编码为 WAL 记录，再插入 MemTable；读取按快照序号合并内存表与 SSTable。
4. MemTable 达到阈值后变为 immutable MemTable，后台用 `TableBuilder` 生成 Level-0 表，并把 VersionEdit 写入 MANIFEST。
5. VersionSet 按层维护表文件，后台 compaction 合并重叠文件、丢弃被覆盖版本和安全的删除标记。
6. Env 抽象文件、锁、线程和调度；POSIX、Windows、MemEnv 可替换同一边界。

## 目录导航

- [总览层](00-overview/project-overview.md)：定位、架构、运行、数据流、依赖、错误、构建。
- [模块注册表](01-modules/module-registry.md)：M01–M07 的边界和证据。
- [模块层](01-modules/)：公共 API、DB 协调、WAL/MemTable、Version/Compaction、SSTable、Env、测试工程。
- [关联层总览](90-cross-module/end-to-end-flows.md)：跨模块流程导航。
- [端到端深度链路](90-cross-module/end-to-end-traces.md)：八条从 API 到文件、线程、资源和错误出口的逐阶段轨迹。
- [池化与资源管理专题](90-cross-module/pooling-and-resource-management.md)：Arena、两级 LRU、引用计数、文件句柄和后台输出生命周期。
- Graph 边界：LevelDB 核心不包含 CUDA Graph 或执行图内存池；专题仅覆盖 CPU Arena/cache/文件资源。
- [实践层](99-roadmap/quick-start.md)：上手、调试、测试、功能开发和路线图。
- [真实测试 Demo 注册表](80-demos/demo-registry.md)：以 `db/db_test.cc`/`leveldb_tests` 为入口的 D01 端到端源码解剖。

## 总体架构

```mermaid
flowchart LR
  U[应用程序] --> API[M01 公共 API]
  API --> DBI[M02 DBImpl 协调器]
  DBI --> WAL[M03 WAL / WriteBatch / MemTable]
  DBI --> VS[M04 VersionSet / Manifest / Compaction]
  VS --> SST[M05 SSTable / Block / Iterator]
  DBI --> ENV[M06 Env / Cache / Port]
  ENV --> OS[POSIX 或 Windows]
  M07[M07 测试与构建] -.验证.-> API
  M07 -.验证.-> DBI
  M07 -.验证.-> SST
```

箭头表示调用或资源依赖；虚线表示测试/构建验证关系。M01 的公开头文件是稳定边界，`db/` 和 `table/` 的内部头文件不是公共契约（证据：[README.md:207-214](../source/leveldb/README.md#L207-L214)）。

## 三条最重要流程

| 流程 | 入口 | 关键链 | 深度轨迹 |
|---|---|---|---|
| 打开/恢复 | `DB::Open` | `DBImpl::Recover` → `VersionSet::Recover` → `RecoverLogFile` | [链 1、2](90-cross-module/end-to-end-traces.md#链-1首次打开与创建数据库) |
| 写入/flush | `DB::Write` | writer 队列 → WAL → MemTable → immutable flush → MANIFEST | [链 3、6](90-cross-module/end-to-end-traces.md#链-3并发批量写入限流与日志先行) |
| 读取/压缩 | `DBImpl::Get` | MemTable → `Version::Get` → `TableCache` → Block；后台 `DoCompactionWork` | [链 4、7](90-cross-module/end-to-end-traces.md#链-4单键读取从快照序号到-table-block) |

## 主 Demo 与覆盖矩阵

仓库没有独立的 `demo/` 程序；知识库以真实公共 API 场景和 `db/db_test.cc` 等测试 fixture 为主线，另提供源码驱动的贯穿示例。该场景从 `DB::Open`、`Put`、Snapshot、Delete 经过 WAL/MemTable、flush、compaction、reopen 和 `Get`，具体步骤见[端到端深度链路贯穿示例](90-cross-module/end-to-end-traces.md#一条贯穿示例k-从写入到重开)。它是“合成、未运行”的概念轨迹，不应当作本机执行结果。

| Demo/测试阶段 | 模块 | 真实入口/实现 | 源码证据 |
|---|---|---|---|
| Open/reopen | M01/M02/M04/M06 | `DB::Open` → `DBImpl::Recover` → `VersionSet::Recover` → `Env` | [`DB::Open`](../source/leveldb/db/db_impl.cc#L1503-L1544) |
| Put/WAL/MemTable | M01/M02/M03 | `DB::Put` → `DBImpl::Write` → `log::Writer` → `MemTable` | [`DBImpl::Write`](../source/leveldb/db/db_impl.cc#L1206-L1276) |
| flush/compaction | M02/M04/M05/M06 | `CompactMemTable`/`DoCompactionWork` → `BuildTable` → `LogAndApply` | [`DBImpl::CompactMemTable`](../source/leveldb/db/db_impl.cc#L549-L580) |
| Get/Snapshot/Table | M01/M02/M04/M05 | `DBImpl::Get` → `Version::Get` → `Table::InternalGet` | [`DBImpl::Get`](../source/leveldb/db/db_impl.cc#L1121-L1165) |

## 构建与运行入口

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug   # 已验证：configure 成功
cmake --build build -j2                        # 已验证：100% 完成
ctest --test-dir build --output-on-failure     # 已验证：3/3 通过
./build/db_bench --benchmarks=fillseq,readrandom \
  --num=10000 --value_size=100 --threads=1 \
  --db=/tmp/leveldb-knowledge-bench-20260910-r2  # 已验证：10000/10000 命中
```

CMake 默认打开测试、基准和安装选项（[CMakeLists.txt:32-34](../source/leveldb/CMakeLists.txt#L32-L34)）；CI 使用配置、构建、CTest、benchmark 和安装步骤（[.github/workflows/build.yml:74-102](../source/leveldb/.github/workflows/build.yml#L74-L102)）。本机 Debug 构建、CTest 和有限 benchmark 已执行；此前缺少的 GoogleTest/benchmark 子模块已在用户授权后初始化。CTest 结果为 `leveldb_tests`、`c_test`、`env_posix_test` 全部通过（本轮总计 91.24 s）。benchmark 在 Debug、启用断言且未启用 Snappy 的条件下得到 `fillseq 3.114 micros/op`、`readrandom 1.199 micros/op`，仅作可运行性记录。

## 按角色阅读

- C++/API 使用者：M01 → M02 `DBImpl::Open/Write/Get` → M03。
- 存储引擎开发者：先读[端到端深度链路](90-cross-module/end-to-end-traces.md)，再读 M03 → M04 → M05。
- 平台/测试开发者：M06 → M07 → 实践层。
- 故障定位：[错误边界](90-cross-module/error-boundaries.md) → [调试指南](99-roadmap/debugging-guide.md)。

## 状态与缺口

当前已完成仓库盘点、目标版本确认、总体模型、模块划分、全部标准模块文档、八条端到端深度链路，以及 Linux Debug 构建验证。最新静态检查覆盖 117 个 Markdown 文件、1212 个相对链接（断链 0）、源码锚点越界 0；`git diff --check` 无输出。深度文档使用的函数控制流、锁边界、引用动作和错误出口已由源码核对；CTest 聚合目标和有限 benchmark 已实际运行。

本机已完成 Debug configure、build、CTest 和有限 benchmark；本轮 CTest 的 3 个目标全部通过。此前本地 prefix install 也已完成。Release/RelWithDebInfo、Sanitizer、fault injection、跨平台构建、D01 各 filter 的独立记录和生产性能基线仍未验证。没有运行服务或修改 LevelDB 源码。

## 文档元数据（规范补充）

- 文档目的：说明 `README.md` 的源码分析范围、结论和维护入口。
- 适用范围：当前项目对应模块/入口的静态源码与测试分析。
- 对应源码版本：以本项目 `00-overview/analysis-state.md` 或同页版本字段为准。
- 证据状态：静态源码证据；未执行的构建、测试、GPU、网络或多进程行为保持“未验证”。
- 最后更新：2026-09-15
- 前置阅读：本项目根 README 与 `00-overview/analysis-state.md`。
- 后续阅读：本模块/示例的实现、测试和风险页面。

## 全文档索引

### 00-overview
- [00-overview/analysis-state.md](00-overview/analysis-state.md)
- [00-overview/architecture.md](00-overview/architecture.md)
- [00-overview/build-and-deploy.md](00-overview/build-and-deploy.md)
- [00-overview/decision-log.md](00-overview/decision-log.md)
- [00-overview/dependency-map.md](00-overview/dependency-map.md)
- [00-overview/design-principles.md](00-overview/design-principles.md)
- [00-overview/evidence-index.md](00-overview/evidence-index.md)
- [00-overview/global-data-flow.md](00-overview/global-data-flow.md)
- [00-overview/global-error-model.md](00-overview/global-error-model.md)
- [00-overview/glossary.md](00-overview/glossary.md)
- [00-overview/project-overview.md](00-overview/project-overview.md)
- [00-overview/runtime-model.md](00-overview/runtime-model.md)

### 01-modules
- [01-modules/M01-public-api/README.md](01-modules/M01-public-api/README.md)
- [01-modules/M01-public-api/call-chains.md](01-modules/M01-public-api/call-chains.md)
- [01-modules/M01-public-api/data-structures.md](01-modules/M01-public-api/data-structures.md)
- [01-modules/M01-public-api/design.md](01-modules/M01-public-api/design.md)
- [01-modules/M01-public-api/development-guide.md](01-modules/M01-public-api/development-guide.md)
- [01-modules/M01-public-api/diagrams.md](01-modules/M01-public-api/diagrams.md)
- [01-modules/M01-public-api/examples.md](01-modules/M01-public-api/examples.md)
- [01-modules/M01-public-api/interfaces.md](01-modules/M01-public-api/interfaces.md)
- [01-modules/M01-public-api/line-level-analysis.md](01-modules/M01-public-api/line-level-analysis.md)
- [01-modules/M01-public-api/risks-and-debt.md](01-modules/M01-public-api/risks-and-debt.md)
- [01-modules/M01-public-api/source-map.md](01-modules/M01-public-api/source-map.md)
- [01-modules/M01-public-api/testing.md](01-modules/M01-public-api/testing.md)
- [01-modules/M02-db-coordinator/README.md](01-modules/M02-db-coordinator/README.md)
- [01-modules/M02-db-coordinator/call-chains.md](01-modules/M02-db-coordinator/call-chains.md)
- [01-modules/M02-db-coordinator/data-structures.md](01-modules/M02-db-coordinator/data-structures.md)
- [01-modules/M02-db-coordinator/design.md](01-modules/M02-db-coordinator/design.md)
- [01-modules/M02-db-coordinator/development-guide.md](01-modules/M02-db-coordinator/development-guide.md)
- [01-modules/M02-db-coordinator/diagrams.md](01-modules/M02-db-coordinator/diagrams.md)
- [01-modules/M02-db-coordinator/examples.md](01-modules/M02-db-coordinator/examples.md)
- [01-modules/M02-db-coordinator/interfaces.md](01-modules/M02-db-coordinator/interfaces.md)
- [01-modules/M02-db-coordinator/line-level-analysis.md](01-modules/M02-db-coordinator/line-level-analysis.md)
- [01-modules/M02-db-coordinator/risks-and-debt.md](01-modules/M02-db-coordinator/risks-and-debt.md)
- [01-modules/M02-db-coordinator/source-map.md](01-modules/M02-db-coordinator/source-map.md)
- [01-modules/M02-db-coordinator/testing.md](01-modules/M02-db-coordinator/testing.md)
- [01-modules/M03-wal-memtable/README.md](01-modules/M03-wal-memtable/README.md)
- [01-modules/M03-wal-memtable/call-chains.md](01-modules/M03-wal-memtable/call-chains.md)
- [01-modules/M03-wal-memtable/data-structures.md](01-modules/M03-wal-memtable/data-structures.md)
- [01-modules/M03-wal-memtable/design.md](01-modules/M03-wal-memtable/design.md)
- [01-modules/M03-wal-memtable/development-guide.md](01-modules/M03-wal-memtable/development-guide.md)
- [01-modules/M03-wal-memtable/diagrams.md](01-modules/M03-wal-memtable/diagrams.md)
- [01-modules/M03-wal-memtable/examples.md](01-modules/M03-wal-memtable/examples.md)
- [01-modules/M03-wal-memtable/interfaces.md](01-modules/M03-wal-memtable/interfaces.md)
- [01-modules/M03-wal-memtable/line-level-analysis.md](01-modules/M03-wal-memtable/line-level-analysis.md)
- [01-modules/M03-wal-memtable/risks-and-debt.md](01-modules/M03-wal-memtable/risks-and-debt.md)
- [01-modules/M03-wal-memtable/source-map.md](01-modules/M03-wal-memtable/source-map.md)
- [01-modules/M03-wal-memtable/testing.md](01-modules/M03-wal-memtable/testing.md)
- [01-modules/M04-version-compaction/README.md](01-modules/M04-version-compaction/README.md)
- [01-modules/M04-version-compaction/call-chains.md](01-modules/M04-version-compaction/call-chains.md)
- [01-modules/M04-version-compaction/data-structures.md](01-modules/M04-version-compaction/data-structures.md)
- [01-modules/M04-version-compaction/design.md](01-modules/M04-version-compaction/design.md)
- [01-modules/M04-version-compaction/development-guide.md](01-modules/M04-version-compaction/development-guide.md)
- [01-modules/M04-version-compaction/diagrams.md](01-modules/M04-version-compaction/diagrams.md)
- [01-modules/M04-version-compaction/examples.md](01-modules/M04-version-compaction/examples.md)
- [01-modules/M04-version-compaction/interfaces.md](01-modules/M04-version-compaction/interfaces.md)
- [01-modules/M04-version-compaction/line-level-analysis.md](01-modules/M04-version-compaction/line-level-analysis.md)
- [01-modules/M04-version-compaction/risks-and-debt.md](01-modules/M04-version-compaction/risks-and-debt.md)
- [01-modules/M04-version-compaction/source-map.md](01-modules/M04-version-compaction/source-map.md)
- [01-modules/M04-version-compaction/testing.md](01-modules/M04-version-compaction/testing.md)
- [01-modules/M05-sstable-table/README.md](01-modules/M05-sstable-table/README.md)
- [01-modules/M05-sstable-table/call-chains.md](01-modules/M05-sstable-table/call-chains.md)
- [01-modules/M05-sstable-table/data-structures.md](01-modules/M05-sstable-table/data-structures.md)
- [01-modules/M05-sstable-table/design.md](01-modules/M05-sstable-table/design.md)
- [01-modules/M05-sstable-table/development-guide.md](01-modules/M05-sstable-table/development-guide.md)
- [01-modules/M05-sstable-table/diagrams.md](01-modules/M05-sstable-table/diagrams.md)
- [01-modules/M05-sstable-table/examples.md](01-modules/M05-sstable-table/examples.md)
- [01-modules/M05-sstable-table/implementation.md](01-modules/M05-sstable-table/implementation.md)
- [01-modules/M05-sstable-table/interfaces.md](01-modules/M05-sstable-table/interfaces.md)
- [01-modules/M05-sstable-table/line-level-analysis.md](01-modules/M05-sstable-table/line-level-analysis.md)
- [01-modules/M05-sstable-table/risks-and-debt.md](01-modules/M05-sstable-table/risks-and-debt.md)
- [01-modules/M05-sstable-table/source-map.md](01-modules/M05-sstable-table/source-map.md)
- [01-modules/M05-sstable-table/testing.md](01-modules/M05-sstable-table/testing.md)
- [01-modules/M06-env-platform/README.md](01-modules/M06-env-platform/README.md)
- [01-modules/M06-env-platform/call-chains.md](01-modules/M06-env-platform/call-chains.md)
- [01-modules/M06-env-platform/data-structures.md](01-modules/M06-env-platform/data-structures.md)
- [01-modules/M06-env-platform/design.md](01-modules/M06-env-platform/design.md)
- [01-modules/M06-env-platform/development-guide.md](01-modules/M06-env-platform/development-guide.md)
- [01-modules/M06-env-platform/diagrams.md](01-modules/M06-env-platform/diagrams.md)
- [01-modules/M06-env-platform/examples.md](01-modules/M06-env-platform/examples.md)
- [01-modules/M06-env-platform/implementation.md](01-modules/M06-env-platform/implementation.md)
- [01-modules/M06-env-platform/interfaces.md](01-modules/M06-env-platform/interfaces.md)
- [01-modules/M06-env-platform/line-level-analysis.md](01-modules/M06-env-platform/line-level-analysis.md)
- [01-modules/M06-env-platform/risks-and-debt.md](01-modules/M06-env-platform/risks-and-debt.md)
- [01-modules/M06-env-platform/source-map.md](01-modules/M06-env-platform/source-map.md)
- [01-modules/M06-env-platform/testing.md](01-modules/M06-env-platform/testing.md)
- [01-modules/M07-tests-build/README.md](01-modules/M07-tests-build/README.md)
- [01-modules/M07-tests-build/call-chains.md](01-modules/M07-tests-build/call-chains.md)
- [01-modules/M07-tests-build/data-structures.md](01-modules/M07-tests-build/data-structures.md)
- [01-modules/M07-tests-build/design.md](01-modules/M07-tests-build/design.md)
- [01-modules/M07-tests-build/development-guide.md](01-modules/M07-tests-build/development-guide.md)
- [01-modules/M07-tests-build/diagrams.md](01-modules/M07-tests-build/diagrams.md)
- [01-modules/M07-tests-build/examples.md](01-modules/M07-tests-build/examples.md)
- [01-modules/M07-tests-build/interfaces.md](01-modules/M07-tests-build/interfaces.md)
- [01-modules/M07-tests-build/line-level-analysis.md](01-modules/M07-tests-build/line-level-analysis.md)
- [01-modules/M07-tests-build/risks-and-debt.md](01-modules/M07-tests-build/risks-and-debt.md)
- [01-modules/M07-tests-build/source-map.md](01-modules/M07-tests-build/source-map.md)
- [01-modules/M07-tests-build/testing.md](01-modules/M07-tests-build/testing.md)
- [01-modules/module-registry.md](01-modules/module-registry.md)

### 80-demos
- [80-demos/D01-dbtest-lifecycle/README.md](80-demos/D01-dbtest-lifecycle/README.md)
- [80-demos/D01-dbtest-lifecycle/build-and-run.md](80-demos/D01-dbtest-lifecycle/build-and-run.md)
- [80-demos/D01-dbtest-lifecycle/data-and-state-trace.md](80-demos/D01-dbtest-lifecycle/data-and-state-trace.md)
- [80-demos/D01-dbtest-lifecycle/debug-walkthrough.md](80-demos/D01-dbtest-lifecycle/debug-walkthrough.md)
- [80-demos/D01-dbtest-lifecycle/execution-trace.md](80-demos/D01-dbtest-lifecycle/execution-trace.md)
- [80-demos/D01-dbtest-lifecycle/failure-paths.md](80-demos/D01-dbtest-lifecycle/failure-paths.md)
- [80-demos/D01-dbtest-lifecycle/modification-exercises.md](80-demos/D01-dbtest-lifecycle/modification-exercises.md)
- [80-demos/demo-registry.md](80-demos/demo-registry.md)

### 90-cross-module
- [90-cross-module/change-impact-map.md](90-cross-module/change-impact-map.md)
- [90-cross-module/configuration-impact-map.md](90-cross-module/configuration-impact-map.md)
- [90-cross-module/cross-module-call-chains.md](90-cross-module/cross-module-call-chains.md)
- [90-cross-module/end-to-end-flows.md](90-cross-module/end-to-end-flows.md)
- [90-cross-module/end-to-end-traces.md](90-cross-module/end-to-end-traces.md)
- [90-cross-module/error-boundaries.md](90-cross-module/error-boundaries.md)
- [90-cross-module/interface-contracts.md](90-cross-module/interface-contracts.md)
- [90-cross-module/performance-critical-paths.md](90-cross-module/performance-critical-paths.md)
- [90-cross-module/pooling-and-resource-management.md](90-cross-module/pooling-and-resource-management.md)
- [90-cross-module/runtime-trace.md](90-cross-module/runtime-trace.md)
- [90-cross-module/shared-data-and-types.md](90-cross-module/shared-data-and-types.md)
- [90-cross-module/system-wiring.md](90-cross-module/system-wiring.md)

### 99-roadmap
- [99-roadmap/debugging-guide.md](99-roadmap/debugging-guide.md)
- [99-roadmap/feature-development-recipes.md](99-roadmap/feature-development-recipes.md)
- [99-roadmap/next-steps.md](99-roadmap/next-steps.md)
- [99-roadmap/performance-guide.md](99-roadmap/performance-guide.md)
- [99-roadmap/quick-start.md](99-roadmap/quick-start.md)
- [99-roadmap/reading-guide.md](99-roadmap/reading-guide.md)
- [99-roadmap/risk-register.md](99-roadmap/risk-register.md)
- [99-roadmap/technical-debt.md](99-roadmap/technical-debt.md)
- [99-roadmap/testing-recipes.md](99-roadmap/testing-recipes.md)

## 深度审计

| 分析对象 | 入口落地 | 正常路径 | 分支 | 异常 | 清理 | 数据生命周期 | 执行上下文 | 行级证据 | Demo 映射 | 状态/缺口 |
|---|---|---|---|---|---|---|---|---|---|---|
| `README.md` | 已完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 部分完成 | 已映射或不适用 | 部分完成：动态行为、边界或专用变体仍需验证 |

## 相关文档

- [项目概览](00-overview/project-overview.md)
- [总体架构](00-overview/architecture.md)
- [模块注册表](01-modules/module-registry.md)
- [分析状态](00-overview/analysis-state.md)
- [端到端深度链路](90-cross-module/end-to-end-traces.md)
- [构建/调试/测试实践](99-roadmap/quick-start.md)

## 源码证据摘要

- [README.md:1-31](../source/leveldb/README.md#L1-L31)
- [CMakeLists.txt:4-34](../source/leveldb/CMakeLists.txt#L4-L34)
- [db/db_impl.h:28-71](../source/leveldb/db/db_impl.h#L28-L71)
- [doc/impl.md:7-49](../source/leveldb/doc/impl.md#L7-L49)

## 未解决问题

- Release/RelWithDebInfo、Sanitizer、跨平台构建、真实故障注入和生产性能基线仍需实验确认。
- 独立 D01 GoogleTest filter 尚未逐组登记；全量 `leveldb_tests` 已作为 CTest 目标通过。

## 下一步阅读建议

先读 [端到端深度链路](90-cross-module/end-to-end-traces.md) 的贯穿示例，再按目标选择 [项目概览](00-overview/project-overview.md)、[模块注册表](01-modules/module-registry.md) 或 [阅读指南](99-roadmap/reading-guide.md)。
