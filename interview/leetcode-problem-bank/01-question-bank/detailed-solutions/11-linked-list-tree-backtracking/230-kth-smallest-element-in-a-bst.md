# 230. 二叉搜索树中第 K 小的元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/kth-smallest-element-in-a-bst/
- 题目 slug：`kth-smallest-element-in-a-bst`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.9 二叉搜索树
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/kth-smallest-element-in-a-bst/solutions/2952810/zhong-xu-bian-li-pythonjavaccgojsrust-by-wc02/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[中序遍历（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/kth-smallest-element-in-a-bst/solutions/2952810/zhong-xu-bian-li-pythonjavaccgojsrust-by-wc02/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhong-xu-bian-li-pythonjavaccgojsrust-by-wc02`
- topic id：`2952810`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

## 前置题目

[98. 验证二叉搜索树](https://leetcode.cn/problems/validate-binary-search-tree/)，请看 [视频讲解【基础算法精讲 11】](https://www.bilibili.com/video/BV14G411P7C1/)。

## 思路

由于**中序遍历**就是在从小到大遍历节点值，所以遍历到的第 $k$ 个节点值就是答案。

## 写法一：记录答案

在中序遍历，即「左-根-右」的过程中，每次递归完左子树，就把 $k$ 减少 $1$，表示我们按照中序遍历访问到了一个节点。如果减一后 $k$ 变成 $0$，那么答案就是当前节点的值，用一个外部变量 $\textit{ans}$ 记录。

```py [sol-Python3]
class Solution:
    def kthSmallest(self, root: Optional[TreeNode], k: int) -> int:
        ans = 0
        def dfs(node: Optional[TreeNode]) -> None:
            nonlocal k, ans
            if node is None or k <= 0:
                return
            dfs(node.left)  # 左
            k -= 1
            if k == 0:
                ans = node.val  # 根
            dfs(node.right)  # 右
        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans;
    private int k;

    public int kthSmallest(TreeNode root, int k) {
        this.k = k;
        dfs(root);
        return ans;
    }

    private void dfs(TreeNode node) {
        if (node == null || k <= 0) {
            return;
        }
        dfs(node.left); // 左
        if (--k == 0) {
            ans = node.val; // 根
        }
        dfs(node.right); // 右
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int kthSmallest(TreeNode* root, int k) {
        int ans;
        auto dfs = [&](this auto&& dfs, TreeNode* node) -> void {
            if (node == nullptr || k <= 0) {
                return;
            }
            dfs(node->left); // 左
            if (--k == 0) {
                ans = node->val; // 根
            }
            dfs(node->right); // 右
        };
        dfs(root);
        return ans;
    }
};
```

```c [sol-C]
int kthSmallest(struct TreeNode* root, int k) {
    int ans;
    void dfs(struct TreeNode* node) {
        if (node == NULL || k <= 0) {
            return;
        }
        dfs(node->left); // 左
        if (--k == 0) {
            ans = node->val; // 根
        }
        dfs(node->right); // 右
    }
    dfs(root);
    return ans;
}
```

```go [sol-Go]
func kthSmallest(root *TreeNode, k int) (ans int) {
    var dfs func(*TreeNode)
    dfs = func(node *TreeNode) {
        if node == nil || k <= 0 {
            return
        }
        dfs(node.Left) // 左
        k--
        if k == 0 {
            ans = node.Val // 根
        }
        dfs(node.Right) // 右
    }
    dfs(root)
    return
}
```

```js [sol-JavaScript]
var kthSmallest = function(root, k) {
    let ans = 0;
    function dfs(node) {
        if (node === null || k <= 0) {
            return;
        }
        dfs(node.left); // 左
        if (--k === 0) {
            ans = node.val; // 根
        }
        dfs(node.right); // 右
    }
    dfs(root);
    return ans;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn kth_smallest(root: Option<Rc<RefCell<TreeNode>>>, mut k: i32) -> i32 {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, k: &mut i32, ans: &mut i32) {
            if *k <= 0 {
                return;
            }
            if let Some(node) = node {
                let node = node.borrow();
                let left_res = dfs(&node.left, k, ans); // 左
                *k -= 1;
                if *k == 0 {
                    *ans = node.val; // 根
                }
                dfs(&node.right, k, ans); // 右
            }
        }
        let mut ans = 0;
        dfs(&root, &mut k, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的大小（节点个数）。
- 空间复杂度：$\mathcal{O}(h)$，其中 $h$ 是树高，递归需要 $\mathcal{O}(h)$ 的栈空间。最坏情况下树是一条链，$h=n$，空间复杂度为 $\mathcal{O}(n)$。

## 写法二：不记录答案 + 提前返回

写法一使用了一个外部变量记录答案，能否不使用外部变量记录呢？

可以，做法如下：

1. 递归边界：如果当前节点是空节点，返回 $-1$，表示没有找到。注意题目保证节点值非负。
2. 执行中序遍历，先递归左子树。
3. 判断左子树的返回值 $\textit{leftRes}$ 是否为 $-1$。如果不是 $-1$，说明我们在左子树中找到了答案，返回 $\textit{leftRes}$。如果是 $-1$，说明尚未找到答案，继续下一步。
4. 把 $k$ 减少 $1$。如果 $k=0$，那么答案就是当前节点值，返回当前节点值。
5. 现在，答案要么在当前节点的右子树中，要么在除了当前子树的其余节点中。递归右子树，如果答案在右子树中，那么直接返回答案；如果答案不在右子树中，那么右子树也会返回 $-1$，由于当前子树搜索完毕，所以当前子树没有找到答案，返回 $-1$。综上所述，可以直接返回右子树的返回值。

```py [sol-Python3]
class Solution:
    def kthSmallest(self, root: Optional[TreeNode], k: int) -> int:
        def dfs(node: Optional[TreeNode]) -> int:
            if node is None:
                return -1  # 题目保证节点值非负，用 -1 表示没有找到
            left_res = dfs(node.left)
            if left_res != -1:  # 答案在左子树中
                return left_res
            nonlocal k
            k -= 1
            if k == 0:  # 答案就是当前节点
                return node.val
            return dfs(node.right)  # 右子树会返回答案或者 -1
        return dfs(root)
```

```java [sol-Java]
class Solution {
    private int k;

    public int kthSmallest(TreeNode root, int k) {
        this.k = k;
        return dfs(root);
    }

    private int dfs(TreeNode node) {
        if (node == null) {
            return -1; // 题目保证节点值非负，用 -1 表示没有找到
        }
        int leftRes = dfs(node.left);
        if (leftRes != -1) { // 答案在左子树中
            return leftRes;
        }
        if (--k == 0) { // 答案就是当前节点
            return node.val;
        }
        return dfs(node.right); // 右子树会返回答案或者 -1
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int kthSmallest(TreeNode* root, int& k) { // 注意这里改成了引用
        if (root == nullptr) {
            return -1; // 题目保证节点值非负，用 -1 表示没有找到
        }
        int left_res = kthSmallest(root->left, k);
        if (left_res != -1) { // 答案在左子树中
            return left_res;
        }
        if (--k == 0) { // 答案就是当前节点
            return root->val;
        }
        return kthSmallest(root->right, k); // 右子树会返回答案或者 -1
    }
};
```

```cpp [sol-C++ 写法二]
class Solution {
public:
    int kthSmallest(TreeNode* root, int k) {
        auto dfs = [&](this auto&& dfs, TreeNode* node) -> int {
            if (node == nullptr) {
                return -1; // 题目保证节点值非负，用 -1 表示没有找到
            }
            int left_res = dfs(node->left);
            if (left_res != -1) { // 答案在左子树中
                return left_res;
            }
            if (--k == 0) { // 答案就是当前节点
                return node->val;
            }
            return dfs(node->right); // 右子树会返回答案或者 -1
        };
        return dfs(root);
    }
};
```

```c [sol-C]
int kthSmallest(struct TreeNode* root, int k) {
    int dfs(struct TreeNode* node) {
        if (node == NULL) {
            return -1; // 题目保证节点值非负，用 -1 表示没有找到
        }
        int left_res = dfs(node->left);
        if (left_res != -1) { // 答案在左子树中
            return left_res;
        }
        if (--k == 0) { // 答案就是当前节点
            return node->val;
        }
        return dfs(node->right); // 右子树会返回答案或者 -1
    }
    return dfs(root);
}
```

```go [sol-Go]
func kthSmallest(root *TreeNode, k int) int {
    var dfs func(*TreeNode) int
    dfs = func(node *TreeNode) int {
        if node == nil {
            return -1 // 题目保证节点值非负，用 -1 表示没有找到
        }
        leftRes := dfs(node.Left)
        if leftRes != -1 { // 答案在左子树中
            return leftRes
        }
        k--
        if k == 0 { // 答案就是当前节点
            return node.Val
        }
        return dfs(node.Right) // 右子树会返回答案或者 -1
    }
    return dfs(root)
}
```

```js [sol-JavaScript]
var kthSmallest = function(root, k) {
    function dfs(node) {
        if (node === null) {
            return -1; // 题目保证节点值非负，用 -1 表示没有找到
        }
        const leftRes = dfs(node.left);
        if (leftRes !== -1) { // 答案在左子树中
            return leftRes;
        }
        if (--k === 0) { // 答案就是当前节点
            return node.val;
        }
        return dfs(node.right); // 右子树会返回答案或者 -1
    }
    return dfs(root);
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn kth_smallest(root: Option<Rc<RefCell<TreeNode>>>, mut k: i32) -> i32 {
        fn dfs(node: &Option<Rc<RefCell<TreeNode>>>, k: &mut i32) -> i32 {
            if let Some(node) = node {
                let node = node.borrow();
                let left_res = dfs(&node.left, k);
                if left_res != -1 { // 答案在左子树中
                    return left_res;
                }
                *k -= 1;
                if *k == 0 { // 答案就是当前节点
                    return node.val;
                }
                return dfs(&node.right, k); // 右子树会返回答案或者 -1
            }
            -1 // 题目保证节点值非负，用 -1 表示没有找到
        }
        dfs(&root, &mut k)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是二叉树的大小（节点个数）。
- 空间复杂度：$\mathcal{O}(h)$，其中 $h$ 是树高，递归需要 $\mathcal{O}(h)$ 的栈空间。最坏情况下树是一条链，$h=n$，空间复杂度为 $\mathcal{O}(n)$。

## 思考题

改成求第 $k$ **大**要怎么做？

更多相似题目，见下面链表与二叉树题单中的「**二叉搜索树**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)
