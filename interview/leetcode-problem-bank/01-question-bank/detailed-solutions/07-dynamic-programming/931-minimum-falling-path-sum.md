# 931. 下降路径最小和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-falling-path-sum/
- 题目 slug：`minimum-falling-path-sum`
- 来源专题：动态规划
- 来源分类路径：二、网格图 DP / §2.1 基础
- 难度分：1573
- 外部题解来源：https://leetcode.cn/problems/minimum-falling-path-sum/solutions/2341851/cong-di-gui-dao-di-tui-jiao-ni-yi-bu-bu-2cwkb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从递归到递推，教你一步步思考动态规划！（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/minimum-falling-path-sum/solutions/2341851/cong-di-gui-dao-di-tui-jiao-ni-yi-bu-bu-2cwkb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-di-gui-dao-di-tui-jiao-ni-yi-bu-bu-2cwkb`
- topic id：`2341851`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 前置知识：动态规划入门

[动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)

## 一、启发思考：寻找子问题

为了方便后面改成递推，我们从下往上移动。

对于示例 1，思考「第一步」要怎么走。

假设从最后一排的 $\textit{matrix}[2][1]=8$ 出发：

- 往左上方走一步，问题变成从 $\textit{matrix}[1][0]=6$ 出发，向上走到第一排的路径最小和。
- 往正上方走一步，问题变成从 $\textit{matrix}[1][1]=5$ 出发，向上走到第一排的路径最小和。
- 往右上方走一步，问题变成从 $\textit{matrix}[1][2]=4$ 出发，向上走到第一排的路径最小和。

由于这些都是**和原问题相似的、规模更小的子问题**，所以可以用**递归**解决。

> 注：动态规划有「选或不选」和「枚举选哪个」这两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「枚举选哪个」。

## 二、递归怎么写：状态定义与状态转移方程

由于每个子问题计算的都是从某个格子出发的路径最小和，不妨定义 $\textit{dfs}(r,c)$ 表示从 $\textit{matrix}[r][c]$ 出发，向上走到第一排的路径最小和。

考虑往哪个方向走一步：

- 往左上方走一步，问题变成从 $\textit{matrix}[r-1][c-1]$ 出发，向上走到第一排的路径最小和。
- 往正上方走一步，问题变成从 $\textit{matrix}[r-1][c]$ 出发，向上走到第一排的路径最小和。
- 往右上方走一步，问题变成从 $\textit{matrix}[r-1][c+1]$ 出发，向上走到第一排的路径最小和。

这三种情况取最小值，加上 $\textit{matrix}[r][c]$，就得到了 $\textit{dfs}(r,c)$。写成式子就是

$$
\textit{dfs}(r,c) = \min(\textit{dfs}(r-1,c-1),\textit{dfs}(r-1,c),\textit{dfs}(r-1,c+1)) + \textit{matrix}[r][c]
$$

递归边界：$\textit{dfs}(0,c)=\textit{matrix}[0][c]$ 以及 $\textit{dfs}(r,-1)=\textit{dfs}(r,n)=\infty$，出界是不合法的，设置成无穷大，这样取 $\min$ 的时候就会自动忽略不合法的情况。

递归入口：$\textit{dfs}(n-1,c)$，遍历所有 $c$，取最小值，即为答案。

```py [sol-Python3]
# 会超时的递归代码
class Solution:
    def minFallingPathSum(self, matrix: List[List[int]]) -> int:
        n = len(matrix)
        # dfs(r, c) 表示从 matrix[r][c] 出发，向上走到第一行的最小路径和
        def dfs(r: int, c: int) -> int:
            if c < 0 or c >= n:  # 出界
                return inf
            if r == 0:  # 到达第一行
                return matrix[0][c]
            return min(dfs(r - 1, c - 1), dfs(r - 1, c), dfs(r - 1, c + 1)) + matrix[r][c]
        return min(dfs(n - 1, i) for i in range(n))  # 枚举起点，取最小值
```

