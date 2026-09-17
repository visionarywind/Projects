# 1123. 最深叶节点的最近公共祖先

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/lowest-common-ancestor-of-deepest-leaves/
- 题目 slug：`lowest-common-ancestor-of-deepest-leaves`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.8 最近公共祖先
- 难度分：1607
- 外部题解来源：https://leetcode.cn/problems/lowest-common-ancestor-of-deepest-leaves/solutions/2428724/liang-chong-di-gui-si-lu-pythonjavacgojs-xxnk/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一次 DFS，两种写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/lowest-common-ancestor-of-deepest-leaves/solutions/2428724/liang-chong-di-gui-si-lu-pythonjavacgojs-xxnk/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-di-gui-si-lu-pythonjavacgojs-xxnk`
- topic id：`2428724`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

## 前言

推荐先把 [236. 二叉树的最近公共祖先](https://leetcode.cn/problems/lowest-common-ancestor-of-a-binary-tree/) 做了，对理解本题做法有帮助。

本题最深的叶子可能只有一个，此时这个叶子就是答案。如果最深的叶子不止一个，那么答案为**所有**最深叶子的最近公共祖先。

## 方法一：递归递归，有递有归

![](https://pic.leetcode.cn/1693882910-FIoXPj-sketch1.png)

回顾 236 题的做法：

- 如果要找的节点只在左子树中，那么最近公共祖先也只在左子树中。
- 如果要找的节点只在右子树中，那么最近公共祖先也只在右子树中。
- 如果要找的节点左右子树都有，那么最近公共祖先就是当前节点。

对于本题，要找的节点是最深的叶子。

如果左子树的最大深度比右子树的大，那么（子树中的）最深叶子就只在左子树中，所以（子树中的）最深叶子的最近公共祖先也只在左子树中。

如果左右子树的最大深度一样呢？当前节点一定是最近公共祖先吗？

不一定。比如上图节点 $1$ 的左右子树最深叶子 $0,8$ 的深度都是 $2$，但该深度并不是**全局最大深度**，所以节点 $1$ 并不是答案。

根据以上讨论，正确做法如下：

1. 从根节点开始递归，同时维护全局最大深度 $\textit{maxDepth}$。
2. 在「递」的时候往下传 $\textit{depth}$，用来表示当前节点的深度。
3. 在「归」的时候往上传当前子树最深的空节点的深度。这里为了方便，用空节点代替叶子，因为**最深的空节点上面一定是最深的叶子**。
4. 设左子树最深空节点的深度为 $\textit{leftMaxDepth}$，右子树最深空节点的深度为 $\textit{rightMaxDepth}$。如果最深的空节点左右子树都有，即 $\textit{leftMaxDepth}=\textit{rightMaxDepth}=\textit{maxDepth}$，那么更新答案为当前节点。注意这并不代表我们找到了答案，如果后面发现了更深的空节点，答案还会更新。另外注意，这个判断方式在只有一个最深叶子的情况下，也是正确的。

```py [sol-Python3]
class Solution:
    def lcaDeepestLeaves(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        ans = None
        max_depth = -1  # 全局最大深度
        def dfs(node: Optional[TreeNode], depth: int) -> int:
            nonlocal ans, max_depth
            if node is None:
                max_depth = max(max_depth, depth)  # 维护全局最大深度
                return depth
            left_max_depth = dfs(node.left, depth + 1)  # 左子树最深空节点的深度
            right_max_depth = dfs(node.right, depth + 1)  # 右子树最深空节点的深度
            if left_max_depth == right_max_depth == max_depth:  # 最深的空节点左右子树都有
                ans = node  # 如果后面发现了更大的 max_depth，答案还会更新
            return max(left_max_depth, right_max_depth)  # 当前子树最深空节点的深度
        dfs(root, 0)
        return ans
```

```java [sol-Java]
class Solution {
    private TreeNode ans;
    private int maxDepth = -1; // 全局最大深度

    public TreeNode lcaDeepestLeaves(TreeNode root) {
        dfs(root, 0);
        return ans;
    }

    private int dfs(TreeNode node, int depth) {
        if (node == null) {
            maxDepth = Math.max(maxDepth, depth); // 维护全局最大深度
            return depth;
        }
        int leftMaxDepth = dfs(node.left, depth + 1); // 左子树最深空节点的深度
        int rightMaxDepth = dfs(node.right, depth + 1); // 右子树最深空节点的深度
        if (leftMaxDepth == rightMaxDepth && leftMaxDepth == maxDepth) { // 最深的空节点左右子树都有
            ans = node; // 如果后面发现了更大的 maxDepth，答案还会更新
        }
        return Math.max(leftMaxDepth, rightMaxDepth); // 当前子树最深空节点的深度
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    TreeNode* lcaDeepestLeaves(TreeNode* root) {
        TreeNode* ans = nullptr;
        int max_depth = -1; // 全局最大深度
        auto dfs = [&](this auto&& dfs, TreeNode* node, int depth) {
            if (node == nullptr) {
                max_depth = max(max_depth, depth); // 维护全局最大深度
                return depth;
            }
            int left_max_depth = dfs(node->left, depth + 1); // 左子树最深空节点的深度
            int right_max_depth = dfs(node->right, depth + 1); // 右子树最深空节点的深度
            if (left_max_depth == right_max_depth && left_max_depth == max_depth) { // 最深的空节点左右子树都有
                ans = node; // 如果后面发现了更大的 max_depth，答案还会更新
            }
            return max(left_max_depth, right_max_depth); // 当前子树最深空节点的深度
        };
        dfs(root, 0);
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

struct TreeNode* lcaDeepestLeaves(struct TreeNode* root) {
    struct TreeNode* ans = NULL;
    int max_depth = -1; // 全局最大深度

    int dfs(struct TreeNode* node, int depth) {
        if (node == NULL) {
            max_depth = MAX(max_depth, depth); // 维护全局最大深度
            return depth;
        }
        int left_max_depth = dfs(node->left, depth + 1); // 左子树最深空节点的深度
        int right_max_depth = dfs(node->right, depth + 1); // 右子树最深空节点的深度
        if (left_max_depth == right_max_depth && left_max_depth == max_depth) { // 最深的空节点左右子树都有
            ans = node; // 如果后面发现了更大的 max_depth，答案还会更新
        }
        return MAX(left_max_depth, right_max_depth); // 当前子树最深空节点的深度
    }

    dfs(root, 0);
    return ans;
}
```

```go [sol-Go]
func lcaDeepestLeaves(root *TreeNode) (ans *TreeNode) {
    maxDepth := -1 // 全局最大深度
    var dfs func(*TreeNode, int) int
    dfs = func(node *TreeNode, depth int) int {
        if node == nil {
            maxDepth = max(maxDepth, depth) // 维护全局最大深度
            return depth
        }
        leftMaxDepth := dfs(node.Left, depth+1) // 左子树最深空节点的深度
        rightMaxDepth := dfs(node.Right, depth+1) // 右子树最深空节点的深度
        if leftMaxDepth == rightMaxDepth && leftMaxDepth == maxDepth { // 最深的空节点左右子树都有
            ans = node // 如果后面发现了更大的 maxDepth，答案还会更新
        }
        return max(leftMaxDepth, rightMaxDepth) // 当前子树最深空节点的深度
    }
    dfs(root, 0)
    return
}
```

```js [sol-JavaScript]
var lcaDeepestLeaves = function(root) {
    let ans = null;
    let maxDepth = -1; // 全局最大深度
    function dfs(node, depth) {
        if (node === null) {
            maxDepth = Math.max(maxDepth, depth); // 维护全局最大深度
            return depth;
        }
        const leftMaxDepth = dfs(node.left, depth + 1); // 左子树最深空节点的深度
        const rightMaxDepth = dfs(node.right, depth + 1); // 右子树最深空节点的深度
        if (leftMaxDepth === rightMaxDepth && leftMaxDepth === maxDepth) { // 最深的空节点左右子树都有
            ans = node; // 如果后面发现了更大的 maxDepth，答案还会更新
        }
        return Math.max(leftMaxDepth, rightMaxDepth); // 当前子树最深空节点的深度
    }
    dfs(root, 0);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn lca_deepest_leaves(root: Option<Rc<RefCell<TreeNode>>>) -> Option<Rc<RefCell<TreeNode>>> {
        let mut ans = None;
        let mut max_depth = -1; // 全局最大深度

        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, depth: i32, max_depth: &mut i32, ans: &mut Option<Rc<RefCell<TreeNode>>>) -> i32 {
            if let Some(n) = node {
                let n = n.borrow();
                let left_max_depth = dfs(&n.left, depth + 1, max_depth, ans); // 左子树最深空节点的深度
                let right_max_depth = dfs(&n.right, depth + 1, max_depth, ans); // 右子树最深空节点的深度
                if left_max_depth == right_max_depth && left_max_depth == *max_depth { // 最深的空节点左右子树都有
                    *ans = node.clone(); // node 可能是答案
                }
                left_max_depth.max(right_max_depth) // 当前子树最深空节点的深度
            } else {
                *max_depth = (*max_depth).max(depth); // 维护全局最大深度
                depth
            }
        }

        dfs(&root, 0, &mut max_depth, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。每个节点都会恰好访问一次。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树是一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

## 方法二：自底向上

能否不用外部变量 $\textit{ans}$ 和 $\textit{maxDepth}$ 呢？

把每棵子树都看成是一个「子问题」，即对于每棵子树，我们需要知道：

- 这棵子树最深叶子的深度。这里是指叶子在这棵子树内的深度，而不是在整棵二叉树的视角下的深度。相当于这棵子树的**高度**。
- 这棵子树的最深叶子的最近公共祖先 $\textit{lca}$。

设子树的根节点为 $\textit{node}$，$\textit{node}$ 的左子树的高度为 $\textit{leftHeight}$，$\textit{node}$ 的右子树的高度为 $\textit{rightHeight}$。分类讨论：

- 如果 $\textit{leftHeight} > \textit{rightHeight}$，那么 $\textit{node}$ 子树的高度为 $\textit{leftHeight} + 1$，$\textit{lca}$ 是左子树的 $\textit{lca}$。
- 如果 $\textit{leftHeight} < \textit{rightHeight}$，那么 $\textit{node}$ 子树的高度为 $\textit{rightHeight} + 1$，$\textit{lca}$ 是右子树的 $\textit{lca}$。
- 如果 $\textit{leftHeight} = \textit{rightHeight}$，那么 $\textit{node}$ 子树的高度为 $\textit{leftHeight} + 1$，$\textit{lca}$ 就是 $\textit{node}$。

```py [sol-Python3]
class Solution:
    def lcaDeepestLeaves(self, root: Optional[TreeNode]) -> Optional[TreeNode]:
        def dfs(node: Optional[TreeNode]) -> Tuple[int, Optional[TreeNode]]:
            if node is None:
                return 0, None
            left_height, left_lca = dfs(node.left)
            right_height, right_lca = dfs(node.right)
            if left_height > right_height:  # 左子树更高
                return left_height + 1, left_lca
            if left_height < right_height:  # 右子树更高
                return right_height + 1, right_lca
            return left_height + 1, node  # 一样高
        return dfs(root)[1]
```

```java [sol-Java]
class Solution {
    public TreeNode lcaDeepestLeaves(TreeNode root) {
        return dfs(root).getValue();
    }

    private Pair<Integer, TreeNode> dfs(TreeNode node) {
        if (node == null) {
            return new Pair<>(0, null);
        }
        Pair<Integer, TreeNode> left = dfs(node.left);
        Pair<Integer, TreeNode> right = dfs(node.right);
        if (left.getKey() > right.getKey()) { // 左子树更高
            return new Pair<>(left.getKey() + 1, left.getValue());
        }
        if (left.getKey() < right.getKey()) { // 右子树更高
            return new Pair<>(right.getKey() + 1, right.getValue());
        }
        return new Pair<>(left.getKey() + 1, node); // 一样高
    }
}
```

```cpp [sol-C++]
class Solution {
    pair<int, TreeNode*> dfs(TreeNode* node) {
        if (node == nullptr) {
            return {0, nullptr};
        }
        auto [left_height, left_lca] = dfs(node->left);
        auto [right_height, right_lca] = dfs(node->right);
        if (left_height > right_height) { // 左子树更高
            return {left_height + 1, left_lca};
        }
        if (left_height < right_height) { // 右子树更高
            return {right_height + 1, right_lca};
        }
        return {left_height + 1, node}; // 一样高
    }

public:
    TreeNode* lcaDeepestLeaves(TreeNode* root) {
        return dfs(root).second;
    }
};
```

```c [sol-C]
typedef struct {
    int height;
    struct TreeNode* lca;
} Pair;

Pair dfs(struct TreeNode* node) {
    if (node == NULL) {
        return (Pair) {0, NULL};
    }
    Pair left = dfs(node->left);
    Pair right = dfs(node->right);
    if (left.height > right.height) { // 左子树更高
        return (Pair) {left.height + 1, left.lca};
    }
    if (left.height < right.height) { // 右子树更高
        return (Pair) {right.height + 1, right.lca};
    }
    return (Pair) {left.height + 1, node}; // 一样高
}

struct TreeNode* lcaDeepestLeaves(struct TreeNode* root) {
    return dfs(root).lca;
}
```

```go [sol-Go]
func dfs(node *TreeNode) (int, *TreeNode) {
    if node == nil {
        return 0, nil
    }
    leftHeight, leftLCA := dfs(node.Left)
    rightHeight, rightLCA := dfs(node.Right)
    if leftHeight > rightHeight { // 左子树更高
        return leftHeight + 1, leftLCA
    }
    if leftHeight < rightHeight { // 右子树更高
        return rightHeight + 1, rightLCA
    }
    return leftHeight + 1, node // 一样高
}

func lcaDeepestLeaves(root *TreeNode) *TreeNode {
    _, lca := dfs(root)
    return lca
}
```

```js [sol-JavaScript]
var dfs = function(node) {
    if (node === null) {
        return [0, null];
    }
    const [leftHeight, leftLca] = dfs(node.left);
    const [rightHeight, rightLca] = dfs(node.right);
    if (leftHeight > rightHeight) { // 左子树更高
        return [leftHeight + 1, leftLca];
    }
    if (leftHeight < rightHeight) { // 右子树更高
        return [rightHeight + 1, rightLca];
    }
    return [leftHeight + 1, node]; // 一样高
};

var lcaDeepestLeaves = function(root) {
    return dfs(root)[1];
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn lca_deepest_leaves(root: Option<Rc<RefCell<TreeNode>>>) -> Option<Rc<RefCell<TreeNode>>> {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>) -> (i32, Option<Rc<RefCell<TreeNode>>>) {
            if let Some(n) = node {
                let n = n.borrow();
                let (left_height, left_lca) = dfs(&n.left);
                let (right_height, right_lca) = dfs(&n.right);
                if left_height > right_height {
                    return (left_height + 1, left_lca); // 左子树更高
                }
                if left_height < right_height {
                    return (right_height + 1, right_lca); // 右子树更高
                }
                (left_height + 1, node.clone()) // 一样高
            } else {
                (0, None)
            }
        }
        dfs(&root).1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。每个节点都会恰好访问一次。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树是一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

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

本题来自 `二、二叉树 / §2.8 最近公共祖先`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.8 最近公共祖先`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
