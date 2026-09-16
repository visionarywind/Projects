# 743. 网络延迟时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/network-delay-time/
- 题目 slug：`network-delay-time`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/network-delay-time/solutions/2668220/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种 Dijkstra 写法，附题单（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/network-delay-time/solutions/2668220/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8`
- topic id：`2668220`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## Dijkstra 算法介绍

定义 $g[i][j]$ 表示节点 $i$ 到节点 $j$ 这条边的边权。如果没有 $i$ 到 $j$ 的边，则 $g[i][j]=\infty$。

定义 $\textit{dis}[i]$ 表示起点 $k$ 到节点 $i$ 的最短路长度，一开始 $\textit{dis}[k]=0$，其余 $\textit{dis}[i]=\infty$ 表示尚未计算出。

我们的目标是计算出最终的 $\textit{dis}$ 数组。

- 首先更新起点 $k$ 到其邻居 $y$ 的最短路，即更新 $\textit{dis}[y]$ 为 $g[k][y]$。
- 然后取除了起点 $k$ 以外的 $\textit{dis}[i]$ 的最小值，假设最小值对应的节点是 $3$。此时可以断言：$\textit{dis}[3]$ 已经是 $k$ 到 $3$ 的最短路长度，不可能有其它 $k$ 到 $3$ 的路径更短！反证法：假设存在更短的路径，那我们一定会从 $k$ 出发经过一个点 $u$，它的 $\textit{dis}[u]$ 比 $\textit{dis}[3]$ 还要小，然后再经过一些边到达 $3$，得到更小的 $\textit{dis}[3]$。但 $\textit{dis}[3]$ 已经是最小的了，并且图中**没有负数边权**，所以 $u$ 是不存在的，矛盾。故原命题成立，此时我们得到了 $\textit{dis}[3]$ 的最终值。
- 用节点 $3$ 到其邻居 $y$ 的边权 $g[3][y]$ 更新 $\textit{dis}[y]$：如果 $\textit{dis}[3] + g[3][y] < \textit{dis}[y]$，那么更新 $\textit{dis}[y]$ 为 $\textit{dis}[3] + g[3][y]$，否则不更新。
- 然后取除了节点 $k,3$ 以外的 $\textit{dis}[i]$ 的最小值，重复上述过程。
- 由数学归纳法可知，这一做法可以得到每个点的最短路。当所有点的最短路都已确定时，算法结束。

## 写法一：朴素 Dijkstra（适用于稠密图）

> **稠密图**：边的数量级和 $n^2$ 相当的图。

对于本题，在计算最短路时，如果发现当前找到的最小最短路等于 $\infty$，说明有节点无法到达，可以提前结束算法，返回 $-1$。

如果所有节点都可以到达，返回 $\max(\textit{dis})$。

代码实现时，节点编号改成从 $0$ 开始。

```py [sol-Python3]
class Solution:
    def networkDelayTime(self, times: List[List[int]], n: int, k: int) -> int:
        g = [[inf for _ in range(n)] for _ in range(n)]  # 邻接矩阵
        for x, y, d in times:
            g[x - 1][y - 1] = d

        dis = [inf] * n
        ans = dis[k - 1] = 0
        done = [False] * n
        while True:
            x = -1
            for i, ok in enumerate(done):
                if not ok and (x < 0 or dis[i] < dis[x]):
                    x = i
            if x < 0:
                return ans  # 最后一次算出的最短路就是最大的
            if dis[x] == inf:  # 有节点无法到达
                return -1
            ans = dis[x]  # 求出的最短路会越来越大
            done[x] = True  # 最短路长度已确定（无法变得更小）
            for y, d in enumerate(g[x]):
                # 更新 x 的邻居的最短路
                dis[y] = min(dis[y], dis[x] + d)
