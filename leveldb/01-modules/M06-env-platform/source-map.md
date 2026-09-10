# M06 源码地图

- 文档目的：定位平台和资源基础设施的实现块。
- 适用范围：M06。
- 源码版本：`main` / `7ee830d`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[M06 design](design.md)
- 后续阅读：[M06 line-level-analysis](line-level-analysis.md)

| 文件 | 逻辑块 | 职责 |
|---|---|---|
| `include/leveldb/env.h` | 50-217 | Env 文件/锁/调度/时间接口 |
| `include/leveldb/env.h` | 220-315 | Sequential/Random/Writable/Logger/FileLock |
| `include/leveldb/env.h` | 324-403 | 文件读写辅助和 EnvWrapper |
| `util/env.cc` | 16-38 | 默认方法、析构和兼容转发 |
| `util/env.cc` | 49-102 | WriteString/ReadFile 辅助 |
| `util/env_posix.cc` | 60-128 | POSIX 错误和资源 limiter |
| `util/env_posix.cc` | 135-230 | sequential/random fd 实现 |
| `util/env_posix.cc` | 232-300 | mmap random file |
| `util/env_posix.cc` | 约 600-900 | POSIX Env、目录、锁和调度 |
| `util/env_windows.cc` | 全文件 | Windows Env 对应实现 |
| `port/port.h` | 9-18 | 平台选择 |
| `port/port_stdcxx.h` | 44-87 | mutex/CondVar |
| `port/port_stdcxx.h` | 89-217 | Snappy/Zstd/CRC wrapper |
| `include/leveldb/cache.h` | 33-97 | Cache 生命周期契约 |
| `util/cache.cc` | 40-146 | hash table 和 LRUHandle |
| `util/cache.cc` | 198-334 | ref/eviction/erase/prune |
| `util/cache.cc` | 336-399 | 16-shard LRU |
| `util/arena.h/.cc` | 15-66 | arena 分配和整体释放 |
| `helpers/memenv/memenv.cc` | 388 | 测试内存 Env 工厂 |

## 依赖方向

上层通过 `Options::env` 调用公共 Env；POSIX/Windows 实现依赖 port；TableCache 和 block cache 依赖 Cache；MemTable 依赖 Arena。M06 不定义数据库语义，也不决定 compaction。

## 相关源码

- [EnvWrapper](../../../../include/leveldb/env.h#L332-L403)
- [Cache](../../../../include/leveldb/cache.h#L33-L98)
- [Arena](../../../../util/arena.h#L15-L66)
