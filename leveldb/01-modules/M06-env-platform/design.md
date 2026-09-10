# M06 设计：抽象操作系统与资源生命周期

- 文档目的：说明 Env/port/cache/arena 的分层设计。
- 适用范围：M06。
- 源码版本：`main` / `7ee830d`。
- 证据状态：接口和实现行为已确认；设计动机部分为推断。
- 最后更新：2026-09-10
- 前置阅读：[M06 README](README.md)
- 后续阅读：[M06 interfaces](interfaces.md)

## Env 抽象

`Env` 把文件系统、锁、后台调度、线程、测试目录、日志和时间统一成虚接口；实现可以替换，例如限速、故障注入或内存文件系统。[include/leveldb/env.h:4-10](../../../../include/leveldb/env.h#L4-L10)、[include/leveldb/env.h:50-64](../../../../include/leveldb/env.h#L50-L64) `EnvWrapper` 允许只重载少数方法。[include/leveldb/env.h:332-403](../../../../include/leveldb/env.h#L332-L403)

## 文件对象按访问模式拆分

SequentialFile 只承诺单线程访问，RandomAccessFile 的 `Read` 可并发，WritableFile 由调用者串行使用；这些契约让实现可分别选择 `read`、`pread`、缓冲写和同步。[include/leveldb/env.h:66-109](../../../../include/leveldb/env.h#L66-L109)、[include/leveldb/env.h:220-290](../../../../include/leveldb/env.h#L220-L290)

## POSIX 资源降级

POSIX 随机文件优先持有 fd；资源达到 limiter 上限时释放 fd，在每次 Read 时临时 open/close，仍满足 RandomAccessFile 并发契约。[util/env_posix.cc:68-128](../../../../util/env_posix.cc#L68-L128)、[util/env_posix.cc:175-222](../../../../util/env_posix.cc#L175-L222) mmap 同样受独立 limiter 控制。[util/env_posix.cc:232-255](../../../../util/env_posix.cc#L232-L255)

## 资源基础设施

Cache 用 handle 引用计数将“缓存拥有的引用”和“客户端正在使用的引用”分离；Erase 不会在仍有 handle 时立即调用 deleter。[include/leveldb/cache.h:44-80](../../../../include/leveldb/cache.h#L44-L80) Arena 批量拥有内存块，析构时整体释放，适合 MemTable 的短期对象集合。[util/arena.cc:13-17](../../../../util/arena.cc#L13-L17)

## 平台边界

`port/port.h` 根据平台选择 `port_stdcxx.h`、Chromium 或其他实现；mutex/condition variable 和 Snappy/Zstd/CRC wrapper 在此层汇合。[port/port.h:9-18](../../../../port/port.h#L9-L18)、[port/port_stdcxx.h:44-87](../../../../port/port_stdcxx.h#L44-L87)

## 推断与未知

- 抽象层的直接收益是可测试和可移植，这是架构结果；历史上每个接口的决策原因需查提交记录确认。
- mmap 与 fd 的实际性能边界、线程池规模和文件系统差异需基准/实验验证。
