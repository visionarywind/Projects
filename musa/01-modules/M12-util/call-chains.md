# Util 基础库：调用链

- 文档目的：util：关键调用链：从公共入口到真实副作用的追踪。
- 适用范围：M12 模块的《Util 基础库：调用链》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：部分推断（引用较少，证据密度不足）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)


## 链 1：loader 加载平台 thunk（M11 → M12）

```
PlatformThunks::Load                 src/loader/driver_resolver.cpp:71
  -> Util::Library::ReloadPath       src/util/utilLibrary.h:158
    -> Util::Library::Unload         src/util/utilLibrary.h:70
    -> Util::Library::LoadPath       src/util/utilLibrary.h:57
      -> LoadLibraryExW              src/util/utilLibrary.h:60     （Win）
      -> dlopen                      src/util/utilLibrary.h:66     （WSL）
  -> PlatformThunks::Resolve         src/loader/driver_resolver.cpp:89
    -> Util::Library::GetSymbol      src/util/utilLibrary.h:172
      -> Util::Library::GetSymbol    src/util/utilLibrary.h:85
        -> GetProcAddress            src/util/utilLibrary.h:87     （Win）
        -> dlsym                     src/util/utilLibrary.h:89     （WSL）
```

## 链 2：loader 解析驱动导出符号

```
ResolveDriverExports                 src/loader/loader.cpp:146
  -> ResolveFunction                 src/loader/loader.cpp:138
    -> Util::Library::GetSymbol<T>   src/util/utilLibrary.h:85
      -> GetProcAddress / dlsym      src/util/utilLibrary.h:87 / :89
  -> ResolveData                     src/loader/loader.cpp:142
    -> Util::Library::GetSymbol<T*>  src/util/utilLibrary.h:85
```

## 链 3：完整驱动模块加载

```
LoadDriverModule                     src/loader/loader.cpp:47
  -> Util::Library::LoadPath         src/util/utilLibrary.h:57
    -> LoadLibraryExW                src/util/utilLibrary.h:60   （Win，安全搜索标志来自 src/loader/loader.cpp:51）
    -> dlopen                        src/util/utilLibrary.h:66   （WSL，RTLD_NOW|RTLD_LOCAL|RTLD_DEEPBIND 来自 src/loader/loader.cpp:53）
```

## 链 4：日志

```
tprintf(flag, fmt, ...)              src/util/utilLog.h:222
  -> Util::Log::Contains             src/util/utilLog.h:103
    -> Util::Log::Setting::Get       src/util/utilLog.h:94
      -> std::getenv("MUSA_LOG")     src/util/utilLog.h:96
  -> Util::Log::GetPrefix<flag>      src/util/utilLog.h:136
    -> Util::ThreadInfo::Get         src/util/utilLog.h:28
      -> Util::Os::GetProcessId      src/util/utilOs.h:56
      -> Util::Os::GetThreadId       src/util/utilOs.h:57
  -> Util::Log::GetSystemTime        src/util/utilLog.h:107
    -> localtime_r                   src/util/utilLog.h:117
  -> std::fprintf(stderr, ...)       src/util/utilLog.h:225
```

输出目标是 **stderr**，由 `UNLIKELY(Contains(flag))` 门控 [src/util/utilLog.h:222]。

## 链 5：对象池分配（M03 → M12）

```
ShardedPool<T>::Borrow(stream)       src/util/utilObjectPool.h:149
  -> HashShard                       src/util/utilObjectPool.h:160
    -> ObjectPool<T>::Borrow         src/util/utilObjectPool.h:85
      -> std::lock_guard<SpinLock>   src/util/utilObjectPool.h:86
      -> Buffer::GetBlock            src/util/utilObjectPool.h:57
```

两个真实调用点：

```
GraphKernelNode::Create              src/musa/core/graphKernelNode.cpp:14
  -> dev->GetGraphKernelNodePool()->Borrow(stream)
  -> new (mem) GraphKernelNode(...)  src/musa/core/graphKernelNode.cpp:15

dispatchCommand.cpp:1354             src/musa/core/command/dispatchCommand.cpp:1354
  -> Borrow() -> placement new -> 显式析构 -> Return()
```

## 链 6：单线程执行器（M03 → M12）

```
Executor::QueueTask                  src/util/utilExecutor.h:42
  -> std::make_shared<std::packaged_task<ReturnType()>>  src/util/utilExecutor.h:45
  -> std::lock_guard / unique_lock   src/util/utilExecutor.h:53
  -> m_tasks.emplace                 src/util/utilExecutor.h:57
  -> m_cv.notify_all                 src/util/utilExecutor.h:59

Executor::Worker                     src/util/utilExecutor.h:66
  -> m_cv.wait(lock, pred)           src/util/utilExecutor.h:69
  -> (*newTask)()                    src/util/utilExecutor.h:79
    -> packaged_task 捕获异常并写入 future 共享状态
    -> fut.get() 返回
```

## 链 7：Splay 树查找（M09 → M12）

```
MemMgr::FindPool                     src/hal/m3d/memMgr.cpp:41
  -> m_PoolRefs.Find(MakeKey(...))   src/util/utilSplayTree.h:97
    -> Search                        src/util/utilSplayTree.h:216
      -> （递归方向反转，见 risks-and-debt.md 风险 1）
```

## 链 8：计时

```
TimeStatistics::Start                src/util/perfStatistics.cpp:12
TimeStatistics::End                  src/util/perfStatistics.cpp:17
  -> m_totalDuration.fetch_add       src/util/perfStatistics.cpp:20
  -> m_callCount.fetch_add           src/util/perfStatistics.cpp:21
~TimeStatistics                      src/util/perfStatistics.h:13
  -> PrintStatistics                 src/util/perfStatistics.cpp:24
    -> std::cout 输出 CSV: name,count,total,avg   src/util/perfStatistics.cpp:27-31
```

## 链 9：环境变量读取（M03 → M12）

```
platformSettings 初始化              src/musa/core/platformSettings.cpp:9-34 起（48 处调用）
  -> Utils::iReadEnv<T>              src/util/utils.h:58
    -> getenv(var_name1)             src/util/utils.h:62
    -> getenv(var_name2) 仅当 name1 未定义且 name2 != "0"   src/util/utils.h:64-66
```

## 未被使用的设施

以下经检索确认在 `src/util/` 之外**无任何引用**：

| 设施 | 证据 |
|---|---|
| `perfStatistics.{h,cpp}` 整个文件 | 仅 `src/util/CMakeLists.txt:1` 的 glob 把它编入 `libutil.a` |
| `SteadyClock` | [src/util/utils.cpp:9-11] 的唯一目的就是为它定义 `SetStartTime` |
| `logging.h` 的 `ToHexString` | [src/util/logging.h:7] 无外部调用方 |

`utilExecutor.h` 声明的“线程池”实际只有一个工作线程；多模式退避在 `utilSched.h`，不在此处。

## 相关文档

- [`README.md`](README.md)
- [`call-chains.md`](call-chains.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/util/utilLog.h`（1 处，例：[src/util/utilLog.h:222]）
- `src/util/utils.cpp`（1 处，例：[src/util/utils.cpp:9-11]）
- `src/util/logging.h`（1 处，例：[src/util/logging.h:7]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
