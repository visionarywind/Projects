# 1155. 掷骰子等于目标和的方法数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-dice-rolls-with-target-sum/
- 题目 slug：`number-of-dice-rolls-with-target-sum`
- 来源专题：动态规划
- 来源分类路径：三、背包 / §3.4 分组背包
- 难度分：1654
- 外部题解来源：https://leetcode.cn/problems/number-of-dice-rolls-with-target-sum/solutions/2495836/ji-bai-100cong-ji-yi-hua-sou-suo-dao-di-421ab/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从记忆化搜索到递推，教你一步步思考动态规划（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/number-of-dice-rolls-with-target-sum/solutions/2495836/ji-bai-100cong-ji-yi-hua-sou-suo-dao-di-421ab/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-bai-100cong-ji-yi-hua-sou-suo-dao-di-421ab`
- topic id：`2495836`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、启发思考：寻找子问题

假设每个骰子都有 $6$ 个面，现在要计算用 $3$ 个骰子掷出数字之和恰好等于 $13$ 的方案数。

掷一个骰子，枚举掷出的数字（正面朝上的数字）：

- 掷出了 $1$：问题变成「用 $2$ 个骰子掷出数字之和恰好等于 $12$ 的方案数」。
- 掷出了 $2$：问题变成「用 $2$ 个骰子掷出数字之和恰好等于 $11$ 的方案数」。
- ...
- 掷出了 $6$：问题变成「用 $2$ 个骰子掷出数字之和恰好等于 $7$ 的方案数」。

这 $6$ 种情况，都会把原问题变成一个**和原问题相似的、规模更小的子问题**，所以可以用**递归**解决。

> 注：动态规划有「选或不选」和「枚举选哪个」这两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「枚举选哪个」。

## 二、递归怎么写：状态定义与状态转移方程

不同的子问题，需要处理的「骰子个数」和「数字之和」是不同的，所以用两个参数就可以表示一个子问题。

具体地，定义 $\textit{dfs}(i,j)$ 表示用 $i$ 个骰子掷出数字之和恰好等于 $j$ 的方案数。

掷一个骰子，枚举掷出的数字（正面朝上的数字）：

- 掷出了 $1$：问题变成「用 $i-1$ 个骰子掷出数字之和恰好等于 $j-1$ 的方案数」。
- 掷出了 $2$：问题变成「用 $i-1$ 个骰子掷出数字之和恰好等于 $j-2$ 的方案数」。
- ...
- 掷出了 $k$：问题变成「用 $i-1$ 个骰子掷出数字之和恰好等于 $j-k$ 的方案数」。

根据加法原理，累加这 $k$ 种情况的方案数之和，就得到了 $\textit{dfs}(i,j)$。写成式子就是

$$
\textit{dfs}(i,j) = \textit{dfs}(i-1,j-1) + \textit{dfs}(i-1,j-2) + \cdots + \textit{dfs}(i-1,j-k)
$$

递归边界：$\textit{dfs}(0, 0)=1$，表示没有骰子，数字之和恰好等于 $0$ 有 $1$ 种方案。对于 $j<0$ 和 $i=0,j>0$ 的情况，都无法做到，返回 $0$。

递归入口：$\textit{dfs}(n,\textit{target})$，也就是答案。

此外，递归之前可以判断下：如果 $\textit{target}<n$ 或者 $\textit{target}>nk$，那么无法得到 $\textit{target}$，返回 $0$。

关于取模的技巧，见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

```py [sol-Python3]
# 会超时的递归代码
class Solution:
    def numRollsToTarget(self, n: int, k: int, target: int) -> int:
        if not (n <= target <= n * k):
            return 0  # 无法组成 target
        MOD = 10 ** 9 + 7
        def dfs(i: int, j: int) -> int:
            if j < 0:
                return 0  # j 不能是负数
            if i == 0:
                return int(j == 0)
            res = 0
            for x in range(1, k + 1):  # 掷出了 x
                res += dfs(i - 1, j - x)
            return res % MOD
        return dfs(n, target)
```

