# 2538. 最大价值和与最小价值和的差值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/difference-between-maximum-and-minimum-price-sum/
- 题目 slug：`difference-between-maximum-and-minimum-price-sum`
- 来源专题：动态规划
- 来源分类路径：十二、树形 DP / §12.1 树的直径
- 难度分：2398
- 外部题解来源：https://leetcode.cn/problems/difference-between-maximum-and-minimum-price-sum/solutions/2062782/by-endlesscheng-5l70/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[树形 DP：转换成树上最大路径和（Python/Java/C++/Go）](https://leetcode.cn/problems/difference-between-maximum-and-minimum-price-sum/solutions/2062782/by-endlesscheng-5l70/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-5l70`
- topic id：`2062782`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 提示 1

由于价值都是正数，因此价值和最小的一条路径一定**只有一个点**。

## 提示 2

根据提示 1，「价值和最大的一条路径与最小的一条路径的差值」等价于「去掉路径的一个端点」，因为这两条路径都是从一个点出发的。

## 提示 3

由于价值都是正数，一条路径能延长就尽量延长，这样路径和就越大，那么最优是延长到叶子。

根据提示 2，问题转换成**去掉一个叶子**后的**最大路径和**。

> 这里的叶子严格来说是度为 $1$ 的点，因为根的度数也可能是 $1$。

## 提示 4

