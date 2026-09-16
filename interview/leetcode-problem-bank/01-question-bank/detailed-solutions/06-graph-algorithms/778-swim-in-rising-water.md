# 778. 水位上升的泳池中游泳

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/swim-in-rising-water/
- 题目 slug：`swim-in-rising-water`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：2097
- 外部题解来源：https://leetcode.cn/problems/swim-in-rising-water/solutions/3799008/liang-chong-fang-fa-er-fen-da-an-dijkstr-xfcn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：二分答案 / Dijkstra 最短路（Python/Java/C++/Go）](https://leetcode.cn/problems/swim-in-rising-water/solutions/3799008/liang-chong-fang-fa-er-fen-da-an-dijkstr-xfcn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-er-fen-da-an-dijkstr-xfcn`
- topic id：`3799008`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：二分答案

### 题意

找到一条从起点 $(0,0)$ 到终点 $(n-1,n-1)$ 的路径，且路径上的 $\textit{grid}[i][j]$ 的最大值尽量小。返回这个最大值。

### 转化

设路径上的 $\textit{grid}[i][j]$ 均不超过 $\textit{mx}$。

举例来说，如果在 $\textit{mx}=5$ 的情况下，存在从 $(0,0)$ 到 $(n-1,n-1)$ 的路径，那么当 $\textit{mx}=6,7,8,\ldots$ 更大的时候，同样存在从 $(0,0)$ 到 $(n-1,n-1)$ 的路径（直接使用 $\textit{mx}=5$ 时的路径）。

反之，如果在 $\textit{mx}=5$ 的情况下，不存在从 $(0,0)$ 到 $(n-1,n-1)$ 的路径，那么当 $\textit{mx}=4,3,2,\ldots$ 更小的时候，同样不存在从 $(0,0)$ 到 $(n-1,n-1)$ 的路径（反证法，如果存在，那么 $\textit{mx}=5$ 时也会存在）。

据此，可以**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

现在问题转化成一个判定性问题：

- 给定 $\textit{mx}$，判断是否存在一条从 $(0,0)$ 到 $(n-1,n-1)$ 的路径，满足路径上的 $\textit{grid}[i][j]$ 均不超过 $\textit{mx}$。

如果存在，说明答案 $\le \textit{mx}$，否则答案 $> \textit{mx}$。

### 思路

用 DFS，从 $(0,0)$ 出发，只访问 $\textit{grid}[i][j]\le \textit{mx}$ 的相邻格子，判断能否移动到 $(n-1,n-1)$。

注意起点 $\textit{grid}[0][0]$ 必须 $\le \textit{mx}$。

