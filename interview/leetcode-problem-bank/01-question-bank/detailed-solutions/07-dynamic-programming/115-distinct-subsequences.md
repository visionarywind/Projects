# 115. 不同的子序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/distinct-subsequences/
- 题目 slug：`distinct-subsequences`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/distinct-subsequences/solutions/3060706/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-9va6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推到空间优化！（Python/Java/C++/Go）](https://leetcode.cn/problems/distinct-subsequences/solutions/3060706/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-9va6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-9va6`
- topic id：`3060706`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 什么是子序列？

$s$ 的子序列，是从 $s$ 中删除某些字母后，得到的字符串。比如 $\texttt{abcde}$ 删除 $\texttt{b}$ 和 $\texttt{d}$ 后，得到子序列 $\texttt{ace}$。

**这个定义看似简单，但它就是解决本题的钥匙！请记住这个定义。**

## 一、寻找子问题

在示例 2 中，我们要解决的问题（原问题）是：

- 统计 $s=\texttt{babgbag}$ 的所有子序列中，$t=\texttt{bag}$ 出现的个数。

考虑怎么通过删除字母得到子序列：

- 如果删除 $s$ 最右边的字母 $\textit{g}$，那么需要解决的子问题为：统计 $\texttt{babgba}$ 的所有子序列中，$\texttt{bag}$ 出现的个数。
- 如果不删除 $s$ 最右边的字母 $\textit{g}$，那么需要解决的子问题为：统计 $\texttt{babgba}$ 的所有子序列中，$\texttt{ba}$ 出现的个数。这些等于 $\texttt{ba}$ 的子序列，在末尾加上（没有删除的）字母 $\textit{g}$，就是子序列 $t=\texttt{bag}$ 了。

由于删或不删都会把原问题变成一个**和原问题相似的、规模更小的子问题**，所以可以用**递归**解决。

> 注 1：从右往左思考，主要是**为了方便把递归翻译成递推**。从左往右思考也是可以的。
> 
> 注 2：动态规划有「**选或不选**」和「**枚举选哪个**」两种基本思考方式。子序列相邻无关一般是「选或不选」，子序列相邻相关（例如 LIS 问题）一般是「枚举选哪个」。本题用到的是「选或不选」，或者说「删或不删」。

## 二、状态定义与状态转移方程

根据上面的讨论，我们需要在递归过程中跟踪以下信息：

- $i$：$s$ 最右边的字母的下标。
- $j$：$t$ 最右边的字母的下标。

因此，定义状态为 $\textit{dfs}(i,j)$，表示 $s[..i]$ 的所有子序列中，$t[..j]$ 出现的个数。这里记号 $s[..i]$ 表示 $s[0]$ 到 $s[i]$。

接下来，思考如何从一个状态转移到另一个状态。

考虑是否删除 $s[i]$：

- 删除 $s[i]$，那么接下来要解决的问题是：统计 $s[..i-1]$ 的所有子序列中，$t[..j]$ 出现的个数，即 $\textit{dfs}(i-1,j)$。
- 不删除 $s[i]$（前提是 $s[i]=t[j]$），那么接下来要解决的问题是：统计 $s[..i-1]$ 的所有子序列中，$t[..j-1]$ 出现的个数，即 $\textit{dfs}(i-1,j-1)$。

这两种情况互斥，那么根据**加法原理**，有

$$
\textit{dfs}(i,j) =
\begin{cases}
\textit{dfs}(i-1,j) + \textit{dfs}(i-1,j-1), & s[i]=t[j]    \\
\textit{dfs}(i-1,j), & s[i]\ne t[j]    \\
\end{cases}
$$

**递归边界**：

- $\textit{dfs}(i,-1)=1$。此时 $t$ 是空串，根据定义，我们只有一种方法可以从 $s$ 中得到空串，即删除 $s$ 的所有字母。
- 如果 $i<j$，那么 $\textit{dfs}(i,j)=0$。这是因为我们无法得到一个比 $s[..i]$ 还长的子序列。

**递归入口**：$\textit{dfs}(|s|-1,|t|-1)$，这是原问题，也是答案。其中 $|s|$ 表示字符串 $s$ 的长度。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def numDistinct(self, s: str, t: str) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（一行代码实现记忆化）
        def dfs(i: int, j: int) -> int:
            if i < j:
                return 0
            if j < 0:
                return 1
            res = dfs(i - 1, j)  # 删除 s[i]
            if s[i] == t[j]:
                res += dfs(i - 1, j - 1)  # 不删 s[i]，和 t[j] 匹配
            return res
        return dfs(len(s) - 1, len(t) - 1)
```

```java [sol-Java]
class Solution {
    public int numDistinct(String s, String t) {
        int n = s.length(), m = t.length();
        int[][] memo = new int[n][m];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                memo[i][j] = -1; // -1 表示没有计算过
            }
        }
        return dfs(n - 1, m - 1, s.toCharArray(), t.toCharArray(), memo);
    }

    private int dfs(int i, int j, char[] s, char[] t, int[][] memo) {
        if (i < j) {
            return 0;
        }
        if (j < 0) {
            return 1;
        }
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }
        int res = dfs(i - 1, j, s, t, memo);
        if (s[i] == t[j]) {
            res += dfs(i - 1, j - 1, s, t, memo);
        }
        return memo[i][j] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numDistinct(string s, string t) {
        int n = s.size(), m = t.size();
        vector memo(n, vector<int>(m, -1)); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i < j) {
                return 0;
            }
            if (j < 0) {
                return 1;
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            res = dfs(i - 1, j);
            if (s[i] == t[j]) {
                res += dfs(i - 1, j - 1);
            }
            return res;
        };
        return dfs(n - 1, m - 1);
    }
};
```

```go [sol-Go]
func numDistinct(s, t string) int {
    n, m := len(s), len(t)
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, m)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) (res int) {
        if i < j {
            return
        }
        if j < 0 {
            return 1
        }
        p := &memo[i][j]
        if *p != -1 { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        res = dfs(i-1, j)
        if s[i] == t[j] {
            res += dfs(i-1, j-1)
        }
        return
    }
    return dfs(n-1, m-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 是 $s$ 的长度，$m$ 是 $t$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(nm)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(nm)$。
- 空间复杂度：$\mathcal{O}(nm)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i+1][j+1]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示 $s[..i]$ 的所有子序列中，$t[..j]$ 出现的个数。这里 $+1$ 是为了把 $\textit{dfs}(-1,-1)$ 这个状态也翻译过来，这样我们可以把 $f[0][0]$ 以及其他 $f[i][0]$ 作为初始值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i+1][j+1] =
\begin{cases}
f[i][j+1] + f[i][j], & s[i]=t[j]    \\
f[i][j+1], & s[i]\ne t[j]    \\
\end{cases}
$$

