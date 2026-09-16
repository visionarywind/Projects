# 2008. 出租车的最大盈利

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-earnings-from-taxi/
- 题目 slug：`maximum-earnings-from-taxi`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.2 不相交区间
- 难度分：1872
- 外部题解来源：https://leetcode.cn/problems/maximum-earnings-from-taxi/solutions/2558504/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-k15a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考动态规划：从记忆化搜索到递推（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-earnings-from-taxi/solutions/2558504/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-k15a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-k15a`
- topic id：`2558504`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、启发思考：寻找子问题

假设 $n=9$。

我们要解决的问题是从 $1$ 开到 $9$ 最多可以赚多少钱。

如果没有乘客在 $9$ 下车，或者我们不载在 $9$ 下车的乘客，那么问题变成：

- 从 $1$ 开到 $8$ 最多可以赚多少钱。

如果有乘客在 $9$ 下车，我们可以枚举载哪位乘客。假设所载乘客在 $5$ 上车，那么从 $5$ 到 $9$ 不能载其它乘客（题目要求同时最多只能接一个订单），问题变成：

- 从 $1$ 开到 $5$ 最多可以赚多少钱。

注意可以在一个地点放下一位乘客，并在同一个地点接上另一位乘客。

不载乘客和枚举载哪个乘客，都会把原问题变成一个**和原问题相似的、规模更小的子问题**，这意味着我们可以用**递归**解决。

> 注：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。

## 二、递归怎么写：状态定义与状态转移方程

因为要解决的问题都是「从 $1$ 开到 $i$」，所以定义 $\textit{dfs}(i)$ 表示从 $1$ 开到 $i$ 最多可以赚多少钱。

如果没有乘客在 $i$ 下车，或者我们不载在 $i$ 下车的乘客，问题变成：从 $1$ 开到 $i-1$ 最多可以赚多少钱，即

$$
\textit{dfs}(i) = \textit{dfs}(i-1)
$$

如果有乘客在 $i$ 下车，我们可以枚举载哪位乘客，取其中最赚钱的方案，即

$$
\textit{dfs}(i) = \max_{j}\{\textit{dfs}(\textit{start}_j) + i - \textit{start}_j + \textit{tip}_j\}
$$

其中 $j$ 满足 $\textit{end}_j = i$。

这两种情况取最大值，就得到了 $\textit{dfs}(i)$，即

$$
\textit{dfs}(i) = \max(\textit{dfs}(i-1), \max_{j}\{\textit{dfs}(\textit{start}_j) + i - \textit{start}_j + \textit{tip}_j\})
$$

递归边界：$\textit{dfs}(1)=0$。注意没有在 $1$ 下车的乘客。

递归入口：$\textit{dfs}(n)$，也就是答案。

代码实现时，为了方便枚举所有在 $i$ 下车的乘客，可以把 $\textit{rides}$ 按照 $\textit{end}$ 分组。分组时，对相同的 $\textit{end}$，记录 $\textit{start}$ 以及 $\textit{end} - \textit{start} + \textit{tip}$。

```py [sol-Python3]
# 会超时的递归代码
class Solution:
    def maxTaxiEarnings(self, n: int, rides: List[List[int]]) -> int:
        groups = defaultdict(list)
        for start, end, tip in rides:
            groups[end].append((start, end - start + tip))

        def dfs(i: int) -> int:
            if i == 1:
                return 0
            return max(dfs(i - 1), max((dfs(s) + t for s, t in groups[i]), default=0))
        return dfs(n)
```

```java [sol-Java]
// 会超时的递归代码
class Solution {
    public long maxTaxiEarnings(int n, int[][] rides) {
        List<int[]>[] groups = new ArrayList[n + 1];
        for (int[] r : rides) {
            int start = r[0], end = r[1], tip = r[2];
            if (groups[end] == null) {
                groups[end] = new ArrayList<>();
            }
            groups[end].add(new int[]{start, end - start + tip});
        }
        return dfs(n, groups);
    }

    private long dfs(int i, List<int[]>[] groups) {
        if (i == 1) {
            return 0;
        }
        long res = dfs(i - 1, groups);
        if (groups[i] != null) {
            for (int[] p : groups[i]) {
                res = Math.max(res, dfs(p[0], groups) + p[1]);
            }
        }
        return res;
    }
}
```

