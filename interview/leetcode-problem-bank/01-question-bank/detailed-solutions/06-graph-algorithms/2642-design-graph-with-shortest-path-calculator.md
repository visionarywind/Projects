# 2642. 设计可以求最短路径的图类

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/design-graph-with-shortest-path-calculator/
- 题目 slug：`design-graph-with-shortest-path-calculator`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：1811
- 外部题解来源：https://leetcode.cn/problems/design-graph-with-shortest-path-calculator/solutions/2229013/dijkstra-suan-fa-mo-ban-pythonjavacgo-by-unmv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：Dijkstra / Floyd（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/design-graph-with-shortest-path-calculator/solutions/2229013/dijkstra-suan-fa-mo-ban-pythonjavacgo-by-unmv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dijkstra-suan-fa-mo-ban-pythonjavacgo-by-unmv`
- topic id：`2229013`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：Dijkstra 算法

请先阅读 [Dijkstra 算法介绍](https://leetcode.cn/problems/network-delay-time/solution/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)。

- 对于 $\texttt{addEdge}$，直接把边加入图中即可。
- 对于 $\texttt{shortestPath}$，用 Dijkstra 算法计算从起点 $\textit{start}$ 到终点 $\textit{end}$ 的最短路长度。

#### 写法一：邻接矩阵建图 + 朴素 Dijkstra

```py [sol-Python3]
class Graph:
    def __init__(self, n: int, edges: List[List[int]]):
        self.g = [[inf] * n for _ in range(n)]  # 邻接矩阵
        for x, y, w in edges:
            self.g[x][y] = w  # 添加一条边（题目保证没有重边）

    def addEdge(self, e: List[int]) -> None:
        self.g[e[0]][e[1]] = e[2]  # 添加一条边（题目保证这条边之前不存在）

    def shortestPath(self, start: int, end: int) -> int:
        n = len(self.g)
        dis = [inf] * n  # 从 start 出发，到各个点的最短路，如果不存在则为无穷大
        dis[start] = 0
        vis = [False] * n
        while True:  # 至多循环 n 次
            x = -1
            for i, (b, d) in enumerate(zip(vis, dis)):
                if not b and (x < 0 or d < dis[x]):
                    x = i
            if x < 0 or dis[x] == inf:  # 所有从 start 能到达的点都被更新了
                return -1  # 无法到达终点
            if x == end:  # 找到终点，提前退出
                return dis[x]
            vis[x] = True  # 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
            for y, w in enumerate(self.g[x]):
                if dis[x] + w < dis[y]:
                    dis[y] = dis[x] + w  # 更新最短路长度
```

```java [sol-Java]
class Graph {
    private static final int INF = Integer.MAX_VALUE / 2; // 防止更新最短路时加法溢出

    private final int[][] g;

    public Graph(int n, int[][] edges) {
        g = new int[n][n]; // 邻接矩阵
        for (int[] row : g) {
            Arrays.fill(row, INF);
        }
        for (int[] e : edges) {
            addEdge(e); // 添加一条边（题目保证没有重边）
        }
    }

    public void addEdge(int[] e) {
        g[e[0]][e[1]] = e[2]; // 添加一条边（题目保证这条边之前不存在）
    }

    public int shortestPath(int start, int end) {
        int n = g.length;
        int[] dis = new int[n]; // 从 start 出发，到各个点的最短路，如果不存在则为无穷大
        Arrays.fill(dis, INF);
        dis[start] = 0;
        boolean[] vis = new boolean[n];
        while (true) {
            int x = -1;
            for (int i = 0; i < n; i++) {
                if (!vis[i] && (x < 0 || dis[i] < dis[x])) {
                    x = i;
                }
            }
            if (x < 0 || dis[x] == INF) {// 所有从 start 能到达的点都被更新了
                return -1;
            }
            if (x == end) {// 找到终点，提前退出
                return dis[x];
            }
            vis[x] = true; // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
            for (int y = 0; y < n; y++) {
                dis[y] = Math.min(dis[y], dis[x] + g[x][y]); // 更新最短路长度
            }
        }
    }
}
```

```cpp [sol-C++]
class Graph {
    vector<vector<int>> g; // 邻接矩阵
public:
    Graph(int n, vector<vector<int>> &edges) : g(n, vector<int>(n, INT_MAX / 2)) {
        for (auto &e: edges) {
            g[e[0]][e[1]] = e[2]; // 添加一条边（题目保证没有重边）
        }
    }

    void addEdge(vector<int> e) {
        g[e[0]][e[1]] = e[2]; // 添加一条边（题目保证这条边之前不存在）
    }

    int shortestPath(int start, int end) {
        int n = g.size();
        vector<int> dis(n, INT_MAX / 2), vis(n);
        dis[start] = 0;
        while (true) {
            int x = -1;
            for (int i = 0; i < n; i++) {
                if (!vis[i] && (x < 0 || dis[i] < dis[x])) {
                    x = i;
                }
            }
            if (x < 0 || dis[x] == INT_MAX / 2) { // 所有从 start 能到达的点都被更新了
                return -1;
            }
            if (x == end) { // 找到终点，提前退出
                return dis[x];
            }
            vis[x] = true; // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
            for (int y = 0; y < n; y++) {
                dis[y] = min(dis[y], dis[x] + g[x][y]); // 更新最短路长度
            }
        }
    }
};
```

```go [sol-Go]
const inf = math.MaxInt / 2 // 防止更新最短路时加法溢出

type Graph [][]int

func Constructor(n int, edges [][]int) Graph {
	g := make([][]int, n) // 邻接矩阵
	for i := range g {
		g[i] = make([]int, n)
		for j := range g[i] {
			g[i][j] = inf // 初始化为无穷大，表示 i 到 j 没有边
		}
	}
	for _, e := range edges {
		g[e[0]][e[1]] = e[2] // 添加一条边（题目保证没有重边）
	}
	return g
}

func (g Graph) AddEdge(e []int) {
	g[e[0]][e[1]] = e[2] // 添加一条边（题目保证这条边之前不存在）
}

func (g Graph) ShortestPath(start, end int) int {
	n := len(g)
	dis := make([]int, n) // 从 start 出发，到各个点的最短路，如果不存在则为无穷大
	for i := range dis {
		dis[i] = inf
	}
	dis[start] = 0
	vis := make([]bool, n)
	for {
		x := -1
		for i, b := range vis {
			if !b && (x < 0 || dis[i] < dis[x]) {
				x = i
			}
		}
		if x < 0 || dis[x] == inf { // 所有从 start 能到达的点都被更新了
			return -1
		}
		if x == end { // 找到终点，提前退出
			return dis[x]
		}
		vis[x] = true // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
		for y, w := range g[x] {
			dis[y] = min(dis[y], dis[x]+w) // 更新最短路长度
		}
	}
}
```

```js [sol-JavaScript]
var Graph = function(n, edges) {
    const g = Array.from({length: n}, () => Array(n).fill(Infinity)); // 邻接矩阵
    for (const [x, y, w] of edges) {
        g[x][y] = w; // 添加一条边（题目保证没有重边）
    }

    this.addEdge = function(e) {
        g[e[0]][e[1]] = e[2]; // 添加一条边（题目保证这条边之前不存在）
    }

    this.shortestPath = function(start, end) {
        // dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        const dis = Array(n).fill(Infinity);
        dis[start] = 0;
        const vis = Array(n).fill(false);
        while (true) {
            let x = -1;
            for (let i = 0; i < n; i++) {
                if (!vis[i] && (x < 0 || dis[i] < dis[x])) {
                    x = i;
                }
            }
            if (x < 0 || dis[x] === Infinity) { // 所有从起点能到达的点都被更新了
                return -1; // 无法到达终点
            }
            if (x === end) { // 找到终点，提前退出
                return dis[x];
            }
            vis[x] = true; // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
            for (let y = 0; y < n; y++) {
                dis[y] = Math.min(dis[y], dis[x] + g[x][y]); // 更新最短路长度
            }
        }
    }
};
```

```rust [sol-Rust]
struct Graph {
    g: Vec<Vec<i32>>, // 邻接矩阵
}

impl Graph {
    fn new(n: i32, edges: Vec<Vec<i32>>) -> Self {
        let n = n as usize;
        let mut g = vec![vec![i32::MAX / 2; n]; n];
        for e in &edges {
            g[e[0] as usize][e[1] as usize] = e[2]; // 添加一条边（题目保证没有重边）
        }
        Self { g }
    }

    fn add_edge(&mut self, e: Vec<i32>) {
        self.g[e[0] as usize][e[1] as usize] = e[2]; // 添加一条边（题目保证这条边之前不存在）
    }

    fn shortest_path(&self, node1: i32, node2: i32) -> i32 {
        let n = self.g.len();
        let start = node1 as usize;
        let end = node2 as usize;
        // dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        let mut dis = vec![i32::MAX / 2; self.g.len()];
        dis[start] = 0;
        let mut vis = vec![false; n];
        loop {
            let mut x = n;
            for i in 0..n {
                if !vis[i] && (x == n || dis[i] < dis[x]) {
                    x = i;
                }
            }
            if x == n || dis[x] == i32::MAX / 2 { // 所有从 start 能到达的点都被更新了
                return -1; // 无法到达终点
            }
            if x == end { // 找到终点，提前退出
                return dis[x];
            }
            vis[x] = true; // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
            for y in 0..n {
                dis[y] = dis[y].min(dis[x] + self.g[x][y]); // 更新最短路长度
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：
  - 初始化 $\mathcal{O}(n^2)$。
  - $\texttt{addEdge}$ $\mathcal{O}(1)$。
  - $\texttt{shortestPath}$ $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

#### 写法二：邻接表建图 + 堆优化 Dijkstra

```py [sol-Python3]
class Graph:
    def __init__(self, n: int, edges: List[List[int]]):
        self.g = [[] for _ in range(n)]  # 邻接表
        for x, y, w in edges:
            self.g[x].append((y, w))

    def addEdge(self, e: List[int]) -> None:
        self.g[e[0]].append((e[1], e[2]))

    def shortestPath(self, start: int, end: int) -> int:
        dis = [inf] * len(self.g)  # dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        dis[start] = 0
        h = [(0, start)]
        while h:
            d, x = heappop(h)
            if x == end:  # 计算出从起点到终点的最短路长度
                return d
            if d > dis[x]:  # x 之前出堆过，无需更新邻居的最短路
                continue
            for y, w in self.g[x]:
                if d + w < dis[y]:
                    dis[y] = d + w  # 更新最短路长度
                    heappush(h, (dis[y], y))
        return -1  # 无法到达终点
```

```java [sol-Java]
class Graph {
    private final List<int[]>[] g; // 邻接表

    public Graph(int n, int[][] edges) {
        g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            addEdge(e);
        }
    }

    public void addEdge(int[] e) {
        g[e[0]].add(new int[]{e[1], e[2]});
    }

    public int shortestPath(int start, int end) {
        int[] dis = new int[g.length]; // dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        Arrays.fill(dis, Integer.MAX_VALUE);
        dis[start] = 0;
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> (a[0] - b[0]));
        pq.offer(new int[]{0, start});
        while (!pq.isEmpty()) {
            int[] p = pq.poll();
            int d = p[0];
            int x = p[1];
            if (x == end) { // 计算出从起点到终点的最短路长度
                return d;
            }
            if (d > dis[x]) { // x 之前出堆过，无需更新邻居的最短路
                continue;
            }
            for (int[] e : g[x]) {
                int y = e[0];
                int w = e[1];
                if (d + w < dis[y]) {
                    dis[y] = d + w; // 更新最短路长度
                    pq.offer(new int[]{dis[y], y});
                }
            }
        }
        return -1; // 无法到达终点
    }
}
```

```cpp [sol-C++]
class Graph {
    vector<vector<pair<int, int>>> g; // 邻接表
public:
    Graph(int n, vector<vector<int>> &edges) : g(n) {
        for (auto &e : edges) {
            g[e[0]].emplace_back(e[1], e[2]);
        }
    }