关于网格图 DFS 的原理，见 [200. 岛屿数量](https://leetcode.cn/problems/number-of-islands/)，[我的题解](https://leetcode.cn/problems/number-of-islands/solutions/2965773/ba-fang-wen-guo-de-ge-zi-cha-shang-qi-zi-9gs0/)。

### 细节

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的，喜欢哪种写法就用哪种。

- 开区间左端点初始值：$-1$。由于 $\textit{grid}[i][j]\ge 0$，此时无法移动。
- 开区间左端点初始值（优化）：$\max(\textit{grid}[0][0], \textit{grid}[n-1][n-1])-1$。起点和终点一定在路径上，减一后，$\textit{grid}[i][j]\le \textit{mx}$ 的路径不存在。
- 开区间右端点初始值：$n^2-1$。这是 $\textit{grid}[i][j]$ 的最大值，此时可以随便移动，一定可以走到终点。

> 对于开区间写法，简单来说 `check(mid) == true` 时更新的是谁，最后就返回谁。相比其他二分写法，开区间写法不需要思考加一减一等细节，更简单。推荐使用开区间写二分。

### 答疑

**问**：为什么二分结束后，答案 $\textit{ans}$ 一定来自 $\textit{grid}$ 中的某个数？

**答**：反证法。假设 $\textit{grid}$ 没有任何数等于 $\textit{ans}$，这意味着路径最大值 $< \textit{ans}$，即 $\le \textit{ans}-1$。换句话说，$\text{check}(\textit{ans}-1)=\texttt{true}$。但根据循环不变量，二分结束后 $\text{check}(\textit{ans}-1)=\texttt{false}$，矛盾。故原命题成立。

```py [sol-Python3]
class Solution:
    def swimInWater(self, grid: List[List[int]]) -> int:
        n = len(grid)

        # 判断在只访问 grid[i][j] <= mx 的情况下，能否到达终点
        def check(mx: int) -> bool:
            vis = set()

            def dfs(i: int, j: int) -> bool:
                if i == j == n - 1:  # 到达终点
                    return True
                vis.add((i, j))  # 标记访问过，避免重复访问
                for x, y in (i - 1, j), (i, j + 1), (i + 1, j), (i, j - 1):  # 按照这个顺序访问邻居，代码跑得快（和数据有关系）
                    if 0 <= x < n and 0 <= y < n and grid[x][y] <= mx and (x, y) not in vis and dfs(x, y):
                        return True
                return False

            return dfs(0, 0)

        left = max(grid[0][0], grid[-1][-1]) - 1
        right = n * n - 1
        while left + 1 < right:  # 开区间二分
            mid = (left + right) // 2
            if check(mid):
                right = mid
            else:
                left = mid
        return right
```

```py [sol-Python3 库函数]
class Solution:
    def swimInWater(self, grid: List[List[int]]) -> int:
        n = len(grid)

        # 判断在只访问 grid[i][j] <= mx 的情况下，能否到达终点
        def check(mx: int) -> bool:
            vis = set()

            def dfs(i: int, j: int) -> bool:
                if i == j == n - 1:  # 到达终点
                    return True
                vis.add((i, j))  # 标记访问过，避免重复访问
                for x, y in (i - 1, j), (i, j + 1), (i + 1, j), (i, j - 1):  # 按照这个顺序访问邻居，代码跑得快（和数据有关系）
                    if 0 <= x < n and 0 <= y < n and grid[x][y] <= mx and (x, y) not in vis and dfs(x, y):
                        return True
                return False

            return dfs(0, 0)

        # 库函数是左闭右开区间
        left = max(grid[0][0], grid[-1][-1])
        right = n * n - 1
        return bisect_left(range(right), True, lo=left, key=check)
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

    public int swimInWater(int[][] grid) {
        int n = grid.length;
        int[][] vis = new int[n][n];
        for (int i = 0; i < n; i++) {
            Arrays.fill(vis[i], -1);
        }

        int left = Math.max(grid[0][0], grid[n - 1][n - 1]) - 1;
        int right = n * n - 1;
        while (left + 1 < right) { // 开区间二分
            int mid = left + (right - left) / 2;
            if (dfs(0, 0, mid, grid, vis)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    // 判断在只访问 grid[i][j] <= mx 的情况下，能否到达终点
    private boolean dfs(int i, int j, int mx, int[][] grid, int[][] vis) {
        int n = grid.length;
        if (i == n - 1 && j == n - 1) { // 到达终点
            return true;
        }
        // 标记访问过，避免重复访问
        // 用 mx 区分不同时候的二分，如果 vis[x][y] != mx，说明不是本轮二分访问过的格子
        vis[i][j] = mx;
        for (int[] dir : DIRS) { // 访问相邻的格子
            int x = i + dir[0], y = j + dir[1];
            if (0 <= x && x < n && 0 <= y && y < n && grid[x][y] <= mx && vis[x][y] != mx) {
                if (dfs(x, y, mx, grid, vis)) {
                    return true;
                }
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int DIRS[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

public:
    int swimInWater(vector<vector<int>>& grid) {
        int n = grid.size();
        vector vis(n, vector<int>(n, -1)); // 避免在 check 中反复创建 vis 数组

        // 判断在只访问 grid[i][j] <= mx 的情况下，能否到达终点
        auto check = [&](int mx) -> bool {
            auto dfs = [&](this auto&& dfs, int i, int j) -> bool {
                if (i == n - 1 && j == n - 1) { // 到达终点
                    return true;
                }
                // 标记访问过，避免重复访问
                // 用 mx 区分不同的 check，如果 vis[x][y] != mx，说明不是本次 check 访问过的格子
                vis[i][j] = mx; 
                for (auto& [dx, dy] : DIRS) { // 访问相邻的格子
                    int x = i + dx, y = j + dy;
                    if (0 <= x && x < n && 0 <= y && y < n && grid[x][y] <= mx && vis[x][y] != mx && dfs(x, y)) {
                        return true;
                    }
                }
                return false;
            };
            return dfs(0, 0);
        };

        int left = max(grid[0][0], grid[n - 1][n - 1]) - 1;
        int right = n * n - 1;
        while (left + 1 < right) { // 开区间二分
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right;
    }
};
```

```go [sol-Go]
var dirs = []struct{ x, y int }{{-1, 0}, {0, 1}, {1, 0}, {0, -1}}

func swimInWater(grid [][]int) int {
	n := len(grid)
	// 避免在二分中反复创建 vis 数组
	vis := make([][]int, n)
	for i := range vis {
		vis[i] = make([]int, n)
		for j := range vis[i] {
			vis[i][j] = -1
		}
	}

	left := max(grid[0][0], grid[n-1][n-1])
	right := n*n - 1
	ans := left + sort.Search(right-left, func(mx int) bool {
		mx += left
		// 判断在只访问 grid[i][j] <= mx 的情况下，能否到达终点
		var dfs func(int, int) bool
		dfs = func(i, j int) bool {
			if i == n-1 && j == n-1 { // 到达终点
				return true
			}
			// 标记访问过，避免重复访问
			// 用 mx 区分不同时候的二分，如果 vis[x][y] != mx，说明不是本轮二分访问过的格子
			vis[i][j] = mx
			for _, dir := range dirs { // 访问相邻的格子
				x, y := i+dir.x, j+dir.y
				if 0 <= x && x < n && 0 <= y && y < n && grid[x][y] <= mx && vis[x][y] != mx && dfs(x, y) {
					return true
				}
			}
			return false
		}
		return dfs(0, 0)
	})
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2\log n)$，其中 $n$ 是 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n^2)$。最坏情况下，递归需要 $\mathcal{O}(n^2)$ 的栈空间（如示例 2）。

## 方法二：Dijkstra 最短路

用 Dijkstra 算法解决，原理请看 [Dijkstra 算法介绍](https://leetcode.cn/problems/network-delay-time/solution/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)。

本题计算的不是路径边权和的最小值，而是路径点权最大值的最小值。

把 Dijkstra 算法中的 $+$ 改成 $\max$ 即可。

```py [sol-Python3]
class Solution:
    def swimInWater(self, grid: List[List[int]]) -> int:
        n = len(grid)
        dis = [[inf] * n for _ in range(n)]
        dis[0][0] = grid[0][0]
        h = [(grid[0][0], 0, 0)]  # 堆中保存 (起点到 (i,j) 的最少时间, i, j)

        while True:
            d, i, j = heappop(h)
            if i == j == n - 1:  # 到终点的最短路已确定
                return d
            if d > dis[i][j]:  # (i,j) 之前出堆过
                continue
            for x, y in (i, j - 1), (i, j + 1), (i - 1, j), (i + 1, j):  # 顺序无所谓，我们只看堆中最小的
                if 0 <= x < n and 0 <= y < n:
                    new_dis = max(d, grid[x][y])
                    if new_dis < dis[x][y]:
                        dis[x][y] = new_dis  # 更新 (i,j) 的邻居的最短路
                        # 懒更新堆：只插入数据，不更新堆中数据
                        # 相同节点可能有多个不同的 new_dis，除了最小的 new_dis，其余值都会触发上面的 continue
                        heappush(h, (new_dis, x, y))
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 顺序无所谓，我们只看堆中最小的

    public int swimInWater(int[][] grid) {
        int n = grid.length;
        int[][] dis = new int[n][n];
        for (int[] row : dis) {
            Arrays.fill(row, Integer.MAX_VALUE);
        }
        dis[0][0] = grid[0][0];

        // 堆中保存 (起点到 (i,j) 的最少时间, i, j)
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> a[0] - b[0]);
        pq.offer(new int[]{grid[0][0], 0, 0});

        while (true) {
            int[] top = pq.poll();
            int d = top[0], i = top[1], j = top[2];
            if (i == n - 1 && j == n - 1) { // 到终点的最短路已确定
                return d;
            }
            if (d > dis[i][j]) { // (i,j) 之前出堆过
                continue;
            }
            for (int[] dir : DIRS) {
                int x = i + dir[0], y = j + dir[1];
                if (0 <= x && x < n && 0 <= y && y < n) {
                    int newDis = Math.max(d, grid[x][y]);
                    if (newDis < dis[x][y]) {
                        dis[x][y] = newDis; // 更新 (i,j) 的邻居的最短路
                        // 懒更新堆：只插入数据，不更新堆中数据
                        // 相同节点可能有多个不同的 newDis，其余值都会触发上面的 continue
                        pq.offer(new int[]{newDis, x, y});
                    }
                }
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int DIRS[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 顺序无所谓，我们只看堆中最小的

public:
    int swimInWater(vector<vector<int>>& grid) {
        int n = grid.size();
        vector dis(n, vector<int>(n, INT_MAX));
        dis[0][0] = grid[0][0];

        // (起点到 (i,j) 的最少时间, i, j)
        priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<>> pq;
        pq.emplace(grid[0][0], 0, 0);

        while (true) {
            auto [d, i, j] = pq.top();
            pq.pop();
            if (i == n - 1 && j == n - 1) { // 到终点的最短路已确定
                return d;
            }
            if (d > dis[i][j]) { // (i,j) 之前出堆过
                continue;
            }
            for (auto& [dx, dy] : DIRS) { // 访问相邻的格子
                int x = i + dx, y = j + dy;
                if (0 <= x && x < n && 0 <= y && y < n) {
                    int new_dis = max(d, grid[x][y]);
                    if (new_dis < dis[x][y]) {
                        dis[x][y] = new_dis; // 更新 (i,j) 的邻居的最短路
                        // 懒更新堆：只插入数据，不更新堆中数据
                        // 相同节点可能有多个不同的 new_dis，其余值都会触发上面的 continue
                        pq.emplace(new_dis, x, y);
                    }
                }
            }
        }
    }
};
```

```go [sol-Go]
var dirs = []struct{ x, y int }{{0, -1}, {0, 1}, {-1, 0}, {1, 0}}

func swimInWater(grid [][]int) int {
	n := len(grid)
	dis := make([][]int, n)
	for i := range dis {
		dis[i] = make([]int, n)
		for j := range dis[i] {
			dis[i][j] = math.MaxInt
		}
	}
	dis[0][0] = grid[0][0]

	// 堆中保存 (起点到 (i,j) 的最少时间, i, j)
	h := &hp{{grid[0][0], 0, 0}}
	for {
		p := heap.Pop(h).(tuple)
		d, i, j := p.dis, p.i, p.j
		if i == n-1 && j == n-1 { // 到终点的最短路已确定
			return d
		}
		if d > dis[i][j] { // (i,j) 之前出堆过
			continue
		}
		for _, dir := range dirs {
			x, y := i+dir.x, j+dir.y
			if 0 <= x && x < n && 0 <= y && y < n {
				newDis := max(d, grid[x][y])
				if newDis < dis[x][y] {
					dis[x][y] = newDis // 更新 (i,j) 的邻居的最短路
					// 懒更新堆：只插入数据，不更新堆中数据
					// 相同节点可能有多个不同的 newDis，其余值都会触发上面的 continue
					heap.Push(h, tuple{newDis, x, y})
				}
			}
		}
	}
}

type tuple struct{ dis, i, j int }
type hp []tuple
func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].dis < h[j].dis }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(tuple)) }
func (h *hp) Pop() any          { a := *h; v := a[len(a)-1]; *h = a[:len(a)-1]; return v }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2\log n)$，其中 $n$ 是 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 思考题

如果移动速度不是无限大，而是移动一步就需要 $1$ 秒呢？

## 专题训练

1. 下面二分题单的「**§2.4 最小化最大值**」。
2. 下面图论题单的「**§3.1 单源最短路**」。

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
