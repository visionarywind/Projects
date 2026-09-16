# 1035. 不相交的线

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/uncrossed-lines/
- 题目 slug：`uncrossed-lines`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.1 基础
- 难度分：1806
- 外部题解来源：https://leetcode.cn/problems/uncrossed-lines/solutions/2875980/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-ahzh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推到空间优化！（Python/Java/C++/Go）](https://leetcode.cn/problems/uncrossed-lines/solutions/2875980/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-ahzh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-ahzh`
- topic id：`2875980`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、寻找子问题

为方便描述，下文把 $\textit{nums}_1$ 记作 $s$，把 $\textit{nums}_2$ 记作 $t$。

看示例 1，$s=[1,4,2],\ t=[1,2,4]$，我们要计算这两个数组的最大连线数。

用「选或不选」分类讨论：

- 不选 $s[2]=2$，那么需要解决的问题为：$s=[1,4],\ t=[1,2,4]$ 的最大连线数。
- 不选 $t[2]=4$，那么需要解决的问题为：$s=[1,4,2],\ t=[1,2]$ 的最大连线数。

除此以外，对于 $s=[1,4],\ t=[1,2,4]$，由于 $s[1]=t[2]=4$，我们可以都选，也就是在两个数字之间连线，问题变成：$s=[1],\ t=[1,2]$ 的最大连线数。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

> 注 1：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。
> 
> 注 2：动态规划有「选或不选」和「枚举选哪个」两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「选或不选」。

## 二、状态定义与状态转移方程

