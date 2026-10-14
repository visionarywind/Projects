# 938. 二叉搜索树的范围和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/range-sum-of-bst/
- 题目 slug：`range-sum-of-bst`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.9 二叉搜索树
- 难度分：1335
- 外部题解来源：https://leetcode.cn/problems/range-sum-of-bst/solutions/2653989/jian-ji-xie-fa-pythonjavacgojsrust-by-en-7jw4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/range-sum-of-bst/solutions/2653989/jian-ji-xie-fa-pythonjavacgojsrust-by-en-7jw4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-pythonjavacgojsrust-by-en-7jw4`
- topic id：`2653989`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

二叉搜索树满足如下性质：

- 左子树的节点值都小于根节点的值。
- 右子树的节点值都大于根节点的值。
- 任意节点的左子树和右子树都是二叉搜索树。

根据这些性质，设根节点的值为 $x$，分类讨论：

- 如果 $x>\textit{high}$，那么 $x$ 不在 $[\textit{low},\textit{high}]$ 范围内，并且根据二叉搜索树的性质，右子树的所有节点值都大于 $x$，从而大于 $\textit{high}$，所以也不在 $[\textit{low},\textit{high}]$ 范围内，我们只需计算**左子树**的在 $[\textit{low},\textit{high}]$ 范围内的节点值之和。
- 如果 $x<\textit{low}$，那么 $x$ 不在 $[\textit{low},\textit{high}]$ 范围内，并且根据二叉搜索树的性质，左子树的所有节点值都小于 $x$，从而小于 $\textit{low}$，所以也不在 $[\textit{low},\textit{high}]$ 范围内，我们只需计算**右子树**的在 $[\textit{low},\textit{high}]$ 范围内的节点值之和。
- 否则，$\textit{low}\le x \le \textit{high}$，那么 $x$ 在 $[\textit{low},\textit{high}]$ 范围内，且左右子树都可能有节点在 $[\textit{low},\textit{high}]$ 范围内，我们累加 $x$、左子树的在 $[\textit{low},\textit{high}]$ 范围内的节点值之和、右子树的在 $[\textit{low},\textit{high}]$ 范围内的节点值之和，这三部分的和作为答案。

由于要计算的问题都形如「某棵子树在 $[\textit{low},\textit{high}]$ 范围内的节点值之和」，所以可以用**递归**解决。

