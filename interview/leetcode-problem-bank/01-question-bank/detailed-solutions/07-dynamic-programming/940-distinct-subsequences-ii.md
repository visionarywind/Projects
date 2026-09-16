# 940. 不同的子序列 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/distinct-subsequences-ii/
- 题目 slug：`distinct-subsequences-ii`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.4 合法子序列 DP
- 难度分：1985
- 外部题解来源：https://leetcode.cn/problems/distinct-subsequences-ii/solutions/1890716/xi-fen-wen-ti-fu-za-du-you-hua-pythonjav-1ihu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[细分问题 + 复杂度优化（Python/Java/C++/Go）](https://leetcode.cn/problems/distinct-subsequences-ii/solutions/1890716/xi-fen-wen-ti-fu-za-du-you-hua-pythonjav-1ihu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xi-fen-wen-ti-fu-za-du-you-hua-pythonjav-1ihu`
- topic id：`1890716`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

#### 提示 1

对于子序列问题，通常可以从每个元素「选或不选」的角度入手。

#### 提示 2

麻烦的地方在于，如何保证对同样的子序列，我们只统计一次？

比如示例 2，从左往右遍历 $\text{`aba'}$，对于前两个字符 $\text{`ab'}$ 可以得到 $\text{`a'},\text{`b'},\text{`ab'}$ 这三个不同的子序列，但是遍历到第三个字符 $\text{`a'}$ 时，如果选 $\text{`a'}$ 单独作为子序列，会和前面得到的 $\text{`a'}$ 重复。

你可能会想，如果我把这个 $\text{`a'}$ **加到 $\text{`a'},\text{`b'},\text{`ab'}$ 的末尾**呢？这样可以得到不同的子序列 $\text{`aa'},\text{`ba'},\text{`aba'}$，同时把 $\text{`a'}$ 单独作为一个子序列，也不会和这些子序列重复。

这个想法不错，但是这种做法没有把 $\text{`b'},\text{`ab'}$ 计入（即不选 $\text{`a'}$ 的情况）。不过注意到这两个都不是以 $\text{`a'}$ 结尾的，那如果把原问题细分一下，改为**分别统计以 $\text{`a'},\text{`b'},\cdots,\text{`z'}$ 结尾的不同子序列的个数**，问题就迎刃而解了。

具体来说，在遍历 $s$ 的过程中，把 $s[i]$ 加到**前 $i-1$ 个字符**组成的不同子序列的末尾，同时把 $s[i]$ 单独作为一个子序列，即得到了**前 $i$ 个字符**以 $s[i]$ 结尾的不同子序列的个数。遍历结束后把以 $\text{`a'},\text{`b'},\cdots,\text{`z'}$ 结尾的不同子序列的个数相加，就得到了答案。

你也可以这样理解：这种做法对于相同的子序列，只会考虑其最后一次出现的位置（下标序列的字典序最大）。

#### 提示 3

怎么实现？

上述思路是一个标准的关于**递推**的描述，我们定义 $f[i+1][j]$ 表示在 $s[0]$ 到 $s[i]$ 中的以字母 $j$ 结尾的不同非空子序列的个数。$+1$ 是方便用 $f[0]$ 表示空。

根据上述思路，得

$$
f[i+1][s[i]] = 1 + \sum_{j=\text{`a'}}^{\text{`z'}} f[i][j]
$$

这里的 $1$ 指的是单独把 $s[i]$ 作为一个子序列。

初始值 $f[0][j] = 0$，答案为 $\sum\limits_{j=\text{`a'}}^{\text{`z'}} f[n][j]$。

为方便计算，代码实现时把字母 $\text{`a'}$ 到 $\text{`z'}$ 用 $0$ 到 $25$ 表示。

记得取模。关于取模的知识点，见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

```py [sol-Python3]
MOD = 1_000_000_007

class Solution:
    def distinctSubseqII(self, s: str) -> int:
        f = [[0] * 26 for _ in range(len(s) + 1)]
        for i, c in enumerate(s):
            c = ord(c) - ord('a')
            f[i + 1] = f[i].copy()
            f[i + 1][c] = (1 + sum(f[i])) % MOD
        return sum(f[-1]) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int distinctSubseqII(String s) {
        int n = s.length();
        long[][] f = new long[n + 1][26];
        for (int i = 0; i < n; i++) {
            f[i + 1] = f[i].clone();
            f[i + 1][s.charAt(i) - 'a'] = (1 + Arrays.stream(f[i]).sum()) % MOD;
        }
        return (int) (Arrays.stream(f[n]).sum() % MOD);
    }
}
```

```cpp [sol-C++]
class Solution {
    const int MOD = 1'000'000'007;
public:
    int distinctSubseqII(string s) {
        int n = s.size();
        vector<array<int, 26>> f(n + 1);
        for (int i = 0; i < n; i++) {
            f[i + 1] = f[i];
            f[i + 1][s[i] - 'a'] = reduce(f[i].begin(), f[i].end(), 1LL) % MOD;
        }
        return reduce(f[n].begin(), f[n].end(), 0LL) % MOD;
    }
};
```

```go [sol-Go]
func distinctSubseqII(s string) int {
	const mod = 1_000_000_007
	n := len(s)
	f := make([][26]int, n+1)
	for i, b := range s {
		total := 0
		for _, v := range f[i] {
			total += v
		}
		f[i+1] = f[i]
		f[i+1][b-'a'] = 1 + total%mod
	}
	total := 0
	for _, v := range f[n] {
		total += v
	}
	return total % mod
}
```

由于状态转移只发生在 $f[i+1]$ 和 $f[i]$ 之间，因此可以去掉 $f$ 的第一个维度，简化成一个长为 $26$ 的一维数组。由于除了 $f[s[i]]$ 以外，其余值都不变，因此只需要更新 $f[s[i]]$ 的值。

```py [sol-Python3]
MOD = 1_000_000_007

class Solution:
    def distinctSubseqII(self, s: str) -> int:
        f = [0] * 26
        for c in s:
            f[ord(c) - ord('a')] = (1 + sum(f)) % MOD
        return sum(f) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int distinctSubseqII(String s) {
        long[] f = new long[26];
        int n = s.length();
        for (int i = 0; i < n; i++) {
            f[s.charAt(i) - 'a'] = (1 + Arrays.stream(f).sum()) % MOD;
        }
        return (int) (Arrays.stream(f).sum() % MOD);
    }
}
```

```cpp [sol-C++]
class Solution {
    const int MOD = 1'000'000'007;
public:
    int distinctSubseqII(string s) {
        int f[26]{};
        for (char c : s) {
            f[c - 'a'] = reduce(f, f + 26, 1LL) % MOD;
        }
        return reduce(f, f + 26, 0LL) % MOD;
    }
};
```

```go [sol-Go]
func distinctSubseqII(s string) int {
	const mod = 1_000_000_007
	f := [26]int{}
	for _, b := range s {
		total := 0
		for _, v := range f {
			total += v
		}
		f[b-'a'] = 1 + total%mod
	}
	total := 0
	for _, v := range f {
		total += v
	}
	return total % mod
}
```

由于在 $\textit{total}=\sum\limits_{j=0}^{25} f[j]$ 中，每次只有 $f[s[i]]$ 在变动，因此我们可以直接更新 $f[s[i]]$ 和 $\textit{total}$，不需要每次都重新算一遍 $\textit{total}$。

```py [sol-Python3]
MOD = 1_000_000_007

class Solution:
    def distinctSubseqII(self, s: str) -> int:
        f = [0] * 26
        total = 0
        for c in s:
            c = ord(c) - ord('a')
            others = total - f[c]  # total 中不含 f[c] 的部分
            f[c] = 1 + total
            total = (f[c] + others) % MOD
        return total
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int distinctSubseqII(String s) {
        int total = 0;
        int[] f = new int[26];
        int n = s.length();
        for (int i = 0; i < n; i++) {
            int c = s.charAt(i) - 'a';
            int others = (total - f[c] + MOD) % MOD; // total 中不含 f[c] 的部分（+MOD 保证结果非负）
            f[c] = 1 + total;
            total = (f[c] + others) % MOD;
        }
        return total;
    }
}
```

```cpp [sol-C++]
class Solution {
    const int MOD = 1'000'000'007;
public:
    int distinctSubseqII(string s) {
        int total = 0, f[26]{};
        for (char c : s) {
            c -= 'a';
            int others = (total - f[c] + MOD) % MOD; // total 中不含 f[c] 的部分（+MOD 保证结果非负）
            f[c] = 1 + total;
            total = (f[c] + others) % MOD;
        }
        return total;
    }
};
```

```go [sol-Go]
func distinctSubseqII(s string) (total int) {
	const mod = 1_000_000_007
	f := [26]int{}
	for _, b := range s {
		b -= 'a'
		others := total - f[b] // total 中不含 f[b] 的部分（由于取模的原因，这里的减法可能会产生负数）
		f[b] = 1 + total
		total = (f[b] + others) % mod
	}
	return (total + mod) % mod // 保证结果非负
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+|\Sigma|)$，其中 $n$ 为 $s$ 的长度，$|\Sigma|=26$ 为字符集合的大小。
- 空间复杂度：$\mathcal{O}(|\Sigma|)$。

## 相似题目

- 动态规划题单中的「**§7.4 合法子序列 DP**」。
- [CF1183H. Subsequences (hard version)](https://codeforces.com/problemset/problem/1183/H)
- [CF756D. Bacterial Melee](https://codeforces.com/problemset/problem/756/D)

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

本题来自 `七、其他线性 DP / §7.4 合法子序列 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.4 合法子序列 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
