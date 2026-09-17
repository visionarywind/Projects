# 120. 三角形最小路径和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/triangle/
- 题目 slug：`triangle`
- 来源专题：动态规划
- 来源分类路径：二、网格图 DP / §2.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/triangle/solutions/2997752/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-42r2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推到空间优化！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/triangle/solutions/2997752/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-42r2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-42r2`
- topic id：`2997752`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 一、寻找子问题

由于 $\textit{triangle}$ 每排的下标都是从 $0$ 开始的，示例 1 的正确图示应该是**左对齐**，即

$$
\begin{aligned}
& 2          \\
& 3\ 4        \\
& 6\ 5\ 7      \\
& 4\ 1\ 8\ 3    \\
\end{aligned}
$$

我们要解决的问题（原问题）是：

- 从最上面的 $(0,0)$ 出发，移动到 $\textit{triangle}$ 的最后一排，路径上的元素之和的最小值。

考虑下一步往哪走：

- 走到 $(1,0)$，那么需要解决的问题为：从 $(1,0)$ 出发，移动到 $\textit{triangle}$ 最后一排，路径上的元素之和的最小值。
- 走到 $(1,1)$，那么需要解决的问题为：从 $(1,1)$ 出发，移动到 $\textit{triangle}$ 最后一排，路径上的元素之和的最小值。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

## 二、状态定义与状态转移方程

根据上面的讨论，定义状态为 $\textit{dfs}(i,j)$，表示从 $(i,j)$ 出发，移动到 $\textit{triangle}$ 最后一排，路径上的元素之和的最小值。

考虑下一步往哪走：

- 走到 $(i+1,j)$，那么需要解决的问题为：从 $(i+1,j)$ 出发，移动到 $\textit{triangle}$ 最后一排，路径上的元素之和的最小值，即 $\textit{dfs}(i+1,j)$。
- 走到 $(i+1,j+1)$，那么需要解决的问题为：从 $(i+1,j+1)$ 出发，移动到 $\textit{triangle}$ 最后一排，路径上的元素之和的最小值，即 $\textit{dfs}(i+1,j+1)$。

这两种情况取最小值，再加上当前位置的元素值 $\textit{triangle}[i][j]$ 就得到了 $\textit{dfs}(i,j)$，即

$$
\textit{dfs}(i,j) = \min(\textit{dfs}(i+1,j),\textit{dfs}(i+1,j+1)) + \textit{triangle}[i][j]
$$

**递归边界**：$\textit{dfs}(n-1,j)=\textit{triangle}[n-1][j]$。走到最后一排就无法再走了，路径上只有一个元素 $\textit{triangle}[n-1][j]$。

**递归入口**：$\textit{dfs}(0,0)$，这是原问题，也是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。本题由于 $\textit{triangle}[i][j]$ 可以是负数，所以改用 $-\infty$ 作为初始值。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def minimumTotal(self, triangle: List[List[int]]) -> int:
        n = len(triangle)
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int, j: int) -> int:
            if i == n - 1:
                return triangle[i][j]
            return min(dfs(i + 1, j), dfs(i + 1, j + 1)) + triangle[i][j]
        return dfs(0, 0)
```