    void addEdge(vector<int> e) {
        g[e[0]].emplace_back(e[1], e[2]);
    }

    int shortestPath(int start, int end) {
        // dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        vector<int> dis(g.size(), INT_MAX);
        dis[start] = 0;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        pq.emplace(0, start);
        while (!pq.empty()) {
            auto [d, x] = pq.top();
            pq.pop();
            if (x == end) { // 计算出从起点到终点的最短路长度
                return d;
            }
            if (d > dis[x]) { // x 之前出堆过，无需更新邻居的最短路
                continue;
            }
            for (auto &[y, w] : g[x]) {
                if (d + w < dis[y]) {
                    dis[y] = d + w; // 更新最短路长度
                    pq.push({dis[y], y});
                }
            }
        }
        return -1; // 无法到达终点
    }
};
```

```go [sol-Go]
type Graph [][]pair

func Constructor(n int, edges [][]int) Graph {
	g := make(Graph, n) // 邻接表
	for _, e := range edges {
		g[e[0]] = append(g[e[0]], pair{e[1], e[2]})
	}
	return g
}

func (g Graph) AddEdge(e []int) {
	g[e[0]] = append(g[e[0]], pair{e[1], e[2]})
}

func (g Graph) ShortestPath(start, end int) int {
	// dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
	dis := make([]int, len(g))
	for i := range dis {
		dis[i] = math.MaxInt
	}
	dis[start] = 0
	h := hp{{start, 0}}
	for len(h) > 0 {
		p := heap.Pop(&h).(pair)
		x, d := p.x, p.d
		if x == end { // 计算出从起点到终点的最短路长度
			return d
		}
		if d > dis[x] { // x 之前出堆过，无需更新邻居的最短路
			continue
		}
		for _, e := range g[x] {
			y, w := e.x, e.d
			newD := d + w
			if newD < dis[y] {
				dis[y] = newD // 更新最短路长度
				heap.Push(&h, pair{y, newD})
			}
		}
	}
	return -1 // 无法到达终点
}