```java [sol-Java]
// 会超时的递归代码
class Solution {
    private int[][] matrix;

    public int minFallingPathSum(int[][] matrix) {
        this.matrix = matrix;
        int n = matrix.length;
        int ans = Integer.MAX_VALUE;
        for (int c = 0; c < n; c++) {
            ans = Math.min(ans, dfs(n - 1, c));
        }
        return ans;
    }

    private int dfs(int r, int c) {
        if (c < 0 || c >= matrix.length) return Integer.MAX_VALUE; // 出界
        if (r == 0) return matrix[0][c]; // 到达第一行
        return Math.min(Math.min(dfs(r - 1, c - 1), dfs(r - 1, c)), dfs(r - 1, c + 1)) + matrix[r][c];
    }
}
```

```cpp [sol-C++]
// 会超时的递归代码
class Solution {
public:
    int minFallingPathSum(vector<vector<int>>& matrix) {
        int n = matrix.size();
        auto dfs = [&](this auto&& dfs, int r, int c) -> int {
            if (c < 0 || c >= n) return INT_MAX; // 出界
            if (r == 0) return matrix[0][c]; // 到达第一行
            return min(min(dfs(r - 1, c - 1), dfs(r - 1, c)), dfs(r - 1, c + 1)) + matrix[r][c];
        };
        int ans = INT_MAX;
        for (int c = 0; c < n; c++) {
            ans = min(ans, dfs(n - 1, c));
        }
        return ans;
    }
};
```

```go [sol-Go]
// 会超时的递归代码
func minFallingPathSum(matrix [][]int) int {
    n := len(matrix)
    var dfs func(int, int) int
    dfs = func(r, c int) int {
        if c < 0 || c >= n { // 出界
            return math.MaxInt
        }
        if r == 0 { // 到达第一行
            return matrix[0][c]
        }
        return min(min(dfs(r-1, c-1), dfs(r-1, c)), dfs(r-1, c+1)) + matrix[r][c]
    }
    ans := math.MaxInt
    for c := range n {
        ans = min(ans, dfs(n-1, c))
    }
    return ans
}
```

```js [sol-JavaScript]
// 会超时的递归代码
var minFallingPathSum = function(matrix) {
    const n = matrix.length;
    function dfs(r, c) {
        if (c < 0 || c >= n) return Number.MAX_SAFE_INTEGER; // 出界
        if (r === 0) return matrix[0][c]; // 到达第一行
        return Math.min(dfs(r - 1, c - 1), dfs(r - 1, c), dfs(r - 1, c + 1)) + matrix[r][c];
    }

    let ans = Number.MAX_SAFE_INTEGER;
    for (let c = 0; c < n; c++)
        ans = Math.min(ans, dfs(n - 1, c));
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n3^n)$，其中 $n$ 为 $\textit{matrix}$ 的长度。搜索树可以近似为一棵三叉树，树高为 $\mathcal{O}(n)$，所以节点个数至多为 $\mathcal{O}(3^n)$。从 $n$ 个起点出发递归，所以总的时间复杂度为 $\mathcal{O}(n3^n)$。
- 空间复杂度：$\mathcal{O}(n)$。递归需要 $\mathcal{O}(n)$ 的栈空间。

## 三、递归 + 记录返回值 = 记忆化搜索

上面的做法太慢了，怎么优化呢？

由于有多种方式可以移动到同一个格子，那么整个递归中必然有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组（或哈希表）中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

请注意：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(r,c)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了。初始值一般设为 $-1$，但是本题 $\textit{matrix}[i][j]$ 可以为负数，那么初始值可以改成一个很大或者很小的数。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

```py [sol-Python3]
class Solution:
    def minFallingPathSum(self, matrix: List[List[int]]) -> int:
        n = len(matrix)
        # dfs(r, c) 表示从 matrix[r][c] 出发，向上走到第一行的最小路径和
        @cache  # 记忆化搜索
        def dfs(r: int, c: int) -> int:
            if c < 0 or c >= n:  # 出界
                return inf
            if r == 0:  # 到达第一行
                return matrix[0][c]
            return min(dfs(r - 1, c - 1), dfs(r - 1, c), dfs(r - 1, c + 1)) + matrix[r][c]
        return min(dfs(n - 1, i) for i in range(n))  # 枚举起点，取最小值
