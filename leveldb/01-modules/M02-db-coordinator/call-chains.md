# M02 调用链

- 文档目的：给出 DBImpl 的初始化、正常、异常、清理和后台路径。
- 适用范围：`db_impl.cc`。
- 证据状态：主要节点已确认。
- 最后更新：2026-09-10
- 前置阅读：[M02 README](README.md)
- 后续阅读：[跨模块调用链](../../90-cross-module/cross-module-call-chains.md)

## 初始化/恢复

```text
DB::Open (1503)
  -> new DBImpl
  -> DBImpl::Recover (292)
    -> NewDB (180) 或 VersionSet::Recover
    -> RecoverLogFile (385)
      -> log::Reader::ReadRecord
      -> WriteBatchInternal::InsertInto
    -> WriteLevel0Table (505)
```

## 写入

```text
DBImpl::Write (1206)
  -> BuildBatchGroup (1280)
  -> MakeRoomForWrite (1331)
  -> log::Writer::AddRecord
  -> WriteBatchInternal::InsertInto
  -> MaybeScheduleCompaction
```

## 读取

```text
DBImpl::Get (1121)
  -> LookupKey(snapshot)
  -> MemTable::Get(mem_/imm_)
  -> Version::Get
    -> TableCache::Get
```

## 后台/清理

```text
MaybeScheduleCompaction (668)
  -> Env::Schedule(BGWork)
    -> BackgroundCall (689)
      -> CompactMemTable 或 BackgroundCompaction
        -> DoCompactionWork (898)
        -> InstallCompactionResults (880)
```

每个 `->` 表示直接调用或明确调度；`Schedule` 后线程上下文可能变化。[db/db_impl.cc:668-708](../../../source/leveldb/db/db_impl.cc#L668-L708)

## 相关文档

- [line-level-analysis](line-level-analysis.md)
- [diagrams](diagrams.md)

## 源码证据摘要

见各节点行号。

## 未解决问题

具体调度线程由 Env 决定。

## 下一步阅读建议

配合 mutex 注解逐条确认锁状态。
