# 修改影响地图

- 文档目的：把常见修改主题映射到源码、测试和风险。
- 适用范围：全仓库。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：路径和测试入口已确认；影响深度需按具体 patch 复核。
- 最后更新：2026-09-10
- 前置阅读：[模块注册表](../01-modules/module-registry.md)
- 后续阅读：[功能开发配方](../99-roadmap/feature-development-recipes.md)
## 结论摘要

本页聚焦 90-cross-module/change-impact-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

| 修改主题 | 必看源码 | 必补验证 |
|---|---|---|
| 公共 API/Options | `include/leveldb/*`、`db_impl`、C API、CMake install | db/c test、ABI、文档 |
| WAL/WriteBatch | `log_*`、`write_batch*`、`dbformat*` | log/write_batch、recovery、损坏 |
| MemTable/sequence | `memtable*`、`skiplist*`、`db_impl` | snapshot、多版本、长压 |
| Version/Manifest | `version_set*`、`version_edit*` | version/recovery、重开、故障注入 |
| Compaction/drop | `db_impl.cc`、`version_set.cc` | autocompact、边界、删除标记 |
| SSTable/block | `table/*`、`builder.cc`、`table_cache` | table/filter、兼容、CRC |
| Env/platform | `env.h`、`env_posix/windows`、`memenv`、port | 平台 Env、故障、并发 |
| Cache/ownership | `cache*`、iterator cleanup、TableCache | eviction、ASan/TSan |
| Build/test | `CMakeLists.txt`、CI | CTest、benchmark、install |

## 影响等级

持久化格式、公共 ABI、文件集合、线程/锁、所有权和默认配置属于高影响；应从单元测试扩展到重开、跨平台和性能验证。

## 相关文档
- [项目入口](../README.md)
- [分析状态](../00-overview/analysis-state.md)
- [源码证据索引](../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
