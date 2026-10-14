# 122. 买卖股票的最佳时机 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-ii/
- 题目 slug：`best-time-to-buy-and-sell-stock-ii`
- 来源专题：动态规划
- 来源分类路径：六、状态机 DP / §6.1 买卖股票
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-ii/solutions/2201406/shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-o3y4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】教你一步步思考动态规划，股票问题通用解法（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-ii/solutions/2201406/shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-o3y4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-o3y4`
- topic id：`2201406`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

请看[【基础算法精讲 21】](https://www.bilibili.com/video/BV1ho4y1W7QK/)。如果这个视频对你有帮助，欢迎一键三连~

## 一、递归搜索 + 保存计算结果 = 记忆化搜索

注意本题 $\textit{dfs}$ 可能会算出负数，不能用 $-1$ 作为 $\textit{memo}$ 数组的初始值。可以改用其他 $\textit{dfs}$ 算不出的值。

```py [sol-Python3]
class Solution:
    def maxProfit(self, prices: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int, hold: bool) -> int:
            if i < 0:
                return -inf if hold else 0
            if hold:
                return max(dfs(i - 1, True), dfs(i - 1, False) - prices[i])
            return max(dfs(i - 1, False), dfs(i - 1, True) + prices[i])

        return dfs(len(prices) - 1, False)
```

```java [sol-Java]
class Solution {
    private int[] prices;
    private int[][] memo;

    public int maxProfit(int[] prices) {
        this.prices = prices;
        int n = prices.length;
        memo = new int[n][2];
        for (int[] row : memo) {
            Arrays.fill(row, Integer.MIN_VALUE); // MIN_VALUE 表示还没有计算过
        }
        return dfs(n - 1, 0);
    }

