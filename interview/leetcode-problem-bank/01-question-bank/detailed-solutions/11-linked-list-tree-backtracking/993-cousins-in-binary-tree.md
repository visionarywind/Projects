# 993. 二叉树的堂兄弟节点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/cousins-in-binary-tree/
- 题目 slug：`cousins-in-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.13 二叉树 BFS
- 难度分：1288
- 外部题解来源：https://leetcode.cn/problems/cousins-in-binary-tree/solutions/2635560/liang-chong-dfs-xie-fa-pythonjavacgojsru-wzb2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[DFS+记录深度和父节点（Python/Java/C++/Go）](https://leetcode.cn/problems/cousins-in-binary-tree/solutions/2635560/liang-chong-dfs-xie-fa-pythonjavacgojsru-wzb2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-dfs-xie-fa-pythonjavacgojsru-wzb2`
- topic id：`2635560`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

DFS 传入三个参数：

- 当前节点 $\textit{node}$。
- $\textit{node}$ 的父节点 $\textit{fa}$。
- $\textit{node}$ 的深度 $\textit{d}$。为了方便判断，根节点的深度为 $1$。

如果 $\textit{node}$ 的节点值等于 $x$ 或者 $y$：

- 如果之前没有找到 $x$ 或 $y$，那么记录 $\textit{node}$ 的深度到变量 $\textit{depth}$ 中，记录 $\textit{node}$ 的父节点到变量 $\textit{father}$ 中。
- 如果之前找到 $x$ 或 $y$，那么现在 $x$ 和 $y$ 都找到了。如果 $\textit{depth}=d$ 且 $\textit{father}\ne \textit{fa}$，说明 $x$ 和 $y$ 对应的节点是堂兄弟节点，否则不是。

注意题目保证树中节点值互不相同。

