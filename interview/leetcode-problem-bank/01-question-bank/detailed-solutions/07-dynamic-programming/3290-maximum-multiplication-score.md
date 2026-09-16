# 3290. 最高乘法得分

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-multiplication-score/
- 题目 slug：`maximum-multiplication-score`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.2 进阶
- 难度分：1692
- 外部题解来源：https://leetcode.cn/problems/maximum-multiplication-score/solutions/2917840/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-9ul8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推到空间优化！（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-multiplication-score/solutions/2917840/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-9ul8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-9ul8`
- topic id：`2917840`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意

从 $b$ 中选一个长为 $4$ 的子序列，与 $a$ 计算**点积**，返回最大点积。

## 视频讲解

请看 [本题视频讲解](https://www.bilibili.com/video/BV1Qp4me2Emz/)（第二题），欢迎点赞关注~

## 一、寻找子问题

看示例 1，$a=[3,2,5,6],\ b = [2,-6,4,-5,-3,2,-7]$。

考虑从右往左选 $b$ 数字，分类讨论：

- 如果不选 $b[6]$，那么需要解决的问题为：从 $b[0]$ 到 $b[5]$ 选 $4$ 个数，与 $a[0]$ 到 $a[3]$ 计算点积，结果的最大值。
- 如果选 $b[6]$，那么需要解决的问题为：从 $b[0]$ 到 $b[5]$ 选 $3$ 个数，与 $a[0]$ 到 $a[2]$ 计算点积，结果的最大值。

由于选或不选都会把原问题变成一个**和原问题相似的、规模更小的子问题**，所以可以用**递归**解决。

> 注 1：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。
> 
> 注 2：动态规划有「**选或不选**」和「**枚举选哪个**」两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「**选或不选**」。一般来说相邻无关用选或不选，相邻相关（比如 LIS）用枚举选哪个。

## 二、状态定义与状态转移方程

根据上面的讨论，我们需要在递归过程中跟踪以下信息：

- $i$：当前要考虑 $b[i]$ 选或不选。
- $j$：如果选 $b[i]$，那么与 $a[j]$ 相乘。

因此，定义状态为 $\textit{dfs}(i,j)$，表示从 $b[0]$ 到 $b[i]$ 选 $j+1$ 个数，与 $a[0]$ 到 $a[j]$ 计算点积，结果的最大值。

接下来，思考如何从一个状态转移到另一个状态。

考虑 $b[i]$ 数字，分类讨论：

- 如果不选 $b[i]$，那么需要解决的问题为：从 $b[0]$ 到 $b[i-1]$ 选 $j+1$ 个数，与 $a[0]$ 到 $a[j]$ 计算点积，结果的最大值，即 $\textit{dfs}(i-1,j)$。
- 如果选 $b[i]$，那么需要解决的问题为：从 $b[0]$ 到 $b[i-1]$ 选 $j$ 个数，与 $a[0]$ 到 $a[j-1]$ 计算点积，结果的最大值，即 $\textit{dfs}(i-1,j-1) + a[j]\cdot b[i]$。

这两种情况取最大值，就得到了 $\textit{dfs}(i,j)$，即

$$
\textit{dfs}(i,j) = \max(\textit{dfs}(i-1,j),\textit{dfs}(i-1,j-1) + a[j]\cdot b[i])
$$

**递归边界**：$\textit{dfs}(i,-1)=0,\ \textit{dfs}(-1,j\ge 0)=-\infty$。用 $-\infty$ 表示不合法的状态，保证 $\max$ 不会取到不合法的状态。

**递归入口**：$\textit{dfs}(n-1,3)$，也就是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$ 或者 $-1$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$ 或者 $-1$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。本题由于可以算出负数，可以把初始值设为一个算不出的值 $-\infty$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

⚠**注意**：代码先判断了 $j<0$，然后判断 $i<0$。如果你想反过来写，要在 $i<0$ 的逻辑里面额外判断 $j<0$ 的情况。

```py [sol-Python3]
class Solution:
    def maxScore(self, a: List[int], b: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int, j: int) -> int:
            if j < 0:  # 选完了
                return 0
            if i < 0:  # j >= 0，没选完
                return -inf
            return max(dfs(i - 1, j), dfs(i - 1, j - 1) + a[j] * b[i])
        ans = dfs(len(b) - 1, 3)
        dfs.cache_clear()  # 状态个数比较多的题目需要用，防止爆内存
        return ans
```

```java [sol-Java]
class Solution {
    public long maxScore(int[] a, int[] b) {
        int n = b.length;
        long[][] memo = new long[n][4];
        for (long[] row : memo) {
            Arrays.fill(row, Long.MIN_VALUE); // 表示没有计算过
        }
        return dfs(n - 1, 3, a, b, memo);
    }

    private long dfs(int i, int j, int[] a, int[] b, long[][] memo) {
        if (j < 0) { // 选完了
            return 0;
        }
        if (i < 0) { // j >= 0，没选完
            return Long.MIN_VALUE / 2; // 防止溢出
        }
        if (memo[i][j] == Long.MIN_VALUE) { // 需要计算，并记忆化
            memo[i][j] = Math.max(dfs(i - 1, j, a, b, memo), dfs(i - 1, j - 1, a, b, memo) + (long) a[j] * b[i]);
        }
        return memo[i][j];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxScore(vector<int>& a, vector<int>& b) {
        int n = b.size();
        vector<array<long long, 4>> memo(n);
        for (auto& row : memo) {
            ranges::fill(row, LLONG_MIN); // 表示没有计算过
        }

        auto dfs = [&](auto&& dfs, int i, int j) -> long long {
            if (j < 0) { // 选完了
                return 0;
            }
            if (i < 0) { // j >= 0，没选完
                return LLONG_MIN / 2; // 防止溢出
            }
            auto& res = memo[i][j]; // 注意这里是引用
            if (res == LLONG_MIN) { // 需要计算，并记忆化
                res = max(dfs(dfs, i - 1, j), dfs(dfs, i - 1, j - 1) + (long long) a[j] * b[i]);
            }
            return res;
        };

        return dfs(dfs, n - 1, 3);
    }
};
```

```go [sol-Go]
func maxScore(a, b []int) int64 {
	n := len(b)
	memo := make([][4]int64, n)
	for i := range memo {
		for j := range memo[i] {
			memo[i][j] = math.MinInt64 // 表示没有计算过
		}
	}
	var dfs func(int, int) int64
	dfs = func(i, j int) int64 {
		if j < 0 { // 选完了
			return 0
		}
		if i < 0 { // j >= 0，没选完
			return math.MinInt64 / 2 // 防止溢出
		}
		p := &memo[i][j]
		if *p == math.MinInt64 { // 需要计算，并记忆化
			*p = max(dfs(i-1, j), dfs(i-1, j-1)+int64(a[j])*int64(b[i]))
		}
		return *p
	}
	return dfs(n-1, 3)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m=4$ 是 $a$ 的长度，$n$ 是 $b$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(mn)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(mn)$。
- 空间复杂度：$\mathcal{O}(mn)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i+1][j+1]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示从 $b[0]$ 到 $b[i]$ 选 $j+1$ 个数，与 $a[0]$ 到 $a[j]$ 计算点积，结果的最大值。这里 $+1$ 是为了把 $\textit{dfs}(-1,j)$ 和 $\textit{dfs}(i,-1)$ 也翻译过来，这样我们可以把 $f[0][j]$ 和 $f[i][0]$ 作为初始值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i+1][j+1] = \max(f[i][j+1],f[i][j] + a[j]\cdot b[i])
$$

初始值 $f[i][0]=0,\ f[0][j>0]=-\infty$，翻译自递归边界 $\textit{dfs}(i,-1)=0,\ \textit{dfs}(-1,j\ge 0)=-\infty$。

答案为 $f[n][4]$，翻译自递归入口 $\textit{dfs}(n-1,3)$。

```py [sol-Python3]
class Solution:
    def maxScore(self, a: List[int], b: List[int]) -> int:
        n = len(b)
        f = [[0] * 5 for _ in range(n + 1)]
        f[0][1:] = [-inf] * 4
        for i, y in enumerate(b):
            for j, x in enumerate(a):
                f[i + 1][j + 1] = max(f[i][j + 1], f[i][j] + x * y)
        return f[n][4]
```

```java [sol-Java]
class Solution {
    public long maxScore(int[] a, int[] b) {
        int n = b.length;
        long[][] f = new long[n + 1][5];
        Arrays.fill(f[0], 1, 5, Long.MIN_VALUE / 2);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < 4; j++) {
                f[i + 1][j + 1] = Math.max(f[i][j + 1], f[i][j] + (long) a[j] * b[i]);
            }
        }
        return f[n][4];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxScore(vector<int>& a, vector<int>& b) {
        int n = b.size();
        vector<array<long long, 5>> f(n + 1);
        for (int j = 1; j < 5; j++) {
            f[0][j] = LLONG_MIN / 2;
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < 4; j++) {
                f[i + 1][j + 1] = max(f[i][j + 1], f[i][j] + (long long) a[j] * b[i]);
            }
        }
        return f[n][4];
    }
};
```

```go [sol-Go]
func maxScore(a, b []int) int64 {
	n := len(b)
	f := make([][5]int64, n+1)
	for j := 1; j < 5; j++ {
		f[0][j] = math.MinInt64 / 2
	}
	for i, y := range b {
		for j, x := range a {
			f[i+1][j+1] = max(f[i][j+1], f[i][j]+int64(x)*int64(y))
		}
	}
	return f[n][4]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m=4$ 是 $a$ 的长度，$n$ 是 $b$ 的长度。
- 空间复杂度：$\mathcal{O}(mn)$。

## 五、空间优化

像 [0-1 背包](https://www.bilibili.com/video/BV16Y411v7Y6/) 那样，去掉第一个维度，把 $f[i+1]$ 和 $f[i]$ 保存到**同一个数组**中。

状态转移方程类似 0-1 背包，$j$ 要倒序枚举。

```py [sol-Python3]
class Solution:
    def maxScore(self, a: List[int], b: List[int]) -> int:
        f = [0] + [-inf] * 4
        for y in b:
            for j in range(3, -1, -1):
                f[j + 1] = max(f[j + 1], f[j] + a[j] * y)
        return f[4]
```

```py [sol-Python3 写法二]
class Solution:
    def maxScore(self, a: List[int], b: List[int]) -> int:
        f0 = f1 = f2 = f3 = -inf
        for y in b:
            f3 = max(f3, a[3] * y + f2)
            f2 = max(f2, a[2] * y + f1)
            f1 = max(f1, a[1] * y + f0)
            f0 = max(f0, a[0] * y)
        return f3
```

```py [sol-Python3 极致优化]
class Solution:
    def maxScore(self, a: List[int], b: List[int]) -> int:
        a0, a1, a2, a3 = a  # 去掉访问 list 的开销
        f0 = f1 = f2 = f3 = -inf
        for y in b:
            t = a3 * y + f2
            if t > f3: f3 = t  # 手动 max 效率更高

            t = a2 * y + f1
            if t > f2: f2 = t

            t = a1 * y + f0
            if t > f1: f1 = t

            t = a0 * y
            if t > f0: f0 = t
        return f3
```

```java [sol-Java]
class Solution {
    public long maxScore(int[] a, int[] b) {
        long[] f = new long[5];
        Arrays.fill(f, 1, 5, Long.MIN_VALUE / 2);
        for (int y : b) {
            for (int j = 3; j >= 0; j--) {
                f[j + 1] = Math.max(f[j + 1], f[j] + (long) a[j] * y);
            }
        }
        return f[4];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxScore(vector<int>& a, vector<int>& b) {
        long long f[5]{};
        fill(f + 1, f + 5, LLONG_MIN / 2);
        for (int y : b) {
            for (int j = 3; j >= 0; j--) {
                f[j + 1] = max(f[j + 1], f[j] + (long long) a[j] * y);
            }
        }
        return f[4];
    }
};
```

```go [sol-Go]
func maxScore(a, b []int) int64 {
	f := [5]int64{}
	for j := 1; j < 5; j++ {
		f[j] = math.MinInt64 / 2
	}
	for _, y := range b {
		for j := 3; j >= 0; j-- {
			f[j+1] = max(f[j+1], f[j]+int64(a[j])*int64(y))
		}
	}
	return f[4]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m=4$ 是 $a$ 的长度，$n$ 是 $b$ 的长度。
- 空间复杂度：$\mathcal{O}(m)$。

## 相似题目

- [1458. 两个子序列的最大点积](https://leetcode.cn/problems/max-dot-product-of-two-subsequences/)

更多相似题目，见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**§4.1 最长公共子序列（LCS）**」和「**五、状态机 DP**」。

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
