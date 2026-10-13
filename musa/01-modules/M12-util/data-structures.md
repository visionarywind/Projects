# Util 基础库：数据结构与关键实现

- 文档目的：util：核心数据结构、字段含义与持有关系。
- 适用范围：M12 模块的《Util 基础库：数据结构与关键实现》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)


## `Util::Library`（[src/util/utilLibrary.h:25-179]）

RAII 动态库句柄。

| 项 | 内容 | 证据 |
|---|---|---|
| 私有成员 | `NativeHandle m_handle` | [src/util/utilLibrary.h:178] |
| `NativeHandle` | `HMODULE`（Win）/ `void*`（Linux） | [src/util/utilLibrary.h:40]、[src/util/utilLibrary.h:42] |
| 拷贝/移动 | 两者均禁止 | [src/util/utilLibrary.h:148-151] |
| 析构 | `Unload(m_handle)` | [src/util/utilLibrary.h:146] |
| 判定 | `operator bool()` | [src/util/utilLibrary.h:176] |

`LoadMode` 位掩码 [src/util/utilLibrary.h:28-37]：`Lazy`、`Now`、`Global`、`DeepBind`，配 `operator|`。

## `Util::Executor`（[src/util/utilExecutor.h:13-89]）

**只有一个**工作线程的极简线程池，不是多线程池。

| 访问 | 成员 | 证据 |
|---|---|---|
| private | `std::thread m_thread` | [src/util/utilExecutor.h:84] |
| private | `std::mutex m_mtx` | [src/util/utilExecutor.h:85] |
| private | `std::condition_variable m_cv` | [src/util/utilExecutor.h:86] |
| private | `bool m_stop`（非原子） | [src/util/utilExecutor.h:87] |
| private | `std::queue<std::function<void()>> m_tasks` | [src/util/utilExecutor.h:88] |

- 工作线程在**构造函数**中创建 [src/util/utilExecutor.h:21]。
- 析构调 `Shutdown()` 并 join [src/util/utilExecutor.h:24-26]。
- 以值成员形式嵌入 `src/musa/core/platform.h:171` 的 `Util::Executor m_Executor`。
- `QueueTask` 返回 `std::future` [src/util/utilExecutor.h:42]，内部用 `std::packaged_task` [src/util/utilExecutor.h:45-49]。

## `Musa::ObjectPool<T>`（[src/util/utilObjectPool.h:44-139]）

分块侵入式空闲链表。

| 访问 | 成员 | 证据 |
|---|---|---|
| private | `FreeList* mFreeList` | [src/util/utilObjectPool.h:68] |
| private | `Buffer* mBufferHead` | [src/util/utilObjectPool.h:69] |
| private | `size_t mBufferIndex` | [src/util/utilObjectPool.h:70] |
| private | `size_t mAllocated` | [src/util/utilObjectPool.h:71] |
| private | `mutable SpinLock mLock` | [src/util/utilObjectPool.h:72] |
| 嵌套 | `FreeList`（`FreeList* mNext`） | [src/util/utilObjectPool.h:45-47] |
| 嵌套 | `Buffer`（`Buffer* const mNext`；`alignas(T) uint8_t mData[...]`） | [src/util/utilObjectPool.h:53-66] |
| 常量 | `kPoolGrowSize = 1<<7` | [src/util/utilObjectPool.h:49] |
| 常量 | `kBucketSize` | [src/util/utilObjectPool.h:50-51] |

- 不可拷贝**且不可移动** [src/util/utilObjectPool.h:76-79]。
- `Borrow()` **不执行 placement new**，调用方须自行构造。两个真实调用点均如此，且都**未检查返回指针**：`src/musa/core/graphKernelNode.cpp:15`、`src/musa/core/command/dispatchCommand.cpp:1354`。
- `ShardedPool<T>` [src/util/utilObjectPool.h:145-167] 按值持有 64 个分片 [src/util/utilObjectPool.h:166]，经 `std::unique_ptr` 挂在 `src/musa/core/device.h:70-71`，创建于 `src/musa/core/device.cpp:710-711`。

## `Musa::SpinLock`（[src/util/utilObjectPool.h:14-34]）

Test-and-test-and-set，使用 `acquire`/`release` 内存序 [src/util/utilObjectPool.h:22]、[src/util/utilObjectPool.h:29]。成员 `std::atomic<bool> mFlag{false}` [src/util/utilObjectPool.h:33]，不可拷贝 [src/util/utilObjectPool.h:18-19]。

## `Util::SplayTree<K,V>`（[src/util/utilSplayTree.h:61-81]）