```

```java [sol-Java]
class Solution {
    public int networkDelayTime(int[][] times, int n, int k) {
        final int INF = Integer.MAX_VALUE / 2; // 防止加法溢出
        int[][] g = new int[n][n]; // 邻接矩阵
        for (int[] row : g) {
            Arrays.fill(row, INF);
        }
        for (int[] t : times) {
            g[t[0] - 1][t[1] - 1] = t[2];
        }

        int maxDis = 0;
        int[] dis = new int[n];
        Arrays.fill(dis, INF);
        dis[k - 1] = 0;
        boolean[] done = new boolean[n];
        while (true) {
            int x = -1;
            for (int i = 0; i < n; i++) {
                if (!done[i] && (x < 0 || dis[i] < dis[x])) {
                    x = i;
                }
            }
            if (x < 0) {
                return maxDis; // 最后一次算出的最短路就是最大的
            }
            if (dis[x] == INF) { // 有节点无法到达
                return -1;
            }
            maxDis = dis[x]; // 求出的最短路会越来越大
            done[x] = true; // 最短路长度已确定（无法变得更小）
            for (int y = 0; y < n; y++) {
                // 更新 x 的邻居的最短路
                dis[y] = Math.min(dis[y], dis[x] + g[x][y]);
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        vector<vector<int>> g(n, vector<int>(n, INT_MAX / 2)); // 邻接矩阵
        for (auto& t : times) {
            g[t[0] - 1][t[1] - 1] = t[2];
        }

        vector<int> dis(n, INT_MAX / 2), done(n);
        dis[k - 1] = 0;
        while (true) {
            int x = -1;
            for (int i = 0; i < n; i++) {
                if (!done[i] && (x < 0 || dis[i] < dis[x])) {
                    x = i;
                }
            }
            if (x < 0) {
                return ranges::max(dis);
            }
            if (dis[x] == INT_MAX / 2) { // 有节点无法到达
                return -1;
            }
            done[x] = true; // 最短路长度已确定（无法变得更小）
            for (int y = 0; y < n; y++) {
                // 更新 x 的邻居的最短路
                dis[y] = min(dis[y], dis[x] + g[x][y]);
            }
        }
    }
};
```

```go [sol-Go]
func networkDelayTime(times [][]int, n, k int) int {
    const inf = math.MaxInt / 2 // 防止加法溢出
    g := make([][]int, n) // 邻接矩阵
    for i := range g {
        g[i] = make([]int, n)
        for j := range g[i] {
            g[i][j] = inf
        }
    }
    for _, t := range times {
        g[t[0]-1][t[1]-1] = t[2]
    }

    dis := make([]int, n)
    for i := range dis {
        dis[i] = inf
    }
    dis[k-1] = 0
    done := make([]bool, n)
    for {
        x := -1
        for i, ok := range done {
            if !ok && (x < 0 || dis[i] < dis[x]) {
                x = i
            }
        }
        if x < 0 {
            return slices.Max(dis)
        }
        if dis[x] == inf { // 有节点无法到达
            return -1
        }
        done[x] = true // 最短路长度已确定（无法变得更小）
        for y, d := range g[x] {
            // 更新 x 的邻居的最短路
            dis[y] = min(dis[y], dis[x]+d)
        }
    }
}
```

```js [sol-JavaScript]
var networkDelayTime = function(times, n, k) {
    const g = Array.from({length: n}, () => Array(n).fill(Infinity)); // 邻接矩阵
    for (const [x, y, d] of times) {
        g[x - 1][y - 1] = d;
    }

    const dis = Array(n).fill(Infinity);
    dis[k - 1] = 0;
    const done = Array(n).fill(false);
    while (true) {
        let x = -1;
        for (let i = 0; i < n; i++) {
            if (!done[i] && (x < 0 || dis[i] < dis[x])) {
                x = i;
            }
        }
        if (x < 0) {
            return Math.max(...dis);
        }
        if (dis[x] === Infinity) { // 有节点无法到达
            return -1;
        }
        done[x] = true; // 最短路长度已确定（无法变得更小）
        for (let y = 0; y < n; y++) {
            // 更新 x 的邻居的最短路
            dis[y] = Math.min(dis[y], dis[x] + g[x][y]);
        }
    }
};
```

```rust [sol-Rust]
impl Solution {
    pub fn network_delay_time(times: Vec<Vec<i32>>, n: i32, k: i32) -> i32 {
        const INF: i32 = i32::MAX / 2; // 防止加法溢出
        let n = n as usize;
        let mut g = vec![vec![INF; n]; n]; // 邻接矩阵
        for t in times {
            g[t[0] as usize - 1][t[1] as usize - 1] = t[2];
        }

        let mut dis = vec![INF; n];
        dis[k as usize - 1] = 0;
        let mut done = vec![false; n];
        loop {
            let mut x = n;
            for (i, &ok) in done.iter().enumerate() {
                if !ok && (x == n || dis[i] < dis[x]) {
                    x = i;
                }
            }
            if x == n {
                return *dis.iter().max().unwrap();
            }
            if dis[x] == INF { // 有节点无法到达
                return -1;
            }
            done[x] = true; // 最短路长度已确定（无法变得更小）
            for (y, &d) in g[x].iter().enumerate() {
                // 更新 x 的邻居的最短路
                dis[y] = dis[y].min(dis[x] + d);
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 写法二：堆优化 Dijkstra（适用于稀疏图）

> **稀疏图**：边的数量远小于 $n^2$ 的图。

寻找最小值的过程可以用一个最小堆来快速完成：

- 一开始把 $(\textit{dis}[k],k)$ 二元组入堆。
- 当节点 $x$ 首次出堆时，$\textit{dis}[x]$ 就是写法一中寻找的最小最短路。
- 更新 $\textit{dis}[y]$ 时，把 $(\textit{dis}[y],y)$ 二元组入堆。

注意，如果一个节点 $x$ 在出堆前，其最短路长度 $\textit{dis}[x]$ 被多次更新，那么堆中会有多个重复的 $x$，并且包含 $x$ 的二元组中的 $\textit{dis}[x]$ 是互不相同的（因为我们只在找到更小的最短路时才会把二元组入堆）。

所以写法一中的 $\textit{done}$ 数组可以省去，取而代之的是用出堆的最短路值（记作 $\textit{dx}$）与当前的 $\textit{dis}[x]$ 比较，如果 $\textit{dx} > \textit{dis}[x]$ 说明 $x$ 之前出堆过，我们已经更新了 $x$ 的邻居的最短路，所以这次就不用更新了，继续外层循环。

### 答疑

**问**：为什么代码要判断 `dx > dis[x]`？

**答**：对于同一个 $x$，例如先入堆一个比较大的 $\textit{dis}[x]=10$，后面又把 $\textit{dis}[x]$ 更新成 $5$，之后这个 $5$ 会先出堆，然后再把 $10$ 出堆。$10$ 出堆时候是没有必要去更新周围邻居的最短路的，因为 $5$ 出堆之后，就已经把邻居的最短路更新过了，用 $10$ 是无法把邻居的最短路变得更短的，所以直接 `continue`。

```py [sol-Python3]
class Solution:
    def networkDelayTime(self, times: List[List[int]], n: int, k: int) -> int:
        g = [[] for _ in range(n)]  # 邻接表
        for x, y, d in times:
            g[x - 1].append((y - 1, d))

        dis = [inf] * n
        dis[k - 1] = 0
        h = [(0, k - 1)]
        while h:
            dx, x = heappop(h)
            if dx > dis[x]:  # x 之前出堆过
                continue
            for y, d in g[x]:
                new_dis = dx + d
                if new_dis < dis[y]:
                    dis[y] = new_dis  # 更新 x 的邻居的最短路
                    heappush(h, (new_dis, y))
        mx = max(dis)
        return mx if mx < inf else -1
```

```java [sol-Java]
class Solution {
    public int networkDelayTime(int[][] times, int n, int k) {
        List<int[]>[] g = new ArrayList[n]; // 邻接表
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] t : times) {
            g[t[0] - 1].add(new int[]{t[1] - 1, t[2]});
        }

        int maxDis = 0;
        int left = n; // 未确定最短路的节点个数
        int[] dis = new int[n];
        Arrays.fill(dis, Integer.MAX_VALUE);
        dis[k - 1] = 0;
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> (a[0] - b[0]));
        pq.offer(new int[]{0, k - 1});
        while (!pq.isEmpty()) {
            int[] p = pq.poll();
            int dx = p[0];
            int x = p[1];
            if (dx > dis[x]) { // x 之前出堆过
                continue;
            }
            maxDis = dx; // 求出的最短路会越来越大
            left--;
            for (int[] e : g[x]) {
                int y = e[0];
                int newDis = dx + e[1];
                if (newDis < dis[y]) {
                    dis[y] = newDis; // 更新 x 的邻居的最短路
                    pq.offer(new int[]{newDis, y});
                }
            }
        }
        return left == 0 ? maxDis : -1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int networkDelayTime(vector<vector<int>>& times, int n, int k) {
        vector<vector<pair<int, int>>> g(n); // 邻接表
        for (auto& t : times) {
            g[t[0] - 1].emplace_back(t[1] - 1, t[2]);
        }

        vector<int> dis(n, INT_MAX);
        dis[k - 1] = 0;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        pq.emplace(0, k - 1);
        while (!pq.empty()) {
            auto [dx, x] = pq.top();
            pq.pop();
            if (dx > dis[x]) { // x 之前出堆过
                continue;
            }
            for (auto &[y, d] : g[x]) {
                int new_dis = dx + d;
                if (new_dis < dis[y]) {
                    dis[y] = new_dis; // 更新 x 的邻居的最短路
                    pq.emplace(new_dis, y);
                }
            }
        }
        int mx = ranges::max(dis);
        return mx < INT_MAX ? mx : -1;
    }
};
```

```go [sol-Go]
func networkDelayTime(times [][]int, n, k int) int {
    type edge struct{ to, wt int }
    g := make([][]edge, n) // 邻接表
    for _, t := range times {
        g[t[0]-1] = append(g[t[0]-1], edge{t[1] - 1, t[2]})
    }

    dis := make([]int, n)
    for i := range dis {
        dis[i] = math.MaxInt
    }
    dis[k-1] = 0
    h := hp{{0, k - 1}}
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
            if newDis < dis[y] {
                dis[y] = newDis // 更新 x 的邻居的最短路
                heap.Push(&h, pair{newDis, y})
            }
        }
    }
    mx := slices.Max(dis)
    if mx < math.MaxInt {
        return mx
    }
    return -1
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
var networkDelayTime = function(times, n, k) {
    const g = Array.from({length: n}, () => []); // 邻接表
    for (const [x, y, d] of times) {
        g[x - 1].push([y - 1, d]);
    }

    const dis = Array(n).fill(Infinity);
    dis[k - 1] = 0;
    const pq = new MinPriorityQueue(e => e[0]);
    pq.enqueue([0, k - 1]);
    while (!pq.isEmpty()) {
        const [dx, x] = pq.dequeue();
        if (dx > dis[x]) { // x 之前出堆过
            continue;
        }
        for (const [y, d] of g[x]) {
            const newDis = dx + d;
            if (newDis < dis[y]) {
                dis[y] = newDis; // 更新 x 的邻居的最短路
                pq.enqueue([newDis, y]);
            }
        }
    }
    const mx = Math.max(...dis);
    return mx < Infinity ? mx : -1;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn network_delay_time(times: Vec<Vec<i32>>, n: i32, k: i32) -> i32 {
        let n = n as usize;
        let k = k as usize - 1;
        let mut g = vec![vec![]; n]; // 邻接表
        for t in times {
            g[t[0] as usize - 1].push((t[1] as usize - 1, t[2]));
        }

        let mut dis = vec![i32::MAX; n];
        dis[k] = 0;
        let mut h = BinaryHeap::new();
        h.push((0, k));
        while let Some((dx, x)) = h.pop() {
            if -dx > dis[x] { // x 之前出堆过
                continue;
            }
            for &(y, d) in &g[x] {
                let new_dis = -dx + d;
                if new_dis < dis[y] {
                    dis[y] = new_dis; // 更新 x 的邻居的最短路
                    h.push((-new_dis, y));
                }
            }
        }
        let mx = *dis.iter().max().unwrap();
        if mx < i32::MAX { mx } else { -1 }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log m)$，其中 $m$ 为 $\textit{times}$ 的长度。由于 $m\ge n-1$，分析复杂度时以 $m$ 为主。注意堆中会有重复节点，所以至多有 $\mathcal{O}(m)$ 个元素，单次操作的复杂度是 $\mathcal{O}(\log m)$。值得注意的是，如果输入的是稠密图，写法二的时间复杂度为 $\mathcal{O}(n^2\log n)$，不如写法一。
- 空间复杂度：$\mathcal{O}(m)$。

更多相似题目，见下面图论题单中的「**单源最短路**」。

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
