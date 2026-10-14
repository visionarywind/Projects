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
