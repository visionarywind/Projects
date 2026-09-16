# 3685. 含上限元素的子序列和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/subsequence-sum-after-capping-elements/
- 题目 slug：`subsequence-sum-after-capping-elements`
- 来源专题：动态规划
- 来源分类路径：三、背包 / §3.1 0-1 背包
- 难度分：2073
- 外部题解来源：https://leetcode.cn/problems/subsequence-sum-after-capping-elements/solutions/3781344/0-1-bei-bao-shuang-zhi-zhen-pythonjavacg-j4ca/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[0-1 背包 + 双指针，附 bitset 优化（Python/Java/C++/Go）](https://leetcode.cn/problems/subsequence-sum-after-capping-elements/solutions/3781344/0-1-bei-bao-shuang-zhi-zhen-pythonjavacg-j4ca/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`0-1-bei-bao-shuang-zhi-zhen-pythonjavacg-j4ca`
- topic id：`3781344`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

示例 1 的 $\textit{nums}=[4,3,2,4]$，$k=5$。

来看看 $x=3$ 时，问题相当于什么。

- 对于 $\le 3$ 的数，考虑**选或不选**。
- 对于 $> 3$ 的数，考虑**枚举选多少个**。这些数都替换成了 $3$，相当于枚举选多少个 $3$。

当 $x=3$ 时，我们可以枚举选 $0,1$ 个 $3$（不能选 $2$ 个 $3$，超过 $k=5$ 了）：

- 从大于 $3$ 的数中选 $0$ 个，问题变成：从 $\le 3$ 的数，即从 $[3,2]$ 中选择一些数，元素和能否恰好等于 $k=5$？
- 从大于 $3$ 的数中选 $1$ 个，将其替换为 $3$，问题变成：从 $\le 3$ 的数，即从 $[3,2]$ 中选择一些数，元素和能否恰好等于 $k-3=2$？

这是标准的 **0-1 背包**，原理见[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)，包含**为什么要倒序循环**的讲解。

如果对于每个 $x$，都重新算一遍 $\mathcal{O}(nk)$ 的 0-1 背包，时间复杂度为 $\mathcal{O}(n^2k)$，太慢了。

我们可以把 $\textit{nums}$ 从小到大排序，然后写一个双指针，外层循环枚举 $x=1,2,3,\dots,n$，内层循环在 $[1,x-1]$ 的基础上，**增量地**考虑所有满足 $\textit{nums}[i] = x$ 的 $\textit{nums}[i]$ 的 0-1 背包。

然后（从大于 $x$ 的数中）**枚举**选 $j$ 个 $x$。

由于剩下的大于 $x$ 的数有 $n-i$ 个，且必须满足 $k-jx\ge 0$，所以 $j$ 至多枚举到

$$
\min\left(n-i,\left\lfloor\dfrac{k}{x}\right\rfloor\right)
$$

如果我们可以从 $\le x$ 的数中得到元素和 $k-jx$，那么 $\textit{ans}[x-1] = \texttt{true}$。（注意 $\textit{ans}$ 的下标从 $0$ 开始）

