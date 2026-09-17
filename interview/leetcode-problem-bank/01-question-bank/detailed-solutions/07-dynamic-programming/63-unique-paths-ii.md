# 63. 不同路径 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/unique-paths-ii/
- 题目 slug：`unique-paths-ii`
- 来源专题：动态规划
- 来源分类路径：二、网格图 DP / §2.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/unique-paths-ii/solutions/3059862/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-451i/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推到空间优化！（Python/Java/C++/Go）](https://leetcode.cn/problems/unique-paths-ii/solutions/3059862/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-451i/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-451i`
- topic id：`3059862`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 一、寻找子问题

![lc63.jpg](https://pic.leetcode.cn/1738201718-nNFgpn-lc63.jpg)

想一想，**最后一步**发生了什么？

- 如果从 $(1,2)$ 向下走到终点 $(2,2)$，那么要解决的问题是从起点 $(0,0)$ 走到 $(1,2)$ 的方案数。
- 如果从 $(2,1)$ 向右走到终点 $(2,2)$，那么要解决的问题是从起点 $(0,0)$ 走到 $(2,1)$ 的方案数。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

## 二、状态定义与状态转移方程

根据上面的讨论，定义状态为 $\textit{dfs}(i,j)$，表示从起点 $(0,0)$ 走到 $(i,j)$ 的方案数。

讨论我们是如何到达 $(i,j)$ 的：

- 如果是从 $(i-1,j)$ 过来，那么问题变成从起点 $(0,0)$ 走到 $(i-1,j)$ 的方案数，即 $\textit{dfs}(i-1,j)$。
- 如果是从 $(i,j-1)$ 过来，那么问题变成从起点 $(0,0)$ 走到 $(i,j-1)$ 的方案数，即 $\textit{dfs}(i,j-1)$。

这两种情况互斥，根据**加法原理**，有

$$
\textit{dfs}(i,j) = \textit{dfs}(i-1,j) + \textit{dfs}(i,j-1)
$$

**递归边界**：

- $\textit{dfs}(-1,j)=\textit{dfs}(i,-1)=0$，无法从 $(0,0)$ 到达这些位置。
- 如果 $\textit{obstacleGrid}[i][j]=1$，那么 $\textit{dfs}(i,j)=0$。
- $\textit{dfs}(0,0)=1$。

**递归入口**：$\textit{dfs}(m-1,n-1)$，这是原问题，也是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def uniquePathsWithObstacles(self, obstacleGrid: List[List[int]]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（一行代码实现记忆化）
        def dfs(i: int, j: int) -> int:
            if i < 0 or j < 0 or obstacleGrid[i][j]:
                return 0
            if i == 0 and j == 0:
                return 1
            return dfs(i - 1, j) + dfs(i, j - 1)

        m, n = len(obstacleGrid), len(obstacleGrid[0])
        return dfs(m - 1, n - 1)
```

```java [sol-Java]
class Solution {
    public int uniquePathsWithObstacles(int[][] obstacleGrid) {
        int m = obstacleGrid.length;
        int n = obstacleGrid[0].length;
        int[][] memo = new int[m][n];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return dfs(m - 1, n - 1, obstacleGrid, memo);
    }

    private int dfs(int i, int j, int[][] obstacleGrid, int[][] memo) {
        if (i < 0 || j < 0 || obstacleGrid[i][j] == 1) {
            return 0;
        }
        if (i == 0 && j == 0) {
            return 1;
        }
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }
        return memo[i][j] = dfs(i - 1, j, obstacleGrid, memo) + dfs(i, j - 1, obstacleGrid, memo);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
        int m = obstacleGrid.size(), n = obstacleGrid[0].size();
        vector memo(m, vector<int>(n, -1)); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i < 0 || j < 0 || obstacleGrid[i][j]) {
                return 0;
            }
            if (i == 0 && j == 0) {
                return 1;
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            return res = dfs(i - 1, j) + dfs(i, j - 1);
        };
        return dfs(m - 1, n - 1);
    }
};
```

```go [sol-Go]
func uniquePathsWithObstacles(obstacleGrid [][]int) int {
    m, n := len(obstacleGrid), len(obstacleGrid[0])
    memo := make([][]int, m)
    for i := range memo {
        memo[i] = make([]int, n)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i < 0 || j < 0 || obstacleGrid[i][j] == 1 {
            return 0
        }
        if i == 0 && j == 0 {
            return 1
        }
        p := &memo[i][j]
        if *p == -1 { // 没有计算过
            *p = dfs(i-1, j) + dfs(i, j-1)
        }
        return *p
    }
    return dfs(m-1, n-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{obstacleGrid}$ 的行数和列数。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(mn)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(mn)$。
- 空间复杂度：$\mathcal{O}(mn)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i+1][j+1]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示从起点 $(0,0)$ 走到 $(i,j)$ 的方案数。这里 $+1$ 是为了把 $\textit{dfs}(-1,j)$ 和 $\textit{dfs}(i,-1)$ 这些状态也翻译过来，这样我们可以把 $f[0][j]$ 和 $f[i][0]$ 作为初始值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i+1][j+1] =
\begin{cases}
f[i][j+1] + f[i+1][j], & \textit{obstacleGrid}[i][j]=0    \\
0, & \textit{obstacleGrid}[i][j]=1     \\
\end{cases}
$$

初始值：

- $f[0][j]=f[i][0] = 0$，翻译自递归边界 $\textit{dfs}(-1,j)=\textit{dfs}(i,-1)=0$。
- $f[1][1]=1$，翻译自递归边界 $\textit{dfs}(0,0)=1$。

也可以把 $f[0][1]$ 初始化成 $1$，这样我们无需单独计算 $f[1][1]$。

答案为 $f[m][n]$，翻译自递归入口 $\textit{dfs}(m-1,n-1)$。

```py [sol-Python3]
class Solution:
    def uniquePathsWithObstacles(self, obstacleGrid: List[List[int]]) -> int:
        m, n = len(obstacleGrid), len(obstacleGrid[0])
        f = [[0] * (n + 1) for _ in range(m + 1)]
        f[0][1] = 1
        for i, row in enumerate(obstacleGrid):
            for j, x in enumerate(row):
                if x == 0:
                    f[i + 1][j + 1] = f[i][j + 1] + f[i + 1][j]
        return f[m][n]
```

```java [sol-Java]
class Solution {
    public int uniquePathsWithObstacles(int[][] obstacleGrid) {
        int m = obstacleGrid.length;
        int n = obstacleGrid[0].length;
        int[][] f = new int[m + 1][n + 1];
        f[0][1] = 1;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (obstacleGrid[i][j] == 0) {
                    f[i + 1][j + 1] = f[i][j + 1] + f[i + 1][j];
                }
            }
        }
        return f[m][n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
        int m = obstacleGrid.size(), n = obstacleGrid[0].size();
        vector f(m + 1, vector<int>(n + 1));
        f[0][1] = 1;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (obstacleGrid[i][j] == 0) {
                    f[i + 1][j + 1] = f[i][j + 1] + f[i + 1][j];
                }
            }
        }
        return f[m][n];
    }
};
```

```go [sol-Go]
func uniquePathsWithObstacles(obstacleGrid [][]int) int {
    m, n := len(obstacleGrid), len(obstacleGrid[0])
    f := make([][]int, m+1)
    for i := range f {
        f[i] = make([]int, n+1)
    }
    f[0][1] = 1
    for i, row := range obstacleGrid {
        for j, x := range row {
            if x == 0 {
                f[i+1][j+1] = f[i][j+1] + f[i+1][j]
            }
        }
    }
    return f[m][n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{obstacleGrid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(mn)$。

## 五、空间优化

举个例子，在计算 $f[1][1]$ 时，会用到 $f[0][1]$，但是之后就不再用到了。那么干脆把 $f[1][1]$ 记到 $f[0][1]$ 中，这样对于 $f[1][2]$ 来说，它需要的数据就在 $f[0][1]$ 和 $f[0][2]$ 中。$f[1][2]$ 算完后也可以同样记到 $f[0][2]$ 中。

所以只需要一个长为 $n+1$ 的一维数组就够了。

具体可以看[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)中的讲解。本题的转移方程类似完全背包，故采用正序遍历。

```py [sol-Python3]
class Solution:
    def uniquePathsWithObstacles(self, obstacleGrid: List[List[int]]) -> int:
        n = len(obstacleGrid[0])
        f = [0] * (n + 1)
        f[1] = 1
        for row in obstacleGrid:
            for j, x in enumerate(row):
                if x == 0:
                    f[j + 1] += f[j]
                else:
                    f[j + 1] = 0
        return f[n]
```

```java [sol-Java]
class Solution {
    public int uniquePathsWithObstacles(int[][] obstacleGrid) {
        int n = obstacleGrid[0].length;
        int[] f = new int[n + 1];
        f[1] = 1;
        for (int[] row : obstacleGrid) {
            for (int j = 0; j < n; j++) {
                if (row[j] == 0) {
                    f[j + 1] += f[j];
                } else {
                    f[j + 1] = 0;
                }
            }
        }
        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
        int n = obstacleGrid[0].size();
        vector<int> f(n + 1);
        f[1] = 1;
        for (auto& row : obstacleGrid) {
            for (int j = 0; j < n; j++) {
                if (row[j] == 0) {
                    f[j + 1] += f[j];
                } else {
                    f[j + 1] = 0;
                }
            }
        }
        return f[n];
    }
};
```

```go [sol-Go]
func uniquePathsWithObstacles(obstacleGrid [][]int) int {
    n := len(obstacleGrid[0])
    f := make([]int, n+1)
    f[1] = 1
    for _, row := range obstacleGrid {
        for j, x := range row {
            if x == 0 {
                f[j+1] += f[j]
            } else {
                f[j+1] = 0
            }
        }
    }
    return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{obstacleGrid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n)$。

## 六、空间优化（原地修改）

直接用 $\textit{obstacleGrid}[0]$ 当作 $f$ 数组，可以做到 $\mathcal{O}(1)$ 额外空间。

由于 $\textit{obstacleGrid}[0]$ 的长度只有 $n$，所以要按照

$$
f[i][j] = f[i-1][j] + f[i][j-1]
$$

的方式来转移。

$i=0$ 和 $j=0$ 的情况要单独计算：

- $i=0$ 时，上式为 $f[i][j] = f[i][j-1]$；用一个数组时，为 $f[j] = f[j-1]$（$\textit{obstacleGrid}[0]$ 就是 $f$ 数组）。
- $j=0$ 时，上式为 $f[i][j] = f[i-1][j]$；用一个数组时，为 $f[0] = f[0]$，可以省略。所以只需要在 $\textit{obstacleGrid}[i][0]=1$ 的情况下，把 $f[0]$ 置为 $0$。

> 注：对比上下两份代码，你会发现长为 $n+1$ 的数组写起来是更加简洁的，因为可以避免特判位于边界的情况。

```py [sol-Python3]
class Solution:
    def uniquePathsWithObstacles(self, obstacleGrid: List[List[int]]) -> int:
        m, n = len(obstacleGrid), len(obstacleGrid[0])
        f = obstacleGrid[0]
        f[0] ^= 1  # 0 变成 1，1 变成 0
        for j in range(1, n):
            f[j] = 0 if f[j] else f[j - 1]
        for i in range(1, m):
            if obstacleGrid[i][0]:
                f[0] = 0
            for j in range(1, n):
                if obstacleGrid[i][j] == 0:
                    f[j] += f[j - 1]
                else:
                    f[j] = 0
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int uniquePathsWithObstacles(int[][] obstacleGrid) {
        int m = obstacleGrid.length;
        int n = obstacleGrid[0].length;
        int[] f = obstacleGrid[0];
        f[0] ^= 1; // 0 变成 1，1 变成 0
        for (int j = 1; j < n; j++) {
            f[j] = f[j] == 1 ? 0 : f[j - 1];
        }
        for (int i = 1; i < m; i++) {
            if (obstacleGrid[i][0] == 1) {
                f[0] = 0;
            }
            for (int j = 1; j < n; j++) {
                if (obstacleGrid[i][j] == 0) {
                    f[j] += f[j - 1];
                } else {
                    f[j] = 0;
                }
            }
        }
        return f[n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
        int m = obstacleGrid.size(), n = obstacleGrid[0].size();
        auto& f = obstacleGrid[0];
        f[0] ^= 1; // 0 变成 1，1 变成 0
        for (int j = 1; j < n; j++) {
            f[j] = f[j] ? 0 : f[j - 1];
        }
        for (int i = 1; i < m; i++) {
            if (obstacleGrid[i][0]) {
                f[0] = 0;
            }
            for (int j = 1; j < n; j++) {
                if (obstacleGrid[i][j] == 0) {
                    f[j] += f[j - 1];
                } else {
                    f[j] = 0;
                }
            }
        }
        return f[n - 1];
    }
};
```

```go [sol-Go]
func uniquePathsWithObstacles(obstacleGrid [][]int) int {
    m, n := len(obstacleGrid), len(obstacleGrid[0])
    f := obstacleGrid[0]
    f[0] ^= 1 // 0 变成 1，1 变成 0
    for j := 1; j < n; j++ {
        if f[j] == 0 {
            f[j] = f[j-1]
        } else {
            f[j] = 0
        }
    }
    for i := 1; i < m; i++ {
        if obstacleGrid[i][0] == 1 {
            f[0] = 0
        }
        for j := 1; j < n; j++ {
            if obstacleGrid[i][j] == 0 {
                f[j] += f[j-1]
            } else {
                f[j] = 0
            }
        }
    }
    return f[n-1]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{obstacleGrid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 进阶问题

网格图的长和宽增大到 $10^5$，改成输入 $2000$ 个障碍物的坐标，要怎么做？

这题是 [Codeforces 559C. Gerald and Giant Chess](https://codeforces.com/problemset/problem/559/C)。

更多相似题目，见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**二、网格图 DP**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. 【本题相关】[动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、网格图 DP / §2.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、网格图 DP / §2.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
