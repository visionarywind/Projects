# 2065. 最大化一张图中的路径价值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-path-quality-of-a-graph/
- 题目 slug：`maximum-path-quality-of-a-graph`
- 来源专题：图论算法
- 来源分类路径：九、其他
- 难度分：2178
- 外部题解来源：https://leetcode.cn/problems/maximum-path-quality-of-a-graph/solutions/1088414/bao-sou-jian-zhi-by-endlesscheng-iv6z/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[暴搜+最短路剪枝（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-path-quality-of-a-graph/solutions/1088414/bao-sou-jian-zhi-by-endlesscheng-iv6z/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bao-sou-jian-zhi-by-endlesscheng-iv6z`
- topic id：`1088414`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：暴搜

根据题目的数据范围，路径至多有 $\dfrac{\textit{maxTime}}{\min(\textit{time}_j)}\le \dfrac{100}{10} = 10$ 条边，这意味着搜索树至多有 $11$ 层；同时，题目保证每个节点至多有 $4$ 条边与之相连，因此搜索树的每个节点至多有 $4$ 个儿子。所以本题可以视作在一棵层数至多为 $11$ 的四叉树上递归，这可以在时限内跑完。

递归到下一个节点之前，如果发现消耗的时间超过了 $\textit{maxTime}$，则不递归。

```py [sol-Python3]
class Solution:
    def maximalPathQuality(self, values: List[int], edges: List[List[int]], max_time: int) -> int:
        n = len(values)
        g = [[] for _ in range(n)]
        for x, y, t in edges:
            g[x].append((y, t))
            g[y].append((x, t))

        def dfs(x: int, sum_time: int, sum_value: int) -> None:
            if x == 0:
                nonlocal ans
                ans = max(ans, sum_value)
                # 注意这里没有 return，还可以继续走
            for y, t in g[x]:
                if sum_time + t > max_time:
                    continue
                if vis[y]:
                    dfs(y, sum_time + t, sum_value)
                else:
                    vis[y] = True
                    # 每个节点的价值至多算入价值总和中一次
                    dfs(y, sum_time + t, sum_value + values[y])
                    vis[y] = False  # 恢复现场

        ans = 0
        vis = [False] * n
        vis[0] = True
        dfs(0, 0, values[0])
        return ans
```

```java [sol-Java]
class Solution {
    public int maximalPathQuality(int[] values, int[][] edges, int maxTime) {
        int n = values.length;
        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            int t = e[2];
            g[x].add(new int[]{y, t});
            g[y].add(new int[]{x, t});
        }

        boolean[] vis = new boolean[n];
        vis[0] = true;
        return dfs(0, 0, values[0], vis, g, values, maxTime);
    }

    private int dfs(int x, int sumTime, int sumValue, boolean[] vis, List<int[]>[] g, int[] values, int maxTime) {
        int res = x == 0 ? sumValue : 0;
        for (int[] e : g[x]) {
            int y = e[0];
            int t = e[1];
            if (sumTime + t > maxTime) {
                continue;
            }
            if (vis[y]) {
                res = Math.max(res, dfs(y, sumTime + t, sumValue, vis, g, values, maxTime));
            } else {
                vis[y] = true;
                // 每个节点的价值至多算入价值总和中一次
                res = Math.max(res, dfs(y, sumTime + t, sumValue + values[y], vis, g, values, maxTime));
                vis[y] = false; // 恢复现场
            }
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximalPathQuality(vector<int>& values, vector<vector<int>>& edges, int max_time) {
        int n = values.size();
        vector<vector<pair<int, int>>> g(n);
        for (auto& e : edges) {
            int x = e[0], y = e[1], t = e[2];
            g[x].emplace_back(y, t);
            g[y].emplace_back(x, t);
        }

        int ans = 0;
        vector<int> vis(n);
        vis[0] = true;
        auto dfs = [&](auto&& dfs, int x, int sum_time, int sum_value) -> void {
            if (x == 0) {
                ans = max(ans, sum_value);
                // 注意这里没有 return，还可以继续走
            }
            for (auto& [y, t] : g[x]) {
                if (sum_time + t > max_time) {
                    continue;
                }
                if (vis[y]) {
                    dfs(dfs, y, sum_time + t, sum_value);
                } else {
                    vis[y] = true;
                    // 每个节点的价值至多算入价值总和中一次
                    dfs(dfs, y, sum_time + t, sum_value + values[y]);
                    vis[y] = false; // 恢复现场
                }
            }
        };
        dfs(dfs, 0, 0, values[0]);
        return ans;
    }
};
```

