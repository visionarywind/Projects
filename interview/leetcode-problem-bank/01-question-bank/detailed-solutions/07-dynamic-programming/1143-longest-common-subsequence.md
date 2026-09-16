# 1143. 最长公共子序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-common-subsequence/
- 题目 slug：`longest-common-subsequence`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/longest-common-subsequence/solutions/2133188/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-lbz5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考动态规划：从记忆化搜索到递推（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-common-subsequence/solutions/2133188/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-lbz5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-lbz5`
- topic id：`2133188`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 视频讲解

请看[【基础算法精讲 19】](https://www.bilibili.com/video/BV1TM4y1o7ug/)。如果这个视频对你有帮助，欢迎一键三连！

## 一、递归搜索 + 保存计算结果 = 记忆化搜索

```py [sol-Python3]
class Solution:
    def longestCommonSubsequence(self, s: str, t: str) -> int:
        n, m = len(s), len(t)

        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int, j: int) -> int:
            if i < 0 or j < 0:
                return 0
            if s[i] == t[j]:
                return dfs(i - 1, j - 1) + 1
            return max(dfs(i - 1, j), dfs(i, j - 1))

        return dfs(n - 1, m - 1)
```

```java [sol-Java]
class Solution {
    private char[] s, t;
    private int[][] memo;

    public int longestCommonSubsequence(String text1, String text2) {
        s = text1.toCharArray();
        t = text2.toCharArray();
        int n = s.length;
        int m = t.length;
        memo = new int[n][m];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return dfs(n - 1, m - 1);
    }

    private int dfs(int i, int j) {
        if (i < 0 || j < 0) {
            return 0;
        }
        if (memo[i][j] != -1) {
            return memo[i][j]; // 之前计算过
        }
        if (s[i] == t[j]) {
            return memo[i][j] = dfs(i - 1, j - 1) + 1;
        }
        return memo[i][j] = Math.max(dfs(i - 1, j), dfs(i, j - 1));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestCommonSubsequence(string s, string t) {
        int n = s.size(), m = t.size();
        vector memo(n, vector<int>(m, -1)); // -1 表示没有计算过

        // lambda 递归
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i < 0 || j < 0) {
                return 0;
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != -1) {
                return res; // 之前计算过
            }
            if (s[i] == t[j]) {
                return res = dfs(i - 1, j - 1) + 1;
            }
            return res = max(dfs(i - 1, j), dfs(i, j - 1));
        };

        return dfs(n - 1, m - 1);
    }
};
```

```go [sol-Go]
func longestCommonSubsequence(s, t string) int {
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

- 时间复杂度：$\mathcal{O}(nm)$。
- 空间复杂度：$\mathcal{O}(nm)$。

## 二、1:1 翻译成递推

```py [sol-Python3]
class Solution:
    def longestCommonSubsequence(self, s: str, t: str) -> int:
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
    public int longestCommonSubsequence(String text1, String text2) {
        char[] s = text1.toCharArray();
        char[] t = text2.toCharArray();
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
    int longestCommonSubsequence(string s, string t) {
        int n = s.size(), m = t.size();
        vector f(n + 1, vector<int>(m + 1));
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
func longestCommonSubsequence(s, t string) int {
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

- 时间复杂度：$\mathcal{O}(nm)$。
- 空间复杂度：$\mathcal{O}(nm)$。

## 三、空间优化：两个数组（滚动数组）

```py [sol-Python3]
class Solution:
    def longestCommonSubsequence(self, s: str, t: str) -> int:
        n, m = len(s), len(t)
        f = [[0] * (m + 1) for _ in range(2)]
        for i, x in enumerate(s):
            for j, y in enumerate(t):
                f[(i + 1) % 2][j + 1] = f[i % 2][j] + 1 if x == y else \
                                        max(f[i % 2][j + 1], f[(i + 1) % 2][j])
        return f[n % 2][m]
```

```java [sol-Java]
class Solution {
    public int longestCommonSubsequence(String text1, String text2) {
        char[] s = text1.toCharArray();
        char[] t = text2.toCharArray();
        int n = s.length;
        int m = t.length;
        int[][] f = new int[2][m + 1];
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                f[(i + 1) % 2][j + 1] = s[i] == t[j] ? f[i % 2][j] + 1 :
                                        Math.max(f[i % 2][j + 1], f[(i + 1) % 2][j]);
            }
        }
        return f[n % 2][m];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestCommonSubsequence(string s, string t) {
        int n = s.size(), m = t.size();
        vector f(2, vector<int>(m + 1));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                f[(i + 1) % 2][j + 1] = s[i] == t[j] ? f[i % 2][j] + 1 :
                                        max(f[i % 2][j + 1], f[(i + 1) % 2][j]);
            }
        }
        return f[n % 2][m];
    }
};
```

```go [sol-Go]
func longestCommonSubsequence(s, t string) int {
    n, m := len(s), len(t)
    f := [2][]int{make([]int, m+1), make([]int, m+1)}
    for i, x := range s {
        for j, y := range t {
            if x == y {
                f[(i+1)%2][j+1] = f[i%2][j] + 1
            } else {
                f[(i+1)%2][j+1] = max(f[i%2][j+1], f[(i+1)%2][j])
            }
        }
    }
    return f[n%2][m]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$。
- 空间复杂度：$\mathcal{O}(m)$。

## 四、空间优化：一个数组

### 答疑

**问**：为什么 $j$ 不能倒序循环？

**答**：本题 $f[i+1][j+1]$ 需要从 $f[i+1][j]$ 转移过来，这只能正序枚举 $j$。倒序枚举的话，$f[i+1][j]$ 还没有计算出来。

```py [sol-Python3]
class Solution:
    def longestCommonSubsequence(self, s: str, t: str) -> int:
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
    public int longestCommonSubsequence(String text1, String text2) {
        char[] t = text2.toCharArray();
        int m = t.length;
        int[] f = new int[m + 1];
        for (char x : text1.toCharArray()) {
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
    int longestCommonSubsequence(string s, string t) {
        int m = t.size();
        vector<int> f(m + 1);
        for (char x : s) {
            for (int j = 0, pre = 0; j < m; j++) {
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
func longestCommonSubsequence(s, t string) int {
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

- 时间复杂度：$\mathcal{O}(nm)$。
- 空间复杂度：$\mathcal{O}(m)$。

## 思考题

计算最长公共子序列的**个数**。

见 [P2516 [HAOI2010] 最长公共子序列](https://www.luogu.com.cn/problem/P2516)。

## 专题训练

见下面动态规划题单的「**§4.1 最长公共子序列（LCS）**」。

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
