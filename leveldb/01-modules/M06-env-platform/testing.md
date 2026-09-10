# M06 测试分析

- 文档目的：列出 Env、平台和资源基础设施的验证策略。
- 适用范围：M06。
- 源码版本：`main` / `7ee830d`。
- 证据状态：测试入口已确认；执行未验证。
- 最后更新：2026-09-10
- 前置阅读：[M06 examples](examples.md)
- 后续阅读：[M07 testing](../M07-tests-build/testing.md)

## 测试入口

| 主题 | 文件 |
|---|---|
| POSIX Env 行为 | `util/env_posix_test.cc`、`util/env_test.cc` |
| MemEnv | `helpers/memenv/memenv_test.cc` |
| Cache ref/eviction | `util/cache_test.cc` |
| Arena | `util/arena_test.cc` |
| 编解码/CRC/port | `util/*_test.cc` |
| DB 故障注入 | `db/fault_injection_test.cc` |

实际目标是否被当前 CMake 配置编译，应以 CMake/CTest 枚举为准；本页不宣称已运行。

## 必测矩阵

1. 文件不存在、权限错误、短读、短写和 EINTR。
2. RandomAccessFile 在多线程并发 Read 下的结果和 fd limiter 降级。
3. mmap limit、max open files 和资源释放。
4. LockFile 的重复加锁、Unlock 顺序和进程边界。
5. Schedule 多任务并发与 DB shutdown。
6. EnvWrapper 只覆写一个操作时的其余转发。
7. Cache duplicate key、Erase 后仍持 handle、最终 deleter 一次且仅一次。
8. Arena 对齐、大对象、析构后的指针不可用。
9. MemEnv 与 POSIX Env 的行为差异。
10. Snappy/Zstd/CRC 可用和不可用编译分支。

## 回归要求

新增 Env 虚函数必须覆盖 POSIX、Windows（若可构建）、MemEnv 和 EnvWrapper；改变 Cache 引用规则必须配合 ASan/TSan 或至少运行生命周期测试；改变 port 配置必须覆盖禁用可选依赖的构建。

## 当前状态

构建、CTest、Sanitizer 和跨平台 CI 均未在本轮执行。
