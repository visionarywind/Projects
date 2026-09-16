# 3970. 最多 K 个连续相同字符的最短路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/shortest-path-with-at-most-k-consecutive-identical-characters/
- 题目 slug：`shortest-path-with-at-most-k-consecutive-identical-characters`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：1840
- 外部题解来源：https://leetcode.cn/problems/shortest-path-with-at-most-k-consecutive-identical-characters/solutions/3986163/dijkstra-suan-fa-fen-ceng-tu-zui-duan-lu-magt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Dijkstra 算法（分层图最短路）Python/Java/C++/Go](https://leetcode.cn/problems/shortest-path-with-at-most-k-consecutive-identical-characters/solutions/3986163/dijkstra-suan-fa-fen-ceng-tu-zui-duan-lu-magt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dijkstra-suan-fa-fen-ceng-tu-zui-duan-lu-magt`
- topic id：`3986163`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置知识**：[Dijkstra 算法介绍](https://leetcode.cn/problems/network-delay-time/solution/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)

定义 $\textit{dis}[x][\textit{cnt}]$ 表示从节点 $0$ 到节点 $x$ 的最短路长度，满足路径中的最后连续相同字母个数为 $\textit{cnt}$。

初始值 $\textit{dis}[0][1] = 0$。

对于边权为 $w$ 的边 $x\to y$，分类讨论：

- 如果 $\textit{labels}[x] \ne \textit{labels}[y]$，那么重新计数，用 $\textit{dis}[x][\textit{cnt}]+w$ 更新 $\textit{dis}[y][1]$ 的最小值。
- 如果 $\textit{labels}[x] = \textit{labels}[y]$ 且 $\textit{cnt}+1\le k$，那么累加连续相同字母个数，用 $\textit{dis}[x][\textit{cnt}]+w$ 更新 $\textit{dis}[y][\textit{cnt}+1]$ 的最小值。

**注**：根据 Dijkstra 算法的原理，当节点 $n-1$ 首次出堆时，我们就算出了从节点 $0$ 到节点 $n-1$ 的最短路，可以直接返回答案。

```py [sol-Python3]
class Solution:
    def shortestPath(self, n: int, edges: list[list[int]], labels: str, k: int) -> int:
        g = [[] for _ in range(n)]
        for x, y, w in edges:
            g[x].append((y, w))

        dis = [[inf] * (k + 1) for _ in range(n)]
        h = [(0, 0, 1)]  # (最短路长度, 节点编号, 最后连续相同字母个数)

        while h:
            d, x, cnt = heappop(h)
            if x == n - 1:
                return d
            if d > dis[x][cnt]:
                continue
            for y, w in g[x]:
                new_cnt = 1 if labels[y] != labels[x] else cnt + 1
                new_dis = d + w
                if new_cnt <= k and new_dis < dis[y][new_cnt]:
                    dis[y][new_cnt] = new_dis
                    heappush(h, (new_dis, y, new_cnt))

        return -1
```

```java [sol-Java]
class Solution {
    public int shortestPath(int n, int[][] edges, String labels, int k) {
        char[] s = labels.toCharArray();
        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, _ -> new ArrayList<>());
        for (int[] e : edges) {
            g[e[0]].add(new int[]{e[1], e[2]});
        }

        int[][] dis = new int[n][k + 1];
        for (int[] row : dis) {
            Arrays.fill(row, Integer.MAX_VALUE);
        }

        // int[]{最短路长度, 节点编号, 最后连续相同字母个数}
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> a[0] - b[0]);
        pq.add(new int[]{0, 0, 1});

        while (!pq.isEmpty()) {
            int[] top = pq.poll();
            int d = top[0];
            int x = top[1];
            int cnt = top[2];
            if (x == n - 1) {
                return d;
            }
            if (d > dis[x][cnt]) {
                continue;
            }
            for (int[] e : g[x]) {
                int y = e[0];
                int newCnt = s[y] == s[x] ? cnt + 1 : 1;
                int newDis = d + e[1];
                if (newCnt <= k && newDis < dis[y][newCnt]) {
                    dis[y][newCnt] = newDis;
                    pq.add(new int[]{newDis, y, newCnt});
                }
            }
        }

        return -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int shortestPath(int n, vector<vector<int>>& edges, string labels, int k) {
        vector<vector<pair<int, int>>> g(n);
        for (auto& e : edges) {
            g[e[0]].emplace_back(e[1], e[2]);
        }

        vector dis(n, vector<int>(k + 1, INT_MAX));
        // tuple{最短路长度, 节点编号, 最后连续相同字母个数}
        priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<>> pq;
        pq.emplace(0, 0, 1);

        while (!pq.empty()) {
            auto [d, x, cnt] = pq.top();
            pq.pop();
            if (x == n - 1) {
                return d;
            }
            if (d > dis[x][cnt]) {
                continue;
            }
            for (auto& [y, w] : g[x]) {
                int new_cnt = labels[y] == labels[x] ? cnt + 1 : 1;
                int new_dis = d + w;
                if (new_cnt <= k && new_dis < dis[y][new_cnt]) {
                    dis[y][new_cnt] = new_dis;
                    pq.emplace(new_dis, y, new_cnt);
                }
            }
        }

        return -1;
    }
};
```

```go [sol-Go]
func shortestPath(n int, edges [][]int, labels string, k int) int {
	type edge struct{ to, w int }
	g := make([][]edge, n)
	for _, e := range edges {
		x, y, w := e[0], e[1], e[2]
		g[x] = append(g[x], edge{y, w})
	}

	dis := make([][]int, n)
	for i := range dis {
		dis[i] = make([]int, k+1)
		for j := range dis[i] {
			dis[i][j] = math.MaxInt
		}
	}
	h := hp{{0, 0, 1}}

	for len(h) > 0 {
		top := heap.Pop(&h).(tuple)
		d := top.dis
		x, cnt := top.x, top.cnt
		if x == n-1 {
			return d
		}
		if d > dis[x][cnt] {
			continue
		}
		for _, e := range g[x] {
			y := e.to
			newCnt := 1
			if labels[y] == labels[x] {
				newCnt = cnt + 1
			}
			newDis := d + e.w
			if newCnt <= k && newDis < dis[y][newCnt] {
				dis[y][newCnt] = newDis
				heap.Push(&h, tuple{newDis, y, newCnt})
			}
		}
	}

	return -1
}

// 最短路长度, 节点编号, 最后连续相同字母个数
type tuple struct{ dis, x, cnt int }
type hp []tuple

func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].dis < h[j].dis }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(tuple)) }
func (h *hp) Pop() (v any)      { a := *h; *h, v = a[:len(a)-1], a[len(a)-1]; return }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk + mk\log (mk))$，其中 $m$ 是 $\textit{edges}$ 的长度。分层图中有 $\mathcal{O}(nk)$ 个点，$\mathcal{O}(mk)$ 条边。注意我们用的是懒更新堆，堆中有 $\mathcal{O}(mk)$ 个元素。创建 $\textit{dis}$ 数组需要 $\mathcal{O}(nk)$ 时间。
- 空间复杂度：$\mathcal{O}(nk + mk)$。

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