type pair struct{ x, d int }
type hp []pair
func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].d < h[j].d }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(pair)) }
func (h *hp) Pop() (v any)      { a := *h; *h, v = a[:len(a)-1], a[len(a)-1]; return }
```

```js [sol-JavaScript]
var Graph = function(n, edges) {
    const g = Array.from({length: n}, () => []); // 邻接表
    for (const [x, y, w] of edges) {
        g[x].push([y, w]);
    }

    this.addEdge = function(e) {
        g[e[0]].push([e[1], e[2]]);
    }

    this.shortestPath = function(start, end) {
        // dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        const dis = Array(n).fill(Infinity);
        const pq = new MinPriorityQueue(p => p[0]);
        dis[start] = 0;
        pq.enqueue([0, start]);
        while (!pq.isEmpty()) {
            const [d, x] = pq.dequeue();
            if (x === end) { // 计算出从起点到终点的最短路长度
                return d;
            }
            if (d > dis[x]) { // x 之前出堆过，无需更新邻居的最短路
                continue;
            }
            for (const [y, w] of g[x]) {
                if (d + w < dis[y]) {
                    dis[y] = d + w; // 更新最短路长度
                    pq.enqueue([dis[y], y]);
                }
            }
        }
        return -1; // 无法到达终点
    }
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

struct Graph {
    g: Vec<Vec<(usize, i32)>>, // 邻接表
}

impl Graph {
    fn new(n: i32, edges: Vec<Vec<i32>>) -> Self {
        let mut g = vec![vec![]; n as usize];
        for e in &edges {
            g[e[0] as usize].push((e[1] as usize, e[2]));
        }
        Self { g }
    }

    fn add_edge(&mut self, e: Vec<i32>) {
        self.g[e[0] as usize].push((e[1] as usize, e[2]));
    }

    fn shortest_path(&self, node1: i32, node2: i32) -> i32 {
        let start = node1 as usize;
        let end = node2 as usize;
        // dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        let mut dis = vec![i32::MAX; self.g.len()];
        dis[start] = 0;
        let mut h = BinaryHeap::new();
        h.push((0, start));
        while let Some((d, x)) = h.pop() {
            let d = -d;
            if x == end { // 计算出从起点到终点的最短路长度
                return d;
            }
            if d > dis[x] { // x 之前出堆过，无需更新邻居的最短路
                continue;
            }
            for &(y, w) in &self.g[x] {
                if d + w < dis[y] {
                    dis[y] = d + w; // 更新最短路长度
                    h.push((-dis[y], y)); // 加负号变最小堆
                }
            }
        }
        -1 // 无法到达终点
    }
}
```

#### 复杂度分析

- 时间复杂度：
    - 初始化 $\mathcal{O}(n+m)$。其中 $m$ 为 $\textit{edges}$ 的长度。
    - $\texttt{addEdge}$ $\mathcal{O}(1)$。
    - $\texttt{shortestPath}$ $\mathcal{O}(n + m\log m)$。其中 $m$ 为调用 $\texttt{shortestPath}$ 时，图的边数。当图是稠密图时，复杂度为 $\mathcal{O}(n^2\log n)$。
- 空间复杂度：$\mathcal{O}(n+m)$。其中 $m$ 为 $\textit{edges}$ 的长度加上 $\texttt{addEdge}$ 的调用次数。

## 方法二：Floyd 算法

请先阅读 [带你发明 Floyd 算法：从记忆化搜索到递推](https://leetcode.cn/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/solution/dai-ni-fa-ming-floyd-suan-fa-cong-ji-yi-m8s51/)。

对于 $\texttt{addEdge}$，记 $x=\textit{from},y=\textit{to}$。如果 $\textit{edgeCost} \ge f[x][y]$，则无法更新任何点对的最短路。否则枚举所有 $f[i][j]$，尝试看看能否更新成更小，即从 $i$ 到 $j$ 的如下路径

$$
i---x-y---j
$$

是否更短，写成式子就是

$$
f[i][j] = \min(f[i][j], f[i][x] + \textit{edgeCost} + f[y][j])
$$

**注意**：当 $i=x$ 或 $j=y$ 时，我们需要用到 $f[i][i]$ 这样的值，所以初始化的时候，$f[i][i]$ 要置为 $0$。

对于 $\texttt{shortestPath}$，返回 $f[\textit{start}][\textit{end}]$ 即可。

#### 答疑

**问**：在 $\texttt{addEdge}$ 中，上式 $f[i][j]$ 的计算依赖于 $f[i][x]$ 的值，如果先计算 $f[i][j]$，再计算 $f[i][x]$，我们还需要重新计算 $f[i][j]$ 吗？

**答**：没有必要，每个 $f[i][j]$ 只需要计算一次。如果 $f[i][x]$ 因为 $x-y$ 这条边变小，说明从 $i$ 到 $x$ 的最短路包含 $x-y$ 这条边。那么对于 $f[i][j]$，最短路不可能是 $i---x-y---j$，这意味着最短路会经过 $x-y$ 这条边**两次**。所以重新计算 $f[i][j]$ 是不可能让 $f[i][j]$ 变小的。

```py [sol-Python3]
class Graph:
    def __init__(self, n: int, edges: List[List[int]]):
        f = [[inf] * n for _ in range(n)]
        for i in range(n):
            f[i][i] = 0
        for x, y, w in edges:
            f[x][y] = w  # 添加一条边（题目保证没有重边和自环）
        for k in range(n):
            for i in range(n):
                if f[i][k] == inf: continue
                for j in range(n):
                    f[i][j] = min(f[i][j], f[i][k] + f[k][j])
        self.f = f

    def addEdge(self, edge: List[int]) -> None:
        f = self.f
        x, y, w = edge
        if w >= f[x][y]:  # 无需更新
            return
        n = len(f)
        for i in range(n):
            for j in range(n):
                f[i][j] = min(f[i][j], f[i][x] + w + f[y][j])

    def shortestPath(self, start: int, end: int) -> int:
        ans = self.f[start][end]
        return ans if ans < inf else -1
```

```java [sol-Java]
class Graph {
    private static final int INF = Integer.MAX_VALUE / 3; // 防止更新最短路时加法溢出

    private final int[][] f;

    public Graph(int n, int[][] edges) {
        f = new int[n][n];
        for (int i = 0; i < n; i++) {
            Arrays.fill(f[i], INF);
            f[i][i] = 0;
        }
        for (int[] e : edges) {
            f[e[0]][e[1]] = e[2]; // 添加一条边（题目保证没有重边和自环）
        }
        for (int k = 0; k < n; k++) {
            for (int i = 0; i < n; i++) {
                if (f[i][k] == INF) {
                    continue;
                }
                for (int j = 0; j < n; j++) {
                    f[i][j] = Math.min(f[i][j], f[i][k] + f[k][j]);
                }
            }
        }
    }

    public void addEdge(int[] e) {
        int x = e[0], y = e[1], w = e[2], n = f.length;
        if (w >= f[x][y]) { // 无需更新
            return;
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                f[i][j] = Math.min(f[i][j], f[i][x] + w + f[y][j]);
            }
        }
    }

    public int shortestPath(int start, int end) {
        int ans = f[start][end];
        return ans < INF ? ans : -1;
    }
}
```

```cpp [sol-C++]
class Graph {
    const int INF = INT_MAX / 3;
    vector<vector<int>> f;
public:
    Graph(int n, vector<vector<int>> &edges) : f(n, vector<int>(n, INF)) {
        for (int i = 0; i < n; i++) {
            f[i][i] = 0;
        }
        for (auto &e : edges) {
            f[e[0]][e[1]] = e[2]; // 添加一条边（题目保证没有重边和自环）
        }
        for (int k = 0; k < n; k++) {
            for (int i = 0; i < n; i++) {
                if (f[i][k] == INF) continue;
                for (int j = 0; j < n; j++) {
                    f[i][j] = min(f[i][j], f[i][k] + f[k][j]);
                }
            }
        }
    }

