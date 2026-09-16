# 2699. 修改图中的边权

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/modify-graph-edge-weights/
- 题目 slug：`modify-graph-edge-weights`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：2874
- 外部题解来源：https://leetcode.cn/problems/modify-graph-edge-weights/solutions/2278296/xiang-xi-fen-xi-liang-ci-dijkstrachou-mi-gv1m/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[详细分析两次 Dijkstra（稠密图下是线性做法）Python/Java/C++/Go](https://leetcode.cn/problems/modify-graph-edge-weights/solutions/2278296/xiang-xi-fen-xi-liang-ci-dijkstrachou-mi-gv1m/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xiang-xi-fen-xi-liang-ci-dijkstrachou-mi-gv1m`
- topic id：`2278296`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 什么时候无解？

题目要求把边权为 $-1$ 的至少修改为 $1$。如果都修改成 $1$，跑最短路（Dijkstra），发现从起点到终点的最短路长度大于 $\textit{target}$，那么由于边权变大，最短路不可能变小，所以此时无解。

另外一种无解的情况是，如果都修改成无穷大（本题限制为 $2\cdot 10^9$），发现从起点到终点的最短路长度小于 $\textit{target}$，那么由于边权变小，最短路不可能变大，所以此时也无解。

## 一个错误的思路

先把 $-1$ 都修改成 $1$，然后跑 Dijkstra。设从起点到终点的最短路长度为 $d$。

如果 $d<\textit{target}$，**看上去**把其中一个 $1$ 加上 $\textit{target}-d$，就可以使从起点到终点的最短路长度恰好为 $\textit{target}$ 了。

这是不对的，因为在增加边权后，最短路可能就走别的边了，不走刚才修改的这条边了。（具体见视频中画的例子。）

只修改一条边不行，那么修改两条边呢？也是不行的，最短路仍然可以走别的边。

## 正确思路

先把 $-1$ 都修改成 $1$，然后跑第一遍 Dijkstra，设从 $\textit{source}$ 到点 $i$ 的最短路长度为 $d_{i,0}$。

如果从起点到终点的最短路长度不超过 $\textit{target}$（否则无解，返回空数组），那我们就来尝试修改某些边权，使得从起点到终点的最短路长度恰好等于 $\textit{target}$。

这会引出两个问题：

1. 要按照什么样的**顺序**修改这些边？
2. 修改成多少合适？

正所谓「牵一发而动全身」，从上面对错误思路的分析可知，仅仅修改一个边权，就可能影响很多最短路的值。

那不妨再跑一遍 Dijkstra，由于 Dijkstra 算法保证每次拿到的点的最短路就是最终的最短路，所以按照 Dijkstra 算法遍历点/边的顺序去修改，就不会对**已确定的**最短路产生影响。

对于第二遍 Dijkstra，设从 $\textit{source}$ 到点 $i$ 的最短路长度为 $d_{i,1}$。

对于一条可以修改的边 $x-y$，假设要把它的边权改为 $w$，那么 $\textit{source}-x-y-\textit{destination}$ 这条路径由三部分组成：

1. 从 $\textit{source}$ 到 $x$ 的最短路，这是第二遍 Dijkstra 算出来的，即 $d_{x,1}$。
2. 从 $x$ 到 $y$，即 $w$。
3. 从 $y$ 到 $\textit{destination}$ 的最短路，由于后面的边还没有修改，这个最短路是第一遍 Dijkstra 算出来的，即 $d_{\textit{destination},0} - d_{y,0}$。

这三部分之和需要等于 $\textit{target}$，所以有

$$
d_{x,1} + w + d_{\textit{destination},0} - d_{y,0} = \textit{target}
$$

解得

$$
w = \textit{target} - d_{\textit{destination},0} + d_{y,0} - d_{x,1}
$$

> 注意上式中的 $\textit{target} - d_{\textit{destination},0}$ 是一个定值，代码中用 $\textit{delta}$ 表示。

根据「什么时候无解」中的分析，如果第二遍 Dijkstra 跑完后，从起点到终点的最短路仍然小于 $\textit{target}$，那么就说明无法修改，返回空数组。

否则，答案就是我们在第二遍 Dijkstra 中做出的修改。注意第二遍 Dijkstra 跑完后可能还有些边是 $-1$（因为在 $w=1$ 的时候没有修改，或者有些边不影响最短路），把这些边都改成 $1$ 就行。

代码实现时，为了修改边权，需要在邻接表中额外记录边的编号。

此外，由于输入最坏是稠密图，可以用朴素版的 Dijkstra 算法。

