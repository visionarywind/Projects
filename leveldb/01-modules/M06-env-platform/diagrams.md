# M06 图示

- 文档目的：展示 Env 分层、资源限额和 Cache 引用状态。
- 适用范围：M06。
- 源码版本：`main` / `7ee830d`。
- 证据状态：图示根据源码和公共契约整理。
- 最后更新：2026-09-10
- 前置阅读：[call-chains](call-chains.md)
- 后续阅读：[testing](testing.md)

## Env 分层

```mermaid
flowchart TB
  C[DBImpl/TableCache] --> E[Env interface]
  E --> P[POSIX Env]
  E --> W[Windows Env]
  E --> M[MemEnv test double]
  P --> FD[fd/pread/mmap]
  P --> TH[std::thread/condition_variable]
  W --> OS[Windows file/thread APIs]
```

## 随机文件资源降级

```mermaid
flowchart TD
  O[Open random file] --> A{fd limiter Acquire}
  A -->|success| H[retain fd]
  A -->|failure| N[retain filename]
  N --> R[each Read: open -> pread -> close]
  H --> R2[each Read: pread retained fd]
```

## Cache 引用状态

```mermaid
stateDiagram-v2
  [*] --> InUse: Insert returns handle
  InUse --> LRU: external Release, cache retains ref
  LRU --> InUse: Lookup/Ref
  LRU --> Evicted: capacity/Erase
  Evicted --> Freed: refs == 0 -> deleter
  InUse --> Freed: Erase + final Release
```

`LRUCache` 明确维护 in-use 与 LRU 两条链，避免活跃客户端持有的对象被提前释放。[util/cache.cc:21-38](../../../source/leveldb/util/cache.cc#L21-L38)

## 锁边界

```mermaid
sequenceDiagram
  participant DB as DBImpl
  participant E as Env
  participant T as Worker
  DB->>DB: hold mutex while scheduling state update
  DB->>E: Schedule(callback)
  E->>T: invoke callback unspecified thread
  T->>DB: callback reacquires DB mutex
```

Env 不替 DBImpl 管理共享状态；任务并发约束必须由调用方承担。
