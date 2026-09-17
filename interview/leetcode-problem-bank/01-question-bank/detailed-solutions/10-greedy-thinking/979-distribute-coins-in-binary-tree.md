# 979. 在二叉树中分配硬币

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/distribute-coins-in-binary-tree/
- 题目 slug：`distribute-coins-in-binary-tree`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.5 贡献法
- 难度分：1709
- 外部题解来源：https://leetcode.cn/problems/distribute-coins-in-binary-tree/solutions/2343262/tu-jie-mei-you-si-lu-jin-lai-miao-dong-p-vrni/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】贡献法（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/distribute-coins-in-binary-tree/solutions/2343262/tu-jie-mei-you-si-lu-jin-lai-miao-dong-p-vrni/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-mei-you-si-lu-jin-lai-miao-dong-p-vrni`
- topic id：`2343262`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

![lc979-c.png](https://pic.leetcode.cn/1689151400-COcNQp-lc979-c.png)

## 答疑

**问**：如何理解计算 $\textit{coins}$ 和 $\textit{nodes}$ 的公式？

**答**：请学习「递归」「子问题」等概念，具体请看[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

**问**：算法二的公式是怎么得到的？

**答**：（注意 $d$ 没有取绝对值，只在加到答案时取了绝对值）

$$
\begin{aligned}
d=&\ \textit{coins}-\textit{nodes}\\
=&\ (\textit{coins}_{\textit{left}} + \textit{coins}_{\textit{right}} + \textit{node}.\textit{val}) - (\textit{nodes}_{\textit{left}} + \textit{nodes}_{\textit{right}} + 1)\\
=&\ (\textit{coins}_{\textit{left}} -\textit{nodes}_{\textit{left}}) +(\textit{coins}_{\textit{right}} -\textit{nodes}_{\textit{right}}) +  \textit{node}.\textit{val} - 1\\
=&\ d_{\textit{left}} +d_{\textit{right}} +  \textit{node}.\textit{val} - 1
\end{aligned}
$$

**问**：根节点没有父节点，代码中为什么没有特判根节点呢？

**答**：根节点处统计的是整棵树，其中 $\textit{coins}=n,\textit{nodes}=n$。因为 $|\textit{coins}-\textit{nodes}|=0$，对答案无影响，所以无需特判根节点。

**问**：这种思路的**本质**是什么？

**答**：横看成岭侧成峰，每枚硬币移动的路径长度并不好计算，但是把这些路径**叠起来**，转换成每条边经过了多少枚硬币，就容易计算了（如下图）。

![lc979-3-c.png](https://pic.leetcode.cn/1689153803-DaPEzB-lc979-3-c.png)

路径是由边组成的，所有路径长度之和，等同于把「每条边出现在多少条路径中」相加。这种技巧叫做**贡献法**。更多相关题目，见文末的题单。

## 算法一

```py [sol-Python3]
class Solution:
    def distributeCoins(self, root: Optional[TreeNode]) -> int:
        def dfs(node: Optional[TreeNode]) -> Tuple[int, int]:
            if node is None:
                return 0, 0
            coins_l, nodes_l = dfs(node.left)
            coins_r, nodes_r = dfs(node.right)
            coins = coins_l + coins_r + node.val  # 子树硬币个数
            nodes = nodes_l + nodes_r + 1  # 子树节点数
            nonlocal ans
            ans += abs(coins - nodes)
            return coins, nodes

        ans = 0
        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans;

    public int distributeCoins(TreeNode root) {
        dfs(root);
        return ans;
    }

    private int[] dfs(TreeNode node) {
        if (node == null)
            return new int[]{0, 0};
        var left = dfs(node.left);
        var right = dfs(node.right);
        int coins = left[0] + right[0] + node.val; // 子树硬币个数
        int nodes = left[1] + right[1] + 1; // 子树节点数
        ans += Math.abs(coins - nodes);
        return new int[]{coins, nodes};
    }
}
```

```cpp [sol-C++]
class Solution {
    int ans = 0;

    pair<int, int> dfs(TreeNode *node) {
        if (node == nullptr)
            return {0, 0};
        auto [coins_l, nodes_l] = dfs(node->left);
        auto [coins_r, nodes_r] = dfs(node->right);
        int coins = coins_l + coins_r + node->val; // 子树硬币个数
        int nodes = nodes_l + nodes_r + 1; // 子树节点数
        ans += abs(coins - nodes);
        return {coins, nodes};
    }

public:
    int distributeCoins(TreeNode *root) {
        dfs(root);
        return ans;
    }
};
```

```go [sol-Go]
func distributeCoins(root *TreeNode) (ans int) {
    var dfs func(*TreeNode) (int, int)
    dfs = func(node *TreeNode) (int, int) {
        if node == nil {
            return 0, 0
        }
        coinsL, nodesL := dfs(node.Left)
        coinsR, nodesR := dfs(node.Right)
        coins := coinsL + coinsR + node.Val // 子树硬币个数
        nodes := nodesL + nodesR + 1        // 子树节点数
        ans += abs(coins - nodes)
        return coins, nodes
    }
    dfs(root)
    return
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

```js [sol-JavaScript]
var distributeCoins = function (root) {
    let ans = 0;
    function dfs(node) {
        if (node === null)
            return [0, 0];
        const [coinsL, nodesL] = dfs(node.left);
        const [coinsR, nodesR] = dfs(node.right);
        const coins = coinsL + coinsR + node.val; // 子树硬币个数
        const nodes = nodesL + nodesR + 1; // 子树节点数
        ans += Math.abs(coins - nodes);
        return [coins, nodes];
    }
    dfs(root);
    return ans;
};
```

## 算法二（优化）

```py [sol-Python3]
class Solution:
    def distributeCoins(self, root: Optional[TreeNode]) -> int:
        ans = 0
        def dfs(node: Optional[TreeNode]) -> int:
            if node is None:
                return 0
            d = dfs(node.left) + dfs(node.right) + node.val - 1
            nonlocal ans
            ans += abs(d)
            return d
        dfs(root)
        return ans
```

```java [sol-Java]
class Solution {
    private int ans;

    public int distributeCoins(TreeNode root) {
        dfs(root);
        return ans;
    }

    private int dfs(TreeNode node) {
        if (node == null)
            return 0;
        int d = dfs(node.left) + dfs(node.right) + node.val - 1;
        ans += Math.abs(d);
        return d;
    }
}
```

```cpp [sol-C++]
class Solution {
    int ans = 0;

    int dfs(TreeNode *node) {
        if (node == nullptr)
            return 0;
        int d = dfs(node->left) + dfs(node->right) + node->val - 1;
        ans += abs(d);
        return d;
    }

public:
    int distributeCoins(TreeNode *root) {
        dfs(root);
        return ans;
    }
};
```

```go [sol-Go]
func distributeCoins(root *TreeNode) (ans int) {
    var dfs func(*TreeNode) int
    dfs = func(node *TreeNode) int {
        if node == nil {
            return 0
        }
        d := dfs(node.Left) + dfs(node.Right) + node.Val - 1
        ans += abs(d)
        return d
    }
    dfs(root)
    return
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

```js [sol-JavaScript]
var distributeCoins = function (root) {
    let ans = 0;
    function dfs(node) {
        if (node === null)
            return 0;
        const d = dfs(node.left) + dfs(node.right) + node.val - 1;
        ans += Math.abs(d);
        return d;
    }
    dfs(root);
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为二叉树的节点个数。每个节点都会递归恰好一次。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，二叉树是一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

## 思考题

1. 如果硬币总个数 $\le n$ 个，要求最终每个节点**至多**有一枚硬币，最少要移动多少次呢？
2. 如果硬币总个数 $\ge n$ 个，要求最终每个节点**至少**有一枚硬币，最少要移动多少次呢？
3. 假设每个节点 $\textit{node}$ 都多了一个属性值 $\textit{target}$，要求最终**至少**有 $\textit{node}.\textit{target}$ 枚硬币，最少要移动多少次呢？（保证总硬币个数不低于所有节点的 $\textit{target}$ 之和。）

欢迎在评论区分享你的思路。

## 专题训练

见下面贪心与思维题单的「**§5.5 贡献法**」。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.5 贡献法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.5 贡献法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
