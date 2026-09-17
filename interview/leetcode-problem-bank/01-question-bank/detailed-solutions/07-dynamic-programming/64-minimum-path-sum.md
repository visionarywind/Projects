# 64. 最小路径和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-path-sum/
- 题目 slug：`minimum-path-sum`
- 来源专题：动态规划
- 来源分类路径：二、网格图 DP / §2.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/minimum-path-sum/solutions/3045828/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-zfb2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推到空间优化！（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-path-sum/solutions/3045828/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-zfb2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-zfb2`
- topic id：`3045828`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

对于不少动态规划问题，「如何想出状态定义和状态转移方程」是有套路的。我在 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/) 中讲了「递归->记忆化搜索->递推」的思考套路。本文将遵照这个过程，来讲讲怎么从递归开始，一步步写出最后的递推代码。

## 一、寻找子问题

怎么把一个大问题变成小问题？

![lc64-2c.png](https://pic.leetcode.cn/1770460629-KnVnBA-lc64-2c.png){:width=450px}

**见微知著，想清楚最后一步发生了什么，就想清楚每一步发生了什么。**

受上图启发，定义 $\textit{dfs}(i,j)$ 表示从左上角到第 $i$ 行第 $j$ 列这个格子（记作 $(i,j)$）的最小价值和。

分类讨论怎么到达 $(i,j)$：

- 如果是从左边过来，那么必须先到达 $(i,j-1)$，我们需要知道从左上角到 $(i,j-1)$ 的最小价值和，再加上 $\textit{grid}[i][j]$，得到 $\textit{dfs}(i,j-1) + \textit{grid}[i][j]$。
- 如果是从上边过来，那么必须先到达 $(i-1,j)$，我们需要知道从左上角到 $(i-1,j)$ 的最小价值和，再加上 $\textit{grid}[i][j]$，得到 $\textit{dfs}(i-1,j) + \textit{grid}[i][j]$。

二者取最小值，得到**状态转移方程**：

$$
\textit{dfs}(i,j) = \min(\textit{dfs}(i,j-1), \textit{dfs}(i-1,j)) + \textit{grid}[i][j]
$$

**递归边界**：

- $\textit{dfs}(-1,j)=\textit{dfs}(i,-1)=\infty$。用 $\infty$ 表示不合法（出界）的状态，从而保证 $\min$ 不会取到不合法的状态。
- $\textit{dfs}(0,0)=\textit{grid}[0][0]$。

**递归入口**：$\textit{dfs}(m-1,n-1)$，这是原问题，也是答案。

#### 答疑

**问**：看上去，这计算的是从右下角到左上角的最小价值和？

**答**：注意加法运算发生在递归返回后，即递归的「归」的时候我们才**开始**计算最小价值和，所以计算顺序是从左上角到右下角。

**问**：为什么要倒着思考？

**答**：方便后面 1:1 地翻译成递推。

```py [sol-Python3]
# 会超时的递归写法
class Solution:
    def minPathSum(self, grid: List[List[int]]) -> int:
        def dfs(i: int, j: int) -> int:
            if i < 0 or j < 0:
                return inf
            if i == 0 and j == 0:
                return grid[i][j]
            return min(dfs(i, j - 1), dfs(i - 1, j)) + grid[i][j]
        return dfs(len(grid) - 1, len(grid[0]) - 1)
```

```java [sol-Java]
// 会超时的递归写法
class Solution {
    public int minPathSum(int[][] grid) {
        return dfs(grid.length - 1, grid[0].length - 1, grid);
    }

