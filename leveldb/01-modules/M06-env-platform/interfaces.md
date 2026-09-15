# M06 接口与契约

- 文档目的：记录 Env、文件对象和 Cache 的线程、所有权和错误契约。
- 适用范围：M06。
- 对应源码版本：source/leveldb HEAD 7ee830d（2026-09-15 只读确认）。
- 证据状态：公共头文件契约已确认。
- 最后更新：2026-09-10
- 前置阅读：[M06 design](design.md)
- 后续阅读：[M06 data structures](data-structures.md)
## 结论摘要

本页聚焦 01-modules/M06-env-platform/interfaces.md；具体事实以正文引用的目标源码版本为准，未执行的构建、运行和硬件行为保持未验证。


- 源码版本：`main` / `7ee830d`。

## Env

- `Env::Default()` 返回由 LevelDB 所有、不得 delete 的单例/默认对象。[include/leveldb/env.h:59-64](../../../source/leveldb/include/leveldb/env.h#L59-L64)
- `NewSequentialFile` 返回的对象只由一个线程访问；`NewRandomAccessFile` 返回对象可被多线程并发读取；WritableFile、AppendableFile 由一个线程访问。[include/leveldb/env.h:66-109](../../../source/leveldb/include/leveldb/env.h#L66-L109)
- LockFile 不等待已有锁；成功后必须 UnlockFile，进程退出时锁自动释放。[include/leveldb/env.h:170-189](../../../source/leveldb/include/leveldb/env.h#L170-L189)
- `Schedule` 的任务可能并发执行，调用者不能假定同一 Env 串行。[include/leveldb/env.h:191-201](../../../source/leveldb/include/leveldb/env.h#L191-L201)

## File ownership

成功创建文件对象后，调用者拥有返回指针并负责 delete；`TableCache`、DBImpl 等上层对象负责保持所需文件在借用期间有效。`Read` 输出的 Slice 可能指向 caller 提供的 scratch，scratch 必须保持存活到 Slice 不再使用。[include/leveldb/env.h:230-238](../../../source/leveldb/include/leveldb/env.h#L230-L238)、[include/leveldb/env.h:260-270](../../../source/leveldb/include/leveldb/env.h#L260-L270)

## Cache

Insert/Lookup 返回的 handle 必须恰好 Release；Value 只能在 handle 未释放时使用；Erase 只从查找表中移除，底层 value 要等所有 handle 释放。[include/leveldb/cache.h:44-80](../../../source/leveldb/include/leveldb/cache.h#L44-L80)

## Arena

`Allocate` 禁止 0 字节，返回指针由 Arena 持有；调用者不能单独 delete，Arena 析构统一释放 blocks。[util/arena.h:22-66](../../../source/leveldb/util/arena.h#L22-L66)

## Status 与兼容

POSIX ENOENT 映射为 NotFound，其余常见系统错误映射为 IOError。[util/env_posix.cc:60-66](../../../source/leveldb/util/env_posix.cc#L60-L66) `RemoveFile/DeleteFile`、`RemoveDir/DeleteDir` 互相兼容转发，新的实现应覆盖现代名称。[include/leveldb/env.h:120-161](../../../source/leveldb/include/leveldb/env.h#L120-L161)、[util/env.cc:20-28](../../../source/leveldb/util/env.cc#L20-L28)

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
