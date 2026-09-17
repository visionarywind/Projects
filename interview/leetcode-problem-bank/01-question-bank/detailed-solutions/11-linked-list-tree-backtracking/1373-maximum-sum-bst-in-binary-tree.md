# 1373. 二叉搜索子树的最大键值和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-sum-bst-in-binary-tree/
- 题目 slug：`maximum-sum-bst-in-binary-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.9 二叉搜索树
- 难度分：1914
- 外部题解来源：https://leetcode.cn/problems/maximum-sum-bst-in-binary-tree/solutions/2276783/hou-xu-bian-li-pythonjavacgo-by-endlessc-gll3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[后序遍历（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-sum-bst-in-binary-tree/solutions/2276783/hou-xu-bian-li-pythonjavacgo-by-endlessc-gll3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hou-xu-bian-li-pythonjavacgo-by-endlessc-gll3`
- topic id：`2276783`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:39:45 +0800

## 本题前置题目

推荐先做 [98. 验证二叉搜索树](https://leetcode.cn/problems/validate-binary-search-tree/)，用**后序遍历**实现。

讲解：[验证二叉搜索树【基础算法精讲 11】](https://www.bilibili.com/video/BV14G411P7C1/)，欢迎点赞支持！

> APP 用户如果无法打开，可以分享到微信。

## 思路

每棵子树返回：

- 这棵子树的最小节点值。
- 这棵子树的最大节点值。
- 这棵子树的所有节点值之和。

如果左子树的最大值 $\ge$ 当前节点值，或者右子树的最小值 $\le$ 当前节点值，都不符合二叉搜索树的定义，那么当前这棵子树不是二叉搜索树。此时可以返回 $-\infty, \infty, 0$（第三个数随意）。这种返回值可以简化代码，对于父节点，同样通过判断子树的最小值/最大值和当前节点值的大小关系，就可以知道子树是否为二叉搜索树了。

如果当前子树是二叉搜索树，那么把左右子树的节点值之和，加上当前节点值，去更新答案的最大值，并作为当前子树的所有节点值之和，返回给父节点。

#### 答疑

**问**：如果左子树不是二叉搜索树，那么当前子树也不是二叉搜索树，此时能否直接返回，不去递归右子树？

**答**：这是不对的，因为答案可能在右子树中（如示例 1）。如果你觉得这是显然的，不妨做做 [2246. 相邻字符不同的最长路径](https://leetcode.cn/problems/longest-path-with-different-adjacent-characters/)，一个易错点是没有遍历所有节点。 

```py [sol1-Python3]
class Solution:
    def maxSumBST(self, root: Optional[TreeNode]) -> int:
        ans = 0  # 二叉搜索树可以为空

        def dfs(node: Optional[TreeNode]) -> Tuple:
            if node is None:
                return inf, -inf, 0

            l_min, l_max, l_sum = dfs(node.left)  # 递归左子树
            r_min, r_max, r_sum = dfs(node.right)  # 递归右子树
            x = node.val
            if x <= l_max or x >= r_min:  # 不是二叉搜索树
                return -inf, inf, 0

            s = l_sum + r_sum + x  # 这棵子树的所有节点值之和
            nonlocal ans
            ans = max(ans, s)

            return min(l_min, x), max(r_max, x), s

        dfs(root)
        return ans
```

```java [sol1-Java]
class Solution {
    private int ans; // 二叉搜索树可以为空

    public int maxSumBST(TreeNode root) {
        dfs(root);
        return ans;
    }

    private int[] dfs(TreeNode node) {
        if (node == null)
            return new int[]{Integer.MAX_VALUE, Integer.MIN_VALUE, 0};

        int[] left = dfs(node.left); // 递归左子树
        int[] right = dfs(node.right); // 递归右子树
        int x = node.val;
        if (x <= left[1] || x >= right[0]) // 不是二叉搜索树
            return new int[]{Integer.MIN_VALUE, Integer.MAX_VALUE, 0};

        int s = left[2] + right[2] + x; // 这棵子树的所有节点值之和
        ans = Math.max(ans, s);

        return new int[]{Math.min(left[0], x), Math.max(right[1], x), s};
    }
}
```

```cpp [sol1-C++]
class Solution {
    int ans = 0; // 二叉搜索树可以为空

    tuple<int, int, int> dfs(TreeNode *node) {
        if (node == nullptr)
            return {INT_MAX, INT_MIN, 0};

        auto [l_min, l_max, l_sum] = dfs(node->left); // 递归左子树
        auto [r_min, r_max, r_sum] = dfs(node->right); // 递归右子树
        int x = node->val;
        if (x <= l_max || x >= r_min) // 不是二叉搜索树
            return {INT_MIN, INT_MAX, 0};

        int s = l_sum + r_sum + x; // 这棵子树的所有节点值之和
        ans = max(ans, s);

        return {min(l_min, x), max(r_max, x), s};
    }

public:
    int maxSumBST(TreeNode *root) {
        dfs(root);
        return ans;
    }
};
```

```go [sol1-Go]
func maxSumBST(root *TreeNode) (ans int) {
    var dfs func(*TreeNode) (int, int, int)
    dfs = func(node *TreeNode) (int, int, int) {
        if node == nil {
            return math.MaxInt, math.MinInt, 0
        }

        lMin, lMax, lSum := dfs(node.Left) // 递归左子树
        rMin, rMax, rSum := dfs(node.Right) // 递归右子树
        x := node.Val
        if x <= lMax || x >= rMin { // 不是二叉搜索树
            return math.MinInt, math.MaxInt, 0
        }

        s := lSum + rSum + x // 这棵子树的所有节点值之和
        ans = max(ans, s)

        return min(lMin, x), max(rMax, x), s
    }
    dfs(root)
    return
}

func min(a, b int) int { if b < a { return b }; return a }
func max(a, b int) int { if b > a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉搜索树的节点个数。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉搜索树退化成一条链（注意题目没有保证它是**平衡**树），因此递归需要 $\mathcal{O}(n)$ 的栈空间。

#### 练习：二叉搜索树

- [98. 验证二叉搜索树](https://leetcode.cn/problems/validate-binary-search-tree/)
- [230. 二叉搜索树中第 K 小的元素](https://leetcode.cn/problems/kth-smallest-element-in-a-bst/)
- [501. 二叉搜索树中的众数](https://leetcode.cn/problems/find-mode-in-binary-search-tree/)
- [530. 二叉搜索树的最小绝对差](https://leetcode.cn/problems/minimum-absolute-difference-in-bst/)
- [700. 二叉搜索树中的搜索](https://leetcode.cn/problems/search-in-a-binary-search-tree/)

[往期每日一题题解](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

---

欢迎关注[ biIibiIi@灵茶山艾府](https://space.bilibili.com/206214)，高质量算法教学，持续输出中~

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
