# M06 接口与契约

- 文档目的：记录 Env、文件对象和 Cache 的线程、所有权和错误契约。
- 适用范围：M06。
- 源码版本：`main` / `7ee830d`。
- 证据状态：公共头文件契约已确认。
- 最后更新：2026-09-10
- 前置阅读：[M06 design](design.md)
- 后续阅读：[M06 data structures](data-structures.md)

## Env

- `Env::Default()` 返回由 LevelDB 所有、不得 delete 的单例/默认对象。[include/leveldb/env.h:59-64](../../../../include/leveldb/env.h#L59-L64)
- `NewSequentialFile` 返回的对象只由一个线程访问；`NewRandomAccessFile` 返回对象可被多线程并发读取；WritableFile、AppendableFile 由一个线程访问。[include/leveldb/env.h:66-109](../../../../include/leveldb/env.h#L66-L109)
- LockFile 不等待已有锁；成功后必须 UnlockFile，进程退出时锁自动释放。[include/leveldb/env.h:170-189](../../../../include/leveldb/env.h#L170-L189)
- `Schedule` 的任务可能并发执行，调用者不能假定同一 Env 串行。[include/leveldb/env.h:191-201](../../../../include/leveldb/env.h#L191-L201)

## File ownership

成功创建文件对象后，调用者拥有返回指针并负责 delete；`TableCache`、DBImpl 等上层对象负责保持所需文件在借用期间有效。`Read` 输出的 Slice 可能指向 caller 提供的 scratch，scratch 必须保持存活到 Slice 不再使用。[include/leveldb/env.h:230-238](../../../../include/leveldb/env.h#L230-L238)、[include/leveldb/env.h:260-270](../../../../include/leveldb/env.h#L260-L270)

## Cache

Insert/Lookup 返回的 handle 必须恰好 Release；Value 只能在 handle 未释放时使用；Erase 只从查找表中移除，底层 value 要等所有 handle 释放。[include/leveldb/cache.h:44-80](../../../../include/leveldb/cache.h#L44-L80)

## Arena

`Allocate` 禁止 0 字节，返回指针由 Arena 持有；调用者不能单独 delete，Arena 析构统一释放 blocks。[util/arena.h:22-66](../../../../util/arena.h#L22-L66)

## Status 与兼容

POSIX ENOENT 映射为 NotFound，其余常见系统错误映射为 IOError。[util/env_posix.cc:60-66](../../../../util/env_posix.cc#L60-L66) `RemoveFile/DeleteFile`、`RemoveDir/DeleteDir` 互相兼容转发，新的实现应覆盖现代名称。[include/leveldb/env.h:120-161](../../../../include/leveldb/env.h#L120-L161)、[util/env.cc:20-28](../../../../util/env.cc#L20-L28)