```

```java [sol-Java]
class Solution {
    private int[][] matrix, memo;

    public int minFallingPathSum(int[][] matrix) {
        this.matrix = matrix;
        int n = matrix.length;
        memo = new int[n][n];
        for (int i = 0; i < n; i++)
            Arrays.fill(memo[i], Integer.MIN_VALUE);

        int ans = Integer.MAX_VALUE;
        for (int c = 0; c < n; c++) {
            ans = Math.min(ans, dfs(n - 1, c));
        }
        return ans;
    }

    private int dfs(int r, int c) {
        if (c < 0 || c >= matrix.length) return Integer.MAX_VALUE; // 出界
        if (r == 0) return matrix[0][c]; // 到达第一行
        if (memo[r][c] != Integer.MIN_VALUE) return memo[r][c]; // 之前算过了
        return memo[r][c] = Math.min(Math.min(
                dfs(r - 1, c - 1), dfs(r - 1, c)), dfs(r - 1, c + 1)) + matrix[r][c];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minFallingPathSum(vector<vector<int>>& matrix) {
        int n = matrix.size(), memo[n][n];
        memset(memo, 0x3f, sizeof(memo));
        auto dfs = [&](this auto dfs, int r, int c) -> int {
            if (c < 0 || c >= n) return INT_MAX; // 出界
            if (r == 0) return matrix[0][c]; // 到达第一行
            int &res = memo[r][c]; // 注意这里是引用，下面会直接修改 memo[r][c]
            if (res != 0x3f3f3f3f) return res; // 之前算过了
            return res = min(min(dfs(r - 1, c - 1), dfs(r - 1, c)), dfs(r - 1, c + 1)) + matrix[r][c];
        };
        int ans = INT_MAX;
        for (int c = 0; c < n; c++) {
            ans = min(ans, dfs(n - 1, c));
        }
        return ans;
    }
};
```

```go [sol-Go]
func minFallingPathSum(matrix [][]int) int {
    n := len(matrix)
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, n)
        for j := range memo[i] {
            memo[i][j] = math.MinInt
        }
    }
    var dfs func(int, int) int
    dfs = func(r, c int) int {
        if c < 0 || c >= n { // 出界
            return math.MaxInt
        }
        if r == 0 { // 到达第一行
            return matrix[0][c]
        }
        p := &memo[r][c]
        if *p == math.MinInt { // 没有计算过
            *p = min(min(dfs(r-1, c-1), dfs(r-1, c)), dfs(r-1, c+1)) + matrix[r][c]
        }
        return *p
    }
    ans := math.MaxInt
    for c := range n {
        ans = min(ans, dfs(n-1, c))
    }
    return ans
}
```

```js [sol-JavaScript]
var minFallingPathSum = function(matrix) {
    const n = matrix.length;
    let memo = Array(n).fill(null).map(() => Array(n).fill(Number.MIN_SAFE_INTEGER));

    function dfs(r, c) {
        if (c < 0 || c >= n) return Number.MAX_SAFE_INTEGER; // 出界
        if (r === 0) return matrix[0][c]; // 到达第一行
        if (memo[r][c] !== Number.MIN_SAFE_INTEGER) return memo[r][c]; // 之前算过了
        return memo[r][c] = Math.min(
            dfs(r - 1, c - 1), dfs(r - 1, c), dfs(r - 1, c + 1)) + matrix[r][c];
    }

    let ans = Number.MAX_SAFE_INTEGER;
    for (let c = 0; c < n; c++)
        ans = Math.min(ans, dfs(n - 1, c));
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{matrix}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n^2)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

做法：

- $\textit{dfs}$ 改成 $f$ 数组；
- 递归改成循环（每个参数都对应一层循环）；
- 递归边界改成 $f$ 数组的初始值。

> 相当于之前是用递归去计算每个状态，现在是（按照某种顺序）枚举并计算每个状态。

