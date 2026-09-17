# 2973. 树中每个节点放置的金币数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-number-of-coins-to-place-in-tree-nodes/
- 题目 slug：`find-number-of-coins-to-place-in-tree-nodes`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.3 自底向上 DFS
- 难度分：2277
- 外部题解来源：https://leetcode.cn/problems/find-number-of-coins-to-place-in-tree-nodes/solutions/2577675/tan-xin-pythonjavacgo-by-endlesscheng-j58c/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/find-number-of-coins-to-place-in-tree-nodes/solutions/2577675/tan-xin-pythonjavacgo-by-endlesscheng-j58c/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-pythonjavacgo-by-endlesscheng-j58c`
- topic id：`2577675`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

[本题视频讲解](https://www.bilibili.com/video/BV1jg4y1y7PA/)

首先来看如下问题：

给你一个长度 $\ge 3$ 的数组，返回**三数之积**的最大值，如果最大值是负数，返回 $0$。

- 如果只考虑正数，显然最大的三个数的乘积是最大的。
- 如果把负数也考虑进来，那么最小的两个负数乘最大的正数也可能是最大值。

这两种情况再和 $0$ 取最大值，即为返回值。

由上述讨论可知，只需要知道 $5$ 个数，就能算出三数之积的最大值。每棵子树只需要返回它最小的 $2$ 个 $\textit{cost}$ 值和最大的 $3$ 个 $\textit{cost}$ 值就行，其余数字可以舍弃。

对于一棵子树，把它的所有儿子子树的返回值与当前节点的 $\textit{cost}$ 值排序后，按照三数之积的方法，就得到了当前节点的答案。

```py [sol-Python3]
class Solution:
    def placedCoins(self, edges: List[List[int]], cost: List[int]) -> List[int]:
        n = len(cost)
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        ans = [1] * n
        def dfs(x: int, fa: int) -> List[int]:
            a = [cost[x]]
            for y in g[x]:
                if y != fa:
                    a.extend(dfs(y, x))
            a.sort()
            m = len(a)
            if m >= 3:
                ans[x] = max(a[-3] * a[-2] * a[-1], a[0] * a[1] * a[-1], 0)
            if m > 5:
                a = a[:2] + a[-3:]
            return a
        dfs(0, -1)
        return ans
```

```java [sol-Java]
class Solution {
    public long[] placedCoins(int[][] edges, int[] cost) {
        int n = cost.length;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0], y = e[1];
            g[x].add(y);
            g[y].add(x);
        }

        long[] ans = new long[n];
        dfs(0, -1, cost, g, ans);
        return ans;
    }

    private List<Integer> dfs(int x, int fa, int[] cost, List<Integer>[] g, long[] ans) {
        List<Integer> a = new ArrayList<>();
        a.add(cost[x]);
        for (int y : g[x]) {
            if (y != fa) {
                a.addAll(dfs(y, x, cost, g, ans));
            }
        }
        Collections.sort(a);
        int m = a.size();
        if (m < 3) {
            ans[x] = 1;
        } else {
            ans[x] = Math.max((long) a.get(m - 3) * a.get(m - 2) * a.get(m - 1),
                    Math.max((long) a.get(0) * a.get(1) * a.get(m - 1), 0));
        }
        if (m > 5) {
            a = List.of(a.get(0), a.get(1), a.get(m - 3), a.get(m - 2), a.get(m - 1));
        }
        return a;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<long long> placedCoins(vector<vector<int>>& edges, vector<int>& cost) {
        int n = cost.size();
        vector<vector<int>> g(n);
        for (auto& e: edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        vector<long long> ans(n, 1);
        auto dfs = [&](auto&& dfs, int x, int fa) -> vector<int> {
            vector<int> a = {cost[x]};
            for (int y: g[x]) {
                if (y != fa) {
                    auto res = dfs(dfs, y, x);
                    a.insert(a.end(), res.begin(), res.end());
                }
            }
            ranges::sort(a);
            int m = a.size();
            if (m >= 3) {
                ans[x] = max(max((long long) a[m - 3] * a[m - 2] * a[m - 1], (long long) a[0] * a[1] * a[m - 1]), 0LL);
            }
            if (m > 5) {
                a = {a[0], a[1], a[m - 3], a[m - 2], a[m - 1]};
            }
            return a;
        };
        dfs(dfs, 0, -1);
        return ans;
    }
};
```

```go [sol-Go]
func placedCoins(edges [][]int, cost []int) []int64 {
	n := len(cost)
	g := make([][]int, n)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x)
	}

	ans := make([]int64, n)
	var dfs func(int, int) []int
	dfs = func(x, fa int) []int {
		a := []int{cost[x]}
		for _, y := range g[x] {
			if y != fa {
				a = append(a, dfs(y, x)...)
			}
		}

		slices.Sort(a)
		m := len(a)
		if m < 3 {
			ans[x] = 1
		} else {
			ans[x] = int64(max(a[m-3]*a[m-2]*a[m-1], a[0]*a[1]*a[m-1], 0))
		}
		if m > 5 {
			a = append(a[:2], a[m-3:]...)
		}
		return a
	}
	dfs(0, -1)
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。瓶颈在排序上，如果手动维护最小的两个数和最大的三个数可以做到 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `三、一般树 / §3.3 自底向上 DFS`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.3 自底向上 DFS`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
