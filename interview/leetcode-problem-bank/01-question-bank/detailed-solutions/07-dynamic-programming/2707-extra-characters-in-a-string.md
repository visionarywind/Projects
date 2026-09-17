# 2707. 字符串中的额外字符

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/extra-characters-in-a-string/
- 题目 slug：`extra-characters-in-a-string`
- 来源专题：动态规划
- 来源分类路径：五、划分型 DP / §5.2 最优划分
- 难度分：1736
- 外部题解来源：https://leetcode.cn/problems/extra-characters-in-a-string/solutions/2286613/dong-tai-gui-hua-cong-ji-yi-hua-sou-suo-wtd7a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划：从记忆化搜索到递推（Python/Java/C++/Go）](https://leetcode.cn/problems/extra-characters-in-a-string/solutions/2286613/dong-tai-gui-hua-cong-ji-yi-hua-sou-suo-wtd7a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-cong-ji-yi-hua-sou-suo-wtd7a`
- topic id：`2286613`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 一、寻找子问题

> 为了方便转成递推，从后往前考虑。

设 $n$ 为 $s$ 的长度。我们可以：

- 直接跳过 $s[n-1]$，那么问题变成 $s$ 的前缀 $[0,n-2]$ 的子问题（剩余字符的最少个数）。
- 考虑「枚举选哪个」，如果后缀 $[j,n-1]$ 在 $\textit{dictionary}$ 中，那么问题变成 $s$ 的前缀 $[0,j-1]$ 的子问题。

## 二、记忆化搜索

根据上面的讨论，定义 $\textit{dfs}(i)$ 表示 $s$ 的前缀 $[0,i]$ 的子问题（剩余字符的最少个数）。

分类讨论：

- 跳过 $s[i]$，问题变成 $s$ 的前缀 $[0,i-1]$ 的子问题，即 $\textit{dfs}(i)=\textit{dfs}(i-1)+1$，其中 $+1$ 是因为 $s[i]$ 是剩余字符。
- 考虑「枚举选哪个」，如果 $s$ 的子串 $[j,i]$ 在 $\textit{dictionary}$ 中，那么问题变成 $s$ 的前缀 $[0,j-1]$ 的子问题，有

$$
\textit{dfs}(i)=\min_{j=0}^{i}\textit{dfs}(j-1)
$$

这两种情况取最小值，即为 $\textit{dfs}(i)$。

递归边界：$\textit{dfs}(-1)=0$。

递归入口：$\textit{dfs}(n-1)$。

关于记忆化搜索，请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def minExtraChar(self, s: str, dictionary: List[str]) -> int:
        d = set(dictionary)

        @cache
        def dfs(i: int) -> int:
            if i < 0:
                return 0
            res = dfs(i - 1) + 1  # 不选
            for j in range(i + 1):  # 枚举选哪个
                if s[j: i + 1] in d:
                    res = min(res, dfs(j - 1))
            return res

        return dfs(len(s) - 1)
