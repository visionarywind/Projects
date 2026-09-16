# 1976. 到达目的地的方案数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-ways-to-arrive-at-destination/
- 题目 slug：`number-of-ways-to-arrive-at-destination`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：2095
- 外部题解来源：https://leetcode.cn/problems/number-of-ways-to-arrive-at-destination/solutions/2668041/zai-ji-suan-zui-duan-lu-de-tong-shi-dpfu-g4f3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[在计算最短路的同时 DP（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/number-of-ways-to-arrive-at-destination/solutions/2668041/zai-ji-suan-zui-duan-lu-de-tong-shi-dpfu-g4f3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zai-ji-suan-zui-duan-lu-de-tong-shi-dpfu-g4f3`
- topic id：`2668041`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[Dijkstra 算法介绍](https://leetcode.cn/problems/network-delay-time/solution/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)

定义 $f[i]$ 表示节点 $0$ 到节点 $i$ 的最短路个数。

在用 $\textit{dis}[x]$ 更新 $\textit{dis}[y]$ 时：

- 如果 $\textit{dis}[x] + g[x][y] < \textit{dis}[y]$，说明从 $0$ 到 $x$ 再到 $y$ 的路径是目前最短的，所以更新 $f[y]$ 为 $f[x]$。
- 如果 $\textit{dis}[x] + g[x][y] = \textit{dis}[y]$，说明从 $0$ 到 $x$ 再到 $y$ 的路径与之前找到的路径一样短，所以把 $f[y]$ 增加 $f[x]$。

初始值：$f[0]=1$，因为 $0$ 到 $0$ 只有一种方案，即原地不动。

答案：$f[n-1]$。

代码实现时，如果当前确定的最小最短路是 $\textit{dis}[n-1]$，可以直接返回 $f[n-1]$，不再继续循环。这是因为本题边权均为正数，后面不可能算出小于或等于 $\textit{dis}[n-1]$ 的最短路，所以 $f[n-1]$ 不会改变。

最后，别忘了对 $10^9+7$ 取模。

## 写法一：朴素 Dijkstra（适用于稠密图）

```py [sol-Python3]
class Solution:
    def countPaths(self, n: int, roads: List[List[int]]) -> int:
        g = [[inf for _ in range(n)] for _ in range(n)]  # 邻接矩阵
        for x, y, d in roads:
            g[x][y] = g[y][x] = d

        dis = [inf] * n
        dis[0] = 0
        f = [0] * n
        f[0] = 1
        done = [False] * n
        while True:
            x = -1
            for i, ok in enumerate(done):
                if not ok and (x < 0 or dis[i] < dis[x]):
                    x = i
            if x == n - 1:
                # 不可能找到比 dis[-1] 更短，或者一样短的最短路了（注意本题边权都是正数）
                return f[-1]
            done[x] = True  # 最短路长度已确定（无法变得更小）
            dx = dis[x]
            for y, d in enumerate(g[x]):  # 尝试更新 x 的邻居的最短路
                new_dis = dx + d
                if new_dis < dis[y]:
                    # 就目前来说，最短路必须经过 x
                    dis[y] = new_dis
                    f[y] = f[x]
                elif new_dis == dis[y]:
                    # 和之前求的最短路一样长
                    f[y] = (f[y] + f[x]) % 1_000_000_007
```

```java [sol-Java]
class Solution {
    public int countPaths(int n, int[][] roads) {
        long[][] g = new long[n][n]; // 邻接矩阵
        for (long[] row : g) {
            Arrays.fill(row, Long.MAX_VALUE / 2); // 防止溢出
        }
        for (int[] r : roads) {
            int x = r[0];
            int y = r[1];
            int d = r[2];
            g[x][y] = d;
            g[y][x] = d;
        }

        long[] dis = new long[n];
        Arrays.fill(dis, 1, n, Long.MAX_VALUE / 2);
        int[] f = new int[n];
        f[0] = 1;
        boolean[] done = new boolean[n];
        while (true) {
            int x = -1;
            for (int i = 0; i < n; i++) {
                if (!done[i] && (x < 0 || dis[i] < dis[x])) {
                    x = i;
                }
            }
            if (x == n - 1) {
                // 不可能找到比 dis[n-1] 更短，或者一样短的最短路了（注意本题边权都是正数）
                return f[n - 1];
            }
            done[x] = true; // 最短路长度已确定（无法变得更小）
            for (int y = 0; y < n; y++) { // 尝试更新 x 的邻居的最短路
                long newDis = dis[x] + g[x][y];
                if (newDis < dis[y]) {
                    // 就目前来说，最短路必须经过 x
                    dis[y] = newDis;
                    f[y] = f[x];
                } else if (newDis == dis[y]) {
                    // 和之前求的最短路一样长
                    f[y] = (f[y] + f[x]) % 1_000_000_007;
                }
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countPaths(int n, vector<vector<int>> &roads) {
        vector<vector<long long>> g(n, vector<long long>(n, LLONG_MAX / 2)); // 邻接矩阵
        for (auto &r : roads) {
            int x = r[0], y = r[1], d = r[2];
            g[x][y] = g[y][x] = d;
        }

        vector<long long> dis(n, LLONG_MAX / 2); // 防止溢出
        dis[0] = 0;
        vector<int> f(n), done(n);
        f[0] = 1;
        while (true) {
            int x = -1;
            for (int i = 0; i < n; i++) {
                if (!done[i] && (x < 0 || dis[i] < dis[x])) {
                    x = i;
                }
            }
            if (x == n - 1) {
                // 不可能找到比 dis[n-1] 更短，或者一样短的最短路了（注意本题边权都是正数）
                return f[n - 1];
            }
            done[x] = true; // 最短路长度已确定（无法变得更小）
            for (int y = 0; y < n; y++) { // 尝试更新 x 的邻居的最短路
                long long new_dis = dis[x] + g[x][y];
                if (new_dis < dis[y]) {
                    // 就目前来说，最短路必须经过 x
                    dis[y] = new_dis;
                    f[y] = f[x];
                } else if (new_dis == dis[y]) {
                    // 和之前求的最短路一样长
                    f[y] = (f[y] + f[x]) % 1'000'000'007;
                }
            }
        }
    }
};
```

```go [sol-Go]
func countPaths(n int, roads [][]int) int {
	g := make([][]int, n) // 邻接矩阵
	for i := range g {
		g[i] = make([]int, n)
		for j := range g[i] {
			g[i][j] = math.MaxInt / 2 // 防止溢出
		}
	}
	for _, r := range roads {
		x, y, d := r[0], r[1], r[2]
		g[x][y] = d
		g[y][x] = d
	}

	dis := make([]int, n)
	for i := 1; i < n; i++ {
		dis[i] = math.MaxInt / 2
	}
	f := make([]int, n)
	f[0] = 1
	done := make([]bool, n)
	for {
		x := -1
		for i, ok := range done {
			if !ok && (x < 0 || dis[i] < dis[x]) {
				x = i
			}
		}
		if x == n-1 {
			// 不可能找到比 dis[n-1] 更短，或者一样短的最短路了（注意本题边权都是正数）
			return f[n-1]
		}
		done[x] = true // 最短路长度已确定（无法变得更小）
		for y, d := range g[x] { // 尝试更新 x 的邻居的最短路
			newDis := dis[x] + d
			if newDis < dis[y] { 
				// 就目前来说，最短路必须经过 x
				dis[y] = newDis
				f[y] = f[x]
			} else if newDis == dis[y] {
				// 和之前求的最短路一样长
				f[y] = (f[y] + f[x]) % 1_000_000_007
			}
		}
	}
}
```

```js [sol-JavaScript]
var countPaths = function(n, roads) {
    const g = Array.from({length: n}, () => Array(n).fill(Infinity)); // 邻接矩阵
    for (const [x, y, d] of roads) {
        g[x][y] = g[y][x] = d;
    }

    const dis = Array(n).fill(Infinity);
    dis[0] = 0;
    const f = Array(n).fill(0);
    f[0] = 1;
    const done = Array(n).fill(false);
    while (true) {
        let x = -1;
        for (let i = 0; i < n; i++) {
            if (!done[i] && (x < 0 || dis[i] < dis[x])) {
                x = i;
            }
        }
        if (x === n - 1) {
            // 不可能找到比 dis[-1] 更短，或者一样短的最短路了（注意本题边权都是正数）
            return f[n - 1];
        }
        done[x] = true; // 最短路长度已确定（无法变得更小）
        for (let y = 0; y < n; y++) { // 尝试更新 x 的邻居的最短路
            const newDis = dis[x] + g[x][y];
            if (newDis < dis[y]) {
                // 就目前来说，最短路必须经过 x
                dis[y] = newDis;
                f[y] = f[x];
            } else if (newDis === dis[y]) {
                // 和之前求的最短路一样长
                f[y] = (f[y] + f[x]) % 1_000_000_007;
            }
        }
    }
};
```

```rust [sol-Rust]
impl Solution {
    pub fn count_paths(n: i32, roads: Vec<Vec<i32>>) -> i32 {
        let n = n as usize;
        let mut g = vec![vec![i64::MAX / 2; n]; n]; // 邻接矩阵
        for r in &roads {
            let x = r[0] as usize;
            let y = r[1] as usize;
            let d = r[2] as i64;
            g[x][y] = d;
            g[y][x] = d;
        }

        let mut dis = vec![i64::MAX / 2; n]; // 防止溢出
        dis[0] = 0;
        let mut f = vec![0; n];
        f[0] = 1;
        let mut done = vec![false; n];
        loop {
            let mut x = n;
            for (i, &ok) in done.iter().enumerate() {
                if !ok && (x == n || dis[i] < dis[x]) {
                    x = i;
                }
            }
            if x == n - 1 {
                // 不可能找到比 dis[n-1] 更短，或者一样短的最短路了（注意本题边权都是正数）
                return f[n - 1];
            }
            done[x] = true; // 最短路长度已确定（无法变得更小）
            for (y, &d) in g[x].iter().enumerate() { // 尝试更新 x 的邻居的最短路
                let new_dis = dis[x] + d;
                if new_dis < dis[y] {
                    // 就目前来说，最短路必须经过 x
                    dis[y] = new_dis;
                    f[y] = f[x];
                } else if new_dis == dis[y] {
                    // 和之前求的最短路一样长
                    f[y] = (f[y] + f[x]) % 1_000_000_007;
                }
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 写法二：堆优化 Dijkstra（适用于稀疏图）

寻找最小值的过程可以用一个最小堆来快速完成：

- 一开始把 $(\textit{dis}[0],0)$ 二元组入堆。
- 当节点 $x$ 首次出堆时，$\textit{dis}[x]$ 就是写法一中寻找的最小最短路。
- 更新 $\textit{dis}[y]$ 时，把 $(\textit{dis}[y],y)$ 二元组入堆。

注意，如果一个节点 $x$ 在出堆前，其最短路长度 $\textit{dis}[x]$ 被多次更新，那么堆中会有多个重复的 $x$，并且包含 $x$ 的二元组中的 $\textit{dis}[x]$ 是互不相同的（因为我们只在找到更小的最短路时才会把二元组入堆）。

所以写法一中的 $\textit{done}$ 数组可以省去，取而代之的是用出堆的最短路值（记作 $\textit{dx}$）与当前的 $\textit{dis}[x]$ 比较，如果 $\textit{dx} > \textit{dis}[x]$ 说明 $x$ 之前出堆过，我们已经更新了 $x$ 的邻居的最短路，所以这次就不用更新了，继续外层循环。

```py [sol-Python3]
class Solution:
    def countPaths(self, n: int, roads: List[List[int]]) -> int:
        g = [[] for _ in range(n)]  # 邻接表
        for x, y, d in roads:
            g[x].append((y, d))
            g[y].append((x, d))

        dis = [inf] * n
        dis[0] = 0
        f = [0] * n
        f[0] = 1
        h = [(0, 0)]
        while True:
            dx, x = heappop(h)
            if x == n - 1:
                # 不可能找到比 dis[-1] 更短，或者一样短的最短路了（注意本题边权都是正数）
                return f[-1]
            if dx > dis[x]:
                continue
            for y, d in g[x]:  # 尝试更新 x 的邻居的最短路
                new_dis = dx + d
                if new_dis < dis[y]:
                    # 就目前来说，最短路必须经过 x
                    dis[y] = new_dis
                    f[y] = f[x]
                    heappush(h, (new_dis, y))
                elif new_dis == dis[y]:
                    # 和之前求的最短路一样长
                    f[y] = (f[y] + f[x]) % 1_000_000_007
```

```java [sol-Java]
class Solution {
    public int countPaths(int n, int[][] roads) {
        List<int[]>[] g = new ArrayList[n]; // 邻接表
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] r : roads) {
            int x = r[0];
            int y = r[1];
            int d = r[2];
            g[x].add(new int[]{y, d});
            g[y].add(new int[]{x, d});
        }

        long[] dis = new long[n];
        Arrays.fill(dis, 1, n, Long.MAX_VALUE);
        int[] f = new int[n];
        f[0] = 1;
        PriorityQueue<Pair<Long, Integer>> pq = new PriorityQueue<>(Comparator.comparingLong(Pair::getKey));
        pq.offer(new Pair<>(0L, 0));
        while (true) {
            Pair<Long, Integer> pair = pq.poll();
            long dx = pair.getKey();
            int x = pair.getValue();
            if (x == n - 1) {
                // 不可能找到比 dis[n-1] 更短，或者一样短的最短路了（注意本题边权都是正数）
                return f[n - 1];
            }
            if (dx > dis[x]) {
                continue;
            }
            for (int[] e : g[x]) { // 尝试更新 x 的邻居的最短路
                int y = e[0];
                long newDis = dx + e[1];
                if (newDis < dis[y]) {
                    // 就目前来说，最短路必须经过 x
                    dis[y] = newDis;
                    f[y] = f[x];
                    pq.offer(new Pair<>(newDis, y));
                } else if (newDis == dis[y]) {
                    // 和之前求的最短路一样长
                    f[y] = (f[y] + f[x]) % 1_000_000_007;
                }
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countPaths(int n, vector<vector<int>> &roads) {
        vector<vector<pair<int, int>>> g(n); // 邻接表
        for (auto &r : roads) {
            int x = r[0], y = r[1], d = r[2];
            g[x].emplace_back(y, d);
            g[y].emplace_back(x, d);
        }

        vector<long long> dis(n, LLONG_MAX);
        dis[0] = 0;
        vector<int> f(n);
        f[0] = 1;
        priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> pq;
        pq.emplace(0, 0);
        while (true) {
            auto [dx, x] = pq.top();
            pq.pop();
            if (x == n - 1) {
                // 不可能找到比 dis[n-1] 更短，或者一样短的最短路了（注意本题边权都是正数）
                return f[n - 1];
            }
            if (dx > dis[x]) {
                continue;
            }
            for (auto &[y, d] : g[x]) { // 尝试更新 x 的邻居的最短路
                long long new_dis = dx + d;
                if (new_dis < dis[y]) {
                    // 就目前来说，最短路必须经过 x
                    dis[y] = new_dis;
                    f[y] = f[x];
                    pq.emplace(new_dis, y);
                } else if (new_dis == dis[y]) {
                    // 和之前求的最短路一样长
                    f[y] = (f[y] + f[x]) % 1'000'000'007;
                }
            }
        }
    }
};
```

```go [sol-Go]
func countPaths(n int, roads [][]int) int {
	type edge struct{ to, d int }
	g := make([][]edge, n) // 邻接表
	for _, r := range roads {
		x, y, d := r[0], r[1], r[2]
		g[x] = append(g[x], edge{y, d})
		g[y] = append(g[y], edge{x, d})
	}

	dis := make([]int, n)
	for i := 1; i < n; i++ {
		dis[i] = math.MaxInt
	}
	f := make([]int, n)
	f[0] = 1
	h := &hp{{}}
	for {
		p := heap.Pop(h).(pair)
		x := p.x
		if x == n-1 {
			// 不可能找到比 dis[n-1] 更短，或者一样短的最短路了（注意本题边权都是正数）
			return f[n-1]
		}
		if p.dis > dis[x] {
			continue
		}
		for _, e := range g[x] { // 尝试更新 x 的邻居的最短路
			y := e.to
			newDis := p.dis + e.d
			if newDis < dis[y] {
				// 就目前来说，最短路必须经过 x
				dis[y] = newDis
				f[y] = f[x]
				heap.Push(h, pair{newDis, y})
			} else if newDis == dis[y] {
				// 和之前求的最短路一样长
				f[y] = (f[y] + f[x]) % 1_000_000_007
			}
		}
	}
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
var countPaths = function(n, roads) {
    const g = Array(n).fill(null).map(() => []); // 邻接表
    for (const [x, y, d] of roads) {
        g[x].push([y, d]);
        g[y].push([x, d]);
    }

    const dis = Array(n).fill(Infinity);
    dis[0] = 0;
    const f = Array(n).fill(0);
    f[0] = 1;
    const pq = new MinPriorityQueue(p => p[0]);
    pq.enqueue([0, 0]);
    while (!pq.isEmpty()) {
        const [dx, x] = pq.dequeue();
        if (x === n - 1) {
            // 不可能找到比 dis[n-1] 更短，或者一样短的最短路了（注意本题边权都是正数）
            return f[n - 1];
        }
        if (dx > dis[x]) {
            continue;
        }
        for (const [y, d] of g[x]) { // 尝试更新 x 的邻居的最短路
            const newDis = dx + d;
            if (newDis < dis[y]) {
                // 就目前来说，最短路必须经过 x
                dis[y] = newDis;
                f[y] = f[x];
                pq.enqueue([newDis, y]);
            } else if (newDis === dis[y]) {
                // 和之前求的最短路一样长
                f[y] = (f[y] + f[x]) % 1_000_000_007;
            }
        }
    }
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn count_paths(n: i32, roads: Vec<Vec<i32>>) -> i32 {
        let n = n as usize;
        let mut g = vec![vec![]; n]; // 邻接表
        for r in &roads {
            let x = r[0] as usize;
            let y = r[1] as usize;
            let d = r[2];
            g[x].push((y, d));
            g[y].push((x, d));
        }

        let mut dis = vec![i64::MAX; n];
        dis[0] = 0;
        let mut f = vec![0; n];
        f[0] = 1;
        let mut pq = BinaryHeap::new();
        pq.push((0, 0));
        loop {
            let (dx, x) = pq.pop().unwrap();
            if x == n - 1 {
                // 不可能找到比 dis[n-1] 更短，或者一样短的最短路了（注意本题边权都是正数）
                return f[n - 1];
            }
            if -dx > dis[x] {
                continue;
            }
            for &(y, d) in &g[x] { // 尝试更新 x 的邻居的最短路
                let new_dis = -dx + d as i64;
                if new_dis < dis[y] {
                    // 就目前来说，最短路必须经过 x
                    dis[y] = new_dis;
                    f[y] = f[x];
                    pq.push((-new_dis, y));
                } else if new_dis == dis[y] {
                    // 和之前求的最短路一样长
                    f[y] = (f[y] + f[x]) % 1_000_000_007;
                }
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log m)$，其中 $m$ 为 $\textit{roads}$ 的长度。由于 $m\ge n-1$，分析复杂度时以 $m$ 为主。注意堆中会有重复节点，所以至多有 $\mathcal{O}(m)$ 个元素，单次操作的复杂度是 $\mathcal{O}(\log m)$。值得注意的是，如果输入的是稠密图，写法二的时间复杂度为 $\mathcal{O}(n^2\log n)$，不如写法一。
- 空间复杂度：$\mathcal{O}(m)$。

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
