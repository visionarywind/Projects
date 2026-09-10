# M06 行级分析

- 文档目的：标记平台和资源代码的修改边界。
- 适用范围：M06。
- 源码版本：`main` / `7ee830d`。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[source-map](source-map.md)
- 后续阅读：[development-guide](development-guide.md)

| 代码块 | 意图 | 修改风险 |
|---|---|---|
| `env.h:59-110` | Env 默认对象和文件工厂 | 所有平台实现都要满足契约 |
| `env.h:170-217` | 锁、Schedule、线程、时间 | 并发和关闭竞态 |
| `env.h:230-290` | 文件对象读写契约 | Slice/scratch 生命周期 |
| `env.h:332-403` | EnvWrapper 转发 | 新增虚方法必须同步转发 |
| `env.cc:20-28` | 兼容 API 互相转发 | 避免递归/旧实现不兼容 |
| `env.cc:49-77` | 原子写辅助和失败清理 | Sync/Close/Remove 顺序 |
| `env_posix.cc:60-128` | 系统错误和资源 limiter | Acquire/Release 配对 |
| `env_posix.cc:135-230` | read/pread 文件对象 | EINTR、短读、并发 |
| `env_posix.cc:232-255` | mmap 对象 | munmap 和 limiter 释放 |
| `port_stdcxx.h:49-87` | mutex/CondVar | adopt_lock 必须和持锁调用配对 |
| `cache.cc:218-237` | ref/unref 和 deleter | UAF、double free |
| `cache.cc:267-315` | Insert/eviction | duplicate key 和 charge |
| `arena.cc:19-63` | block 分配和对齐 | 指针只在 Arena 生命周期内有效 |

## 调试变量

记录 fd 是否 permanent、limiter 可用额度、RandomAccessFile 的 filename、Cache `refs/in_cache/usage`、Arena `MemoryUsage`，以及 Env 调度任务的执行线程。