```java [sol-Java]
// 会超时的递归代码
class Solution {
    private static final int MOD = 1_000_000_007;

    public int numRollsToTarget(int n, int k, int target) {
        if (target < n || target > n * k) {
            return 0; // 无法组成 target
        }
        return dfs(n, target, k);
    }

    private int dfs(int i, int j, int k) {
        if (j < 0) {
            return 0; // j 不能是负数
        }
        if (i == 0) {
            return j == 0 ? 1 : 0;
        }
        int res = 0;
        for (int x = 1; x <= k; x++) { // 掷出了 x
            res = (res + dfs(i - 1, j - x, k)) % MOD;
        }
        return res;
    }
}
```

```cpp [sol-C++]
// 会超时的递归代码
class Solution {
public:
    int numRollsToTarget(int n, int k, int target) {
        if (target < n || target > n * k) {
            return 0; // 无法组成 target
        }
        const int MOD = 1'000'000'007;
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (j < 0) {
                return 0; // j 不能是负数
            }
            if (i == 0) {
                return j == 0;
            }
            int res = 0;
            for (int x = 1; x <= k; x++) { // 掷出了 x
                res = (res + dfs(i - 1, j - x)) % MOD;
            }
            return res;
        };
        return dfs(n, target);
    }
};
```

```go [sol-Go]
// 会超时的递归代码
func numRollsToTarget(n, k, target int) int {
    if target < n || target > n*k {
        return 0 // 无法组成 target
    }
    const mod = 1_000_000_007
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if j < 0 {
            return 0 // j 不能是负数
        }
        if i == 0 {
            if j == 0 {
                return 1
            }
            return 0
        }
        res := 0
        for x := 1; x <= k; x++ { // 掷出了 x
            res = (res + dfs(i-1, j-x)) % mod
        }
        return res
    }
    return dfs(n, target)
}
```

```js [sol-JavaScript]
// 会超时的递归代码
var numRollsToTarget = function(n, k, target) {
    if (target < n || target > n * k) {
        return 0; // 无法组成 target
    }
    const MOD = 1_000_000_007;
    function dfs(i, j) {
        if (j < 0) {
            return 0; // j 不能是负数
        }
        if (i === 0) {
            return j === 0 ? 1 : 0;
        }
        let res = 0;
        for (let x = 1; x <= k; x++) { // 掷出了 x
            res = (res + dfs(i - 1, j - x)) % MOD;
        }
        return res;
    }
    return dfs(n, target);
};
```

```rust [sol-Rust]
// 会超时的递归代码
impl Solution {
    pub fn num_rolls_to_target(n: i32, k: i32, target: i32) -> i32 {
        if target < n || target > n * k {
            return 0; // 无法组成 target
        }
        const MOD: i32 = 1_000_000_007;
        let (n, k, target) = (n as usize, k as usize, target as usize);
        fn dfs(i: usize, j: usize, k: usize) -> i32 {
            if j < 0 {
                return 0; // j 不能是负数
            }
            if i == 0 {
                return if j == 0 { 1 } else { 0 };
            }
            let mut res = 0;
            for x in 1..=k { // 掷出了 x
                res = (res + dfs(i - 1, j - x, k)) % MOD;
            }
            res
        }
        dfs(n, target, k)
    }
}
```

为了避免讨论 $j<0$ 的情况，可以把问题改成：每个骰子上的数字是 $0$ 到 $k-1$，数字之和是 $\textit{target}-n$。相当于把每个骰子掷出的数字都提一个 $1$ 出来，那么这些骰子的数字之和就等于 $\textit{target}-n$。

如此一来，如果 $x>j$，那么 $j-x<0$，方案数必然为 $0$，所以 $x$ 至多枚举到 $j$。

修改后的式子为

$$
\textit{dfs}(i,j) = \textit{dfs}(i-1,j) + \textit{dfs}(i-1,j-1) + \cdots + \textit{dfs}(i-1,j-\min(k-1,j))
$$

