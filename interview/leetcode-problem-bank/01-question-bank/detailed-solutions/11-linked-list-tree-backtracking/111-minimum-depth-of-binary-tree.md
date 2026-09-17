# 111. 二叉树的最小深度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-depth-of-binary-tree/
- 题目 slug：`minimum-depth-of-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.2 自顶向下 DFS（先序遍历）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/minimum-depth-of-binary-tree/solutions/2730984/liang-chong-fang-fa-zi-ding-xiang-xia-zi-0sxz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：自顶向下/自底向上（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-depth-of-binary-tree/solutions/2730984/liang-chong-fang-fa-zi-ding-xiang-xia-zi-0sxz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-zi-ding-xiang-xia-zi-0sxz`
- topic id：`2730984`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 方法一：自顶向下「递」

![lc111.jpg](https://pic.leetcode.cn/1712631372-ryxZkR-lc111.jpg)

我们可以在 DFS 这棵树的同时，额外传入一个计数器 $\textit{cnt}$，表示路径上的节点个数，例如上图从根到叶子的路径 $3\rightarrow 20\rightarrow 15$：

1. 递归前，$\textit{cnt}=0$。
2. 从 $3$ 开始递归，$\textit{cnt}$ 加一，现在 $\textit{cnt}=1$。
3. 向下递归到 $20$，$\textit{cnt}$ 加一，现在 $\textit{cnt}=2$。
4. 向下递归到 $15$，$\textit{cnt}$ 加一，现在 $\textit{cnt}=3$。由于 $15$ 是叶子，用 $3$ 更新答案的最小值。

晕递归的同学，可以看视频讲解[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)，制作不易，欢迎点赞关注~

该方法对应视频中讲的第二种写法。

```py [sol-Python3]
class Solution:
    def minDepth(self, root: Optional[TreeNode]) -> int:
        ans = inf
        def dfs(node: Optional[TreeNode], cnt: int) -> None:
            if node is None:
                return
            cnt += 1
            if node.left is None and node.right is None:  # node 是叶子
                nonlocal ans
                ans = min(ans, cnt)
                return
            dfs(node.left, cnt)
            dfs(node.right, cnt)
        dfs(root, 0)
        return ans if root else 0
```

```java [sol-Java]
class Solution {
    private int ans = Integer.MAX_VALUE;

    public int minDepth(TreeNode root) {
        dfs(root, 0);
        return root != null ? ans : 0;
    }

    private void dfs(TreeNode node, int cnt) {
        if (node == null) {
            return;
        }
        cnt++;
        if (node.left == null && node.right == null) { // node 是叶子
            ans = Math.min(ans, cnt);
            return;
        }
        dfs(node.left, cnt);
        dfs(node.right, cnt);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minDepth(TreeNode* root) {
        int ans = INT_MAX;
        auto dfs = [&](this auto&& dfs, TreeNode* node, int cnt) -> void {
            if (node == nullptr) {
                return;
            }
            cnt++;
            if (node->left == nullptr && node->right == nullptr) { // node 是叶子
                ans = min(ans, cnt);
                return;
            }
            dfs(node->left, cnt);
            dfs(node->right, cnt);
        };
        dfs(root, 0);
        return root ? ans : 0;
    }
};
```

```go [sol-Go]
func minDepth(root *TreeNode) int {
    ans := math.MaxInt
    var dfs func(*TreeNode, int)
    dfs = func(node *TreeNode, cnt int) {
        if node == nil {
            return
        }
        cnt++
        if node.Left == nil && node.Right == nil { // node 是叶子
            ans = min(ans, cnt)
            return
        }
        dfs(node.Left, cnt)
        dfs(node.Right, cnt)
    }
    dfs(root, 0)
    if root != nil {
        return ans
    }
    return 0
}
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int minDepth(struct TreeNode* root) {
    int ans = INT_MAX;
    void dfs(struct TreeNode* node, int cnt) {
        if (node == NULL) {
            return;
        }
        cnt++;
        if (node->left == NULL && node->right == NULL) { // node 是叶子
            ans = MIN(ans, cnt);
            return;
        }
        dfs(node->left, cnt);
        dfs(node->right, cnt);
    }
    dfs(root, 0);
    return root ? ans : 0;
}
```

```js [sol-JavaScript]
var minDepth = function(root) {
    let ans = Infinity;
    function dfs(node, cnt) {
        if (node === null) {
            return;
        }
        cnt++;
        if (node.left === null && node.right === null) { // node 是叶子
            ans = Math.min(ans, cnt);
            return;
        }
        dfs(node.left, cnt);
        dfs(node.right, cnt);
    }
    dfs(root, 0);
    return root ? ans : 0;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn min_depth(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, cnt: i32, ans: &mut i32) {
            if let Some(node) = node {
                let node = node.borrow();
                let cnt = cnt + 1;
                if node.left.is_none() && node.right.is_none() {
                    *ans = (*ans).min(cnt);
                    return;
                }
                dfs(&node.left, cnt, ans);
                dfs(&node.right, cnt, ans);
            }
        }
        let mut ans = i32::MAX;
        dfs(&root, 0, &mut ans);
        if root.is_some() { ans } else { 0 }
    }
}
```

### 优化

如果递归中发现 $\textit{cnt}\ge \textit{ans}$，由于继续向下递归也不会让 $\textit{ans}$ 变小，直接返回。

这一技巧叫做「最优性剪枝」。

```py [sol-Python3]
class Solution:
    def minDepth(self, root: Optional[TreeNode]) -> int:
        ans = inf
        def dfs(node: Optional[TreeNode], cnt: int) -> None:
            if node is None:
                return
            nonlocal ans
            cnt += 1
            if cnt >= ans:
                return  # 最优性剪枝
            if node.left is None and node.right is None:  # node 是叶子
                ans = cnt
                return
            dfs(node.left, cnt)
            dfs(node.right, cnt)
        dfs(root, 0)
        return ans if root else 0
```

```java [sol-Java]
class Solution {
    private int ans = Integer.MAX_VALUE;

    public int minDepth(TreeNode root) {
        dfs(root, 0);
        return root != null ? ans : 0;
    }

    private void dfs(TreeNode node, int cnt) {
        if (node == null || ++cnt >= ans) { // 最优性剪枝
            return;
        }
        if (node.left == null && node.right == null) { // node 是叶子
            ans = cnt;
            return;
        }
        dfs(node.left, cnt);
        dfs(node.right, cnt);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minDepth(TreeNode* root) {
        int ans = INT_MAX;
        auto dfs = [&](this auto&& dfs, TreeNode* node, int cnt) -> void {
            if (node == nullptr || ++cnt >= ans) { // 最优性剪枝
                return;
            }
            if (node->left == nullptr && node->right == nullptr) { // node 是叶子
                ans = cnt;
                return;
            }
            dfs(node->left, cnt);
            dfs(node->right, cnt);
        };
        dfs(root, 0);
        return root ? ans : 0;
    }
};
```

```c [sol-C]
int minDepth(struct TreeNode* root) {
    int ans = INT_MAX;
    void dfs(struct TreeNode* node, int cnt) {
        if (node == NULL || ++cnt >= ans) { // 最优性剪枝
            return;
        }
        if (node->left == NULL && node->right == NULL) { // node 是叶子
            ans = cnt;
            return;
        }
        dfs(node->left, cnt);
        dfs(node->right, cnt);
    }
    dfs(root, 0);
    return root ? ans : 0;
}
```

```go [sol-Go]
func minDepth(root *TreeNode) int {
    ans := math.MaxInt
    var dfs func(*TreeNode, int)
    dfs = func(node *TreeNode, cnt int) {
        if node == nil {
            return
        }
        cnt++
        if cnt >= ans {
            return // 最优性剪枝
        }
        if node.Left == nil && node.Right == nil { // node 是叶子
            ans = cnt
            return
        }
        dfs(node.Left, cnt)
        dfs(node.Right, cnt)
    }
    dfs(root, 0)
    if root != nil {
        return ans
    }
    return 0
}
```

```js [sol-JavaScript]
var minDepth = function(root) {
    let ans = Infinity;
    function dfs(node, cnt) {
        if (node === null || ++cnt >= ans) { // 最优性剪枝
            return;
        }
        if (node.left === null && node.right === null) { // node 是叶子
            ans = cnt;
            return;
        }
        dfs(node.left, cnt);
        dfs(node.right, cnt);
    }
    dfs(root, 0);
    return root ? ans : 0;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn min_depth(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, cnt: i32, ans: &mut i32) {
            if let Some(node) = node {
                let node = node.borrow();
                let cnt = cnt + 1;
                if cnt >= *ans {
                    return; // 最优性剪枝
                }
                if node.left.is_none() && node.right.is_none() { // node 是叶子
                    *ans = cnt;
                    return;
                }
                dfs(&node.left, cnt, ans);
                dfs(&node.right, cnt, ans);
            }
        }
        let mut ans = i32::MAX;
        dfs(&root, 0, &mut ans);
        if root.is_some() { ans } else { 0 }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

## 方法二：自底向上「归」

定义 $\textit{dfs}(\textit{node})$ 表示以节点 $\textit{node}$ 为根的**子树**的最小深度。

### 写法一

分类讨论：

- 如果 $\textit{node}$ 是空节点，由于没有节点，返回 $0$。
- 如果 $\textit{node}$ 没有右儿子，那么深度就是左子树的深度加一，即 $\textit{dfs}(\textit{node}) = \textit{dfs}(\textit{node}.\textit{left}) + 1$。
- 如果 $\textit{node}$ 没有左儿子，那么深度就是右子树的深度加一，即 $\textit{dfs}(\textit{node}) = \textit{dfs}(\textit{node}.\textit{right}) + 1$。
- 如果 $\textit{node}$ 左右儿子都有，那么分别递归计算左子树的深度，以及右子树的深度，二者取最小值再加一，即
  $$
  \textit{dfs}(\textit{node}) = \min(\textit{dfs}(\textit{node}.\textit{left}), \textit{dfs}(\textit{node}.\textit{right})) + 1
  $$

注意：并不需要特判 $\textit{node}$ 是叶子的情况，因为在没有右儿子的情况下，我们会递归 $\textit{node}.\textit{left}$，如果它是空节点，递归的返回值是 $0$，加一后得到 $1$，这正是叶子节点要返回的值。

答案：$\textit{dfs}(\textit{root})$。

代码实现时，可以直接递归调用 $\texttt{minDepth}$。

### 答疑

**问**：本题和 [104. 二叉树的最大深度](https://leetcode.cn/problems/maximum-depth-of-binary-tree/) 的区别是什么？为什么本题代码要更复杂一些？

**答**：对于**非叶节点**，把握一个共同原则：如果一个儿子是空节点，另一个儿子不是空节点，那么**答案只能来自非空的那一侧**。

- 求最大深度，空节点返回 $0$，直接计算 $\max$，一定会取到有节点的那一侧（因为深度比 $0$ 大）。
- 求最小深度，空节点返回 $0$，直接计算 $\min$，会取到空节点，不符合「答案只能来自非空的那一侧」。所以求最小深度必须多写一些逻辑。

```py [sol-Python3]
class Solution:
    def minDepth(self, root: Optional[TreeNode]) -> int:
        if root is None:
            return 0
        if root.right is None:
            return self.minDepth(root.left) + 1
        if root.left is None:
            return self.minDepth(root.right) + 1
        return min(self.minDepth(root.left), self.minDepth(root.right)) + 1
```

```java [sol-Java]
class Solution {
    public int minDepth(TreeNode root) {
        if (root == null) {
            return 0;
        }
        if (root.right == null) {
            return minDepth(root.left) + 1;
        }
        if (root.left == null) {
            return minDepth(root.right) + 1;
        }
        return Math.min(minDepth(root.left), minDepth(root.right)) + 1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minDepth(TreeNode* root) {
        if (root == nullptr) {
            return 0;
        }
        if (root->right == nullptr) {
            return minDepth(root->left) + 1;
        }
        if (root->left == nullptr) {
            return minDepth(root->right) + 1;
        }
        return min(minDepth(root->left), minDepth(root->right)) + 1;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int minDepth(struct TreeNode* root) {
    if (root == NULL) {
        return 0;
    }
    if (root->right == NULL) {
        return minDepth(root->left) + 1;
    }
    if (root->left == NULL) {
        return minDepth(root->right) + 1;
    }
    int left = minDepth(root->left);
    int right = minDepth(root->right);
    return MIN(left, right) + 1;
}
```

```go [sol-Go]
func minDepth(root *TreeNode) int {
    if root == nil {
        return 0
    }
    if root.Right == nil {
        return minDepth(root.Left) + 1
    }
    if root.Left == nil {
        return minDepth(root.Right) + 1
    }
    return min(minDepth(root.Left), minDepth(root.Right)) + 1
}
```

```js [sol-JavaScript]
var minDepth = function(root) {
    if (root === null) {
        return 0;
    }
    if (root.right === null) {
        return minDepth(root.left) + 1;
    }
    if (root.left === null) {
        return minDepth(root.right) + 1;
    }
    return Math.min(minDepth(root.left), minDepth(root.right)) + 1;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn min_depth(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        if let Some(node) = root {
            let mut node = node.borrow_mut();
            if node.right.is_none() {
                return Self::min_depth(node.left.take()) + 1;
            }
            if node.left.is_none() {
                return Self::min_depth(node.right.take()) + 1;
            }
            return Self::min_depth(node.left.take()).min(Self::min_depth(node.right.take())) + 1;
        }
        0
    }
}
```

### 写法二

分类讨论：

- 如果 $\textit{node}$ 是空节点，返回 $0$。
- 如果 $\textit{node}$ 是叶子节点，返回 $1$。
- 否则，计算 $\textit{leftDepth}$，如果左儿子不是空节点，那么 $\textit{leftDepth}=\texttt{minDepth}(\textit{node}.\textit{left})$，否则 $\textit{leftDepth}=\infty$，这样后面计算 $\min$ 不会取到 $\infty$。对于右儿子也同理，计算出 $\textit{rightDepth}$。最后返回 $\min(\textit{leftDepth},\textit{rightDepth}) + 1$。

```py [sol-Python3]
class Solution:
    def minDepth(self, root: Optional[TreeNode]) -> int:
        if root is None:
            return 0
        if root.left is None and root.right is None:  # root 是叶子
            return 1
        left_depth = self.minDepth(root.left) if root.left else inf
        right_depth = self.minDepth(root.right) if root.right else inf
        return min(left_depth, right_depth) + 1
```

```java [sol-Java]
class Solution {
    public int minDepth(TreeNode root) {
        if (root == null) {
            return 0;
        }
        if (root.left == null && root.right == null) { // root 是叶子
            return 1;
        }
        int leftDepth = root.left != null ? minDepth(root.left) : Integer.MAX_VALUE;
        int rightDepth = root.right != null ? minDepth(root.right) : Integer.MAX_VALUE;
        return Math.min(leftDepth, rightDepth) + 1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minDepth(TreeNode* root) {
        if (root == nullptr) {
            return 0;
        }
        if (root->left == nullptr && root->right == nullptr) { // root 是叶子
            return 1;
        }
        int left_depth = root->left ? minDepth(root->left) : INT_MAX;
        int right_depth = root->right ? minDepth(root->right) : INT_MAX;
        return min(left_depth, right_depth) + 1;
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int minDepth(struct TreeNode* root) {
    if (root == NULL) {
        return 0;
    }
    if (root->left == NULL && root->right == NULL) { // root 是叶子
        return 1;
    }
    int left_depth = root->left ? minDepth(root->left) : INT_MAX;
    int right_depth = root->right ? minDepth(root->right) : INT_MAX;
    return MIN(left_depth, right_depth) + 1;
}
```

```go [sol-Go]
func minDepth(root *TreeNode) int {
    if root == nil {
        return 0
    }
    if root.Left == nil && root.Right == nil { // root 是叶子
        return 1
    }
    leftDepth := math.MaxInt
    if root.Left != nil {
        leftDepth = minDepth(root.Left)
    }
    rightDepth := math.MaxInt
    if root.Right != nil {
        rightDepth = minDepth(root.Right)
    }
    return min(leftDepth, rightDepth) + 1
}
```

```js [sol-JavaScript]
var minDepth = function(root) {
    if (root === null) {
        return 0;
    }
    if (root.left === null && root.right === null) { // root 是叶子
        return 1;
    }
    const leftDepth = root.left ? minDepth(root.left) : Infinity;
    const rightDepth = root.right ? minDepth(root.right) : Infinity;
    return Math.min(leftDepth, rightDepth) + 1;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn min_depth(root: Option<Rc<RefCell<TreeNode>>>) -> i32 {
        if let Some(node) = root {
            let mut node = node.borrow_mut();
            if node.left.is_none() && node.right.is_none() {
                return 1;
            }
            let left_depth = if node.left.is_none() { i32::MAX } else { Self::min_depth(node.left.take()) };
            let right_depth = if node.right.is_none() { i32::MAX } else { Self::min_depth(node.right.take()) };
            return left_depth.min(right_depth) + 1;
        }
        0
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

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

本题来自 `二、二叉树 / §2.2 自顶向下 DFS（先序遍历）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.2 自顶向下 DFS（先序遍历）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