```cpp [sol-C++]
// 会超时的递归代码
class Solution {
public:
    long long maxTaxiEarnings(int n, vector<vector<int>>& rides) {
        vector<vector<pair<int, int>>> groups(n + 1);
        for (auto& r : rides) {
            int start = r[0], end = r[1], tip = r[2];
            groups[end].emplace_back(start, end - start + tip);
        }

        auto dfs = [&](this auto&& dfs, int i) -> long long {
            if (i == 1) {
                return 0;
            }
            long long res = dfs(i - 1);
            for (auto& [s, t] : groups[i]) {
                res = max(res, dfs(s) + t);
            }
            return res;
        };
        return dfs(n);
    }
};
```

```go [sol-Go]
// 会超时的递归代码
func maxTaxiEarnings(n int, rides [][]int) int64 {
    type pair struct{ s, t int }
    groups := make([][]pair, n+1)
    for _, r := range rides {
        start, end, tip := r[0], r[1], r[2]
        groups[end] = append(groups[end], pair{start, end - start + tip})
    }

    var dfs func(int) int64
    dfs = func(i int) int64 {
        if i == 1 {
            return 0
        }
        res := dfs(i - 1)
        for _, p := range groups[i] {
            res = max(res, dfs(p.s)+int64(p.t))
        }
        return res
    }
    return dfs(n)
}
```

```js [sol-JavaScript]
// 会超时的递归代码
var maxTaxiEarnings = function (n, rides) {
    const groups = Array(n + 1).fill(null).map(() => []);
    for (const [start, end, tip] of rides) {
        groups[end].push([start, end - start + tip]);
    }

    function dfs(i) {
        if (i === 1) {
            return 0;
        }
        let res = dfs(i - 1);
        for (const [s, t] of groups[i]) {
            res = Math.max(res, dfs(s) + t);
        }
        return res;
    }
    return dfs(n);
};
```

```rust [sol-Rust]
// 会超时的递归代码
impl Solution {
    pub fn max_taxi_earnings(n: i32, rides: Vec<Vec<i32>>) -> i64 {
        let n = n as usize;
        let mut groups: Vec<Vec<(i32, i32)>> = vec![vec![]; (n + 1)];
        for r in &rides {
            let start = r[0];
            let end = r[1];
            let tip = r[2];
            groups[end as usize].push((start, end - start + tip));
        }

        fn dfs(i: usize, groups: &Vec<Vec<(i32, i32)>>) -> i64 {
            if i == 1 {
                return 0;
            }
            let mut res = dfs(i - 1, groups);
            for &(s, t) in &groups[i] {
                res = res.max(dfs(s as usize, groups) + t as i64);
            }
            res
        }
        dfs(n, &groups)
    }
}
```

## 三、递归 + 记录返回值 = 记忆化搜索

由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

```py [sol-Python3]
# 更快的写法请看下面的递推
class Solution:
    def maxTaxiEarnings(self, n: int, rides: List[List[int]]) -> int:
        groups = defaultdict(list)
        for start, end, tip in rides:
            groups[end].append((start, end - start + tip))

        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int) -> int:
            if i == 1:
                return 0
            return max(dfs(i - 1), max((dfs(s) + t for s, t in groups[i]), default=0))
        return dfs(n)
```

```java [sol-Java]
class Solution {
    public long maxTaxiEarnings(int n, int[][] rides) {
        List<int[]>[] groups = new ArrayList[n + 1];
        for (int[] r : rides) {
            int start = r[0], end = r[1], tip = r[2];
            if (groups[end] == null) {
                groups[end] = new ArrayList<>();
            }
            groups[end].add(new int[]{start, end - start + tip});
        }

        long[] memo = new long[n + 1];
        Arrays.fill(memo, -1); // -1 表示没有计算过
        return dfs(n, memo, groups);
    }

    private long dfs(int i, long[] memo, List<int[]>[] groups) {
        if (i == 1) {
            return 0;
        }
        if (memo[i] != -1) { // 之前计算过
            return memo[i];
        }
        long res = dfs(i - 1, memo, groups);
        if (groups[i] != null) {
            for (int[] p : groups[i]) {
                res = Math.max(res, dfs(p[0], memo, groups) + p[1]);
            }
        }
        return memo[i] = res; // 记忆化
    }
} 
```

