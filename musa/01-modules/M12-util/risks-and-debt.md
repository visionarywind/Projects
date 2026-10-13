# Util 基础库：风险与技术债

- 文档目的：util：风险、技术债及源码证据与修复/验证建议。
- 适用范围：M12 模块的《Util 基础库：风险与技术债》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`risks-and-debt.md`](risks-and-debt.md)


本模块是全库缺陷密度最高的部分。以下条目均定位到具体行号，按影响排序。**均只记录，不修改源码。**

## 严重

### 1. `SplayTree::Search` 递归方向反转

[src/util/utilSplayTree.h:218-221]：

```cpp
if (node->m_Key < key) {
    node = Search(node->m_pLeft, key);   // 错：应走 m_pRight
} else if (node->m_Key > key) {
    node = Search(node->m_pRight, key);  // 错：应走 m_pLeft
}
```

当前节点 key **小于**目标时却进入左子树，而左子树只含更小的 key，**永远不可能到达更大的 key**。只有当目标恰好与根节点相等时才可能返回正确结果。

`Search` 只在目标键与当前节点键相等时返回该节点（两个 `<`/`>` 分支都会继续递归），其余情况一路下探至哨兵节点 `GetNull()` 并返回它。因此对**已存在但不在错误路径上**的键，`Find` 同样返回空。

**影响路径**：这是生产路径。`MemMgr::FindPool` 经 `m_PoolRefs.Find(...)` 查找内存池 [src/hal/m3d/memMgr.cpp:43]，即 M09 HAL 的内存池定位依赖此函数。树类型定义见 [src/hal/m3d/memMgr.h:37]，成员见 [src/hal/m3d/memMgr.h:125]。

注意 `Get` 不受分支反转影响：`Get` 走独立的 `Splay` 算法 [src/util/utilSplayTree.h:90-93]，比较方向正确。但 `Get` 的失败语义是"最近节点"（设计注释 [src/util/utilSplayTree.h:87-88]）：空树返回 `&s_Null`（`m_Value` 为 0，[src/util/utilSplayTree.h:63]、[src/util/utilSplayTree.h:83-84]），缺失键返回最近节点。它**从不返回 `nullptr`**。

### 2. `Find` 返回 `nullptr` 与 `Get` 返回"最近节点"，M09 调用方均未消化

`Find` 未命中时经 [src/util/utilSplayTree.h:99] 返回 `End()`，即 `nullptr` [src/util/utilSplayTree.h:70]。结合风险 1，`Find` 对已存在的键也 miss。

M09 的 `MemMgr` 有四处直接 `->m_Value`，失败形态各不相同：

| 位置 | 调用 | 失败形态 | 防护 |
|---|---|---|---|
| [src/hal/m3d/memMgr.cpp:38] | `Get(...)->m_Value` | 空树返回空池；错键返回最近池 | 无 |
| [src/hal/m3d/memMgr.cpp:43] | `Find(...)->m_Value` | **`Find` 返回 `nullptr`，解引用即崩** | 无 |
| [src/hal/m3d/memMgr.cpp:116] | `Get(...)->m_Value` | 错键返回最近池 | `if (!pPool)` [src/hal/m3d/memMgr.cpp:117] 只拦空值；错池靠 [src/hal/m3d/memMgr.cpp:117-121] 字段比对兜底 |
| [src/hal/m3d/memMgr.cpp:158] | `Get(...)->m_Value` | 错池 / 空值 | `MUSA_ASSERT` [src/hal/m3d/memMgr.cpp:159] 仅 debug，release 被裁剪 |

完整根因分析见 [`../../99-roadmap/rca-splaytree-memmgr.md`](../../99-roadmap/rca-splaytree-memmgr.md)。

### 3. `SplayTree` 泄漏全部节点

`~SplayTree() {}` 为空 [src/util/utilSplayTree.h:64]。HAL 的 `MemMgr` 靠手工遍历 `Delete(key, true)` 排空节点 [src/hal/m3d/memMgr.cpp:25-28] 绕过该问题——但任何其他使用者都会泄漏。

### 4. `IsPowerOfTwo(0)` 返回 `true`

[src/util/utilMath.h:18]：`0 & (0-1) == 0`，即 `0 == 0`。调用方包括 `IsMultipleOf` [src/util/utilMath.h:33] 与 `RoundUpToNextPowerOfTwo` [src/util/utilMath.h:77]。

## 高危

### 5. `Executor::QueueTask` 在关闭后返回无效 future

