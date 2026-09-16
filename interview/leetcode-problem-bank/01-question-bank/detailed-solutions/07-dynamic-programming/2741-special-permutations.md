# 2741. 特别的排列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/special-permutations/
- 题目 slug：`special-permutations`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.2 排列型状压 DP ② 相邻相关
- 难度分：2021
- 外部题解来源：https://leetcode.cn/problems/special-permutations/solutions/2312798/zhuang-ya-dp-by-endlesscheng-4jkr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[状压 DP：排列型 ② 相邻相关（Python/Java/C++/Go）](https://leetcode.cn/problems/special-permutations/solutions/2312798/zhuang-ya-dp-by-endlesscheng-4jkr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuang-ya-dp-by-endlesscheng-4jkr`
- topic id：`2312798`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前言

本题属于 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的状压 DP「排列型 ② 相邻相关」。

如果你从未做过状压 DP，推荐先做一道「排列型 ① 相邻无关」的题目，例如 [526. 优美的排列](https://leetcode.cn/problems/beautiful-arrangement/)。

由于本题与 526 题类似，所以下面的内容，我会接着 [526 题解](https://leetcode.cn/problems/beautiful-arrangement/solution/jiao-ni-yi-bu-bu-si-kao-zhuang-ya-dpcong-c6kd/) 继续讲。

## 一、记忆化搜索

下面会用到一些集合论的术语和符号，请看 [从集合论到位运算](https://leetcode.cn/circle/discuss/CaOJ45/)。

相比 526（题解的第二种状态定义），本题需要额外知道上一个选的数的下标是多少，因此要多一个参数。

定义 $\textit{dfs}(S,i)$ 表示在可以选的下标集合为 $S$，上一个选的数的下标是 $i$ 时，可以构造出多少个特别排列。

枚举当前要选的数的下标 $j$，那么接下来要解决的问题是，在可以选的下标集合为 $S\setminus \{j\}$，上一个选的数的下标是 $j$ 时，可以构造出多少个特别排列。

累加这些方案数，得

$$
\textit{dfs}(S,i) = \sum_{j\in S} \textit{dfs}(S\setminus \{j\},j)
$$

其中 $j$ 满足 $\textit{nums}[j]\bmod \textit{nums}[i]=0$ 或 $\textit{nums}[i]\bmod \textit{nums}[j]=0$。

**递归边界**：$\textit{dfs}(\varnothing,i) = 1$，表示找到了一个特别排列。

**递归入口**：$\textit{dfs}(U\setminus \{i\},i)$，其中全集 $U=\{0,1,2,\cdots,n-1\}$。

枚举特别排列的第一个数的下标 $i$，累加所有 $\textit{dfs}(U\setminus \{i\},i)$，即为答案。

```py [sol-Python3]
class Solution:
    def specialPerm(self, nums: List[int]) -> int:
        @cache
        def dfs(s: int, i: int) -> int:
            if s == 0:
                return 1  # 找到一个特别排列
            res = 0
            pre = nums[i]
            for j, x in enumerate(nums):
                if s >> j & 1 and (pre % x == 0 or x % pre == 0):
                    res += dfs(s ^ (1 << j), j)
            return res

        n = len(nums)
        u = (1 << n) - 1
        return sum(dfs(u ^ (1 << i), i) for i in range(n)) % 1_000_000_007
```

```java [sol-Java]
class Solution {
    public int specialPerm(int[] nums) {
        int n = nums.length;
        int u = (1 << n) - 1;
        long[][] memo = new long[u][n];
        for (long[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        long ans = 0;
        for (int i = 0; i < n; i++) {
            ans += dfs(u ^ (1 << i), i, nums, memo);
        }
        return (int) (ans % 1_000_000_007);
    }

    private long dfs(int s, int i, int[] nums, long[][] memo) {
        if (s == 0) {
            return 1; // 找到一个特别排列
        }
        if (memo[s][i] != -1) { // 之前计算过
            return memo[s][i];
        }
        long res = 0;
        for (int j = 0; j < nums.length; j++) {
            if ((s >> j & 1) > 0 && (nums[i] % nums[j] == 0 || nums[j] % nums[i] == 0)) {
                res += dfs(s ^ (1 << j), j, nums, memo);
            }
        }
        return memo[s][i] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int specialPerm(vector<int>& nums) {
        int n = nums.size(), u = (1 << n) - 1;
        vector<vector<long long>> memo(u, vector<long long>(n, -1)); // -1 表示没有计算过
        auto dfs = [&](auto&& dfs, int s, int i) -> long long {
            if (s == 0) {
                return 1; // 找到一个特别排列
            }
            auto& res = memo[s][i]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            res = 0;
            for (int j = 0; j < n; j++) {
                if ((s >> j & 1) && (nums[i] % nums[j] == 0 || nums[j] % nums[i] == 0)) {
                    res += dfs(dfs, s ^ (1 << j), j);
                }
            }
            return res;
        };
        long long ans = 0;
        for (int i = 0; i < n; i++) {
            ans += dfs(dfs, u ^ (1 << i), i);
        }
        return ans % 1'000'000'007;
    }
};
```

```go [sol-Go]
func specialPerm(nums []int) (ans int) {
    n := len(nums)
    u := 1<<n - 1
    memo := make([][]int, u)
    for i := range memo {
        memo[i] = make([]int, n)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(s, i int) (res int) {
        if s == 0 {
            return 1 // 找到一个特别排列
        }
        p := &memo[s][i]
        if *p != -1 { // 之前计算过
            return *p
        }
        for j, x := range nums {
            if s>>j&1 > 0 && (nums[i]%x == 0 || x%nums[i] == 0) {
                res += dfs(s^(1<<j), j)
            }
        }
        *p = res // 记忆化
        return
    }
    for i := range nums {
        ans += dfs(u^(1<<i), i)
    }
    return ans % 1_000_000_007
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^22^n)$，其中 $n$ 为 $\textit{nums}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n2^n)$，单个状态的计算时间为 $\mathcal{O}(n)$，因此时间复杂度为 $\mathcal{O}(n^22^n)$。
- 空间复杂度：$\mathcal{O}(n2^n)$。保存多少状态，就需要多少空间。

## 二、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[S][i]$ 的定义和 $\textit{dfs}(S,i)$ 的定义是一样的，都表示在可以选的下标集合为 $S$，上一个选的数的下标是 $i$ 时，可以构造出多少个特别排列。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[S][i] =\sum_{j\in S} f[S\setminus \{j\}][j]
$$

初始值 $f[\varnothing][i]=1$，翻译自递归边界 $\textit{dfs}(\varnothing,i)=1$。

答案为 $f[U\setminus \{i\}][i]$ 之和，翻译自递归入口 $\textit{dfs}(U\setminus \{i\},i)$。

> 注：在随机数据下，由于相邻元素无法整除，记忆化搜索有很多状态无法访问到，记忆化搜索比递推快。

```py [sol-Python3]
class Solution:
    def specialPerm(self, nums: List[int]) -> int:
        n = len(nums)
        u = (1 << n) - 1
        f = [[0] * n for _ in range(u)]
        f[0] = [1] * n
        for s in range(1, u):
            for i, pre in enumerate(nums):
                if s >> i & 1:
                    continue
                for j, x in enumerate(nums):
                    if s >> j & 1 and (pre % x == 0 or x % pre == 0):
                        f[s][i] += f[s ^ (1 << j)][j]
        return sum(f[u ^ (1 << i)][i] for i in range(n)) % 1_000_000_007
```

```java [sol-Java]
class Solution {
    public int specialPerm(int[] nums) {
        int n = nums.length;
        int u = (1 << n) - 1;
        long[][] f = new long[u][n];
        Arrays.fill(f[0], 1L);
        for (int s = 1; s < u; s++) {
            for (int i = 0; i < n; i++) {
                if ((s >> i & 1) != 0) {
                    continue;
                }
                for (int j = 0; j < n; j++) {
                    if ((s >> j & 1) != 0 && (nums[i] % nums[j] == 0 || nums[j] % nums[i] == 0)) {
                        f[s][i] += f[s ^ (1 << j)][j];
                    }
                }
            }
        }
        long ans = 0;
        for (int i = 0; i < n; i++) {
            ans += f[u ^ (1 << i)][i];
        }
        return (int) (ans % 1_000_000_007);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int specialPerm(vector<int>& nums) {
        int n = nums.size(), u = (1 << n) - 1;
        vector<vector<long long>> f(u, vector<long long>(n));
        ranges::fill(f[0], 1LL);
        for (int s = 1; s < u; s++) {
            for (int i = 0; i < n; i++) {
                if (s >> i & 1) {
                    continue;
                }
                for (int j = 0; j < n; j++) {
                    if ((s >> j & 1) && (nums[i] % nums[j] == 0 || nums[j] % nums[i] == 0)) {
                        f[s][i] += f[s ^ (1 << j)][j];
                    }
                }
            }
        }
        long long ans = 0;
        for (int i = 0; i < n; i++) {
            ans += f[u ^ (1 << i)][i];
        }
        return ans % 1'000'000'007;
    }
};
```

```go [sol-Go]
func specialPerm2(nums []int) (ans int) {
    n := len(nums)
    u := 1<<n - 1
    f := make([][]int, u)
    for i := range f {
        f[i] = make([]int, n)
    }
    for i := range nums {
        f[0][i] = 1
    }
    for s := 1; s < u; s++ {
        for i, pre := range nums {
            if s>>i&1 != 0 {
                continue
            }
            for j, x := range nums {
                if s>>j&1 != 0 && (pre%x == 0 || x%pre == 0) {
                    f[s][i] += f[s^(1<<j)][j]
                }
            }
        }
    }
    for i := range nums {
        ans += f[u^(1<<i)][i]
    }
    return ans % 1_000_000_007
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^22^n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n2^n)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `九、状态压缩 DP（状压 DP） / §9.2 排列型状压 DP ② 相邻相关`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.2 排列型状压 DP ② 相邻相关`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
