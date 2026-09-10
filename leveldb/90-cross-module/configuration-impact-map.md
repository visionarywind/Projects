# 配置影响图

- 文档目的：追踪 Options/CMake 配置对运行行为和文件格式的影响。
- 适用范围：M01–M07。
- 源码版本：`main` / `7ee830d`。
- 证据状态：配置字段和主要使用点已确认；性能效果需 benchmark。
- 最后更新：2026-09-10
- 前置阅读：[共享数据](shared-data-and-types.md)
- 后续阅读：[性能关键路径](performance-critical-paths.md)

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
