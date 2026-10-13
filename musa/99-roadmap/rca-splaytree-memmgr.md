# RCA：`MemMgr` 内存池查找链（`SplayTree::Search` 分支反转 + 无防护解引用）

- 文档目的：《RCA：`MemMgr` 内存池查找链（`SplayTree::Search` 分支反转 + 无防护解引用）》的分析、证据与结论。
- 适用范围：99-roadmap《RCA：`MemMgr` 内存池查找链（`SplayTree::Search` 分支反转 + 无防护解引用）》；源码根 `source/musa`。
- 对应源码版本：5.3.0 develop（本地快照 `source/musa`，无 Git 元数据）
- 证据状态：已确认（引用与符号已脚本核对；运行期未验证）
- 最后更新：2026-10-07
- 前置阅读：[`../00-overview/analysis-state.md`](../00-overview/analysis-state.md)
- 后续阅读：[`reading-guide.md`](reading-guide.md)、[`risk-register.md`](risk-register.md)


> 按"症状 → 直接原因 → 根因 → 验证 → 修复建议"结构记录。**只分析，不改源码。** 所有行号基于当前 `5.3.0 develop` 快照。

## 症状

1. `MemMgr::FindPool` 返回未定义结果乃至崩溃：`FindPool` 无条件解引用 `SplayTree::Find` 的返回值（[src/hal/m3d/memMgr.cpp:41-44]），而 `Find` 对**缺失键**返回 `nullptr`（[src/util/utilSplayTree.h:97-100]），对**大部分已存在键**也返回 `nullptr`（见根因）。解引用 `nullptr->m_Value` 是必然崩溃。
2. `MemMgr::Free` 可能释放到错误的池或空指针：`Free` 用 `Get` 查找（[src/hal/m3d/memMgr.cpp:158]），`Get` 对缺失键返回**最近节点**而非失败，`->m_Value` 是另一个池的指针；对空树返回 `&s_Null`，`->m_Value` 为 0。release 构建下断言被裁剪，第 [src/hal/m3d/memMgr.cpp:160] 行 `pMemoryPool->Free(...)` 会空指针崩溃或释放到错误的池。

## 直接原因

- `MemMgr` 把树查找当作"必中"：`GetPool` [src/hal/m3d/memMgr.cpp:38]、`FindPool` [src/hal/m3d/memMgr.cpp:43]、`Allocate` 内部 [src/hal/m3d/memMgr.cpp:116]、`Free` [src/hal/m3d/memMgr.cpp:158] 四处直接 `->m_Value`，没有先判空、也没有用 `MakeKey` 校验命中是否为精确匹配。
- `Util::SplayTree::Get` 与 `Find` 的失败语义不同且未在接口层表达：
  - `Get` [src/util/utilSplayTree.h:90-93] 走 `Splay`，**从不返回 `nullptr`**：空树返回 `&s_Null`（`m_Value == 0`），缺失键返回"最近"节点（设计注释见 [src/util/utilSplayTree.h:87-88]）。
  - `Find` [src/util/utilSplayTree.h:97-100] 走 `Search`，缺失时把哨兵转成 `nullptr`（[src/util/utilSplayTree.h:99]、[src/util/utilSplayTree.h:70]）。

## 根因

### 根因 A：`SplayTree::Search` 两个递归分支方向互换

```cpp
if (node->m_Key < key) {
    node = Search(node->m_pLeft, key);   // 错：key 更大，应走 m_pRight
} else if (node->m_Key > key) {
    node = Search(node->m_pRight, key);  // 错：key 更小，应走 m_pLeft
}
```

[src/util/utilSplayTree.h:216-225]。`node->m_Key < key` 说明目标键比当前节点大，正确的下一跳是**右子树**（更大的键都在右子树），代码却进了**左子树**；反之亦然。因此查找永远背离目标键下降，最终在哨兵节点 `GetNull()` 停止。

后果：
- `Find` 只在"错误下降路径恰好路过键值相等节点"时才返回正确节点（几乎不可能），绝大多数情况下返回 `GetNull()` → `Find` 转成 `nullptr`。
- **`Find` 对已存在的键同样会 miss**，这是比"缺失键返回空"更严重的一层破坏。

### 根因 B：`Get` 的"最近节点"语义与调用方的"精确命中"假设不匹配

`Splay` 的设计注释明确写"返回键值相等**或最近**的节点"（[src/util/utilSplayTree.h:87-88]）。`MemMgr` 用它做精确键查找，只有部分调用点用字段逐一比对兜底（[src/hal/m3d/memMgr.cpp:117-121]），`GetPool` 与 `Free` 没有。`MakeKey` 五字段复合键（[src/hal/m3d/memMgr.cpp:108-113]）拼出任意组合，`Get` 返回的"最近"不一定就是目标键。

### 根因 C：树查找可空性未在 `Hal::IMemMgr` 接口表达

[src/hal/halMemMgr.h:17-19] 声明 `FindPool`/`GetPool` 返回裸指针，无 `nullable` 标注、无空值约定、无错误码出口。调用方没有理由认为它们可能失败。

## 验证

1. 分支互换的静态验证：
   - `node->m_Key < key` 即 `key > node->m_Key`，BST 序要求下一步找比当前节点大的键 → 右子树。代码走 `m_pLeft`，方向相反。[src/util/utilSplayTree.h:218-221]
   - `node->m_Key > key` 即 `key < node->m_Key`，应走左子树。代码走 `m_pRight`。[src/util/utilSplayTree.h:220-222]
