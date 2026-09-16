# 97. 交错字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/interleaving-string/
- 题目 slug：`interleaving-string`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/interleaving-string/solutions/3060419/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-qcen/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考动态规划：从记忆化搜索到递推到空间优化！（Python/Java/C++/Go）](https://leetcode.cn/problems/interleaving-string/solutions/3060419/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-qcen/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-qcen`
- topic id：`3060419`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前言

首先，$s_1$ 的长度加上 $s_2$ 的长度，必须等于 $s_3$ 的长度。

题目要求 $|n-m|\le 1$，这其实是多余的。看下图，本质上我们把 $s_3=\texttt{aadbbcbcac}$ 交替涂成了红蓝两种颜色，既然是**交替涂色**，那么无论怎么涂色，红色段数和蓝色段数之差不会超过 $1$。

![lc97.jpg](https://pic.leetcode.cn/1738291029-Ytviqc-lc97.jpg)

简单证明：初始 $n=m=0$。不失一般性，把第一段涂成红色，$n$ 加一，现在 $n=1,\ m=0$。下一段的颜色必然是蓝色（否则这两段就合并成一整段红色了），所以 $m$ 加一，现在 $n=m=1$。继续，下一段的颜色必然是红色，所以 $n$ 加一，现在 $n=2,\ m=1$。依此类推，任意时刻均满足 $|n-m|\le 1$。

## 一、寻找子问题

在示例 1 中，我们要解决的问题（原问题）是：

- $s_3=\texttt{aadbbcbcac}$ 能否由 $s_1=\texttt{aabcc}$ 和 $s_2=\texttt{dbbca}$ 交错组成？

从右往左思考，如果 $s_3$ 的最后一个字母 $\texttt{c}$ 来自 $s_1$ 的最后一个字母，那么去掉 $s_1$ 和 $s_3$ 的最后一个字母，问题变成：

- $\texttt{aadbbcbca}$ 能否由 $\texttt{aabc}$ 和 $\texttt{dbbca}$ 交错组成？

如果 $s_3$ 的最后一个字母来自 $s_2$ 的最后一个字母，那么去掉 $s_2$ 和 $s_3$ 的最后一个字母，问题变成：

- $\texttt{aadbbcbca}$ 能否由 $\texttt{aabcc}$ 和 $\texttt{dbbc}$ 交错组成？

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

> **注**：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。

## 二、状态定义与状态转移方程

根据上面的讨论，我们需要在递归过程中跟踪以下信息：

- $i$：$s_1$ 的最后一个字母的下标。换句话说，$s_1$ 的剩余长度为 $i+1$。
- $j$：$s_2$ 的最后一个字母的下标。换句话说，$s_2$ 的剩余长度为 $j+1$。

知道了 $i$ 和 $j$，就知道了 $s_3$ 此时的剩余长度 $(i+1)+(j+1)=i+j+2$，所以 $s_3$ 最后一个字母的下标为 $i+j+1$。

定义状态为 $\textit{dfs}(i,j)$，表示 $s_3[:i+j+2]$ 能否由 $s_1[:i+1]$ 和 $s_2[:j+1]$ 交错组成。其中记号 $s[:k]$ 表示 $s$ 的长为 $k$ 的前缀，即 $s[0]$ 到 $s[k-1]$。

接下来，思考如何从一个状态转移到另一个状态。

考虑 $s_3[i+j+1]$ 来自哪：

- 如果 $s_3[i+j+1]$ 来自 $s_1[i]$，去掉这两个字母，接下来要解决的问题是：$s_3[:i+j+1]$ 能否由 $s_1[:i]$ 和 $s_2[:j+1]$ 交错组成，即 $\textit{dfs}(i-1,j)$。
- 如果 $s_3[i+j+1]$ 来自 $s_2[j]$，去掉这两个字母，接下来要解决的问题是：$s_3[:i+j+1]$ 能否由 $s_1[:i+1]$ 和 $s_2[:j]$ 交错组成，即 $\textit{dfs}(i,j-1)$。

这两种情况只要有一个成立，那么 $\textit{dfs}(i,j)$ 就是 $\texttt{true}$，即

$$
\textit{dfs}(i,j) = \vee
\begin{cases}
i\ge 0 \wedge s_1[i] = s_3[i+j+1] \wedge \textit{dfs}(i-1,j)    \\
j\ge 0 \wedge s_2[j] = s_3[i+j+1] \wedge \textit{dfs}(i,j-1)    \\
\end{cases}
$$

**递归边界**：$\textit{dfs}(-1,-1)=\texttt{true}$。所有字母匹配完毕。

**递归入口**：$\textit{dfs}(n-1,m-1)$，这是原问题，也是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。    

本题 $i$ 和 $j$ 可以是 $-1$，为避免下标越界，可以把 $\textit{dfs}(i,j)$ 记录到 $\textit{memo}[i+1][j+1]$ 中。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def isInterleave(self, s1: str, s2: str, s3: str) -> bool:
        n, m = len(s1), len(s2)
        if n + m != len(s3):
            return False

        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（一行代码实现记忆化）
        def dfs(i: int, j: int) -> bool:
            if i < 0 and j < 0:
                return True
            return i >= 0 and s1[i] == s3[i + j + 1] and dfs(i - 1, j) or \
                   j >= 0 and s2[j] == s3[i + j + 1] and dfs(i, j - 1)

        return dfs(n - 1, m - 1)
```

```java [sol-Java]
class Solution {
    public boolean isInterleave(String s1, String s2, String s3) {
        int n = s1.length();
        int m = s2.length();
        if (n + m != s3.length()) {
            return false;
        }

        int[][] memo = new int[n + 1][m + 1];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return dfs(n - 1, m - 1, s1.toCharArray(), s2.toCharArray(), s3.toCharArray(), memo);
    }

    private boolean dfs(int i, int j, char[] s1, char[] s2, char[] s3, int[][] memo) {
        if (i < 0 && j < 0) {
            return true;
        }
        if (memo[i + 1][j + 1] != -1) { // 之前计算过
            return memo[i + 1][j + 1] == 1;
        }
        boolean res = i >= 0 && s1[i] == s3[i + j + 1] && dfs(i - 1, j, s1, s2, s3, memo) ||
                      j >= 0 && s2[j] == s3[i + j + 1] && dfs(i, j - 1, s1, s2, s3, memo);
        memo[i + 1][j + 1] = res ? 1 : 0; // 记忆化
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isInterleave(string s1, string s2, string s3) {
        int n = s1.size(), m = s2.size();
        if (n + m != s3.size()) {
            return false;
        }

        vector memo(n + 1, vector<int>(m + 1, -1)); // -1 表示没有计算过
        auto dfs = [&](this auto&& dfs, int i, int j) -> bool {
            if (i < 0 && j < 0) {
                return true;
            }
            int& res = memo[i + 1][j + 1]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            return res = i >= 0 && s1[i] == s3[i + j + 1] && dfs(i - 1, j) ||
                         j >= 0 && s2[j] == s3[i + j + 1] && dfs(i, j - 1);
        };
        return dfs(n - 1, m - 1);
    }
};
```

```go [sol-Go]
func isInterleave(s1, s2, s3 string) bool {
    n, m := len(s1), len(s2)
    if n+m != len(s3) {
        return false
    }

    memo := make([][]int, n+1)
    for i := range memo {
        memo[i] = make([]int, m+1)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }
    var dfs func(int, int) bool
    dfs = func(i, j int) bool {
        if i < 0 && j < 0 {
            return true
        }
        p := &memo[i+1][j+1]
        if *p < 0 { // 没有计算过
            if i >= 0 && s1[i] == s3[i+j+1] && dfs(i-1, j) ||
               j >= 0 && s2[j] == s3[i+j+1] && dfs(i, j-1) {
                *p = 1
            } else {
                *p = 0
            }
        }
        return *p == 1
    }
    return dfs(n-1, m-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 是 $s_1$ 的长度，$m$ 是 $s_2$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(nm)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(nm)$。
- 空间复杂度：$\mathcal{O}(nm)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i+1][j+1]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示 $s_3[:i+j+2]$ 能否由 $s_1[:i+1]$ 和 $s_2[:j+1]$ 交错组成。这里 $+1$ 是为了把 $\textit{dfs}(-1,-1)$ 这个状态也翻译过来，这样我们可以把 $f[0][0]$ 作为初始值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i+1][j+1] = \vee
\begin{cases}
s_1[i] = s_3[i+j+1] \wedge f[i][j+1]    \\
s_2[j] = s_3[i+j+1] \wedge f[i+1][j]    \\
\end{cases}
$$

这里我们单独计算 $f[0][j]$ 和 $f[i][0]$。

初始值 $f[0][0]=\texttt{true}$，翻译自递归边界 $\textit{dfs}(-1,-1)=\texttt{true}$。

答案为 $f[n][m]$，翻译自递归入口 $\textit{dfs}(n-1,m-1)$。

```py [sol-Python3]
class Solution:
    def isInterleave(self, s1: str, s2: str, s3: str) -> bool:
        n, m = len(s1), len(s2)
        if n + m != len(s3):
            return False

        f = [[False] * (m + 1) for _ in range(n + 1)]
        f[0][0] = True
        for j, y in enumerate(s2):
            f[0][j + 1] = y == s3[j] and f[0][j]
        for i, x in enumerate(s1):
            f[i + 1][0] = x == s3[i] and f[i][0]
            for j, y in enumerate(s2):
                f[i + 1][j + 1] = x == s3[i + j + 1] and f[i][j + 1] or \
                                  y == s3[i + j + 1] and f[i + 1][j]
        return f[n][m]
```

```java [sol-Java]
class Solution {
    public boolean isInterleave(String S1, String S2, String S3) {
        int n = S1.length();
        int m = S2.length();
        if (n + m != S3.length()) {
            return false;
        }

        char[] s1 = S1.toCharArray();
        char[] s2 = S2.toCharArray();
        char[] s3 = S3.toCharArray();

        boolean[][] f = new boolean[n + 1][m + 1];
        f[0][0] = true;
        for (int j = 0; j < m; j++) {
            f[0][j + 1] = s2[j] == s3[j] && f[0][j];
        }
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = s1[i] == s3[i] && f[i][0];
            for (int j = 0; j < m; j++) {
                f[i + 1][j + 1] = s1[i] == s3[i + j + 1] && f[i][j + 1] ||
                                  s2[j] == s3[i + j + 1] && f[i + 1][j];
            }
        }
        return f[n][m];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isInterleave(string s1, string s2, string s3) {
        int n = s1.size(), m = s2.size();
        if (n + m != s3.size()) {
            return false;
        }

        vector f(n + 1, vector<int>(m + 1));
        f[0][0] = true;
        for (int j = 0; j < m; j++) {
            f[0][j + 1] = s2[j] == s3[j] && f[0][j];
        }
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = s1[i] == s3[i] && f[i][0];
            for (int j = 0; j < m; j++) {
                f[i + 1][j + 1] = s1[i] == s3[i + j + 1] && f[i][j + 1] ||
                                  s2[j] == s3[i + j + 1] && f[i + 1][j];
            }
        }
        return f[n][m];
    }
};
```

```go [sol-Go]
func isInterleave(s1, s2, s3 string) bool {
    n, m := len(s1), len(s2)
    if n+m != len(s3) {
        return false
    }

    f := make([][]bool, n+1)
    for i := range f {
        f[i] = make([]bool, m+1)
    }
    f[0][0] = true
    for j := range m {
        f[0][j+1] = s2[j] == s3[j] && f[0][j]
    }
    for i := range n {
        f[i+1][0] = s1[i] == s3[i] && f[i][0]
        for j := range m {
            f[i+1][j+1] = s1[i] == s3[i+j+1] && f[i][j+1] ||
                          s2[j] == s3[i+j+1] && f[i+1][j]
        }
    }
    return f[n][m]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 是 $s_1$ 的长度，$m$ 是 $s_2$ 的长度。
- 空间复杂度：$\mathcal{O}(nm)$。

## 五、空间优化

举个例子，在计算 $f[1][1]$ 时，会用到 $f[0][1]$，但是之后就不再用到了。那么干脆把 $f[1][1]$ 记到 $f[0][1]$ 中，这样对于 $f[1][2]$ 来说，它需要的数据就在 $f[0][1]$ 和 $f[0][2]$ 中。$f[1][2]$ 算完后也可以同样记到 $f[0][2]$ 中。

所以只需要一个长为 $m+1$ 的一维数组就够了。

具体可以看[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)中的讲解。本题的转移方程类似完全背包，故采用正序遍历。

```py [sol-Python3]
class Solution:
    def isInterleave(self, s1: str, s2: str, s3: str) -> bool:
        n, m = len(s1), len(s2)
        if n + m != len(s3):
            return False

        f = [False] * (m + 1)
        f[0] = True
        for j, y in enumerate(s2):
            f[j + 1] = f[j] and y == s3[j]
        for i, x in enumerate(s1):
            f[0] = f[0] and x == s3[i]
            for j, y in enumerate(s2):
                f[j + 1] = f[j + 1] and x == s3[i + j + 1] or \
                           f[j] and y == s3[i + j + 1]
        return f[m]
```

```java [sol-Java]
class Solution {
    public boolean isInterleave(String S1, String S2, String S3) {
        int n = S1.length();
        int m = S2.length();
        if (n + m != S3.length()) {
            return false;
        }

        char[] s1 = S1.toCharArray();
        char[] s2 = S2.toCharArray();
        char[] s3 = S3.toCharArray();

        boolean[] f = new boolean[m + 1];
        f[0] = true;
        for (int j = 0; j < m; j++) {
            f[j + 1] = f[j] && s2[j] == s3[j];
        }
        for (int i = 0; i < n; i++) {
            f[0] = f[0] && s1[i] == s3[i];
            for (int j = 0; j < m; j++) {
                f[j + 1] = f[j + 1] && s1[i] == s3[i + j + 1] ||
                           f[j] && s2[j] == s3[i + j + 1];
            }
        }
        return f[m];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isInterleave(string s1, string s2, string s3) {
        int n = s1.size(), m = s2.size();
        if (n + m != s3.size()) {
            return false;
        }

        vector<int> f(m + 1);
        f[0] = true;
        for (int j = 0; j < m; j++) {
            f[j + 1] = f[j] && s2[j] == s3[j];
        }
        for (int i = 0; i < n; i++) {
            f[0] = f[0] && s1[i] == s3[i];
            for (int j = 0; j < m; j++) {
                f[j + 1] = f[j + 1] && s1[i] == s3[i + j + 1] ||
                           f[j] && s2[j] == s3[i + j + 1];
            }
        }
        return f[m];
    }
};
```

```go [sol-Go]
func isInterleave(s1, s2, s3 string) bool {
    n, m := len(s1), len(s2)
    if n+m != len(s3) {
        return false
    }

    f := make([]bool, m+1)
    f[0] = true
    for j := range m {
        f[j+1] = f[j] && s2[j] == s3[j]
    }
    for i := range n {
        f[0] = f[0] && s1[i] == s3[i]
        for j := range m {
            f[j+1] = f[j+1] && s1[i] == s3[i+j+1] ||
                     f[j] && s2[j] == s3[i+j+1]
        }
    }
    return f[m]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 是 $s_1$ 的长度，$m$ 是 $s_2$ 的长度。
- 空间复杂度：$\mathcal{O}(m)$。

## 专题训练

见下面动态规划题单的「**§4.1 最长公共子序列**」。

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
