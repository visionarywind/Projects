# 516. 最长回文子序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-palindromic-subsequence/
- 题目 slug：`longest-palindromic-subsequence`
- 来源专题：动态规划
- 来源分类路径：八、区间 DP / §8.1 最长回文子序列
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/longest-palindromic-subsequence/solutions/2203001/shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-kgkg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】教你一步步思考动态规划：从记忆化搜索到递推到空间优化！（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-palindromic-subsequence/solutions/2203001/shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-kgkg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-kgkg`
- topic id：`2203001`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

请看 [区间 DP【基础算法精讲 22】](https://www.bilibili.com/video/BV1Gs4y1E7EU/)，制作不易，欢迎点赞！

## 一、记忆化搜索

```py [sol-Python3]
class Solution:
    def longestPalindromeSubseq(self, s: str) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int, j: int) -> int:
            if i > j:
                return 0  # 空串
            if i == j:
                return 1  # 只有一个字母
            if s[i] == s[j]:
                return dfs(i + 1, j - 1) + 2  # 都选
            return max(dfs(i + 1, j), dfs(i, j - 1))  # 枚举哪个不选
        return dfs(0, len(s) - 1)
```

```java [sol-Java]
class Solution {
    public int longestPalindromeSubseq(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        int[][] memo = new int[n][n];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示还没有计算过
        }
        return dfs(0, n - 1, s, memo);
    }

    private int dfs(int i, int j, char[] s, int[][] memo) {
        if (i > j) {
            return 0; // 空串
        }
        if (i == j) {
            return 1; // 只有一个字母
        }
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }
        if (s[i] == s[j]) {
            return memo[i][j] = dfs(i + 1, j - 1, s, memo) + 2; // 都选
        }
        return memo[i][j] = Math.max(dfs(i + 1, j, s, memo), dfs(i, j - 1, s, memo)); // 枚举哪个不选
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestPalindromeSubseq(string s) {
        int n = s.length();
        vector memo(n, vector<int>(n, -1)); // -1 表示没有计算过        
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i > j) {
                return 0; // 空串
            }
            if (i == j) {
                return 1; // 只有一个字母
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            if (s[i] == s[j]) {
                return res = dfs(i + 1, j - 1) + 2; // 都选
            }
            return res = max(dfs(i + 1, j), dfs(i, j - 1)); // 枚举哪个不选
        };
        return dfs(0, n - 1);
    }
};
```

```go [sol-Go]
func longestPalindromeSubseq(s string) int {
    n := len(s)
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, n)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示还没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) (res int) {
        if i > j { // 空串
            return
        }
        if i == j { // 只有一个字母
            return 1
        }
        p := &memo[i][j]
        if *p != -1 { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if s[i] == s[j] {
            return dfs(i+1, j-1) + 2 // 都选
        }
        return max(dfs(i+1, j), dfs(i, j-1)) // 枚举哪个不选
    }
    return dfs(0, n-1)
}
```

### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $s$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的转移个数。本题中状态个数等于 $\mathcal{O}(n^2)$，而单个状态的转移个数为 $\mathcal{O}(1)$，因此时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。保存多少状态，就需要多少空间。

## 二、1:1 翻译成递推

根据视频中讲的，把 $\textit{dfs}$ 改成 $f$ 数组，把递归改成循环就好了。

#### 答疑

**问**：为什么 $j$ 是从 $i+1$ 开始循环的？

**答**：在记忆化搜索中，$i \ge j$ 是递归边界，直接返回。什么情况下会继续递归呢？就是 $i < j$，所以 1:1 翻译成递推的时候也同样要满足 $i < j$，所以 $j$ 要从 $i+1$ 开始。

**问**：如何思考循环顺序？什么时候要正序，什么时候要倒序？

**答**：这里有一个通用的做法：盯着状态转移方程，想一想，要计算 $f[i][j]$，必须先把 $f[i+1][\cdot]$ 算出来，那么只有 $i$ **从大到小**枚举才能做到。

对于 $j$ 来说，由于在计算 $f[i][j]$ 的时候，需要用到 $f[i][j-1]$，也就是必须先把 $f[i][j-1]$ 算出来，所以 $j$ 必须**从小到大**枚举。

```py [sol-Python3]
class Solution:
    def longestPalindromeSubseq(self, s: str) -> int:
        n = len(s)
        f = [[0] * n for _ in range(n)]
        for i in range(n - 1, -1, -1):
            f[i][i] = 1
            for j in range(i + 1, n):
                if s[i] == s[j]:
                    f[i][j] = f[i + 1][j - 1] + 2
                else:
                    f[i][j] = max(f[i + 1][j], f[i][j - 1])
        return f[0][-1]
