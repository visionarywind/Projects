# 518. 零钱兑换 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/coin-change-ii/
- 题目 slug：`coin-change-ii`
- 来源专题：动态规划
- 来源分类路径：三、背包 / §3.2 完全背包
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/coin-change-ii/solutions/2706227/shi-pin-wan-quan-bei-bao-cong-ji-yi-hua-o3ew0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】完全背包：从记忆化搜索到递推，附题单！（Python/Java/C++/Go）](https://leetcode.cn/problems/coin-change-ii/solutions/2706227/shi-pin-wan-quan-bei-bao-cong-ji-yi-hua-o3ew0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-wan-quan-bei-bao-cong-ji-yi-hua-o3ew0`
- topic id：`2706227`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

请看[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)。如果这个视频对你有帮助，欢迎一键三连！

## 思路

本题思路和 [322. 零钱兑换](https://leetcode.cn/problems/coin-change/) 一样，定义 $\textit{dfs}(i,c)$ 表示用前 $i$ 种硬币组成金额 $c$ 的方案数，考虑「选或不选」，有：

- 不再继续选第 $i$ 种硬币：$\textit{dfs}(i-1,c)$。
- 继续选一枚第 $i$ 种硬币：$\textit{dfs}(i,c-\textit{coins}[i])$。

根据**加法原理**，二者相加得

$$
\textit{dfs}(i,c) = \textit{dfs}(i-1,c) + \textit{dfs}(i,c-\textit{coins}[i])
$$

> **加法原理**：如果事件 A 和事件 B 是互斥的（即不能同时发生，不再选硬币的同时，又继续选同一种硬币），那么发生事件 A 或事件 B 的总数等于事件 A 的数量加上事件 B 的数量。

递归边界：$\textit{dfs}(-1,0) = 1,\ \textit{dfs}(-1,>0) = 0$

递归入口：$\textit{dfs}(n-1, \textit{amount})$。

#### 答疑

**问**：当 $c=0$ 的时候，为什么会有一种方案，组成金额 $0$ 不应该没有方案吗？

**答**：「不用任何硬币」也算一种方案。也可以这样想，如果 $c=0$ 的时候返回 $0$，那么所有返回值都会是 $0$，这显然是错误的。

## 一、递归搜索 + 保存计算结果 = 记忆化搜索

```py [sol-Python3]
class Solution:
    def change(self, amount: int, coins: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int, c: int) -> int:
            if i < 0:
                return 1 if c == 0 else 0
            if c < coins[i]:  # 只能不选
                return dfs(i - 1, c)
            # 不选 + 继续选
            return dfs(i - 1, c) + dfs(i, c - coins[i])

        return dfs(len(coins) - 1, amount)
```

```java [sol-Java]
class Solution {
    public int change(int amount, int[] coins) {
        int n = coins.length;
        int[][] memo = new int[n][amount + 1];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }

        return dfs(n - 1, amount, coins, memo);
    }

    private int dfs(int i, int c, int[] coins, int[][] memo) {
        if (i < 0) {
            return c == 0 ? 1 : 0;
        }
        if (memo[i][c] != -1) { // 之前算过了
            return memo[i][c];
        }
        if (c < coins[i]) { // 只能不选
            return memo[i][c] = dfs(i - 1, c, coins, memo);
        }
        // 不选 + 继续选
        return memo[i][c] = dfs(i - 1, c, coins, memo) + dfs(i, c - coins[i], coins, memo);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int change(int amount, vector<int>& coins) {
        int n = coins.size();
        vector memo(n, vector<int>(amount + 1, -1)); // -1 表示没有计算过

        // lambda 递归函数
        auto dfs = [&](this auto&& dfs, int i, int c) -> int {
            if (i < 0) {
                return c == 0 ? 1 : 0;
            }
            int& res = memo[i][c]; // 注意这里是引用
            if (res != -1) { // 之前算过了
                return res;
            }
            if (c < coins[i]) { // 只能不选
                return res = dfs(i - 1, c);
            }
            // 不选 + 继续选
            return res = dfs(i - 1, c) + dfs(i, c - coins[i]);
        };

        return dfs(n - 1, amount);
    }
};
```

```go [sol-Go]
func change(amount int, coins []int) int {
    n := len(coins)
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, amount+1)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }

    var dfs func(int, int) int
    dfs = func(i, c int) (res int) {
        if i < 0 {
            if c == 0 {
                return 1
            }
            return
        }
        p := &memo[i][c]
        if *p != -1 { // 之前算过了
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if c < coins[i] { // 只能不选
            return dfs(i-1, c)
        }
        return dfs(i-1, c) + dfs(i, c-coins[i]) // 不选 + 继续选
    }

    return dfs(n-1, amount)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot\textit{amount})$，其中 $n$ 为 $\textit{coins}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n\cdot\textit{amount})$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n\cdot\textit{amount})$。
- 空间复杂度：$\mathcal{O}(n\cdot\textit{amount})$。

## 二、1:1 翻译成递推

请注意循环的顺序。如果改成外层枚举背包容量，内层枚举物品，那么计算的是排列（本题我们要算的是组合）。

读者可以做做 [377. 组合总和 Ⅳ](https://leetcode.cn/problems/combination-sum-iv/)，比对这两题的代码，加深对这类 DP 的理解。

```py [sol-Python3]
class Solution:
    def change(self, amount: int, coins: List[int]) -> int:
        n = len(coins)
        f = [[0] * (amount + 1) for _ in range(n + 1)]
        f[0][0] = 1
        for i, x in enumerate(coins):
            for c in range(amount + 1):
                if c < x:
                    f[i + 1][c] = f[i][c]
                else:
                    f[i + 1][c] = f[i][c] + f[i + 1][c - x]
        return f[n][amount]
```

```java [sol-Java]
class Solution {
    public int change(int amount, int[] coins) {
        int n = coins.length;
        int[][] f = new int[n + 1][amount + 1];
        f[0][0] = 1;
        for (int i = 0; i < n; i++) {
            for (int c = 0; c <= amount; c++) {
                if (c < coins[i]) {
                    f[i + 1][c] = f[i][c];
                } else {
                    f[i + 1][c] = f[i][c] + f[i + 1][c - coins[i]];
                }
            }
        }
        return f[n][amount];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int change(int amount, vector<int>& coins) {
        int n = coins.size();
        // 和答案无关的转移可能会溢出，从而报错
        // 为了避免报错，使用 unsigned
        vector f(n + 1, vector<unsigned>(amount + 1));
        f[0][0] = 1;
        for (int i = 0; i < n; i++) {
            for (int c = 0; c <= amount; c++) {
                if (c < coins[i]) {
                    f[i + 1][c] = f[i][c];
                } else {
                    f[i + 1][c] = f[i][c] + f[i + 1][c - coins[i]];
                }
            }
        }
        return f[n][amount];
    }
};
```

```go [sol-Go]
func change(amount int, coins []int) int {
    n := len(coins)
    f := make([][]int, n+1)
    for i := range f {
        f[i] = make([]int, amount+1)
    }
    f[0][0] = 1
    for i, x := range coins {
        for c := 0; c <= amount; c++ {
            if c < x {
                f[i+1][c] = f[i][c]
            } else {
                f[i+1][c] = f[i][c] + f[i+1][c-x]
            }
        }
    }
    return f[n][amount]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot\textit{amount})$，其中 $n$ 为 $\textit{coins}$ 的长度。
- 空间复杂度：$\mathcal{O}(n\cdot\textit{amount})$。

## 三、空间优化

```py [sol-Python3]
class Solution:
    def change(self, amount: int, coins: List[int]) -> int:
        f = [1] + [0] * amount
        for x in coins:
            for c in range(x, amount + 1):
                f[c] += f[c - x]
        return f[amount]
```

```java [sol-Java]
class Solution {
    public int change(int amount, int[] coins) {
        int[] f = new int[amount + 1];
        f[0] = 1;
        for (int x : coins) {
            for (int c = x; c <= amount; c++) {
                f[c] += f[c - x];
            }
        }
        return f[amount];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int change(int amount, vector<int>& coins) {
        // 和答案无关的转移可能会溢出，从而报错
        // 为了避免报错，使用 unsigned
        vector<unsigned> f(amount + 1);
        f[0] = 1;
        for (int x : coins) {
            for (int c = x; c <= amount; c++) {
                f[c] += f[c - x];
            }
        }
        return f[amount];
    }
};
```

```go [sol-Go]
func change(amount int, coins []int) int {
    f := make([]int, amount+1)
    f[0] = 1
    for _, x := range coins {
        for c := x; c <= amount; c++ {
            f[c] += f[c-x]
        }
    }
    return f[amount]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\cdot\textit{amount})$，其中 $n$ 为 $\textit{coins}$ 的长度。
- 空间复杂度：$\mathcal{O}(\textit{amount})$。

## 反向问题

给定 $f$ 数组，请你还原 $\textit{coins}$ 数组。

这题是 [3592. 硬币面值还原](https://leetcode.cn/problems/inverse-coin-change/)。

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

本题来自 `三、背包 / §3.2 完全背包`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、背包 / §3.2 完全背包`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