具体来说，$f[r][c]$ 的含义和 $\textit{dfs}(r,c)$ 的含义是一致的，都表示从 $\textit{matrix}[r][c]$ 出发，向上走到第一排的路径最小和。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 的一致：

$$
f[r][c] =  \min(f[r-1][c-1],f[r-1][c],f[r-1][c+1]) + \textit{matrix}[r][c]
$$

但是，这种定义方式**没有状态能表示递归边界**，即 $c=-1$ 和 $c=n$ 的情况。

解决办法：在 $f$ 的最左边和最右边各增加一列状态，那么原有的状态就要全部向右偏移一位，把 $f[r][c]$ 改为 $f[r][c+1]$。

修改后 $f[r][c+1]$ 表示从 $\textit{matrix}[r][c]$ 出发，向上走到第一排的路径最小和。此时 $f[r][0]$ 和 $f[r][n+1]$ 就对应递归边界了。

修改后的递推式为

$$
f[r][c+1] = \min(f[r-1][c],f[r-1][c+1],f[r-1][c+2]) + \textit{matrix}[r][c]
$$

> 注：另外一种写法是 $c$ 从 $1$ 开始遍历，从而避免数组越界，即 $f[r][c] =  \min(f[r-1][c-1],f[r-1][c],f[r-1][c+1]) + \textit{matrix}[r][c-1]$。

初始值 $f[0][c+1]=\textit{matrix}[0][c], f[r][0]=f[r][n+1]=\infty$。

答案为所有 $f[n-1][c]$ 的最小值。

```py [sol-Python3]
class Solution:
    def minFallingPathSum(self, matrix: List[List[int]]) -> int:
        n = len(matrix)
        f = [[inf] * (n + 2) for _ in range(n)]
        f[0][1: n + 1] = matrix[0]
        for r in range(1, n):
            for c in range(n):
                f[r][c + 1] = min(f[r - 1][c], f[r - 1][c + 1], f[r - 1][c + 2]) + matrix[r][c]
        return min(f[-1])
```

```java [sol-Java]
class Solution {
    public int minFallingPathSum(int[][] matrix) {
        int n = matrix.length;
        var f = new int[n][n + 2];
        System.arraycopy(matrix[0], 0, f[0], 1, n);
        for (int r = 1; r < n; r++) {
            f[r - 1][0] = f[r - 1][n + 1] = Integer.MAX_VALUE;
            for (int c = 0; c < n; c++)
                f[r][c + 1] = Math.min(Math.min(f[r - 1][c], f[r - 1][c + 1]), f[r - 1][c + 2]) + matrix[r][c];
        }
        int ans = Integer.MAX_VALUE;
        for (int c = 1; c <= n; c++) {
            ans = Math.min(ans, f[n - 1][c]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minFallingPathSum(vector<vector<int>>& matrix) {
        int n = matrix.size(), f[n][n + 2];
        memset(f, 0x3f, sizeof(f));
        for (int i = 0; i < n; i++)
            f[0][i + 1] = matrix[0][i];
        for (int r = 1; r < n; r++)
            for (int c = 0; c < n; c++)
                f[r][c + 1] = min(min(f[r - 1][c], f[r - 1][c + 1]), f[r - 1][c + 2]) + matrix[r][c];
        return *min_element(f[n - 1] + 1, f[n - 1] + 1 + n);
    }
};
```

```go [sol-Go]
func minFallingPathSum(matrix [][]int) int {
    n := len(matrix)
    f := make([][]int, n)
    for i := range f {
        f[i] = make([]int, n+2)
        f[i][0], f[i][n+1] = math.MaxInt, math.MaxInt
    }
    copy(f[0][1:], matrix[0])
    for r := 1; r < n; r++ {
        for c, x := range matrix[r] {
            f[r][c+1] = min(min(f[r-1][c], f[r-1][c+1]), f[r-1][c+2]) + x
        }
    }
    return slices.Min(f[n-1])
}
```

