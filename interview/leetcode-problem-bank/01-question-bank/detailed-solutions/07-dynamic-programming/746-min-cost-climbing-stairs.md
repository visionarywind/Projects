# 746. 使用最小花费爬楼梯

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/min-cost-climbing-stairs/
- 题目 slug：`min-cost-climbing-stairs`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.1 爬楼梯
- 难度分：1500
- 外部题解来源：https://leetcode.cn/problems/min-cost-climbing-stairs/solutions/2569116/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-j99e/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考动态规划：从记忆化搜索到递推（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/min-cost-climbing-stairs/solutions/2569116/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-j99e/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-j99e`
- topic id：`2569116`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意解读

设数组的长度为 $n$，一共有 $n+1$ 个台阶，编号从 $0$ 到 $n$。你需要从编号为 $0$ 或 $1$ 的台阶开始，向上爬到编号为 $n$ 的台阶，并且每次只能爬一个或者两个台阶。从编号为 $i$ 的台阶向上爬，需要支付 $\textit{cost}[i]$ 的花费。求爬到 $n$ 的花费之和的最小值。

例如 $\textit{cost}=[10,15,20]$，表示有 $0,1,2,3$ 四个台阶，起点为 $0$ 或 $1$，终点为 $3$。从编号为 $1$ 的台阶往上爬两个台阶就可以到达终点 $3$ 了，对应的花费也最小，所以答案是 $15$。

## 前置知识

请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含如何把记忆化搜索 1:1 翻译成递推的技巧。

## 一、启发思考：寻找子问题

假设数组长度 $n=9$。

我们要解决的问题是从 $0$ 或 $1$ 爬到 $9$ 的最小花费。注意楼梯顶部是 $n$，不是 $n-1$。

枚举最后一步爬了几个台阶，分类讨论：

- 如果最后一步爬了 $1$ 个台阶，那么我们得先爬到 $8$，要解决的问题缩小成：从 $0$ 或 $1$ 爬到 $8$ 的最小花费。
- 如果最后一步爬了 $2$ 个台阶，那么我们得先爬到 $7$，要解决的问题缩小成：从 $0$ 或 $1$ 爬到 $7$ 的最小花费。

由于这两种情况都会把原问题变成一个**和原问题相似的、规模更小的子问题**，所以可以用**递归**解决。

> 注 1：从大往小思考，主要是为了方便把递归翻译成递推。从小往大思考也是可以的。
>
> 注 2：动态规划有「选或不选」和「枚举选哪个」两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「枚举选哪个」。

## 二、递归怎么写：状态定义与状态转移方程

因为要解决的问题都是「从 $0$ 或 $1$ 爬到 $i$」，所以定义 $\textit{dfs}(i)$ 表示从 $0$ 或 $1$ 爬到 $i$ 的最小花费。

枚举最后一步爬了几个台阶，分类讨论：

- 如果最后一步爬了 $1$ 个台阶，那么我们得先爬到 $i-1$，要解决的问题缩小成：从 $0$ 或 $1$ 爬到 $i-1$ 的最小花费。再加上从 $i-1$ 爬到 $i$ 的花费 $\textit{cost}[i-1]$，就得到了 $\textit{dfs}(i)$，即 $\textit{dfs}(i) = \textit{dfs}(i-1) + \textit{cost}[i-1]$。
- 如果最后一步爬了 $2$ 个台阶，那么我们得先爬到 $i-2$，要解决的问题缩小成：从 $0$ 或 $1$ 爬到 $i-2$ 的最小花费。再加上从 $i-2$ 爬到 $i$ 的花费 $\textit{cost}[i-2]$，就得到了 $\textit{dfs}(i)$，即 $\textit{dfs}(i) = \textit{dfs}(i-2) + \textit{cost}[i-2]$。

两种情况取最小值，即为从 $0$ 或 $1$ 爬到 $i$ 的最小花费：

$$
\textit{dfs}(i) = \min(\textit{dfs}(i-1) + \textit{cost}[i-1], \textit{dfs}(i-2) + \textit{cost}[i-2])
$$

递归边界：$\textit{dfs}(0)=0,\ \textit{dfs}(1)=0$。爬到 $0$ 或 $1$ 无需花费，因为我们一开始在 $0$ 或 $1$。

递归入口：$\textit{dfs}(n)$，也就是答案。

