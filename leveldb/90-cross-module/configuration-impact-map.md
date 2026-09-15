# 配置影响图

- 文档目的：追踪 Options/CMake 配置对运行行为和文件格式的影响。
- 适用范围：M01–M07。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：配置字段和主要使用点已确认；性能效果需 benchmark。
- 最后更新：2026-09-10
- 前置阅读：[共享数据](shared-data-and-types.md)
- 后续阅读：[性能关键路径](performance-critical-paths.md)
## 结论摘要

本页聚焦 90-cross-module/configuration-impact-map.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

| 配置 | 影响模块 | 主要效果 | 验证 |
|---|---|---|---|
| `comparator` | M01/M03/M04/M05 | key 排序、separator、Seek | reverse comparator 测试 |
| `env` | M02/M04/M05/M06 | 文件、锁、调度、故障注入 | MemEnv/fault injection |
| `write_buffer_size` | M02/M03/M04 | MemTable 切换频率 | autocompact/benchmark |
| `block_size` | M05/M06 | block 数量、读放大、索引大小 | table test/benchmark |
| `block_restart_interval` | M05 | prefix compression 与 Seek | block test |
| `compression`/`zstd_compression_level` | M05/M06 | 文件大小/CPU/兼容分支 | compression benchmark |
| `filter_policy` | M05 | 可能跳过 data block | filter test |
| `block_cache` | M05/M06 | 读取缓存和内存占用 | cache/eviction test |
| `max_open_files` | M04/M05/M06 | TableCache 与 fd limiter | env/table cache |
| `paranoid_checks` | M02/M05 | checksum/metadata 错误策略 | corruption test |
| `sync` | M02/M03/M06 | WAL 持久性延迟与故障边界 | fault injection |
| `snapshot` | M01/M02/M03/M04 | 可见 sequence 和删除标记清理 | snapshot tests |
| CMake `BUILD_SHARED_LIBS` | M07 | 测试集合和 ABI/visibility | 双构建 |
| `HAVE_SNAPPY/ZSTD/CRC32C` | M05/M06/M07 | 编译/链接和数据路径 | feature matrix |

## 改动规则

Options 默认值或 CMake feature 检测的改变都可能成为跨模块行为变化；先做影响图，再更新模块测试和实践文档。

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
