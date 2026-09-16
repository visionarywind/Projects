# 3259. 超级饮料的最大强化能量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-energy-boost-from-two-drinks/
- 题目 slug：`maximum-energy-boost-from-two-drinks`
- 来源专题：动态规划
- 来源分类路径：六、状态机 DP / §6.2 基础
- 难度分：1484
- 外部题解来源：https://leetcode.cn/problems/maximum-energy-boost-from-two-drinks/solutions/2884478/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-iebb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-energy-boost-from-two-drinks/solutions/2884478/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-iebb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-iebb`
- topic id：`2884478`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意

给你两个数组 $a$ 和 $b$。从 $i=0$ 开始，要么选 $a[i]$，要么选 $b[i]$。如果你当前选了 $a$ 中的元素，后面想选 $b$ 中的元素，那么下一个元素必须不能选。例如现在选 $a[i]$，那么后面可以选 $b[i+2]$，但不能选 $b[i+1]$。

返回所选元素之和的最大值。

## 一、寻找子问题

本题有点类似 [198. 打家劫舍](https://leetcode.cn/problems/house-robber/)，不过我们还是从子问题开始讨论。

例如 $a=[1,3,1,2,3],\ b=[3,1,1,2,3]$。

如果最后一个数我们选了 $a[4]=3$，那么：

- 继续选 $a$ 中的元素，那么下一个数选 $a[3]=2$，需要解决的问题为：从下标 $[0,3]$ 中选数字，且最后选的是 $a[3]$ 的情况下，所选元素之和的最大值。
- 改成选 $b$ 中的元素，那么下一个数选 $b[2]=1$，需要解决的问题为：从下标 $[0,2]$ 中选数字，且最后选的是 $b[2]$ 的情况下，所选元素之和的最大值。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

> 注 1：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。
>
> 注 2：动态规划有「**选或不选**」和「**枚举选哪个**」两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「**选或不选**」：如果当前选了 $a$ 中的元素，接下来要么继续选 $a$ 中的元素，要么不继续选 $a$ 中的元素，也就是选 $b$ 中的元素。

## 二、状态定义与状态转移方程

因为要解决的问题都形如「从下标 $[0,i]$ 中选数字，且最后选的是 $a[i]$ 或 $b[i]$ 的情况下，所选元素之和的最大值」，所以用它作为本题的状态定义 $\textit{dfs}(i,j)$。其中 $j=0,1$，分别表示最后选的是 $a[i]$ 还是 $b[i]$。

为方便实现，把 $a$ 和 $b$ 加到一个长为 $2$ 的二维数组 $c$ 中。 

分类讨论：

- 继续选 $c[j]$ 中的元素，那么下一个数选 $c[j][i-1]$，需要解决的问题为：从下标 $[0,i-1]$ 中选数字，且最后选的是 $c[j]$ 中的元素的情况下，所选元素之和的最大值，即 $\textit{dfs}(i-1,j)$。
- 改成选 $c[j\oplus 1]$ 中的元素，那么下一个数选 $c[j\oplus 1][i-2]$，需要解决的问题为：从下标 $[0,i-2]$ 中选数字，且最后选的是 $c[j\oplus 1]$ 中的元素的情况下，所选元素之和的最大值，即 $\textit{dfs}(i-2,j\oplus 1)$。其中 $\oplus$ 为异或运算，通过异或 $1$，可以把 $0$ 变成 $1$，把 $1$ 变成 $0$。

这两种情况取最大值，就得到了 $\textit{dfs}(i,j)$，即

$$
\textit{dfs}(i,j) = \max(\textit{dfs}(i-1,j),\textit{dfs}(i-2,j\oplus 1)) + c[j][i]
$$

递归边界：$\textit{dfs}(-2, j)=\textit{dfs}(-1, j)=0$。没有元素可以选了。

递归入口：枚举最后一个数选的是 $a[n-1]$ 还是 $b[n-1]$，取最大值，即 $\max(\textit{dfs}(n-1,0), \textit{dfs}(n-1,1))$，也就是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

本题元素值均为正数，初始化成 $0$ 也可以。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

本题 [视频讲解](https://www.bilibili.com/video/BV1hH4y1c7T5/) 第二题，欢迎点赞关注！

```py [sol-Python3]
class Solution:
    def maxEnergyBoost(self, a: List[int], b: List[int]) -> int:
        c = (a, b)
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int, j: int) -> int:
            if i < 0:
                return 0
            return max(dfs(i - 1, j), dfs(i - 2, j ^ 1)) + c[j][i]
        return max(dfs(len(a) - 1, 0), dfs(len(a) - 1, 1))
```

```java [sol-Java]
class Solution {
    public long maxEnergyBoost(int[] a, int[] b) {
        int n = a.length;
        int[][] c = {a, b};
        long[][] memo = new long[n][2];
        return Math.max(dfs(n - 1, 0, c, memo), dfs(n - 1, 1, c, memo));
    }

    private long dfs(int i, int j, int[][] c, long[][] memo) {
        if (i < 0) {
            return 0;
        }
        if (memo[i][j] > 0) { // 之前计算过
            return memo[i][j];
        }
        return memo[i][j] = Math.max(dfs(i - 1, j, c, memo), dfs(i - 2, j ^ 1, c, memo)) + c[j][i];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxEnergyBoost(vector<int>& a, vector<int>& b) {
        int n = a.size();
        vector<int> c[2] = {move(a), move(b)};
        vector<array<long long, 2>> memo(n);
        auto dfs = [&](auto&& dfs, int i, int j) -> long long {
            if (i < 0) {
                return 0;
            }
            auto& res = memo[i][j]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }
            return res = max(dfs(dfs, i - 1, j), dfs(dfs, i - 2, j ^ 1)) + c[j][i];
        };
        return max(dfs(dfs, n - 1, 0), dfs(dfs, n - 1, 1));
    }
};
```

```go [sol-Go]
func maxEnergyBoost(a, b []int) int64 {
	n := len(a)
	c := [2][]int{a, b}
	memo := make([][2]int64, n)
	var dfs func(int, int) int64
	dfs = func(i, j int) int64 {
		if i < 0 {
			return 0
		}
		p := &memo[i][j]
		if *p == 0 { // 首次计算
			*p = max(dfs(i-1, j), dfs(i-2, j^1)) + int64(c[j][i])
		}
		return *p
	}
	return max(dfs(n-1, 0), dfs(n-1, 1))
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $a$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i+2][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示从下标 $[0,i]$ 中选数字，且最后选的是 $a[i]$ 或 $b[i]$ 的情况下，所选元素之和的最大值。这里 $+2$ 是为了把 $\textit{dfs}(-2,j)$ 和 $\textit{dfs}(-1,j)$ 这两个状态也翻译过来，这样我们可以把 $f[0]$ 和 $f[1]$ 作为初始值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样（注意 $+2$）：

$$
f[i+2][j] = \max(f[i+1][j],f[i][j\oplus 1]) + c[j][i]
$$

初始值 $f[0][j]=f[1][j]=0$，翻译自递归边界 $\textit{dfs}(-2,j)=\textit{dfs}(-1,j)=0$。

答案为 $\max(f[n+1][0],f[n+1][1])$，翻译自递归入口 $\max(\textit{dfs}(n-1,0), \textit{dfs}(n-1,1))$。

```py [sol-Python3]
class Solution:
    def maxEnergyBoost(self, a: List[int], b: List[int]) -> int:
        n = len(a)
        f = [[0, 0] for _ in range(n + 2)]
        for i, (x, y) in enumerate(zip(a, b)):
            f[i + 2][0] = max(f[i + 1][0], f[i][1]) + x
            f[i + 2][1] = max(f[i + 1][1], f[i][0]) + y
        return max(f[-1])
```

```java [sol-Java]
class Solution {
    public long maxEnergyBoost(int[] a, int[] b) {
        int n = a.length;
        long[][] f = new long[n + 2][2];
        for (int i = 0; i < n; i++) {
            f[i + 2][0] = Math.max(f[i + 1][0], f[i][1]) + a[i];
            f[i + 2][1] = Math.max(f[i + 1][1], f[i][0]) + b[i];
        }
        return Math.max(f[n + 1][0], f[n + 1][1]);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxEnergyBoost(vector<int>& a, vector<int>& b) {
        int n = a.size();
        vector<array<long long, 2>> f(n + 2);
        for (int i = 0; i < n; i++) {
            f[i + 2][0] = max(f[i + 1][0], f[i][1]) + a[i];
            f[i + 2][1] = max(f[i + 1][1], f[i][0]) + b[i];
        }
        return max(f[n + 1][0], f[n + 1][1]);
    }
};
```

```go [sol-Go]
func maxEnergyBoost(a, b []int) int64 {
	n := len(a)
	f := make([][2]int64, n+2)
	for i, x := range a {
		f[i+2][0] = max(f[i+1][0], f[i][1]) + int64(x)
		f[i+2][1] = max(f[i+1][1], f[i][0]) + int64(b[i])
	}
	return max(f[n+1][0], f[n+1][1])
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $a$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

注：利用滚动变量，空间复杂度可以优化至 $\mathcal{O}(1)$。

注：本题还有另外一种状态定义：考虑选当前元素，还是跳过不选当前元素（切换到另一个数组）。与之对比，本题解的思路是当前元素一定要选，用递归到 $i-2$ 表示跳过 $i-1$ 不选。如果数组中有负数，哪种定义方式更符合题目描述？注意有负数也得喝饮料。

更多相似题目，见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**五、状态机 DP**」。

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
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `六、状态机 DP / §6.2 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、状态机 DP / §6.2 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