    void addEdge(vector<int> e) {
        int x = e[0], y = e[1], w = e[2], n = f.size();
        if (w >= f[x][y]) { // 无需更新
            return;
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                f[i][j] = min(f[i][j], f[i][x] + w + f[y][j]);
            }
        }
    }

    int shortestPath(int start, int end) {
        int ans = f[start][end];
        return ans < INF ? ans : -1;
    }
};
```

```go [sol-Go]
const inf = math.MaxInt / 3 // 防止更新最短路时加法溢出

type Graph [][]int

func Constructor(n int, edges [][]int) Graph {
	f := make(Graph, n) // 邻接矩阵
	for i := range f {
		f[i] = make([]int, n)
		for j := range f[i] {
			if j != i {
				f[i][j] = inf
			}
		}
	}
	for _, e := range edges {
		f[e[0]][e[1]] = e[2] // 添加一条边（题目保证没有重边和自环）
	}
	for k := range f {
		for i := range f {
			if f[i][k] == inf {
				continue
			}
			for j := range f {
				f[i][j] = min(f[i][j], f[i][k]+f[k][j])
			}
		}
	}
	return f
}

func (f Graph) AddEdge(e []int) {
	x, y, w := e[0], e[1], e[2]
	if w >= f[x][y] { // 无需更新
		return
	}
	for i := range f {
		for j := range f {
			f[i][j] = min(f[i][j], f[i][x]+w+f[y][j])
		}
	}
}

func (f Graph) ShortestPath(start, end int) int {
	ans := f[start][end]
	if ans == inf {
		return -1
	}
	return ans
}
```

```js [sol-JavaScript]
var Graph = function(n, edges) {
    const f = Array.from({length: n}, () => Array(n).fill(Infinity));
    for (let i = 0; i < n; i++) {
        f[i][i] = 0;
    }
    for (const [x, y, w] of edges) {
        f[x][y] = w; // 添加一条边（题目保证没有重边和自环）
    }
    for (let k = 0; k < n; k++) {
        for (let i = 0; i < n; i++) {
            if (f[i][k] === Infinity) continue;
            for (let j = 0; j < n; j++) {
                f[i][j] = Math.min(f[i][j], f[i][k] + f[k][j]);
            }
        }
    }

    this.addEdge = function(e) {
        const [x, y, w] = e;
        if (w >= f[x][y]) { // 无需更新
            return;
        }
        for (let i = 0; i < n; i++) {
            for (let j = 0; j < n; j++) {
                f[i][j] = Math.min(f[i][j], f[i][x] + w + f[y][j]);
            }
        }
    }

    this.shortestPath = function(start, end) {
        const ans = f[start][end];
        return ans < Infinity ? ans : -1;
    }
};
```

```rust [sol-Rust]
struct Graph {
    f: Vec<Vec<i32>>,
}

impl Graph {
    fn new(n: i32, edges: Vec<Vec<i32>>) -> Self {
        const INF: i32 = i32::MAX / 3; // 防止更新最短路时加法溢出
        let n = n as usize;
        let mut f = vec![vec![INF; n]; n];
        for i in 0..n {
            f[i][i] = 0;
        }
        for e in edges {
            f[e[0] as usize][e[1] as usize] = e[2]; // 添加一条边（题目保证没有重边和自环）
        }
        for k in 0..n {
            for i in 0..n {
                if f[i][k] == INF {
                    continue;
                }
                for j in 0..n {
                    f[i][j] = f[i][j].min(f[i][k] + f[k][j]);
                }
            }
        }
        Self { f }
    }

    fn add_edge(&mut self, e: Vec<i32>) {
        let x = e[0] as usize;
        let y = e[1] as usize;
        let w = e[2];
        if w >= self.f[x][y] { // 无需更新
            return;
        }
        let n = self.f.len();
        for i in 0..n {
            for j in 0..n {
                self.f[i][j] = self.f[i][j].min(self.f[i][x] + w + self.f[y][j]);
            }
        }
    }

