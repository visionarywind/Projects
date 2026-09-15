# M06 数据结构与生命周期

- 文档目的：说明文件对象、Limiter、Cache 和 Arena 的内存/引用关系。
- 适用范围：M06。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：已确认。
- 最后更新：2026-09-10
- 前置阅读：[interfaces](interfaces.md)
- 后续阅读：[call-chains](call-chains.md)
## 结论摘要

本页聚焦 01-modules/M06-env-platform/data-structures.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## 文件对象

```text
Env
 ├─ SequentialFile  -- read/skip，单线程
 ├─ RandomAccessFile -- pread/mmap，多线程 Read
 ├─ WritableFile -- append/flush/sync/close，单线程
 └─ FileLock/Logger
```

对象由 Env 工厂创建，由调用方 delete；POSIX 实现的析构函数关闭 fd 或解除 mmap。[util/env_posix.cc:135-139](../../../source/leveldb/util/env_posix.cc#L135-L139)、[util/env_posix.cc:190-195](../../../source/leveldb/util/env_posix.cc#L190-L195)、[util/env_posix.cc:253-255](../../../source/leveldb/util/env_posix.cc#L253-L255)

## Limiter

Limiter 用 atomic 计数器控制可持有的 fd 或 mmap 数量；Acquire 成功才拥有资源，Release 必须与成功 Acquire 一一对应。[util/env_posix.cc:68-128](../../../source/leveldb/util/env_posix.cc#L68-L128) 当 fd 额度耗尽时，`PosixRandomAccessFile` 保存文件名而不是 fd，并在每次 Read 中临时打开。[util/env_posix.cc:175-222](../../../source/leveldb/util/env_posix.cc#L175-L222)

## Cache 对象

`LRUHandle` 同时连接 hash bucket 和 LRU/in-use 双向链表。`refs` 包含 cache reference（若仍在 cache）和外部 handle references；refs 归零才调用 deleter。[util/cache.cc:40-62](../../../source/leveldb/util/cache.cc#L40-L62)、[util/cache.cc:218-237](../../../source/leveldb/util/cache.cc#L218-L237) ShardedLRUCache 使用 16 个 shard，按 hash 高位分片。[util/cache.cc:336-370](../../../source/leveldb/util/cache.cc#L336-L370)

## Arena

Arena 以约 4096 字节为块分配，小对象复用当前块，大对象单独分配；`blocks_` 持有所有分配块，析构统一 `delete[]`。[util/arena.cc:8-35](../../../source/leveldb/util/arena.cc#L8-L35)

## 生命周期图

```text
TableCache handle
  -> TableAndFile
     -> Table + RandomAccessFile
        -> block cache handle / iterator cleanup
```

Cache entry 即使从 LRU 列表移除，只要客户端仍持有 handle 就不能释放；这与 M05 的 iterator cleanup 直接相连。

## 相关文档
- [项目入口](../../README.md)
- [分析状态](../../00-overview/analysis-state.md)
- [源码证据索引](../../00-overview/evidence-index.md)

## 源码证据摘要
本页结论所需的源码路径和行号以 [源码证据索引](../../00-overview/evidence-index.md) 及正文引用为准；本页不把未执行的构建、运行或硬件行为写成已验证事实。

## 未解决问题
目标环境、动态构建/运行、硬件和外部依赖行为未在本轮执行；缺少直接证据的结论仍标记为未知或未验证。

## 下一步阅读建议
先阅读 [分析状态](../../00-overview/analysis-state.md)，再沿本页已有链接进入对应模块、Demo 或跨模块流程。