初始值 $f[i][0]=1$，翻译自递归边界 $\textit{dfs}(i,-1)=1$。其余状态初始化成 $0$。

答案为 $f[n][m]$，翻译自递归入口 $\textit{dfs}(n-1,m-1)$。

此外，还有三个优化：

1. 如果 $n<m$，我们无法得到一个比 $s$ 还长的子序列，直接返回 $0$。
2. 内层循环的 $j$ 最大是 $\min(i,m-1)$，因为我们无法得到比 $i+1$ 还长的子序列。
3. 内层循环的 $j$ 最小是 $\max(m-n+i,0)$。注意记忆化搜索的过程中，$j$ 减一，$i$ 也会减一。所以 $j$ 减一的次数（$m-j$）小于或等于 $i$ 减一的次数（$n-i$），即 $m-j\le n-i$，解得 $j\ge m-n+i$。

```py [sol-Python3]
class Solution:
    def numDistinct(self, s: str, t: str) -> int:
        n, m = len(s), len(t)
        if n < m:
            return 0

        f = [[1] + [0] * m for _ in range(n + 1)]
        for i, x in enumerate(s):
            for j in range(max(m - n + i, 0), min(i + 1, m)):
                f[i + 1][j + 1] = f[i][j + 1]
                if x == t[j]:
                    f[i + 1][j + 1] += f[i][j]
        return f[n][m]
```