```cpp [sol-C++]
class Solution {
public:
    long long maxTaxiEarnings(int n, vector<vector<int>>& rides) {
        vector<vector<pair<int, int>>> groups(n + 1);
        for (auto& r : rides) {
            int start = r[0], end = r[1], tip = r[2];
            groups[end].emplace_back(start, end - start + tip);
        }

        vector<long long> memo(n + 1, -1); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int i) -> long long {
            if (i == 1) {
                return 0;
            }
            auto& res = memo[i]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            res = dfs(i - 1);
            for (auto& [s, t] : groups[i]) {
                res = max(res, dfs(s) + t);
            }
            return res;
        };
        return dfs(n);
    }
};
```

```go [sol-Go]
func maxTaxiEarnings(n int, rides [][]int) int64 {
    type pair struct{ s, t int }
    groups := make([][]pair, n+1)
    for _, r := range rides {
        start, end, tip := r[0], r[1], r[2]
        groups[end] = append(groups[end], pair{start, end - start + tip})
    }

    memo := make([]int64, n+1)
    for i := range memo {
        memo[i] = -1 // -1 表示没有计算过
    }
    var dfs func(int) int64
    dfs = func(i int) int64 {
        if i == 1 {
            return 0
        }
        p := &memo[i]
        if *p != -1 { // 之前计算过
            return *p
        }
        res := dfs(i - 1)
        for _, p := range groups[i] {
            res = max(res, dfs(p.s)+int64(p.t))
        }
        *p = res // 记忆化
        return res
    }
    return dfs(n)
}
```

```js [sol-JavaScript]
var maxTaxiEarnings = function (n, rides) {
    const groups = Array(n + 1).fill(null).map(() => []);
    for (const [start, end, tip] of rides) {
        groups[end].push([start, end - start + tip]);
    }

    const memo = Array(n + 1).fill(-1); // -1 表示没有计算过
    function dfs(i) {
        if (i === 1) {
            return 0;
        }
        if (memo[i] !== -1) { // 之前计算过
            return memo[i];
        }
        let res = dfs(i - 1);
        for (const [s, t] of groups[i]) {
            res = Math.max(res, dfs(s) + t);
        }
        return memo[i] = res; // 记忆化
    }
    return dfs(n);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_taxi_earnings(n: i32, rides: Vec<Vec<i32>>) -> i64 {
        let n = n as usize;
        let mut groups: Vec<Vec<(i32, i32)>> = vec![vec![]; (n + 1)];
        for r in &rides {
            let start = r[0];
            let end = r[1];
            let tip = r[2];
            groups[end as usize].push((start, end - start + tip));
        }

        let mut memo = vec![-1i64; n + 1]; // -1 表示没有计算过
        fn dfs(i: usize, memo: &mut Vec<i64>, groups: &Vec<Vec<(i32, i32)>>) -> i64 {
            if i == 1 {
                return 0;
            }
            if memo[i] != -1 { // 之前计算过
                return memo[i];
            }
            let mut res = dfs(i - 1, memo, groups);
            for &(s, t) in &groups[i] {
                res = res.max(dfs(s as usize, memo, groups) + t as i64);
            }
            memo[i] = res; // 记忆化
            res
        }
        dfs(n, &mut memo, &groups)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $m$ 为 $\textit{rides}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。状态个数是 $\mathcal{O}(n)$，乍一看时间复杂度是 $\mathcal{O}(nm)$，但考虑到不同的 $i$ 对应的 $\textit{groups}[i]$ 是不同的，遍历所有 $\textit{groups}[i]$ 的循环次数加起来是 $\mathcal{O}(m)$，所以时间复杂度是 $\mathcal{O}(n+m)$。
- 空间复杂度：$\mathcal{O}(n+m)$。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i]$ 的定义和 $\textit{dfs}(i)$ 的定义是一样的，都表示从 $1$ 开到 $i$ 最多可以赚多少钱。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i] = \max(f[i-1], \max_{j}\{f[\textit{start}_j] + i - \textit{start}_j + \textit{tip}_j\})
$$

其中 $j$ 满足 $\textit{end}_j = i$。

> 相当于之前是用递归去计算每个状态，现在是**枚举**并计算每个状态。

初始值 $f[1]=0$，翻译自递归边界 $\textit{dfs}(1)=0$。

答案为 $f[n]$，翻译自递归入口 $\textit{dfs}(n)$。

```py [sol-Python3]
class Solution:
    def maxTaxiEarnings(self, n: int, rides: List[List[int]]) -> int:
        groups = defaultdict(list)
        for start, end, tip in rides:
            groups[end].append((start, end - start + tip))

        f = [0] * (n + 1)
        for i in range(2, n + 1):
            f[i] = f[i - 1]
            if i in groups:
                f[i] = max(f[i], max(f[s] + t for s, t in groups[i]))
        return f[n]