```py [sol-Python3]
# 会超时的递归代码
class Solution:
    def numRollsToTarget(self, n: int, k: int, target: int) -> int:
        if not (n <= target <= n * k):
            return 0  # 无法组成 target
        MOD = 10 ** 9 + 7
        def dfs(i: int, j: int) -> int:
            if i == 0:
                return int(j == 0)
            res = 0
            for x in range(min(k, j + 1)):  # 掷出了 x
                res += dfs(i - 1, j - x)
            return res % MOD
        return dfs(n, target - n)
```

```java [sol-Java]
// 会超时的递归代码
class Solution {
    private static final int MOD = 1_000_000_007;

    public int numRollsToTarget(int n, int k, int target) {
        if (target < n || target > n * k) {
            return 0; // 无法组成 target
        }
        return dfs(n, target - n, k);
    }

    private int dfs(int i, int j, int k) {
        if (i == 0) {
            return j == 0 ? 1 : 0;
        }
        int res = 0;
        for (int x = 0; x < k && x <= j; x++) { // 掷出了 x
            res = (res + dfs(i - 1, j - x, k)) % MOD;
        }
        return res;
    }
}
```

```cpp [sol-C++]
// 会超时的递归代码
class Solution {
public:
    int numRollsToTarget(int n, int k, int target) {
        if (target < n || target > n * k) {
            return 0; // 无法组成 target
        }
        const int MOD = 1'000'000'007;
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i == 0) {
                return j == 0;
            }
            int res = 0;
            for (int x = 0; x < k && x <= j; x++) { // 掷出了 x
                res = (res + dfs(i - 1, j - x)) % MOD;
            }
            return res;
        };
        return dfs(n, target - n);
    }
};
```

```go [sol-Go]
// 会超时的递归代码
func numRollsToTarget(n, k, target int) int {
    if target < n || target > n*k {
        return 0 // 无法组成 target
    }
    const mod = 1_000_000_007
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i == 0 {
            if j == 0 {
                return 1
            }
            return 0
        }
        res := 0
        for x := 0; x < k && x <= j; x++ {
            res = (res + dfs(i-1, j-x)) % mod
        }
        return res
    }
    return dfs(n, target-n)
}
```

```js [sol-JavaScript]
// 会超时的递归代码
var numRollsToTarget = function(n, k, target) {
    if (target < n || target > n * k) {
        return 0; // 无法组成 target
    }
    const MOD = 1_000_000_007;
    function dfs(i, j) {
        if (i === 0) {
            return j === 0 ? 1 : 0;
        }
        let res = 0;
        for (let x = 0; x < k && x <= j; x++) { // 掷出了 x
            res = (res + dfs(i - 1, j - x)) % MOD;
        }
        return res;
    }
    return dfs(n, target - n);
};
```

