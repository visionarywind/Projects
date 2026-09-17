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