```

```java [sol-Java]
class Solution {
    public long maxTaxiEarnings(int n, int[][] rides) {
        List<int[]>[] groups = new ArrayList[n + 1];
        for (int[] r : rides) {
            int start = r[0], end = r[1], tip = r[2];
            if (groups[end] == null) {
                groups[end] = new ArrayList<>();
            }
            groups[end].add(new int[]{start, end - start + tip});
        }

        long[] f = new long[n + 1];
        for (int i = 2; i <= n; i++) {
            f[i] = f[i - 1];
            if (groups[i] != null) {
                for (int[] p : groups[i]) {
                    f[i] = Math.max(f[i], f[p[0]] + p[1]);
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
    long long maxTaxiEarnings(int n, vector<vector<int>>& rides) {
        vector<vector<pair<int, int>>> groups(n + 1);
        for (auto& r : rides) {
            int start = r[0], end = r[1], tip = r[2];
            groups[end].emplace_back(start, end - start + tip);
        }

        vector<long long> f(n + 1);
        for (int i = 2; i <= n; i++) {
            f[i] = f[i - 1];
            for (auto& [s, t] : groups[i]) {
                f[i] = max(f[i], f[s] + t);
            }
        }
        return f[n];
    }
};
```

```go [sol-Go]
func maxTaxiEarnings(n int, rides [][]int) int64 {
    type pair struct{ s, t int }
    groups := make([][]pair, n+1)
    for _, r := range rides {
        start, end, tip := r[0], r[1], r[2]
        groups[end] = append(groups[end], pair{start, end - start + tip})
    }

    f := make([]int64, n+1)
    for i := 2; i <= n; i++ {
        f[i] = f[i-1]
        for _, p := range groups[i] {
            f[i] = max(f[i], f[p.s]+int64(p.t))
        }
    }
    return f[n]
}
```

```js [sol-JavaScript]
var maxTaxiEarnings = function (n, rides) {
    const groups = Array(n + 1).fill(null).map(() => []);
    for (const [start, end, tip] of rides) {
        groups[end].push([start, end - start + tip]);
    }

    const f = Array(n + 1).fill(0);
    for (let i = 2; i <= n; i++) {
        f[i] = f[i - 1];
        for (const [s, t] of groups[i]) {
            f[i] = Math.max(f[i], f[s] + t);
        }
    }
    return f[n];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_taxi_earnings(n: i32, rides: Vec<Vec<i32>>) -> i64 {
        let n = n as usize;
        let mut groups: Vec<Vec<(i32, i32)>> = vec![vec![]; (n + 1)];
        for r in &rides {
            let start = r[0];
            let end = r[1];
            let tip = r[2];
            groups[end as usize].push((start, end - start + tip));
        }

        let mut f: Vec<i64> = vec![0; (n + 1)];
        for i in 2..=n {
            f[i] = f[i - 1];
            for &(s, t) in &groups[i] {
                f[i] = f[i].max(f[s as usize] + t as i64);
            }
        }
        f[n]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $m$ 为 $\textit{rides}$ 的长度。内外层循环次数之和是 $\mathcal{O}(n+m)$，所以时间复杂度是 $\mathcal{O}(n+m)$。
- 空间复杂度：$\mathcal{O}(n+m)$。

## 思考题

如果允许同时载两个乘客，要怎么做？

欢迎在评论区发表你的思路。

## 相似题目（右边数字为难度分）

- [2830. 销售利润最大化](https://leetcode.cn/problems/maximize-the-profit-as-the-salesman/) 1851
- [1235. 规划兼职工作](https://leetcode.cn/problems/maximum-profit-in-job-scheduling/) 2023
- [1751. 最多可以参加的会议数目 II](https://leetcode.cn/problems/maximum-number-of-events-that-can-be-attended-ii/) 2041

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

本题来自 `七、其他线性 DP / §7.2 不相交区间`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.2 不相交区间`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