[src/util/utilExecutor.h:53-62]：若 `m_stop` 已置位，锁块被跳过，函数返回一个**默认构造的 `std::future`** [src/util/utilExecutor.h:51]、[src/util/utilExecutor.h:62]。调用方 `.get()` 会抛 `std::future_error(broken_promise)`。表面上是成功入队，实际是难以理解的失败模式。

### 6. `TimeStatistics` 嵌套使用互相覆盖

`m_threadLocalStartTime` 是 `static thread_local` [src/util/perfStatistics.h:26]，同一线程上两个重叠的 `TimeStatistics` 作用域共享一个起始时刻，统计结果错误。

此外 [src/util/perfStatistics.cpp:28-30] 把 `m_callCount.load()` 非原子地读三次作为一对使用，并发 `End()` 可使 `total/count` 不一致；[src/util/perfStatistics.cpp:25] 判定 `count > 0` 后再读取，若期间被修改则除零防护失效。

### 7. `Library::LoadPath` 在 Windows 上忽略 `LoadMode`

[src/util/utilLibrary.h:59-60] 只把 `nativeFlags` 传给 `LoadLibraryExW`，`mode` 参数被接收后丢弃。`Load` 同理 [src/util/utilLibrary.h:45-47]。`src/loader/loader.cpp:53` 传入的 `Now | DeepBind` 在 Windows 分支无对应行为。

### 8. `Log::Setting::Get()` 中的 `std::stoul` 可抛异常

[src/util/utilLog.h:97]：`MUSA_LOG` 格式非法时抛 `std::invalid_argument` / `std::out_of_range`。该调用位于函数内 static 初始化器中 [src/util/utilLog.h:95-98]，异常会从 `tprintf` [src/util/utilLog.h:224] 逃逸到任意 API 调用路径。

同时“每进程只读一次”的缓存语义使 `MUSA_LOG` 在首次使用后无法更改，对诊断类环境变量而言反直觉。

### 9. `Library::Load` 把 `string_view::data()` 直接交给加载函数

[src/util/utilLibrary.h:47]、[src/util/utilLibrary.h:53] 传 `.data()` 给 `LoadLibraryA`/`dlopen`。`string_view::data()` **不保证 NUL 结尾**，非终止视图会越界读取。`LoadPath` 安全（`fs::path::c_str()` 有终止符），`Load` 不安全。

### 10. `GetError` 尾部空白处理错误

[src/util/utilLibrary.h:129-131]：`lastPos = find_last_not_of("\r\n")` 后取 `substr(0, lastPos + 1)`，把刚剥离的换行符又加了回去。应为 `lastPos`。另有 [src/util/utilLibrary.h:116-142] 在 `FormatMessageA` 返回 size 为 0 时会分配缓冲区却产出空消息，而非预期的 `"No error"` 回退。

## 中危

### 11. `ParseCoreDumpTemplate` 的 `%h` 每次调用都做 WSAStartup/WSACleanup

[src/util/utilLog.h:158-165]：Winsock 的初始化计数是**进程全局**且非线程安全，按调用配对属于错误用法。

### 12. `ObjectPool::Borrow` 返回值无空检查

两个调用点 `src/musa/core/graphKernelNode.cpp:15` 与 `src/musa/core/command/dispatchCommand.cpp:1354` 均直接对返回值做 placement new。而 `Buffer` 在 [src/util/utilObjectPool.h:94] 由 `new` 分配，可抛 `std::bad_alloc`。

`Buffer::GetBlock` [src/util/utilObjectPool.h:57-62] 在 `uint8_t` 数组上做 `reinterpret_cast<T*>`，正确性依赖 `alignas(T)` [src/util/utilObjectPool.h:65] 与 `kBucketSize >= sizeof(T)` [src/util/utilObjectPool.h:50-51]，二者均无断言。

### 13. `ObjectPool::Return` 无任何校验

[src/util/utilObjectPool.h:101-107] 通过任意 `T*` 重解释为 `FreeList*` 写入并无条件递减 `mAllocated` [src/util/utilObjectPool.h:103]，重复归还会使其下溢到 `SIZE_MAX`。

### 14. `util` 未设 PIC 且未链 `Threads::Threads`

[src/util/CMakeLists.txt:2] 的 STATIC 目标未设置位置无关代码属性，无法链入 SHARED 库——这正是 M11 无法链接它的原因（见 [`../M11-loader/risks-and-debt.md`](../M11-loader/risks-and-debt.md) 风险 3）。

[src/util/CMakeLists.txt:4] 只链接 `dl`，而 [src/util/utilExecutor.h:21] 创建 `std::thread`。当前能工作仅靠 `halM3d` 自身的 pthread 链接（`src/hal/m3d/CMakeLists.txt:59`）。

