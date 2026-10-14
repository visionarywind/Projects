# 1448. 统计二叉树中好节点的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-good-nodes-in-binary-tree/
- 题目 slug：`count-good-nodes-in-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.2 自顶向下 DFS（先序遍历）
- 难度分：1360
- 外部题解来源：https://leetcode.cn/problems/count-good-nodes-in-binary-tree/solutions/2403677/jian-ji-xie-fa-pythonjavacgojs-by-endles-gwxt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/count-good-nodes-in-binary-tree/solutions/2403677/jian-ji-xie-fa-pythonjavacgojs-by-endles-gwxt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-pythonjavacgojs-by-endles-gwxt`
- topic id：`2403677`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:30:25 +0800

## 前置知识

[看到递归就晕？带你理解递归的本质！【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)

## 算法

递归递归，先「递」后「归」。

我们可以在向下递归的同时，额外维护一个参数 $\textit{mx}$ 表示从根节点到当前节点之前，路径上的最大节点值。

- 如果当前节点为空，到达递归边界，返回 $0$。
- 递归左子树 `goodNodes(root.left, max(mx, root.val))`，获取左子树的好节点个数 $\textit{left}$。
- 递归右子树 `goodNodes(root.right, max(mx, root.val))`，获取右子树的好节点个数 $\textit{right}$。
- 如果当前节点是好节点，即 `mx <= root.val`，那么返回 $\textit{left}+\textit{right}+1$。否则返回 $\textit{left}+\textit{right}$。

```py [sol-Python3]
class Solution:
    def goodNodes(self, root: TreeNode, mx=-inf) -> int:
        if root is None:
            return 0
        left = self.goodNodes(root.left, max(mx, root.val))
        right = self.goodNodes(root.right, max(mx, root.val))
        return left + right + (mx <= root.val)
```

```java [sol-Java]
class Solution {
    public int goodNodes(TreeNode root) {
        return dfs(root, Integer.MIN_VALUE); // 也可以写 root.val
    }

    private int dfs(TreeNode root, int mx) {
        if (root == null)
            return 0;
        int left = dfs(root.left, Math.max(mx, root.val));
        int right = dfs(root.right, Math.max(mx, root.val));
        return left + right + (mx <= root.val ? 1 : 0);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int goodNodes(TreeNode *root, int mx = INT_MIN) {
        if (root == nullptr)
            return 0;
        int left = goodNodes(root->left, max(mx, root->val));
        int right = goodNodes(root->right, max(mx, root->val));
        return left + right + (mx <= root->val);
    }
};
```

```go [sol-Go]
func dfs(root *TreeNode, mx int) int {
    if root == nil {
        return 0
    }
    left := dfs(root.Left, max(mx, root.Val))
    right := dfs(root.Right, max(mx, root.Val))
    if mx <= root.Val {
        return left + right + 1
    }
    return left + right
}

func goodNodes(root *TreeNode) int {
    return dfs(root, math.MinInt) // 也可以写 root.Val
}

func max(a, b int) int { if b > a { return b }; return a }
```

```js [sol-JavaScript]
var goodNodes = function (root, mx = -Infinity) {
    if (root === null)
        return 0;
    const left = goodNodes(root.left, Math.max(mx, root.val));
    const right = goodNodes(root.right, Math.max(mx, root.val));
    return left + right + (mx <= root.val);
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。每个节点都会递归恰好一次。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树是一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

#### 练习题

- [100. 相同的树](https://leetcode.cn/problems/same-tree/)
- [101. 对称二叉树](https://leetcode.cn/problems/symmetric-tree/)
- [110. 平衡二叉树](https://leetcode.cn/problems/balanced-binary-tree/)

练习题的讲解请看[【基础算法精讲 10】](https://www.bilibili.com/video/BV18M411z7bb/)

[往期每日一题题解（按 tag 分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)
