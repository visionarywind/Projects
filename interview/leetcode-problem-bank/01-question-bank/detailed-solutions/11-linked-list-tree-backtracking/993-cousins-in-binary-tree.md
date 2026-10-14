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
