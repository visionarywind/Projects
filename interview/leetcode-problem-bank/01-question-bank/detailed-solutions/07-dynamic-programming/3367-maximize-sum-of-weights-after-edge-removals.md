# 3367. 移除边之后的权重最大和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximize-sum-of-weights-after-edge-removals/
- 题目 slug：`maximize-sum-of-weights-after-edge-removals`
- 来源专题：动态规划
- 来源分类路径：十二、树形 DP / §12.5 其他树形 DP
- 难度分：2602
- 外部题解来源：https://leetcode.cn/problems/maximize-sum-of-weights-after-edge-removals/solutions/2998845/shu-xing-dp-tan-xin-pythonjavacgo-by-end-i3g3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[树形 DP + 贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/maximize-sum-of-weights-after-edge-removals/solutions/2998845/shu-xing-dp-tan-xin-pythonjavacgo-by-end-i3g3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-xing-dp-tan-xin-pythonjavacgo-by-end-i3g3`
- topic id：`2998845`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

从特殊到一般。想一想，如果这棵树是一条链，且 $k=1$，要怎么选？

由于不能同时选两条相邻的边，所以问题变成：

- 给你一个长为 $n-1$ 的 $w$ 数组，你需要从中选择若干元素，且不能选相邻的元素。你选的元素之和的最大值是多少？

