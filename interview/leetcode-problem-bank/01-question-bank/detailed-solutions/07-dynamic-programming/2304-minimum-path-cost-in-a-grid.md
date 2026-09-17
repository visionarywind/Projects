# 2304. 网格中的最小路径代价

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-path-cost-in-a-grid/
- 题目 slug：`minimum-path-cost-in-a-grid`
- 来源专题：动态规划
- 来源分类路径：二、网格图 DP / §2.1 基础
- 难度分：1658
- 外部题解来源：https://leetcode.cn/problems/minimum-path-cost-in-a-grid/solutions/2536856/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-bd25/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考动态规划：从记忆化搜索到递推，再到 O(1) 空间！（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-path-cost-in-a-grid/solutions/2536856/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-bd25/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-bd25`
- topic id：`2536856`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 前置知识：动态规划入门

请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，制作不易，欢迎点赞！

## 一、启发思考：寻找子问题

![lc2304.png](https://pic.leetcode.cn/1700527245-yKjdhc-lc2304.png)

看示例 1。假设我们从第一行的 $\textit{grid}[0][0]$ 出发，要计算的问题是：从 $\textit{grid}[0][0]$ 出发，到达最后一行任意单元格的最小路径代价。

枚举下一步的位置。例如下一步移动到 $\textit{grid}[1][1]$，那么要计算的问题是：从 $\textit{grid}[1][1]$ 出发，到达最后一行任意单元格的最小路径代价。这是一个**和原问题相似的、规模更小的子问题**，所以我们可以用**递归**解决。

> 注：动态规划有「选或不选」和「枚举选哪个」两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「枚举选哪个」。

## 二、递归怎么写：状态定义与状态转移方程

如何用参数表示一个子问题？我们需要知道从哪个格子出发，所以用格子的坐标 $(i,j)$ 就能表示一个子问题。

因此，定义 $\textit{dfs}(i,j)$ 表示从 $\textit{grid}[i][j]$ 出发，到达最后一行任意单元格的最小路径代价。

枚举下一步的位置。假设移动到下一行的 $\textit{grid}[i+1][k]$，那么移动代价就是 

$$
\textit{moveCost}[\textit{grid}[i][j]][k]
$$

枚举所有的 $k$，取最小值，再加上当前单元格的值 $\textit{grid}[i][j]$，得到

$$
\textit{dfs}(i,j) = \textit{grid}[i][j] + \min_{k=0}^{n-1} \textit{dfs}(i+1,k) + \textit{moveCost}[\textit{grid}[i][j]][k]
$$

递归边界：$\textit{dfs}(m-1,j)=\textit{grid}[m-1][j]$。最后一行无需移动，代价只有单元格的值。

递归入口：$\textit{dfs}(0,j)$。枚举所有的 $j$，取最小值，答案就是 

$$
\min_{j=0}^{n-1} \textit{dfs}(0,j)
$$

```py [sol-Python3]
# 会超时的递归代码
class Solution:
    def minPathCost(self, grid: List[List[int]], moveCost: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])

        def dfs(i: int, j: int) -> int:
            if i == m - 1:  # 递归边界
                return grid[i][j]
            res = inf
            for k, c in enumerate(moveCost[grid[i][j]]):  # 移动到下一行的第 k 列
                res = min(res, dfs(i + 1, k) + c)
            return res + grid[i][j]

        return min(dfs(0, j) for j in range(n))  # 枚举起点
```

```java [sol-Java]
// 会超时的递归代码
class Solution {
    public int minPathCost(int[][] grid, int[][] moveCost) {
        int ans = Integer.MAX_VALUE;
        for (int j = 0; j < grid[0].length; j++) { // 枚举起点
            ans = Math.min(ans, dfs(0, j, grid, moveCost));
        }
        return ans;
    }

    private int dfs(int i, int j, int[][] grid, int[][] moveCost) {
        if (i == grid.length - 1) { // 递归边界
            return grid[i][j];
        }
        int res = Integer.MAX_VALUE;
        for (int k = 0; k < grid[0].length; k++) { // 移动到下一行的第 k 列
            res = Math.min(res, dfs(i + 1, k, grid, moveCost) + moveCost[grid[i][j]][k]);
        }
        return res + grid[i][j];
    }
}
```

```cpp [sol-C++]
// 会超时的递归代码
class Solution {
public:
    int minPathCost(vector<vector<int>>& grid, vector<vector<int>>& moveCost) {
        int m = grid.size(), n = grid[0].size();
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i == m - 1) { // 递归边界
                return grid[i][j];
            }
            int res = INT_MAX;
            for (int k = 0; k < n; k++) { // 移动到下一行的第 k 列
                res = min(res, dfs(i + 1, k) + moveCost[grid[i][j]][k]);
            }
            return res + grid[i][j];
        };
        int ans = INT_MAX;
        for (int j = 0; j < n; j++) { // 枚举起点
            ans = min(ans, dfs(0, j));
        }
        return ans;
    }
};
```

```go [sol-Go]
// 会超时的递归代码
func minPathCost(grid [][]int, moveCost [][]int) int {
    m, n := len(grid), len(grid[0])
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i == m-1 { // 递归边界
            return grid[i][j]
        }
        res := math.MaxInt
        for k, c := range moveCost[grid[i][j]] { // 移动到下一行的第 k 列
            res = min(res, dfs(i+1, k)+c)
        }
        return res + grid[i][j]
    }
    ans := math.MaxInt
    for j := 0; j < n; j++ { // 枚举起点
        ans = min(ans, dfs(0, j))
    }
    return ans
}
```

```js [sol-JavaScript]
// 会超时的递归代码
var minPathCost = function(grid, moveCost) {
    const m = grid.length, n = grid[0].length;

    function dfs(i, j) {
        if (i === m - 1) { // 递归边界
            return grid[i][j];
        }
        let res = Infinity;
        for (let k = 0; k < n; k++) { // 移动到下一行的第 k 列
            res = Math.min(res, dfs(i + 1, k) + moveCost[grid[i][j]][k]);
        }
        return res + grid[i][j];
    }

    let ans = Infinity;
    for (let j = 0; j < n; j++) { // 枚举起点
        ans = Math.min(ans, dfs(0, j));
    }
    return ans;
};
```

```rust [sol-Rust]
// 会超时的递归代码
impl Solution {
    pub fn min_path_cost(grid: Vec<Vec<i32>>, move_cost: Vec<Vec<i32>>) -> i32 {
        fn dfs(i: usize, j: usize, grid: &Vec<Vec<i32>>, move_cost: &Vec<Vec<i32>>) -> i32 {
            if i == grid.len() - 1 { // 递归边界
                return grid[i][j];
            }
            let mut res = i32::MAX;
            for (k, &c) in move_cost[grid[i][j] as usize].iter().enumerate() {
                // 移动到下一行的第 k 列
                res = res.min(dfs(i + 1, k, grid, move_cost) + c);
            }
            res + grid[i][j]
        }
        let mut ans = i32::MAX;
        for j in 0..grid[0].len() { // 枚举起点
            ans = ans.min(dfs(0, j, &grid, &move_cost));
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^m)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。搜索树是一棵 $n$ 叉树，树高为 $\mathcal{O}(m)$，所以节点个数为 $\mathcal{O}(n^m)$，遍历搜索树需要 $\mathcal{O}(n^m)$ 的时间。
- 空间复杂度：$\mathcal{O}(m)$。递归需要 $\mathcal{O}(m)$ 的栈空间。

## 三、递归 + 记录返回值 = 记忆化搜索

上面的做法太慢了，怎么优化呢？

对于 $\textit{grid}[i][j]$ 来说，上一行的每个格子都可以移动到它，所以我们会多次递归到 $\textit{dfs}(i,j)$。

一叶知秋，整个递归中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的初始值一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。本题由于 $\textit{moveCost}$ 中的数都是正数，所以可以把初始值设置为 $0$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

```py [sol-Python3]
class Solution:
    def minPathCost(self, grid: List[List[int]], moveCost: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])

        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int, j: int) -> int:
            if i == m - 1:  # 递归边界
                return grid[i][j]
            res = inf
            for k, c in enumerate(moveCost[grid[i][j]]):  # 移动到下一行的第 k 列
                res = min(res, dfs(i + 1, k) + c)
            return res + grid[i][j]

        return min(dfs(0, j) for j in range(n))  # 枚举起点
