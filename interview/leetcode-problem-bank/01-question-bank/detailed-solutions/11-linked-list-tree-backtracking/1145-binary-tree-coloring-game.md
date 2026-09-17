# 1145. 二叉树着色游戏

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/binary-tree-coloring-game/
- 题目 slug：`binary-tree-coloring-game`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.3 自底向上 DFS（后序遍历）
- 难度分：1741
- 外部题解来源：https://leetcode.cn/problems/binary-tree-coloring-game/solutions/2089813/mei-you-si-lu-yi-zhang-tu-miao-dong-pyth-btav/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[没有思路？一张图秒懂！（Python/Java/C++/Go）](https://leetcode.cn/problems/binary-tree-coloring-game/solutions/2089813/mei-you-si-lu-yi-zhang-tu-miao-dong-pyth-btav/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-you-si-lu-yi-zhang-tu-miao-dong-pyth-btav`
- topic id：`2089813`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

![1145-2.png](https://pic.leetcode.cn/1675081530-kqtMKF-1145-2.png)

以 $x$ 为根，它的三个邻居（左儿子、右儿子和父节点）就对应着三棵子树：

- 左子树
- 右子树
- 父节点子树

哪棵子树最大，二号玩家就选哪棵。

设 $n_2$ 为二号玩家最多可以染的节点个数，左子树的大小为 $\textit{lsz}$，右子树的大小为 $\textit{rsz}$，那么父节点子树的大小就是 $n-1-\textit{lsz}-\textit{rsz}$，因此

$$
n_2 = \max(\textit{lsz},\textit{rsz},n-1-\textit{lsz}-\textit{rsz})
$$

一号玩家染的节点个数为 $n-n_2$，获胜条件为 $n_2 > n-n_2$，即 $2\cdot n_2 > n$。

计算子树大小可以用**深度优先搜索**，如果你不了解这块内容，可以看我精心制作的[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

```py [sol1-Python3]
class Solution:
    def btreeGameWinningMove(self, root: Optional[TreeNode], n: int, x: int) -> bool:
        lsz = rsz = 0
        def dfs(node: Optional[TreeNode]) -> int:
            if node is None:
                return 0
            ls = dfs(node.left)
            rs = dfs(node.right)
            if node.val == x:
                nonlocal lsz, rsz
                lsz, rsz = ls, rs
            return ls + rs + 1
        dfs(root)
        return max(lsz, rsz, n - 1 - lsz - rsz) * 2 > n
```

```java [sol1-Java]
class Solution {
    private int x, lsz, rsz;

    public boolean btreeGameWinningMove(TreeNode root, int n, int x) {
        this.x = x;
        dfs(root);
        return Math.max(Math.max(lsz, rsz), n - 1 - lsz - rsz) * 2 > n;
    }

    private int dfs(TreeNode node) {
        if (node == null) 
            return 0;
        int ls = dfs(node.left);
        int rs = dfs(node.right);
        if (node.val == x) {
            lsz = ls;
            rsz = rs;
        }
        return ls + rs + 1;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    bool btreeGameWinningMove(TreeNode *root, int n, int x) {
        int lsz, rsz;
        function<int(TreeNode *)> dfs = [&](TreeNode *node) {
            if (node == nullptr)
                return 0;
            int ls = dfs(node->left);
            int rs = dfs(node->right);
            if (node->val == x)
                lsz = ls, rsz = rs;
            return ls + rs + 1;
        };
        dfs(root);
        return max({lsz, rsz, n - 1 - lsz - rsz}) * 2 > n;
    }
};
```

```go [sol1-Go]
func btreeGameWinningMove(root *TreeNode, n, x int) bool {
    lsz, rsz := 0, 0
    var dfs func(*TreeNode) int
    dfs = func(node *TreeNode) int {
        if node == nil {
            return 0
        }
        ls := dfs(node.Left)
        rs := dfs(node.Right)
        if node.Val == x {
            lsz, rsz = ls, rs
        }
        return ls + rs + 1
    }
    dfs(root)
    return max(max(lsz, rsz), n-1-lsz-rsz)*2 > n
}

func max(a, b int) int { if b > a { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为二叉树的节点个数。每个节点仅被访问一次。
- 空间复杂度：$O(n)$。最坏情况下，二叉树是一条链，递归需要 $O(n)$ 的栈空间。

### 思考题

假如你是一号玩家，$x$ 由你决定，你是否有**必胜**策略？

如果题目给的是一般的树呢（不是二叉树）？

见 [树的重心](https://oi-wiki.org/graph/tree-centroid/)。

把 $x$ 选在重心上，一号玩家是必胜的。

## 本地原创解析

### 1. 题意重述

本题来自 `二、二叉树 / §2.3 自底向上 DFS（后序遍历）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二叉树 / §2.3 自底向上 DFS（后序遍历）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
