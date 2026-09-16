# 3123. 最短路径中的边

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-edges-in-shortest-paths/
- 题目 slug：`find-edges-in-shortest-paths`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：2093
- 外部题解来源：https://leetcode.cn/problems/find-edges-in-shortest-paths/solutions/2749274/dijkstra-zui-duan-lu-dfsbfs-zhao-bian-py-yf48/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Dijkstra 最短路 + DFS/BFS 找边（Python/Java/C++/Go）](https://leetcode.cn/problems/find-edges-in-shortest-paths/solutions/2749274/dijkstra-zui-duan-lu-dfsbfs-zhao-bian-py-yf48/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dijkstra-zui-duan-lu-dfsbfs-zhao-bian-py-yf48`
- topic id：`2749274`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置知识**：[Dijkstra 算法介绍](https://leetcode.cn/problems/network-delay-time/solution/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)

首先用 Dijkstra 算法（堆优化版本）计算出起点 $0$ 到所有节点的最短路长度 $\textit{dis}$。

如果 $\textit{dis}[n-1]=\infty$，说明无法从起点 $0$ 到终点 $n-1$，答案全为 $\textit{false}$。

否则，我们可以从终点 $n-1$ 出发，倒着 DFS 或 BFS，设当前在点 $y$，邻居为 $x$，边权为 $w$，如果满足

$$
\textit{dis}[x] + w = \textit{dis}[y]
$$

则说明 $x\textit{-}y$ 这条边在从 $0$ 到 $n-1$ 的最短路上。

### 答疑

**问**：为什么在求出最短路后，不能从起点 $0$ 出发去寻找在最短路上的边？

**答**：从起点 $0$ 出发，当发现 $\textit{dis}[x] + w = \textit{dis}[y]$ 时，这仅仅意味着 $x\textit{-}y$ 这条边在从 $0$ 出发的最短路上，但这条最短路不一定通向终点 $n-1$（比如图是一棵树，这条边不通往 $n-1$）。而从终点 $n-1$ 出发倒着寻找，就能保证符合等式的边在通向终点的最短路上。

请看 [视频讲解](https://www.bilibili.com/video/BV1gu4m1F7B8/) 第四题，欢迎点赞关注！

## 写法一：Dijkstra 算法 + DFS

```py [sol-Python3]
class Solution:
    def findAnswer(self, n: int, edges: List[List[int]]) -> List[bool]:
        g = [[] for _ in range(n)]
        for i, (x, y, w) in enumerate(edges):
            g[x].append((y, w, i))
            g[y].append((x, w, i))

        # Dijkstra 算法模板
        dis = [inf] * n
        dis[0] = 0
        h = [(0, 0)]
        while h:
            dx, x = heappop(h)
            if dx > dis[x]:
                continue
            for y, w, _ in g[x]:
                new_dis = dx + w
                if new_dis < dis[y]:
                    dis[y] = new_dis
                    heappush(h, (new_dis, y))

        ans = [False] * len(edges)
        # 图不连通
        if dis[-1] == inf:
            return ans

        # 从终点出发 DFS
        vis = [False] * n
        def dfs(y: int) -> None:
            vis[y] = True
            for x, w, i in g[y]:
                if dis[x] + w != dis[y]:
                    continue
                ans[i] = True
                if not vis[x]:
                    dfs(x)
        dfs(n - 1)
        return ans
```

```java [sol-Java]
class Solution {
    public boolean[] findAnswer(int n, int[][] edges) {
        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int i = 0; i < edges.length; i++) {
            int[] e = edges[i];
            int x = e[0], y = e[1], w = e[2];
            g[x].add(new int[]{y, w, i});
            g[y].add(new int[]{x, w, i});
        }

        long[] dis = new long[n];
        Arrays.fill(dis, Long.MAX_VALUE);
        dis[0] = 0;
        PriorityQueue<long[]> pq = new PriorityQueue<>((a, b) -> Long.compare(a[0], b[0]));
        pq.offer(new long[]{0, 0});
        while (!pq.isEmpty()) {
            long[] dxPair = pq.poll();
            long dx = dxPair[0];
            int x = (int) dxPair[1];
            if (dx > dis[x]) {
                continue;
            }
            for (int[] t : g[x]) {
                int y = t[0];
                int w = t[1];
                long newDis = dx + w;
                if (newDis < dis[y]) {
                    dis[y] = newDis;
                    pq.offer(new long[]{newDis, y});
                }
            }
        }

        boolean[] ans = new boolean[edges.length];
        // 图不连通
        if (dis[n - 1] == Long.MAX_VALUE) {
            return ans;
        }

        // 从终点出发 DFS
        boolean[] vis = new boolean[n];
        dfs(n - 1, g, dis, ans, vis);
        return ans;
    }

    private void dfs(int y, List<int[]>[] g, long[] dis, boolean[] ans, boolean[] vis) {
        vis[y] = true;
        for (int[] t : g[y]) {
            int x = t[0];
            int w = t[1];
            int i = t[2];
            if (dis[x] + w != dis[y]) {
                continue;
            }
            ans[i] = true;
            if (!vis[x]) {
                dfs(x, g, dis, ans, vis);
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<bool> findAnswer(int n, vector<vector<int>>& edges) {
        vector<vector<tuple<int, int, int>>> g(n);
        for (int i = 0; i < edges.size(); i++) {
            auto& e = edges[i];
            int x = e[0], y = e[1], w = e[2];
            g[x].emplace_back(y, w, i);
            g[y].emplace_back(x, w, i);
        }

        vector<long long> dis(n, LLONG_MAX);
        dis[0] = 0;
        priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> pq;
        pq.emplace(0, 0);
        while (!pq.empty()) {
            auto [dx, x] = pq.top();
            pq.pop();
            if (dx > dis[x]) {
                continue;
            }
            for (auto [y, w, _] : g[x]) {
                int new_dis = dx + w;
                if (new_dis < dis[y]) {
                    dis[y] = new_dis;
                    pq.emplace(new_dis, y);
                }
            }
        }

        vector<bool> ans(edges.size());
        // 图不连通
        if (dis[n - 1] == LLONG_MAX) {
            return ans;
        }

        // 从终点出发 DFS
        vector<int> vis(n);
        function<void(int)> dfs = [&](int y) {
            vis[y] = true;
            for (auto [x, w, i] : g[y]) {
                if (dis[x] + w != dis[y]) {
                    continue;
                }
                ans[i] = true;
                if (!vis[x]) {
                    dfs(x);
                }
            }
        };
        dfs(n - 1);
        return ans;
    }
};
```

```go [sol-Go]
func findAnswer(n int, edges [][]int) []bool {
	type edge struct{ to, w, i int }
	g := make([][]edge, n)
	for i, e := range edges {
		x, y, w := e[0], e[1], e[2]
		g[x] = append(g[x], edge{y, w, i})
		g[y] = append(g[y], edge{x, w, i})
	}

	// Dijkstra 算法模板
	dis := make([]int, n)
	for i := 1; i < n; i++ {
		dis[i] = math.MaxInt
	}
	h := hp{{}}
	for len(h) > 0 {
		p := heap.Pop(&h).(pair)
		x := p.x
		if p.dis > dis[x] {
			continue
		}
		for _, e := range g[x] {
			y := e.to
			newD := p.dis + e.w
			if newD < dis[y] {
				dis[y] = newD
				heap.Push(&h, pair{newD, y})
			}
		}
	}

	ans := make([]bool, len(edges))
	// 图不连通
	if dis[n-1] == math.MaxInt {
		return ans
	}

	// 从终点出发 DFS
	vis := make([]bool, n)
	var dfs func(int)
	dfs = func(y int) {
		vis[y] = true
		for _, e := range g[y] {
			x := e.to
			if dis[x]+e.w != dis[y] {
				continue
			}
			ans[e.i] = true
			if !vis[x] {
				dfs(x)
			}
		}
	}
	dfs(n - 1)
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

- 时间复杂度：$\mathcal{O}(n + m\log m)$，其中 $m$ 为 $\textit{edges}$ 的长度。
- 空间复杂度：$\mathcal{O}(n+m)$。

## 写法二：Dijkstra 算法 + BFS

```py [sol-Python3]
class Solution:
    def findAnswer(self, n: int, edges: List[List[int]]) -> List[bool]:
        g = [[] for _ in range(n)]
        for i, (x, y, w) in enumerate(edges):
            g[x].append((y, w, i))
            g[y].append((x, w, i))

        # Dijkstra 算法模板
        dis = [inf] * n
        dis[0] = 0
        h = [(0, 0)]
        while h:
            dx, x = heappop(h)
            if dx > dis[x]:
                continue
            for y, w, _ in g[x]:
                new_dis = dx + w
                if new_dis < dis[y]:
                    dis[y] = new_dis
                    heappush(h, (new_dis, y))

        ans = [False] * len(edges)
        # 图不连通
        if dis[-1] == inf:
            return ans

        # 从终点出发 BFS
        vis = [False] * n
        vis[-1] = True
        q = deque([n - 1])
        while q:
            y = q.popleft()
            for x, w, i in g[y]:
                if dis[x] + w != dis[y]:
                    continue
                ans[i] = True
                if not vis[x]:
                    vis[x] = True
                    q.append(x)
        return ans
```

```java [sol-Java]
class Solution {
    public boolean[] findAnswer(int n, int[][] edges) {
        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int i = 0; i < edges.length; i++) {
            int[] e = edges[i];
            int x = e[0], y = e[1], w = e[2];
            g[x].add(new int[]{y, w, i});
            g[y].add(new int[]{x, w, i});
        }

        long[] dis = new long[n];
        Arrays.fill(dis, Long.MAX_VALUE);
        dis[0] = 0;
        PriorityQueue<long[]> pq = new PriorityQueue<>((a, b) -> Long.compare(a[0], b[0]));
        pq.offer(new long[]{0, 0});
        while (!pq.isEmpty()) {
            long[] dxPair = pq.poll();
            long dx = dxPair[0];
            int x = (int) dxPair[1];
            if (dx > dis[x]) {
                continue;
            }
            for (int[] t : g[x]) {
                int y = t[0];
                int w = t[1];
                long newDis = dx + w;
                if (newDis < dis[y]) {
                    dis[y] = newDis;
                    pq.offer(new long[]{newDis, y});
                }
            }
        }

        boolean[] ans = new boolean[edges.length];
        // 图不连通
        if (dis[n - 1] == Long.MAX_VALUE) {
            return ans;
        }

        // 从终点出发 BFS
        boolean[] vis = new boolean[n];
        vis[n - 1] = true;
        Queue<Integer> q = new ArrayDeque<>();
        q.offer(n - 1);
        while (!q.isEmpty()) {
            int y = q.poll();
            for (int[] t : g[y]) {
                int x = t[0];
                int w = t[1];
                int i = t[2];
                if (dis[x] + w != dis[y]) {
                    continue;
                }
                ans[i] = true;
                if (!vis[x]) {
                    vis[x] = true;
                    q.offer(x);
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<bool> findAnswer(int n, vector<vector<int>>& edges) {
        vector<vector<tuple<int, int, int>>> g(n);
        for (int i = 0; i < edges.size(); i++) {
            auto& e = edges[i];
            int x = e[0], y = e[1], w = e[2];
            g[x].emplace_back(y, w, i);
            g[y].emplace_back(x, w, i);
        }

        vector<long long> dis(n, LLONG_MAX);
        dis[0] = 0;
        priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> pq;
        pq.emplace(0, 0);
        while (!pq.empty()) {
            auto [dx, x] = pq.top();
            pq.pop();
            if (dx > dis[x]) {
                continue;
            }
            for (auto [y, w, _] : g[x]) {
                int new_dis = dx + w;
                if (new_dis < dis[y]) {
                    dis[y] = new_dis;
                    pq.emplace(new_dis, y);
                }
            }
        }

        vector<bool> ans(edges.size());
        // 图不连通
        if (dis[n - 1] == LLONG_MAX) {
            return ans;
        }

        // 从终点出发 BFS
        vector<int> vis(n);
        vis[n - 1] = true;
        queue<int> q;
        q.push(n - 1);
        while (!q.empty()) {
            int y = q.front();
            q.pop();
            for (auto [x, w, i] : g[y]) {
                if (dis[x] + w != dis[y]) {
                    continue;
                }
                ans[i] = true;
                if (!vis[x]) {
                    vis[x] = true;
                    q.push(x);
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func findAnswer(n int, edges [][]int) []bool {
	type edge struct{ to, w, i int }
	g := make([][]edge, n)
	for i, e := range edges {
		x, y, w := e[0], e[1], e[2]
		g[x] = append(g[x], edge{y, w, i})
		g[y] = append(g[y], edge{x, w, i})
	}

	dis := make([]int, n)
	for i := 1; i < n; i++ {
		dis[i] = math.MaxInt
	}
	h := hp{{}}
	for len(h) > 0 {
		p := heap.Pop(&h).(pair)
		x := p.x
		if p.dis > dis[x] {
			continue
		}
		for _, e := range g[x] {
			y := e.to
			newD := p.dis + e.w
			if newD < dis[y] {
				dis[y] = newD
				heap.Push(&h, pair{newD, y})
			}
		}
	}

	ans := make([]bool, len(edges))
	// 图不连通
	if dis[n-1] == math.MaxInt {
		return ans
	}

	// 从终点出发 BFS
	vis := make([]bool, n)
	vis[n-1] = true
	q := []int{n - 1}
	for len(q) > 0 {
		y := q[0]
		q = q[1:]
		for _, e := range g[y] {
			x := e.to
			if dis[x]+e.w != dis[y] {
				continue
			}
			ans[e.i] = true
			if !vis[x] {
				vis[x] = true
				q = append(q, x)
			}
		}
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

- 时间复杂度：$\mathcal{O}(n + m\log m)$，其中 $m$ 为 $\textit{edges}$ 的长度。
- 空间复杂度：$\mathcal{O}(n+m)$。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

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
