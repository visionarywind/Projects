# 3112. 访问消失节点的最少时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-time-to-visit-disappearing-nodes/
- 题目 slug：`minimum-time-to-visit-disappearing-nodes`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：1757
- 外部题解来源：https://leetcode.cn/problems/minimum-time-to-visit-disappearing-nodes/solutions/2738888/dijkstra-suan-fa-pythonjavacgo-by-endles-7q6u/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Dijkstra 算法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-time-to-visit-disappearing-nodes/solutions/2738888/dijkstra-suan-fa-pythonjavacgo-by-endles-7q6u/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dijkstra-suan-fa-pythonjavacgo-by-endles-7q6u`
- topic id：`2738888`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**请先阅读** [Dijkstra 算法介绍](https://leetcode.cn/problems/network-delay-time/solution/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)。

对于本题，$\textit{answer}$ 几乎就是 $\textit{dis}$ 数组。只需要在 Dijkstra 算法的过程中，添加一个判断：

- 在更新最短路之前，如果最短路长度 $\ge \textit{disappear}[i]$，说明无法及时到达节点 $i$，不更新。

请看 [视频讲解](https://www.bilibili.com/video/BV1et42177VM/) 第三题，欢迎点赞关注！

### 答疑

**问**：我是这样做的，先不管 $\textit{disappear}$，用 Dijkstra 算法模板求出 $\textit{dis}$ 数组，然后把其中 $\textit{dis}[i]\ge \textit{disappear}[i]$ 的 $\textit{dis}[i]$ 改成 $-1$。这个做法是否正确？

**答**：这个做法是错的。考虑这样的数据：离节点 $0$ 近的节点 $x$，其 $\textit{disappear}[x]$ 很小；离节点 $0$ 远的节点 $y$，其 $\textit{disappear}[y]$ 很大。由于 $\textit{disappear}[x]$ 很小，我们无法通过节点 $x$，所以远处的节点 $y$ 实际上也无法到达。但错误做法会因为 $\textit{dis}[y] < \textit{disappear}[y]$，误认为节点 $y$ 可以到达，最终返回错误的答案。

```py [sol-Python3]
class Solution:
    def minimumTime(self, n: int, edges: List[List[int]], disappear: List[int]) -> List[int]:
        g = [[] for _ in range(n)]  # 稀疏图用邻接表
        for x, y, wt in edges:
            g[x].append((y, wt))
            g[y].append((x, wt))

        dis = [-1] * n
        dis[0] = 0
        h = [(0, 0)]
        while h:
            dx, x = heappop(h)
            if dx > dis[x]:  # x 之前出堆过
                continue
            for y, wt in g[x]:
                new_dis = dx + wt
                if new_dis < disappear[y] and (dis[y] < 0 or new_dis < dis[y]):
                    dis[y] = new_dis  # 更新 x 的邻居的最短路
                    heappush(h, (new_dis, y))
        return dis
```

```java [sol-Java]
class Solution {
    public int[] minimumTime(int n, int[][] edges, int[] disappear) {
        List<int[]>[] g = new ArrayList[n]; // 稀疏图用邻接表
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            int wt = e[2];
            g[x].add(new int[]{y, wt});
            g[y].add(new int[]{x, wt});
        }

        int[] dis = new int[n];
        Arrays.fill(dis, -1);
        dis[0] = 0;
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> (a[0] - b[0]));
        pq.offer(new int[]{0, 0});
        while (!pq.isEmpty()) {
            int[] p = pq.poll();
            int dx = p[0];
            int x = p[1];
            if (dx > dis[x]) { // x 之前出堆过
                continue;
            }
            for (int[] e : g[x]) {
                int y = e[0];
                int newDis = dx + e[1];
                if (newDis < disappear[y] && (dis[y] < 0 || newDis < dis[y])) {
                    dis[y] = newDis; // 更新 x 的邻居的最短路
                    pq.offer(new int[]{newDis, y});
                }
            }
        }
        return dis;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> minimumTime(int n, vector<vector<int>>& edges, vector<int>& disappear) {
        vector<vector<pair<int, int>>> g(n); // 稀疏图用邻接表
        for (auto& e : edges) {
            int x = e[0], y = e[1], wt = e[2];
            g[x].emplace_back(y, wt);
            g[y].emplace_back(x, wt);
        }

        vector<int> dis(n, -1);
        dis[0] = 0;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        pq.emplace(0, 0);
        while (!pq.empty()) {
            auto [dx, x] = pq.top();
            pq.pop();
            if (dx > dis[x]) { // x 之前出堆过
                continue;
            }
            for (auto& [y, d] : g[x]) {
                int new_dis = dx + d;
                if (new_dis < disappear[y] && (dis[y] < 0 || new_dis < dis[y])) {
                    dis[y] = new_dis; // 更新 x 的邻居的最短路
                    pq.emplace(new_dis, y);
                }
            }
        }
        return dis;
    }
};
```

```go [sol-Go]
func minimumTime(n int, edges [][]int, disappear []int) []int {
	type edge struct{ to, wt int }
	g := make([][]edge, n) // 稀疏图用邻接表
	for _, e := range edges {
		x, y, wt := e[0], e[1], e[2]
		g[x] = append(g[x], edge{y, wt})
		g[y] = append(g[y], edge{x, wt})
	}

	dis := make([]int, n)
	for i := range dis {
		dis[i] = -1
	}
	dis[0] = 0
	h := hp{{}}
	for len(h) > 0 {
		p := heap.Pop(&h).(pair)
		dx := p.dis
		x := p.x
		if dx > dis[x] { // x 之前出堆过
			continue
		}
		for _, e := range g[x] {
			y := e.to
			newDis := dx + e.wt
			if newDis < disappear[y] && (dis[y] < 0 || newDis < dis[y]) {
				dis[y] = newDis // 更新 x 的邻居的最短路
				heap.Push(&h, pair{newDis, y})
			}
		}
	}
	return dis
}

type pair struct{ dis, x int }
type hp []pair
func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].dis < h[j].dis }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(pair)) }
func (h *hp) Pop() (v any)      { a := *h; *h, v = a[:len(a)-1], a[len(a)-1]; return }
```

```js [sol-JavaScript]
var minimumTime = function(n, edges, disappear) {
    const g = Array.from({length: n}, () => []);
    for (const [x, y, wt] of edges) {
        g[x].push([y, wt]);
        g[y].push([x, wt]);
    }

    const dis = Array(n).fill(-1);
    const pq = new MinPriorityQueue(p => p[0]);
    dis[0] = 0;
    pq.enqueue([0, 0]);
    while (!pq.isEmpty()) {
        const [dx, x] = pq.dequeue();
        if (dx > dis[x]) { // x 之前出堆过
            continue;
        }
        for (const [y, wt] of g[x]) {
            const new_dis = dx + wt;
            if (new_dis < disappear[y] && (dis[y] < 0 || new_dis < dis[y])) {
                dis[y] = new_dis; // 更新 x 的邻居的最短路
                pq.enqueue([new_dis, y]);
            }
        }
    }
    return dis;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn minimum_time(n: i32, edges: Vec<Vec<i32>>, disappear: Vec<i32>) -> Vec<i32> {
        let n = n as usize;
        let mut g = vec![vec![]; n]; // 邻接表
        for e in edges {
            let x = e[0] as usize;
            let y = e[1] as usize;
            let wt = e[2];
            g[x].push((y, wt));
            g[y].push((x, wt));
        }

        let mut dis = vec![-1; n];
        dis[0] = 0;
        let mut h = BinaryHeap::new(); // 下面取相反数，变成小根堆
        h.push((0, 0));
        while let Some((dx, x)) = h.pop() {
            if -dx > dis[x] { // x 之前出堆过
                continue;
            }
            for &(y, d) in &g[x] {
                let new_dis = -dx + d;
                if new_dis < disappear[y] && (dis[y] < 0 || new_dis < dis[y]) {
                    dis[y] = new_dis; // 更新 x 的邻居的最短路
                    h.push((-new_dis, y));
                }
            }
        }
        dis
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m\log m)$，其中 $m$ 为 $\textit{edges}$ 的长度。注意堆中会有重复节点，所以至多有 $\mathcal{O}(m)$ 个元素，单次操作的复杂度是 $\mathcal{O}(\log m)$，不是 $\mathcal{O}(\log n)$。
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