晕递归的同学可以看 [深刻理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

## 写法一

```py [sol-Python3]
class Solution:
    def isCousins(self, root: Optional[TreeNode], x: int, y: int) -> bool:
        depth = father = None
        def dfs(node: Optional[TreeNode], fa: Optional[TreeNode], d: int) -> bool:
            if node is None:
                return False
            if node.val == x or node.val == y:  # 找到 x 或 y
                nonlocal depth, father
                if depth:  # 之前已找到 x y 其中一个
                    return depth == d and father != fa
                depth, father = d, fa  # 之前没找到，记录信息
            return dfs(node.left, node, d + 1) or dfs(node.right, node, d + 1)
        return dfs(root, None, 1)
```

```java [sol-Java]
class Solution {
    private int depth;
    private TreeNode father;

    public boolean isCousins(TreeNode root, int x, int y) {
        return dfs(root, null, 1, x, y);
    }

    private boolean dfs(TreeNode node, TreeNode fa, int d, int x, int y) {
        if (node == null) {
            return false;
        }
        if (node.val == x || node.val == y) { // 找到 x 或 y
            if (depth > 0) { // 之前已找到 x y 其中一个
                return depth == d && father != fa;
            }
            depth = d; // 之前没找到，记录信息
            father = fa;
        }
        return dfs(node.left, node, d + 1, x, y) || dfs(node.right, node, d + 1, x, y);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isCousins(TreeNode *root, int x, int y) {
        int depth = 0;
        TreeNode *father = nullptr;
        function<bool(TreeNode*, TreeNode*, int)> dfs = [&](TreeNode *node, TreeNode *fa, int d) -> bool {
            if (node == nullptr) {
                return false;
            }
            if (node->val == x || node->val == y) { // 找到 x 或 y
                if (depth) { // 之前已找到 x y 其中一个
                    return depth == d && father != fa;
                }
                depth = d; // 之前没找到，记录信息
                father = fa;
            }
            return dfs(node->left, node, d + 1) || dfs(node->right, node, d + 1);
        };
        return dfs(root, nullptr, 1);
    }
};
```

```go [sol-Go]
func isCousins(root *TreeNode, x, y int) bool {
    depth := 0
    var father *TreeNode
    var dfs func(*TreeNode, *TreeNode, int) bool
    dfs = func(node, fa *TreeNode, d int) bool {
        if node == nil {
            return false
        }
        if node.Val == x || node.Val == y { // 找到 x 或 y
            if depth > 0 { // 之前已找到 x y 其中一个
                return depth == d && father != fa
            }
            depth, father = d, fa // 之前没找到，记录信息
        }
        return dfs(node.Left, node, d+1) || dfs(node.Right, node, d+1)
    }
    return dfs(root, nil, 1)
}
```

## 写法二（优化）

上面的代码在 $x$ 和 $y$ 都找到，但不是堂兄弟节点时，仍然会继续递归。

改为在此时记录答案，并将 DFS 返回值的含义改为「$x$ 和 $y$ 都已找到」。

```py [sol-Python3]
class Solution:
    def isCousins(self, root: Optional[TreeNode], x: int, y: int) -> bool:
        ans = False
        depth = father = None
        def dfs(node: Optional[TreeNode], fa: Optional[TreeNode], d: int) -> bool:
            if node is None:
                return False
            if node.val == x or node.val == y:  # 找到 x 或 y
                nonlocal ans, depth, father
                if depth:  # 之前找到 x y 其中一个
                    ans = depth == d and father != fa
                    return True  # 表示 x 和 y 都找到
                depth, father = d, fa  # 之前没找到，记录信息
            return dfs(node.left, node, d + 1) or dfs(node.right, node, d + 1)
        dfs(root, None, 1)
        return ans
```

```java [sol-Java]
class Solution {
    private boolean ans;
    private int depth;
    private TreeNode father;

    public boolean isCousins(TreeNode root, int x, int y) {
        dfs(root, null, 1, x, y);
        return ans;
    }

    private boolean dfs(TreeNode node, TreeNode fa, int d, int x, int y) {
        if (node == null) {
            return false;
        }
        if (node.val == x || node.val == y) { // 找到 x 或 y
            if (depth > 0) { // 之前已找到 x y 其中一个
                ans = depth == d && father != fa;
                return true; // 表示 x 和 y 都找到
            }
            depth = d; // 之前没找到，记录信息
            father = fa;
        }
        return dfs(node.left, node, d + 1, x, y) || dfs(node.right, node, d + 1, x, y);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isCousins(TreeNode *root, int x, int y) {
        bool ans = false;
        int depth = 0;
        TreeNode *father = nullptr;
        function<bool(TreeNode*, TreeNode*, int)> dfs = [&](TreeNode *node, TreeNode *fa, int d) -> bool {
            if (node == nullptr) {
                return false;
            }
            if (node->val == x || node->val == y) { // 找到 x 或 y
                if (depth) { // 之前已找到 x y 其中一个
                    ans = depth == d && father != fa;
                    return true; // 表示 x 和 y 都找到
                }
                depth = d; // 之前没找到，记录信息
                father = fa;
            }
            return dfs(node->left, node, d + 1) || dfs(node->right, node, d + 1);
        };
        dfs(root, nullptr, 1);
        return ans;
    }
};
```

```go [sol-Go]
func isCousins(root *TreeNode, x, y int) (ans bool) {
    depth := 0
    var father *TreeNode
    var dfs func(*TreeNode, *TreeNode, int) bool
    dfs = func(node, fa *TreeNode, d int) bool {
        if node == nil {
            return false
        }
        if node.Val == x || node.Val == y { // 找到 x 或 y
            if depth > 0 { // 之前已找到 x y 其中一个
                ans = depth == d && father != fa
                return true // 表示 x 和 y 都找到
            }
            depth, father = d, fa // 之前没找到，记录信息
        }
        return dfs(node.Left, node, d+1) || dfs(node.Right, node, d+1)
    }
    dfs(root, nil, 1)
    return
}
```

## 写法三（优化）

如果已经找到了 $x$ 和 $y$ 的其中一个，此时 $\textit{depth}>0$，我们无需递归深度超过 $\textit{depth}$ 的点。

```py [sol-Python3]
class Solution:
    def isCousins(self, root: Optional[TreeNode], x: int, y: int) -> bool:
        ans = False
        depth = father = None
        def dfs(node: Optional[TreeNode], fa: Optional[TreeNode], d: int) -> bool:
            nonlocal ans, depth, father
            if node is None or depth and d > depth:
                return False
            if node.val == x or node.val == y:  # 找到 x 或 y
                if depth:  # 之前找到 x y 其中一个
                    ans = depth == d and father != fa
                    return True  # 表示 x 和 y 都找到
                depth, father = d, fa  # 之前没找到，记录信息
            return dfs(node.left, node, d + 1) or dfs(node.right, node, d + 1)
        dfs(root, None, 1)
        return ans
```

```java [sol-Java]
class Solution {
    private boolean ans;
    private int depth;
    private TreeNode father;

    public boolean isCousins(TreeNode root, int x, int y) {
        dfs(root, null, 1, x, y);
        return ans;
    }

    private boolean dfs(TreeNode node, TreeNode fa, int d, int x, int y) {
        if (node == null || depth > 0 && d > depth) {
            return false;
        }
        if (node.val == x || node.val == y) { // 找到 x 或 y
            if (depth > 0) { // 之前已找到 x y 其中一个
                ans = depth == d && father != fa;
                return true; // 表示 x 和 y 都找到
            }
            depth = d; // 之前没找到，记录信息
            father = fa;
        }
        return dfs(node.left, node, d + 1, x, y) || dfs(node.right, node, d + 1, x, y);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isCousins(TreeNode *root, int x, int y) {
        bool ans = false;
        int depth = 0;
        TreeNode *father = nullptr;
        function<bool(TreeNode*, TreeNode*, int)> dfs = [&](TreeNode *node, TreeNode *fa, int d) -> bool {
            if (node == nullptr || depth && d > depth) {
                return false;
            }
            if (node->val == x || node->val == y) { // 找到 x 或 y
                if (depth) { // 之前已找到 x y 其中一个
                    ans = depth == d && father != fa;
                    return true; // 表示 x 和 y 都找到
                }
                depth = d; // 之前没找到，记录信息
                father = fa;
            }
            return dfs(node->left, node, d + 1) || dfs(node->right, node, d + 1);
        };
        dfs(root, nullptr, 1);
        return ans;
    }
};
```

```go [sol-Go]
func isCousins(root *TreeNode, x, y int) (ans bool) {
    depth := 0
    var father *TreeNode
    var dfs func(*TreeNode, *TreeNode, int) bool
    dfs = func(node, fa *TreeNode, d int) bool {
        if node == nil || depth > 0 && d > depth {
            return false
        }
        if node.Val == x || node.Val == y { // 找到 x 或 y
            if depth > 0 { // 之前已找到 x y 其中一个
                ans = depth == d && father != fa
                return true // 表示 x 和 y 都找到
            }
            depth, father = d, fa // 之前没找到，记录信息
        }
        return dfs(node.Left, node, d+1) || dfs(node.Right, node, d+1)
    }
    dfs(root, nil, 1)
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树退化成一条链，递归需要 $O(n)$ 的栈空间。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.13 二叉树 BFS`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.13 二叉树 BFS`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
