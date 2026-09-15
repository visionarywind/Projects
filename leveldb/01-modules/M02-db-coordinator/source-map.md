# M02 源码地图

- 文档目的：映射 DBImpl 关键文件、符号和测试。
- 适用范围：M02。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M02 README](README.md)
- 后续阅读：[M02 调用链](call-chains.md)
## 结论摘要

本页聚焦 01-modules/M02-db-coordinator/source-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


| 源码位置 | 作用 | 调用/依赖 | 重要性 |
|---|---|---|---|
| `db/db_impl.h:28-204` | 类接口、状态和私有流程 | M01、M03/M04/M06 | 高 |
| `db/db_impl.cc:125-178` | 构造和析构 | Options/Env/Cache | 高 |
| `db/db_impl.cc:180-504` | 新库与恢复 | VersionSet/WAL | 高 |
| `db/db_impl.cc:668-1079` | 调度、flush、compaction | M04/M05/Env | 高 |
| `db/db_impl.cc:1121-1206` | 读、迭代、快照 | M03/M04 | 高 |
| `db/db_impl.cc:1206-1467` | 写、限流、属性 | M03/M04 | 高 |
| `db/c.cc` | C API 适配 | M01 | 中 |
| `db/repair.cc` | 修复损坏库 | WAL/MemTable/Table | 中 |
| `db/db_test.cc` | 综合行为测试 | 全部核心 | 高 |
| `db/recovery_test.cc` | 重启/日志恢复 | M03/M04 | 高 |

## 相关文档

- [line-level-analysis](line-level-analysis.md)
- [testing](testing.md)

## 源码证据摘要

CMake 将 DBImpl 相关源文件纳入 `leveldb` 目标（[CMakeLists.txt:119-152](../../../source/leveldb/CMakeLists.txt#L119-L152)）。

## 未解决问题

C API 每个资源包装器的完整生命周期留给 M01/M07 深入分析。

## 下一步阅读建议

先读头文件字段，再按启动/写/读/后台顺序读实现。
