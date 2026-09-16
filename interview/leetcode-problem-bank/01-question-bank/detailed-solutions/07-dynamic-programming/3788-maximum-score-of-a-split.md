# 3788. 分割的最大得分

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-score-of-a-split/
- 题目 slug：`maximum-score-of-a-split`
- 来源专题：动态规划
- 来源分类路径：专题：前后缀分解
- 难度分：1306
- 外部题解来源：https://leetcode.cn/problems/maximum-score-of-a-split/solutions/3867597/qian-hou-zhui-fen-jie-o1-kong-jian-you-h-ytf3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前后缀分解，O(1) 空间优化（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-score-of-a-split/solutions/3867597/qian-hou-zhui-fen-jie-o1-kong-jian-you-h-ytf3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-hou-zhui-fen-jie-o1-kong-jian-you-h-ytf3`
- topic id：`3867597`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

问题让我们计算 $\textit{nums}$ 的 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/) 以及后缀最小值，这都可以通过遍历一遍 $\textit{nums}$ 求出。

然后枚举分割位置，计算得分，更新答案的最大值。

注意保证分割后，两部分都至少有一个数。

[本题视频讲解](https://www.bilibili.com/video/BV1Y5vrBoEuQ/)，欢迎点赞关注~

## 优化前

```py [sol-Python3]
class Solution:
    def maximumScore(self, nums: List[int]) -> int:
        n = len(nums)
        suf_min = [0] * n  # 后缀最小值
        suf_min[-1] = nums[-1]
        for i in range(n - 2, -1, -1):
            suf_min[i] = min(suf_min[i + 1], nums[i])

        ans = -inf
        pre_sum = 0  # 前缀和
        for i in range(n - 1):  # 保证后缀至少有一个数
            pre_sum += nums[i]
            ans = max(ans, pre_sum - suf_min[i + 1])
        return ans
```

```java [sol-Java]
class Solution {
    public long maximumScore(int[] nums) {
        int n = nums.length;
        int[] sufMin = new int[n]; // 后缀最小值
        sufMin[n - 1] = nums[n - 1];
        for (int i = n - 2; i >= 0; i--) {
            sufMin[i] = Math.min(sufMin[i + 1], nums[i]);
        }

        long ans = Long.MIN_VALUE;
        long preSum = 0; // 前缀和
        for (int i = 0; i < n - 1; i++) {
            preSum += nums[i];
            ans = Math.max(ans, preSum - sufMin[i + 1]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maximumScore(vector<int>& nums) {
        int n = nums.size();
        vector<int> suf_min(n); // 后缀最小值
        suf_min[n - 1] = nums[n - 1];
        for (int i = n - 2; i >= 0; i--) {
            suf_min[i] = min(suf_min[i + 1], nums[i]);
        }

        long long ans = LLONG_MIN;
        long long pre_sum = 0; // 前缀和
        for (int i = 0; i < n - 1; i++) {
            pre_sum += nums[i];
            ans = max(ans, pre_sum - suf_min[i + 1]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumScore(nums []int) int64 {
	n := len(nums)
	sufMin := make([]int, n) // 后缀最小值
	sufMin[n-1] = nums[n-1]
	for i := n - 2; i >= 0; i-- {
		sufMin[i] = min(sufMin[i+1], nums[i])
	}

	ans := math.MinInt
	preSum := 0 // 前缀和
	for i, x := range nums[:n-1] {
		preSum += x
		ans = max(ans, preSum-sufMin[i+1])
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 优化

计算 $\min$ 不可逆，但计算加法可逆（减法）。

我们可以先计算整个数组的总和，再倒着遍历 $\textit{nums}$，在计算后缀最小值的同时，从总和中减去 $\textit{nums}[i]$，就得到了前缀和。

这样写无需创建 $\textit{sufMin}$ 数组。

```py [sol-Python3]
class Solution:
    def maximumScore(self, nums: List[int]) -> int:
        ans = -inf
        pre_sum = sum(nums)
        suf_min = inf
        for i in range(len(nums) - 1, 0, -1):  # 保证前缀至少有一个数
            pre_sum -= nums[i]  # 撤销
            suf_min = min(suf_min, nums[i])
            ans = max(ans, pre_sum - suf_min)
        return ans
```

```java [sol-Java]
class Solution {
    public long maximumScore(int[] nums) {
        long preSum = 0;
        for (int x : nums) {
            preSum += x;
        }

        long ans = Long.MIN_VALUE;
        int sufMin = Integer.MAX_VALUE;
        for (int i = nums.length - 1; i > 0; i--) { // 保证前缀至少有一个数
            preSum -= nums[i]; // 撤销
            sufMin = Math.min(sufMin, nums[i]);
            ans = Math.max(ans, preSum - sufMin);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maximumScore(vector<int>& nums) {
        long long ans = LLONG_MIN;
        long long pre_sum = reduce(nums.begin(), nums.end(), 0LL);
        int suf_min = INT_MAX;
        for (int i = nums.size() - 1; i > 0; i--) { // 保证前缀至少有一个数
            pre_sum -= nums[i]; // 撤销
            suf_min = min(suf_min, nums[i]);
            ans = max(ans, pre_sum - suf_min);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumScore(nums []int) int64 {
	preSum := 0
	for _, x := range nums {
		preSum += x
	}

	ans := math.MinInt
	sufMin := math.MaxInt
	for i := len(nums) - 1; i > 0; i-- { // 保证前缀至少有一个数
		preSum -= nums[i] // 撤销
		sufMin = min(sufMin, nums[i])
		ans = max(ans, preSum-sufMin)
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面动态规划题单的「**专题：前后缀分解**」。

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

本题来自 `专题：前后缀分解`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `专题：前后缀分解`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