```go [sol-Go]
func maximalPathQuality(values []int, edges [][]int, maxTime int) (ans int) {
    n := len(values)
    type edge struct{ to, time int }
    g := make([][]edge, n)
    for _, e := range edges {
        x, y, t := e[0], e[1], e[2]
        g[x] = append(g[x], edge{y, t})
        g[y] = append(g[y], edge{x, t})
    }

    vis := make([]bool, n)
    vis[0] = true
    var dfs func(int, int, int)
    dfs = func(x, sumTime, sumValue int) {
        if x == 0 {
            ans = max(ans, sumValue)
            // 注意这里没有 return，还可以继续走
        }
        for _, e := range g[x] {
            y, t := e.to, e.time
            if sumTime+t > maxTime {
                continue
            }
            if vis[y] {
                dfs(y, sumTime+t, sumValue)
            } else {
                vis[y] = true
                // 每个节点的价值至多算入价值总和中一次
                dfs(y, sumTime+t, sumValue+values[y])
                vis[y] = false // 恢复现场
            }
        }
    }
    dfs(0, 0, values[0])
    return ans
}
```

```js [sol-JavaScript]
var maximalPathQuality = function(values, edges, maxTime) {
    const n = values.length;
    const g = Array.from({length: n}, () => []);
    for (const [x, y, t] of edges) {
        g[x].push([y, t]);
        g[y].push([x, t]);
    }

    let ans = 0;
    const vis = Array(n).fill(false);
    vis[0] = true;
    function dfs(x, sumTime, sumValue) {
        if (x === 0) {
            ans = Math.max(ans, sumValue);
            // 注意这里没有 return，还可以继续走
        }
        for (const [y, t] of g[x]) {
            if (sumTime + t > maxTime) {
                continue;
            }
            if (vis[y]) {
                dfs(y, sumTime + t, sumValue);
            } else {
                vis[y] = true;
                // 每个节点的价值至多算入价值总和中一次
                dfs(y, sumTime + t, sumValue + values[y]);
                vis[y] = false; // 恢复现场
            }
        }
    }
    dfs(0, 0, values[0]);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximal_path_quality(values: Vec<i32>, edges: Vec<Vec<i32>>, max_time: i32) -> i32 {
        let n = values.len();
        let mut g = vec![vec![]; n];
        for e in edges {
            let x = e[0] as usize;
            let y = e[1] as usize;
            let t = e[2];
            g[x].push((y, t));
            g[y].push((x, t));
        }

        fn dfs(x: usize, sum_time: i32, sum_value: i32, vis: &mut Vec<bool>, g: &Vec<Vec<(usize, i32)>>, values: &Vec<i32>, max_time: i32) -> i32 {
            let mut ans = if x == 0 { sum_value } else { 0 };
            for &(y, t) in &g[x] {
                if sum_time + t > max_time {
                    continue;
                }
                if vis[y] {
                    ans = ans.max(dfs(y, sum_time + t, sum_value, vis, g, values, max_time));
                } else {
                    vis[y] = true;
                    // 每个节点的价值至多算入价值总和中一次
                    ans = ans.max(dfs(y, sum_time + t, sum_value + values[y], vis, g, values, max_time));
                    vis[y] = false; // 恢复现场
                }
            }
            ans
        }

        let mut vis = vec![false; n];
        vis[0] = true;
        dfs(0, 0, values[0], &mut vis, &g, &values, max_time)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m+4^k)$，其中 $n$ 是 $\textit{values}$ 的长度，$m$ 是 $\textit{edges}$ 的长度，$k=\dfrac{\textit{maxTime}}{\min(\textit{time}_j)}$。
- 空间复杂度：$\mathcal{O}(n+m+k)$。注意递归会消耗 $\mathcal{O}(k)$ 的栈空间。

## 方法二：暴搜+最短路剪枝

**前置知识**：[Dijkstra 算法介绍](https://leetcode.cn/problems/network-delay-time/solution/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)

用 Dijkstra 算法预处理起点 $0$ 到每个节点的最短路，这也是每个节点到 $0$ 的最短路。

> 由于本题是稀疏图，使用堆优化的 Dijkstra 算法。

在递归到下一个节点之前，判断下一个节点在走最短路的前提下，能否在 $\textit{maxTime}$ 时间内回到起点 $0$，若不能则不递归。

```py [sol-Python3]
class Solution:
    def maximalPathQuality(self, values: List[int], edges: List[List[int]], max_time: int) -> int:
        n = len(values)
        g = [[] for _ in range(n)]
        for x, y, t in edges:
            g[x].append((y, t))
            g[y].append((x, t))

        # Dijkstra 算法
        dis = [inf] * n
        dis[0] = 0
        h = [(0, 0)]
        while h:
            dx, x = heappop(h)
            if dx > dis[x]:  # x 之前出堆过
                continue
            for y, d in g[x]:
                new_dis = dx + d
                if new_dis < dis[y]:
                    dis[y] = new_dis  # 更新 x 的邻居的最短路
                    heappush(h, (new_dis, y))

        def dfs(x: int, sum_time: int, sum_value: int) -> None:
            if x == 0:
                nonlocal ans
                ans = max(ans, sum_value)
                # 注意这里没有 return，还可以继续走
            for y, t in g[x]:
                # 相比方法一，这里多了 dis[y]
                if sum_time + t + dis[y] > max_time:
                    continue
                if vis[y]:
                    dfs(y, sum_time + t, sum_value)
                else:
                    vis[y] = True
                    # 每个节点的价值至多算入价值总和中一次
                    dfs(y, sum_time + t, sum_value + values[y])
                    vis[y] = False  # 恢复现场

        ans = 0
        vis = [False] * n
        vis[0] = True
        dfs(0, 0, values[0])
        return ans
