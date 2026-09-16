# 2101. 引爆最多的炸弹

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/detonate-the-maximum-bombs/
- 题目 slug：`detonate-the-maximum-bombs`
- 来源专题：图论算法
- 来源分类路径：一、图的遍历 / §1.1 深度优先搜索（DFS）
- 难度分：1880
- 外部题解来源：https://leetcode.cn/problems/detonate-the-maximum-bombs/solutions/1152450/jian-tu-bao-li-mei-ju-suo-you-qi-dian-by-h4mj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：DFS / Bitset+Floyd（Python/Java/C++/Go）](https://leetcode.cn/problems/detonate-the-maximum-bombs/solutions/1152450/jian-tu-bao-li-mei-ju-suo-you-qi-dian-by-h4mj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-tu-bao-li-mei-ju-suo-you-qi-dian-by-h4mj`
- topic id：`1152450`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：建图 + 枚举起点 DFS

建图：$n$ 个炸弹看成 $n$ 个节点，如果炸弹 $x$ 可以引爆炸弹 $y$，那么就连一条从节点 $x$ 到节点 $y$ 的**有向边**。

为什么不是无向边？看示例 1，右边的炸弹可以引爆左边的炸弹，但是左边的炸弹无法引爆右边的炸弹，所以是有向边。

建图后，枚举 $i$ 作为一开始引爆的炸弹，从 $i$ 开始 **DFS** 这张图，统计能访问到的节点个数，更新答案的最大值。

**小优化**：如果某次 DFS 能访问到 $n$ 个点，说明我们可以引爆所有炸弹，答案就是 $n$，推出循环。

#### 答疑

**问**：为什么不能用并查集？

**答**：注意本题是有向图。例如炸弹 $0$ 可以引爆炸弹 $2$，炸弹 $1$ 可以引爆炸弹 $2$，对应有向边 $0\rightarrow 2,\ 1\rightarrow 2$，那么正确答案是 $2$。如果用并查集做的话，会把 $0,1,2$ 三个点合并起来，计算出错误的答案 $3$。

