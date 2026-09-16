# 309. 买卖股票的最佳时机含冷冻期

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-with-cooldown/
- 题目 slug：`best-time-to-buy-and-sell-stock-with-cooldown`
- 来源专题：动态规划
- 来源分类路径：六、状态机 DP / §6.1 买卖股票
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-with-cooldown/solutions/2201415/shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-0k0l/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】教你一步步思考动态规划！股票问题通用解法！（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-with-cooldown/solutions/2201415/shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-0k0l/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-0k0l`
- topic id：`2201415`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

请看[【基础算法精讲 21】](https://www.bilibili.com/video/BV1ho4y1W7QK/)，欢迎点赞关注~

## 一、递归搜索 + 保存计算结果 = 记忆化搜索

在 [122. 买卖股票的最佳时机 II](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-ii/solution/shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-o3y4/) 的基础上，只需修改**一处**：在计算持有股票的状态时，把 $\textit{dfs}(i-1,0)$ 改成 $\textit{dfs}(i-2,0)$。道理和 [198. 打家劫舍](https://leetcode.cn/problems/house-robber/) 是一样的，因为第 $i$ 天买股票的话第 $i-1$ 天不能卖，只能从第 $i-2$ 天没有股票的状态转移过来。注意 $\textit{dfs}(i-2,0)$ 并不意味着第 $i-2$ 天一定卖了股票，而是在没有股票下的最优状态。

请注意，这会导致边界条件多了一个 $\textit{dfs}(-2,0)=0$，后面空间优化中的 $\textit{pre}_0$ 指的就是这个状态。

请注意，$\textit{dfs}(-2,1)$ 是访问不到的，所以下面翻译成递推时，无需初始化这个状态（不需要写 $f[0][1]=-\infty$）。

```py [sol-Python3]
class Solution:
    def maxProfit(self, prices: List[int]) -> int:
        n = len(prices)
        @cache
        def dfs(i: int, hold: bool) -> int:
            if i < 0:
                return -inf if hold else 0
            if hold:
                return max(dfs(i - 1, True), dfs(i - 2, False) - prices[i])
            return max(dfs(i - 1, False), dfs(i - 1, True) + prices[i])
        return dfs(n - 1, False)
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
            return memo[i][hold] = Math.max(dfs(i - 1, 1), dfs(i - 2, 0) - prices[i]);
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
                return res = max(dfs(i - 1, true), dfs(i - 2, false) - prices[i]);
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
            return max(dfs(i-1, 1), dfs(i-2, 0)-prices[i])
        }
        return max(dfs(i-1, 0), dfs(i-1, 1)+prices[i])
    }

    return dfs(n-1, 0)
}
```

```js [sol-JavaScript]
var maxProfit = function(prices) {
    const n = prices.length;
    const memo = Array.from({length: n}, () => [Infinity, Infinity]); // Infinity 表示没有计算过

    function dfs(i, hold) {
        if (i < 0) {
            return hold ? -Infinity : 0;
        }
        if (memo[i][hold] !== Infinity) { // 之前计算过
            return memo[i][hold];
        }
        let res;
        if (hold) {
            res = Math.max(dfs(i - 1, 1), dfs(i - 2, 0) - prices[i]);
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
                return if hold { i32::MIN / 2 } else { 0 };
            }
            let i = idx as usize;
            let j = hold as usize;
            if memo[i][j] != i32::MIN { // 之前计算过
                return memo[i][j];
            }
            if hold {
                memo[i][j] = dfs(idx - 1, true, prices, memo).max(dfs(idx - 2, false, prices, memo) - prices[i]);
            } else {
                memo[i][j] = dfs(idx - 1, false, prices, memo).max(dfs(idx - 1, true, prices, memo) + prices[i]);
            }
            memo[i][j]
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
        f = [[0] * 2 for _ in range(n + 2)]
        f[1][1] = -inf
        for i, p in enumerate(prices):
            f[i + 2][0] = max(f[i + 1][0], f[i + 1][1] + p)
            f[i + 2][1] = max(f[i + 1][1], f[i][0] - p)
        return f[-1][0]
```

```java [sol-Java]
class Solution {
    public int maxProfit(int[] prices) {
        int n = prices.length;
        int[][] f = new int[n + 2][2];
        f[1][1] = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++) {
            f[i + 2][0] = Math.max(f[i + 1][0], f[i + 1][1] + prices[i]);
            f[i + 2][1] = Math.max(f[i + 1][1], f[i][0] - prices[i]);
        }
        return f[n + 1][0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int n = prices.size();
        vector<array<int, 2>> f(n + 2);
        f[1][1] = INT_MIN;
        for (int i = 0; i < n; i++) {
            f[i + 2][0] = max(f[i + 1][0], f[i + 1][1] + prices[i]);
            f[i + 2][1] = max(f[i + 1][1], f[i][0] - prices[i]);
        }
        return f[n + 1][0];
    }
};
```

```go [sol-Go]
func maxProfit(prices []int) int {
    n := len(prices)
    f := make([][2]int, n+2)
    f[1][1] = math.MinInt
    for i, p := range prices {
        f[i+2][0] = max(f[i+1][0], f[i+1][1]+p)
        f[i+2][1] = max(f[i+1][1], f[i][0]-p)
    }
    return f[n+1][0]
}
```

```js [sol-JavaScript]
var maxProfit = function(prices) {
    const n = prices.length;
    const f = Array(n + 2).fill(null).map(() => [0, 0]);
    f[1][1] = -Infinity;
    for (let i = 0; i < n; i++) {
        f[i + 2][0] = Math.max(f[i + 1][0], f[i + 1][1] + prices[i]);
        f[i + 2][1] = Math.max(f[i + 1][1], f[i][0] - prices[i]);
    }
    return f[n + 1][0];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_profit(prices: Vec<i32>) -> i32 {
        let n = prices.len();
        let mut f = vec![(0, 0); n + 2];
        f[1].1 = i32::MIN;
        for (i, &p) in prices.iter().enumerate() {
            f[i + 2].0 = f[i + 1].0.max(f[i + 1].1 + p);
            f[i + 2].1 = f[i + 1].1.max(f[i].0 - p);
        }
        f[n + 1].0
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
        pre0, f0, f1, = 0, 0, -inf
        for p in prices:
            pre0, f0, f1 = f0, max(f0, f1 + p), max(f1, pre0 - p)
        return f0
```

```java [sol-Java]
class Solution {
    public int maxProfit(int[] prices) {
        int pre0 = 0;
        int f0 = 0;
        int f1 = Integer.MIN_VALUE;
        for (int p : prices) {
            int newF0 = Math.max(f0, f1 + p); // f[i+2][0]
            f1 = Math.max(f1, pre0 - p); // f[i+2][1]
            pre0 = f0;
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
        int pre0 = 0, f0 = 0, f1 = INT_MIN;
        for (int p: prices) {
            int new_f0 = max(f0, f1 + p); // f[i+2][0]
            f1 = max(f1, pre0 - p); // f[i+2][1]
            pre0 = f0;
            f0 = new_f0;
        }
        return f0;
    }
};
```

```go [sol-Go]
func maxProfit(prices []int) int {
    pre0, f0, f1 := 0, 0, math.MinInt
    for _, p := range prices {
        pre0, f0, f1 = f0, max(f0, f1+p), max(f1, pre0-p)
    }
    return f0
}
```

```js [sol-JavaScript]
var maxProfit = function(prices) {
    let pre0 = 0, f0 = 0, f1 = -Infinity;
    for (let p of prices) {
        [pre0, f0, f1] = [f0, Math.max(f0, f1 + p), Math.max(f1, pre0 - p)];
    }
    return f0;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_profit(prices: Vec<i32>) -> i32 {
        let mut pre0 = 0;
        let mut f0 = 0;
        let mut f1 = i32::MIN;
        for p in prices {
            let new_f0 = f0.max(f1 + p); // f[i+2][0]
            f1 = f1.max(pre0 - p); // f[i+2][1]
            pre0 = f0;
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

本题来自 `六、状态机 DP / §6.1 买卖股票`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、状态机 DP / §6.1 买卖股票`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