[本题视频讲解](https://www.bilibili.com/video/BV1TBpczdE8P/?t=3m48s)，包含 bitset 优化的讲解，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def subsequenceSumAfterCapping(self, nums: List[int], k: int) -> List[bool]:
        nums.sort()

        n = len(nums)
        ans = [False] * n
        f = [False] * (k + 1)
        f[0] = True  # 不选元素，和为 0

        i = 0
        for x in range(1, n + 1):
            # 增量地考虑所有恰好等于 x 的数
            # 小于 x 的数在之前的循环中已计算完毕，无需重复计算
            while i < n and nums[i] == x:
                for j in range(k, nums[i] - 1, -1):
                    f[j] = f[j] or f[j - nums[i]]  # 0-1 背包：不选 or 选
                i += 1

            # 枚举（从大于 x 的数中）选了 j 个 x
            for j in range(min(n - i, k // x) + 1):
                if f[k - j * x]:
                    ans[x - 1] = True
                    break
        return ans
```

```java [sol-Java]
class Solution {
    public boolean[] subsequenceSumAfterCapping(int[] nums, int k) {
        Arrays.sort(nums);

        int n = nums.length;
        boolean[] ans = new boolean[n];
        boolean[] f = new boolean[k + 1];
        f[0] = true; // 不选元素，和为 0

        int i = 0;
        for (int x = 1; x <= n; x++) {
            // 增量地考虑所有恰好等于 x 的数
            // 小于 x 的数在之前的循环中已计算完毕，无需重复计算
            while (i < n && nums[i] == x) {
                for (int j = k; j >= nums[i]; j--) {
                    f[j] = f[j] || f[j - nums[i]]; // 0-1 背包：不选 or 选
                }
                i++;
            }

            // 枚举（从大于 x 的数中）选了 j 个 x
            for (int j = 0; j <= Math.min(n - i, k / x); j++) {
                if (f[k - j * x]) {
                    ans[x - 1] = true;
                    break;
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<bool> subsequenceSumAfterCapping(vector<int>& nums, int k) {
        ranges::sort(nums);

        int n = nums.size();
        vector<bool> ans(n);
        vector<int8_t> f(k + 1);
        f[0] = true; // 不选元素，和为 0

        int i = 0;
        for (int x = 1; x <= n; x++) {
            // 增量地考虑所有恰好等于 x 的数
            // 小于 x 的数在之前的循环中已计算完毕，无需重复计算
            while (i < n && nums[i] == x) {
                for (int j = k; j >= nums[i]; j--) {
                    f[j] = f[j] || f[j - nums[i]]; // 0-1 背包：不选 or 选
                }
                i++;
            }

            // 枚举（从大于 x 的数中）选了 j 个 x
            for (int j = 0; j <= min(n - i, k / x); j++) {
                if (f[k - j * x]) {
                    ans[x - 1] = true;
                    break;
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func subsequenceSumAfterCapping(nums []int, k int) []bool {
	slices.Sort(nums)

	n := len(nums)
	ans := make([]bool, n)
	f := make([]bool, k+1)
	f[0] = true // 不选元素，和为 0

	i := 0
	for x := 1; x <= n; x++ {
		// 增量地考虑所有恰好等于 x 的数
		// 小于 x 的数在之前的循环中已计算完毕，无需重复计算
		for i < n && nums[i] == x {
			for j := k; j >= nums[i]; j-- {
				f[j] = f[j] || f[j-nums[i]] // 0-1 背包：不选 or 选
			}
			i++
		}

		// 枚举（从大于 x 的数中）选了 j 个 x
		for j := range min(n-i, k/x) + 1 {
			if f[k-j*x] {
				ans[x-1] = true
				break
			}
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk + n\log n + \min(n^2,k\log n))$，其中 $n$ 是 $\textit{nums}$ 的长度。计算 0-1 背包的双指针是 $\mathcal{O}(nk)$。排序是 $\mathcal{O}(n\log n)$。枚举 $j$ 的循环，如果 $k$ 很大，循环次数是 $\mathcal{O}(n^2)$；如果 $n$ 很大，循环次数是 $\dfrac{k}{1} + \dfrac{k}{2} + \dots + \dfrac{k}{n}$，由调和级数可知，循环次数为 $\mathcal{O}(k\log n)$，二者取最小值。
- 空间复杂度：$\mathcal{O}(k)$。忽略排序的栈开销。返回值不计入。

## 附：bitset 优化

把布尔数组用二进制表示，用位运算加速状态转移。

```py [sol-Python3]
class Solution:
    def subsequenceSumAfterCapping(self, nums: List[int], k: int) -> List[bool]:
        nums.sort()

        n = len(nums)
        ans = [False] * n
        f = 1
        u = (1 << (k + 1)) - 1

        i = 0
        for x in range(1, n + 1):
            # 增量地考虑所有恰好等于 x 的数
            while i < n and nums[i] == x:
                f |= (f << nums[i]) & u  # 保证 f 的二进制长度 <= k+1
                i += 1

            if f >> k & 1:  # 等价于优化前的 f[k]
                ans[x - 1:] = [True] * (n - x + 1)  # 后面都是 True
                break

            # 枚举（从大于 x 的数中）选了 j 个 x
            for j in range(min(n - i, k // x) + 1):
                if f >> (k - j * x) & 1:  # 等价于优化前的 f[k - j * x]
                    ans[x - 1] = True
                    break
        return ans
```

```java [sol-Java]
class Solution {
    public boolean[] subsequenceSumAfterCapping(int[] nums, int k) {
        Arrays.sort(nums);

        int n = nums.length;
        boolean[] ans = new boolean[n];
        BigInteger f = BigInteger.ONE;
        BigInteger u = BigInteger.ONE.shiftLeft(k + 1).subtract(BigInteger.ONE); // (1 << (k + 1)) - 1

        int i = 0;
        for (int x = 1; x <= n; x++) {
            // 增量地考虑所有恰好等于 x 的数
            while (i < n && nums[i] == x) {
                f = f.or(f.shiftLeft(nums[i])).and(u); // 保证 f 的二进制长度 <= k+1
                i++;
            }

            // 枚举（从大于 x 的数中）选了 j 个 x
            for (int j = 0; j <= Math.min(n - i, k / x); j++) {
                if (f.testBit(k - j * x)) {
                    ans[x - 1] = true;
                    break;
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<bool> subsequenceSumAfterCapping(vector<int>& nums, int k) {
        ranges::sort(nums);

        int n = nums.size();
        vector<bool> ans(n);
        bitset<4001> f = 1;

        int i = 0;
        for (int x = 1; x <= n; x++) {
            // 增量地考虑所有恰好等于 x 的数
            while (i < n && nums[i] == x) {
                f |= f << nums[i];
                i++;
            }

            // 枚举（从大于 x 的数中）选了 j 个 x
            for (int j = 0; j <= min(n - i, k / x); j++) {
                if (f[k - j * x]) {
                    ans[x - 1] = true;
                    break;
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func subsequenceSumAfterCapping(nums []int, k int) []bool {
	slices.Sort(nums)

	n := len(nums)
	ans := make([]bool, n)
	f := big.NewInt(1)
	u := new(big.Int).Lsh(big.NewInt(1), uint(k+1))
	u.Sub(u, big.NewInt(1))

	i := 0
	for x := 1; x <= n; x++ {
		// 增量地考虑所有恰好等于 x 的数
		for i < n && nums[i] == x {
			shifted := new(big.Int).Lsh(f, uint(nums[i]))
			f.Or(f, shifted).And(f, u) // And(f, u) 保证 f 的二进制长度 <= k+1
			i++
		}

		// 枚举（从大于 x 的数中）选了 j 个 x
		for j := range min(n-i, k/x) + 1 {
			if f.Bit(k-j*x) > 0 {
				ans[x-1] = true
				break
			}
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk/w + n\log n + \min(n^2,k\log n))$，其中 $n$ 是 $\textit{nums}$ 的长度，$w=32$ 或 $64$。计算 0-1 背包的双指针是 $\mathcal{O}(nk/w)$。排序是 $\mathcal{O}(n\log n)$。枚举 $j$ 的循环，如果 $k$ 很大，循环次数是 $\mathcal{O}(n^2)$；如果 $n$ 很大，循环次数是 $\dfrac{k}{1} + \dfrac{k}{2} + \dots + \dfrac{k}{n}$，由调和级数可知，循环次数为 $\mathcal{O}(k\log n)$，二者取最小值。
- 空间复杂度：$\mathcal{O}(k/w)$。忽略排序的栈开销。返回值不计入。

## 专题训练

见下面动态规划题单的「**§3.1 0-1 背包**」。

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