```

```java [sol-Java]
class Solution {
    public int maximalPathQuality(int[] values, int[][] edges, int maxTime) {
        int n = values.length;
        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            int t = e[2];
            g[x].add(new int[]{y, t});
            g[y].add(new int[]{x, t});
        }

        // Dijkstra 算法
        int[] dis = new int[n];
        Arrays.fill(dis, Integer.MAX_VALUE);
        dis[0] = 0;
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> a[0] - b[0]);
        pq.add(new int[]{0, 0});
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
                if (newDis < dis[y]) {
                    dis[y] = newDis; // 更新 x 的邻居的最短路
                    pq.offer(new int[]{newDis, y});
                }
            }
        }

        boolean[] vis = new boolean[n];
        vis[0] = true;
        return dfs(0, 0, values[0], vis, g, values, maxTime, dis);
    }

    private int dfs(int x, int sumTime, int sumValue, boolean[] vis, List<int[]>[] g, int[] values, int maxTime, int[] dis) {
        int res = x == 0 ? sumValue : 0;
        for (int[] e : g[x]) {
            int y = e[0];
            int t = e[1];
            // 相比方法一，这里多了 dis[y]
            if (sumTime + t + dis[y] > maxTime) {
                continue;
            }
            if (vis[y]) {
                res = Math.max(res, dfs(y, sumTime + t, sumValue, vis, g, values, maxTime, dis));
            } else {
                vis[y] = true;
                // 每个节点的价值至多算入价值总和中一次
                res = Math.max(res, dfs(y, sumTime + t, sumValue + values[y], vis, g, values, maxTime, dis));
                vis[y] = false; // 恢复现场
            }
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximalPathQuality(vector<int>& values, vector<vector<int>>& edges, int max_time) {
        int n = values.size();
        vector<vector<pair<int, int>>> g(n);
        for (auto& e : edges) {
            int x = e[0], y = e[1], t = e[2];
            g[x].emplace_back(y, t);
            g[y].emplace_back(x, t);
        }

        // Dijkstra 算法
        vector<int> dis(n, INT_MAX);
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
                if (new_dis < dis[y]) {
                    dis[y] = new_dis; // 更新 x 的邻居的最短路
                    pq.emplace(new_dis, y);
                }
            }
        }

        int ans = 0;
        vector<int> vis(n);
        vis[0] = true;
        auto dfs = [&](auto&& dfs, int x, int sum_time, int sum_value) -> void {
            if (x == 0) {
                ans = max(ans, sum_value);
                // 注意这里没有 return，还可以继续走
            }
            for (auto& [y, t] : g[x]) {
                // 相比方法一，这里多了 dis[y]
                if (sum_time + t + dis[y] > max_time) {
                    continue;
                }
                if (vis[y]) {
                    dfs(dfs, y, sum_time + t, sum_value);
                } else {
                    vis[y] = true;
                    // 每个节点的价值至多算入价值总和中一次
                    dfs(dfs, y, sum_time + t, sum_value + values[y]);
                    vis[y] = false; // 恢复现场
                }
            }
        };
        dfs(dfs, 0, 0, values[0]);
        return ans;
    }
};
```

```go [sol-Go]
func maximalPathQuality(values []int, edges [][]int, maxTime int) (ans int) {
    n := len(values)
    type edge struct{ to, time int }
    g := make([][]edge, n)
    for _, e := range edges {
        x, y, t := e[0], e[1], e[2]
        g[x] = append(g[x], edge{y, t})
        g[y] = append(g[y], edge{x, t})
    }

    // Dijkstra 算法
    dis := make([]int, n)
    for i := 1; i < n; i++ {
        dis[i] = math.MaxInt
    }
    h := hp{{0, 0}}
    for len(h) > 0 {
        p := heap.Pop(&h).(pair)
        dx := p.dis
        x := p.x
        if dx > dis[x] { // x 之前出堆过
            continue
        }
        for _, e := range g[x] {
            y := e.to
            newDis := dx + e.time
            if newDis < dis[y] {
                dis[y] = newDis // 更新 x 的邻居的最短路
                heap.Push(&h, pair{newDis, y})
            }
        }
    }

    vis := make([]bool, n)
    vis[0] = true
    var dfs func(int, int, int)
    dfs = func(x, sumTime, sumValue int) {
        if x == 0 {
            ans = max(ans, sumValue)
            // 注意这里没有 return，还可以继续走
        }
        for _, e := range g[x] {
            y, t := e.to, e.time
            // 相比方法一，这里多了 dis[y]
            if sumTime+t+dis[y] > maxTime {
                continue
            }
            if vis[y] {
                dfs(y, sumTime+t, sumValue)
            } else {
                vis[y] = true
                // 每个节点的价值至多算入价值总和中一次
                dfs(y, sumTime+t, sumValue+values[y])
                vis[y] = false // 恢复现场
            }
        }
    }
    dfs(0, 0, values[0])
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

```js [sol-JavaScript]
var maximalPathQuality = function(values, edges, maxTime) {
    const n = values.length;
    const g = Array.from({ length: n }, () => []);
    for (const [x, y, t] of edges) {
        g[x].push([y, t]);
        g[y].push([x, t]);
    }

    // Dijkstra 算法
    const dis = Array(n).fill(Infinity);
    dis[0] = 0;
    const pq = new MinPriorityQueue(p => p[0]);
    pq.enqueue([0, 0]);
    while (!pq.isEmpty()) {
        const [dx, x] = pq.dequeue();
        if (dx > dis[x]) { // x 之前出堆过
            continue;
        }
        for (const [y, t] of g[x]) {
            const new_dis = dx + t;
            if (new_dis < dis[y]) {
                dis[y] = new_dis; // 更新 x 的邻居的最短路
                pq.enqueue([new_dis, y]);
            }
        }
    }

    let ans = 0;
    const vis = Array(n).fill(false);
    vis[0] = true;
    function dfs(x, sumTime, sumValue) {
        if (x === 0) {
            ans = Math.max(ans, sumValue);
            // 注意这里没有 return，还可以继续走
        }
        for (const [y, t] of g[x]) {
            // 相比方法一，这里多了 dis[y]
            if (sumTime + t + dis[y] > maxTime) {
                continue;
            }
            if (vis[y]) {
                dfs(y, sumTime + t, sumValue);
            } else {
                vis[y] = true;
                // 每个节点的价值至多算入价值总和中一次
                dfs(y, sumTime + t, sumValue + values[y]);
                vis[y] = false; // 恢复现场
            }
        }
    }
    dfs(0, 0, values[0]);
    return ans;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn maximal_path_quality(values: Vec<i32>, edges: Vec<Vec<i32>>, max_time: i32) -> i32 {
        let n = values.len();
        let mut g = vec![vec![]; n];
        for e in edges {
            let x = e[0] as usize;
            let y = e[1] as usize;
            let t = e[2];
            g[x].push((y, t));
            g[y].push((x, t));
        }

        // Dijkstra 算法
        let mut dis = vec![i32::MAX; n];
        dis[0] = 0;
        let mut h = BinaryHeap::new();
        h.push((0, 0));
        while let Some((dx, x)) = h.pop() {
            if -dx > dis[x] { // x 之前出堆过
                continue;
            }
            for &(y, t) in &g[x] {
                let new_dis = -dx + t;
                if new_dis < dis[y] {
                    dis[y] = new_dis; // 更新 x 的邻居的最短路
                    h.push((-new_dis, y));
                }
            }
        }

        fn dfs(x: usize, sum_time: i32, sum_value: i32, vis: &mut Vec<bool>, g: &Vec<Vec<(usize, i32)>>, values: &Vec<i32>, max_time: i32, dis: &Vec<i32>) -> i32 {
            let mut ans = if x == 0 { sum_value } else { 0 };
            for &(y, t) in &g[x] {
                // 相比方法一，这里多了 dis[y]
                if sum_time + t + dis[y] > max_time {
                    continue;
                }
                if vis[y] {
                    ans = ans.max(dfs(y, sum_time + t, sum_value, vis, g, values, max_time, dis));
                } else {
                    vis[y] = true;
                    // 每个节点的价值至多算入价值总和中一次
                    ans = ans.max(dfs(y, sum_time + t, sum_value + values[y], vis, g, values, max_time, dis));
                    vis[y] = false; // 恢复现场
                }
            }
            ans
        }

        let mut vis = vec![false; n];
        vis[0] = true;
        dfs(0, 0, values[0], &mut vis, &g, &values, max_time, &dis)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m\log m+4^k)$，其中 $n$ 是 $\textit{values}$ 的长度，$m$ 是 $\textit{edges}$ 的长度，$k=\dfrac{\textit{maxTime}}{\min(\textit{time}_j)}$。Dijkstra 算法的时间复杂度为 $\mathcal{O}(n + m\log m)$。
- 空间复杂度：$\mathcal{O}(n+m+k)$。注意递归会消耗 $\mathcal{O}(k)$ 的栈空间。

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

本题来自 `九、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