```py [sol-Python3]
class Solution:
    def maximumDetonation(self, bombs: List[List[int]]) -> int:
        n = len(bombs)
        g = [[] for _ in range(n)]
        for i, (x, y, r) in enumerate(bombs):
            for j, (x2, y2, _) in enumerate(bombs):
                dx = x - x2
                dy = y - y2
                if j != i and dx * dx + dy * dy <= r * r:
                    g[i].append(j)  # i 可以引爆 j

        def dfs(x: int) -> int:
            vis[x] = True
            cnt = 1
            for y in g[x]:
                if not vis[y]:
                    cnt += dfs(y)
            return cnt

        ans = 0
        for i in range(n):
            vis = [False] * n
            ans = max(ans, dfs(i))
            if ans == n:
                break
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumDetonation(int[][] bombs) {
        int n = bombs.length;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int i = 0; i < n; i++) {
            long x = bombs[i][0];
            long y = bombs[i][1];
            long r = bombs[i][2];
            for (int j = 0; j < n; j++) {
                long dx = x - bombs[j][0];
                long dy = y - bombs[j][1];
                if (j != i && dx * dx + dy * dy <= r * r) {
                    g[i].add(j); // i 可以引爆 j
                }
            }
        }

        int ans = 0;
        boolean[] vis = new boolean[n];
        for (int i = 0; i < n && ans < n; i++) {
            Arrays.fill(vis, false);
            ans = Math.max(ans, dfs(g, vis, i));
        }
        return ans;
    }

    private int dfs(List<Integer>[] g, boolean[] vis, int x) {
        vis[x] = true;
        int cnt = 1;
        for (int y : g[x]) {
            if (!vis[y]) {
                cnt += dfs(g, vis, y);
            }
        }
        return cnt;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumDetonation(vector<vector<int>>& bombs) {
        int n = bombs.size();
        vector<vector<int>> g(n);
        for (int i = 0; i < n; i++) {
            long long x = bombs[i][0], y = bombs[i][1], r = bombs[i][2];
            for (int j = 0; j < n; j++) {
                long long dx = x - bombs[j][0];
                long long dy = y - bombs[j][1];
                if (j != i && dx * dx + dy * dy <= r * r) {
                    g[i].push_back(j); // i 可以引爆 j
                }
            }
        }

        int ans = 0;
        vector<int> vis(n);
        auto dfs = [&](auto&& dfs, int x) -> int {
            vis[x] = true;
            int cnt = 1;
            for (int y : g[x]) {
                if (!vis[y]) {
                    cnt += dfs(dfs, y);
                }
            }
            return cnt;
        };
        for (int i = 0; i < n && ans < n; i++) {
            ranges::fill(vis, 0);
            ans = max(ans, dfs(dfs, i));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumDetonation(bombs [][]int) (ans int) {
    n := len(bombs)
    g := make([][]int, n)
    for i, p := range bombs {
        x, y, r := p[0], p[1], p[2]
        for j, q := range bombs {
            dx := x - q[0]
            dy := y - q[1]
            if j != i && dx*dx+dy*dy <= r*r {
                g[i] = append(g[i], j) // i 可以引爆 j
            }
        }
    }

    vis := make([]bool, n)
    var dfs func(int) int
    dfs = func(x int) int {
        vis[x] = true
        cnt := 1
        for _, y := range g[x] {
            if !vis[y] {
                cnt += dfs(y)
            }
        }
        return cnt
    }
    for i := range g {
        clear(vis)
        ans = max(ans, dfs(i))
        if ans == n {
            break
        }
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^3)$，其中 $n$ 是 $\textit{bombs}$ 的长度。注意图中至多有 $\mathcal{O}(n^2)$ 条边，每次 DFS 的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 方法二：Bitset 优化 Floyd

**前置知识**：[带你发明 Floyd 算法：从记忆化搜索到递推](https://leetcode.cn/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/solution/dai-ni-fa-ming-floyd-suan-fa-cong-ji-yi-m8s51/)

> Floyd 算法可以计算任意两点间的最短路长度。但如果只要求计算任意两点间「能否」到达，则可以用 bool 数组代替 int 数组。进一步地，可以把 bool 数组压缩成 bitset。

定义 $f[i]$ 表示 $i$ 可以到达的节点集合。注意 $i$ 一定在 $f[i]$ 中。

如果 $i$ 可以到达 $k$，那么 $k$ 能到达的点，$i$ 也可以到达，于是 $i$ 能到达的点就是 $f[i]$ 和 $f[k]$ 的并集，即

$$
f[i] = f[i]\cup f[k]
$$

代码循环顺序同 Floyd 算法。

初始值：集合 $f[i]$ 初始包含 $i$ 及其邻居节点。

答案：所有 $f[i]$ 集合大小的最大值。

代码中用到了一些位运算，原理见 [从集合论到位运算](https://leetcode.cn/circle/discuss/CaOJ45/)。

```py [sol-Python3]
class Solution:
    def maximumDetonation(self, bombs: List[List[int]]) -> int:
        n = len(bombs)
        f = [0] * n
        for i, (x, y, r) in enumerate(bombs):
            for j, (x2, y2, _) in enumerate(bombs):
                dx = x - x2
                dy = y - y2
                if dx * dx + dy * dy <= r * r:
                    f[i] |= 1 << j  # i 可以到达 j

        for k in range(n):
            for i in range(n):
                if f[i] >> k & 1:  # i 可以到达 k
                    f[i] |= f[k]  # i 也可以到 k 可以到达的点

        return max(s.bit_count() for s in f)  # 集合大小的最大值