    fn shortest_path(&self, start: i32, end: i32) -> i32 {
        let ans = self.f[start as usize][end as usize];
        if ans < i32::MAX / 3 { ans } else { -1 }
    }
}
```

#### 复杂度分析

- 时间复杂度：
    - 初始化 $\mathcal{O}(n^3)$。
    - $\texttt{addEdge}$ $\mathcal{O}(n^2)$。
    - $\texttt{shortestPath}$ $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 总结

方法一适用于调用 $\texttt{addEdge}$ 比较多的场景，方法二适用于调用 $\texttt{shortestPath}$ 比较多的场景。

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

## 方法一：Dijkstra 算法

请先阅读 [Dijkstra 算法介绍](https://leetcode.cn/problems/network-delay-time/solution/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)。

- 对于 $\texttt{addEdge}$，直接把边加入图中即可。
- 对于 $\texttt{shortestPath}$，用 Dijkstra 算法计算从起点 $\textit{start}$ 到终点 $\textit{end}$ 的最短路长度。

#### 写法一：邻接矩阵建图 + 朴素 Dijkstra

```py [sol-Python3]
class Graph:
    def __init__(self, n: int, edges: List[List[int]]):
        self.g = [[inf] * n for _ in range(n)]  # 邻接矩阵
        for x, y, w in edges:
            self.g[x][y] = w  # 添加一条边（题目保证没有重边）

    def addEdge(self, e: List[int]) -> None:
        self.g[e[0]][e[1]] = e[2]  # 添加一条边（题目保证这条边之前不存在）

    def shortestPath(self, start: int, end: int) -> int:
        n = len(self.g)
        dis = [inf] * n  # 从 start 出发，到各个点的最短路，如果不存在则为无穷大
        dis[start] = 0
        vis = [False] * n
        while True:  # 至多循环 n 次
            x = -1
            for i, (b, d) in enumerate(zip(vis, dis)):
                if not b and (x < 0 or d < dis[x]):
                    x = i
            if x < 0 or dis[x] == inf:  # 所有从 start 能到达的点都被更新了
                return -1  # 无法到达终点
            if x == end:  # 找到终点，提前退出
                return dis[x]
            vis[x] = True  # 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
            for y, w in enumerate(self.g[x]):
                if dis[x] + w < dis[y]:
                    dis[y] = dis[x] + w  # 更新最短路长度
```

```java [sol-Java]
class Graph {
    private static final int INF = Integer.MAX_VALUE / 2; // 防止更新最短路时加法溢出

    private final int[][] g;

    public Graph(int n, int[][] edges) {
        g = new int[n][n]; // 邻接矩阵
        for (int[] row : g) {
            Arrays.fill(row, INF);
        }
        for (int[] e : edges) {
            addEdge(e); // 添加一条边（题目保证没有重边）
        }
    }

    public void addEdge(int[] e) {
        g[e[0]][e[1]] = e[2]; // 添加一条边（题目保证这条边之前不存在）
    }

    public int shortestPath(int start, int end) {
        int n = g.length;
        int[] dis = new int[n]; // 从 start 出发，到各个点的最短路，如果不存在则为无穷大
        Arrays.fill(dis, INF);
        dis[start] = 0;
        boolean[] vis = new boolean[n];
        while (true) {
            int x = -1;
            for (int i = 0; i < n; i++) {
                if (!vis[i] && (x < 0 || dis[i] < dis[x])) {
                    x = i;
                }
            }
            if (x < 0 || dis[x] == INF) {// 所有从 start 能到达的点都被更新了
                return -1;
            }
            if (x == end) {// 找到终点，提前退出
                return dis[x];
            }
            vis[x] = true; // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
            for (int y = 0; y < n; y++) {
                dis[y] = Math.min(dis[y], dis[x] + g[x][y]); // 更新最短路长度
            }
        }
    }
}
```

```cpp [sol-C++]
class Graph {
    vector<vector<int>> g; // 邻接矩阵
public:
    Graph(int n, vector<vector<int>> &edges) : g(n, vector<int>(n, INT_MAX / 2)) {
        for (auto &e: edges) {
            g[e[0]][e[1]] = e[2]; // 添加一条边（题目保证没有重边）
        }
    }

    void addEdge(vector<int> e) {
        g[e[0]][e[1]] = e[2]; // 添加一条边（题目保证这条边之前不存在）
    }

    int shortestPath(int start, int end) {
        int n = g.size();
        vector<int> dis(n, INT_MAX / 2), vis(n);
        dis[start] = 0;
        while (true) {
            int x = -1;
            for (int i = 0; i < n; i++) {
                if (!vis[i] && (x < 0 || dis[i] < dis[x])) {
                    x = i;
                }
            }
            if (x < 0 || dis[x] == INT_MAX / 2) { // 所有从 start 能到达的点都被更新了
                return -1;
            }
            if (x == end) { // 找到终点，提前退出
                return dis[x];
            }
            vis[x] = true; // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
            for (int y = 0; y < n; y++) {
                dis[y] = min(dis[y], dis[x] + g[x][y]); // 更新最短路长度
            }
        }
    }
};
```

```go [sol-Go]
const inf = math.MaxInt / 2 // 防止更新最短路时加法溢出

type Graph [][]int

func Constructor(n int, edges [][]int) Graph {
	g := make([][]int, n) // 邻接矩阵
	for i := range g {
		g[i] = make([]int, n)
		for j := range g[i] {
			g[i][j] = inf // 初始化为无穷大，表示 i 到 j 没有边
		}
	}
	for _, e := range edges {
		g[e[0]][e[1]] = e[2] // 添加一条边（题目保证没有重边）
	}
	return g
}

func (g Graph) AddEdge(e []int) {
	g[e[0]][e[1]] = e[2] // 添加一条边（题目保证这条边之前不存在）
}