```py [sol-Python3]
# 会超时的递归代码
class Solution:
    def minCostClimbingStairs(self, cost: List[int]) -> int:
        def dfs(i: int) -> int:
            if i <= 1:  # 递归边界
                return 0
            return min(dfs(i - 1) + cost[i - 1], dfs(i - 2) + cost[i - 2])
        return dfs(len(cost))
```

```java [sol-Java]
// 会超时的递归代码
class Solution {
    public int minCostClimbingStairs(int[] cost) {
        int n = cost.length;
        return dfs(n, cost);
    }

    private int dfs(int i, int[] cost) {
        if (i <= 1) { // 递归边界
            return 0;
        }
        int res1 = dfs(i - 1, cost) + cost[i - 1];
        int res2 = dfs(i - 2, cost) + cost[i - 2];
        return Math.min(res1, res2);
    }
}
```

```cpp [sol-C++]
// 会超时的递归代码
class Solution {
public:
    int minCostClimbingStairs(vector<int>& cost) {
        int n = cost.size();
        // lambda 递归
        auto dfs = [&](this auto&& dfs, int i) -> int {
            if (i <= 1) { // 递归边界
                return 0;
            }
            return min(dfs(i - 1) + cost[i - 1], dfs(i - 2) + cost[i - 2]);
        };
        return dfs(n);
    }
};
```

```go [sol-Go]
// 会超时的递归代码
func minCostClimbingStairs(cost []int) int {
    n := len(cost)
    var dfs func(int) int
    dfs = func(i int) int {
        if i <= 1 { // 递归边界
            return 0
        }
        return min(dfs(i-1)+cost[i-1], dfs(i-2)+cost[i-2])
    }
    return dfs(n)
}
```

```js [sol-JavaScript]
// 会超时的递归代码
var minCostClimbingStairs = function(cost) {
    const n = cost.length;
    function dfs(i) {
        if (i <= 1) { // 递归边界
            return 0;
        }
        return Math.min(dfs(i - 1) + cost[i - 1], dfs(i - 2) + cost[i - 2]);
    }
    return dfs(n);
};
```

```rust [sol-Rust]
// 会超时的递归代码
impl Solution {
    pub fn min_cost_climbing_stairs(cost: Vec<i32>) -> i32 {
        fn dfs(i: usize, cost: &Vec<i32>) -> i32 {
            if i <= 1 { // 递归边界
                return 0;
            }
            let res1 = dfs(i - 1, cost) + cost[i - 1];
            let res2 = dfs(i - 2, cost) + cost[i - 2];
            res1.min(res2)
        }
        let n = cost.len();
        dfs(n, &cost)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(2^n)$，其中 $n$ 为 $\textit{cost}$ 的长度。搜索树可以近似为一棵二叉树，树高为 $\mathcal{O}(n)$，所以节点个数为 $\mathcal{O}(2^n)$，遍历搜索树需要 $\mathcal{O}(2^n)$ 的时间。
- 空间复杂度：$\mathcal{O}(n)$。递归需要 $\mathcal{O}(n)$ 的栈空间。

## 三、递归 + 记录返回值 = 记忆化搜索

上面的做法太慢了，怎么优化呢？

注意到「先爬 $1$ 个台阶，再爬 $2$ 个台阶」和「先爬 $2$ 个台阶，再爬 $1$ 个台阶」，都相当于爬 $3$ 个台阶，都会从 $\textit{dfs}(i)$ 递归到 $\textit{dfs}(i-3)$。

一叶知秋，整个递归中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

```py [sol-Python3]
class Solution:
    def minCostClimbingStairs(self, cost: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int) -> int:
            if i <= 1:  # 递归边界
                return 0
            return min(dfs(i - 1) + cost[i - 1], dfs(i - 2) + cost[i - 2])
        return dfs(len(cost))
```

```java [sol-Java]
class Solution {
    public int minCostClimbingStairs(int[] cost) {
        int n = cost.length;
        int[] memo = new int[n + 1];
        Arrays.fill(memo, -1); // -1 表示没有计算过
        return dfs(n, memo, cost);
    }