| 项 | 内容 | 证据 |
|---|---|---|
| static 哨兵 | `s_Null`（定义于 [src/util/utilSplayTree.h:84]） | [src/util/utilSplayTree.h:78] |
| 根指针 | `m_pRoot`，构造时初始化为 `&s_Null` | [src/util/utilSplayTree.h:63]、[src/util/utilSplayTree.h:80] |
| 析构 | **空实现** `~SplayTree() {}` | [src/util/utilSplayTree.h:64] |
| 未命中返回 | `End()` 返回 `nullptr` | [src/util/utilSplayTree.h:70] |
| 取哨兵 | `GetNull()` 用 `const_cast` 去掉 const | [src/util/utilSplayTree.h:73] |

节点 `Util::SplayTreeNode<K,V>` [src/util/utilSplayTree.h:12-37] 用裸指针 `m_pLeft`/`m_pRight` [src/util/utilSplayTree.h:35-36]，`~SplayTreeNode()` 为空 [src/util/utilSplayTree.h:25]，**节点不持有任何资源**。`Delete(key, deleteValue)` 可选地 `delete m_Value` [src/util/utilSplayTree.h:201]。

哨兵常量：`s_UndefinedKey = UINT64_MAX` [src/util/utilSplayTree.h:30]、`s_UndefinedVal = 0` [src/util/utilSplayTree.h:31]。节点默认构造用 `reinterpret_cast<V>(s_UndefinedVal)` [src/util/utilSplayTree.h:15]，**仅对指针类 `V` 可编译**。

`Search` 递归体 [src/util/utilSplayTree.h:216-226]：

```cpp
if (node->m_Key < key) {
    node = Search(node->m_pLeft, key);   // 应为 m_pRight
} else if (node->m_Key > key) {
    node = Search(node->m_pRight, key);  // 应为 m_pLeft
}
```

## `Util::Log::ThreadInfo`（[src/util/utilLog.h:26-55]）

| 成员 | 类型 | 证据 |
|---|---|---|
| `m_shortId` | `int` | [src/util/utilLog.h:49] |
| `m_pid` | `Os::Tid` | [src/util/utilLog.h:50] |
| `m_tid` | `Os::Tid` | [src/util/utilLog.h:51] |
| `m_apiSeqNum` | `uint32_t` | [src/util/utilLog.h:52] |
| `LastShortId` | `static inline std::atomic<int>{0}` | [src/util/utilLog.h:54] |

`Get()` 返回函数内 `thread_local static` 实例 [src/util/utilLog.h:28-31]，每线程一份，线程退出时销毁。`LastShortId` 是进程级原子计数器 [src/util/utilLog.h:41]、[src/util/utilLog.h:54]。

## `Util::TimeStatistics`（[src/util/perfStatistics.h:9-27]）

| 成员 | 类型 | 证据 |
|---|---|---|
| `m_pName` | `const char*`（**非拥有**） | [src/util/perfStatistics.h:22] |
| `m_callCount` | `std::atomic<int64_t>` | [src/util/perfStatistics.h:23] |
| `m_totalDuration` | `std::atomic<int64_t>` | [src/util/perfStatistics.h:24] |
| `m_threadLocalStartTime` | `static thread_local` time_point | [src/util/perfStatistics.h:26] |

`m_pName` 不拥有字符串，调用方须保证其生命周期。析构函数无条件调 `PrintStatistics()` [src/util/perfStatistics.h:13]、[src/util/perfStatistics.cpp:24]。

## `SteadyClock`（[src/util/utils.h:33-52]）

值类型，成员 `_start` [src/util/utils.h:51]。构造函数调 `SetStartTime()` [src/util/utils.h:35]。`GetDuration<D>()` [src/util/utils.h:41-44] 与 `GetDurationCount<D>()` [src/util/utils.h:47-49] 非 `const` 但类内定义，隐式 inline。

## `Utils`（[src/util/utils.h:54-124]）

无实例的纯静态类，只有 `iReadEnv<T>` [src/util/utils.h:58-94] 与 `iPrintSetting<T>` [src/util/utils.h:97-123]。

## 相关文档

- [`README.md`](README.md)
- [`call-chains.md`](call-chains.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/util/utilObjectPool.h`（18 处，例：[src/util/utilObjectPool.h:44-139]）
- `src/util/utilSplayTree.h`（16 处，例：[src/util/utilSplayTree.h:61-81]）
- `src/util/utilExecutor.h`（10 处，例：[src/util/utilExecutor.h:13-89]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`risks-and-debt.md`](risks-and-debt.md)。

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
