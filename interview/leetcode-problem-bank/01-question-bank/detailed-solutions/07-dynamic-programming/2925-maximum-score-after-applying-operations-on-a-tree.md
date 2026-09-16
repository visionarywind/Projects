# 2925. 在树上执行操作以后得到的最大分数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-score-after-applying-operations-on-a-tree/
- 题目 slug：`maximum-score-after-applying-operations-on-a-tree`
- 来源专题：动态规划
- 来源分类路径：十二、树形 DP / §12.5 其他树形 DP
- 难度分：1940
- 外部题解来源：https://leetcode.cn/problems/maximum-score-after-applying-operations-on-a-tree/solutions/2513101/shu-xing-dpxuan-huo-bu-xuan-pythonjavacg-7aj6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[树形 DP：选或不选（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-score-after-applying-operations-on-a-tree/solutions/2513101/shu-xing-dpxuan-huo-bu-xuan-pythonjavacg-7aj6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-xing-dpxuan-huo-bu-xuan-pythonjavacg-7aj6`
- topic id：`2513101`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

请看 [视频讲解](https://www.bilibili.com/video/BV1Fc411R7xA/) 第三题。

## 前置知识：树形 DP

请看视频讲解 [树形 DP【基础算法精讲 24】](https://www.bilibili.com/video/BV1vu4y1f7dn/)

## 思路

正难则反，先把所有 $\textit{values}[i]$ 加到答案中，然后考虑哪些 $\textit{values}[i]$ 不能选（撤销，不加入答案）。

设当前节点为 $x$，计算以 $x$ 为根的子树是健康时，失去的最小分数。那么答案就是 $\textit{values}$ 的元素和，减去「以 $0$ 为根的子树是健康时，**失去**的最小分数」。

用「**选或不选**」分类讨论：

- 第一种情况：失去 $\textit{values}[x]$，也就是不加入答案，那么 $x$ 的所有子孙节点都可以加入答案，失去的最小分数就是 $\textit{values}[x]$。
- 第二种情况：$\textit{values}[x]$ 加入答案，问题变成「以 $y$ 为根的子树是健康时，失去的最小分数」，这里 $y$ 是 $x$ 的儿子。如果有多个儿子，累加失去的最小分数。

这两种情况取最小值。注意第一种情况是不会往下递归的，所以当我们递归到叶子的时候，叶子一定不能加入答案，此时直接返回 $\textit{values}[x]$。

代码实现时，为了方便判断 $x$ 是否为叶子节点，可以假设还有一条 $0$ 到 $-1$ 的边，这样不会误把根节点 $0$ 当作叶子。 

```py [sol-Python3]
class Solution:
    def maximumScoreAfterOperations(self, edges: List[List[int]], values: List[int]) -> int:
        g = [[] for _ in values]
        g[0].append(-1)  # 避免误把根节点当作叶子
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        # dfs(x, fa) 计算以 x 为根的子树是健康时，失去的最小分数
        def dfs(x: int, fa: int) -> int:
            if len(g[x]) == 1:  # x 是叶子
                return values[x]
            loss = 0  # 第二种情况
            for y in g[x]:
                if y != fa:
                    loss += dfs(y, x)  # 计算以 y 为根的子树是健康时，失去的最小分数
            return min(values[x], loss)  # 两种情况取最小值
        return sum(values) - dfs(0, -1)
```

```java [sol-Java]
class Solution {
    public long maximumScoreAfterOperations(int[][] edges, int[] values) {
        List<Integer>[] g = new ArrayList[values.length];
        Arrays.setAll(g, e -> new ArrayList<>());
        g[0].add(-1); // 避免误把根节点当作叶子
        for (int[] e : edges) {
            int x = e[0], y = e[1];
            g[x].add(y);
            g[y].add(x);
        }

        // 先把所有分数加入答案
        long ans = 0;
        for (int v : values) {
            ans += v;
        }
        return ans - dfs(0, -1, g, values);
    }

    // dfs(x) 计算以 x 为根的子树是健康时，失去的最小分数
    private long dfs(int x, int fa, List<Integer>[] g, int[] values) {
        if (g[x].size() == 1) { // x 是叶子
            return values[x];
        }
        long loss = 0; // 第二种情况
        for (int y : g[x]) {
            if (y != fa) {
                loss += dfs(y, x, g, values); // 计算以 y 为根的子树是健康时，失去的最小分数
            }
        }
        return Math.min(values[x], loss); // 两种情况取最小值
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maximumScoreAfterOperations(vector<vector<int>> &edges, vector<int> &values) {
        vector<vector<int>> g(values.size());
        g[0].push_back(-1); // 避免误把根节点当作叶子
        for (auto &e: edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        // dfs(x, fa) 计算以 x 为根的子树是健康时，失去的最小分数
        function<long long(int, int)> dfs = [&](int x, int fa) -> long long {
            if (g[x].size() == 1) { // x 是叶子
                return values[x];
            }
            long long loss = 0; // 第二种情况
            for (int y: g[x]) {
                if (y != fa) {
                    loss += dfs(y, x); // 计算以 y 为根的子树是健康时，失去的最小分数
                }
            }
            return min((long long) values[x], loss); // 两种情况取最小值
        };
        return accumulate(values.begin(), values.end(), 0LL) - dfs(0, -1);
    }
};
```

```go [sol-Go]
func maximumScoreAfterOperations(edges [][]int, values []int) int64 {
	g := make([][]int, len(values))
	g[0] = append(g[0], -1) // 避免误把根节点当作叶子
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x)
	}

	total := 0
	// dfs(x, fa) 计算以 x 为根的子树是健康时，失去的最小分数
	var dfs func(int, int) int
	dfs = func(x, fa int) int {
		total += values[x]
		if len(g[x]) == 1 { // x 是叶子
			return values[x]
		}
		loss := 0 // 第二种情况
		for _, y := range g[x] {
			if y != fa {
				loss += dfs(y, x) // 计算以 y 为根的子树是健康时，失去的最小分数
			}
		}
		return min(values[x], loss) // 两种情况取最小值
	}
	return int64(total - dfs(0, -1))
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{values}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

#### 相似题目

- [337. 打家劫舍 III](https://leetcode.cn/problems/house-robber-iii/)

更多题目见【基础算法精讲】视频简介中的课后题。

## 本地原创解析

### 1. 题意重述

本题来自 `十二、树形 DP / §12.5 其他树形 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十二、树形 DP / §12.5 其他树形 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