2. 崩溃链验证：`Find` miss → `Search` 返回 `GetNull()` → `Find` 经 [src/util/utilSplayTree.h:99] 返回 `End()` = `nullptr`（[src/util/utilSplayTree.h:70]）→ [src/hal/m3d/memMgr.cpp:43] `Find(...)->m_Value` 解引用空指针。
3. `Get` 不返回 `nullptr` 的验证：`Splay` 无论命中与否都返回一个非空节点（[src/util/utilSplayTree.h:105-142]），空树时 `m_pRoot` 保持 `&s_Null`（构造 [src/util/utilSplayTree.h:63]，静态实例 [src/util/utilSplayTree.h:83-84]），`s_Null` 默认构造的 `m_Value` 为 0（[src/util/utilSplayTree.h:15-17]）。
4. 防护有效性验证：
   - [src/hal/m3d/memMgr.cpp:117] `if (!pPool || ...)`：仅空树（`Get` 返回空值）时可达，**不是死代码**；但它拦不住"最近节点 ≠ 精确键"的错池情况——后者靠 [src/hal/m3d/memMgr.cpp:117-121] 的五字段比对兜底。
   - [src/hal/m3d/memMgr.cpp:159-160]：`MUSA_ASSERT(pMemoryPool && ...)` 在 debug 生效、release 被裁剪，故**不能**作为 `Free` 的可靠防护。

## 影响面与暴露频率

| 位置 | 方法 | 暴露面 | 频率 |
|---|---|---|---|
| [src/hal/m3d/memMgr.cpp:43] | `FindPool` | 本快照无可见调用方（接口在 [src/hal/halMemMgr.h:17]，`MemMgr` 覆盖在 [src/hal/m3d/memMgr.h:23]）；`src/hal/m3d/m3d` 私有子模块未知 | 调用即崩 |
| [src/hal/m3d/memMgr.cpp:36-39] | `GetPool`（解引用在 [src/hal/m3d/memMgr.cpp:38]） | 本快照无可见调用方 | 调用于空树/错键即返回错值 |
| [src/hal/m3d/memMgr.cpp:116] | `Allocate` 自动选池路径 | 核心分配热路径，经 `GetMemMgr()->Allocate`（[src/musa/core/memory.cpp:636]、[src/musa/core/device.cpp:1263]） | 高，但被字段比对兜底 |
| [src/hal/m3d/memMgr.cpp:149-160] | `Free`（解引用在 [src/hal/m3d/memMgr.cpp:158]） | 核心释放热路径，经 `GetMemMgr()->Free`（[src/musa/core/memory.cpp:494] 等） | 高，release 下错池/空指针 |

## 修复建议（不实施）

1. 交换 `Search` 两个分支（[src/util/utilSplayTree.h:218-221]）为 `>` → 右、`<` → 左。
2. `MemMgr` 四处查找全部先判空：
   - `GetPool`/`FindPool` 返回 `nullptr` 作为"池不存在"。
   - `Allocate` 内部 [src/hal/m3d/memMgr.cpp:116] 先取节点、判空、再取 `m_Value`。
   - `Free`（[src/hal/m3d/memMgr.cpp:149-160]）改为可失败路径：池缺失时记录错误并 return，禁止裸 `pMemoryPool->Free(...)`。
3. 在 `Hal::IMemMgr` 接口文档化 `FindPool`（缺失返回空）与 `GetPool`（最近节点）的语义差异；如不需要"最近"语义，`GetPool` 改用 `Find` + 判空。
4. 回归用例：`SplayTree` 单测插入 N 个键后，逐个 `Find` 已存在键（当前全部 miss）、`Find` 缺失键（应返回空）、`Get` 缺失键（确认"最近"行为）。这些用例在本快照缺失（`unittest/` 目录不存在，见风险登记）。

## 结论

`FindPool` 的崩溃是**必然缺陷**（只要被调用）；`Free` 在 release 下的错池/空指针是**高概率缺陷**（热路径 + 断言裁剪）。根因是 `Search` 分支反转破坏 `Find` 语义，叠加 `Get` 的"最近节点"语义未被接口与调用方消化。修复优先级为：分支反转 → `MemMgr` 判空 → 接口语义文档化 → 单测。

## 相关文档

- [`../01-modules/M09-hal-m3d/risks-and-debt.md`](../01-modules/M09-hal-m3d/risks-and-debt.md)
- [`../01-modules/M12-util/risks-and-debt.md`](../01-modules/M12-util/risks-and-debt.md)
- [`../99-roadmap/risk-register.md`](risk-register.md)
- [`../90-cross-module/system-wiring.md`](../90-cross-module/system-wiring.md)

## 源码证据摘要

- `src/hal/m3d/memMgr.cpp`（21 处，例：[src/hal/m3d/memMgr.cpp:41-44]）
- `src/util/utilSplayTree.h`（17 处，例：[src/util/utilSplayTree.h:97-100]）
- `src/hal/halMemMgr.h`（2 处，例：[src/hal/halMemMgr.h:17-19]）

## 未解决问题

1. 本文所有结论均为静态分析，未在授权硬件上执行构建、测试或运行验证。
2. 本文未单列的风险条目见 [`../99-roadmap/risk-register.md`](../99-roadmap/risk-register.md)。

## 下一步阅读建议

阅读 [`reading-guide.md`](reading-guide.md)；阅读 [`risk-register.md`](risk-register.md)。