晕递归的同学推荐先看 [深入理解递归【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

> 注：如果你知道线段树，可以看出下面的代码就是线段树的 `query`。

#### 写法一

```py [sol-Python3]
class Solution:
    def rangeSumBST(self, root: Optional[TreeNode], low: int, high: int) -> int:
        if root is None:
            return 0
        x = root.val
        if x > high:  # 右子树没有节点在范围内，只需递归左子树
            return self.rangeSumBST(root.left, low, high)
        if x < low:  # 左子树没有节点在范围内，只需递归右子树
            return self.rangeSumBST(root.right, low, high)
        return x + self.rangeSumBST(root.left, low, high) + \
                   self.rangeSumBST(root.right, low, high)
```

```java [sol-Java]
class Solution {
    public int rangeSumBST(TreeNode root, int low, int high) {
        if (root == null) {
            return 0;
        }
        int x = root.val;
        if (x > high) { // 右子树没有节点在范围内，只需递归左子树
            return rangeSumBST(root.left, low, high);
        }
        if (x < low) { // 左子树没有节点在范围内，只需递归右子树
            return rangeSumBST(root.right, low, high);
        }
        return x + rangeSumBST(root.left, low, high) +
                   rangeSumBST(root.right, low, high);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int rangeSumBST(TreeNode *root, int low, int high) {
        if (root == nullptr) {
            return 0;
        }
        int x = root->val;
        if (x > high) { // 右子树没有节点在范围内，只需递归左子树
            return rangeSumBST(root->left, low, high);
        }
        if (x < low) { // 左子树没有节点在范围内，只需递归右子树
            return rangeSumBST(root->right, low, high);
        }
        return x + rangeSumBST(root->left, low, high) +
                   rangeSumBST(root->right, low, high);
    }
};
```

```go [sol-Go]
func rangeSumBST(root *TreeNode, low, high int) int {
    if root == nil {
        return 0
    }
    x := root.Val
    if x > high { // 右子树没有节点在范围内，只需递归左子树
        return rangeSumBST(root.Left, low, high)
    }
    if x < low { // 左子树没有节点在范围内，只需递归右子树
        return rangeSumBST(root.Right, low, high)
    }
    return x + rangeSumBST(root.Left, low, high) +
               rangeSumBST(root.Right, low, high)
}
```

```js [sol-JavaScript]
var rangeSumBST = function(root, low, high) {
    if (root === null) {
        return 0;
    }
    const x = root.val;
    if (x > high) { // 右子树没有节点在范围内，只需递归左子树
        return rangeSumBST(root.left, low, high);
    }
    if (x < low) { // 左子树没有节点在范围内，只需递归右子树
        return rangeSumBST(root.right, low, high);
    }
    return x + rangeSumBST(root.left, low, high) +
               rangeSumBST(root.right, low, high);
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn range_sum_bst(root: Option<Rc<RefCell<TreeNode>>>, low: i32, high: i32) -> i32 {
        if let Some(node) = root {
            let mut node = node.borrow_mut();
            let x = node.val;
            if x > high { // 右子树没有节点在范围内，只需递归左子树
                Self::range_sum_bst(node.left.take(), low, high)
            } else if x < low { // 左子树没有节点在范围内，只需递归右子树
                Self::range_sum_bst(node.right.take(), low, high)
            } else {
                x + Self::range_sum_bst(node.left.take(), low, high) +
                    Self::range_sum_bst(node.right.take(), low, high)
            }
        } else {
            0
        }
    }
}
```

#### 写法二

```py [sol-Python3]
class Solution:
    def rangeSumBST(self, root: Optional[TreeNode], low: int, high: int) -> int:
        if root is None:
            return 0
        x = root.val
        s = x if low <= x <= high else 0
        if x > low:  # 左子树可能有节点值在范围内
            s += self.rangeSumBST(root.left, low, high)
        if x < high:  # 右子树可能有节点值在范围内
            s += self.rangeSumBST(root.right, low, high)
        return s
```

```java [sol-Java]
class Solution {
    public int rangeSumBST(TreeNode root, int low, int high) {
        if (root == null) {
            return 0;
        }
        int x = root.val;
        int sum = low <= x && x <= high ? x : 0;
        if (x > low) { // 左子树可能有节点值在范围内
            sum += rangeSumBST(root.left, low, high);
        }
        if (x < high) { // 右子树可能有节点值在范围内
            sum += rangeSumBST(root.right, low, high);
        }
        return sum;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int rangeSumBST(TreeNode *root, int low, int high) {
        if (root == nullptr) {
            return 0;
        }
        int x = root->val;
        int sum = low <= x && x <= high ? x : 0;
        if (x > low) { // 左子树可能有节点值在范围内
            sum += rangeSumBST(root->left, low, high);
        }
        if (x < high) { // 右子树可能有节点值在范围内
            sum += rangeSumBST(root->right, low, high);
        }
        return sum;
    }
};
```

```go [sol-Go]
func rangeSumBST(root *TreeNode, low, high int) (sum int) {
    if root == nil {
        return
    }
    x := root.Val
    if low <= x && x <= high { // x 在范围内
        sum = x
    }
    if x > low { // 左子树可能有节点值在范围内
        sum += rangeSumBST(root.Left, low, high)
    }
    if x < high { // 右子树可能有节点值在范围内
        sum += rangeSumBST(root.Right, low, high)
    }
    return
}
```

```js [sol-JavaScript]
var rangeSumBST = function(root, low, high) {
    if (root === null) {
        return 0;
    }
    const x = root.val;
    let sum = low <= x && x <= high ? x : 0;
    if (x > low) { // 左子树可能有节点值在范围内
        sum += rangeSumBST(root.left, low, high);
    }
    if (x < high) { // 右子树可能有节点值在范围内
        sum += rangeSumBST(root.right, low, high);
    }
    return sum;
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn range_sum_bst(root: Option<Rc<RefCell<TreeNode>>>, low: i32, high: i32) -> i32 {
        if let Some(node) = root {
            let mut node = node.borrow_mut();
            let x = node.val;
            let mut sum = if low <= x && x <= high { x } else { 0 };
            if x > low { // 左子树可能有节点值在范围内
                sum += Self::range_sum_bst(node.left.take(), low, high);
            }
            if x < high { // 右子树可能有节点值在范围内
                sum += Self::range_sum_bst(node.right.take(), low, high);
            }
            sum
        } else {
            0
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉搜索树的节点个数。最坏情况下每个节点都在范围内，需要遍历整棵树。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下二叉搜索树是一条链（注意题目没有保证树是平衡的），递归需要 $\mathcal{O}(n)$ 的栈空间。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