最大路径和是一个经典树形 DP 问题，类似 [树的直径](https://www.bilibili.com/video/BV17o4y187h1/)。

设 $y$ 是 $x$ 的一个儿子，想象有一条路径在 $x$ 拐弯，那么这条路径可以看成是由「从 $x$ 出发往下的路径」和「从 $y$ 出发往下的路径」拼接而成（这两条路径不重叠）。

对于当前节点 $x$，假设它有多棵子树，我们一棵棵 DFS，同时维护「从 $x$ 出发往下的最大带叶子的路径和」和「从 $x$ 出发往下的最大不带叶子的路径和」。

假设当前 DFS 完了 $x$ 的其中一棵子树 $y$，它返回了「从 $y$ 出发往下的最大带叶子的路径和」和「从 $y$ 出发往下的最大不带叶子的路径和」，那么答案可能是：

- 从 $x$ 出发往下的最大带叶子的路径和 + 从 $y$ 出发往下的最大不带叶子的路径和。
- 从 $x$ 出发往下的最大不带叶子的路径和 + 从 $y$ 出发往下的最大带叶子的路径和。

然后更新：

- 从 $y$ 出发往下的最大带叶子的路径和，加上 $\textit{price}[x]$，去更新从 $x$ 出发往下的最大带叶子的路径和。
- 从 $y$ 出发往下的最大不带叶子的路径和，加上 $\textit{price}[x]$，去更新从 $x$ 出发往下的最大不带叶子的路径和。

最后返回「从 $x$ 出发往下的最大带叶子的路径和」和「从 $x$ 出发往下的最大不带叶子的路径和」，提供给 $x$ 的父节点计算。

附：[本题视频讲解](https://www.bilibili.com/video/BV1QT41127kJ/)。

```py [sol1-Python3]
class Solution:
    def maxOutput(self, n: int, edges: List[List[int]], price: List[int]) -> int:
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)  # 建树

        ans = 0
        def dfs(x: int, fa: int) -> (int, int):
            nonlocal ans
            max_s1 = p = price[x]
            max_s2 = 0
            for y in g[x]:
                if y == fa: continue
                s1, s2 = dfs(y, x)
                # 前面最大带叶子的路径和 + 当前不带叶子的路径和
                # 前面最大不带叶子的路径和 + 当前带叶子的路径和
                ans = max(ans, max_s1 + s2, max_s2 + s1)
                max_s1 = max(max_s1, s1 + p)
                max_s2 = max(max_s2, s2 + p)  # 这里加上 p 是因为 x 必然不是叶子
            return max_s1, max_s2
        dfs(0, -1)
        return ans
```

```java [sol1-Java]
class Solution {
    private List<Integer>[] g;
    private int[] price;
    private long ans;

    public long maxOutput(int n, int[][] edges, int[] price) {
        this.price = price;
        g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (var e : edges) {
            int x = e[0], y = e[1];
            g[x].add(y);
            g[y].add(x); // 建树
        }
        dfs(0, -1);
        return ans;
    }

    private long[] dfs(int x, int fa) {
        long p = price[x], maxS1 = p, maxS2 = 0;
        for (var y : g[x])
            if (y != fa) {
                var res = dfs(y, x);
                long s1 = res[0], s2 = res[1];
                // 前面最大带叶子的路径和 + 当前不带叶子的路径和
                // 前面最大不带叶子的路径和 + 当前带叶子的路径和
                ans = Math.max(ans, Math.max(maxS1 + s2, maxS2 + s1));
                maxS1 = Math.max(maxS1, s1 + p);
                maxS2 = Math.max(maxS2, s2 + p); // 这里加上 p 是因为 x 必然不是叶子
            }
        return new long[]{maxS1, maxS2};
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    long long maxOutput(int n, vector<vector<int>> &edges, vector<int> &price) {
        vector<vector<int>> g(n);
        for (auto &e : edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x); // 建树
        }

        long ans = 0;
        function<pair<long, long>(int, int)> dfs = [&](int x, int fa) -> pair<long, long> {
            long p = price[x], max_s1 = p, max_s2 = 0;
            for (int y : g[x])
                if (y != fa) {
                    auto[s1, s2] = dfs(y, x);
                    // 前面最大带叶子的路径和 + 当前不带叶子的路径和
                    // 前面最大不带叶子的路径和 + 当前带叶子的路径和
                    ans = max(ans, max(max_s1 + s2, max_s2 + s1));
                    max_s1 = max(max_s1, s1 + p);
                    max_s2 = max(max_s2, s2 + p); // 这里加上 p 是因为 x 必然不是叶子
                }
            return {max_s1, max_s2};
        };
        dfs(0, -1);
        return ans;
    }
};
```

```go [sol1-Go]
func maxOutput(n int, edges [][]int, price []int) int64 {
	ans := 0
	g := make([][]int, n)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x) // 建树
	}
	var dfs func(int, int) (int, int)
	dfs = func(x, fa int) (int, int) {
		p := price[x]
		maxS1, maxS2 := p, 0
		for _, y := range g[x] {
			if y != fa {
				s1, s2 := dfs(y, x)
				// 前面最大带叶子的路径和 + 当前不带叶子的路径和
				// 前面最大不带叶子的路径和 + 当前带叶子的路径和
				ans = max(ans, max(maxS1+s2, maxS2+s1))
				maxS1 = max(maxS1, s1+p)
				maxS2 = max(maxS2, s2+p) // 这里加上 p 是因为 x 必然不是叶子
			}
		}
		return maxS1, maxS2
	}
	dfs(0, -1)
	return int64(ans)
}

func max(a, b int) int { if b > a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$O(n)$。
- 空间复杂度：$O(n)$。

## 相似题目

- [543. 二叉树的直径](https://leetcode.cn/problems/diameter-of-binary-tree/)
- [124. 二叉树中的最大路径和](https://leetcode.cn/problems/binary-tree-maximum-path-sum/)
- [1245. 树的直径](https://leetcode.cn/problems/tree-diameter/)（会员题）
- [2246. 相邻字符不同的最长路径](https://leetcode.cn/problems/longest-path-with-different-adjacent-characters/)
- [687. 最长同值路径](https://leetcode.cn/problems/longest-univalue-path/)
- [1617. 统计子树中城市之间最大距离](https://leetcode.cn/problems/count-subtrees-with-max-distance-between-cities/)

## 本地原创解析

### 1. 题意重述

本题来自 `十二、树形 DP / §12.1 树的直径`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十二、树形 DP / §12.1 树的直径`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