    private int dfs(int i, int j, int[][] grid) {
        if (i < 0 || j < 0) {
            return Integer.MAX_VALUE;
        }
        if (i == 0 && j == 0) {
            return grid[i][j];
        }
        return Math.min(dfs(i, j - 1, grid), dfs(i - 1, j, grid)) + grid[i][j];
    }
}
```

```cpp [sol-C++]
// 会超时的递归写法
class Solution {
public:
    int minPathSum(vector<vector<int>>& grid) {
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i < 0 || j < 0) {
                return INT_MAX;
            }
            if (i == 0 && j == 0) {
                return grid[i][j];
            }
            return min(dfs(i, j - 1), dfs(i - 1, j)) + grid[i][j];
        };
        return dfs(grid.size() - 1, grid[0].size() - 1);
    }
};
```

```go [sol-Go]
// 会超时的递归写法
func minPathSum(grid [][]int) int {
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i < 0 || j < 0 {
            return math.MaxInt
        }
        if i == 0 && j == 0 {
            return grid[i][j]
        }
        return min(dfs(i, j-1), dfs(i-1, j)) + grid[i][j]
    }
    return dfs(len(grid)-1, len(grid[0])-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(2^{m+n})$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。搜索树可以近似为一棵二叉树，树高为 $\mathcal{O}(m+n)$，即从 $\textit{grid}$ 左上角到右下角经过的格子数，所以节点个数为 $\mathcal{O}(2^{m+n})$。
- 空间复杂度：$\mathcal{O}(m+n)$。递归需要 $\mathcal{O}(m+n)$ 的栈空间。

## 二、用记忆化搜索优化

举个例子，对于 $\textit{dfs}(2,2)$ 来说，「先左再上」和「先上再左」，都会调用 $\textit{dfs}(1,1)$。

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

```py [sol-Python3]
class Solution:
    def minPathSum(self, grid: List[List[int]]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int, j: int) -> int:
            if i < 0 or j < 0:
                return inf
            if i == 0 and j == 0:
                return grid[i][j]
            return min(dfs(i, j - 1), dfs(i - 1, j)) + grid[i][j]
        return dfs(len(grid) - 1, len(grid[0]) - 1)
```

```java [sol-Java]
class Solution {
    public int minPathSum(int[][] grid) {
        int m = grid.length;
        int n = grid[0].length;
        int[][] memo = new int[m][n];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return dfs(m - 1, n - 1, grid, memo);
    }

    private int dfs(int i, int j, int[][] grid, int[][] memo) {
        if (i < 0 || j < 0) {
            return Integer.MAX_VALUE;
        }
        if (i == 0 && j == 0) {
            return grid[i][j];
        }
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }
        return memo[i][j] = Math.min(dfs(i, j - 1, grid, memo), dfs(i - 1, j, grid, memo)) + grid[i][j];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minPathSum(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        vector memo(m, vector<int>(n, -1)); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i < 0 || j < 0) {
                return INT_MAX;
            }
            if (i == 0 && j == 0) {
                return grid[i][j];
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            return res = min(dfs(i, j - 1), dfs(i - 1, j)) + grid[i][j];
        };
        return dfs(m - 1, n - 1);
    }
};
```

```go [sol-Go]
func minPathSum(grid [][]int) int {
    m, n := len(grid), len(grid[0])
    memo := make([][]int, m)
    for i := range memo {
        memo[i] = make([]int, n)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i < 0 || j < 0 {
            return math.MaxInt
        }
        if i == 0 && j == 0 {
            return grid[i][j]
        }
        p := &memo[i][j]
        if *p == -1 { // 没有计算过
            *p = min(dfs(i, j-1), dfs(i-1, j)) + grid[i][j]
        }
        return *p
    }
    return dfs(m-1, n-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(mn)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(mn)$。
- 空间复杂度：$\mathcal{O}(mn)$。保存多少状态，就需要多少空间。

## 三、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i+1][j+1]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示从左上角到第 $i$ 行第 $j$ 列这个格子（记作 $(i,j)$）的最小价值和。这里 $+1$ 是为了把 $\textit{dfs}(-1,j)$ 和 $\textit{dfs}(i,-1)$ 这些状态也翻译过来，这样我们可以把 $f[0][j]$ 和 $f[i][0]$ 作为初始值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i+1][j+1] = \min(f[i+1][j], f[i][j+1]) + \textit{grid}[i][j]
$$

> 问：为什么 $\textit{grid}[i][j]$ 的下标不用变？
>
> 答：既然是在 $f$ 的最左边和最上边插入一排状态，那么就只需要修改和 $f$ 有关的下标，其余任何逻辑都无需修改。或者说，如果把 $\textit{grid}[i][j]$ 也改成 $\textit{grid}[i+1][j+1]$，那么当 $i=m-1$ 或者 $j=n-1$ 时 $\textit{grid}[i+1][j+1]$ 会下标越界，这显然是错误的。

初始值：

- $f[0][j]=f[i][0]=\infty$，翻译自递归边界 $\textit{dfs}(-1,j)=\textit{dfs}(i,-1)=\infty$。
- $f[1][1]=\textit{grid}[0][0]$，翻译自递归边界 $\textit{dfs}(0,0)=\textit{grid}[0][0]$。

答案为 $f[m][n]$，翻译自递归入口 $\textit{dfs}(m-1,n-1)$。

### 写法一

```py [sol-Python3]
class Solution:
    def minPathSum(self, grid: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])
        f = [[inf] * (n + 1) for _ in range(m + 1)]
        for i, row in enumerate(grid):
            for j, x in enumerate(row):
                if i == j == 0:
                    f[1][1] = x
                else:
                    f[i + 1][j + 1] = min(f[i + 1][j], f[i][j + 1]) + x
        return f[m][n]