```js [sol-JavaScript]
var minFallingPathSum = function(matrix) {
    const n = matrix.length, inf = Number.MAX_SAFE_INTEGER;
    let f = Array(n).fill(null).map(() => Array(n + 2).fill(inf));
    f[0] = [inf, ...matrix[0], inf];
    for (let r = 1; r < n; r++)
        for (let c = 0; c < n; c++)
            f[r][c + 1] = Math.min(f[r - 1][c], f[r - 1][c + 1], f[r - 1][c + 2]) + matrix[r][c];
    return Math.min(...f[n - 1]); // 也可以 _.min(f[n - 1]);
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{matrix}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n^2)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 五、空间优化

观察上面的状态转移方程，在计算 $f[r][\cdot]$ 时，只会用到上一排的 $f[r-1][\cdot]$，不会用到 $< r-1$ 的状态。

因此可以像 [0-1 背包](https://www.bilibili.com/video/BV16Y411v7Y6/) 那样，反复利用同一个长为 $n+2$ 的一维数组。

状态转移方程改为

$$
f[c+1] = \min(f[c],f[c+1],f[c+2]) + \textit{matrix}[r][c]
$$

注意这里 $f[c]$ 必须是上一排算出的，为避免它被当前排覆盖，可以用一个变量 $\textit{pre}$ 暂存。

```py [sol-Python3]
class Solution:
    def minFallingPathSum(self, matrix: List[List[int]]) -> int:
        n = len(matrix)
        f = [inf] + matrix[0] + [inf]
        for row in matrix[1:]:
            pre = f[0]  # 充当 f[c]
            for c, x in enumerate(row):
                pre, f[c + 1] = f[c + 1], min(pre, f[c + 1], f[c + 2]) + x
        return min(f)
```

```java [sol-Java]
class Solution {
    public int minFallingPathSum(int[][] matrix) {
        int n = matrix.length;
        var f = new int[n + 2];
        f[0] = f[n + 1] = Integer.MAX_VALUE;
        System.arraycopy(matrix[0], 0, f, 1, n);
        for (int r = 1; r < n; r++) {
            int pre = f[0];
            for (int c = 0; c < n; c++) {
                int tmp = pre;
                pre = f[c + 1];
                f[c + 1] = Math.min(tmp, Math.min(f[c + 1], f[c + 2])) + matrix[r][c];
            }
        }
        int ans = Integer.MAX_VALUE;
        for (int c = 1; c <= n; c++) {
            ans = Math.min(ans, f[c]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minFallingPathSum(vector<vector<int>>& matrix) {
        int n = matrix.size(), f[n + 2];
        f[0] = f[n + 1] = INT_MAX;
        for (int i = 0; i < n; i++)
            f[i + 1] = matrix[0][i];
        for (int r = 1; r < n; r++) {
            int pre = f[0];
            for (int c = 0; c < n; c++) {
                int tmp = pre;
                pre = f[c + 1];
                f[c + 1] = min(tmp, min(f[c + 1], f[c + 2])) + matrix[r][c];
            }
        }
        return *min_element(f + 1, f + 1 + n);
    }
};
```

```go [sol-Go]
func minFallingPathSum(matrix [][]int) int {
    f := append(append([]int{math.MaxInt}, matrix[0]...), math.MaxInt)
    for _, row := range matrix[1:] {
        pre := f[0]
        for c, x := range row {
            pre, f[c+1] = f[c+1], min(pre, min(f[c+1], f[c+2]))+x
        }
    }
    return slices.Min(f)
}
```

```js [sol-JavaScript]
var minFallingPathSum = function(matrix) {
    const n = matrix.length, inf = Number.MAX_SAFE_INTEGER;
    let f = [inf, ...matrix[0], inf];
    for (let r = 1; r < n; r++) {
        let pre = f[0];
        for (let c = 0; c < n; c++)
            [pre, f[c + 1]] = [f[c + 1], Math.min(pre, f[c + 1], f[c + 2]) + matrix[r][c]];
    }
    return Math.min(...f); // 也可以 _.min(f);
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{matrix}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n^2)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n)$。

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