```

```java [sol-Java]
class Solution {
    public int maximumDetonation(int[][] bombs) {
        int n = bombs.length;
        BitSet[] f = new BitSet[n];
        for (int i = 0; i < n; i++) {
            long x = bombs[i][0];
            long y = bombs[i][1];
            long r = bombs[i][2];
            f[i] = new BitSet(n);
            for (int j = 0; j < n; j++) {
                long dx = x - bombs[j][0];
                long dy = y - bombs[j][1];
                if (dx * dx + dy * dy <= r * r) {
                    f[i].set(j); // i 可以到达 j
                }
            }
        }

        for (int k = 0; k < n; k++) {
            for (BitSet fi : f) {
                if (fi.get(k)) { // i 可以到达 k
                    fi.or(f[k]); // i 也可以到 k 可以到达的点
                }
            }
        }

        int ans = 0;
        for (BitSet s : f) {
            ans = Math.max(ans, s.cardinality()); // 集合大小的最大值
        }
        return ans;
    }
}
```

```java [sol-Java 手写更快]
class Solution {
    public int maximumDetonation(int[][] bombs) {
        int n = bombs.length;
        Bitset[] f = new Bitset[n];
        for (int i = 0; i < n; i++) {
            long x = bombs[i][0];
            long y = bombs[i][1];
            long r = bombs[i][2];
            f[i] = new Bitset(n);
            for (int j = 0; j < n; j++) {
                long dx = x - bombs[j][0];
                long dy = y - bombs[j][1];
                if (dx * dx + dy * dy <= r * r) {
                    f[i].set(j); // i 可以到达 j
                }
            }
        }

        for (int k = 0; k < n; k++) {
            for (Bitset fi : f) {
                if (fi.has(k)) { // i 可以到达 k
                    fi.or(f[k]); // i 也可以到 k 可以到达的点
                }
            }
        }

        int ans = 0;
        for (Bitset s : f) {
            ans = Math.max(ans, s.count()); // 集合大小的最大值
        }
        return ans;
    }
}

// 相比 java.util.BitSet，去掉了一些边界检查、assert 等
class Bitset {
    private static final int W = Long.SIZE;

    private final long[] bits;

    Bitset(int n) {
        bits = new long[(n + W - 1) / W]; // 需要 ceil(n/W) 个 W 位整数
    }

    void set(int p) {
        bits[p / W] |= 1L << (p % W);
    }

    boolean has(int p) {
        return (bits[p / W] & (1L << (p % W))) != 0;
    }

    void or(Bitset other) {
        for (int i = 0; i < bits.length; i++) {
            bits[i] |= other.bits[i];
        }
    }