func (g Graph) ShortestPath(start, end int) int {
	n := len(g)
	dis := make([]int, n) // 从 start 出发，到各个点的最短路，如果不存在则为无穷大
	for i := range dis {
		dis[i] = inf
	}
	dis[start] = 0
	vis := make([]bool, n)
	for {
		x := -1
		for i, b := range vis {
			if !b && (x < 0 || dis[i] < dis[x]) {
				x = i
			}
		}
		if x < 0 || dis[x] == inf { // 所有从 start 能到达的点都被更新了
			return -1
		}
		if x == end { // 找到终点，提前退出
			return dis[x]
		}
		vis[x] = true // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
		for y, w := range g[x] {
			dis[y] = min(dis[y], dis[x]+w) // 更新最短路长度
		}
	}
}
```

```js [sol-JavaScript]
var Graph = function(n, edges) {
    const g = Array.from({length: n}, () => Array(n).fill(Infinity)); // 邻接矩阵
    for (const [x, y, w] of edges) {
        g[x][y] = w; // 添加一条边（题目保证没有重边）
    }

    this.addEdge = function(e) {
        g[e[0]][e[1]] = e[2]; // 添加一条边（题目保证这条边之前不存在）
    }

    this.shortestPath = function(start, end) {
        // dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        const dis = Array(n).fill(Infinity);
        dis[start] = 0;
        const vis = Array(n).fill(false);
        while (true) {
            let x = -1;
            for (let i = 0; i < n; i++) {
                if (!vis[i] && (x < 0 || dis[i] < dis[x])) {
                    x = i;
                }
            }
            if (x < 0 || dis[x] === Infinity) { // 所有从起点能到达的点都被更新了
                return -1; // 无法到达终点
            }
            if (x === end) { // 找到终点，提前退出
                return dis[x];
            }
            vis[x] = true; // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
            for (let y = 0; y < n; y++) {
                dis[y] = Math.min(dis[y], dis[x] + g[x][y]); // 更新最短路长度
            }
        }
    }
};
```

```rust [sol-Rust]
struct Graph {
    g: Vec<Vec<i32>>, // 邻接矩阵
}

impl Graph {
    fn new(n: i32, edges: Vec<Vec<i32>>) -> Self {
        let n = n as usize;
        let mut g = vec![vec![i32::MAX / 2; n]; n];
        for e in &edges {
            g[e[0] as usize][e[1] as usize] = e[2]; // 添加一条边（题目保证没有重边）
        }
        Self { g }
    }

    fn add_edge(&mut self, e: Vec<i32>) {
        self.g[e[0] as usize][e[1] as usize] = e[2]; // 添加一条边（题目保证这条边之前不存在）
    }