```java [sol-Java]
class Solution {
    public int numDistinct(String S, String T) {
        int n = S.length();
        int m = T.length();
        if (n < m) {
            return 0;
        }

        char[] s = S.toCharArray();
        char[] t = T.toCharArray();

        int[][] f = new int[n + 1][m + 1];
        f[0][0] = 1;
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = 1;
            for (int j = Math.max(m - n + i, 0); j < Math.min(i + 1, m); j++) {
                f[i + 1][j + 1] = f[i][j + 1];
                if (s[i] == t[j]) {
                    f[i + 1][j + 1] += f[i][j];
                }
            }
        }
        return f[n][m];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numDistinct(string s, string t) {
        int n = s.size(), m = t.size();
        if (n < m) {
            return 0;
        }

        // 和答案无关的转移可能会溢出，从而报错
        // 为了避免报错，使用 unsigned
        vector f(n + 1, vector<unsigned>(m + 1));
        f[0][0] = 1;
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = 1;
            for (int j = max(m - n + i, 0); j < min(i + 1, m); j++) {
                f[i + 1][j + 1] = f[i][j + 1];
                if (s[i] == t[j]) {
                    f[i + 1][j + 1] += f[i][j];
                }
            }
        }
        return f[n][m];
    }
};
```

```go [sol-Go]
func numDistinct(s, t string) int {
    n, m := len(s), len(t)
    if n < m {
        return 0
    }

    f := make([][]int, n+1)
    for i := range f {
        f[i] = make([]int, m+1)
        f[i][0] = 1
    }
    for i, x := range s {
        for j := max(m-n+i, 0); j < min(i+1, m); j++ {
            f[i+1][j+1] = f[i][j+1]
            if byte(x) == t[j] {
                f[i+1][j+1] += f[i][j]
            }
        }
    }
    return f[n][m]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 是 $s$ 的长度，$m$ 是 $t$ 的长度。
- 空间复杂度：$\mathcal{O}(nm)$。

## 五、空间优化

观察上面的状态转移方程，在计算 $f[i+1]$ 时，只会用到 $f[i]$，不会用到比 $i$ 更早的状态。

因此可以像 [0-1 背包](https://www.bilibili.com/video/BV16Y411v7Y6/) 那样，去掉第一个维度，把 $f[i+1]$ 和 $f[i]$ 保存到**同一个数组**中。

状态转移方程改为

$$
f[j+1] =
\begin{cases}
f[j+1] + f[j], & s[i]=t[j]    \\
f[j+1], & s[i]\ne t[j]    \\
\end{cases}
$$

初始值 $f[0]=1$。

答案为 $f[m]$。

注意 $j$ 要倒序枚举，原理和 [0-1 背包](https://www.bilibili.com/video/BV16Y411v7Y6/) 一样。

```py [sol-Python3]
class Solution:
    def numDistinct(self, s: str, t: str) -> int:
        n, m = len(s), len(t)
        if n < m:
            return 0

        f = [1] + [0] * m
        for i, x in enumerate(s):
            for j in range(min(i, m - 1), max(m - n + i, 0) - 1, -1):
                if x == t[j]:
                    f[j + 1] += f[j]
        return f[m]
```

```java [sol-Java]
class Solution {
    public int numDistinct(String S, String T) {
        int n = S.length();
        int m = T.length();
        if (n < m) {
            return 0;
        }

        char[] s = S.toCharArray();
        char[] t = T.toCharArray();

        int[] f = new int[m + 1];
        f[0] = 1;
        for (int i = 0; i < n; i++) {
            for (int j = Math.min(i, m - 1); j >= Math.max(m - n + i, 0); j--) {
                if (s[i] == t[j]) {
                    f[j + 1] += f[j];
                }
            }
        }
        return f[m];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numDistinct(string s, string t) {
        int n = s.size(), m = t.size();
        if (n < m) {
            return 0;
        }

        vector<unsigned> f(m + 1);
        f[0] = 1;
        for (int i = 0; i < n; i++) {
            for (int j = min(i, m - 1); j >= max(m - n + i, 0); j--) {
                if (s[i] == t[j]) {
                    f[j + 1] += f[j];
                }
            }
        }
        return f[m];
    }
};
```

```go [sol-Go]
func numDistinct(s, t string) int {
    n, m := len(s), len(t)
    if n < m {
        return 0
    }

    f := make([]int, m+1)
    f[0] = 1
    for i, x := range s {
        for j := min(i, m-1); j >= max(m-n+i, 0); j-- {
            if byte(x) == t[j] {
                f[j+1] += f[j]
            }
        }
    }
    return f[m]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 是 $s$ 的长度，$m$ 是 $t$ 的长度。
- 空间复杂度：$\mathcal{O}(m)$。

## 思考题

改成计算在 $s$ 的**子串**中 $t$ 出现的个数，怎么做？

欢迎在评论区分享你的思路/代码。

## 相似题目

更多相似题目（双序列 DP），见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**§4.1 最长公共子序列**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. 【本题相关】[动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