```java [sol-Java]
class Solution {
    public int minimumTotal(List<List<Integer>> triangle) {
        int n = triangle.size();
        int[][] memo = new int[n][n];
        for (int[] row : memo) {
            Arrays.fill(row, Integer.MIN_VALUE); // Integer.MIN_VALUE 表示没有计算过
        }
        return dfs(triangle, 0, 0, memo);
    }

    private int dfs(List<List<Integer>> triangle, int i, int j, int[][] memo) {
        if (i == triangle.size() - 1) {
            return triangle.get(i).get(j);
        }
        if (memo[i][j] != Integer.MIN_VALUE) { // 之前计算过
            return memo[i][j];
        }
        return memo[i][j] = Math.min(dfs(triangle, i + 1, j, memo),
                dfs(triangle, i + 1, j + 1, memo)) + triangle.get(i).get(j);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumTotal(vector<vector<int>>& triangle) {
        int n = triangle.size();
        vector memo(n, vector<int>(n, INT_MIN)); // INT_MIN 表示没有计算过
        // lambda 递归
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i == n - 1) {
                return triangle[i][j];
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != INT_MIN) { // 之前计算过
                return res;
            }
            return res = min(dfs(i + 1, j), dfs(i + 1, j + 1)) + triangle[i][j];
        };
        return dfs(0, 0);
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int dfs(int** triangle, int i, int j, int n, int** memo) {
    if (i == n - 1) {
        return triangle[i][j];
    }
    if (memo[i][j] != 0x3f3f3f3f) { // 之前计算过
        return memo[i][j];
    }
    int res1 = dfs(triangle, i + 1, j, n, memo);
    int res2 = dfs(triangle, i + 1, j + 1, n, memo);
    return memo[i][j] = MIN(res1, res2) + triangle[i][j];
}

int minimumTotal(int** triangle, int triangleSize, int* triangleColSize) {
    int** memo = malloc(triangleSize * sizeof(int*));
    for (int i = 0; i < triangleSize; i++) {
        memo[i] = malloc((i + 1) * sizeof(int));
        memset(memo[i], 0x3f, triangleColSize[i] * sizeof(int));
    }

    int ans = dfs(triangle, 0, 0, triangleSize, memo);

    for (int i = 0; i < triangleSize; i++) {
        free(memo[i]);
    }
    free(memo);
    return ans;
}
```

```go [sol-Go]
func minimumTotal(triangle [][]int) int {
    n := len(triangle)
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, n)
        for j := range memo[i] {
            memo[i][j] = math.MinInt // math.MinInt 表示没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i == n-1 {
            return triangle[i][j]
        }
        p := &memo[i][j]
        if *p != math.MinInt { // 之前计算过
            return *p
        }
        *p = min(dfs(i+1, j), dfs(i+1, j+1)) + triangle[i][j]
        return *p
    }
    return dfs(0, 0)
}
```

```js [sol-JavaScript]
var minimumTotal = function(triangle) {
    const n = triangle.length;
    const memo = Array.from({ length: n }, () => Array(n));
    function dfs(i, j) {
        if (i === n - 1) {
            return triangle[i][j];
        }
        if (memo[i][j] !== undefined) { // 之前计算过
            return memo[i][j];
        }
        return memo[i][j] = Math.min(dfs(i + 1, j), dfs(i + 1, j + 1)) + triangle[i][j];
    }
    return dfs(0, 0);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_total(triangle: Vec<Vec<i32>>) -> i32 {
        fn dfs(i: usize, j: usize, triangle: &[Vec<i32>], memo: &mut [Vec<i32>]) -> i32 {
            if i == triangle.len() - 1 {
                return triangle[i][j];
            }
            if memo[i][j] != i32::MIN { // 之前计算过
                return memo[i][j];
            }
            memo[i][j] = dfs(i + 1, j, triangle, memo).min(dfs(i + 1, j + 1, triangle, memo)) + triangle[i][j];
            memo[i][j]
        }
        let n = triangle.len();
        let mut memo = vec![vec![i32::MIN; n]; n]; // i32::MIN 表示没有计算过
        dfs(0, 0, &triangle, &mut memo)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{triangle}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n^2)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示从 $(i,j)$ 出发，移动到 $\textit{triangle}$ 最后一排，路径上的元素之和的最小值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i][j] = \min(f[i+1][j],f[i+1][j+1]) + \textit{triangle}[i][j]
$$

初始值 $f[n-1][j]=\textit{triangle}[n-1][j]$，翻译自递归边界 $\textit{dfs}(n-1,j)=\textit{triangle}[n-1][j]$。

答案为 $f[0][0]$，翻译自递归入口 $\textit{dfs}(0,0)$。

#### 答疑

**问**：如何思考循环顺序？什么时候要正序枚举，什么时候要倒序枚举？

**答**：这里有一个通用的做法：盯着状态转移方程，想一想，要计算 $f[i][j]$，必须先把 $f[i+1][j]$ 和 $f[i+1][j+1]$ 算出来，那么只有 $i$ 从大到小枚举才能做到。对于 $j$ 来说，正序倒序都可以。

```py [sol-Python3]
class Solution:
    def minimumTotal(self, triangle: List[List[int]]) -> int:
        n = len(triangle)
        f = [[0] * (i + 1) for i in range(n)]
        f[-1] = triangle[-1]
        for i in range(n - 2, -1, -1):
            for j, x in enumerate(triangle[i]):
                f[i][j] = min(f[i + 1][j], f[i + 1][j + 1]) + x
        return f[0][0]
