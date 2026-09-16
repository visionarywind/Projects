# 3780. 能被 3 整除的三元组最大和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-sum-of-three-numbers-divisible-by-three/
- 题目 slug：`maximum-sum-of-three-numbers-divisible-by-three`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-sum-of-three-numbers-divisible-by-three/solutions/3861987/tong-yong-zuo-fa-er-wei-0-1-bei-bao-pyth-9gwu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[通用做法：二维 0-1 背包（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-sum-of-three-numbers-divisible-by-three/solutions/3861987/tong-yong-zuo-fa-er-wei-0-1-bei-bao-pyth-9gwu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tong-yong-zuo-fa-er-wei-0-1-bei-bao-pyth-9gwu`
- topic id：`3861987`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

把每个 $x = \textit{nums}[i]$ 看成一个体积为 $(1,x)$，价值为 $x$ 的物品。其中 $(1,x)$ 表示物品的第一类体积为 $1$，第二类体积为 $x$。

我们需要选择一些物品装入背包，使得的第一类体积之和恰好是 $3$，第二类体积之和恰好是 $3$ 的倍数。所以本题是一个「恰好装满型」二维 0-1 背包。

请先完成二维 0-1 背包的经典题目 [474. 一和零](https://leetcode.cn/problems/ones-and-zeroes/)，并阅读 [我的题解](https://leetcode.cn/problems/ones-and-zeroes/solutions/3038333/yi-bu-bu-si-kao-cong-ji-yi-hua-sou-suo-d-lqio/)。

类似 474 题，定义 $f[i+1][j][r]$ 表示在下标 $[0,i]$ 中选元素，选了恰好 $j$ 个数，元素和模 $3$ 为 $r$ 时，所选元素之和的最大值。

讨论 $x = \textit{nums}[i]$ 选或不选：

- 不选 $x$，问题变成在下标 $[0,i-1]$ 中选元素，选了恰好 $j$ 个数，元素和模 $3$ 为 $r$ 时，所选元素之和的最大值，即 $f[i][j][r]$。
- 选 $x$，问题变成在下标 $[0,i-1]$ 中选元素，选了恰好 $j-1$ 个数，元素和模 $3$ 为 $(r-x)\bmod 3$ 时，所选元素之和的最大值，即 $f[i][j-1][(r-x)\bmod 3]$，再加上 $x$（选了 $x$，获得了 $x$ 的价值）。

二者取最大值，得

$$
f[i+1][j][r] = \max(f[i][j][r], f[i][j-1][(r-x)\bmod 3] + x)
$$

初始值 $f[0][0][0] = 0$，其余 $f[0][j][r]=-\infty$。一开始没有选数字，只有 $j=r=0$ 是合法的。

答案为 $f[n][3][0]$，表示从 $\textit{nums}$ 中选了恰好 $3$ 个数，元素和模 $3$ 为 $0$ 时，所选元素之和的最大值。

如果 $f[n][3][0] < 0$，则无解，返回 $0$。

代码实现时，第一个维度可以优化掉，原理见[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)。

本题涉及到中途取模，以及减法的取模，原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

## 写法一：查表法

```py [sol-Python3]
max = lambda a, b: b if b > a else a

class Solution:
    def maximumSum(self, nums: List[int]) -> int:
        K = 3
        MOD = 3
        f = [[-inf] * MOD for _ in range(K + 1)]
        f[0][0] = 0
        for x in nums:
            for j in range(K, 0, -1):
                for r in range(MOD):
                    f[j][r] = max(f[j][r], f[j - 1][(r - x) % MOD] + x)
        return max(f[K][0], 0)
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] nums) {
        final int K = 3;
        final int MOD = 3;
        int[][] f = new int[K + 1][MOD];
        for (int[] row : f) {
            Arrays.fill(row, Integer.MIN_VALUE);
        }
        f[0][0] = 0;
        for (int x : nums) {
            for (int j = K; j > 0; j--) {
                for (int r = 0; r < MOD; r++) {
                    f[j][r] = Math.max(f[j][r], f[j - 1][(r - x % MOD + MOD) % MOD] + x); // 保证取模结果非负
                }
            }
        }
        return Math.max(f[K][0], 0);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& nums) {
        constexpr int K = 3;
        constexpr int MOD = 3;
        vector f(K + 1, vector<int>(MOD, INT_MIN));
        f[0][0] = 0;
        for (int x : nums) {
            for (int j = K; j > 0; j--) {
                for (int r = 0; r < MOD; r++) {
                    f[j][r] = max(f[j][r], f[j - 1][(r - x % MOD + MOD) % MOD] + x); // 保证取模结果非负
                }
            }
        }
        return max(f[K][0], 0);
    }
};
```

```go [sol-Go]
func maximumSum(nums []int) int {
	const K = 3
	const MOD = 3
	f := [K + 1][MOD]int{}
	for i := range f {
		for j := range f[i] {
			f[i][j] = math.MinInt
		}
	}
	f[0][0] = 0
	for _, x := range nums {
		for j := K; j > 0; j-- {
			for r := range MOD {
				f[j][r] = max(f[j][r], f[j-1][(r-x%MOD+MOD)%MOD]+x) // 保证取模结果非负
			}
		}
	}
	return max(f[K][0], 0)
}
```

## 写法二：刷表法

用当前状态 $f[i][j][r]$ 去更新其他状态 $f[i+1][j+1][(r+x)\bmod 3]$，即为刷表法。

```py [sol-Python3]
max = lambda a, b: b if b > a else a