```rust [sol-Rust]
// 会超时的递归代码
impl Solution {
    pub fn num_rolls_to_target(n: i32, k: i32, target: i32) -> i32 {
        if target < n || target > n * k {
            return 0; // 无法组成 target
        }
        const MOD: i32 = 1_000_000_007;
        let (n, k, target) = (n as usize, k as usize, target as usize);
        fn dfs(i: usize, j: usize, k: usize) -> i32 {
            if i == 0 {
                return if j == 0 { 1 } else { 0 };
            }
            let mut res = 0;
            for x in 0..k.min(j + 1) { // 掷出了 x
                res = (res + dfs(i - 1, j - x, k)) % MOD;
            }
            res
        }
        dfs(n, target - n, k)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k^n)$。搜索树可以近似为一棵 $k$ 叉树，树高为 $\mathcal{O}(n)$，所以节点个数为 $\mathcal{O}(k^n)$，遍历搜索树需要 $\mathcal{O}(k^n)$ 的时间。
- 空间复杂度：$\mathcal{O}(n)$。递归需要 $\mathcal{O}(n)$ 的栈空间。

## 三、递归 + 记录返回值 = 记忆化搜索

上面的做法太慢了，怎么优化呢？

注意到，「先掷出 $1$ 再掷出 $2$」和「先掷出 $2$ 再掷出 $1$」，都相当于用 $2$ 个骰子掷出 $3$，都会从 $\textit{dfs}(i,j)$ 递归到 $\textit{dfs}(i-2,j-3)$。

一叶知秋，整个递归中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。对于包含取模的题目，是有可能会算出 $0$ 的。一般初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def numRollsToTarget(self, n: int, k: int, target: int) -> int:
        if not (n <= target <= n * k):
            return 0  # 无法组成 target
        MOD = 10 ** 9 + 7
        @cache
        def dfs(i: int, j: int) -> int:
            if i == 0:
                return int(j == 0)
            res = 0
            for x in range(min(k, j + 1)):  # 掷出了 x
                res += dfs(i - 1, j - x)
            return res % MOD
        return dfs(n, target - n)
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int numRollsToTarget(int n, int k, int target) {
        if (target < n || target > n * k) {
            return 0; // 无法组成 target
        }
        int[][] memo = new int[n + 1][target - n + 1];
        for (int[] m : memo) {
            Arrays.fill(m, -1); // -1 表示没有计算过
        }
        return dfs(n, target - n, memo, k);
    }

    private int dfs(int i, int j, int[][] memo, int k) {
        if (i == 0) {
            return j == 0 ? 1 : 0;
        }
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }
        int res = 0;
        for (int x = 0; x < k && x <= j; x++) { // 掷出了 x
            res = (res + dfs(i - 1, j - x, memo, k)) % MOD;
        }
        return memo[i][j] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numRollsToTarget(int n, int k, int target) {
        if (target < n || target > n * k) {
            return 0; // 无法组成 target
        }
        constexpr int MOD = 1'000'000'007;
        vector memo(n + 1, vector<int>(target - n + 1, -1)); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i == 0) {
                return j == 0;
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            res = 0;
            for (int x = 0; x < k && x <= j; x++) { // 掷出了 x
                res = (res + dfs(i - 1, j - x)) % MOD;
            }
            return res;
        };
        return dfs(n, target - n);
    }
};
```

```go [sol-Go]
func numRollsToTarget(n, k, target int) int {
    if target < n || target > n*k {
        return 0 // 无法组成 target
    }
    const mod = 1_000_000_007
    memo := make([][]int, n+1)
    for i := range memo {
        memo[i] = make([]int, target-n+1)
        for j := range memo[i] {
            memo[i][j] = -1
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i == 0 {
            if j == 0 {
                return 1
            }
            return 0
        }
        p := &memo[i][j]
        if *p != -1 { // 之前计算过
            return *p
        }
        res := 0
        for x := 0; x < k && x <= j; x++ { // 掷出了 x
            res = (res + dfs(i-1, j-x)) % mod
        }
        *p = res // 记忆化
        return res
    }
    return dfs(n, target-n)
}
```

