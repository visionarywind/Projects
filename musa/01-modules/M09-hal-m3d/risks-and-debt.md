# HAL/M3D 适配：风险与技术债

- 文档目的：hal m3d：风险、技术债及源码证据与修复/验证建议。
- 适用范围：M09 模块的《HAL/M3D 适配：风险与技术债》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`README.md`](README.md)、[`../module-registry.md`](../module-registry.md)
- 后续阅读：[`design.md`](design.md)、[`risks-and-debt.md`](risks-and-debt.md)


以下条目均定位到具体行号。**只记录，不修改源码。**

## 严重

### 1. `FindPool` 无条件解引用 `Find` 结果，`Find` 对已存在键也 miss，调用即崩

```cpp
IMemoryPool* MemMgr::FindPool(const MemoryPoolInfo& poolInfo) {
    std::lock_guard<std::mutex> lg(m_Lock);
    return m_PoolRefs.Find(MakeKey(poolInfo))->m_Value;  // src/hal/m3d/memMgr.cpp:43
}
```

`Util::SplayTree::Find` 对缺失键返回 `nullptr`（[src/util/utilSplayTree.h:97-100]），且因 `Search` 分支反转对**大部分已存在键也 miss**。`->m_Value` 解引用空指针是必然崩溃。根因与分析见 [`../../99-roadmap/rca-splaytree-memmgr.md`](../../99-roadmap/rca-splaytree-memmgr.md)。

**注意 `Get` 与 `Find` 语义不同**：`Get` 走独立的 `Splay` 路径（[src/util/utilSplayTree.h:90-93]），**从不返回 `nullptr`**——空树返回 `&s_Null`（`m_Value` 为 0，见 [src/util/utilSplayTree.h:13-17]），缺失键返回"最近节点"。因此 `Get` 的失败形态是**返回空值或错误的池**，而非崩溃。四处查找的实际风险如下：

| 位置 | 调用 | 失败形态 | 防护 |
|---|---|---|---|
| [src/hal/m3d/memMgr.cpp:38] | `Get(...)->m_Value` | 空树返回空池；错键返回最近池 | 无 |
| [src/hal/m3d/memMgr.cpp:43] | `Find(...)->m_Value` | **`Find` 返回 `nullptr`，解引用即崩** | 无 |
| [src/hal/m3d/memMgr.cpp:116] | `Get(...)->m_Value` | 错键返回最近池 | `if (!pPool)` [src/hal/m3d/memMgr.cpp:117] 只拦空值；错池靠 [src/hal/m3d/memMgr.cpp:117-121] 五字段比对兜底 |
| [src/hal/m3d/memMgr.cpp:158] | `Get(...)->m_Value` | 错池 / 空值 | `MUSA_ASSERT` [src/hal/m3d/memMgr.cpp:159] 仅 debug 生效，release 被裁剪 |

### 2. `SplayTree::Search` 两个递归分支方向互换，`Find` 对已存在键也 miss

```cpp
if (node->m_Key < key) {
    node = Search(node->m_pLeft, key);    // 应走右子树
} else if (node->m_Key > key) {
    node = Search(node->m_pRight, key);   // 应走左子树
}
```

[src/util/utilSplayTree.h:216-225]。`node->m_Key < key` 说明目标键更大，正确下一跳是右子树，代码却进左子树；反之亦然。查找总是背离目标键下降，最终在哨兵节点 `GetNull()` 停止。

`Find` 把哨兵转成 `nullptr`（[src/util/utilSplayTree.h:99]、[src/util/utilSplayTree.h:70]）。因此 `FindPool` 对**大部分已存在的键也返回空指针**。

影响范围：`MemPoolTree` 类型定义在 [src/hal/m3d/memMgr.h:37]，成员在 [src/hal/m3d/memMgr.h:125]，是 M09 内存池注册表的唯一存储。

## 高危

### 3. `Free` 路径在 release 构建下可能错池或空指针

```cpp
IMemoryPool* pMemoryPool = m_PoolRefs.Get(MakeKey(poolInfo))->m_Value;  // src/hal/m3d/memMgr.cpp:158
MUSA_ASSERT(pMemoryPool && (MakeKey(pMemoryPool->GetInfo()) == MakeKey(poolInfo)));  // src/hal/m3d/memMgr.cpp:159
pMemoryPool->Free(pMemory, base, size);   // src/hal/m3d/memMgr.cpp:160
```