### 15. `SplayTree::s_Null` 是可变共享哨兵

[src/util/utilSplayTree.h:78] 定义、[src/util/utilSplayTree.h:84] 实例化，[src/util/utilSplayTree.h:73] 用 `const_cast` 去掉 const。跨 DSO 的两个 `SplayTree<K,V>` 实例会各自持有不同哨兵，却与各自编译进库的 `&s_Null` 比较——潜在的 ODR/ABI 隐患。

节点默认构造的 `reinterpret_cast<V>(s_UndefinedVal)` [src/util/utilSplayTree.h:15] 仅对指针类 `V` 可编译。

## 低危

| 问题 | 证据 |
|---|---|
| `perfStatistics.{h,cpp}` 整体未被引用，却经 glob 编入 `libutil.a`，并把 `<iostream>` 和 `static thread_local` 拖进每个链接该归档的翻译单元 | [src/util/CMakeLists.txt:1-2]、[src/util/perfStatistics.h:5]、[src/util/perfStatistics.cpp:5] |
| `SteadyClock` 完全未使用 | [src/util/utils.h:33-52] |
| `ToHexString` 无调用方 | [src/util/logging.h:7] |
| `~TimeStatistics` 无条件 `std::cout` 输出，无开关无环境变量门控 | [src/util/perfStatistics.h:13]、[src/util/perfStatistics.cpp:24] |
| `ObjectPool::Capacity()`/`FreeCount()` 在 TTAS 自旋锁下遍历整个结构（O(n)） | [src/util/utilObjectPool.h:110-117]、[src/util/utilObjectPool.h:119-126] |
| `mAllocated` 只在锁内写、无任何读取方 | [src/util/utilObjectPool.h:90]、[src/util/utilObjectPool.h:97]、[src/util/utilObjectPool.h:103]、[src/util/utilObjectPool.h:137] |
| `Executor::m_stop` 是普通 `bool` 而非 `atomic`，当前正确但类型 invites 误用 | [src/util/utilExecutor.h:87] |
| `AlignDown`/`AlignUp`/`IsMultipleOf` 静默要求 2 的幂对齐，无断言 | [src/util/utilMath.h:23]、[src/util/utilMath.h:28]、[src/util/utilMath.h:31-38] |
| `AlignedAlloc` 用 `std::aligned_alloc`，未校验 size 是 alignment 的整数倍（C11/C++17 要求） | [src/util/utilOs.h:70-76] |
| `Unload`/`FreeLibrary` 返回值被忽略；`Reload` 先 `Unload` 后 `Load`，失败后句柄为空且无诊断 | [src/util/utilLibrary.h:71-77]、[src/util/utilLibrary.h:153-161] |
| `utilMath.h` 文件头注释误写为 `@file utilOs.h` | [src/util/utilMath.h:2] |
| `utils.h` 重复 `#include <cassert>` | [src/util/utils.h:3]、[src/util/utils.h:8] |
| `logging.h`/`utils.h` 污染全局命名空间（`ToString`、`fatal`、`MUSA_ASSERT`、`LIKELY` 等） | [src/util/logging.h:7-30]、[src/util/utils.h:16-29] |
| `GetPath` 内的 `fs::canonical` 会抛异常，且可达路径上存在 `noexcept` 调用方 | [src/util/utilLibrary.h:111] |
| `GetError` 无错误时返回字面量 `"No error"`，无法区分“无错误”与“无法确定” | [src/util/utilLibrary.h:116-142] |

## 未解决问题

1. `SplayTree` 的正确性缺陷是否已被 `memMgr` 的调用方式掩盖，需读 `src/hal/m3d/memMgr.cpp` 全文确认。
2. `Splay`（自顶向下）[src/util/utilSplayTree.h:105-142] 的旋转逻辑是否也有方向问题未验证。
3. `Executor` 是否在 M03 中被多线程并发提交任务（当前只有单线程池设计）未确认。

## 相关文档

- [`README.md`](README.md)
- [`call-chains.md`](call-chains.md)
- [`risks-and-debt.md`](risks-and-debt.md)
- [`../module-registry.md`](../module-registry.md)

## 源码证据摘要

- `src/util/utilSplayTree.h`（13 处，例：[src/util/utilSplayTree.h:218-221]）
- `src/util/utilObjectPool.h`（12 处，例：[src/util/utilObjectPool.h:94]）
- `src/util/utilLibrary.h`（10 处，例：[src/util/utilLibrary.h:59-60]）

## 下一步阅读建议

阅读 [`risks-and-debt.md`](risks-and-debt.md)。