上面的思考过程和 [1143. 最长公共子序列](https://leetcode.cn/problems/longest-common-subsequence/) 是一模一样的，按照[【基础算法精讲 19】](https://www.bilibili.com/video/BV1TM4y1o7ug/)中的讲解，定义 $\textit{dfs}(i,j)$ 表示 $s[0]$ 到 $s[i]$ 与 $t[0]$ 到 $t[j]$ 之间的最大连线数。

状态转移方程为

$$
\textit{dfs}(i,j) =
\begin{cases} 
\textit{dfs}(i-1,j-1) + 1, & s[i]=t[j]     \\
\max(\textit{dfs}(i-1,j), \textit{dfs}(i,j-1)), & s[i]\ne t[j]     \\
\end{cases}
$$

⚠**注意**：当 $s[i]=t[j]$ 时，只需要考虑都选的情况，证明过程请看 [视频讲解](https://www.bilibili.com/video/BV1TM4y1o7ug/)。

递归边界：$\textit{dfs}(-1,j)=\textit{dfs}(i,-1)=0$。当其中一个数组为空时，连线数等于 $0$。

递归入口：$\textit{dfs}(n-1,m-1)$，也就是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def maxUncrossedLines(self, s: List[int], t: List[int]) -> int:
        n, m = len(s), len(t)
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i, j):
            if i < 0 or j < 0:
                return 0
            if s[i] == t[j]:
                return dfs(i - 1, j - 1) + 1
            return max(dfs(i - 1, j), dfs(i, j - 1))
        return dfs(n - 1, m - 1)
```

```java [sol-Java]
class Solution {
    public int maxUncrossedLines(int[] s, int[] t) {
        int n = s.length;
        int m = t.length;
        int[][] memo = new int[n][m];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return dfs(n - 1, m - 1, s, t, memo);
    }

    private int dfs(int i, int j, int[] s, int[] t, int[][] memo) {
        if (i < 0 || j < 0) {
            return 0;
        }
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }
        if (s[i] == t[j]) {
            return memo[i][j] = dfs(i - 1, j - 1, s, t, memo) + 1;
        }
        return memo[i][j] = Math.max(dfs(i - 1, j, s, t, memo), dfs(i, j - 1, s, t, memo));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxUncrossedLines(vector<int>& s, vector<int>& t) {
        int n = s.size(), m = t.size();
        vector<vector<int>> memo(n, vector<int>(m, -1)); // -1 表示没有计算过
        auto dfs = [&](auto&& dfs, int i, int j) -> int {
            if (i < 0 || j < 0) {
                return 0;
            }
            int &res = memo[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            if (s[i] == t[j]) {
                return res = dfs(dfs, i - 1, j - 1) + 1;
            }
            return res = max(dfs(dfs, i - 1, j), dfs(dfs, i, j - 1));
        };
        return dfs(dfs, n - 1, m - 1);
    }
};
```

```go [sol-Go]
func maxUncrossedLines(s, t []int) int {
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
        if i < 0 || j < 0 {
            return
        }
        p := &memo[i][j]
        if *p != -1 { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if s[i] == t[j] {
            return dfs(i-1, j-1) + 1
        }
        return max(dfs(i-1, j), dfs(i, j-1))
    }
    return dfs(n-1, m-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 是 $\textit{s}$ 的长度，$m$ 是 $\textit{t}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(nm)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(nm)$。
- 空间复杂度：$\mathcal{O}(nm)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i+1][j+1]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示 $s[0]$ 到 $s[i]$ 与 $t[0]$ 到 $t[j]$ 之间的最大连线数。注意 $f$ 数组的下标要加一，从而有状态与 $\textit{dfs}$ 中的 $i=-1$ 和 $j=-1$ 对应。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i+1][j+1] =
\begin{cases}
f[i][j] + 1, & s[i]=t[j]     \\
\max(f[i][j+1], f[i+1][j]), & s[i]\ne t[j]     \\
\end{cases}
$$

初始值 $f[0][j]=f[i][0]=0$，翻译自递归边界 $\textit{dfs}(-1,j)=\textit{dfs}(i,-1)=0$。

答案为 $f[n][m]$，翻译自递归入口 $\textit{dfs}(n-1,m-1)$。

```py [sol-Python3]
class Solution:
    def maxUncrossedLines(self, s: List[int], t: List[int]) -> int:
        n, m = len(s), len(t)
        f = [[0] * (m + 1) for _ in range(n + 1)]
        for i, x in enumerate(s):
            for j, y in enumerate(t):
                f[i + 1][j + 1] = f[i][j] + 1 if x == y else \
                                  max(f[i][j + 1], f[i + 1][j])
        return f[n][m]
```

```java [sol-Java]
class Solution {
    public int maxUncrossedLines(int[] s, int[] t) {
        int n = s.length;
        int m = t.length;
        int[][] f = new int[n + 1][m + 1];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                f[i + 1][j + 1] = s[i] == t[j] ? f[i][j] + 1 :
                                  Math.max(f[i][j + 1], f[i + 1][j]);
            }
        }
        return f[n][m];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxUncrossedLines(vector<int>& s, vector<int>& t) {
        int n = s.size(), m = t.size();
        vector<vector<int>> f(n + 1, vector<int>(m + 1));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                f[i + 1][j + 1] = s[i] == t[j] ? f[i][j] + 1 :
                                  max(f[i][j + 1], f[i + 1][j]);
            }
        }
        return f[n][m];
    }
};
```

```go [sol-Go]
func maxUncrossedLines(s, t []int) int {
    n, m := len(s), len(t)
    f := make([][]int, n+1)
    for i := range f {
        f[i] = make([]int, m+1)
    }
    for i, x := range s {
        for j, y := range t {
            if x == y {
                f[i+1][j+1] = f[i][j] + 1
            } else {
                f[i+1][j+1] = max(f[i][j+1], f[i+1][j])
            }
        }
    }
    return f[n][m]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$。其中 $n$ 是 $\textit{s}$ 的长度，$m$ 是 $\textit{t}$ 的长度。
- 空间复杂度：$\mathcal{O}(nm)$。

## 五、空间优化

观察上面的状态转移方程，在计算 $f[i+1]$ 时，只会用到 $f[i]$，不会用到比 $i$ 更早的状态。

因此可以像 [0-1 背包](https://www.bilibili.com/video/BV16Y411v7Y6/) 那样，去掉第一个维度，把 $f[i+1]$ 和 $f[i]$ 保存到**同一个数组**中。

状态转移方程改为

$$
f[j+1] =
\begin{cases}
f[j] + 1, & s[i]=t[j]     \\
\max(f[j+1], f[j]), & s[i]\ne t[j]     \\
\end{cases}
$$

当 $s[i]=t[j]$ 时，计算 $f[j+1]$ 会用到 $f[j]$（这里相当于空间优化前的 $f[i][j]$），但 $f[j]$ 已经被覆盖成 $f[i+1][j]$ 了，怎么办？用一个变量 $\textit{pre}$ 记录被覆盖前的 $f[j]$ 即可。

注意 $s[i]\ne t[j]$ 时，转移来源 $f[j]$ 是空间优化前的 $f[i+1][j]$，我们需要的正是被覆盖的 $f[j]$，所以这里是不需要 $\textit{pre}$ 的。

```py [sol-Python3]
class Solution:
    def maxUncrossedLines(self, s: List[int], t: List[int]) -> int:
        f = [0] * (len(t) + 1)
        for x in s:
            pre = 0  # f[0]
            for j, y in enumerate(t):
                tmp = f[j + 1]
                f[j + 1] = pre + 1 if x == y else max(f[j + 1], f[j])
                pre = tmp
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int maxUncrossedLines(int[] s, int[] t) {
        int m = t.length;
        int[] f = new int[m + 1];
        for (int x : s) {
            int pre = 0; // f[0]
            for (int j = 0; j < m; j++) {
                int tmp = f[j + 1];
                f[j + 1] = x == t[j] ? pre + 1 : Math.max(f[j + 1], f[j]);
                pre = tmp;
            }
        }
        return f[m];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxUncrossedLines(vector<int>& s, vector<int>& t) {
        int m = t.size();
        vector<int> f(m + 1);
        for (int x : s) {
            int pre = 0; // f[0]
            for (int j = 0; j < m; j++) {
                int tmp = f[j + 1];
                f[j + 1] = x == t[j] ? pre + 1 : max(f[j + 1], f[j]);
                pre = tmp;
            }
        }
        return f[m];
    }
};
```

```go [sol-Go]
func maxUncrossedLines(s, t []int) int {
    m := len(t)
    f := make([]int, m+1)
    for _, x := range s {
        pre := 0 // f[0]
        for j, y := range t {
            if x == y {
                f[j+1], pre = pre+1, f[j+1]
            } else {
                pre = f[j+1]
                f[j+1] = max(f[j+1], f[j])
            }
        }
    }
    return f[m]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$。其中 $n$ 是 $\textit{s}$ 的长度，$m$ 是 $\textit{t}$ 的长度。
- 空间复杂度：$\mathcal{O}(m)$。

## 相似题目

见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**§4.1 最长公共子序列（LCS）**」。

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

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
