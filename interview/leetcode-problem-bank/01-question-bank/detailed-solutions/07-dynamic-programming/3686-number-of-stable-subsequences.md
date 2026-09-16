# 3686. 稳定子序列的数量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-stable-subsequences/
- 题目 slug：`number-of-stable-subsequences`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.4 合法子序列 DP
- 难度分：1969
- 外部题解来源：https://leetcode.cn/problems/number-of-stable-subsequences/solutions/3781347/he-fa-zi-xu-lie-dppythonjavacgo-by-endle-7sz0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[合法子序列 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-stable-subsequences/solutions/3781347/he-fa-zi-xu-lie-dppythonjavacgo-by-endle-7sz0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`he-fa-zi-xu-lie-dppythonjavacgo-by-endle-7sz0`
- topic id：`3781347`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 寻找子问题

考虑最后一个数 $x = \textit{nums}[n-1]$ 选或不选。

- 不选：问题变成在 $[0,n-2]$ 中能选出多少个稳定子序列。
- 选，那么有如下情况：
  - $x$ 单独组成一个长为 $1$ 的子序列。
  - 如果 $x$ 是偶数，那么 $x$ 可以添加到末尾为奇数的子序列的后面。问题变成在 $[0,n-2]$ 中能选出多少个末尾为奇数的稳定子序列。
  - 如果 $x$ 是偶数，那么 $x$ 可以添加到后两个数为奇数、偶数的子序列的后面。问题变成在 $[0,n-2]$ 中能选出多少个后两个数为奇数、偶数的稳定子序列。
  - 对于 $x$ 是奇数的情况，同理。

这些问题都是**和原问题相似的、规模更小的子问题**。

## 状态定义和状态转移方程

根据上面的讨论，定义 $f[i+1][x][j]$ 表示满足如下约束的稳定子序列的个数（$+1$ 是为了用 $f[0]$ 表示空前缀）：

- 元素下标在 $[0,i]$ 中。
- 子序列末尾元素的奇偶性为 $x$，其中 $x=0$ 或者 $1$。
- 子序列末尾恰好有连续 $j+1$ 个奇偶性都为 $x$ 的数。恰好的意思是，倒数第 $j+2$ 个数的奇偶性与 $x$ 相反，即 $x$ 异或 $1$，下文用 $x\oplus 1$ 表示。

设 $\textit{nums}[i]$ 的奇偶性为 $x$。考虑 $\textit{nums}[i]$ 选或不选。

- 不选：问题变成在 $[0,i-1]$ 中的满足 $x$ 和 $j$ 的稳定子序列的个数，即 $f[i][x][j]$。
- 选：
  - $\textit{nums}[i]$ 单独组成一个长为 $1$ 的稳定子序列，个数为 $1$。
  - 当 $j=0$ 时，$\textit{nums}[i]$ 只能添加到末尾元素奇偶性为 $x\oplus 1$ 的稳定子序列的后面，从 $f[i][x\oplus 1][0] + f[i][x\oplus 1][1]$ 转移过来。比如 $\textit{nums}[i]$ 是偶数，那么可以添加到末尾恰好有 $1$ 个或者 $2$ 个奇数的稳定子序列的后面。
  - 当 $j=1$ 时，$\textit{nums}[i]$ 只能添加到末尾元素奇偶性为 $x$，且末尾恰好有一个奇偶性为 $x$ 的稳定子序列的后面，从 $f[i][x][0]$ 转移过来。比如 $\textit{nums}[i]$ 是偶数，那么可以添加到末尾恰好有 $1$ 个偶数的稳定子序列的后面。

累加得

$$
\begin{aligned}
f[i+1][x][0] &= f[i][x][0] + f[i][x\oplus 1][0] + f[i][x\oplus 1][1] + 1     \\
f[i+1][x][1] &= f[i][x][1] + f[i][x][0]     \\
\end{aligned}
$$

对于奇偶性不等于 $x$ 的状态，$\textit{nums}[i]$ 一定不能选，即

$$
f[i+1][x\oplus 1][j] = f[i][x\oplus 1][j]
$$

初始值：$f[0][x][j] = 0$。

答案：$f[n][0][0] + f[n][0][1] + f[n][1][0] + f[n][1][1]$。

代码实现时，$f$ 的第一个维度可以去掉。去掉后，由于 $f[x][1]$ 会用到 $f[x][0]$，所以要先更新 $f[x][1]$，再更新 $f[x][0]$。

注意取模。为什么可以在计算中途取模？见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

[本题视频讲解](https://www.bilibili.com/video/BV1TBpczdE8P/?t=24m20s)，欢迎点赞关注~

```py [sol-Python3]
# 更快的写法见【Python3 写法二】
class Solution:
    def countStableSubsequences(self, nums: list[int]) -> int:
        MOD = 1_000_000_007
        f = [[0, 0], [0, 0]]
        for x in nums:
            x %= 2
            f[x][1] = (f[x][1] + f[x][0]) % MOD
            f[x][0] = (f[x][0] + f[x ^ 1][0] + f[x ^ 1][1] + 1) % MOD
        return (f[0][0] + f[0][1] + f[1][0] + f[1][1]) % MOD
```

```py [sol-Python3 写法二]
class Solution:
    def countStableSubsequences(self, nums: list[int]) -> int:
        MOD = 1_000_000_007
        f00 = f01 = f10 = f11 = 0  # 去掉访问 list 的开销
        for x in nums:
            if x % 2 == 0:
                f01 = (f01 + f00) % MOD
                f00 = (f00 + f10 + f11 + 1) % MOD
            else:
                f11 = (f11 + f10) % MOD
                f10 = (f10 + f00 + f01 + 1) % MOD
        return (f00 + f01 + f10 + f11) % MOD
```

```java [sol-Java]
class Solution {
    public int countStableSubsequences(int[] nums) {
        final int MOD = 1_000_000_007;
        long[][] f = new long[2][2];
        for (int x : nums) {
            x %= 2;
            f[x][1] = (f[x][1] + f[x][0]) % MOD;
            f[x][0] = (f[x][0] + f[x ^ 1][0] + f[x ^ 1][1] + 1) % MOD;
        }
        return (int) ((f[0][0] + f[0][1] + f[1][0] + f[1][1]) % MOD);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countStableSubsequences(vector<int>& nums) {
        const int MOD = 1'000'000'007;
        long long f[2][2]{};
        for (int x : nums) {
            x %= 2;
            f[x][1] = (f[x][1] + f[x][0]) % MOD;
            f[x][0] = (f[x][0] + f[x ^ 1][0] + f[x ^ 1][1] + 1) % MOD;
        }
        return (f[0][0] + f[0][1] + f[1][0] + f[1][1]) % MOD;
    }
};
```

```go [sol-Go]
func countStableSubsequences(nums []int) int {
	const mod = 1_000_000_007
	f := [2][2]int{}
	for _, x := range nums {
		x %= 2
		f[x][1] = (f[x][1] + f[x][0]) % mod
		f[x][0] = (f[x][0] + f[x^1][0] + f[x^1][1] + 1) % mod
	}
	return (f[0][0] + f[0][1] + f[1][0] + f[1][1]) % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk)$，其中 $n$ 是 $\textit{nums}$ 的长度，$k=2$ 表示最多 $k$ 个连续元素的奇偶性相同。
- 空间复杂度：$\mathcal{O}(k)$。

## 专题训练

见下面动态规划题单的「**§7.4 合法子序列 DP**」。

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