```js [sol-JavaScript]
var numRollsToTarget = function(n, k, target) {
    if (target < n || target > n * k) {
        return 0; // 无法组成 target
    }
    const MOD = 1_000_000_007;
    const memo = new Array(n + 1).fill(null).map(() => new Array(target - n + 1).fill(-1)); // -1 表示没有计算过
    function dfs(i, j) {
        if (i === 0) {
            return j === 0 ? 1 : 0;
        }
        if (memo[i][j] !== -1) { // 之前计算过
            return memo[i][j];
        }
        let res = 0;
        for (let x = 0; x < k && x <= j; x++) { // 掷出了 x
            res = (res + dfs(i - 1, j - x)) % MOD;
        }
        return memo[i][j] = res; // 记忆化
    }
    return dfs(n, target - n);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn num_rolls_to_target(n: i32, k: i32, target: i32) -> i32 {
        if target < n || target > n * k {
            return 0; // 无法组成 target
        }
        const MOD: i32 = 1_000_000_007;
        let (n, k, target) = (n as usize, k as usize, target as usize);
        let mut memo = vec![vec![-1; target - n + 1]; n + 1]; // -1 表示没有计算过
        fn dfs(i: usize, j: usize, memo: &mut Vec<Vec<i32>>, k: usize) -> i32 {
            if i == 0 {
                return if j == 0 { 1 } else { 0 };
            }
            if memo[i][j] != -1 { // 之前计算过
                return memo[i][j];
            }
            let mut res = 0;
            for x in 0..k.min(j + 1) { // 掷出了 x
                res = (res + dfs(i - 1, j - x, memo, k)) % MOD;
            }
            memo[i][j] = res; // 记忆化
            res
        }
        dfs(n, target - n, &mut memo, k)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot k\cdot(\textit{target}-n))$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n\cdot(\textit{target}-n))$，单个状态的计算时间为 $\mathcal{O}(k)$，所以动态规划的时间复杂度为 $\mathcal{O}(n\cdot k\cdot(\textit{target}-n))$。
- 空间复杂度：$\mathcal{O}(n\cdot(\textit{target}-n))$。有多少个状态，$\textit{memo}$ 数组的大小就是多少。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

做法：

- $\textit{dfs}$ 改成 $f$ 数组；
- 递归改成循环（每个参数都对应一层循环）；
- 递归边界改成 $f$ 数组的初始值。

> 相当于之前是用递归去计算每个状态，现在是（按照某种顺序）枚举并计算每个状态。

具体来说，$f[i][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示用 $i$ 个骰子掷出数字之和恰好等于 $j$ 的方案数。注意和前文一样，把每个骰子掷出的数字都提一个 $1$ 出来，这些骰子的数字之和等于 $\textit{target}-n$。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 是一样的：

$$
f[i][j] = f[i-1][j] + f[i-1][j-1] + \cdots + f[i-1][j-\min(k-1,j)]
$$

初始值 $f[0][0]=1$，翻译自 $\textit{dfs}(0,0)=1$。

答案为 $f[n][\textit{target}-n]$，翻译自 $\textit{dfs}(n,\textit{target}-n)$。

```py [sol-Python3]
class Solution:
    def numRollsToTarget(self, n: int, k: int, target: int) -> int:
        if not (n <= target <= n * k):
            return 0  # 无法组成 target
        MOD = 10 ** 9 + 7
        f = [[0] * (target - n + 1) for _ in range(n + 1)]
        f[0][0] = 1  # dfs(0, 0) = 1
        for i in range(1, n + 1):
            for j in range(target - n + 1):
                for x in range(min(k, j + 1)):  # 掷出了 x
                    f[i][j] = (f[i][j] + f[i - 1][j - x]) % MOD
        return f[n][-1]
