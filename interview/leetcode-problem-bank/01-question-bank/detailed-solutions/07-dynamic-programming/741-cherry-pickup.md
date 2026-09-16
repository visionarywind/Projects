# 741. 摘樱桃

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/cherry-pickup/
- 题目 slug：`cherry-pickup`
- 来源专题：动态规划
- 来源分类路径：二、网格图 DP / §2.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/cherry-pickup/solutions/2766975/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-ruue/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推到空间优化！（Python/Java/C++/Go）](https://leetcode.cn/problems/cherry-pickup/solutions/2766975/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-ruue/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-ruue`
- topic id：`2766975`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、寻找子问题

从左上角 $(0,0)$ 走到右下角 $(n-1,n-1)$，再走回到左上角 $(0,0)$，相当于有两条从 $(0,0)$ 到 $(n-1,n-1)$ 的路径。

我们要计算的，是这两条路径的**并集**上的樱桃个数的最大值。

如果只有一条路径，我们可以像 [64. 最小路径和](https://leetcode.cn/problems/minimum-path-sum/) 那样，定义子问题为：从 $(0,0)$ 到 $(i,j)$ 可以得到的樱桃个数的最大值。

有两条路径要怎么做呢？

跑两次 DP？先走一遍最优路径（把 $1$ 改成 $0$），再走一遍剩下的最优路径？这个做法是错的，[反例](https://leetcode.cn/problems/cherry-pickup/description/comments/500785/)，这个反例可以取到所有樱桃，但两次 DP 的做法没法取到所有樱桃。

所以必须**同时考虑两条路径**。

为方便描述，假设有两个人 A 和 B，都从 $(0,0)$ 走到右下角 $(n-1,n-1)$。

假设 $n=3$，如果 A 最后一步从 $(2,1)$ 走到 $(2,2)$，B 最后一步从 $(1,2)$ 走到 $(2,2)$，我们需要计算两人从 $(0,0)$ 分别走到 $(2,1)$ 和 $(1,2)$，这两条路径并集的樱桃个数的最大值。

虽然说，这是一个和原问题相似的子问题，但两条路径并不是互相独立的，**同一个格子上的樱桃只能统计一次**，我们需要考虑同一个格子重复经过的情况。

如果把子问题定义成：从 $(0,0)$ 出发，A 走了 $p$ 步，B 走了 $q$ 步，分别走到 $(i_1,j_1)$ 和 $(i_2,j_2)$ 可以得到的樱桃个数的最大值。在 $p\ne q$ 的情况下，比如 $p<q$，A 不知道哪些格子 B 已经走过了，无法计算重复经过的情况。所以，把子问题定义成两个人**都走了同样的步数**，也就是 $p=q$ 的情况，才能直接计算出重复经过的情况，也就是两人走到同一个格子。在上面的例子中，我们需要计算两人从 $(0,0)$ 出发，都走了 $3$ 步，分别走到 $(2,1)$ 和 $(1,2)$，可以得到的樱桃个数的最大值。

## 二、状态定义与状态转移方程

定义 $\textit{dfs}(t,i_1,j,i_2,k)$ 表示两人从 $(0,0)$ 出发，都走了 $t$ 步，分别走到 $(i_1,j)$ 和 $(i_2,k)$，可以得到的樱桃个数的最大值。

状态维度有点多，能否简化？

注意到，从 $(0,0)$ 出发，我们只能向下（这会让 $i$ 加一）或向右（这会让 $j$ 加一），所以 $i+j$ 就是走的步数 $t$，所以 $i_1$ 和 $i_2$ 是多余的，可以去掉。

于是，定义 $\textit{dfs}(t,j,k)$ 表示两人从 $(0,0)$ 出发，都走了 $t$ 步，分别走到 $(t-j,j)$ 和 $(t-k,k)$，可以得到的樱桃个数的最大值。

考虑 A 和 B 最后一步怎么走，也就是他们上一步的位置：

- A 往下走，B 往下走，那么问题变成两人从 $(0,0)$ 出发，都走了 $t-1$ 步，分别走到 $(t-1-j,j)$ 和 $(t-1-k,k)$，可以得到的樱桃个数的最大值，即 $\textit{dfs}(t-1,j,k)$。
- A 往下走，B 往右走，那么问题变成两人从 $(0,0)$ 出发，都走了 $t-1$ 步，分别走到 $(t-1-j,j)$ 和 $(t-k,k-1)$，可以得到的樱桃个数的最大值，即 $\textit{dfs}(t-1,j,k-1)$。
- A 往右走，B 往下走，那么问题变成两人从 $(0,0)$ 出发，都走了 $t-1$ 步，分别走到 $(t-j,j-1)$ 和 $(t-1-k,k)$，可以得到的樱桃个数的最大值，即 $\textit{dfs}(t-1,j-1,k)$。
- A 往右走，B 往右走，那么问题变成两人从 $(0,0)$ 出发，都走了 $t-1$ 步，分别走到 $(t-j,j-1)$ 和 $(t-k,k-1)$，可以得到的樱桃个数的最大值，即 $\textit{dfs}(t-1,j-1,k-1)$。

这四种情况取最大值，再加上 $\textit{grid}[t-j][j]$ 和 $\textit{grid}[t-k][k]$（两人位置重合时只加一个），就得到了 $\textit{dfs}(t,j,k)$，即

$$
\textit{dfs}(t,j,k) = \max(\textit{dfs}(t-1,j,k), \textit{dfs}(t-1,j,k-1), \textit{dfs}(t-1,j-1,k), \textit{dfs}(t-1,j-1,k-1)) + \textit{val}
$$

其中

$$
\textit{val} =
\begin{cases}
\textit{grid}[t-j][j] + \textit{grid}[t-k][k],&j \ne k\\
\textit{grid}[t-j][j],&j=k
\end{cases}
$$

递归边界 1：如果 $j,k,t-j,t-k,\textit{grid}[t-j][j], \textit{grid}[t-k][k]$ 中的任何一个数小于 $0$（出界或者访问到 $-1$ 格子），返回 $-\infty$。

递归边界 2：$\textit{dfs}(0,0,0) = \textit{grid}[0][0]$。

递归入口：$\textit{dfs}(2n-2,n-1,n-1)$，也就是答案。如果答案小于 $0$，说明路被荆棘（$-1$）挡住了，按照题目要求，返回 $0$。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(t,j,k)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含如何把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def cherryPickup(self, grid: List[List[int]]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(t: int, j: int, k: int) -> int:
            # 不能出界，不能访问 -1 格子
            if j < 0 or k < 0 or t < j or t < k or grid[t - j][j] < 0 or grid[t - k][k] < 0:
                return -inf
            if t == 0:  # 此时 j = k = 0
                return grid[0][0]
            return max(dfs(t - 1, j, k), dfs(t - 1, j, k - 1), dfs(t - 1, j - 1, k), dfs(t - 1, j - 1, k - 1)) + \
                grid[t - j][j] + (grid[t - k][k] if k != j else 0)
        n = len(grid)
        return max(dfs(n * 2 - 2, n - 1, n - 1), 0)
```

```java [sol-Java]
class Solution {
    public int cherryPickup(int[][] grid) {
        int n = grid.length;
        int[][][] memo = new int[n * 2 - 1][n][n];
        for (int[][] m : memo) {
            for (int[] r : m) {
                Arrays.fill(r, -1); // -1 表示没有计算过
            }
        }
        return Math.max(dfs(n * 2 - 2, n - 1, n - 1, grid, memo), 0);
    }

    private int dfs(int t, int j, int k, int[][] grid, int[][][] memo) {
        // 不能出界，不能访问 -1 格子
        if (j < 0 || k < 0 || t < j || t < k || grid[t - j][j] < 0 || grid[t - k][k] < 0) {
            return Integer.MIN_VALUE;
        }
        if (t == 0) { // 此时 j = k = 0
            return grid[0][0];
        }
        if (memo[t][j][k] != -1) { // 之前计算过
            return memo[t][j][k];
        }
        int res = Math.max(
                Math.max(dfs(t - 1, j, k, grid, memo), dfs(t - 1, j, k - 1, grid, memo)),
                Math.max(dfs(t - 1, j - 1, k, grid, memo), dfs(t - 1, j - 1, k - 1, grid, memo)))
                + grid[t - j][j] + (k != j ? grid[t - k][k] : 0);
        memo[t][j][k] = res; // 记忆化
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int cherryPickup(vector<vector<int>>& grid) {
        int n = grid.size();
        vector memo(n * 2 - 1, vector(n, vector<int>(n, -1))); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int t, int j, int k) -> int {
            // 不能出界，不能访问 -1 格子
            if (j < 0 || k < 0 || t < j || t < k || grid[t - j][j] < 0 || grid[t - k][k] < 0) {
                return INT_MIN;
            }
            if (t == 0) { // 此时 j = k = 0
                return grid[0][0];
            }
            int& res = memo[t][j][k]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            return res = max({dfs(t - 1, j, k), dfs(t - 1, j, k - 1), dfs(t - 1, j - 1, k), dfs(t - 1, j - 1, k - 1)}) +
                         grid[t - j][j] + (k != j ? grid[t - k][k] : 0);
        };
        return max(dfs(n * 2 - 2, n - 1, n - 1), 0);
    }
};
```

```go [sol-Go]
func cherryPickup(grid [][]int) int {
    n := len(grid)
    memo := make([][][]int, n*2-1)
    for i := range memo {
        memo[i] = make([][]int, n)
        for j := range memo[i] {
            memo[i][j] = make([]int, n)
            for k := range memo[i][j] {
                memo[i][j][k] = -1 // -1 表示没有计算过
            }
        }
    }
    var dfs func(int, int, int) int
    dfs = func(t, j, k int) int {
        // 不能出界，不能访问 -1 格子
        if j < 0 || k < 0 || t < j || t < k || grid[t-j][j] < 0 || grid[t-k][k] < 0 {
            return math.MinInt
        }
        if t == 0 { // 此时 j = k = 0
            return grid[0][0]
        }
        p := &memo[t][j][k]
        if *p != -1 { // 之前计算过
            return *p
        }
        res := max(dfs(t-1, j, k), dfs(t-1, j, k-1), dfs(t-1, j-1, k), dfs(t-1, j-1, k-1)) + grid[t-j][j]
        if k != j {
            res += grid[t-k][k]
        }
        *p = res // 记忆化
        return res
    }
    return max(dfs(n*2-2, n-1, n-1), 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^3)$，其中 $n$ 为 $\textit{grid}$ 的行数和列数。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n^3)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n^3)$。
- 空间复杂度：$\mathcal{O}(n^3)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[t][j][k]$ 的定义和 $\textit{dfs}(t,j,k)$ 的定义是一样的，都表示两人从 $(0,0)$ 出发，都走了 $t$ 步，分别走到 $(t-j,j)$ 和 $(t-k,k)$，可以得到的樱桃个数的最大值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[t][j][k] = \max(f[t-1][j][k], f[t-1][j][k-1], f[t-1][j-1][k], f[t-1][j-1][k-1]) + \textit{val}
$$

其中

$$
\textit{val} =
\begin{cases}
\textit{grid}[t-j][j] + \textit{grid}[t-k][k],&j \ne k\\
\textit{grid}[t-j][j],&j=k
\end{cases}
$$

但是，这种定义方式**没有状态能表示递归边界**，即 $j=-1,\ k=-1$ 这种出界的情况。

解决办法：在每个 $f[i]$ 的最左边和最上边各插入一排状态，那么其余状态全部向右和向下偏移一位，把 $f[t][j][k]$ 改为 $f[t][j+1][k+1]$。

修改后 $f[t][j+1][k+1]$ 表示两人从 $(0,0)$ 出发，都走了 $t$ 步，分别走到 $(t-j,j)$ 和 $(t-k,k)$，可以得到的樱桃个数的最大值。此时 $f[t][0][\cdot]$ 和 $f[t][\cdot][0]$ 就对应出界的情况了。

修改后的递推式为

$$
f[t][j+1][k+1] = \max(f[t-1][j+1][k+1], f[t-1][j+1][k], f[t-1][j][k+1], f[t-1][j][k]) + \textit{val}
$$

注意我们只在 $f$ 数组上插入了状态，这只会影响 $f$ 的下标，$\textit{val}$ 的计算方式不变。

初始值 $f[t][j][k]= -\infty,\ f[0][1][1] = \textit{grid}[0][0]$。

答案为 $\max(f[2n-2][n][n],0)$。

### 循环范围

代码实现时，我们还需要讨论清楚 $j$ 和 $k$ 的范围。

由于 $i+j = t$ 且 $0\le i \le n-1$ 且 $0\le j \le n-1$，联立得

$$
\max(t-n+1, 0) \le j \le \min(t, n-1)
$$

对于 $k$ 也同理。

此外，我们可以假定 A 走的是两条路径的下轮廓，B 走的是两条路径的上轮廓（求并集后看不出区别），所以只需计算 $k\ge j$ 的状态。

```py [sol-Python3]
class Solution:
    def cherryPickup(self, grid: List[List[int]]) -> int:
        n = len(grid)
        f = [[[-inf] * (n + 1) for _ in range(n + 1)] for _ in range(n * 2 - 1)]
        f[0][1][1] = grid[0][0]
        for t in range(1, n * 2 - 1):
            for j in range(max(t - n + 1, 0), min(t + 1, n)):
                if grid[t - j][j] < 0: continue
                for k in range(j, min(t + 1, n)):
                    if grid[t - k][k] < 0: continue
                    f[t][j + 1][k + 1] = max(f[t - 1][j + 1][k + 1], f[t - 1][j + 1][k], f[t - 1][j][k + 1], f[t - 1][j][k]) + \
                                         grid[t - j][j] + (grid[t - k][k] if k != j else 0)
        return max(f[-1][n][n], 0)
```

```java [sol-Java]
class Solution {
    public int cherryPickup(int[][] grid) {
        int n = grid.length;
        int[][][] f = new int[n * 2 - 1][n + 1][n + 1];
        for (int[][] m : f) {
            for (int[] r : m) {
                Arrays.fill(r, Integer.MIN_VALUE);
            }
        }
        f[0][1][1] = grid[0][0];
        for (int t = 1; t < n * 2 - 1; t++) {
            for (int j = Math.max(t - n + 1, 0); j <= Math.min(t, n - 1); j++) {
                if (grid[t - j][j] < 0) continue;
                for (int k = j; k <= Math.min(t, n - 1); k++) {
                    if (grid[t - k][k] < 0) continue;
                    f[t][j + 1][k + 1] = Math.max(Math.max(f[t - 1][j + 1][k + 1], f[t - 1][j + 1][k]), Math.max(f[t - 1][j][k + 1], f[t - 1][j][k])) +
                            grid[t - j][j] + (k != j ? grid[t - k][k] : 0);
                }
            }
        }
        return Math.max(f[n * 2 - 2][n][n], 0);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int cherryPickup(vector<vector<int>>& grid) {
        int n = grid.size();
        vector f(n * 2 - 1, vector(n + 1, vector<int>(n + 1, INT_MIN)));
        f[0][1][1] = grid[0][0];
        for (int t = 1; t < n * 2 - 1; t++) {
            for (int j = max(t - n + 1, 0); j <= min(t, n - 1); j++) {
                if (grid[t - j][j] < 0) continue;
                for (int k = j; k <= min(t, n - 1); k++) {
                    if (grid[t - k][k] < 0) continue;
                    f[t][j + 1][k + 1] = max({f[t - 1][j + 1][k + 1], f[t - 1][j + 1][k], f[t - 1][j][k + 1], f[t - 1][j][k]}) +
                                         grid[t - j][j] + (k != j ? grid[t - k][k] : 0);
                }
            }
        }
        return max(f[n * 2 - 2][n][n], 0);
    }
};
```

```go [sol-Go]
func cherryPickup(grid [][]int) int {
    n := len(grid)
    f := make([][][]int, n*2-1)
    for i := range f {
        f[i] = make([][]int, n+1)
        for j := range f[i] {
            f[i][j] = make([]int, n+1)
            for k := range f[i][j] {
                f[i][j][k] = math.MinInt
            }
        }
    }
    f[0][1][1] = grid[0][0]
    for t := 1; t < n*2-1; t++ {
        for j := max(t-n+1, 0); j <= min(t, n-1); j++ {
            if grid[t-j][j] < 0 {
                continue
            }
            for k := j; k <= min(t, n-1); k++ {
                if grid[t-k][k] < 0 {
                    continue
                }
                f[t][j+1][k+1] = max(f[t-1][j+1][k+1], f[t-1][j+1][k], f[t-1][j][k+1], f[t-1][j][k]) + grid[t-j][j]
                if k != j {
                    f[t][j+1][k+1] += grid[t-k][k]
                }
            }
        }
    }
    return max(f[n*2-2][n][n], 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^3)$，其中 $n$ 为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n^3)$。

## 五、空间优化

观察上面的状态转移方程，在计算 $f[t]$ 时，只会用到 $f[t-1]$，不会用到比 $t-1$ 更早的状态。

因此可以像 0-1 背包那样，去掉第一个维度，倒序枚举 $j$ 和 $k$。为什么要倒序请看 [视频讲解：0-1 背包](https://www.bilibili.com/video/BV16Y411v7Y6/)。

状态转移方程改为

$$
f[j+1][k+1] = \max(f[j+1][k+1], f[j+1][k], f[j][k+1], f[j][k]) + \textit{val}
$$

初始值 $f[j][k]= -\infty,\ f[1][1] = \textit{grid}[0][0]$。

答案为 $\max(f[n][n],0)$。

```py [sol-Python3]
class Solution:
    def cherryPickup(self, grid: List[List[int]]) -> int:
        n = len(grid)
        f = [[-inf] * (n + 1) for _ in range(n + 1)]
        f[1][1] = grid[0][0]
        for t in range(1, n * 2 - 1):
            for j in range(min(t, n - 1), max(t - n, -1), -1):
                for k in range(min(t, n - 1), j - 1, -1):
                    if grid[t - j][j] < 0 or grid[t - k][k] < 0:
                        f[j + 1][k + 1] = -inf
                    else:
                        f[j + 1][k + 1] = max(f[j + 1][k + 1], f[j + 1][k], f[j][k + 1], f[j][k]) + \
                                          grid[t - j][j] + (grid[t - k][k] if k != j else 0)
        return max(f[n][n], 0)
```

```java [sol-Java]
class Solution {
    public int cherryPickup(int[][] grid) {
        int n = grid.length;
        int[][] f = new int[n + 1][n + 1];
        for (int[] r : f) {
            Arrays.fill(r, Integer.MIN_VALUE);
        }
        f[1][1] = grid[0][0];
        for (int t = 1; t < n * 2 - 1; t++) {
            for (int j = Math.min(t, n - 1); j >= Math.max(t - n + 1, 0); j--) {
                for (int k = Math.min(t, n - 1); k >= j; k--) {
                    if (grid[t - j][j] < 0 || grid[t - k][k] < 0) {
                        f[j + 1][k + 1] = Integer.MIN_VALUE;
                    } else {
                        f[j + 1][k + 1] = Math.max(Math.max(f[j + 1][k + 1], f[j + 1][k]), Math.max(f[j][k + 1], f[j][k])) +
                                grid[t - j][j] + (k != j ? grid[t - k][k] : 0);
                    }
                }
            }
        }
        return Math.max(f[n][n], 0);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int cherryPickup(vector<vector<int>>& grid) {
        int n = grid.size();
        vector f(n + 1, vector<int>(n + 1, INT_MIN));
        f[1][1] = grid[0][0];
        for (int t = 1; t < n * 2 - 1; t++) {
            for (int j = min(t, n - 1); j >= max(t - n + 1, 0); j--) {
                for (int k = min(t, n - 1); k >= j; k--) {
                    if (grid[t - j][j] < 0 || grid[t - k][k] < 0) {
                        f[j + 1][k + 1] = INT_MIN;
                    } else {
                        f[j + 1][k + 1] = max({f[j + 1][k + 1], f[j + 1][k], f[j][k + 1], f[j][k]}) +
                                          grid[t - j][j] + (k != j ? grid[t - k][k] : 0);
                    }
                }
            }
        }
        return max(f[n][n], 0);
    }
};
```

```go [sol-Go]
func cherryPickup(grid [][]int) int {
    n := len(grid)
    f := make([][]int, n+1)
    for i := range f {
        f[i] = make([]int, n+1)
        for j := range f[i] {
            f[i][j] = math.MinInt
        }
    }
    f[1][1] = grid[0][0]
    for t := 1; t < n*2-1; t++ {
        for j := min(t, n-1); j >= max(t-n+1, 0); j-- {
            for k := min(t, n-1); k >= j; k-- {
                if grid[t-j][j] < 0 || grid[t-k][k] < 0 {
                    f[j+1][k+1] = math.MinInt
                    continue
                }
                f[j+1][k+1] = max(f[j+1][k+1], f[j+1][k], f[j][k+1], f[j][k]) + grid[t-j][j]
                if k != j {
                    f[j+1][k+1] += grid[t-k][k]
                }
            }
        }
    }
    return max(f[n][n], 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^3)$，其中 $n$ 为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n^2)$。

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

本题来自 `二、网格图 DP / §2.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、网格图 DP / §2.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