```

```java [sol-Java]
class Solution {
    public int longestPalindromeSubseq(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        int[][] f = new int[n][n];
        for (int i = n - 1; i >= 0; i--) {
            f[i][i] = 1;
            for (int j = i + 1; j < n; j++) {
                f[i][j] = s[i] == s[j] ? f[i + 1][j - 1] + 2 :
                        Math.max(f[i + 1][j], f[i][j - 1]);
            }
        }
        return f[0][n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestPalindromeSubseq(string s) {
        int n = s.length();
        vector f(n, vector<int>(n));
        for (int i = n - 1; i >= 0; i--) {
            f[i][i] = 1;
            for (int j = i + 1; j < n; j++) {
                f[i][j] = s[i] == s[j] ? f[i + 1][j - 1] + 2 :
                          max(f[i + 1][j], f[i][j - 1]);
            }
        }
        return f[0][n - 1];
    }
};
```

```go [sol-Go]
func longestPalindromeSubseq(s string) int {
    n := len(s)
    f := make([][]int, n)
    for i := range f {
        f[i] = make([]int, n)
    }
    for i := n - 1; i >= 0; i-- {
        f[i][i] = 1
        for j := i + 1; j < n; j++ {
            if s[i] == s[j] {
                f[i][j] = f[i+1][j-1] + 2
            } else {
                f[i][j] = max(f[i+1][j], f[i][j-1])
            }
        }
    }
    return f[0][n-1]
}
```

### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $s$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的转移个数。本题中状态个数等于 $\mathcal{O}(n^2)$，而单个状态的转移个数为 $\mathcal{O}(1)$，因此时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 三、空间优化

把 $f$ 数组的第一个维度去掉。相当于把 $f[i]$ 和 $f[i+1]$ 保存到**同一个一维数组**中。

但一个萝卜一个坑，$f[j-1]$ 要么保存的是 $f[i+1][j-1]$，要么保存的是 $f[i][j-1]$，怎么妥当地处理新旧数据？对于本题来说，可以用变量 $\textit{pre}$ 记录 $f[i+1][j-1]$ 的值。计算到 $f[j]$ 时，$f[j-1]$ 保存的是新数据 $f[i][j-1]$，旧数据 $f[i+1][j-1]$ 可以从 $\textit{pre}$ 中取到。

```py [sol-Python3]
class Solution:
    def longestPalindromeSubseq(self, s: str) -> int:
        n = len(s)
        f = [0] * n
        for i in range(n - 1, -1, -1):
            f[i] = 1
            pre = 0  # 初始值为 f[i+1][i]
            for j in range(i + 1, n):
                tmp = f[j]
                f[j] = pre + 2 if s[i] == s[j] else max(f[j], f[j - 1])
                pre = tmp
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int longestPalindromeSubseq(String S) {
        char[] s = S.toCharArray();
        int n = s.length;
        int[] f = new int[n];
        for (int i = n - 1; i >= 0; i--) {
            f[i] = 1;
            int pre = 0; // 初始值为 f[i+1][i]
            for (int j = i + 1; j < n; j++) {
                int tmp = f[j];
                f[j] = s[i] == s[j] ? pre + 2 : Math.max(f[j], f[j - 1]);
                pre = tmp;
            }
        }
        return f[n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestPalindromeSubseq(string s) {
        int n = s.length();
        vector<int> f(n);
        for (int i = n - 1; i >= 0; i--) {
            f[i] = 1;
            int pre = 0; // 初始值为 f[i+1][i]
            for (int j = i + 1; j < n; j++) {
                int tmp = f[j];
                f[j] = s[i] == s[j] ? pre + 2 : max(f[j], f[j - 1]);
                pre = tmp;
            }
        }
        return f[n - 1];
    }
};
```

```go [sol-Go]
func longestPalindromeSubseq(s string) int {
    n := len(s)
    f := make([]int, n)
    for i := n - 1; i >= 0; i-- {
        f[i] = 1
        pre := 0 // 初始值为 f[i+1][i]
        for j := i + 1; j < n; j++ {
            if s[i] == s[j] {
                pre, f[j] = f[j], pre+2
            } else {
                pre = f[j]
                f[j] = max(f[j], f[j-1])
            }
        }
    }
    return f[n-1]
}
```

### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

### 课后作业

- [375. 猜数字大小 II](https://leetcode.cn/problems/guess-number-higher-or-lower-ii/)
- [1312. 让字符串成为回文串的最少插入次数](https://leetcode.cn/problems/minimum-insertion-steps-to-make-a-string-palindrome/)
- [1547. 切棍子的最小成本](https://leetcode.cn/problems/minimum-cost-to-cut-a-stick/)
- [1000. 合并石头的最低成本](https://leetcode.cn/problems/minimum-cost-to-merge-stones/)

更多相似题目，见下面 DP 题单中的「**区间 DP**」。

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

本题来自 `八、区间 DP / §8.1 最长回文子序列`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、区间 DP / §8.1 最长回文子序列`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