```

```java [sol-Java]
class Solution {
    public int minPathCost(int[][] grid, int[][] moveCost) {
        int m = grid.length;
        int n = grid[0].length;
        int[][] memo = new int[m][n];
        int ans = Integer.MAX_VALUE;
        for (int j = 0; j < n; j++) { // 枚举起点
            ans = Math.min(ans, dfs(0, j, memo, grid, moveCost));
        }
        return ans;
    }

    private int dfs(int i, int j, int[][] memo, int[][] grid, int[][] moveCost) {
        if (i == grid.length - 1) { // 递归边界
            return grid[i][j];
        }
        if (memo[i][j] != 0) { // 之前计算过
            return memo[i][j];
        }
        int res = Integer.MAX_VALUE;
        for (int k = 0; k < grid[0].length; k++) { // 移动到下一行的第 k 列
            res = Math.min(res, dfs(i + 1, k, memo, grid, moveCost) + moveCost[grid[i][j]][k]);
        }
        return memo[i][j] = res + grid[i][j]; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minPathCost(vector<vector<int>>& grid, vector<vector<int>>& moveCost) {
        int m = grid.size(), n = grid[0].size();
        vector memo(m, vector<int>(n));
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i == m - 1) { // 递归边界
                return grid[i][j];
            }
            auto& res = memo[i][j]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }
            res = INT_MAX;
            for (int k = 0; k < n; k++) { // 移动到下一行的第 k 列
                res = min(res, dfs(i + 1, k) + moveCost[grid[i][j]][k]);
            }
            res += grid[i][j];
            return res;
        };
        int ans = INT_MAX;
        for (int j = 0; j < n; j++) { // 枚举起点
            ans = min(ans, dfs(0, j));
        }
        return ans;
    }
};
```

```go [sol-Go]
func minPathCost(grid [][]int, moveCost [][]int) int {
    m, n := len(grid), len(grid[0])
    memo := make([][]int, m)
    for i := range memo {
        memo[i] = make([]int, n)
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i == m-1 { // 递归边界
            return grid[i][j]
        }
        p := &memo[i][j]
        if *p != 0 { // 之前计算过
            return *p
        }
        res := math.MaxInt
        for k, c := range moveCost[grid[i][j]] { // 移动到下一行的第 k 列
            res = min(res, dfs(i+1, k)+c)
        }
        *p = res + grid[i][j] // 记忆化
        return *p
    }
    ans := math.MaxInt
    for j := 0; j < n; j++ { // 枚举起点
        ans = min(ans, dfs(0, j))
    }
    return ans
}
```

```js [sol-JavaScript]
var minPathCost = function(grid, moveCost) {
    const m = grid.length, n = grid[0].length;
    const memo = Array(m).fill(null).map(() => Array(n).fill(0));

    function dfs(i, j) {
        if (i === m - 1) { // 递归边界
            return grid[i][j];
        }
        if (memo[i][j] !== 0) { // 之前计算过
            return memo[i][j];
        }
        let res = Infinity;
        for (let k = 0; k < n; k++) { // 移动到下一行的第 k 列
            res = Math.min(res, dfs(i + 1, k) + moveCost[grid[i][j]][k]);
        }
        return memo[i][j] = res + grid[i][j]; // 记忆化
    }

    let ans = Infinity;
    for (let j = 0; j < n; j++) { // 枚举起点
        ans = Math.min(ans, dfs(0, j));
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_path_cost(grid: Vec<Vec<i32>>, move_cost: Vec<Vec<i32>>) -> i32 {
        let m = grid.len();
        let n = grid[0].len();
        let mut memo = vec![vec![0; n]; m];
        fn dfs(i: usize, j: usize, memo: &mut Vec<Vec<i32>>, grid: &Vec<Vec<i32>>, move_cost: &Vec<Vec<i32>>) -> i32 {
            if i == grid.len() - 1 { // 递归边界
                return grid[i][j];
            }
            if memo[i][j] != 0 { // 之前计算过
                return memo[i][j];
            }
            let mut res = i32::MAX;
            for (k, &c) in move_cost[grid[i][j] as usize].iter().enumerate() {
                // 移动到下一行的第 k 列
                res = res.min(dfs(i + 1, k, memo, grid, move_cost) + c);
            }
            res += grid[i][j];
            memo[i][j] = res; // 记忆化
            res
        }
        let mut ans = i32::MAX;
        for j in 0..n { // 枚举起点
            ans = ans.min(dfs(0, j, &mut memo, &grid, &move_cost));
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn^2)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(mn)$，单个状态的计算时间为 $\mathcal{O}(n)$，所以动态规划的时间复杂度为 $\mathcal{O}(mn^2)$。
- 空间复杂度：$\mathcal{O}(mn)$。有多少个状态，$\textit{memo}$ 数组的大小就是多少。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

做法：

- $\textit{dfs}$ 改成 $f$ 数组；
- 递归改成循环（每个参数都对应一层循环）；
- 递归边界改成 $f$ 数组的初始值。
- 递归入口即为答案。

> 相当于之前是用递归去计算每个状态，现在是**枚举**并计算每个状态。

具体来说，$f[i][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是完全一样的，都表示从 $\textit{grid}[i][j]$ 出发，到达最后一行任意单元格的最小路径代价。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i][j] =\textit{grid}[i][j] + \min_{k=0}^{n-1} f[i+1][k] + \textit{moveCost}[\textit{grid}[i][j]][k]
$$

初始值 $f[m-1][j]=\textit{grid}[m-1][j]$，翻译自递归边界 $\textit{dfs}(m-1,j)=\textit{grid}[m-1][j]$。

答案为 $\min\limits_{j=0}^{n-1}f[0][j]$，翻译自递归入口 $\min\limits_{j=0}^{n-1}\textit{dfs}(0,j)$。

#### 答疑

**问**：如何思考循环顺序？什么时候要正序枚举，什么时候要倒序枚举？

**答**：这里有一个通用的做法：盯着状态转移方程，想一想，要计算 $f[i][j]$，必须先把 $f[i+1][\cdot]$ 算出来，那么只有 $i$ 从大到小枚举才能做到。对于 $j$ 来说，由于在计算 $f[i][j]$ 的时候，$f[i+1][\cdot]$ 已经全部计算完毕，所以 $j$ 无论是正序还是倒序枚举都可以。

```py [sol-Python3]
class Solution:
    def minPathCost(self, grid: List[List[int]], moveCost: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])
        f = [[inf] * n for _ in range(m)]
        f[-1] = grid[-1]
        for i in range(m - 2, -1, -1):
            for j, g in enumerate(grid[i]):
                for k, c in enumerate(moveCost[g]):  # 移动到下一行的第 k 列
                    f[i][j] = min(f[i][j], f[i + 1][k] + c)
                f[i][j] += g
        return min(f[0])
```

```java [sol-Java]
class Solution {
    public int minPathCost(int[][] grid, int[][] moveCost) {
        int m = grid.length;
        int n = grid[0].length;
        int[][] f = new int[m][n];
        f[m - 1] = grid[m - 1];
        for (int i = m - 2; i >= 0; i--) {
            for (int j = 0; j < n; j++) {
                f[i][j] = Integer.MAX_VALUE;
                for (int k = 0; k < n; k++) { // 移动到下一行的第 k 列
                    f[i][j] = Math.min(f[i][j], f[i + 1][k] + moveCost[grid[i][j]][k]);
                }
                f[i][j] += grid[i][j];
            }
        }
        int ans = Integer.MAX_VALUE;
        for (int res : f[0]) {
            ans = Math.min(ans, res);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minPathCost(vector<vector<int>>& grid, vector<vector<int>>& moveCost) {
        int m = grid.size(), n = grid[0].size();
        vector f(m, vector<int>(n, INT_MAX));
        f[m - 1] = grid[m - 1];
        for (int i = m - 2; i >= 0; i--) {
            for (int j = 0; j < n; j++) {
                for (int k = 0; k < n; k++) { // 移动到下一行的第 k 列
                    f[i][j] = min(f[i][j], f[i + 1][k] + moveCost[grid[i][j]][k]);
                }
                f[i][j] += grid[i][j];
            }
        }
        return ranges::min(f[0]);
    }
};
```

```go [sol-Go]
func minPathCost(grid [][]int, moveCost [][]int) int {
    m, n := len(grid), len(grid[0])
    f := make([][]int, m)
    for i := range f {
        f[i] = make([]int, n)
    }
    f[m-1] = grid[m-1]
    for i := m - 2; i >= 0; i-- {
        for j, g := range grid[i] {
            f[i][j] = math.MaxInt
            for k, c := range moveCost[g] { // 移动到下一行的第 k 列
                f[i][j] = min(f[i][j], f[i+1][k]+c)
            }
            f[i][j] += g
        }
    }
    return slices.Min(f[0])
}
```

```js [sol-JavaScript]
var minPathCost = function(grid, moveCost) {
    const m = grid.length, n = grid[0].length;
    const f = Array(m).fill(null).map(() => Array(n).fill(Infinity));
    f[m - 1] = grid[m - 1];
    for (let i = m - 2; i >= 0; i--) {
        for (let j = 0; j < n; j++) {
            for (let k = 0; k < n; k++) { // 移动到下一行的第 k 列
                f[i][j] = Math.min(f[i][j], f[i + 1][k] + moveCost[grid[i][j]][k]);
            }
            f[i][j] += grid[i][j];
        }
    }
    return Math.min(...f[0]);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_path_cost(mut grid: Vec<Vec<i32>>, move_cost: Vec<Vec<i32>>) -> i32 {
        let m = grid.len();
        let n = grid[0].len();
        let mut f = vec![vec![i32::MAX; n]; m];
        f[m - 1] = grid[m - 1].clone();
        for i in (0..m - 1).rev() {
            for (j, &g) in grid[i].iter().enumerate() {
                f[i][j] = g + f[i + 1]
                    .iter()
                    .zip(move_cost[g as usize].iter())
                    .map(|(&v, &c)| v + c)
                    .min()
                    .unwrap();
            }
        }
        *f[0].iter().min().unwrap()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn^2)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(mn)$。

## 五、原地计算

也可以直接把 $f[i][j]$ 保存在 $\textit{grid}[i][j]$ 中。

递推式变成：

$$
\textit{grid}[i][j] = \textit{grid}[i][j] + \min_{k=0}^{n-1} \textit{grid}[i+1][k] + \textit{moveCost}[\textit{grid}[i][j]][k]
$$

```py [sol-Python3]
class Solution:
    def minPathCost(self, grid: List[List[int]], moveCost: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])
        for i in range(m - 2, -1, -1):
            for j in range(n):
                grid[i][j] += min(g + c for g, c in zip(grid[i + 1], moveCost[grid[i][j]]))
        return min(grid[0])
```

```java [sol-Java]
class Solution {
    public int minPathCost(int[][] grid, int[][] moveCost) {
        int m = grid.length;
        int n = grid[0].length;
        for (int i = m - 2; i >= 0; i--) {
            for (int j = 0; j < n; j++) {
                int[] cost = moveCost[grid[i][j]];
                int res = Integer.MAX_VALUE;
                for (int k = 0; k < n; k++) {
                    res = Math.min(res, grid[i + 1][k] + cost[k]);
                }
                grid[i][j] += res;
            }
        }
        int ans = Integer.MAX_VALUE;
        for (int res : grid[0]) {
            ans = Math.min(ans, res);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minPathCost(vector<vector<int>>& grid, vector<vector<int>>& moveCost) {
        int m = grid.size(), n = grid[0].size();
        for (int i = m - 2; i >= 0; i--) {
            for (int j = 0; j < n; j++) {
                int res = INT_MAX;
                for (int k = 0; k < n; k++) {
                    res = min(res, grid[i + 1][k] + moveCost[grid[i][j]][k]);
                }
                grid[i][j] += res;
            }
        }
        return ranges::min(grid[0]);
    }
};
```

```go [sol-Go]
func minPathCost(grid [][]int, moveCost [][]int) int {
    for i := len(grid) - 2; i >= 0; i-- {
        for j, g := range grid[i] {
            res := math.MaxInt
            for k, c := range moveCost[g] {
                res = min(res, grid[i+1][k]+c)
            }
            grid[i][j] += res
        }
    }
    return slices.Min(grid[0])
}
```

```js [sol-JavaScript]
var minPathCost = function(grid, moveCost) {
    const m = grid.length, n = grid[0].length;
    for (let i = m - 2; i >= 0; i--) {
        for (let j = 0; j < n; j++) {
            let res = Infinity;
            for (let k = 0; k < n; k++) {
                res = Math.min(res, grid[i + 1][k] + moveCost[grid[i][j]][k]);
            }
            grid[i][j] += res;
        }
    }
    return Math.min(...grid[0]);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_path_cost(mut grid: Vec<Vec<i32>>, move_cost: Vec<Vec<i32>>) -> i32 {
        let m = grid.len();
        let n = grid[0].len();
        for i in (0..m - 1).rev() {
            for j in 0..n {
                grid[i][j] += grid[i + 1]
                    .iter()
                    .zip(move_cost[grid[i][j] as usize].iter())
                    .map(|(&g, &c)| g + c)
                    .min()
                    .unwrap();
            }
        }
        *grid[0].iter().min().unwrap()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn^2)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
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