    private int dfs(int i, int[] memo, int[] cost) {
        if (i <= 1) { // 递归边界
            return 0;
        }
        if (memo[i] != -1) { // 之前计算过
            return memo[i];
        }
        int res1 = dfs(i - 1, memo, cost) + cost[i - 1];
        int res2 = dfs(i - 2, memo, cost) + cost[i - 2];
        return memo[i] = Math.min(res1, res2); // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minCostClimbingStairs(vector<int>& cost) {
        int n = cost.size();
        vector<int> memo(n + 1, -1); // -1 表示没有计算过
        // lambda 递归
        auto dfs = [&](this auto&& dfs, int i) -> int {
            if (i <= 1) { // 递归边界
                return 0;
            }
            int& res = memo[i]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            return res = min(dfs(i - 1) + cost[i - 1], dfs(i - 2) + cost[i - 2]); // 记忆化
        };
        return dfs(n);
    }
};
```

```go [sol-Go]
func minCostClimbingStairs(cost []int) int {
    n := len(cost)
    memo := make([]int, n+1)
    for i := range memo {
        memo[i] = -1 // -1 表示没有计算过
    }
    var dfs func(int) int
    dfs = func(i int) int {
        if i <= 1 { // 递归边界
            return 0
        }
        p := &memo[i]
        if *p != -1 { // 之前计算过
            return *p
        }
        res := min(dfs(i-1)+cost[i-1], dfs(i-2)+cost[i-2])
        *p = res // 记忆化
        return res
    }
    return dfs(n)
}
```

```js [sol-JavaScript]
var minCostClimbingStairs = function(cost) {
    const n = cost.length;
    const memo = Array(n + 1).fill(-1); // -1 表示没有计算过
    function dfs(i) {
        if (i <= 1) { // 递归边界
            return 0;
        }
        if (memo[i] !== -1) { // 之前计算过
            return memo[i];
        }
        return memo[i] = Math.min(dfs(i - 1) + cost[i - 1], dfs(i - 2) + cost[i - 2]); // 记忆化
    }
    return dfs(n);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_cost_climbing_stairs(cost: Vec<i32>) -> i32 {
        fn dfs(i: usize, memo: &mut Vec<i32>, cost: &Vec<i32>) -> i32 {
            if i <= 1 { // 递归边界
                return 0;
            }
            if memo[i] != -1 { // 之前计算过
                return memo[i];
            }
            let res1 = dfs(i - 1, memo, cost) + cost[i - 1];
            let res2 = dfs(i - 2, memo, cost) + cost[i - 2];
            let res = res1.min(res2);
            memo[i] = res; // 记忆化
            res
        }
        let n = cost.len();
        let mut memo = vec![-1; n + 1]; // -1 表示没有计算过
        dfs(n, &mut memo, &cost)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{cost}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。有多少个状态，$\textit{memo}$ 数组的大小就是多少。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i]$ 的定义和 $\textit{dfs}(i)$ 的定义是一样的，都表示从 $0$ 或 $1$ 爬到 $i$ 的最小花费。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i] = \min(f[i-1] + \textit{cost}[i-1], f[i-2] + \textit{cost}[i-2])
$$

> 相当于之前是用递归去计算每个状态，现在是**枚举**并计算每个状态。

初始值 $f[0]=0,\ f[1]=0$，翻译自递归边界 $\textit{dfs}(0)=0,\ \textit{dfs}(1)=0$。

答案为 $f[n]$，翻译自递归入口 $\textit{dfs}(n)$。

```py [sol-Python3]
class Solution:
    def minCostClimbingStairs(self, cost: List[int]) -> int:
        n = len(cost)
        f = [0] * (n + 1)
        for i in range(2, n + 1):
            f[i] = min(f[i - 1] + cost[i - 1], f[i - 2] + cost[i - 2])
        return f[n]
```

```java [sol-Java]
class Solution {
    public int minCostClimbingStairs(int[] cost) {
        int n = cost.length;
        int[] f = new int[n + 1];
        for (int i = 2; i <= n; i++) {
            f[i] = Math.min(f[i - 1] + cost[i - 1], f[i - 2] + cost[i - 2]);
        }
        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minCostClimbingStairs(vector<int>& cost) {
        int n = cost.size();
        vector<int> f(n + 1);
        for (int i = 2; i <= n; i++) {
            f[i] = min(f[i - 1] + cost[i - 1], f[i - 2] + cost[i - 2]);
        }
        return f[n];
    }
};
```

