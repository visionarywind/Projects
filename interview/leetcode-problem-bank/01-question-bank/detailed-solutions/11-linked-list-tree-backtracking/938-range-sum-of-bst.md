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

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.9 二叉搜索树`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.9 二叉搜索树`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