[视频讲解](https://www.bilibili.com/video/BV1Qm4y1t7cx/) 第四题。

```py [sol-Python3]
class Solution:
    def modifiedGraphEdges(self, n: int, edges: List[List[int]], source: int, destination: int, target: int) -> List[List[int]]:
        g = [[] for _ in range(n)]
        for i, (x, y, _) in enumerate(edges):
            g[x].append((y, i))
            g[y].append((x, i))  # 建图，额外保存边的编号

        dis = [[1 << 30] * 2 for _ in range(n)]
        dis[source] = [0] * 2

        def dijkstra(k: int) -> None:  # 这里 k 表示第一次/第二次
            vis = [False] * n
            while True:
                # 找到当前最短路，去更新它的邻居的最短路
                # 根据数学归纳法，dis[x][k] 一定是最短路长度
                x = -1
                for y, (b, d) in enumerate(zip(vis, dis)):
                    if not b and (x < 0 or d[k] < dis[x][k]):
                        x = y
                if x == destination:  # 起点 source 到终点 destination 的最短路已确定
                    return
                vis[x] = True  # 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
                for y, eid in g[x]:
                    wt = edges[eid][2]
                    if wt == -1:
                        wt = 1  # -1 改成 1
                    if k == 1 and edges[eid][2] == -1:
                        # 第二次 Dijkstra，改成 w
                        w = delta + dis[y][0] - dis[x][1]
                        if w > wt:
                            edges[eid][2] = wt = w  # 直接在 edges 上修改
                    # 更新最短路
                    dis[y][k] = min(dis[y][k], dis[x][k] + wt)

        dijkstra(0)
        delta = target - dis[destination][0]
        if delta < 0:  # -1 全改为 1 时，最短路比 target 还大
            return []

        dijkstra(1)
        if dis[destination][1] < target:  # 最短路无法再变大，无法达到 target
            return []

        for e in edges:
            if e[2] == -1:  # 剩余没修改的边全部改成 1
                e[2] = 1
        return edges
```

```java [sol-Java]
class Solution {
    public int[][] modifiedGraphEdges(int n, int[][] edges, int source, int destination, int target) {
        List<int[]> g[] = new ArrayList[n];
        Arrays.setAll(g, _ -> new ArrayList<>());
        for (int i = 0; i < edges.length; i++) {
            int x = edges[i][0];
            int y = edges[i][1];
            g[x].add(new int[]{y, i});
            g[y].add(new int[]{x, i}); // 建图，额外记录边的编号
        }

        int[][] dis = new int[n][2];
        for (int i = 0; i < n; i++) {
            if (i != source) {
                dis[i][0] = dis[i][1] = Integer.MAX_VALUE / 2;
            }
        }

        dijkstra(0, 0, g, edges, destination, dis);
        int delta = target - dis[destination][0];
        if (delta < 0) { // -1 全改为 1 时，最短路比 target 还大
            return new int[][]{};
        }

        dijkstra(delta, 1, g, edges, destination, dis);
        if (dis[destination][1] < target) { // 最短路无法再变大，无法达到 target
            return new int[][]{};
        }

        for (int[] e : edges) {
            if (e[2] == -1) { // 剩余没修改的边全部改成 1
                e[2] = 1;
            }
        }
        return edges;
    }

    // 朴素 Dijkstra 算法
    // 这里 k 表示第一次/第二次
    private void dijkstra(int delta, int k, List<int[]> g[], int[][] edges, int destination, int[][] dis) {
        int n = g.length;
        boolean[] vis = new boolean[n];
        for (;;) {
            // 找到当前最短路，去更新它的邻居的最短路
            // 根据数学归纳法，dis[x][k] 一定是最短路长度
            int x = -1;
            for (int i = 0; i < n; i++) {
                if (!vis[i] && (x < 0 || dis[i][k] < dis[x][k])) {
                    x = i;
                }
            }
            if (x == destination) { // 起点 source 到终点 destination 的最短路已确定
                return;
            }
            vis[x] = true; // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
            for (int[] e : g[x]) {
                int y = e[0];
                int eid = e[1];
                int wt = edges[eid][2];
                if (wt == -1) {
                    wt = 1; // -1 改成 1
                }
                if (k == 1 && edges[eid][2] == -1) {
                    // 第二次 Dijkstra，改成 w
                    int w = delta + dis[y][0] - dis[x][1];
                    if (w > wt) {
                        edges[eid][2] = wt = w; // 直接在 edges 上修改
                    }
                }
                // 更新最短路
                dis[y][k] = Math.min(dis[y][k], dis[x][k] + wt);
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<vector<int>> modifiedGraphEdges(int n, vector<vector<int>>& edges, int source, int destination, int target) {
        vector<vector<pair<int, int>>> g(n);
        for (int i = 0; i < edges.size(); i++) {
            int x = edges[i][0], y = edges[i][1];
            g[x].emplace_back(y, i);
            g[y].emplace_back(x, i); // 建图，额外记录边的编号
        }

        vector<array<int, 2>> dis(n, {INT_MAX / 2, INT_MAX / 2});
        dis[source][0] = dis[source][1] = 0;
        auto dijkstra = [&](int k, int delta) -> void { // 这里 k 表示第一次/第二次
            vector<int8_t> vis(n);
            for (;;) {
                // 找到当前最短路，去更新它的邻居的最短路
                // 根据数学归纳法，dis[x][k] 一定是最短路长度
                int x = -1;
                for (int i = 0; i < n; i++) {
                    if (!vis[i] && (x < 0 || dis[i][k] < dis[x][k])) {
                        x = i;
                    }
                }
                if (x == destination) { // 起点 source 到终点 destination 的最短路已确定
                    return;
                }
                vis[x] = true; // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
                for (auto& [y, eid] : g[x]) {
                    int wt = edges[eid][2];
                    if (wt == -1) {
                        wt = 1; // -1 改成 1
                    }
                    if (k == 1 && edges[eid][2] == -1) {
                        // 第二次 Dijkstra，改成 w
                        int w = delta + dis[y][0] - dis[x][1];
                        if (w > wt) {
                            edges[eid][2] = wt = w; // 直接在 edges 上修改
                        }
                    }
                    // 更新最短路
                    dis[y][k] = min(dis[y][k], dis[x][k] + wt);
                }
            }
        };

        dijkstra(0, 0);
        int delta = target - dis[destination][0];
        if (delta < 0) { // -1 全改为 1 时，最短路比 target 还大
            return {};
        }

        dijkstra(1, delta);
        if (dis[destination][1] < target) { // 最短路无法再变大，无法达到 target
            return {};
        }

        for (auto& e : edges) {
            if (e[2] == -1) { // 剩余没修改的边全部改成 1
                e[2] = 1;
            }
        }
        return edges;
    }
};
```

```go [sol-Go]
func modifiedGraphEdges(n int, edges [][]int, source, destination, target int) [][]int {
	type edge struct{ to, eid int }
	g := make([][]edge, n)
	for i, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], edge{y, i})
		g[y] = append(g[y], edge{x, i}) // 建图，额外记录边的编号
	}

	var delta int
	dis := make([][2]int, n)
	for i := range dis {
		dis[i][0] = math.MaxInt32 / 2
		dis[i][1] = math.MaxInt32 / 2
	}
	dis[source] = [2]int{}
	dijkstra := func(k int) { // 这里 k 表示第一次/第二次
		vis := make([]bool, n)
		for {
			// 找到当前最短路，去更新它的邻居的最短路
			// 根据数学归纳法，dis[x][k] 一定是最短路长度
			x := -1
			for y, b := range vis {
				if !b && (x < 0 || dis[y][k] < dis[x][k]) {
					x = y
				}
			}
			if x == destination { // 起点 source 到终点 destination 的最短路已确定
				return
			}
			vis[x] = true // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
			for _, e := range g[x] {
				y, wt := e.to, edges[e.eid][2]
				if wt == -1 {
					wt = 1 // -1 改成 1
				}
				if k == 1 && edges[e.eid][2] == -1 {
					// 第二次 Dijkstra，改成 w
					w := delta + dis[y][0] - dis[x][1]
					if w > wt {
						wt = w
						edges[e.eid][2] = w // 直接在 edges 上修改
					}
				}
				// 更新最短路
				dis[y][k] = min(dis[y][k], dis[x][k]+wt)
			}
		}
	}

	dijkstra(0)
	delta = target - dis[destination][0]
	if delta < 0 { // -1 全改为 1 时，最短路比 target 还大
		return nil
	}

	dijkstra(1)
	if dis[destination][1] < target { // 最短路无法再变大，无法达到 target
		return nil
	}

	for _, e := range edges {
		if e[2] == -1 { // 剩余没修改的边全部改成 1
			e[2] = 1
		}
	}
	return edges
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$。在稠密图（本题最坏情况）中，算法的时间复杂度与边的数量 $m=\mathcal{O}(n^2)$ 成正比。
- 空间复杂度：$\mathcal{O}(m)$，其中 $m$ 是 $\textit{edges}$ 的长度。注意输入是连通图，$m$ 至少为 $n-1$，所以 $\mathcal{O}(n+m)=\mathcal{O}(m)$

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

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