```

```java [sol-Java]
class Solution {
    public int minimumTotal(List<List<Integer>> triangle) {
        int n = triangle.size();
        int[][] f = new int[n][n];
        for (int j = 0; j < n; j++) {
            f[n - 1][j] = triangle.get(n - 1).get(j);
        }
        for (int i = n - 2; i >= 0; i--) {
            for (int j = 0; j <= i; j++) {
                f[i][j] = Math.min(f[i + 1][j], f[i + 1][j + 1]) + triangle.get(i).get(j);
            }
        }
        return f[0][0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumTotal(vector<vector<int>>& triangle) {
        int n = triangle.size();
        vector f(n, vector<int>(n));
        f[n - 1] = triangle[n - 1];
        for (int i = n - 2; i >= 0; i--) {
            for (int j = 0; j <= i; j++) {
                f[i][j] = min(f[i + 1][j], f[i + 1][j + 1]) + triangle[i][j];
            }
        }
        return f[0][0];
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int minimumTotal(int** triangle, int triangleSize, int* triangleColSize) {
    int** f = malloc(triangleSize * sizeof(int*));
    for (int i = 0; i < triangleSize; i++) {
        f[i] = malloc((i + 1) * sizeof(int));
    }
    f[triangleSize - 1] = triangle[triangleSize - 1];
    for (int i = triangleSize - 2; i >= 0; i--) {
        for (int j = 0; j <= i; j++) {
            f[i][j] = MIN(f[i + 1][j], f[i + 1][j + 1]) + triangle[i][j];
        }
    }
    int ans = f[0][0];
    for (int i = 0; i < triangleSize; i++) {
        free(f[i]);
    }
    free(f);
    return ans;
}
```

```go [sol-Go]
func minimumTotal(triangle [][]int) int {
    n := len(triangle)
    f := make([][]int, n)
    for i := range f {
        f[i] = make([]int, i+1)
    }
    f[n-1] = triangle[n-1]
    for i := n - 2; i >= 0; i-- {
        for j, x := range triangle[i] {
            f[i][j] = min(f[i+1][j], f[i+1][j+1]) + x
        }
    }
    return f[0][0]
}
```

```js [sol-JavaScript]
var minimumTotal = function(triangle) {
    const n = triangle.length;
    const f = Array.from({ length: n }, () => Array(n));
    f[n - 1] = triangle[n - 1];
    for (let i = n - 2; i >= 0; i--) {
        for (let j = 0; j <= i; j++) {
            f[i][j] = Math.min(f[i + 1][j], f[i + 1][j + 1]) + triangle[i][j];
        }
    }
    return f[0][0];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_total(triangle: Vec<Vec<i32>>) -> i32 {
        let n = triangle.len();
        let mut f = vec![vec![0; n]; n];
        f[n - 1] = triangle[n - 1].clone();
        for i in (0..n - 1).rev() {
            for (j, &x) in triangle[i].iter().enumerate() {
                f[i][j] = f[i + 1][j].min(f[i + 1][j + 1]) + x;
            }
        }
        f[0][0]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{triangle}$ 的长度。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 五、空间优化

### 写法一

由于 $f[i][j]$ 只依赖正下方和右下方的相邻数据，可以去掉 $f$ 的第一个维度。

```py [sol-Python3]
class Solution:
    def minimumTotal(self, triangle: List[List[int]]) -> int:
        n = len(triangle)
        f = triangle[-1][:]
        for i in range(n - 2, -1, -1):
            for j, x in enumerate(triangle[i]):
                f[j] = min(f[j], f[j + 1]) + x
        return f[0]
```

```java [sol-Java]
class Solution {
    public int minimumTotal(List<List<Integer>> triangle) {
        int n = triangle.size();
        int[] f = new int[n];
        for (int j = 0; j < n; j++) {
            f[j] = triangle.get(n - 1).get(j);
        }
        for (int i = n - 2; i >= 0; i--) {
            for (int j = 0; j <= i; j++) {
                f[j] = Math.min(f[j], f[j + 1]) + triangle.get(i).get(j);
            }
        }
        return f[0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumTotal(vector<vector<int>>& triangle) {
        int n = triangle.size();
        auto f = triangle[n - 1];
        for (int i = n - 2; i >= 0; i--) {
            for (int j = 0; j <= i; j++) {
                f[j] = min(f[j], f[j + 1]) + triangle[i][j];
            }
        }
        return f[0];
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int minimumTotal(int** triangle, int triangleSize, int* triangleColSize) {
    int* f = triangle[triangleSize - 1];
    for (int i = triangleSize - 2; i >= 0; i--) {
        for (int j = 0; j <= i; j++) {
            f[j] = MIN(f[j], f[j + 1]) + triangle[i][j];
        }
    }
    return f[0];
}
```

```go [sol-Go]
func minimumTotal(triangle [][]int) int {
    n := len(triangle)
    f := slices.Clone(triangle[n-1])
    for i := n - 2; i >= 0; i-- {
        for j, x := range triangle[i] {
            f[j] = min(f[j], f[j+1]) + x
        }
    }
    return f[0]
}
```

```js [sol-JavaScript]
var minimumTotal = function(triangle) {
    const n = triangle.length;
    const f = triangle[n - 1].slice();
    for (let i = n - 2; i >= 0; i--) {
        for (let j = 0; j <= i; j++) {
            f[j] = Math.min(f[j], f[j + 1]) + triangle[i][j];
        }
    }
    return f[0];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_total(triangle: Vec<Vec<i32>>) -> i32 {
        let n = triangle.len();
        let mut f = triangle[n - 1].clone();
        for i in (0..n - 1).rev() {
            for (j, &x) in triangle[i].iter().enumerate() {
                f[j] = f[j].min(f[j + 1]) + x;
            }
        }
        f[0]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{triangle}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。

### 写法二

也可以直接把 $\textit{triangle}$ 当作 $f$ 数组。

```py [sol-Python3]
class Solution:
    def minimumTotal(self, f: List[List[int]]) -> int:
        for i in range(len(f) - 2, -1, -1):
            for j in range(i + 1):
                f[i][j] += min(f[i + 1][j], f[i + 1][j + 1])
        return f[0][0]
```

```java [sol-Java]
class Solution {
    public int minimumTotal(List<List<Integer>> f) {
        for (int i = f.size() - 2; i >= 0; i--) {
            for (int j = 0; j <= i; j++) {
                f.get(i).set(j, f.get(i).get(j) + Math.min(f.get(i + 1).get(j), f.get(i + 1).get(j + 1)));
            }
        }
        return f.get(0).get(0);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumTotal(vector<vector<int>>& f) {
        for (int i = f.size() - 2; i >= 0; i--) {
            for (int j = 0; j <= i; j++) {
                f[i][j] += min(f[i + 1][j], f[i + 1][j + 1]);
            }
        }
        return f[0][0];
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int minimumTotal(int** f, int triangleSize, int* triangleColSize) {
    for (int i = triangleSize - 2; i >= 0; i--) {
        for (int j = 0; j <= i; j++) {
            f[i][j] += MIN(f[i + 1][j], f[i + 1][j + 1]);
        }
    }
    return f[0][0];
}
```

```go [sol-Go]
func minimumTotal(f [][]int) int {
    for i := len(f) - 2; i >= 0; i-- {
        for j := range f[i] {
            f[i][j] += min(f[i+1][j], f[i+1][j+1])
        }
    }
    return f[0][0]
}
```

```js [sol-JavaScript]
var minimumTotal = function(f) {
    for (let i = f.length - 2; i >= 0; i--) {
        for (let j = 0; j <= i; j++) {
            f[i][j] += Math.min(f[i + 1][j], f[i + 1][j + 1]);
        }
    }
    return f[0][0];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_total(mut f: Vec<Vec<i32>>) -> i32 {
        for i in (0..f.len() - 1).rev() {
            for j in 0..=i {
                f[i][j] += f[i + 1][j].min(f[i + 1][j + 1]);
            }
        }
        f[0][0]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{triangle}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面动态规划题单的「**二、网格图 DP**」。

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