    fn shortest_path(&self, node1: i32, node2: i32) -> i32 {
        let n = self.g.len();
        let start = node1 as usize;
        let end = node2 as usize;
        // dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        let mut dis = vec![i32::MAX / 2; self.g.len()];
        dis[start] = 0;
        let mut vis = vec![false; n];
        loop {
            let mut x = n;
            for i in 0..n {
                if !vis[i] && (x == n || dis[i] < dis[x]) {
                    x = i;
                }
            }
            if x == n || dis[x] == i32::MAX / 2 { // 所有从 start 能到达的点都被更新了
                return -1; // 无法到达终点
            }
            if x == end { // 找到终点，提前退出
                return dis[x];
            }
            vis[x] = true; // 标记，在后续的循环中无需反复更新 x 到其余点的最短路长度
            for y in 0..n {
                dis[y] = dis[y].min(dis[x] + self.g[x][y]); // 更新最短路长度
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：
  - 初始化 $\mathcal{O}(n^2)$。
  - $\texttt{addEdge}$ $\mathcal{O}(1)$。
  - $\texttt{shortestPath}$ $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

#### 写法二：邻接表建图 + 堆优化 Dijkstra

```py [sol-Python3]
class Graph:
    def __init__(self, n: int, edges: List[List[int]]):
        self.g = [[] for _ in range(n)]  # 邻接表
        for x, y, w in edges:
            self.g[x].append((y, w))

    def addEdge(self, e: List[int]) -> None:
        self.g[e[0]].append((e[1], e[2]))

    def shortestPath(self, start: int, end: int) -> int:
        dis = [inf] * len(self.g)  # dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        dis[start] = 0
        h = [(0, start)]
        while h:
            d, x = heappop(h)
            if x == end:  # 计算出从起点到终点的最短路长度
                return d
            if d > dis[x]:  # x 之前出堆过，无需更新邻居的最短路
                continue
            for y, w in self.g[x]:
                if d + w < dis[y]:
                    dis[y] = d + w  # 更新最短路长度
                    heappush(h, (dis[y], y))
        return -1  # 无法到达终点
```

```java [sol-Java]
class Graph {
    private final List<int[]>[] g; // 邻接表

    public Graph(int n, int[][] edges) {
        g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            addEdge(e);
        }
    }

    public void addEdge(int[] e) {
        g[e[0]].add(new int[]{e[1], e[2]});
    }

    public int shortestPath(int start, int end) {
        int[] dis = new int[g.length]; // dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        Arrays.fill(dis, Integer.MAX_VALUE);
        dis[start] = 0;
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> (a[0] - b[0]));
        pq.offer(new int[]{0, start});
        while (!pq.isEmpty()) {
            int[] p = pq.poll();
            int d = p[0];
            int x = p[1];
            if (x == end) { // 计算出从起点到终点的最短路长度
                return d;
            }
            if (d > dis[x]) { // x 之前出堆过，无需更新邻居的最短路
                continue;
            }
            for (int[] e : g[x]) {
                int y = e[0];
                int w = e[1];
                if (d + w < dis[y]) {
                    dis[y] = d + w; // 更新最短路长度
                    pq.offer(new int[]{dis[y], y});
                }
            }
        }
        return -1; // 无法到达终点
    }
}
```

```cpp [sol-C++]
class Graph {
    vector<vector<pair<int, int>>> g; // 邻接表
public:
    Graph(int n, vector<vector<int>> &edges) : g(n) {
        for (auto &e : edges) {
            g[e[0]].emplace_back(e[1], e[2]);
        }
    }

    void addEdge(vector<int> e) {
        g[e[0]].emplace_back(e[1], e[2]);
    }

    int shortestPath(int start, int end) {
        // dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        vector<int> dis(g.size(), INT_MAX);
        dis[start] = 0;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        pq.emplace(0, start);
        while (!pq.empty()) {
            auto [d, x] = pq.top();
            pq.pop();
            if (x == end) { // 计算出从起点到终点的最短路长度
                return d;
            }
            if (d > dis[x]) { // x 之前出堆过，无需更新邻居的最短路
                continue;
            }
            for (auto &[y, w] : g[x]) {
                if (d + w < dis[y]) {
                    dis[y] = d + w; // 更新最短路长度
                    pq.push({dis[y], y});
                }
            }
        }
        return -1; // 无法到达终点
    }
};
```

```go [sol-Go]
type Graph [][]pair

func Constructor(n int, edges [][]int) Graph {
	g := make(Graph, n) // 邻接表
	for _, e := range edges {
		g[e[0]] = append(g[e[0]], pair{e[1], e[2]})
	}
	return g
}

func (g Graph) AddEdge(e []int) {
	g[e[0]] = append(g[e[0]], pair{e[1], e[2]})
}

func (g Graph) ShortestPath(start, end int) int {
	// dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
	dis := make([]int, len(g))
	for i := range dis {
		dis[i] = math.MaxInt
	}
	dis[start] = 0
	h := hp{{start, 0}}
	for len(h) > 0 {
		p := heap.Pop(&h).(pair)
		x, d := p.x, p.d
		if x == end { // 计算出从起点到终点的最短路长度
			return d
		}
		if d > dis[x] { // x 之前出堆过，无需更新邻居的最短路
			continue
		}
		for _, e := range g[x] {
			y, w := e.x, e.d
			newD := d + w
			if newD < dis[y] {
				dis[y] = newD // 更新最短路长度
				heap.Push(&h, pair{y, newD})
			}
		}
	}
	return -1 // 无法到达终点
}

type pair struct{ x, d int }
type hp []pair
func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].d < h[j].d }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(pair)) }
func (h *hp) Pop() (v any)      { a := *h; *h, v = a[:len(a)-1], a[len(a)-1]; return }
```

```js [sol-JavaScript]
var Graph = function(n, edges) {
    const g = Array.from({length: n}, () => []); // 邻接表
    for (const [x, y, w] of edges) {
        g[x].push([y, w]);
    }

    this.addEdge = function(e) {
        g[e[0]].push([e[1], e[2]]);
    }

    this.shortestPath = function(start, end) {
        // dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        const dis = Array(n).fill(Infinity);
        const pq = new MinPriorityQueue(p => p[0]);
        dis[start] = 0;
        pq.enqueue([0, start]);
        while (!pq.isEmpty()) {
            const [d, x] = pq.dequeue();
            if (x === end) { // 计算出从起点到终点的最短路长度
                return d;
            }
            if (d > dis[x]) { // x 之前出堆过，无需更新邻居的最短路
                continue;
            }
            for (const [y, w] of g[x]) {
                if (d + w < dis[y]) {
                    dis[y] = d + w; // 更新最短路长度
                    pq.enqueue([dis[y], y]);
                }
            }
        }
        return -1; // 无法到达终点
    }
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

struct Graph {
    g: Vec<Vec<(usize, i32)>>, // 邻接表
}

impl Graph {
    fn new(n: i32, edges: Vec<Vec<i32>>) -> Self {
        let mut g = vec![vec![]; n as usize];
        for e in &edges {
            g[e[0] as usize].push((e[1] as usize, e[2]));
        }
        Self { g }
    }

    fn add_edge(&mut self, e: Vec<i32>) {
        self.g[e[0] as usize].push((e[1] as usize, e[2]));
    }

    fn shortest_path(&self, node1: i32, node2: i32) -> i32 {
        let start = node1 as usize;
        let end = node2 as usize;
        // dis[i] 表示从起点 start 出发，到节点 i 的最短路长度
        let mut dis = vec![i32::MAX; self.g.len()];
        dis[start] = 0;
        let mut h = BinaryHeap::new();
        h.push((0, start));
        while let Some((d, x)) = h.pop() {
            let d = -d;
            if x == end { // 计算出从起点到终点的最短路长度
                return d;
            }
            if d > dis[x] { // x 之前出堆过，无需更新邻居的最短路
                continue;
            }
            for &(y, w) in &self.g[x] {
                if d + w < dis[y] {
                    dis[y] = d + w; // 更新最短路长度
                    h.push((-dis[y], y)); // 加负号变最小堆
                }
            }
        }
        -1 // 无法到达终点
    }
}
```

#### 复杂度分析

- 时间复杂度：
    - 初始化 $\mathcal{O}(n+m)$。其中 $m$ 为 $\textit{edges}$ 的长度。
    - $\texttt{addEdge}$ $\mathcal{O}(1)$。
    - $\texttt{shortestPath}$ $\mathcal{O}(n + m\log m)$。其中 $m$ 为调用 $\texttt{shortestPath}$ 时，图的边数。当图是稠密图时，复杂度为 $\mathcal{O}(n^2\log n)$。
- 空间复杂度：$\mathcal{O}(n+m)$。其中 $m$ 为 $\textit{edges}$ 的长度加上 $\texttt{addEdge}$ 的调用次数。

## 方法二：Floyd 算法

请先阅读 [带你发明 Floyd 算法：从记忆化搜索到递推](https://leetcode.cn/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/solution/dai-ni-fa-ming-floyd-suan-fa-cong-ji-yi-m8s51/)。

对于 $\texttt{addEdge}$，记 $x=\textit{from},y=\textit{to}$。如果 $\textit{edgeCost} \ge f[x][y]$，则无法更新任何点对的最短路。否则枚举所有 $f[i][j]$，尝试看看能否更新成更小，即从 $i$ 到 $j$ 的如下路径

$$
i---x-y---j
$$

是否更短，写成式子就是

$$
f[i][j] = \min(f[i][j], f[i][x] + \textit{edgeCost} + f[y][j])
$$

**注意**：当 $i=x$ 或 $j=y$ 时，我们需要用到 $f[i][i]$ 这样的值，所以初始化的时候，$f[i][i]$ 要置为 $0$。

对于 $\texttt{shortestPath}$，返回 $f[\textit{start}][\textit{end}]$ 即可。

#### 答疑

**问**：在 $\texttt{addEdge}$ 中，上式 $f[i][j]$ 的计算依赖于 $f[i][x]$ 的值，如果先计算 $f[i][j]$，再计算 $f[i][x]$，我们还需要重新计算 $f[i][j]$ 吗？

**答**：没有必要，每个 $f[i][j]$ 只需要计算一次。如果 $f[i][x]$ 因为 $x-y$ 这条边变小，说明从 $i$ 到 $x$ 的最短路包含 $x-y$ 这条边。那么对于 $f[i][j]$，最短路不可能是 $i---x-y---j$，这意味着最短路会经过 $x-y$ 这条边**两次**。所以重新计算 $f[i][j]$ 是不可能让 $f[i][j]$ 变小的。

```py [sol-Python3]
class Graph:
    def __init__(self, n: int, edges: List[List[int]]):
        f = [[inf] * n for _ in range(n)]
        for i in range(n):
            f[i][i] = 0
        for x, y, w in edges:
            f[x][y] = w  # 添加一条边（题目保证没有重边和自环）
        for k in range(n):
            for i in range(n):
                if f[i][k] == inf: continue
                for j in range(n):
                    f[i][j] = min(f[i][j], f[i][k] + f[k][j])
        self.f = f

    def addEdge(self, edge: List[int]) -> None:
        f = self.f
        x, y, w = edge
        if w >= f[x][y]:  # 无需更新
            return
        n = len(f)
        for i in range(n):
            for j in range(n):
                f[i][j] = min(f[i][j], f[i][x] + w + f[y][j])

    def shortestPath(self, start: int, end: int) -> int:
        ans = self.f[start][end]
        return ans if ans < inf else -1
```

```java [sol-Java]
class Graph {
    private static final int INF = Integer.MAX_VALUE / 3; // 防止更新最短路时加法溢出

    private final int[][] f;

    public Graph(int n, int[][] edges) {
        f = new int[n][n];
        for (int i = 0; i < n; i++) {
            Arrays.fill(f[i], INF);
            f[i][i] = 0;
        }
        for (int[] e : edges) {
            f[e[0]][e[1]] = e[2]; // 添加一条边（题目保证没有重边和自环）
        }
        for (int k = 0; k < n; k++) {
            for (int i = 0; i < n; i++) {
                if (f[i][k] == INF) {
                    continue;
                }
                for (int j = 0; j < n; j++) {
                    f[i][j] = Math.min(f[i][j], f[i][k] + f[k][j]);
                }
            }
        }
    }

    public void addEdge(int[] e) {
        int x = e[0], y = e[1], w = e[2], n = f.length;
        if (w >= f[x][y]) { // 无需更新
            return;
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                f[i][j] = Math.min(f[i][j], f[i][x] + w + f[y][j]);
            }
        }
    }

    public int shortestPath(int start, int end) {
        int ans = f[start][end];
        return ans < INF ? ans : -1;
    }
}
```

```cpp [sol-C++]
class Graph {
    const int INF = INT_MAX / 3;
    vector<vector<int>> f;
public:
    Graph(int n, vector<vector<int>> &edges) : f(n, vector<int>(n, INF)) {
        for (int i = 0; i < n; i++) {
            f[i][i] = 0;
        }
        for (auto &e : edges) {
            f[e[0]][e[1]] = e[2]; // 添加一条边（题目保证没有重边和自环）
        }
        for (int k = 0; k < n; k++) {
            for (int i = 0; i < n; i++) {
                if (f[i][k] == INF) continue;
                for (int j = 0; j < n; j++) {
                    f[i][j] = min(f[i][j], f[i][k] + f[k][j]);
                }
            }
        }
    }

    void addEdge(vector<int> e) {
        int x = e[0], y = e[1], w = e[2], n = f.size();
        if (w >= f[x][y]) { // 无需更新
            return;
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                f[i][j] = min(f[i][j], f[i][x] + w + f[y][j]);
            }
        }
    }

    int shortestPath(int start, int end) {
        int ans = f[start][end];
        return ans < INF ? ans : -1;
    }
};
```

```go [sol-Go]
const inf = math.MaxInt / 3 // 防止更新最短路时加法溢出

type Graph [][]int

func Constructor(n int, edges [][]int) Graph {
	f := make(Graph, n) // 邻接矩阵
	for i := range f {
		f[i] = make([]int, n)
		for j := range f[i] {
			if j != i {
				f[i][j] = inf
			}
		}
	}
	for _, e := range edges {
		f[e[0]][e[1]] = e[2] // 添加一条边（题目保证没有重边和自环）
	}
	for k := range f {
		for i := range f {
			if f[i][k] == inf {
				continue
			}
			for j := range f {
				f[i][j] = min(f[i][j], f[i][k]+f[k][j])
			}
		}
	}
	return f
}

func (f Graph) AddEdge(e []int) {
	x, y, w := e[0], e[1], e[2]
	if w >= f[x][y] { // 无需更新
		return
	}
	for i := range f {
		for j := range f {
			f[i][j] = min(f[i][j], f[i][x]+w+f[y][j])
		}
	}
}

func (f Graph) ShortestPath(start, end int) int {
	ans := f[start][end]
	if ans == inf {
		return -1
	}
	return ans
}
```

```js [sol-JavaScript]
var Graph = function(n, edges) {
    const f = Array.from({length: n}, () => Array(n).fill(Infinity));
    for (let i = 0; i < n; i++) {
        f[i][i] = 0;
    }
    for (const [x, y, w] of edges) {
        f[x][y] = w; // 添加一条边（题目保证没有重边和自环）
    }
    for (let k = 0; k < n; k++) {
        for (let i = 0; i < n; i++) {
            if (f[i][k] === Infinity) continue;
            for (let j = 0; j < n; j++) {
                f[i][j] = Math.min(f[i][j], f[i][k] + f[k][j]);
            }
        }
    }

    this.addEdge = function(e) {
        const [x, y, w] = e;
        if (w >= f[x][y]) { // 无需更新
            return;
        }
        for (let i = 0; i < n; i++) {
            for (let j = 0; j < n; j++) {
                f[i][j] = Math.min(f[i][j], f[i][x] + w + f[y][j]);
            }
        }
    }

    this.shortestPath = function(start, end) {
        const ans = f[start][end];
        return ans < Infinity ? ans : -1;
    }
};
```

```rust [sol-Rust]
struct Graph {
    f: Vec<Vec<i32>>,
}

impl Graph {
    fn new(n: i32, edges: Vec<Vec<i32>>) -> Self {
        const INF: i32 = i32::MAX / 3; // 防止更新最短路时加法溢出
        let n = n as usize;
        let mut f = vec![vec![INF; n]; n];
        for i in 0..n {
            f[i][i] = 0;
        }
        for e in edges {
            f[e[0] as usize][e[1] as usize] = e[2]; // 添加一条边（题目保证没有重边和自环）
        }
        for k in 0..n {
            for i in 0..n {
                if f[i][k] == INF {
                    continue;
                }
                for j in 0..n {
                    f[i][j] = f[i][j].min(f[i][k] + f[k][j]);
                }
            }
        }
        Self { f }
    }

    fn add_edge(&mut self, e: Vec<i32>) {
        let x = e[0] as usize;
        let y = e[1] as usize;
        let w = e[2];
        if w >= self.f[x][y] { // 无需更新
            return;
        }
        let n = self.f.len();
        for i in 0..n {
            for j in 0..n {
                self.f[i][j] = self.f[i][j].min(self.f[i][x] + w + self.f[y][j]);
            }
        }
    }

    fn shortest_path(&self, start: i32, end: i32) -> i32 {
        let ans = self.f[start as usize][end as usize];
        if ans < i32::MAX / 3 { ans } else { -1 }
    }
}
```

#### 复杂度分析

- 时间复杂度：
    - 初始化 $\mathcal{O}(n^3)$。
    - $\texttt{addEdge}$ $\mathcal{O}(n^2)$。
    - $\texttt{shortestPath}$ $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 总结

方法一适用于调用 $\texttt{addEdge}$ 比较多的场景，方法二适用于调用 $\texttt{shortestPath}$ 比较多的场景。

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
