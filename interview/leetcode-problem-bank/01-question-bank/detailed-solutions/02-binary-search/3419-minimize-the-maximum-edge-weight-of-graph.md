# 3419. 图的最大边权的最小值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimize-the-maximum-edge-weight-of-graph/
- 题目 slug：`minimize-the-maximum-edge-weight-of-graph`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.4 最小化最大值
- 难度分：2243
- 外部题解来源：https://leetcode.cn/problems/minimize-the-maximum-edge-weight-of-graph/solutions/3045060/liang-chong-fang-fa-er-fen-da-an-dijkstr-eb7d/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：二分答案/Dijkstra（Python/Java/C++/Go）](https://leetcode.cn/problems/minimize-the-maximum-edge-weight-of-graph/solutions/3045060/liang-chong-fang-fa-er-fen-da-an-dijkstr-eb7d/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-er-fen-da-an-dijkstr-eb7d`
- topic id：`3045060`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 等价转换

把图中的每条边**反向**，那么原问题等价于删边之后：

- 从 $0$ 出发，必须能访问到所有点。
- 每个点的**入度**至多为 $\textit{threshold}$。

在这种情况下，由于从 $0$ 出发，在不访问重复节点的情况下，DFS 过程是一棵树（叫做 DFS 树），每个节点都只有一个父节点（除了根节点 $0$ 没有父节点）。所以一定存在一种删边方案，使得每个点的入度恰好为 $1$（除了根节点 $0$ 没有入度），因此我们一定能满足 $\textit{threshold}$ 的要求。

所以 $\textit{threshold}$ 是**多余的**。

> 注意题目保证 $\textit{threshold}\ge 1$。

## 方法一：二分答案 + DFS

设最大边权为 $\textit{upper}$。

由于 $\textit{upper}$ 越大，越能够从 $0$ 出发，访问到所有点。据此，可以**二分猜答案**。

于是问题变成：

- 从 $0$ 出发，只经过边权 $\le \textit{upper}$ 的边，能否访问到所有点。

DFS 即可，过程中统计访问到的节点个数（或者剩余未访问的节点个数）。

关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

### 细节

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的。

- 开区间左端点初始值：$0$。一定无法访问其他节点。
- 开区间右端点初始值：所有边权的最大值 $+1$。如果最终答案为所有边权的最大值 $+1$，那么返回 $-1$。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1HKcue9ETm/?t=14m58s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minMaxWeight(self, n: int, edges: List[List[int]], _: int) -> int:
        if len(edges) < n - 1:
            return -1

        g = [[] for _ in range(n)]
        for x, y, w in edges:
            g[y].append((x, w))

        vis = [0] * n
        def check(upper: int) -> bool:
            def dfs(x: int) -> int:
                vis[x] = upper  # 避免每次二分重新创建 vis 数组
                cnt = 1
                for y, w in g[x]:
                    if w <= upper and vis[y] != upper:
                        cnt += dfs(y)
                return cnt
            return dfs(0) == n

        max_w = max(e[2] for e in edges)
        ans = bisect_left(range(max_w + 1), True, 1, key=check)
        return -1 if ans > max_w else ans