`Get` 对缺失键返回"最近节点"，`->m_Value` 可能是**另一个池**的指针；空树时返回空值。`MUSA_ASSERT` [src/hal/m3d/memMgr.cpp:159] 只在 debug 生效，release 裁剪后第 [src/hal/m3d/memMgr.cpp:160] 行会空指针崩溃或释放到错误的池。释放路径无错误码出口，无法优雅失败。

### 4. `m_InternalPoolRefs` 是定长 `std::vector` 下标直写

```cpp
m_InternalPoolRefs[static_cast<uint32_t>(createInfo.internalType)].emplace_back(pPool);  // src/hal/m3d/memMgr.cpp:187
m_InternalPoolRefs[pMemPool->GetInternalType()].remove(pMemPool);                       // src/hal/m3d/memMgr.cpp:206
```

`internalType` 被直接转成下标，无边界检查。若枚举值超出向量容量即越界写。需确认 `InternalMemoryPoolType` 枚举值域与向量容量的对应关系。

### 5. 锁覆盖范围包含池创建

`GetPool` [src/hal/m3d/memMgr.cpp:37]、`FindPool` [src/hal/m3d/memMgr.cpp:42] 以及 [src/hal/m3d/memMgr.cpp:60]、[src/hal/m3d/memMgr.cpp:213] 两处包装都在 `m_Lock` 下调用，而 `CreatePoolNoLock` [src/hal/m3d/memMgr.cpp:173] 内部会构造池对象并虚调用 `pPool->GetInfo()`。持锁期间执行构造与虚调用，若池实现反向回调 `MemMgr` 会自死锁。

## 中危

| 风险 | 证据/状态 | 说明 |
|---|---|---|
| `Delete(key, true)` 释放 `m_Value` 指向对象 | [src/hal/m3d/memMgr.cpp:208] | 与 [src/hal/m3d/memMgr.cpp:25]-[src/hal/m3d/memMgr.cpp:27] 的批量清理并存，两条删除路径需分别验证不重复释放 |
| 析构时批量 `delete pPool` | [src/hal/m3d/memMgr.cpp:29-32] | 未见与 [src/hal/m3d/memMgr.cpp:200] 的幂等性协调 |
| `MakeKey` 一致性依赖调用方填满 `MemoryPoolInfo` | [src/hal/m3d/memMgr.cpp:116]、[src/hal/m3d/memMgr.cpp:158] | 键由五个字段拼成，任一字段遗漏即产生不同的键，表现为"池明明存在却查不到" |

## 低危

| 风险 | 证据/状态 |
|---|---|
| `src/hal/m3d/m3d` 私有子模块内部实现不可见 | 构建与接口边界已确认，内部函数未展开 |
| `MUSA_ASSERT` 在 release 构建下可能被移除 | [src/hal/m3d/memMgr.cpp:159] |
| `FindPool`/`GetPool` 本快照无可见调用方 | 接口在 [src/hal/halMemMgr.h:17-19]，暴露面取决于 `m3d` 私有子模块与外部构建 |

## 未解决问题

1. `InternalMemoryPoolType` 的枚举值域与 `m_InternalPoolRefs` 容量未逐项核对。
2. `MemoryPool` 构造期间是否回调 `MemMgr` 未确认（死锁风险 5）。
3. `FindPool`/`GetPool` 的实际调用方未定位，暴露频率未知。
4. `src/hal/m3d/m3d` 子模块内的池实现未分析。

## 相关文档

- [`call-chains.md`](call-chains.md)
- [`interfaces.md`](interfaces.md)
- [`../M12-util/risks-and-debt.md`](../M12-util/risks-and-debt.md)
- [`../../99-roadmap/rca-splaytree-memmgr.md`](../../99-roadmap/rca-splaytree-memmgr.md)
- [`../../90-cross-module/system-wiring.md`](../../90-cross-module/system-wiring.md)

## 源码证据摘要

- `src/hal/m3d/memMgr.cpp`（22 处，例：[src/hal/m3d/memMgr.cpp:38]）
- `src/util/utilSplayTree.h`（6 处，例：[src/util/utilSplayTree.h:97-100]）
- `src/hal/m3d/memMgr.h`（2 处，例：[src/hal/m3d/memMgr.h:37]）

## 下一步阅读建议

阅读 [`design.md`](design.md)；阅读 [`risks-and-debt.md`](risks-and-debt.md)。