```go [sol-Go]
func minCostClimbingStairs(cost []int) int {
    n := len(cost)
    f := make([]int, n+1)
    for i := 2; i <= n; i++ {
        f[i] = min(f[i-1]+cost[i-1], f[i-2]+cost[i-2])
    }
    return f[n]
}
```

```js [sol-JavaScript]
var minCostClimbingStairs = function(cost) {
    const n = cost.length;
    const f = Array(n + 1).fill(0);
    for (let i = 2; i <= n; i++) {
        f[i] = Math.min(f[i - 1] + cost[i - 1], f[i - 2] + cost[i - 2]);
    }
    return f[n];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_cost_climbing_stairs(cost: Vec<i32>) -> i32 {
        let n = cost.len();
        let mut f = vec![0; n + 1];
        for i in 2..=n {
            f[i] = (f[i - 1] + cost[i - 1]).min(f[i - 2] + cost[i - 2]);
        }
        f[n]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{cost}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 五、空间优化

观察状态转移方程，发现一旦算出 $f[i]$，那么 $f[i-2]$ 及其左边的状态就永远不会用到了。

这意味着每次循环，只需要知道「上一个状态」和「上上一个状态」的 $f$ 值是多少，分别记作 $f_1$ 和 $f_0$。它俩的初始值均为 $0$，对应着 $f[1]$ 和 $f[0]$。

每次循环，计算出新的状态 $\textit{newF} = \min(f_1 + \textit{cost}[i-1], f_0 + \textit{cost}[i-2])$，那么对于下一轮循环来说：

- 「上上一个状态」就是 $f_1$，更新 $f_0 = f_1$。
- 「上一个状态」就是 $\textit{newF}$，更新 $f_1 = \textit{newF}$。

最后答案为 $f_1$，因为最后一轮循环算出的 $\textit{newF}$ 赋给了 $f_1$。

代码实现时，可以把 $i$ 改成从 $1$ 遍历到 $n-1$，这样 $\textit{newF} = \min(f_1 + \textit{cost}[i], f_0 + \textit{cost}[i-1])$，可以简化一点代码。

```py [sol-Python3]
# 更简洁的代码见写法二
class Solution:
    def minCostClimbingStairs(self, cost: List[int]) -> int:
        n = len(cost)
        f0 = f1 = 0
        for i in range(1, n):
            new_f = min(f1 + cost[i], f0 + cost[i - 1])
            f0 = f1
            f1 = new_f
        return f1
```

```py [sol-Python3 写法二]
class Solution:
    def minCostClimbingStairs(self, cost: List[int]) -> int:
        f0 = f1 = 0
        for c0, c1 in pairwise(cost):
            f0, f1 = f1, min(f1 + c1, f0 + c0)
        return f1
```

```java [sol-Java]
class Solution {
    public int minCostClimbingStairs(int[] cost) {
        int f0 = 0, f1 = 0;
        for (int i = 1; i < cost.length; i++) {
            int newF = Math.min(f1 + cost[i], f0 + cost[i - 1]);
            f0 = f1;
            f1 = newF;
        }
        return f1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minCostClimbingStairs(vector<int>& cost) {
        int f0 = 0, f1 = 0;
        for (int i = 1; i < cost.size(); i++) {
            int new_f = min(f1 + cost[i], f0 + cost[i - 1]);
            f0 = f1;
            f1 = new_f;
        }
        return f1;
    }
};
```

```go [sol-Go]
func minCostClimbingStairs(cost []int) int {
    f0, f1 := 0, 0
    for i := 1; i < len(cost); i++ {
        f0, f1 = f1, min(f1+cost[i], f0+cost[i-1])
    }
    return f1
}
```

```js [sol-JavaScript]
var minCostClimbingStairs = function(cost) {
    let f0 = 0, f1 = 0;
    for (let i = 1; i < cost.length; i++) {
        let newF = Math.min(f1 + cost[i], f0 + cost[i - 1]);
        f0 = f1;
        f1 = newF;
    }
    return f1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_cost_climbing_stairs(cost: Vec<i32>) -> i32 {
        let mut f0 = 0;
        let mut f1 = 0;
        for i in 1..cost.len() {
            let new_f = (f1 + cost[i]).min(f0 + cost[i - 1]);
            f0 = f1;
            f1 = new_f;
        }
        f1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{cost}$ 的长度。
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

本题来自 `一、入门 DP / §1.1 爬楼梯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、入门 DP / §1.1 爬楼梯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