    int count() {
        int c = 0;
        for (long x : bits) {
            c += Long.bitCount(x);
        }
        return c;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumDetonation(vector<vector<int>>& bombs) {
        int n = bombs.size();
        vector<bitset<100>> f(n);
        for (int i = 0; i < n; i++) {
            long long x = bombs[i][0], y = bombs[i][1], r = bombs[i][2];
            for (int j = 0; j < n; j++) {
                long long dx = x - bombs[j][0];
                long long dy = y - bombs[j][1];
                if (dx * dx + dy * dy <= r * r) {
                    f[i].set(j); // i 可以到达 j
                }
            }
        }

        for (int k = 0; k < n; k++) {
            for (auto& fi : f) {
                if (fi[k]) { // i 可以到达 k
                    fi |= f[k]; // i 也可以到 k 可以到达的点
                }
            }
        }

        size_t ans = 0;
        for (auto& s : f) {
            ans = max(ans, s.count()); // 集合大小的最大值
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumDetonation(bombs [][]int) (ans int) {
    n := len(bombs)
    f := make([]bitset, n)
    for i, p := range bombs {
        x, y, r := p[0], p[1], p[2]
        f[i] = newBitset(n)
        for j, q := range bombs {
            dx := x - q[0]
            dy := y - q[1]
            if dx*dx+dy*dy <= r*r {
                f[i].set(j) // i 可以到达 j
            }
        }
    }

    for k, fk := range f {
        for _, fi := range f {
            if fi.has(k) { // i 可以到达 k
                fi.or(fk) // i 也可以到 k 可以到达的点
            }
        }
    }

    for _, s := range f {
        ans = max(ans, s.onesCount()) // 集合大小的最大值
    }
    return
}

const w = bits.UintSize

type bitset []uint

func newBitset(n int) bitset {
    return make(bitset, (n+w-1)/w) // 需要 ceil(n/w) 个 w 位整数
}

func (b bitset) set(p int) {
    b[p/w] |= 1 << (p % w)
}

func (b bitset) has(p int) bool {
    return b[p/w]&(1<<(p%w)) != 0
}

func (b bitset) or(other bitset) {
    for i, x := range other {
        b[i] |= x
    }
}

func (b bitset) onesCount() (c int) {
    for _, x := range b {
        c += bits.OnesCount(x)
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^3/w)$，其中 $n$ 是 $\textit{bombs}$ 的长度，$w=32$ 或 $64$。
- 空间复杂度：$\mathcal{O}(n^2/w)$。

## 思考题

如果可以选择引爆 **两个** 炸弹呢？

欢迎在评论区分享你的思路/代码。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 方法一：建图 + 枚举起点 DFS

建图：$n$ 个炸弹看成 $n$ 个节点，如果炸弹 $x$ 可以引爆炸弹 $y$，那么就连一条从节点 $x$ 到节点 $y$ 的**有向边**。

为什么不是无向边？看示例 1，右边的炸弹可以引爆左边的炸弹，但是左边的炸弹无法引爆右边的炸弹，所以是有向边。

建图后，枚举 $i$ 作为一开始引爆的炸弹，从 $i$ 开始 **DFS** 这张图，统计能访问到的节点个数，更新答案的最大值。

**小优化**：如果某次 DFS 能访问到 $n$ 个点，说明我们可以引爆所有炸弹，答案就是 $n$，推出循环。

#### 答疑

**问**：为什么不能用并查集？

**答**：注意本题是有向图。例如炸弹 $0$ 可以引爆炸弹 $2$，炸弹 $1$ 可以引爆炸弹 $2$，对应有向边 $0\rightarrow 2,\ 1\rightarrow 2$，那么正确答案是 $2$。如果用并查集做的话，会把 $0,1,2$ 三个点合并起来，计算出错误的答案 $3$。

```py [sol-Python3]
class Solution:
    def maximumDetonation(self, bombs: List[List[int]]) -> int:
        n = len(bombs)
        g = [[] for _ in range(n)]
        for i, (x, y, r) in enumerate(bombs):
            for j, (x2, y2, _) in enumerate(bombs):
                dx = x - x2
                dy = y - y2
                if j != i and dx * dx + dy * dy <= r * r:
                    g[i].append(j)  # i 可以引爆 j

        def dfs(x: int) -> int:
            vis[x] = True
            cnt = 1
            for y in g[x]:
                if not vis[y]:
                    cnt += dfs(y)
            return cnt

        ans = 0
        for i in range(n):
            vis = [False] * n
            ans = max(ans, dfs(i))
            if ans == n:
                break
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumDetonation(int[][] bombs) {
        int n = bombs.length;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int i = 0; i < n; i++) {
            long x = bombs[i][0];
            long y = bombs[i][1];
            long r = bombs[i][2];
            for (int j = 0; j < n; j++) {
                long dx = x - bombs[j][0];
                long dy = y - bombs[j][1];
                if (j != i && dx * dx + dy * dy <= r * r) {
                    g[i].add(j); // i 可以引爆 j
                }
            }
        }

        int ans = 0;
        boolean[] vis = new boolean[n];
        for (int i = 0; i < n && ans < n; i++) {
            Arrays.fill(vis, false);
            ans = Math.max(ans, dfs(g, vis, i));
        }
        return ans;
    }

    private int dfs(List<Integer>[] g, boolean[] vis, int x) {
        vis[x] = true;
        int cnt = 1;
        for (int y : g[x]) {
            if (!vis[y]) {
                cnt += dfs(g, vis, y);
            }
        }
        return cnt;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumDetonation(vector<vector<int>>& bombs) {
        int n = bombs.size();
        vector<vector<int>> g(n);
        for (int i = 0; i < n; i++) {
            long long x = bombs[i][0], y = bombs[i][1], r = bombs[i][2];
            for (int j = 0; j < n; j++) {
                long long dx = x - bombs[j][0];
                long long dy = y - bombs[j][1];
                if (j != i && dx * dx + dy * dy <= r * r) {
                    g[i].push_back(j); // i 可以引爆 j
                }
            }
        }

        int ans = 0;
        vector<int> vis(n);
        auto dfs = [&](auto&& dfs, int x) -> int {
            vis[x] = true;
            int cnt = 1;
            for (int y : g[x]) {
                if (!vis[y]) {
                    cnt += dfs(dfs, y);
                }
            }
            return cnt;
        };
        for (int i = 0; i < n && ans < n; i++) {
            ranges::fill(vis, 0);
            ans = max(ans, dfs(dfs, i));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumDetonation(bombs [][]int) (ans int) {
    n := len(bombs)
    g := make([][]int, n)
    for i, p := range bombs {
        x, y, r := p[0], p[1], p[2]
        for j, q := range bombs {
            dx := x - q[0]
            dy := y - q[1]
            if j != i && dx*dx+dy*dy <= r*r {
                g[i] = append(g[i], j) // i 可以引爆 j
            }
        }
    }

    vis := make([]bool, n)
    var dfs func(int) int
    dfs = func(x int) int {
        vis[x] = true
        cnt := 1
        for _, y := range g[x] {
            if !vis[y] {
                cnt += dfs(y)
            }
        }
        return cnt
    }
    for i := range g {
        clear(vis)
        ans = max(ans, dfs(i))
        if ans == n {
            break
        }
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^3)$，其中 $n$ 是 $\textit{bombs}$ 的长度。注意图中至多有 $\mathcal{O}(n^2)$ 条边，每次 DFS 的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 方法二：Bitset 优化 Floyd

**前置知识**：[带你发明 Floyd 算法：从记忆化搜索到递推](https://leetcode.cn/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/solution/dai-ni-fa-ming-floyd-suan-fa-cong-ji-yi-m8s51/)

> Floyd 算法可以计算任意两点间的最短路长度。但如果只要求计算任意两点间「能否」到达，则可以用 bool 数组代替 int 数组。进一步地，可以把 bool 数组压缩成 bitset。

定义 $f[i]$ 表示 $i$ 可以到达的节点集合。注意 $i$ 一定在 $f[i]$ 中。

如果 $i$ 可以到达 $k$，那么 $k$ 能到达的点，$i$ 也可以到达，于是 $i$ 能到达的点就是 $f[i]$ 和 $f[k]$ 的并集，即

$$
f[i] = f[i]\cup f[k]
$$

代码循环顺序同 Floyd 算法。

初始值：集合 $f[i]$ 初始包含 $i$ 及其邻居节点。

答案：所有 $f[i]$ 集合大小的最大值。

代码中用到了一些位运算，原理见 [从集合论到位运算](https://leetcode.cn/circle/discuss/CaOJ45/)。

```py [sol-Python3]
class Solution:
    def maximumDetonation(self, bombs: List[List[int]]) -> int:
        n = len(bombs)
        f = [0] * n
        for i, (x, y, r) in enumerate(bombs):
            for j, (x2, y2, _) in enumerate(bombs):
                dx = x - x2
                dy = y - y2
                if dx * dx + dy * dy <= r * r:
                    f[i] |= 1 << j  # i 可以到达 j

        for k in range(n):
            for i in range(n):
                if f[i] >> k & 1:  # i 可以到达 k
                    f[i] |= f[k]  # i 也可以到 k 可以到达的点

        return max(s.bit_count() for s in f)  # 集合大小的最大值
```

```java [sol-Java]
class Solution {
    public int maximumDetonation(int[][] bombs) {
        int n = bombs.length;
        BitSet[] f = new BitSet[n];
        for (int i = 0; i < n; i++) {
            long x = bombs[i][0];
            long y = bombs[i][1];
            long r = bombs[i][2];
            f[i] = new BitSet(n);
            for (int j = 0; j < n; j++) {
                long dx = x - bombs[j][0];
                long dy = y - bombs[j][1];
                if (dx * dx + dy * dy <= r * r) {
                    f[i].set(j); // i 可以到达 j
                }
            }
        }

        for (int k = 0; k < n; k++) {
            for (BitSet fi : f) {
                if (fi.get(k)) { // i 可以到达 k
                    fi.or(f[k]); // i 也可以到 k 可以到达的点
                }
            }
        }

        int ans = 0;
        for (BitSet s : f) {
            ans = Math.max(ans, s.cardinality()); // 集合大小的最大值
        }
        return ans;
    }
}
```

```java [sol-Java 手写更快]
class Solution {
    public int maximumDetonation(int[][] bombs) {
        int n = bombs.length;
        Bitset[] f = new Bitset[n];
        for (int i = 0; i < n; i++) {
            long x = bombs[i][0];
            long y = bombs[i][1];
            long r = bombs[i][2];
            f[i] = new Bitset(n);
            for (int j = 0; j < n; j++) {
                long dx = x - bombs[j][0];
                long dy = y - bombs[j][1];
                if (dx * dx + dy * dy <= r * r) {
                    f[i].set(j); // i 可以到达 j
                }
            }
        }

        for (int k = 0; k < n; k++) {
            for (Bitset fi : f) {
                if (fi.has(k)) { // i 可以到达 k
                    fi.or(f[k]); // i 也可以到 k 可以到达的点
                }
            }
        }

        int ans = 0;
        for (Bitset s : f) {
            ans = Math.max(ans, s.count()); // 集合大小的最大值
        }
        return ans;
    }
}

// 相比 java.util.BitSet，去掉了一些边界检查、assert 等
class Bitset {
    private static final int W = Long.SIZE;

    private final long[] bits;

    Bitset(int n) {
        bits = new long[(n + W - 1) / W]; // 需要 ceil(n/W) 个 W 位整数
    }

    void set(int p) {
        bits[p / W] |= 1L << (p % W);
    }

    boolean has(int p) {
        return (bits[p / W] & (1L << (p % W))) != 0;
    }

    void or(Bitset other) {
        for (int i = 0; i < bits.length; i++) {
            bits[i] |= other.bits[i];
        }
    }

    int count() {
        int c = 0;
        for (long x : bits) {
            c += Long.bitCount(x);
        }
        return c;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumDetonation(vector<vector<int>>& bombs) {
        int n = bombs.size();
        vector<bitset<100>> f(n);
        for (int i = 0; i < n; i++) {
            long long x = bombs[i][0], y = bombs[i][1], r = bombs[i][2];
            for (int j = 0; j < n; j++) {
                long long dx = x - bombs[j][0];
                long long dy = y - bombs[j][1];
                if (dx * dx + dy * dy <= r * r) {
                    f[i].set(j); // i 可以到达 j
                }
            }
        }

        for (int k = 0; k < n; k++) {
            for (auto& fi : f) {
                if (fi[k]) { // i 可以到达 k
                    fi |= f[k]; // i 也可以到 k 可以到达的点
                }
            }
        }

        size_t ans = 0;
        for (auto& s : f) {
            ans = max(ans, s.count()); // 集合大小的最大值
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumDetonation(bombs [][]int) (ans int) {
    n := len(bombs)
    f := make([]bitset, n)
    for i, p := range bombs {
        x, y, r := p[0], p[1], p[2]
        f[i] = newBitset(n)
        for j, q := range bombs {
            dx := x - q[0]
            dy := y - q[1]
            if dx*dx+dy*dy <= r*r {
                f[i].set(j) // i 可以到达 j
            }
        }
    }

    for k, fk := range f {
        for _, fi := range f {
            if fi.has(k) { // i 可以到达 k
                fi.or(fk) // i 也可以到 k 可以到达的点
            }
        }
    }

    for _, s := range f {
        ans = max(ans, s.onesCount()) // 集合大小的最大值
    }
    return
}

const w = bits.UintSize

type bitset []uint

func newBitset(n int) bitset {
    return make(bitset, (n+w-1)/w) // 需要 ceil(n/w) 个 w 位整数
}

func (b bitset) set(p int) {
    b[p/w] |= 1 << (p % w)
}

func (b bitset) has(p int) bool {
    return b[p/w]&(1<<(p%w)) != 0
}

func (b bitset) or(other bitset) {
    for i, x := range other {
        b[i] |= x
    }
}

func (b bitset) onesCount() (c int) {
    for _, x := range b {
        c += bits.OnesCount(x)
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^3/w)$，其中 $n$ 是 $\textit{bombs}$ 的长度，$w=32$ 或 $64$。
- 空间复杂度：$\mathcal{O}(n^2/w)$。

## 思考题

如果可以选择引爆 **两个** 炸弹呢？

欢迎在评论区分享你的思路/代码。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、图的遍历 / §1.1 深度优先搜索（DFS）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、图的遍历 / §1.1 深度优先搜索（DFS）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
