# 3082. 求出所有子序列的能量和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-sum-of-the-power-of-all-subsequences/
- 题目 slug：`find-the-sum-of-the-power-of-all-subsequences`
- 来源专题：动态规划
- 来源分类路径：三、背包 / §3.1 0-1 背包
- 难度分：2242
- 外部题解来源：https://leetcode.cn/problems/find-the-sum-of-the-power-of-all-subsequences/solutions/2691661/liang-chong-fang-fa-er-wei-yi-wei-0-1-be-2e47/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：从 O(nnk) 到 O(nk)，附题单！（Python/Java/C++/Go）](https://leetcode.cn/problems/find-the-sum-of-the-power-of-all-subsequences/solutions/2691661/liang-chong-fang-fa-er-wei-yi-wei-0-1-be-2e47/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-er-wei-yi-wei-0-1-be-2e47`
- topic id：`2691661`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

请看 [视频讲解](https://www.bilibili.com/video/BV11x421r7q5/) 第四题。

## 方法一：二维 0-1 背包

#### 观察

如果和为 $k$ 的子序列 $S$ 的长度是 $c$，那么有多少个子序列 $T$，会包含 $S$ 呢？即 $S$ 是 $T$ 的子序列。

例如示例 1，子序列 $S=[3]$，出现在子序列 $[1,2,3],[1,3],[2,3],[3]$ 中，这 $4$ 个子序列都可以是 $T$。

除了 $3$ 以外的每个数，都可以选择在/不在包含 $[3]$ 的子序列 $T$ 中。

所以有 $2^{n-c}$ 个子序列 $T$。

这意味着 $S$ 对答案的贡献是 $2^{n-c}$。

#### 思路

枚举和为 $k$ 的子序列的长度 $c$，问题变成：

- 有 $n$ 个物品，每个物品的体积是 $\textit{nums}[i]$，问恰好装满容量为 $k$ 的背包，且选择 $c$ 个物品的方案数。

这可以用二维 0-1 背包解决。不了解 0-1 背包的同学请看[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)。

定义 $f[i][j][c]$ 表示考虑前 $i$ 个物品，所选物品体积和是 $j$，选了 $c$ 个物品的方案数。

考虑第 $i$ 个物品选或不选：

- 不选：$f[i+1][j][c] = f[i][j][c]$。
- 选：$f[i+1][j][c] = f[i][j-\textit{nums}[i]][c-1]$。

两者相加，得

$$
f[i+1][j][c] = f[i][j][c] + f[i][j-\textit{nums}[i]][c-1]
$$

初始值：$f[0][0][0] = 1$。

答案：$\sum\limits_{c=1}^{n} f[n][k][c] \cdot 2^{n-c}$。

代码实现时，第一个维度可以优化掉。

```py [sol-Python3]
class Solution:
    def sumOfPower(self, nums: List[int], k: int) -> int:
        MOD = 1_000_000_007
        n = len(nums)
        f = [[0] * (n + 1) for _ in range(k + 1)]
        f[0][0] = 1
        for i, x in enumerate(nums):
            for j in range(k, x - 1, -1):
                for c in range(i + 1, 0, -1):
                    f[j][c] = (f[j][c] + f[j - x][c - 1]) % MOD

        ans = 0
        pow2 = 1
        for i in range(n, 0, -1):
            ans = (ans + f[k][i] * pow2) % MOD
            pow2 = pow2 * 2 % MOD
        return ans
```

```java [sol-Java]
class Solution {
    public int sumOfPower(int[] nums, int k) {
        final int MOD = 1_000_000_007;
        int n = nums.length;
        int[][] f = new int[k + 1][n + 1];
        f[0][0] = 1;
        for (int i = 0; i < n; i++) {
            for (int j = k; j >= nums[i]; j--) {
                for (int c = i + 1; c > 0; c--) {
                    f[j][c] = (f[j][c] + f[j - nums[i]][c - 1]) % MOD;
                }
            }
        }

        long ans = 0;
        int pow2 = 1;
        for (int i = n; i > 0; i--) {
            ans = (ans + (long) f[k][i] * pow2) % MOD;
            pow2 = pow2 * 2 % MOD;
        }
        return (int) ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumOfPower(vector<int> &nums, int k) {
        const int MOD = 1'000'000'007;
        int n = nums.size();
        vector<vector<int>> f(k + 1, vector<int>(n + 1));
        f[0][0] = 1;
        for (int i = 0; i < n; i++) {
            for (int j = k; j >= nums[i]; j--) {
                for (int c = i + 1; c > 0; c--) {
                    f[j][c] = (f[j][c] + f[j - nums[i]][c - 1]) % MOD;
                }
            }
        }

        int ans = 0;
        int pow2 = 1;
        for (int i = n; i > 0; i--) {
            ans = (ans + (long long) f[k][i] * pow2) % MOD;
            pow2 = pow2 * 2 % MOD;
        }
        return ans;
    }
};
```

```go [sol-Go]
func sumOfPower(nums []int, k int) (ans int) {
	const mod = 1_000_000_007
	n := len(nums)
	f := make([][]int, k+1)
	for i := range f {
		f[i] = make([]int, n+1)
	}
	f[0][0] = 1
	for i, x := range nums {
		for j := k; j >= x; j-- {
			for c := i + 1; c > 0; c-- {
				f[j][c] = (f[j][c] + f[j-x][c-1]) % mod
			}
		}
	}
	pow2 := 1
	for i := n; i > 0; i-- {
		ans = (ans + f[k][i]*pow2) % mod
		pow2 = pow2 * 2 % mod
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2k)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(nk)$。

## 方法二：一维 0-1 背包

定义 $f[i+1][j]$ 表示在 $\textit{nums}[0]$ 到 $\textit{nums}[i]$ 中选子序列，子序列中的元素和恰好等于 $j$ 的子序列个数。

对于 $\textit{nums}[i]$，它只有三种可能：

- 不在子序列 $S$，也不在子序列 $T$ 中：$f[i+1][j] = f[i][j]$。
- 不在子序列 $S$，在子序列 $T$ 中：$f[i+1][j] = f[i][j]$。
- 在子序列 $S$ 中：$f[i+1][j] = f[i][j-\textit{nums}[i]]$。

三者相加，得

$$
f[i+1][j] = f[i][j]\cdot 2 + f[i][j-\textit{nums}[i]]
$$

初始值：$f[0][0] = 1$。

答案：$f[n][k]$。

代码实现时，第一个维度可以优化掉。

```py [sol-Python3]
class Solution:
    def sumOfPower(self, nums: List[int], k: int) -> int:
        f = [1] + [0] * k
        for x in nums:
            for j in range(k, -1, -1):
                f[j] = (f[j] * 2 + (f[j - x] if j >= x else 0)) % 1_000_000_007
        return f[k]
```

```java [sol-Java]
class Solution {
    public int sumOfPower(int[] nums, int k) {
        long[] f = new long[k + 1];
        f[0] = 1;
        for (int x : nums) {
            for (int j = k; j >= 0; j--) {
                f[j] = (f[j] * 2 + (j >= x ? f[j - x] : 0)) % 1_000_000_007;
            }
        }
        return (int) f[k];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int sumOfPower(vector<int> &nums, int k) {
        vector<long long> f(k + 1);
        f[0] = 1;
        for (int x : nums) {
            for (int j = k; j >= 0; j--) {
                f[j] = (f[j] * 2 + (j >= x ? f[j - x] : 0)) % 1'000'000'007;
            }
        }
        return f[k];
    }
};
```

```go [sol-Go]
func sumOfPower(nums []int, k int) int {
	const mod = 1_000_000_007
	f := make([]int, k+1)
	f[0] = 1
	for _, x := range nums {
		for j := k; j >= 0; j-- {
			if j >= x {
				f[j] = (f[j]*2 + f[j-x]) % mod
			} else {
				f[j] = f[j] * 2 % mod
			}
		}
	}
	return f[k]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(k)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. 【本题相关】[动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `三、背包 / §3.1 0-1 背包`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、背包 / §3.1 0-1 背包`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
