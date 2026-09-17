# 132. 分割回文串 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/palindrome-partitioning-ii/
- 题目 slug：`palindrome-partitioning-ii`
- 来源专题：动态规划
- 来源分类路径：五、划分型 DP / §5.2 最优划分
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/palindrome-partitioning-ii/solutions/3588633/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-bnlb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推（Python/Java/C++/Go）](https://leetcode.cn/problems/palindrome-partitioning-ii/solutions/3588633/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-bnlb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-bnlb`
- topic id：`3588633`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 一、寻找子问题

### 1)

例如 $s=\texttt{aabb}$，我们要解决的问题（原问题）是：

- 把 $s=\texttt{aabb}$ 分割成一些子串，使每个子串都是回文串的最少分割次数。

枚举分割出的最右边那段子串的长度（或者左端点）：

- 分割出子串 $\texttt{b}$，这是回文串，那么需要解决的子问题为：把 $\texttt{aab}$ 分割成一些子串，使每个子串都是回文串的最少分割次数。
- 分割出子串 $\texttt{bb}$，这是回文串，那么需要解决的子问题为：把 $\texttt{aa}$ 分割成一些子串，使每个子串都是回文串的最少分割次数。
- 分割出子串 $\texttt{abb}$，这不是回文串。
- 分割出子串 $\texttt{aabb}$，这不是回文串。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

> 注：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。

### 2)

如何判断子串 $s[l]$ 到 $s[r]$ 是否为回文串？

分类讨论：

- 如果 $s[l]\ne s[r]$，那么子串肯定不是回文串。
- 如果 $s[l]= s[r]$，那么问题变成：$s[l+1]$ 到 $s[r-1]$ 是否为回文串？这是个和原问题相似的、规模更小的子问题，也可以用递归解决。

## 二、状态定义与状态转移方程

### 1)

由于每个子问题计算的都是 $s$ 的**前缀** $s[0]$ 到 $s[r]$，定义 $\textit{dfs}(r)$ 表示把前缀 $s[0]$ 到 $s[r]$ 分割成一些子串，使每个子串都是回文串的最少分割次数。

枚举分割出的最右边那段子串的左端点 $l$：

- 如果 $s[l]$ 到 $s[r]$ 是回文串，那么在 $l-1$ 和 $l$ 之间切一刀，接下来需要解决的子问题为：把前缀 $s[0]$ 到 $s[l-1]$ 分割成一些子串，使每个子串都是回文串的最少分割次数，即 $\textit{dfs}(l-1)$。

所有情况取最小值，即

$$
\textit{dfs}(r) = \min_{l=1}^{r} \textit{dfs}(l-1) + 1
$$

其中 $+1$ 表示在 $l-1$ 和 $l$ 之间切一刀，这算作 $1$ 次分割次数。

**递归边界**：如果 $s[0]$ 到 $s[r]$ 是回文串，那么无需分割，直接返回 $0$。

**递归入口**：$\textit{dfs}(n-1)$，这是原问题，也是答案。

### 2)

定义 $\textit{isPalindrome}(l,r)$ 表示子串 $s[l]$ 到 $s[r]$ 是否为回文串。

分类讨论：

- 如果 $s[l]\ne s[r]$，那么子串肯定不是回文串，返回 $\texttt{false}$。
- 如果 $s[l]= s[r]$，那么问题变成 $s[l+1]$ 到 $s[r-1]$ 是否为回文串，即 $\textit{isPalindrome}(l+1,r-1)$。

**递归边界**：$\textit{isPalindrome}(l,l) = \textit{isPalindrome}(l,l-1) = \texttt{true}$。长为 $1$ 的字符串是回文串，空串也是回文串。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def minCut(self, s: str) -> int:
        # 返回 s[l:r+1] 是否为回文串
        @cache  # 缓存装饰器，避免重复计算 is_palindrome（一行代码实现记忆化）
        def is_palindrome(l: int, r: int) -> bool:
            if l >= r:
                return True
            return s[l] == s[r] and is_palindrome(l + 1, r - 1)

        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(r: int) -> int:
            if is_palindrome(0, r):  # 已是回文串，无需分割
                return 0
            res = inf
            for l in range(1, r + 1):  # 枚举分割位置
                if is_palindrome(l, r):
                    res = min(res, dfs(l - 1) + 1)  # 在 l-1 和 l 之间切一刀
            return res

        return dfs(len(s) - 1)
```

```java [sol-Java]
class Solution {
    public int minCut(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        int[][] palMemo = new int[n][n];
        for (int[] row : palMemo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        int[] dfsMemo = new int[n];
        Arrays.fill(dfsMemo, -1); // -1 表示没有计算过
        return dfs(n - 1, s, palMemo, dfsMemo);
    }

    private int dfs(int r, char[] s, int[][] palMemo, int[] dfsMemo) {
        if (isPalindrome(0, r, s, palMemo)) { // 已是回文串，无需分割
            return 0;
        }
        if (dfsMemo[r] != -1) { // 之前计算过
            return dfsMemo[r];
        }
        int res = Integer.MAX_VALUE;
        for (int l = 1; l <= r; l++) { // 枚举分割位置
            if (isPalindrome(l, r, s, palMemo)) {
                res = Math.min(res, dfs(l - 1, s, palMemo, dfsMemo) + 1); // 在 l-1 和 l 之间切一刀
            }
        }
        return dfsMemo[r] = res; // 记忆化
    }

    private boolean isPalindrome(int l, int r, char[] s, int[][] palMemo) {
        if (l >= r) {
            return true;
        }
        if (palMemo[l][r] != -1) { // 之前计算过
            return palMemo[l][r] == 1;
        }
        boolean res = s[l] == s[r] && isPalindrome(l + 1, r - 1, s, palMemo);
        palMemo[l][r] = res ? 1 : 0; // 记忆化
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minCut(string s) {
        int n = s.size();
        vector pal_memo(n, vector<int>(n, -1)); // -1 表示没有计算过
        auto is_palindrome = [&](this auto&& is_palindrome, int l, int r) -> bool {
            if (l >= r) {
                return true;
            }
            int& res = pal_memo[l][r]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            return res = s[l] == s[r] && is_palindrome(l + 1, r - 1);
        };

        vector<int> dfs_memo(n, INT_MAX); // INT_MAX 表示没有计算过
        auto dfs = [&](this auto&& dfs, int r) -> int {
            if (is_palindrome(0, r)) { // 已是回文串，无需分割
                return 0;
            }
            int& res = dfs_memo[r]; // 注意这里是引用
            if (res != INT_MAX) { // 之前计算过
                return res;
            }
            for (int l = 1; l <= r; l++) { // 枚举分割位置
                if (is_palindrome(l, r)) {
                    res = min(res, dfs(l - 1) + 1); // 在 l-1 和 l 之间切一刀
                }
            }
            return res;
        };
        return dfs(n - 1);
    }
};
```

```go [sol-Go]
func minCut(s string) int {
    n := len(s)
    palMemo := make([][]int8, n)
    for i := range palMemo {
        palMemo[i] = make([]int8, n)
        for j := range palMemo[i] {
            palMemo[i][j] = -1 // -1 表示没有计算过
        }
    }

    // 判断 s[l:r+1] 是否是回文串
    var isPalindrome func(int, int) bool
    isPalindrome = func(l, r int) bool {
        if l >= r {
            return true
        }
        p := &palMemo[l][r]
        if *p != -1 { // 之前计算过
            return *p == 1
        }
        res := s[l] == s[r] && isPalindrome(l+1, r-1)
        if res {
            *p = 1 // 记忆化
        } else {
            *p = 0
        }
        return res
    }

    dfsMemo := make([]int, n)
    for i := range dfsMemo {
        dfsMemo[i] = -1 // -1 表示没有计算过
    }
    var dfs func(int) int
    dfs = func(r int) int {
        if isPalindrome(0, r) { // 已是回文串，无需分割
            return 0
        }
        p := &dfsMemo[r]
        if *p != -1 { // 之前计算过
            return *p
        }
        res := math.MaxInt
        for l := 1; l <= r; l++ { // 枚举分割位置
            if isPalindrome(l, r) {
                res = min(res, dfs(l-1)+1) // 在 l-1 和 l 之间切一刀
            }
        }
        *p = res // 记忆化
        return res
    }
    return dfs(n - 1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $s$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题 $\textit{dfs}$ 的状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(n)$，所以时间复杂度为 $\mathcal{O}(n^2)$；$\textit{isPalindrome}$ 的状态个数等于 $\mathcal{O}(n^2)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以时间复杂度也为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

### 1)

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[r]$ 的定义和 $\textit{dfs}(r)$ 的定义是完全一样的，都表示把前缀 $s[0]$ 到 $s[r]$ 分割成一些子串，使每个子串都是回文串的最少分割次数。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[r] = \min_{l=1}^{r} f[l-1] + 1
$$

**初始值**：如果 $s[0]$ 到 $s[r]$ 是回文串，那么 $f[r]=0$，翻译自递归边界。

**答案**：$f[n-1]$，翻译自递归入口 $\textit{dfs}(n-1)$。

### 2)

$\textit{isPalindrome}[l][r]$ 的定义和 $\textit{isPalindrome}(l,r)$ 的定义是完全一样的，都表示子串 $s[l]$ 到 $s[r]$ 是否为回文串。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

- 如果 $s[l]\ne s[r]$，那么 $\textit{isPalindrome}[l][r] = \texttt{false}$。
- 如果 $s[l]= s[r]$，那么 $\textit{isPalindrome}[l][r] = \textit{isPalindrome}[l+1][r-1]$。

初始值 $\textit{isPalindrome}[l][l]=\textit{isPalindrome}[l][l-1]=\texttt{true}$，翻译自递归边界 $\textit{isPalindrome}(l,l) = \textit{isPalindrome}(l,l-1) = \texttt{true}$。

#### 答疑

**问**：如何思考循环顺序？什么时候要正序枚举，什么时候要倒序枚举？

**答**：这里有一个通用的做法：盯着状态转移方程，想一想，要计算 $\textit{isPalindrome}[i][j]$，必须先把 $\textit{isPalindrome}[i+1][j-1]$ 算出来，那么只有 $i$ 从大到小枚举才能做到。

对于 $j$ 来说，由于在计算 $\textit{isPalindrome}[i][j]$ 时，$\textit{isPalindrome}[i+1]$ 已经全部计算完毕，所以 $j$ 无论是正序还是倒序枚举都可以。

```py [sol-Python3]
class Solution:
    def minCut(self, s: str) -> int:
        n = len(s)
        # is_palindrome[l][r] 表示 s[l:r+1] 是否为回文串
        is_palindrome = [[True] * n for _ in range(n)]
        for l in range(n - 2, -1, -1):
            for r in range(l + 1, n):
                is_palindrome[l][r] = s[l] == s[r] and is_palindrome[l + 1][r - 1]

        f = [0] * n
        for r, is_pal in enumerate(is_palindrome[0]):
            if is_pal:  # 已是回文串，无需分割
                continue
            res = inf
            for l in range(1, r + 1):  # 枚举分割位置
                if is_palindrome[l][r]:
                    res = min(res, f[l - 1] + 1)  # 在 l-1 和 l 之间切一刀
            f[r] = res
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int minCut(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        // isPalindrome[l][r] 表示 s[l] 到 s[r] 是否为回文串
        boolean[][] isPalindrome = new boolean[n][n];
        for (boolean[] row : isPalindrome) {
            Arrays.fill(row, true);
        }
        for (int l = n - 2; l >= 0; l--) {
            for (int r = l + 1; r < n; r++) {
                isPalindrome[l][r] = s[l] == s[r] && isPalindrome[l + 1][r - 1];
            }
        }

        int[] f = new int[n];
        for (int r = 0; r < n; r++) {
            if (isPalindrome[0][r]) { // 已是回文串，无需分割
                continue;
            }
            int res = Integer.MAX_VALUE;
            for (int l = 1; l <= r; l++) { // 枚举分割位置
                if (isPalindrome[l][r]) {
                    res = Math.min(res, f[l - 1] + 1); // 在 l-1 和 l 之间切一刀
                }
            }
            f[r] = res;
        }
        return f[n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minCut(string s) {
        int n = s.size();
        // is_palindrome[l][r] 表示 s[l] 到 s[r] 是否为回文串
        vector is_palindrome(n, vector<int8_t>(n, true));
        for (int l = n - 2; l >= 0; l--) {
            for (int r = l + 1; r < n; r++) {
                is_palindrome[l][r] = s[l] == s[r] && is_palindrome[l + 1][r - 1];
            }
        }

        vector<int> f(n);
        for (int r = 0; r < n; r++) {
            if (is_palindrome[0][r]) { // 已是回文串，无需分割
                continue;
            }
            int res = INT_MAX;
            for (int l = 1; l <= r; l++) { // 枚举分割位置
                if (is_palindrome[l][r]) {
                    res = min(res, f[l - 1] + 1); // 在 l-1 和 l 之间切一刀
                }
            }
            f[r] = res;
        }
        return f[n - 1];
    }
};
```

```go [sol-Go]
func minCut(s string) int {
    n := len(s)
    // isPalindrome[l][r] 表示 s[l:r+1] 是否为回文串
    isPalindrome := make([][]bool, n)
    for i := range isPalindrome {
        isPalindrome[i] = make([]bool, n)
        for j := range isPalindrome[i] {
            isPalindrome[i][j] = true
        }
    }
    for l := n - 2; l >= 0; l-- {
        for r := l + 1; r < n; r++ {
            isPalindrome[l][r] = s[l] == s[r] && isPalindrome[l+1][r-1]
        }
    }

    f := make([]int, n)
    for r, isPal := range isPalindrome[0] {
        if isPal { // 已是回文串，无需分割
            continue
        }
        res := math.MaxInt
        for l := 1; l <= r; l++ { // 枚举分割位置
            if isPalindrome[l][r] {
                res = min(res, f[l-1]+1) // 在 l-1 和 l 之间切一刀
            }
        }
        f[r] = res
    }
    return f[n-1]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n^2)$。

**注**：本题计算的东西叫做 **palindromic length**，有 $\mathcal{O}(n)$ [做法](https://cs.emis.de/LIPIcs/volltexte/2017/7338/pdf/LIPIcs-CPM-2017-23_.pdf)。

## 专题训练

见下面动态规划题单的「§5.2 最优划分」。

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

本题来自 `五、划分型 DP / §5.2 最优划分`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、划分型 DP / §5.2 最优划分`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
