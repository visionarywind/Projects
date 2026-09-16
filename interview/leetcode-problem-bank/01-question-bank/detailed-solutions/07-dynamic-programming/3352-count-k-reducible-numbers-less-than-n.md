# 3352. 统计小于 N 的 K 可约简整数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-k-reducible-numbers-less-than-n/
- 题目 slug：`count-k-reducible-numbers-less-than-n`
- 来源专题：动态规划
- 来源分类路径：十、数位 DP / §10.1 统计合法元素的数目
- 难度分：2451
- 外部题解来源：https://leetcode.cn/problems/count-k-reducible-numbers-less-than-n/solutions/2983541/xian-xing-dp-shu-wei-dppythonjavacgo-by-yw0dl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[线性 DP + 数位 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/count-k-reducible-numbers-less-than-n/solutions/2983541/xian-xing-dp-shu-wei-dppythonjavacgo-by-yw0dl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xian-xing-dp-shu-wei-dppythonjavacgo-by-yw0dl`
- topic id：`2983541`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意

定义 $f(x)$ 为 $x$ 的二进制表示中的 $1$ 的个数。

定义 $f^*(x)$ 为使 $f(f(\cdots f(x))) = 1$ 的最少嵌套（迭代）次数。也就是不断地把 $x$ 更新为 $f(x)$，最少要更新多少次，才能使 $x$ 变成 $1$。

例如 $f^*(6) = 2$，因为 $f(f(6)) = f(2) = 1$。

计算 $[1,s)$ 中有多少个数 $x$ 满足 $f^*(x) \le k$。

## 思路

根据定义，我们有

$$
f^*(x) = f^*(f(x)) + 1
$$

设 $s$ 的长度为 $n$。从小到大递推（写一个线性 DP），即可算出 $[1,n-1]$ 的所有 $f^*(x)$ 值。注意题目要求数字小于 $s$，所以不可能有 $n$ 个 $1$。

对于满足 $f^*(x) \le k-1$ 的所有 $x$，我们需要计算，$[1,s)$ 中有多少个二进制数，**恰好**有 $x$ 个 $1$？

这些恰好有 $x$ 个 $1$ 的二进制数，满足 $f^*(x) \le k$。

这可以用**数位 DP** 解决。原理请看 [数位 DP 通用模板](https://www.bilibili.com/video/BV1rS4y1s721/?t=20m05s)。

⚠**注意**：本题需要严格小于 $s$，这可以用 $\textit{isLimit}$ 判断：递归到 $i=n$ 时，若仍有 $\textit{isLimit}=\texttt{true}$，则返回 $0$。此外，我们只关心 $1$ 的个数，是否有前导零无影响，所以无需 $\textit{isNum}$ 参数。

代码实现时，可以定义 $f^*(1) = 1$，上文中 $f^*(x) \le k-1$ 可以简化为 $f^*(x) \le k$。

记得取模。

关于取模的知识点，见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

[本题视频讲解](https://www.bilibili.com/video/BV1JVmBYvEnD/?t=22m35s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def countKReducibleNumbers(self, s: str, k: int) -> int:
        MOD = 1_000_000_007
        n = len(s)

        @cache
        def dfs(i: int, left1: int, is_limit: bool) -> int:
            if i == n:
                return 0 if is_limit or left1 else 1
            up = int(s[i]) if is_limit else 1
            res = 0
            for d in range(min(up, left1) + 1):
                res += dfs(i + 1, left1 - d, is_limit and d == up)
            return res % MOD

        ans = 0
        f = [0] * n
        for i in range(1, n):
            f[i] = f[i.bit_count()] + 1
            if f[i] <= k:
                # 计算有多少个二进制数恰好有 i 个 1
                ans += dfs(0, i, True)
        dfs.cache_clear()  # 防止爆内存
        return ans % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int countKReducibleNumbers(String S, int k) {
        char[] s = S.toCharArray();
        int n = s.length;
        int[][] memo = new int[n][n];
        for (int[] row : memo) {
            Arrays.fill(row, -1);
        }

        long ans = 0;
        int[] f = new int[n];
        for (int i = 1; i < n; i++) {
            f[i] = f[Integer.bitCount(i)] + 1;
            if (f[i] <= k) {
                // 计算有多少个二进制数恰好有 i 个 1
                ans += dfs(0, i, true, s, memo);
            }
        }
        return (int) (ans % MOD);
    }

    private int dfs(int i, int left1, boolean isLimit, char[] s, int[][] memo) {
        if (i == s.length) {
            return !isLimit && left1 == 0 ? 1 : 0;
        }
        if (!isLimit && memo[i][left1] != -1) {
            return memo[i][left1];
        }
        int up = isLimit ? s[i] - '0' : 1;
        int res = 0;
        for (int d = 0; d <= Math.min(up, left1); d++) {
            res = (res + dfs(i + 1, left1 - d, isLimit && d == up, s, memo)) % MOD;
        }
        if (!isLimit) {
            memo[i][left1] = res;
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countKReducibleNumbers(string s, int k) {
        const int MOD = 1'000'000'007;
        int n = s.length();
        vector<vector<int>> memo(n, vector<int>(n, -1));
        auto dfs = [&](auto& dfs, int i, int left1, bool is_limit) -> int {
            if (i == n) {
                return !is_limit && left1 == 0;
            }
            if (!is_limit && memo[i][left1] != -1) {
                return memo[i][left1];
            }
            int up = is_limit ? s[i] - '0' : 1;
            int res = 0;
            for (int d = 0; d <= min(up, left1); d++) {
                res = (res + dfs(dfs, i + 1, left1 - d, is_limit && d == up)) % MOD;
            }
            if (!is_limit) {
                memo[i][left1] = res;
            }
            return res;
        };

        long long ans = 0;
        vector<int> f(n);
        for (int i = 1; i < n; i++) {
            f[i] = f[__builtin_popcount(i)] + 1;
            if (f[i] <= k) {
                // 计算有多少个二进制数恰好有 i 个 1
                ans += dfs(dfs, 0, i, true);
            }
        }
        return ans % MOD;
    }
};
```

```go [sol-Go]
func countKReducibleNumbers(s string, k int) (ans int) {
    const mod = 1_000_000_007
    n := len(s)
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, n)
        for j := range memo[i] {
            memo[i][j] = -1
        }
    }
    var dfs func(int, int, bool) int
    dfs = func(i, left1 int, isLimit bool) (res int) {
        if i == n {
            if !isLimit && left1 == 0 {
                return 1
            }
            return
        }
        if !isLimit {
            p := &memo[i][left1]
            if *p >= 0 {
                return *p
            }
            defer func() { *p = res }()
        }
        up := 1
        if isLimit {
            up = int(s[i] - '0')
        }
        for d := 0; d <= min(up, left1); d++ {
            res += dfs(i+1, left1-d, isLimit && d == up)
        }
        return res % mod
    }

    f := make([]int, n)
    for i := 1; i < n; i++ {
        f[i] = f[bits.OnesCount(uint(i))] + 1
        if f[i] <= k {
            // 计算有多少个二进制数恰好有 i 个 1
            ans += dfs(0, i, true)
        }
    }
    return ans % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n^2)$。

更多相似题目，见下面动态规划题单中的「**十、数位 DP**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `十、数位 DP / §10.1 统计合法元素的数目`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十、数位 DP / §10.1 统计合法元素的数目`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
