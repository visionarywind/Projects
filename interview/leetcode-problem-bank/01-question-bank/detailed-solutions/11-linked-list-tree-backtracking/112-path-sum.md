# 112. 路径总和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/path-sum/
- 题目 slug：`path-sum`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.2 自顶向下 DFS（先序遍历）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/path-sum/solutions/2731531/jian-ji-xie-fa-pythonjavacgojsrust-by-en-icwe/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/path-sum/solutions/2731531/jian-ji-xie-fa-pythonjavacgojsrust-by-en-icwe/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-pythonjavacgojsrust-by-en-icwe`
- topic id：`2731531`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

![lc112.jpg](https://pic.leetcode.cn/1712662366-MFwjGv-lc112.jpg)

看示例 1，我们可以从 $\textit{targetSum}=22$ 开始，不断地减去路径上的节点值，如果走到叶子节点发现 $\textit{targetSum}=0$，就说明我们找到了一条符合题目要求的路径。具体来说：

1. 递归前，$\textit{targetSum}=22$。
2. 从根节点 $5$ 开始递归，把 $\textit{targetSum}$ 减少 $5$，现在 $\textit{targetSum}=17$。
3. 向下递归到 $4$，把 $\textit{targetSum}$ 减少 $4$，现在 $\textit{targetSum}=13$。
4. 向下递归到 $11$，把 $\textit{targetSum}$ 减少 $11$，现在 $\textit{targetSum}=2$。
5. 向下递归到 $2$，把 $\textit{targetSum}$ 减少 $2$，现在 $\textit{targetSum}=0$。

这样「倒着减小」可以让我们直接递归调用 $\texttt{hasPathSum}$：

1. 如果 $\textit{root}$ 是空，返回 $\texttt{false}$。
2. 把 $\textit{targetSum}$ 减少 $\textit{root}.\textit{val}$。
3. 如果 $\textit{root}$ 是叶子节点：如果 $\textit{targetSum}=0$，返回 $\texttt{true}$，否则返回 $\texttt{false}$。
4. 递归左子树，如果左子树返回 $\texttt{true}$，那么当前子树就返回 $\texttt{true}$，否则返回递归右子树的结果。

晕递归的同学，可以看视频讲解[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)，制作不易，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def hasPathSum(self, root: Optional[TreeNode], targetSum: int) -> bool:
        if root is None:
            return False
        targetSum -= root.val
        if root.left is None and root.right is None:  # root 是叶子
            return targetSum == 0
        return self.hasPathSum(root.left, targetSum) or self.hasPathSum(root.right, targetSum)
```

```java [sol-Java]
class Solution {
    public boolean hasPathSum(TreeNode root, int targetSum) {
        if (root == null) {
            return false;
        }
        targetSum -= root.val;
        if (root.left == null && root.right == null) { // root 是叶子
            return targetSum == 0;
        }
        return hasPathSum(root.left, targetSum) || hasPathSum(root.right, targetSum);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool hasPathSum(TreeNode* root, int targetSum) {
        if (root == nullptr) {
            return false;
        }
        targetSum -= root->val;
        if (root->left == nullptr && root->right == nullptr) { // root 是叶子
            return targetSum == 0;
        }
        return hasPathSum(root->left, targetSum) || hasPathSum(root->right, targetSum);
    }
};
```

```c [sol-C]
bool hasPathSum(struct TreeNode* root, int targetSum) {
    if (root == NULL) {
        return false;
    }
    targetSum -= root->val;
    if (root->left == NULL && root->right == NULL) { // root 是叶子
        return targetSum == 0;
    }
    return hasPathSum(root->left, targetSum) || hasPathSum(root->right, targetSum);
}
```

```go [sol-Go]
func hasPathSum(root *TreeNode, targetSum int) bool {
    if root == nil {
        return false
    }
    targetSum -= root.Val
    if root.Left == nil && root.Right == nil { // root 是叶子
        return targetSum == 0
    }
    return hasPathSum(root.Left, targetSum) || hasPathSum(root.Right, targetSum)
}
```

```js [sol-JavaScript]
var hasPathSum = function(root, targetSum) {
    if (root === null) {
        return false;
    }
    targetSum -= root.val;
    if (root.left === null && root.right === null) { // root 是叶子
        return targetSum === 0;
    }
    return hasPathSum(root.left, targetSum) || hasPathSum(root.right, targetSum);
};
```

```rust [sol-Rust]
use std::rc::Rc;
use std::cell::RefCell;

impl Solution {
    pub fn has_path_sum(root: Option<Rc<RefCell<TreeNode>>>, target_sum: i32) -> bool {
        if let Some(node) = root {
            let mut node = node.borrow_mut();
            let target_sum = target_sum - node.val;
            if node.left.is_none() && node.right.is_none() { // root 是叶子
                return target_sum == 0;
            }
            return Self::has_path_sum(node.left.take(), target_sum) ||
                   Self::has_path_sum(node.right.take(), target_sum);
        }
        false
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

更多相似题目，见下面的链表二叉树题单。

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
