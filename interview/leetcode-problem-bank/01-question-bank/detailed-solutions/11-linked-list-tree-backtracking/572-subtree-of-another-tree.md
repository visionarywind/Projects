# 572. 另一棵树的子树

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/subtree-of-another-tree/
- 题目 slug：`subtree-of-another-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.3 自底向上 DFS（后序遍历）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/subtree-of-another-tree/solutions/2868217/cong-onm-dao-onpythonjavacgo-by-endlessc-uukp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从 O(nm) 到 O(n+m)（Python/Java/C++/Go）](https://leetcode.cn/problems/subtree-of-another-tree/solutions/2868217/cong-onm-dao-onpythonjavacgo-by-endlessc-uukp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-onm-dao-onpythonjavacgo-by-endlessc-uukp`
- topic id：`2868217`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

## 方法一：暴力匹配

对于二叉树 $\textit{root}$ 中的每个节点 $\textit{node}$，都与 $\textit{subRoot}$ 计算一次 [100. 相同的树](https://leetcode.cn/problems/same-tree/)，如果有匹配上的，则答案为 $\texttt{true}$。

### 算法

1. 把 $\texttt{isSubtree}$ 当作一个递归函数，直接用这个函数遍历二叉树 $\textit{root}$ 的节点。
2. 如果当前节点是空节点，无法与 $\textit{subRoot}$ 匹配，返回 $\texttt{false}$。
3. 如果当前节点与 $\textit{subRoot}$ 是 [100. 相同的树](https://leetcode.cn/problems/same-tree/)，返回 $\texttt{true}$。
4. 否则，递归左右子树，看看是否能找到匹配，如果找到则返回 $\texttt{true}$。

晕递归的同学可以看 [深刻理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

```py [sol-Python3]
class Solution:
    # 100. 相同的树
    def isSameTree(self, p: Optional[TreeNode], q: Optional[TreeNode]) -> bool:
        if p is None or q is None:
            return p is q  # 必须都是 None
        return p.val == q.val and \
            self.isSameTree(p.left, q.left) and \
            self.isSameTree(p.right, q.right)

    def isSubtree(self, root: Optional[TreeNode], subRoot: Optional[TreeNode]) -> bool:
        if root is None:
            return False
        return self.isSameTree(root, subRoot) or \
            self.isSubtree(root.left, subRoot) or \
            self.isSubtree(root.right, subRoot)
```

```java [sol-Java]
class Solution {
    public boolean isSubtree(TreeNode root, TreeNode subRoot) {
        if (root == null) {
            return false;
        }
        return isSameTree(root, subRoot) ||
               isSubtree(root.left, subRoot) ||
               isSubtree(root.right, subRoot);
    }

    // 100. 相同的树
    private boolean isSameTree(TreeNode p, TreeNode q) {
        if (p == null || q == null) {
            return p == q; // 必须都是 null
        }
        return p.val == q.val &&
               isSameTree(p.left, q.left) &&
               isSameTree(p.right, q.right);
    }
}
```

```cpp [sol-C++]
class Solution {
    // 100. 相同的树
    bool isSameTree(TreeNode* p, TreeNode* q) {
        if (p == nullptr || q == nullptr) {
            return p == q; // 必须都是 nullptr
        }
        return p->val == q->val &&
               isSameTree(p->left, q->left) &&
               isSameTree(p->right, q->right);
    }

public:
    bool isSubtree(TreeNode* root, TreeNode* subRoot) {
        if (root == nullptr) {
            return false;
        }
        return isSameTree(root, subRoot) ||
               isSubtree(root->left, subRoot) ||
               isSubtree(root->right, subRoot);
    }
};
```

```go [sol-Go]
// 100. 相同的树
func isSameTree(p, q *TreeNode) bool {
    if p == nil || q == nil {
        return p == q // 必须都是 nil
    }
    return p.Val == q.Val &&
        isSameTree(p.Left, q.Left) &&
        isSameTree(p.Right, q.Right)
}

func isSubtree(root, subRoot *TreeNode) bool {
    if root == nil {
        return false
    }
    return isSameTree(root, subRoot) ||
        isSubtree(root.Left, subRoot) ||
        isSubtree(root.Right, subRoot)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot \min(n,m))$，其中 $n$ 是二叉树 $\textit{root}$ 的节点个数，$m$ 是二叉树 $\textit{subRoot}$ 的节点个数。理由见方法二。
- 空间复杂度：$\mathcal{O}(n)$。无论 $m$ 大于 $n$ 还是小于 $n$，递归深度都不会超过 $\mathcal{O}(n)$，所需的栈空间为 $\mathcal{O}(n)$。

## 方法二：只在高度相同时匹配

对于方法一，什么情况下会跑到 $\mathcal{O}(n\cdot \min(n,m))$？

如果两棵树都是链（比如节点只有左儿子没有右儿子），且所有节点值均为 $1$，那么每次匹配的时间是 $\mathcal{O}(\min(n,m))$，再算上总共匹配 $\mathcal{O}(n)$ 次，总的时间复杂度是 $\mathcal{O}(n\cdot\min(n,m))$。

⚠**注意**：如果 $\textit{root}$ 是一条长为 $4$ 的链，$\textit{subRoot}$ 是一条长为 $2$ 的链，即使 $\textit{subRoot}$ 与 $\textit{root}$ 的**前两个**节点匹配，也不符合题目要求，因为 $\textit{root}$ 的第二个节点还有儿子，而 $\textit{subRoot}$ 没有。（另一个例子是题目的示例 2。）

定义节点 $\textit{node}$ 的**高度**为 $\textit{node}$ 到其最深叶子的路径上的节点个数。特别地，叶子的高度为 $1$。

设 $\textit{subRoot}$ 的高度为 $h_s$。

设 $\textit{node}$ 为 $\textit{root}$ 中的节点。我们可以在匹配之前，先判断 $\textit{node}$ 的高度是否等于 $h_s$，只有在相等时才做匹配。

这样做的时间复杂度是多少呢？

首先，对于 $\textit{root}$ 中的两个**高度相同**的节点 $p$ 和 $q$（$p\ne q$），不可能出现 $p$ 是 $q$ 的祖先，或者 $q$ 是 $p$ 的祖先。因为如果 $p$ 是 $q$ 的祖先，那么 $p$ 的高度必然大于 $q$ 的高度，反之亦然。

其次，对于 $\textit{root}$ 中的两个**高度相同**的节点 $p$ 和 $q$（$p\ne q$），子树 $p$ 和子树 $q$ 没有任何交集。假如有交集，说明从 $p$ 出发可以到达某个点 $\textit{node}$，从 $q$ 出发也可以到达 $\textit{node}$，但这只有在 $p$ 是 $q$ 的祖先，或者 $q$ 是 $p$ 的祖先时才成立，与两个节点高度相同矛盾。

所以，$\textit{root}$ 中的所有与 $h_s$ 高度相同的节点，对应的子树两两不相交，这些子树的**节点个数之和**不超过 $n$，所以总的匹配次数也不会超过 $n$，下面代码中的 $\texttt{dfs}$ 的时间复杂度为 $\mathcal{O}(n)$。

如何计算节点高度请看[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

```py [sol-Python3]
class Solution:
    # 104. 二叉树的最大深度（高度）
    def getHeight(self, root: Optional[TreeNode]) -> int:
        if root is None:
            return 0
        left_h = self.getHeight(root.left)
        right_h = self.getHeight(root.right)
        return max(left_h, right_h) + 1

    # 100. 相同的树
    def isSameTree(self, p: Optional[TreeNode], q: Optional[TreeNode]) -> bool:
        if p is None or q is None:
            return p is q  # 必须都是 None
        return p.val == q.val and \
            self.isSameTree(p.left, q.left) and \
            self.isSameTree(p.right, q.right)

    def isSubtree(self, root: Optional[TreeNode], subRoot: Optional[TreeNode]) -> bool:
        sub_height = self.getHeight(subRoot)

        # 返回 node 的高度，以及是否找到了 subRoot
        def dfs(node: Optional[TreeNode]) -> Tuple[int, bool]:
            if node is None:
                return 0, False
            left_h, left_found = dfs(node.left)
            right_h, right_found = dfs(node.right)
            if left_found or right_found:
                return 0, True
            node_h = max(left_h, right_h) + 1
            return node_h, node_h == sub_height and self.isSameTree(node, subRoot)

        return dfs(root)[1]
```

```java [sol-Java]
class Solution {
    public boolean isSubtree(TreeNode root, TreeNode subRoot) {
        int h = getHeight(subRoot);
        return dfs(root, subRoot, h).getValue();
    }

    // 104. 二叉树的最大深度（高度）
    private int getHeight(TreeNode root) {
        if (root == null) {
            return 0;
        }
        int leftH = getHeight(root.left);
        int rightH = getHeight(root.right);
        return Math.max(leftH, rightH) + 1;
    }

    // 100. 相同的树
    private boolean isSameTree(TreeNode p, TreeNode q) {
        if (p == null || q == null) {
            return p == q; // 必须都是 null
        }
        return p.val == q.val &&
               isSameTree(p.left, q.left) &&
               isSameTree(p.right, q.right);
    }

    // 返回 node 的高度，以及是否找到了 subRoot
    private Pair<Integer, Boolean> dfs(TreeNode node, TreeNode subRoot, int subHeight) {
        if (node == null) {
            return new Pair<>(0, false);
        }
        Pair<Integer, Boolean> left = dfs(node.left, subRoot, subHeight);
        Pair<Integer, Boolean> right = dfs(node.right, subRoot, subHeight);
        if (left.getValue() || right.getValue()) {
            return new Pair<>(0, true);
        }
        int nodeH = Math.max(left.getKey(), right.getKey()) + 1;
        return new Pair<>(nodeH, nodeH == subHeight && isSameTree(node, subRoot));
    }
}
```

```cpp [sol-C++]
class Solution {
    // 104. 二叉树的最大深度（高度）
    int getHeight(TreeNode* root) {
        if (root == nullptr) {
            return 0;
        }
        int left_h = getHeight(root->left);
        int right_h = getHeight(root->right);
        return max(left_h, right_h) + 1;
    }

    // 100. 相同的树
    bool isSameTree(TreeNode* p, TreeNode* q) {
        if (p == nullptr || q == nullptr) {
            return p == q; // 必须都是 nullptr
        }
        return p->val == q->val &&
               isSameTree(p->left, q->left) &&
               isSameTree(p->right, q->right);
    }

public:
    bool isSubtree(TreeNode* root, TreeNode* subRoot) {
        int sub_height = getHeight(subRoot);

        // 返回 node 的高度，以及是否找到了 subRoot
        auto dfs = [&](this auto&& dfs, TreeNode* node) -> pair<int, bool> {
            if (node == nullptr) {
                return {0, false};
            }
            auto [left_h, left_found] = dfs(node->left);
            auto [right_h, right_found] = dfs(node->right);
            if (left_found || right_found) {
                return {0, true};
            }
            int node_h = max(left_h, right_h) + 1;
            return {node_h, node_h == sub_height && isSameTree(node, subRoot)};
        };

        return dfs(root).second;
    }
};
```

```go [sol-Go]
// 104. 二叉树的最大深度（高度）
func getHeight(root *TreeNode) int {
    if root == nil {
        return 0
    }
    leftH := getHeight(root.Left)
    rightH := getHeight(root.Right)
    return max(leftH, rightH) + 1
}

// 100. 相同的树
func isSameTree(p, q *TreeNode) bool {
    if p == nil || q == nil {
        return p == q // 必须都是 nil
    }
    return p.Val == q.Val &&
        isSameTree(p.Left, q.Left) &&
        isSameTree(p.Right, q.Right)
}

func isSubtree(root, subRoot *TreeNode) bool {
    subHeight := getHeight(subRoot)

    // 返回 node 的高度，以及是否找到了 subRoot
    var dfs func(*TreeNode) (int, bool)
    dfs = func(node *TreeNode) (int, bool) {
        if node == nil {
            return 0, false
        }
        leftH, leftFound := dfs(node.Left)
        rightH, rightFound := dfs(node.Right)
        if leftFound || rightFound {
            return 0, true
        }
        nodeH := max(leftH, rightH) + 1
        return nodeH, nodeH == subHeight && isSameTree(node, subRoot)
    }

    _, found := dfs(root)
    return found
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 是二叉树 $\textit{root}$ 的节点个数，$m$ 是二叉树 $\textit{subRoot}$ 的节点个数。注意当 $\textit{subRoot}$ 的节点个数比 $n$ 还要多时，$\texttt{dfs}$ 并不会遍历 $\textit{subRoot}$ 中的所有节点。其中 $\mathcal{O}(m)$ 是计算 $\textit{subRoot}$ 树高的时间，如果限制 $\texttt{getHeight}$ 递归访问的节点个数至多为 $n$，则可以做到 $\mathcal{O}(n)$ 的时间复杂度。
- 空间复杂度：$\mathcal{O}(n+m)$。注：如果限制 $\texttt{getHeight}$ 递归访问的节点个数至多为 $n$，则可以做到 $\mathcal{O}(n)$ 的（栈）空间复杂度。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.3 自底向上 DFS（后序遍历）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `二、二叉树 / §2.3 自底向上 DFS（后序遍历）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