```

```java [sol-Java]
class Solution {
    public int minMaxWeight(int n, int[][] edges, int threshold) {
        if (edges.length < n - 1) {
            return -1;
        }

        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        int maxW = 0;
        for (int[] e : edges) {
            int x = e[0], y = e[1], w = e[2];
            g[y].add(new int[]{x, w});
            maxW = Math.max(maxW, w);
        }

        int[] vis = new int[n];
        int left = 0;
        int right = maxW + 1;
        while (left + 1 < right) {
            int mid = (left + right) >>> 1;
            if (dfs(0, mid, vis, g) == n) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right > maxW ? -1 : right;
    }

    private int dfs(int x, int upper, int[] vis, List<int[]>[] g) {
        vis[x] = upper; // 避免每次二分重新创建 vis 数组
        int cnt = 1;
        for (int[] e : g[x]) {
            int y = e[0];
            if (e[1] <= upper && vis[y] != upper) {
                cnt += dfs(y, upper, vis, g);
            }
        }
        return cnt;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minMaxWeight(int n, vector<vector<int>>& edges, int) {
        if (edges.size() < n - 1) {
            return -1;
        }

        vector<vector<pair<int, int>>> g(n);
        int max_w = 0;
        for (auto& e : edges) {
            int x = e[0], y = e[1], w = e[2];
            g[y].emplace_back(x, w);
            max_w = max(max_w, w);
        }

        vector<int> vis(n);
        auto check = [&](int upper) -> bool {
            int left = n;
            auto dfs = [&](this auto&& dfs, int x) -> void {
                vis[x] = upper; // 避免每次二分重新创建 vis 数组
                left--;
                for (auto& [y, w] : g[x]) {
                    if (w <= upper && vis[y] != upper) {
                        dfs(y);
                    }
                }
            };
            dfs(0);
            return left == 0;
        };

        int left = 0, right = max_w + 1;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right > max_w ? -1 : right;
    }
};
```

```go [sol-Go]
func minMaxWeight(n int, edges [][]int, _ int) int {
	if len(edges) < n-1 {
		return -1
	}

	type edge struct{ to, w int }
	g := make([][]edge, n)
	maxW := 0
	for _, e := range edges {
		x, y, w := e[0], e[1], e[2]
		g[y] = append(g[y], edge{x, w})
		maxW = max(maxW, w)
	}

	vis := make([]int, n)
	ans := 1 + sort.Search(maxW, func(upper int) bool {
		upper++
		left := n
		var dfs func(int)
		dfs = func(x int) {
			vis[x] = upper // 避免每次二分重新创建 vis 数组
			left--
			for _, e := range g[x] {
				if e.w <= upper && vis[e.to] != upper {
					dfs(e.to)
				}
			}
		}
		dfs(0)
		return left == 0
	})
	if ans > maxW {
		return -1
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log U)$，其中 $m$ 是 $\textit{edges}$ 的长度，$U$ 是所有边权的最大值。
- 空间复杂度：$\mathcal{O}(m)$。

## 方法二：Dijkstra

**前置知识**：[Dijkstra 算法介绍](https://leetcode.cn/problems/network-delay-time/solution/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)。

也可以从 $0$ 出发，每次走当前能访问到的边中，边权最小的边，这类似 Dijkstra 求最短路。

本题是计算路径边权最大值，把 Dijkstra 算法中的 $+$ 改成 $\max$ 即可。

最终答案为 $\max(\textit{dis})$。

### 答疑

问：能否只用 DFS 做？每次优先访问边权小的边。

答：有反例，具体见 [视频讲解](https://www.bilibili.com/video/BV1HKcue9ETm/?t=23m53s)。

```py [sol-Python3]
class Solution:
    def minMaxWeight(self, n: int, edges: List[List[int]], _: int) -> int:
        if len(edges) < n - 1:
            return -1

        g = [[] for _ in range(n)]
        for x, y, w in edges:
            g[y].append((x, w))

        dis = [inf] * n
        dis[0] = 0
        h = [(0, 0)]  # (路径最大边权, 节点编号)
        while h:
            d, x = heappop(h)
            if d > dis[x]:
                continue
            for y, w in g[x]:
                new_d = max(d, w)
                if new_d < dis[y]:
                    dis[y] = new_d
                    heappush(h, (new_d, y))

        ans = max(dis)
        return -1 if ans == inf else ans
```

```java [sol-Java]
class Solution {
    public int minMaxWeight(int n, int[][] edges, int threshold) {
        if (edges.length < n - 1) {
            return -1;
        }

        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0], y = e[1], w = e[2];
            g[y].add(new int[]{x, w});
        }

        int[] dis = new int[n];
        Arrays.fill(dis, Integer.MAX_VALUE);
        dis[0] = 0;
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> a[0] - b[0]);
        pq.offer(new int[]{0, 0}); // (路径最大边权, 节点编号)
        int ans = 0;
        while (!pq.isEmpty()) {
            int[] p = pq.poll();
            int d = p[0], x = p[1];
            if (d > dis[x]) {
                continue;
            }
            ans = d;
            n--;
            for (int[] e : g[x]) {
                int y = e[0];
                int newD = Math.max(d, e[1]);
                if (newD < dis[y]) {
                    dis[y] = newD;
                    pq.offer(new int[]{newD, y});
                }
            }
        }

        return n > 0 ? -1 : ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minMaxWeight(int n, vector<vector<int>>& edges, int) {
        if (edges.size() < n - 1) {
            return -1;
        }

        vector<vector<pair<int, int>>> g(n);
        for (auto& e : edges) {
            int x = e[0], y = e[1], w = e[2];
            g[y].emplace_back(x, w);
        }

        vector<int> dis(n, INT_MAX);
        dis[0] = 0;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        pq.emplace(0, 0); // (路径最大边权, 节点编号)
        while (!pq.empty()) {
            auto [d, x] = pq.top();
            pq.pop();
            if (d > dis[x]) {
                continue;
            }
            for (auto& [y, w] : g[x]) {
                int new_d = max(d, w);
                if (new_d < dis[y]) {
                    dis[y] = new_d;
                    pq.emplace(new_d, y);
                }
            }
        }

        int ans = ranges::max(dis);
        return ans == INT_MAX ? -1 : ans;
    }
};
```

```go [sol-Go]
func minMaxWeight(n int, edges [][]int, _ int) int {
	if len(edges) < n-1 {
		return -1
	}

	type edge struct{ to, w int }
	g := make([][]edge, n)
	for _, e := range edges {
		x, y, w := e[0], e[1], e[2]
		g[y] = append(g[y], edge{x, w})
	}

	dis := make([]int, n)
	for i := range dis {
		dis[i] = math.MaxInt
	}
	dis[0] = 0
	h := hp{{}}
	for len(h) > 0 {
		p := heap.Pop(&h).(pair)
		x := p.x
		d := p.dis
		if d > dis[x] {
			continue
		}
		for _, e := range g[x] {
			y := e.to
			newD := max(d, e.w)
			if newD < dis[y] {
				dis[y] = newD
				heap.Push(&h, pair{newD, y})
			}
		}
	}

	ans := slices.Max(dis)
	if ans == math.MaxInt {
		return -1
	}
	return ans
}

type pair struct{ dis, x int } // 路径最大边权, 节点编号
type hp []pair
func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].dis < h[j].dis }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(pair)) }
func (h *hp) Pop() (v any)      { a := *h; *h, v = a[:len(a)-1], a[len(a)-1]; return }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log m)$，其中 $m$ 是 $\textit{edges}$ 的长度。
- 空间复杂度：$\mathcal{O}(m)$。

## 思考题

把题目中的「出去的边」改成「进来的边」，要怎么做？

注意当 $\textit{threshold}=1$ 的时候，这等价于找一条哈密顿路径，是 NP 完全问题。可以用状压 DP 解决，但必须限制数据范围 $n\le 20$。

## 相似题目

- [1631. 最小体力消耗路径](https://leetcode.cn/problems/path-with-minimum-effort/) 1948
- [778. 水位上升的泳池中游泳](https://leetcode.cn/problems/swim-in-rising-water/) 2097

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二分答案 / §2.4 最小化最大值`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.4 最小化最大值`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
