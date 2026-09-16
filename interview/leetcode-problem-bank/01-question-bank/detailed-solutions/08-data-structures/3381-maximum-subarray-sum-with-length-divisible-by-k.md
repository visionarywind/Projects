# 3381. 长度可被 K 整除的子数组的最大元素和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-subarray-sum-with-length-divisible-by-k/
- 题目 slug：`maximum-subarray-sum-with-length-divisible-by-k`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.2 前缀和与哈希表
- 难度分：1943
- 外部题解来源：https://leetcode.cn/problems/maximum-subarray-sum-with-length-divisible-by-k/solutions/3013897/qian-zhui-he-mei-ju-you-wei-hu-zuo-pytho-0t8p/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和+枚举右维护左（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-subarray-sum-with-length-divisible-by-k/solutions/3013897/qian-zhui-he-mei-ju-you-wei-hu-zuo-pytho-0t8p/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-mei-ju-you-wei-hu-zuo-pytho-0t8p`
- topic id：`3013897`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

子数组和问题，考虑**前缀和**。

计算 $\textit{nums}$ 的前缀和数组 $s$。关于 $s$ 数组的定义，请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

子数组 $[i,j)$ 的元素和为 $s[j]-s[i]$，长度为 $j-i$。

问题相当于：

- 计算最大的 $s[j]-s[i]$，满足 $i < j$ 且 $j-i$ 是 $k$ 的倍数。

> 注：限制 $i<j$ 是为了让子数组非空，符合题目要求。

枚举 $j$，要使 $s[j]-s[i]$ 尽量大，$s[i]$ 要尽量小。

要枚举 $i$ 吗？那样太慢了。

比如 $k=2$：

- 当 $j$ 是偶数时，比如 $j=6$，要使长度是 $k=2$ 的倍数，那么 $i$ 也必须是偶数 $0,2,4$。所以只需维护偶数下标的 $s[i]$ 的最小值，而不是遍历所有 $s[i]$。
- 当 $j$ 是奇数时，比如 $j=7$，要使长度是 $k=2$ 的倍数，那么 $i$ 也必须是奇数 $1,3,5$。所以只需维护奇数下标的 $s[i]$ 的最小值，而不是遍历所有 $s[i]$。

一般地，在遍历前缀和数组 $s$ 的同时，维护：

- 满足 $i < j$ 且 $i$ 与 $j$ 关于模 $k$ **同余**的 $s[i]$ 的最小值。

关于同余的概念，请看 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

[本题视频讲解](https://www.bilibili.com/video/BV1YeqHYSEhK/?t=7m56s)，欢迎点赞关注~

## 优化前

```py [sol-Python3]
class Solution:
    def maxSubarraySum(self, nums: List[int], k: int) -> int:
        pre = list(accumulate(nums, initial=0))
        min_s = [inf] * k
        ans = -inf
        for j, s in enumerate(pre):
            i = j % k
            ans = max(ans, s - min_s[i])
            min_s[i] = min(min_s[i], s)
        return ans
```

```java [sol-Java]
class Solution {
    public long maxSubarraySum(int[] nums, int k) {
        int n = nums.length;
        long[] sum = new long[n + 1];
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i] + nums[i];
        }

        long[] minS = new long[k];
        Arrays.fill(minS, Long.MAX_VALUE / 2); // 防止下面减法溢出

        long ans = Long.MIN_VALUE;
        for (int j = 0; j < sum.length; j++) {
            int i = j % k;
            ans = Math.max(ans, sum[j] - minS[i]);
            minS[i] = Math.min(minS[i], sum[j]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxSubarraySum(vector<int>& nums, int k) {
        int n = nums.size();
        vector<long long> sum(n + 1);
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i] + nums[i];
        }

        vector<long long> min_s(k, LLONG_MAX / 2); // 防止下面减法溢出
        long long ans = LLONG_MIN;
        for (int j = 0; j < sum.size(); j++) {
            int i = j % k;
            ans = max(ans, sum[j] - min_s[i]);
            min_s[i] = min(min_s[i], sum[j]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxSubarraySum(nums []int, k int) int64 {
	sum := make([]int, len(nums)+1)
	for i, x := range nums {
		sum[i+1] = sum[i] + x
	}

	minS := make([]int, k)
	for i := range minS {
		minS[i] = math.MaxInt / 2 // 防止下面减法溢出
	}

	ans := math.MinInt
	for j, s := range sum {
		i := j % k
		ans = max(ans, s-minS[i])
		minS[i] = min(minS[i], s)
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 优化

一边计算前缀和，一边维护 $\textit{minS}$。

这里前缀和的下标从 $-1$ 开始，也就是定义 $s[-1] = 0$。由于 $-1$ 与 $k-1$ 模 $k$ 同余，所以初始化 $\textit{minS}[k-1] = 0$。

```py [sol-Python3]
class Solution:
    def maxSubarraySum(self, nums: List[int], k: int) -> int:
        min_s = [inf] * k
        min_s[-1] = s = 0
        ans = -inf
        for j, x in enumerate(nums):
            s += x
            i = j % k
            ans = max(ans, s - min_s[i])
            min_s[i] = min(min_s[i], s)
        return ans
```

```java [sol-Java]
class Solution {
    public long maxSubarraySum(int[] nums, int k) {
        long[] minS = new long[k];
        Arrays.fill(minS, 0, k - 1, Long.MAX_VALUE / 2); // 防止下面减法溢出

        long ans = Long.MIN_VALUE;
        long s = 0;
        for (int j = 0; j < nums.length; j++) {
            s += nums[j];
            int i = j % k;
            ans = Math.max(ans, s - minS[i]);
            minS[i] = Math.min(minS[i], s);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxSubarraySum(vector<int>& nums, int k) {
        vector<long long> min_s(k, LLONG_MAX / 2); // 防止下面减法溢出
        min_s.back() = 0;

        long long ans = LLONG_MIN, s = 0;
        for (int j = 0; j < nums.size(); j++) {
            s += nums[j];
            int i = j % k;
            ans = max(ans, s - min_s[i]);
            min_s[i] = min(min_s[i], s);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxSubarraySum(nums []int, k int) int64 {
	minS := make([]int, k)
	for i := range k - 1 {
		minS[i] = math.MaxInt / 2 // 防止下面减法溢出
	}

	ans := math.MinInt
	s := 0
	for j, x := range nums {
		s += x
		i := j % k
		ans = max(ans, s-minS[i])
		minS[i] = min(minS[i], s)
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(k)$。

## 专题训练

见下面数据结构题单的「**§1.2 前缀和与哈希表**」。

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

本题来自 `一、前缀和 / §1.2 前缀和与哈希表`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.2 前缀和与哈希表`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