    private int dfs(int i, int hold) {
        if (i < 0) {
            return hold == 1 ? Integer.MIN_VALUE / 2 : 0;
        }

        if (memo[i][hold] != Integer.MIN_VALUE) { // 之前计算过
            return memo[i][hold];
        }

        if (hold == 1) {
            return memo[i][hold] = Math.max(dfs(i - 1, 1), dfs(i - 1, 0) - prices[i]);
        }
        return memo[i][hold] = Math.max(dfs(i - 1, 0), dfs(i - 1, 1) + prices[i]);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int n = prices.size();
        vector<array<int, 2>> memo(n, {INT_MIN, INT_MIN}); // INT_MIN 表示还没有计算过

        auto dfs = [&](this auto&& dfs, int i, bool hold) -> int {
            if (i < 0) {
                return hold ? INT_MIN / 2 : 0;
            }

            int& res = memo[i][hold]; // 注意这里是引用
            if (res != INT_MIN) { // 之前计算过
                return res;
            }

            if (hold) {
                return res = max(dfs(i - 1, true), dfs(i - 1, false) - prices[i]);
            }
            return res = max(dfs(i - 1, false), dfs(i - 1, true) + prices[i]);
        };

        return dfs(n - 1, false);
    }
};
```

```go [sol-Go]
func maxProfit(prices []int) int {
    n := len(prices)
    memo := make([][2]int, n)
    for i := range memo {
        memo[i] = [2]int{math.MinInt, math.MinInt} // MinInt 表示还没有计算过
    }

    var dfs func(int, int) int
    dfs = func(i, hold int) (res int) {
        if i < 0 {
            if hold == 1 {
                return math.MinInt / 2
            }
            return
        }

        p := &memo[i][hold]
        if *p != math.MinInt { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化

        if hold == 1 {
            return max(dfs(i-1, 1), dfs(i-1, 0)-prices[i])
        }
        return max(dfs(i-1, 0), dfs(i-1, 1)+prices[i])
    }

    return dfs(n-1, 0)
}
```

```js [sol-JavaScript]
var maxProfit = function(prices) {
    const n = prices.length;
    const memo = new Array(n).fill(null).map(() => [Infinity, Infinity]); // Infinity 表示没有计算过

    function dfs(i, hold) {
        if (i < 0) {
            return hold ? -Infinity : 0;
        }

        if (memo[i][hold] !== Infinity) { // 之前计算过
            return memo[i][hold];
        }

        let res;
        if (hold) {
            res = Math.max(dfs(i - 1, 1), dfs(i - 1, 0) - prices[i]);
        } else {
            res = Math.max(dfs(i - 1, 0), dfs(i - 1, 1) + prices[i]);
        }
        return memo[i][hold] = res; // 记忆化
    }

    return dfs(n - 1, 0);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_profit(prices: Vec<i32>) -> i32 {
        fn dfs(idx: i32, hold: bool, prices: &Vec<i32>, memo: &mut Vec<Vec<i32>>) -> i32 {
            if idx < 0 {
                if hold {
                    return i32::MIN / 2;
                }
                return 0;
            }

            let i = idx as usize;
            let j = hold as usize;
            if memo[i][j] != i32::MIN { // 之前计算过
                return memo[i][j];
            }

            let res;
            if hold {
                res = dfs(idx - 1, true, prices, memo).max(dfs(idx - 1, false, prices, memo) - prices[i]);
            } else {
                res = dfs(idx - 1, false, prices, memo).max(dfs(idx - 1, true, prices, memo) + prices[i]);
            }
            memo[i][j] = res; // 记忆化
            res
        }

        let n = prices.len();
        let mut memo = vec![vec![i32::MIN; 2]; n]; // i32::MIN 表示还没有计算过
        dfs(n as i32 - 1, false, &prices, &mut memo)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{prices}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 二、1:1 翻译成递推

```py [sol-Python3]
class Solution:
    def maxProfit(self, prices: List[int]) -> int:
        n = len(prices)
        f = [[0] * 2 for _ in range(n + 1)]
        f[0][1] = -inf
        for i, p in enumerate(prices):
            f[i + 1][0] = max(f[i][0], f[i][1] + p)
            f[i + 1][1] = max(f[i][1], f[i][0] - p)
        return f[n][0]
```

```java [sol-Java]
class Solution {
    public int maxProfit(int[] prices) {
        int n = prices.length;
        int[][] f = new int[n + 1][2];
        f[0][1] = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = Math.max(f[i][0], f[i][1] + prices[i]);
            f[i + 1][1] = Math.max(f[i][1], f[i][0] - prices[i]);
        }
        return f[n][0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int n = prices.size();
        vector<array<int, 2>> f(n + 1);
        f[0][1] = INT_MIN;
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = max(f[i][0], f[i][1] + prices[i]);
            f[i + 1][1] = max(f[i][1], f[i][0] - prices[i]);
        }
        return f[n][0];
    }
};
```

```go [sol-Go]
func maxProfit(prices []int) int {
    n := len(prices)
    f := make([][2]int, n+1)
    f[0][1] = math.MinInt
    for i, p := range prices {
        f[i+1][0] = max(f[i][0], f[i][1]+p)
        f[i+1][1] = max(f[i][1], f[i][0]-p)
    }
    return f[n][0]
}
```

```js [sol-JavaScript]
var maxProfit = function(prices) {
    const n = prices.length;
    const f = Array(n + 1).fill(null).map(() => [0, 0]);
    f[0][1] = -Infinity;
    for (let i = 0; i < n; i++) {
        f[i + 1][0] = Math.max(f[i][0], f[i][1] + prices[i]);
        f[i + 1][1] = Math.max(f[i][1], f[i][0] - prices[i]);
    }
    return f[n][0];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_profit(prices: Vec<i32>) -> i32 {
        let n = prices.len();
        let mut f = vec![(0, 0); n + 1];
        f[0].1 = i32::MIN;
        for (i, &p) in prices.iter().enumerate() {
            f[i + 1].0 = f[i].0.max(f[i].1 + p);
            f[i + 1].1 = f[i].1.max(f[i].0 - p);
        }
        f[n].0
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{prices}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 三、空间优化

```py [sol-Python3]
class Solution:
    def maxProfit(self, prices: List[int]) -> int:
        f0, f1 = 0, -inf
        for p in prices:
            f0, f1 = max(f0, f1 + p), max(f1, f0 - p)
        return f0
```

```java [sol-Java]
class Solution {
    public int maxProfit(int[] prices) {
        int f0 = 0;
        int f1 = Integer.MIN_VALUE;
        for (int p : prices) {
            int newF0 = Math.max(f0, f1 + p);
            f1 = Math.max(f1, f0 - p);
            f0 = newF0;
        }
        return f0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int f0 = 0, f1 = INT_MIN;
        for (int p: prices) {
            int new_f0 = max(f0, f1 + p);
            f1 = max(f1, f0 - p);
            f0 = new_f0;
        }
        return f0;
    }
};
```

```go [sol-Go]
func maxProfit(prices []int) int {
    f0, f1 := 0, math.MinInt
    for _, p := range prices {
        f0, f1 = max(f0, f1+p), max(f1, f0-p)
    }
    return f0
}
```

```js [sol-JavaScript]
var maxProfit = function(prices) {
    let f0 = 0, f1 = -Infinity;
    for (let p of prices) {
        [f0, f1] = [Math.max(f0, f1 + p), Math.max(f1, f0 - p)];
    }
    return f0;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_profit(prices: Vec<i32>) -> i32 {
        let mut f0 = 0;
        let mut f1 = i32::MIN;
        for p in prices {
            let new_f0 = f0.max(f1 + p);
            f1 = f1.max(f0 - p);
            f0 = new_f0;
        }
        f0
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{prices}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 股票买卖系列题目

- [121. 买卖股票的最佳时机](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock/)
- [122. 买卖股票的最佳时机 II](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-ii/)
- [123. 买卖股票的最佳时机 III](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-iii/)
- [188. 买卖股票的最佳时机 IV](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-iv/)
- [309. 买卖股票的最佳时机含冷冻期](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-with-cooldown/)
- [714. 买卖股票的最佳时机含手续费](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-with-transaction-fee/)

更多相似题目，见下面动态规划题单中的「**五、状态机 DP**」。

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