```

```java [sol-Java]
class Solution {
    public int minPathSum(int[][] grid) {
        int m = grid.length;
        int n = grid[0].length;
        int[][] f = new int[m + 1][n + 1];
        Arrays.fill(f[0], Integer.MAX_VALUE);
        for (int i = 0; i < m; i++) {
            f[i + 1][0] = Integer.MAX_VALUE;
            for (int j = 0; j < n; j++) {
                if (i == 0 && j == 0) {
                    f[1][1] = grid[i][j];
                } else {
                    f[i + 1][j + 1] = Math.min(f[i + 1][j], f[i][j + 1]) + grid[i][j];
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
    int minPathSum(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        vector f(m + 1, vector<int>(n + 1, INT_MAX));
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (i == 0 && j == 0) {
                    f[1][1] = grid[i][j];
                } else {
                    f[i + 1][j + 1] = min(f[i + 1][j], f[i][j + 1]) + grid[i][j];
                }
            }
        }
        return f[m][n];
    }
};
```

```go [sol-Go]
func minPathSum(grid [][]int) int {
    m, n := len(grid), len(grid[0])
    f := make([][]int, m+1)
    for i := range f {
        f[i] = make([]int, n+1)
    }
    for j := range f[0] {
        f[0][j] = math.MaxInt
    }
    for i, row := range grid {
        f[i+1][0] = math.MaxInt
        for j, x := range row {
            if i == 0 && j == 0 {
                f[1][1] = x
            } else {
                f[i+1][j+1] = min(f[i+1][j], f[i][j+1]) + x
            }
        }
    }
    return f[m][n]
}
```

### 写法二

把 $f[0][1]$（或者 $f[1][0]$）初始化成 $0$，这样 $f[1][1]$ 也可以用递推式计算了，无需特判。

```py [sol-Python3]
class Solution:
    def minPathSum(self, grid: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])
        f = [[inf] * (n + 1) for _ in range(m + 1)]
        f[0][1] = 0
        for i, row in enumerate(grid):
            for j, x in enumerate(row):
                f[i + 1][j + 1] = min(f[i + 1][j], f[i][j + 1]) + x
        return f[m][n]
```

```java [sol-Java]
class Solution {
    public int minPathSum(int[][] grid) {
        int m = grid.length;
        int n = grid[0].length;
        int[][] f = new int[m + 1][n + 1];
        Arrays.fill(f[0], Integer.MAX_VALUE);
        f[0][1] = 0;
        for (int i = 0; i < m; i++) {
            f[i + 1][0] = Integer.MAX_VALUE;
            for (int j = 0; j < n; j++) {
                f[i + 1][j + 1] = Math.min(f[i + 1][j], f[i][j + 1]) + grid[i][j];
            }
        }
        return f[m][n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minPathSum(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        vector f(m + 1, vector<int>(n + 1, INT_MAX));
        f[0][1] = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                f[i + 1][j + 1] = min(f[i + 1][j], f[i][j + 1]) + grid[i][j];
            }
        }
        return f[m][n];
    }
};
```

```go [sol-Go]
func minPathSum(grid [][]int) int {
    m, n := len(grid), len(grid[0])
    f := make([][]int, m+1)
    for i := range f {
        f[i] = make([]int, n+1)
    }
    for j := 2; j <= n; j++ {
        f[0][j] = math.MaxInt
    }
    for i, row := range grid {
        f[i+1][0] = math.MaxInt
        for j, x := range row {
            f[i+1][j+1] = min(f[i+1][j], f[i][j+1]) + x
        }
    }
    return f[m][n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(mn)$。

## 四、空间优化

举个例子，在计算 $f[1][1]$ 时，会用到 $f[0][1]$，但是之后就不再用到了。那么干脆把 $f[1][1]$ 记到 $f[0][1]$ 中，这样对于 $f[1][2]$ 来说，它需要的数据就在 $f[0][1]$ 和 $f[0][2]$ 中。$f[1][2]$ 算完后也可以同样记到 $f[0][2]$ 中。

所以只需要一个长为 $n+1$ 的一维数组就够了。

具体可以看[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)中的讲解。本题的转移方程类似完全背包，故采用正序遍历。

#### 答疑

**问**：可以初始化 $f[0] = 0$ 吗？

**答**：这会导致所有 $f[i][0]$ 都是 $0$。但对于 $i>1$ 的情况，$f[i][0]$ 必须是 $\infty$。如果 $f[i][0]=0\ (i>1)$，相当于出界也是合法的，这就搞错了。

```py [sol-Python3]
class Solution:
    def minPathSum(self, grid: List[List[int]]) -> int:
        f = [inf] * (len(grid[0]) + 1)
        f[1] = 0
        for row in grid:
            for j, x in enumerate(row):
                f[j + 1] = min(f[j], f[j + 1]) + x
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int minPathSum(int[][] grid) {
        int n = grid[0].length;
        int[] f = new int[n + 1];
        Arrays.fill(f, Integer.MAX_VALUE);
        f[1] = 0;
        for (int[] row : grid) {
            for (int j = 0; j < n; j++) {
                f[j + 1] = Math.min(f[j], f[j + 1]) + row[j];
            }
        }
        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minPathSum(vector<vector<int>>& grid) {
        int n = grid[0].size();
        vector<int> f(n + 1, INT_MAX);
        f[1] = 0;
        for (auto& row : grid) {
            for (int j = 0; j < n; j++) {
                f[j + 1] = min(f[j], f[j + 1]) + row[j];
            }
        }
        return f[n];
    }
};
```

```go [sol-Go]
func minPathSum(grid [][]int) int {
    n := len(grid[0])
    f := make([]int, n+1)
    for j := range f {
        f[j] = math.MaxInt
    }
    f[1] = 0
    for _, row := range grid {
        for j, x := range row {
            f[j+1] = min(f[j], f[j+1]) + x
        }
    }
    return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n)$。

## 五、空间优化（原地修改）

直接用 $\textit{grid}[0]$ 当作 $f$ 数组，可以做到 $\mathcal{O}(1)$ 额外空间。

由于 $\textit{grid}[0]$ 的长度只有 $n$，所以要按照

$$
f[i][j] = \min(f[i][j-1] , f[i-1][j]) +\textit{grid}[i][j]
$$

的方式来转移。

$i=0$ 和 $j=0$ 的情况要单独计算：

- $i=0$ 时，上式为 $f[i][j] = f[i][j-1]+\textit{grid}[i][j]$；用一个数组时，为 $f[j] = f[j-1]+\textit{grid}[0][j]=f[j-1]+f[j]$（$\textit{grid}[0]$ 就是 $f$ 数组）。
- $j=0$ 时，上式为 $f[i][j] = f[i-1][j]+\textit{grid}[i][j]$；用一个数组时，为 $f[0] = f[0]+\textit{grid}[i][0]$。

> 注：对比上下两份代码，你会发现长为 $n+1$ 的数组写起来是更加简洁的，因为可以避免特判位于边界的情况。

```py [sol-Python3]
class Solution:
    def minPathSum(self, grid: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])
        f = grid[0]  # 这里没有拷贝，f 和 grid[0] 都持有同一段内存
        for j in range(1, n):
            f[j] += f[j - 1]
        for i in range(1, m):
            f[0] += grid[i][0]
            for j in range(1, n):
                f[j] = min(f[j - 1], f[j]) + grid[i][j]
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int minPathSum(int[][] grid) {
        int m = grid.length;
        int n = grid[0].length;
        int[] f = grid[0]; // 这里没有拷贝，f 和 grid[0] 都持有同一段内存
        for (int j = 1; j < n; j++) {
            f[j] += f[j - 1];
        }
        for (int i = 1; i < m; i++) {
            f[0] += grid[i][0];
            for (int j = 1; j < n; j++) {
                f[j] = Math.min(f[j - 1], f[j]) + grid[i][j];
            }
        }
        return f[n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minPathSum(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        auto& f = grid[0];
        for (int j = 1; j < n; j++) {
            f[j] += f[j - 1];
        }
        for (int i = 1; i < m; i++) {
            f[0] += grid[i][0];
            for (int j = 1; j < n; j++) {
                f[j] = min(f[j - 1], f[j]) + grid[i][j];
            }
        }
        return f[n - 1];
    }
};
```

```go [sol-Go]
func minPathSum(grid [][]int) int {
    n := len(grid[0])
    f := grid[0]
    for j := 1; j < n; j++ {
        f[j] += f[j-1]
    }
    for _, row := range grid[1:] {
        f[0] += row[0]
        for j := 1; j < n; j++ {
            f[j] = min(f[j-1], f[j]) + row[j]
        }
    }
    return f[n-1]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。

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