这就是 [198. 打家劫舍](https://leetcode.cn/problems/house-robber/)。

> 既然特殊情况都只能用 DP 解决，那就全力往 DP 思考吧。

本题是树，考虑节点 $x$ 和它的儿子 $y$ 的这条边（$x\text{-}y$）**选或不选**：

- 不选：那么在节点 $y$ 及其儿子的边中，至多选 $k$ 条边。
- 选：那么在节点 $y$ 及其儿子的边中，至多选 $k-1$ 条边。

假设节点 $x$ 有三个儿子，不选和选计算出的结果分别记作 $(\textit{nc}_1, c_1),(\textit{nc}_2, c_2),(\textit{nc}_3, c_3)$。

假设要从中选两条边，选哪两条边最优呢？

- 先考虑都不选，也就是 $\textit{nc}_1+\textit{nc}_2+\textit{nc}_3$。
- 然后把其中两个 $\textit{nc}_i$ 替换成 $c_i$，那么选「增量」最大的两个 $c_i - \textit{nc}_i$。

所以本题不仅是 DP，还是贪心。我们需要把 $c_i - \textit{nc}_i$ 保存到一个数组 $\textit{inc}$ 中（非正数不需要保存），然后把数组从大到小排序，取最大的 $k$ 个或者 $k-1$ 个。

**优化**：如果不删除边也满足要求，即所有点的度数都 $\le k$，则直接返回所有边权之和。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1fFB4YGEZY/?t=26m45s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maximizeSumOfWeights(self, edges: List[List[int]], k: int) -> int:
        g = [[] for _ in range(len(edges) + 1)]
        for x, y, wt in edges:
            g[x].append((y, wt))
            g[y].append((x, wt))

        # 优化
        if all(len(to) <= k for to in g):
            return sum(e[2] for e in edges)

        def dfs(x: int, fa: int) -> Tuple[int, int]:
            not_choose = 0
            inc = []
            for y, wt in g[x]:
                if y == fa:
                    continue
                nc, c = dfs(y, x)
                not_choose += nc  # 先都不选
                if (d := c + wt - nc) > 0:
                    inc.append(d)
            inc.sort(reverse=True)
            # 再选增量最大的 k 个或者 k-1 个
            return not_choose + sum(inc[:k]), not_choose + sum(inc[:k - 1])
        return dfs(0, -1)[0]  # not_choose >= choose
```

```java [sol-Java]
class Solution {
    public long maximizeSumOfWeights(int[][] edges, int k) {
        List<int[]>[] g = new ArrayList[edges.length + 1];
        Arrays.setAll(g, i -> new ArrayList<>());
        long sumWt = 0;
        for (int[] e : edges) {
            int x = e[0], y = e[1], wt = e[2];
            g[x].add(new int[]{y, wt});
            g[y].add(new int[]{x, wt});
            sumWt += wt;
        }

        // 优化
        boolean simple = true;
        for (List<int[]> to : g) {
            if (to.size() > k) {
                simple = false;
                break;
            }
        }
        if (simple) {
            return sumWt;
        }

        return dfs(0, -1, g, k)[0]; // notChoose >= choose
    }

    private long[] dfs(int x, int fa, List<int[]>[] g, int k) {
        long notChoose = 0;
        List<Integer> inc = new ArrayList<>();
        for (int[] e : g[x]) {
            int y = e[0];
            if (y == fa) {
                continue;
            }
            long[] res = dfs(y, x, g, k);
            notChoose += res[0]; // 先都不选
            int d = (int) (res[1] - res[0]) + e[1];
            if (d > 0) {
                inc.add(d);
            }
        }

        // 再选增量最大的 k 个或者 k-1 个
        inc.sort(Collections.reverseOrder());
        for (int i = 0; i < Math.min(inc.size(), k - 1); i++) {
            notChoose += inc.get(i);
        }
        long choose = notChoose;
        if (inc.size() >= k) {
            notChoose += inc.get(k - 1);
        }
        return new long[]{notChoose, choose};
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maximizeSumOfWeights(vector<vector<int>>& edges, int k) {
        vector<vector<pair<int, int>>> g(edges.size() + 1);
        long long sum_wt = 0;
        for (auto& e : edges) {
            int x = e[0], y = e[1], wt = e[2];
            g[x].emplace_back(y, wt);
            g[y].emplace_back(x, wt);
            sum_wt += wt;
        }

        // 优化
        bool simple = true;
        for (auto& to : g) {
            if (to.size() > k) {
                simple = false;
                break;
            }
        }
        if (simple) {
            return sum_wt;
        }

        auto dfs = [&](auto& dfs, int x, int fa) -> pair<long long, long long> {
            long long not_choose = 0;
            vector<int> inc;
            for (auto& [y, wt] : g[x]) {
                if (y == fa) {
                    continue;
                }
                auto [nc, c] = dfs(dfs, y, x);
                not_choose += nc; // 先都不选
                int d = c + wt - nc;
                if (d > 0) {
                    inc.push_back(d);
                }
            }

            // 再选增量最大的 k 个或者 k-1 个
            ranges::sort(inc, greater()); // 从大到小排序
            for (int i = 0; i < min((int) inc.size(), k - 1); i++) {
                not_choose += inc[i];
            }
            long long choose = not_choose;
            if (inc.size() >= k) {
                not_choose += inc[k - 1];
            }
            return {not_choose, choose};
        };
        return dfs(dfs, 0, -1).first; // not_choose >= choose
    }
};
```

```go [sol-Go]
func maximizeSumOfWeights(edges [][]int, k int) int64 {
	type edge struct{ to, wt int }
	g := make([][]edge, len(edges)+1)
	sumWt := 0
	for _, e := range edges {
		x, y, wt := e[0], e[1], e[2]
		g[x] = append(g[x], edge{y, wt})
		g[y] = append(g[y], edge{x, wt})
		sumWt += wt
	}

	// 优化
	simple := true
	for _, to := range g {
		if len(to) > k {
			simple = false
			break
		}
	}
	if simple {
		return int64(sumWt)
	}

	var dfs func(int, int) (int, int)
	dfs = func(x, fa int) (int, int) {
		notChoose := 0
		inc := []int{}
		for _, e := range g[x] {
			y := e.to
			if y == fa {
				continue
			}
			nc, c := dfs(y, x)
			notChoose += nc // 先都不选
			if d := c + e.wt - nc; d > 0 {
				inc = append(inc, d)
			}
		}

		// 再选增量最大的 k 个或者 k-1 个
		slices.SortFunc(inc, func(a, b int) int { return b - a })
		for i := range min(len(inc), k-1) {
			notChoose += inc[i]
		}
		choose := notChoose
		if len(inc) >= k {
			notChoose += inc[k-1]
		}
		return notChoose, choose
	}
	nc, _ := dfs(0, -1) // notChoose >= choose
	return int64(nc)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。瓶颈在排序上。如果用快速选择，可以做到 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

贪心的那部分和 [2611. 老鼠和奶酪](https://leetcode.cn/problems/mice-and-cheese/) 是一模一样的。

更多相似题目，见下面动态规划题单中的「**十二、树形 DP**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. 【本题相关】[动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. 【本题相关】[贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

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
