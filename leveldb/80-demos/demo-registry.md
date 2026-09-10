# D01 Demo 注册表

- 文档目的：登记知识库使用的真实测试/公共 API 贯穿示例，区分仓库已有入口、合成概念场景和已运行证据。
- 源码版本：`main` / `7ee830d02b623e8ffe0b95d59a74db1e58da04c5`。
- 证据状态：D01 基于真实 `db/db_test.cc` 和 `leveldb_tests`；独立子命令的实验记录仍按 D01 运行页执行。
- 最后更新：2026-09-10

## 注册表

| ID | 场景 | 类型 | 真实入口 | 覆盖模块 | 状态 |
|---|---|---|---|---|---|
| D01 | DBTest 生命周期贯穿 | GoogleTest fixture 源码解剖 | `db/db_test.cc` / `leveldb_tests` | M01–M07（构建和 Env 注入） | 源码已分析；全量 CTest 已通过；独立分组命令未单独记录 |

## D01 文档

- [README](D01-dbtest-lifecycle/README.md)
- [构建与运行](D01-dbtest-lifecycle/build-and-run.md)
- [执行轨迹](D01-dbtest-lifecycle/execution-trace.md)
- [数据与状态轨迹](D01-dbtest-lifecycle/data-and-state-trace.md)
- [调试走读](D01-dbtest-lifecycle/debug-walkthrough.md)
- [失败路径](D01-dbtest-lifecycle/failure-paths.md)
- [修改练习](D01-dbtest-lifecycle/modification-exercises.md)

## 选择依据

仓库没有独立的 `demo/` 目标；CMake 将 `db/db_test.cc` 编入 `leveldb_tests`，该 fixture 同时覆盖：

```text
Options/Env -> DB::Open -> Put/Delete/Write -> WAL/MemTable
  -> Snapshot/Get/Iterator -> immutable flush -> SSTable/Version
  -> compaction -> Close/Reopen -> WAL/MANIFEST recovery
  -> SpecialEnv fault injection -> Status/bg_error_
```

因此 D01 是“真实测试入口 + 源码端到端追踪”，而不是把一个未存在的 demo 文件假装成仓库产物。[`CMakeLists.txt:313-365`](../../../CMakeLists.txt#L313-L365)、[`DBTest` fixture](../../../db/db_test.cc#L260-L365)

## 覆盖矩阵

| 端到端阶段 | 测试/代码 | 详细说明 |
|---|---|---|
| Open/create | `DBTest` 构造、`TryReopen` | [执行轨迹阶段 1](D01-dbtest-lifecycle/execution-trace.md#阶段-1fixture-创建与首次-open) |
| write/read/delete | `ReadWrite`、`PutDeleteGet` | [执行轨迹阶段 2](D01-dbtest-lifecycle/execution-trace.md#阶段-2写入和删除) |
| snapshot | `GetSnapshot`、`Snapshot` | [执行轨迹阶段 4](D01-dbtest-lifecycle/execution-trace.md#阶段-4snapshot-固定可见序列) |
| iterator | `IterMulti`、`IteratorPinsRef` | [执行轨迹阶段 5](D01-dbtest-lifecycle/execution-trace.md#阶段-5iterator-和引用生命周期) |
| flush | `GetFromImmutableLayer` | [执行轨迹阶段 6](D01-dbtest-lifecycle/execution-trace.md#阶段-6写满immutable-flush-与-level-0) |
| recovery | `Recover`、`RecoverWithLargeLog` | [执行轨迹阶段 7](D01-dbtest-lifecycle/execution-trace.md#阶段-7关闭重开与-wal-恢复) |
| errors | `WriteSyncError`、`ManifestWriteError`、`MissingSSTFile` | [失败路径](D01-dbtest-lifecycle/failure-paths.md) |

## 状态标签规则

- **源码已确认**：可以由文件中的控制流、字段和测试断言直接支持。
- **本机已运行**：命令在当前仓库环境实际完成并记录退出状态/结果。
- **概念场景**：用源码和测试组合推导出的最小轨迹，未作为独立程序执行。
- **未验证**：需要额外命令、平台、故障注入或真实磁盘实验。

## 相关入口

- [知识库入口](../README.md)
- [端到端深度链路](../90-cross-module/end-to-end-traces.md)
- [M07 测试/构建](../01-modules/M07-tests-build/README.md)
- [分析状态](../00-overview/analysis-state.md)

## 关键源码

- [`db/db_test.cc`](../../../db/db_test.cc#L83-L365)
- [`DBTest` 核心测试](../../../db/db_test.cc#L598-L725)
- [`恢复测试](../../../db/db_test.cc#L1017-L1129)
- [`错误注入测试`](../../../db/db_test.cc#L1818-L1920)
- [`leveldb_tests` CMake 注册](../../../CMakeLists.txt#L313-L365)
