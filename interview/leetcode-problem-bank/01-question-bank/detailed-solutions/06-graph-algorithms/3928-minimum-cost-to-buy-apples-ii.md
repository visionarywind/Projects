# 3928. 购买苹果的最低成本 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-to-buy-apples-ii/
- 题目 slug：`minimum-cost-to-buy-apples-ii`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：2187
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-to-buy-apples-ii/solutions/3966171/pao-2n-ci-dijkstrapythonjavacgo-by-endle-6hgd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[跑 2n 次 Dijkstra + 优化（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-cost-to-buy-apples-ii/solutions/3966171/pao-2n-ci-dijkstrapythonjavacgo-by-endle-6hgd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pao-2n-ci-dijkstrapythonjavacgo-by-endle-6hgd`
- topic id：`3966171`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题 $n\le 1000$，可以暴力一点。

对于每个 $i$，枚举在商店 $j=0,1,2,\ldots,n-1$ 购买苹果。我们需要知道：

- 从 $i$ 到 $j$ 的边权为 $\textit{cost}$ 的最短路长度。
- 从 $j$ 到 $i$ 的边权为 $\textit{cost}\cdot \textit{tax}$ 的最短路长度。由于图是无向图，从 $j$ 到 $i$ 的最短路长度等于从 $i$ 到 $j$ 的最短路长度。

用 Dijkstra 算法求最短路长度。

**优化**：由于 $\textit{ans}[i] \le \textit{prices}[i]$，所以若算出的最短路长度 $\ge \textit{prices}[i]$，则不入堆。实现时，只需把代码中的 $\textit{dis}$ 元素初始化成 $\textit{prices}[i]$，而不是通常的 $\infty$。同理，边权 $\ge \max(\textit{prices})$ 的边也无需加到图中。

[本题视频讲解](https://www.bilibili.com/video/BV1Vb5L6zEgm/?t=16m55s)，欢迎点赞关注~

```py [sol-Python3]
def dijkstra(g: list[list[tuple[int, int]]], start: int, price: int) -> list[int]:
    dis = [price] * len(g)
    dis[start] = 0  # 起点到自己的距离是 0
    h = [(0, start)]  # 堆中保存 (起点到节点 x 的最短路长度，节点 x)

    while h:
        dis_x, x = heappop(h)
        if dis_x > dis[x]:  # x 之前出堆过
            continue
        for y, wt in g[x]:
            new_dis_y = dis_x + wt
            if new_dis_y < dis[y]:
                dis[y] = new_dis_y  # 更新 x 的邻居的最短路
                # 懒更新堆：只插入数据，不更新堆中数据
                # 相同节点可能有多个不同的 new_dis_y，除了最小的 new_dis_y，其余值都会触发上面的 continue
                heappush(h, (new_dis_y, y))

    return dis

class Solution:
    def minCost(self, n: int, prices: list[int], roads: list[list[int]]) -> list[int]:
        g1 = [[] for _ in range(n)]
        g2 = [[] for _ in range(n)]
        for x, y, cost, tax in roads:
            g1[x].append((y, cost))
            g1[y].append((x, cost))
            g2[x].append((y, cost * tax))
            g2[y].append((x, cost * tax))

        ans = [0] * n
        for i, price in enumerate(prices):
            dis1 = dijkstra(g1, i, price)
            dis2 = dijkstra(g2, i, price)
            ans[i] = min(p + d1 + d2 for p, d1, d2 in zip(prices, dis1, dis2))
        return ans
```

```java [sol-Java]
class Solution {
    public int[] minCost(int n, int[] prices, int[][] roads) {
        int maxPrices = 0;
        for (int p : prices) {
            maxPrices = Math.max(maxPrices, p);
        }

        List<int[]>[] g1 = new ArrayList[n];
        List<int[]>[] g2 = new ArrayList[n];
        Arrays.setAll(g1, _ -> new ArrayList<>());
        Arrays.setAll(g2, _ -> new ArrayList<>());
        for (int[] e : roads) {
            int x = e[0], y = e[1], cost = e[2], tax = e[3];
            if (cost < maxPrices) {
                g1[x].add(new int[]{y, cost});
                g1[y].add(new int[]{x, cost});
            }
            if ((long) cost * tax < maxPrices) {
                g2[x].add(new int[]{y, cost * tax});
                g2[y].add(new int[]{x, cost * tax});
            }
        }

        int[] ans = new int[n];
        for (int i = 0; i < n; i++) {
            int[] dis1 = dijkstra(g1, i, prices[i]);
            int[] dis2 = dijkstra(g2, i, prices[i]);
            int res = Integer.MAX_VALUE;
            for (int j = 0; j < n; j++) {
                if (dis1[j] + dis2[j] < res - prices[j]) { // 为避免加法溢出，把 prices[j] 移到右边
                    res = prices[j] + dis1[j] + dis2[j];
                }
            }
            ans[i] = res;
        }

        return ans;
    }

    private int[] dijkstra(List<int[]>[] g, int start, int price) {
        int[] dis = new int[g.length];
        Arrays.fill(dis, price);
        // 堆中保存 (起点到节点 x 的最短路长度，节点 x)
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> a[0] - b[0]);
        dis[start] = 0; // 起点到自己的距离是 0
        pq.offer(new int[]{0, start});

        while (!pq.isEmpty()) {
            int[] top = pq.poll();
            int disX = top[0];
            int x = top[1];
            if (disX > dis[x]) { // x 之前出堆过
                continue;
            }
            for (int[] e : g[x]) {
                int y = e[0];
                int newDisY = disX + e[1];
                if (newDisY < dis[y]) {
                    dis[y] = newDisY; // 更新 x 的邻居的最短路
                    // 懒更新堆：只插入数据，不更新堆中数据
                    // 相同节点可能有多个不同的 newDisY，除了最小的 newDisY，其余值都会触发上面的 continue
                    pq.offer(new int[]{newDisY, y});
                }
            }
        }

        return dis;
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<int> dijkstra(vector<vector<pair<int, int>>>& g, int start, int price) {
        vector<int> dis(g.size(), price);
        // 堆中保存 (起点到节点 x 的最短路长度，节点 x)
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        dis[start] = 0; // 起点到自己的距离是 0
        pq.emplace(0, start);

        while (!pq.empty()) {
            auto [dis_x, x] = pq.top();
            pq.pop();
            if (dis_x > dis[x]) { // x 之前出堆过
                continue;
            }
            for (auto& [y, wt] : g[x]) {
                auto new_dis_y = dis_x + wt;
                if (new_dis_y < dis[y]) {
                    dis[y] = new_dis_y; // 更新 x 的邻居的最短路
                    // 懒更新堆：只插入数据，不更新堆中数据
                    // 相同节点可能有多个不同的 new_dis_y，除了最小的 new_dis_y，其余值都会触发上面的 continue
                    pq.emplace(new_dis_y, y);
                }
            }
        }

        return dis;
    }

public:
    vector<int> minCost(int n, vector<int>& prices, vector<vector<int>>& roads) {
        int max_price = ranges::max(prices);

        vector<vector<pair<int, int>>> g1(n);
        vector<vector<pair<int, int>>> g2(n);
        for (auto& e : roads) {
            int x = e[0], y = e[1], cost = e[2], tax = e[3];
            if (cost < max_price) {
                g1[x].emplace_back(y, cost);
                g1[y].emplace_back(x, cost);
            }
            if (1LL * cost * tax < max_price) {
                g2[x].emplace_back(y, cost * tax);
                g2[y].emplace_back(x, cost * tax);
            }
        }

        vector<int> ans(n, INT_MAX);
        for (int i = 0; i < n; i++) {
            vector<int> dis1 = dijkstra(g1, i, prices[i]);
            vector<int> dis2 = dijkstra(g2, i, prices[i]);
            for (int j = 0; j < n; j++) {
                if (dis1[j] + dis2[j] < ans[i] - prices[j]) { // 为避免加法溢出，把 prices[j] 移到右边
                    ans[i] = prices[j] + dis1[j] + dis2[j];
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
type edge struct{ to, wt int }

func dijkstra(g [][]edge, start int, price int) []int {
	dis := make([]int, len(g))
	for i := range dis {
		dis[i] = price
	}
	dis[start] = 0
	h := hp{{0, start}}

	for len(h) > 0 {
		top := heap.Pop(&h).(pair)
		d, x := top.dis, top.x
		if d > dis[x] {
			continue
		}
		for _, e := range g[x] {
			y := e.to
			newD := d + e.wt
			if newD < dis[y] {
				dis[y] = newD
				heap.Push(&h, pair{newD, y})
			}
		}
	}

	return dis
}

func minCost(n int, prices []int, roads [][]int) []int {
	g1 := make([][]edge, n)
	g2 := make([][]edge, n)
	for _, e := range roads {
		x, y, cost, tax := e[0], e[1], e[2], e[3]
		g1[x] = append(g1[x], edge{y, cost})
		g1[y] = append(g1[y], edge{x, cost})
		g2[x] = append(g2[x], edge{y, cost * tax})
		g2[y] = append(g2[y], edge{x, cost * tax})
	}

	ans := make([]int, n)
	for i, price := range prices {
		dis1 := dijkstra(g1, i, price)
		dis2 := dijkstra(g2, i, price)
		res := math.MaxInt
		for j, p := range prices {
			res = min(res, p+dis1[j]+dis2[j])
		}
		ans[i] = res
	}
	return ans
}

type pair struct{ dis, x int }
type hp []pair

func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].dis < h[j].dis }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(pair)) }
func (h *hp) Pop() (v any)      { a := *h; *h, v = a[:len(a)-1], a[len(a)-1]; return }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n(n+m\log m))$，其中 $m$ 是 $\textit{roads}$ 的长度。在使用**懒更新堆**的情况下，跑一次 Dijkstra 算法的时间复杂度为 $\mathcal{O}(n + m\log m)$。
- 空间复杂度：$\mathcal{O}(n+m)$。

## 专题训练

见下面图论题单的「**§3.1 单源最短路：Dijkstra 算法**」。

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

本题来自 `三、最短路 / §3.1 单源最短路：Dijkstra 算法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、最短路 / §3.1 单源最短路：Dijkstra 算法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