class Solution:
    def maximumSum(self, nums: List[int]) -> int:
        K = 3
        MOD = 3
        f = [[-inf] * MOD for _ in range(K + 1)]
        f[0][0] = 0
        for x in nums:
            for j in range(K - 1, -1, -1):
                for r in range(MOD):
                    f[j + 1][(r + x) % MOD] = max(f[j + 1][(r + x) % MOD], f[j][r] + x)
        return max(f[K][0], 0)
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] nums) {
        final int K = 3;
        final int MOD = 3;
        int[][] f = new int[K + 1][MOD];
        for (int[] row : f) {
            Arrays.fill(row, Integer.MIN_VALUE);
        }
        f[0][0] = 0;
        for (int x : nums) {
            for (int j = K - 1; j >= 0; j--) {
                for (int r = 0; r < MOD; r++) {
                    f[j + 1][(r + x) % MOD] = Math.max(f[j + 1][(r + x) % MOD], f[j][r] + x);
                }
            }
        }
        return Math.max(f[K][0], 0);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& nums) {
        constexpr int K = 3;
        constexpr int MOD = 3;
        vector f(K + 1, vector<int>(MOD, INT_MIN));
        f[0][0] = 0;
        for (int x : nums) {
            for (int j = K - 1; j >= 0; j--) {
                for (int r = 0; r < MOD; r++) {
                    f[j + 1][(r + x) % MOD] = max(f[j + 1][(r + x) % MOD], f[j][r] + x);
                }
            }
        }
        return max(f[K][0], 0);
    }
};
```

```go [sol-Go]
func maximumSum(nums []int) int {
	const K = 3
	const MOD = 3
	f := [K + 1][MOD]int{}
	for i := range f {
		for j := range f[i] {
			f[i][j] = math.MinInt
		}
	}
	f[0][0] = 0
	for _, x := range nums {
		for j := K - 1; j >= 0; j-- {
			for r := range MOD {
				f[j+1][(r+x)%MOD] = max(f[j+1][(r+x)%MOD], f[j][r]+x)
			}
		}
	}
	return max(f[K][0], 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nKM)$，其中 $n$ 是 $\textit{nums}$ 的长度，$K=3$ 是目标子序列的长度，$M=3$ 是模数（目标子序列之和的因子）。
- 空间复杂度：$\mathcal{O}(KM)$。

## 相似题目

- [474. 一和零](https://leetcode.cn/problems/ones-and-zeroes/)
- [1262. 可被三整除的最大和](https://leetcode.cn/problems/greatest-sum-divisible-by-three/)

## 专题训练

见下面动态规划题单的「**§3.1 0-1 背包**」和「**§7.6 多维 DP**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.6 多维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.6 多维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
