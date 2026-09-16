# 882. 细分图中的可到达节点

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reachable-nodes-in-subdivided-graph/
- 题目 slug：`reachable-nodes-in-subdivided-graph`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：2328
- 外部题解来源：https://leetcode.cn/problems/reachable-nodes-in-subdivided-graph/solutions/1991509/tu-jie-zhuan-huan-cheng-dan-yuan-zui-dua-6l8o/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】转换成单源最短路问题，附题单（Python/Java/C++/Go）](https://leetcode.cn/problems/reachable-nodes-in-subdivided-graph/solutions/1991509/tu-jie-zhuan-huan-cheng-dan-yuan-zui-dua-6l8o/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-zhuan-huan-cheng-dan-yuan-zui-dua-6l8o`
- topic id：`1991509`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置知识**：[Dijkstra 算法介绍](https://leetcode.cn/problems/network-delay-time/solution/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)

![lc882-3.png](https://pic.leetcode.cn/1712301565-JfITbH-lc882-3.png)

```py [sol-Python3]
class Solution:
    def reachableNodes(self, edges: List[List[int]], maxMoves: int, n: int) -> int:
        g = [[] for _ in range(n)]
        for u, v, cnt in edges:
            g[u].append((v, cnt + 1))
            g[v].append((u, cnt + 1))  # 建图

        dist = self.dijkstra(g, 0, maxMoves)  # 从 0 出发的最短路

        ans = sum(d <= maxMoves for d in dist)  # 可以在 maxMoves 步内到达的点的个数
        for u, v, cnt in edges:
            a = max(maxMoves - dist[u], 0)
            b = max(maxMoves - dist[v], 0)
            ans += min(a + b, cnt)  # 这条边上可以到达的节点数
        return ans

    # Dijkstra 算法模板
    # 返回从 start 到每个点的最短路
    def dijkstra(self, g: List[List[Tuple[int]]], start: int, maxMoves: int) -> List[int]:
        dist = [inf] * len(g)
        dist[start] = 0
        h = [(0, start)]

        # 优化：最短路 >= maxMoves 就不用再算了（注意本题边权 > 0）
        while h and h[0][0] < maxMoves:
            d, x = heappop(h)
            if d > dist[x]:
                continue
            for y, wt in g[x]:
                new_d = d + wt
                if new_d < dist[y]:
                    dist[y] = new_d
                    heappush(h, (new_d, y))
        return dist
```

```java [sol-Java]
class Solution {
    public int reachableNodes(int[][] edges, int maxMoves, int n) {
        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<int[]>());
        for (int[] e : edges) {
            int u = e[0], v = e[1], cnt = e[2];
            g[u].add(new int[]{v, cnt + 1});
            g[v].add(new int[]{u, cnt + 1}); // 建图
        }

        int[] dist = dijkstra(g, 0, maxMoves); // 从 0 出发的最短路

        int ans = 0;
        for (int d : dist) {
            if (d <= maxMoves) { // 这个点可以在 maxMoves 步内到达
                ans++;
            }
        }
        for (int[] e : edges) {
            int u = e[0], v = e[1], cnt = e[2];
            int a = Math.max(maxMoves - dist[u], 0);
            int b = Math.max(maxMoves - dist[v], 0);
            ans += Math.min(a + b, cnt); // 这条边上可以到达的节点数
        }
        return ans;
    }

    // Dijkstra 算法模板
    // 返回从 start 到每个点的最短路
    private int[] dijkstra(List<int[]>[] g, int start, int maxMoves) {
        int[] dist = new int[g.length];
        Arrays.fill(dist, Integer.MAX_VALUE);
        dist[start] = 0;
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> a[1] - b[1]);
        pq.offer(new int[]{start, 0});

        // 优化：最短路 >= maxMoves 就不用再算了（注意本题边权 > 0）
        while (!pq.isEmpty() && pq.peek()[1] < maxMoves) {
            int[] p = pq.poll();
            int x = p[0], d = p[1];
            if (d > dist[x]) {
                continue;
            }
            for (int[] e : g[x]) {
                int y = e[0];
                int newD = d + e[1];
                if (newD < dist[y]) {
                    dist[y] = newD;
                    pq.offer(new int[]{y, newD});
                }
            }
        }
        return dist;
    }
}
```

```cpp [sol-C++]
class Solution {
    // Dijkstra 算法模板
    // 返回从 start 到每个点的最短路
    vector<int> dijkstra(vector<vector<pair<int, int>>>& g, int start, int maxMoves) {
        vector<int> dist(g.size(), INT_MAX);
        dist[start] = 0;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        pq.emplace(0, start);

        // 优化：最短路 >= maxMoves 就不用再算了（注意本题边权 > 0）
        while (!pq.empty() && pq.top().first < maxMoves) {
            auto [d, x] = pq.top();
            pq.pop();
            if (d > dist[x]) {
                continue;
            }
            for (auto& [y, wt] : g[x]) {
                int new_d = d + wt;
                if (new_d < dist[y]) {
                    dist[y] = new_d;
                    pq.emplace(new_d, y);
                }
            }
        }
        return dist;
    }

public:
    int reachableNodes(vector<vector<int>>& edges, int maxMoves, int n) {
        vector<vector<pair<int, int>>> g(n);
        for (auto& e: edges) {
            int u = e[0], v = e[1], cnt = e[2];
            g[u].emplace_back(v, cnt + 1);
            g[v].emplace_back(u, cnt + 1); // 建图
        }

        auto dist = dijkstra(g, 0, maxMoves); // 从 0 出发的最短路

        int ans = 0;
        for (int d : dist) {
            if (d <= maxMoves) { // 这个点可以在 maxMoves 步内到达
                ans++;
            }
        }
        for (auto& e: edges) {
            int u = e[0], v = e[1], cnt = e[2];
            int a = max(maxMoves - dist[u], 0);
            int b = max(maxMoves - dist[v], 0);
            ans += min(a + b, cnt); // 这条边上可以到达的节点数
        }
        return ans;
    }
};
```

```go [sol-Go]
func reachableNodes(edges [][]int, maxMoves, n int) (ans int) {
    g := make([][]neighbor, n)
    for _, e := range edges {
        u, v, cnt := e[0], e[1], e[2]
        g[u] = append(g[u], neighbor{v, cnt + 1})
        g[v] = append(g[v], neighbor{u, cnt + 1}) // 建图
    }

    dist := dijkstra(g, 0, maxMoves) // 从 0 出发的最短路

    for _, d := range dist {
        if d <= maxMoves { // 这个点可以在 maxMoves 步内到达
            ans++
        }
    }
    for _, e := range edges {
        u, v, cnt := e[0], e[1], e[2]
        a := max(maxMoves-dist[u], 0)
        b := max(maxMoves-dist[v], 0)
        ans += min(a+b, cnt) // 这条边上可以到达的节点数
    }
    return
}

// 以下为 Dijkstra 算法模板
type neighbor struct{ to, weight int }

func dijkstra(g [][]neighbor, start int, maxMove int) []int {
    dist := make([]int, len(g))
    for i := range dist {
        dist[i] = math.MaxInt
    }
    dist[start] = 0
    h := hp{{start, 0}}

    // 优化：最短路 >= maxMoves 就不用再算了（注意本题边权 > 0）
    for len(h) > 0 && h[0].dist < maxMove {
        p := heap.Pop(&h).(pair)
        x := p.x
        if dist[x] < p.dist {
            continue
        }
        for _, e := range g[x] {
            y := e.to
            if d := p.dist + e.weight; d < dist[y] {
                dist[y] = d
                heap.Push(&h, pair{y, d})
            }
        }
    }
    return dist
}

type pair struct{ x, dist int }
type hp []pair
func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].dist < h[j].dist }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(pair)) }
func (h *hp) Pop() (v any)      { a := *h; *h, v = a[:len(a)-1], a[len(a)-1]; return }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + m\log m)$，其中 $m$ 为 $\textit{edges}$ 的长度。注意如果同一个点的最短路被更新多次，堆中会有多个相同点，因此堆中的元素个数是 $\mathcal{O}(m)$ 的，每次入堆出堆的时间复杂度为 $\mathcal{O}(\log m)$。
- 空间复杂度：$\mathcal{O}(n+m)$。

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