```

```go [sol-Go]
func minExtraChar(s string, dictionary []string) int {
	has := map[string]bool{}
	for _, s := range dictionary {
		has[s] = true
	}
	n := len(s)
	memo := make([]int, n)
	for i := range memo {
		memo[i] = -1
	}
	var dfs func(int) int
	dfs = func(i int) int {
		if i < 0 {
			return 0
		}
		p := &memo[i]
		if *p != -1 {
			return *p
		}

		// 不选
		res := dfs(i-1) + 1

		// 枚举选哪个
		for j := 0; j <= i; j++ {
			if has[s[j:i+1]] {
				res = min(res, dfs(j-1))
			}
		}

		*p = res
		return res
	}
	return dfs(n - 1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(L + n^3)$，其中 $n$ 为 $s$ 的长度，$L$ 为 $\textit{dictionary}$ 所有字符串的长度之和。预处理哈希表需要 $\mathcal{O}(L)$ 的时间。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(n^2)$，因此时间复杂度为 $\mathcal{O}(n^3)$。所以总的时间复杂度为 $\mathcal{O}(L + n^3)$。
- 空间复杂度：$\mathcal{O}(n+L)$。

## 三、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

做法：

- $\textit{dfs}$ 改成 $f$ 数组；
- 递归改成循环（每个参数都对应一层循环）；
- 递归边界改成 $f$ 数组的初始值。

具体来说，$f[i]$ 的含义和 $\textit{dfs}(i)$ 的含义是一样的，都表示 $s$ 的前缀 $[0,i]$ 的子问题（剩余字符的最少个数）。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 的一致：

- 跳过 $s$ 的最后一个字符，有 $f[i]=f[i-1]+1$。
- 考虑「枚举选哪个」，如果从 $s[j]$ 到 $s[i]$ 的子串在 $\textit{dictionary}$ 中，有

$$
f[i]=\min_{j=0}^{i}f[j-1]
$$

这两种情况取最小值。

但当 $i=0$ 或 $j=0$ 时，等号右边会出现负数下标。或者说，**这种定义方式没有状态能表示递归边界**，即出界的情况。

解决办法：在 $f$ 数组左边插入一个状态用于存储 $f[-1]$，把原来的 $f[i]$ 改成 $f[i+1]$，$f[j-1]$ 改成 $f[j]$。

相应的递推式为 $f[i+1]=f[i]+1$ 以及 $f[i+1]=\min\limits_{j=0}^{i}f[j]$。

初始值 $f[0]=0$，翻译自 $\textit{dfs}(-1)=0$。

答案为 $f[n]$，翻译自 $\textit{dfs}(n-1)$。

```py [sol-Python3]
class Solution:
    def minExtraChar(self, s: str, dictionary: List[str]) -> int:
        d = set(dictionary)
        n = len(s)
        f = [0] * (n + 1)
        for i in range(n):
            f[i + 1] = f[i] + 1  # 不选
            for j in range(i + 1):  # 枚举选哪个
                if s[j:i + 1] in d:
                    f[i + 1] = min(f[i + 1], f[j])
        return f[n]
```

```java [sol-Java]
class Solution {
    public int minExtraChar(String s, String[] dictionary) {
        var set = new HashSet<String>(dictionary.length);
        for (var str : dictionary) set.add(str);
        int n = s.length();
        var f = new int[n + 1];
        for (int i = 0; i < n; i++) {
            f[i + 1] = f[i] + 1; // 不选
            for (int j = 0; j <= i; j++) { // 枚举选哪个
                if (set.contains(s.substring(j, i + 1))) {
                    f[i + 1] = Math.min(f[i + 1], f[j]);
                }
            }
        }
        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minExtraChar(string s, vector<string> &dictionary) {
        unordered_set<string> set(dictionary.begin(), dictionary.end());
        int n = s.size();
        vector<int> f(n + 1);
        for (int i = 0; i < n; i++) {
            f[i + 1] = f[i] + 1; // 不选
            for (int j = 0; j <= i; j++) { // 枚举选哪个
                if (set.count(s.substr(j, i - j + 1))) {
                    f[i + 1] = min(f[i + 1], f[j]);
                }
            }
        }
        return f[n];
    }
};
```

```go [sol-Go]
func minExtraChar(s string, dictionary []string) int {
	has := map[string]bool{}
	for _, s := range dictionary {
		has[s] = true
	}
	n := len(s)
	f := make([]int, n+1)
	for i := 0; i < n; i++ {
		f[i+1] = f[i] + 1 // 不选
		for j := 0; j <= i; j++ { // 枚举选哪个
			if has[s[j:i+1]] {
				f[i+1] = min(f[i+1], f[j])
			}
		}
	}
	return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(L + n^3)$，其中 $n$ 为 $s$ 的长度，$L$ 为 $\textit{dictionary}$ 所有字符串的长度之和。预处理哈希表需要 $\mathcal{O}(L)$ 的时间。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(n^2)$，因此时间复杂度为 $\mathcal{O}(n^3)$。所以总的时间复杂度为 $\mathcal{O}(L + n^3)$。
- 空间复杂度：$\mathcal{O}(n+L)$。

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