```

```java [sol-Java]
class Solution {
    public int numRollsToTarget(int n, int k, int target) {
        if (target < n || target > n * k) {
            return 0; // 无法组成 target
        }
        final int MOD = 1_000_000_007;
        int[][] f = new int[n + 1][target - n + 1];
        f[0][0] = 1;
        for (int i = 1; i <= n; i++) {
            for (int j = 0; j <= target - n; j++) {
                for (int x = 0; x < k && x <= j; x++) { // 掷出了 x
                    f[i][j] = (f[i][j] + f[i - 1][j - x]) % MOD;
                }
            }
        }
        return f[n][target - n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numRollsToTarget(int n, int k, int target) {
        if (target < n || target > n * k) {
            return 0; // 无法组成 target
        }
        constexpr int MOD = 1'000'000'007;
        vector f(n + 1, vector<int>(target - n + 1));
        f[0][0] = 1;
        for (int i = 1; i <= n; i++) {
            for (int j = 0; j <= target - n; j++) {
                for (int x = 0; x < k && x <= j; x++) { // 掷出了 x
                    f[i][j] = (f[i][j] + f[i - 1][j - x]) % MOD;
                }
            }
        }
        return f.back().back();
    }
};
```

```go [sol-Go]
func numRollsToTarget(n, k, target int) int {
    if target < n || target > n*k {
        return 0 // 无法组成 target
    }
    const mod = 1_000_000_007
    f := make([][]int, n+1)
    for i := range f {
        f[i] = make([]int, target-n+1)
    }
    f[0][0] = 1
    for i := 1; i <= n; i++ {
        for j := 0; j <= target-n; j++ {
            for x := 0; x < k && x <= j; x++ { // 掷出了 x
                f[i][j] = (f[i][j] + f[i-1][j-x]) % mod
            }
        }
    }
    return f[n][target-n]
}
```

```js [sol-JavaScript]
var numRollsToTarget = function(n, k, target) {
    if (target < n || target > n * k) {
        return 0; // 无法组成 target
    }
    const MOD = 1_000_000_007;
    let f = new Array(n + 1).fill(null).map(() => new Array(target - n + 1).fill(0));
    f[0][0] = 1;
    for (let i = 1; i <= n; i++) {
        for (let j = 0; j <= target - n; j++) {
            for (let x = 0; x < k && x <= j; x++) { // 掷出了 x
                f[i][j] = (f[i][j] + f[i - 1][j - x]) % MOD;
            }
        }
    }
    return f[n][target - n];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn num_rolls_to_target(n: i32, k: i32, target: i32) -> i32 {
        if target < n || target > n * k {
            return 0; // 无法组成 target
        }
        const MOD: i32 = 1_000_000_007;
        let (n, k, target) = (n as usize, k as usize, target as usize);
        let mut f = vec![vec![0; target - n + 1]; n + 1];
        f[0][0] = 1;
        for i in 1..=n {
            for j in 0..=target - n {
                for x in 0..k.min(j + 1) { // 掷出了 x
                    f[i][j] = (f[i][j] + f[i - 1][j - x]) % MOD;
                }
            }
        }
        f[n][target - n]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot k\cdot(\textit{target}-n))$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n\cdot(\textit{target}-n))$，单个状态的计算时间为 $\mathcal{O}(k)$，所以动态规划的时间复杂度为 $\mathcal{O}(n\cdot k\cdot(\textit{target}-n))$。
- 空间复杂度：$\mathcal{O}(n\cdot(\textit{target}-n))$。有多少个状态，$f$ 数组的大小就是多少。

## 五、终极优化

**前置知识**：[前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solutions/2693498/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

#### 1) 时间优化

再看一眼这个转移方程

$$
f[i][j] = f[i-1][j] + f[i-1][j-1] + \cdots + f[i-1][j-\min(k-1,j)]
$$

定义 $f[i-1]$ 的前缀和

$$
s[j] = f[i-1][0] + f[i-1][1] + \cdots + f[i-1][j]
$$

这可以递推算出来，即

$$
s[j] = s[j-1] + f[i-1][j]
$$

那么

$$
f[i][j] =
\begin{cases} 
s[j],&j<k\\
s[j] - s[j-k],&j\ge k
\end{cases}
$$

这样就可以 $\mathcal{O}(1)$ 地算出 $\textit{f}[i][j]$ 了。

此外，由于 $i$ 个骰子掷出的数字之和至多为 $i(k-1)$，所以 $j$ 至多枚举到 $i(k-1)$ 就行。

#### 2) 空间优化

类似 0-1 背包（请看本文开头贴的视频链接），$f$ 可以压缩成一维数组。

此外，无需创建一个数组表示前缀和，可以直接在 $f$ 数组上原地计算，写成代码就是 `f[j] += f[j-1]`。

如此一来，上面的式子就变成了

$$
f[j] =
\begin{cases}
f[j],&j<k\\
f[j] - f[j-k],&j\ge k
\end{cases}
$$

其中 $f[j] = f[j]$ 无需计算，所以 $j$ 最小枚举到 $k$ 就行。

初始值 $f[0]=1$。

答案为 $f[\textit{target}-n]$。

加入这些优化后，我们就能击败 100% 了！（如果没有击败 100% 可以多提交几次）

```py [sol-Python3]
class Solution:
    def numRollsToTarget(self, n: int, k: int, target: int) -> int:
        if not (n <= target <= n * k):
            return 0  # 无法组成 target
        MOD = 10 ** 9 + 7
        f = [1] + [0] * (target - n)
        for i in range(1, n + 1):
            max_j = min(i * (k - 1), target - n)  # i 个骰子至多掷出 i*(k-1)
            for j in range(1, max_j + 1):
                f[j] += f[j - 1]  # 原地计算 f 的前缀和
            for j in range(max_j, k - 1, -1):
                f[j] = (f[j] - f[j - k]) % MOD  # f[j] 是两个前缀和的差
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int numRollsToTarget(int n, int k, int target) {
        if (target < n || target > n * k) {
            return 0; // 无法组成 target
        }
        final int MOD = 1_000_000_007;
        long[] f = new long[target - n + 1];
        f[0] = 1;
        for (int i = 1; i <= n; i++) {
            int maxJ = Math.min(i * (k - 1), target - n); // i 个骰子至多掷出 i*(k-1)
            for (int j = 1; j <= maxJ; j++) {
                f[j] += f[j - 1]; // 原地计算 f 的前缀和
            }
            for (int j = maxJ; j >= k; j--) {
                f[j] = (f[j] - f[j - k]) % MOD; // f[j] 是两个前缀和的差
            }
        }
        return (int) f[target - n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numRollsToTarget(int n, int k, int target) {
        if (target < n || target > n * k) {
            return 0; // 无法组成 target
        }
        constexpr int MOD = 1'000'000'007;
        vector<long long> f(target - n + 1);
        f[0] = 1;
        for (int i = 1; i <= n; i++) {
            int max_j = min(i * (k - 1), target - n); // i 个骰子至多掷出 i*(k-1)
            for (int j = 1; j <= max_j; j++) {
                f[j] += f[j - 1]; // 原地计算 f 的前缀和
            }
            for (int j = max_j; j >= k; j--) {
                f[j] = (f[j] - f[j - k]) % MOD; // f[j] 是两个前缀和的差
            }
        }
        return f.back();
    }
};
```

```go [sol-Go]
func numRollsToTarget(n, k, target int) int {
    if target < n || target > n*k {
        return 0 // 无法组成 target
    }
    const mod = 1_000_000_007
    f := make([]int, target-n+1)
    f[0] = 1
    for i := 1; i <= n; i++ {
        maxJ := min(i*(k-1), target-n) // i 个骰子至多掷出 i*(k-1)
        for j := 1; j <= maxJ; j++ {
            f[j] += f[j-1] // 原地计算 f 的前缀和
        }
        for j := maxJ; j >= k; j-- {
            f[j] = (f[j] - f[j-k]) % mod // f[j] 是两个前缀和的差
        }
    }
    return f[target-n]
}
```

```js [sol-JavaScript]
var numRollsToTarget = function(n, k, target) {
    if (target < n || target > n * k) {
        return 0; // 无法组成 target
    }
    const MOD = 1_000_000_007;
    const f = new Array(target - n + 1).fill(0);
    f[0] = 1;
    for (let i = 1; i <= n; i++) {
        const maxJ = Math.min(i * (k - 1), target - n); // i 个骰子至多掷出 i*(k-1)
        for (let j = 1; j <= maxJ; j++) {
            f[j] = (f[j] + f[j - 1]) % MOD; // 原地计算 f 的前缀和
        }
        for (let j = maxJ; j >= k; j--) {
            f[j] -= f[j - k]; // 注意这里可能减成负数
        }
    }
    return (f[target - n] + MOD * 2) % MOD; // 调整成非负数
};
```

```rust [sol-Rust]
impl Solution {
    pub fn num_rolls_to_target(n: i32, k: i32, target: i32) -> i32 {
        if target < n || target > n * k {
            return 0; // 无法组成 target
        }
        const MOD: i64 = 1_000_000_007;
        let (n, k, target) = (n as usize, k as usize, target as usize);
        let mut f = vec![0i64; target - n + 1];
        f[0] = 1;
        for i in 1..=n {
            let max_j = (i * (k - 1)).min(target - n); // i 个骰子至多掷出 i*(k-1)
            for j in 1..=max_j {
                f[j] += f[j - 1]; // 原地计算 f 的前缀和
            }
            for j in (k..=max_j).rev() {
                f[j] = (f[j] - f[(j - k)]) % MOD; // f[j] 是两个前缀和的差
            }
        }
        f[target - n] as i32
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot(\textit{target}-n))$。
- 空间复杂度：$\mathcal{O}(\textit{target}-n)$。

## 相似题目

[2902. 和带限制的子多重集合的数目]( https://leetcode.cn/problems/count-of-sub-multisets-with-bounded-sum/)

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

本题来自 `三、背包 / §3.4 分组背包`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、背包 / §3.4 分组背包`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
