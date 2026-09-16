# 3584. 子序列首尾元素的最大乘积

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-product-of-first-and-last-elements-of-a-subsequence/
- 题目 slug：`maximum-product-of-first-and-last-elements-of-a-subsequence`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础
- 难度分：1763
- 外部题解来源：https://leetcode.cn/problems/maximum-product-of-first-and-last-elements-of-a-subsequence/solutions/3700555/nao-jin-ji-zhuan-wan-mei-ju-you-wei-hu-z-93zo/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯 + 枚举右维护左，附思考题（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-product-of-first-and-last-elements-of-a-subsequence/solutions/3700555/nao-jin-ji-zhuan-wan-mei-ju-you-wei-hu-z-93zo/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-mei-ju-you-wei-hu-z-93zo`
- topic id：`3700555`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

题目意思是首尾元素下标相差至少是 $m-1$，这样子序列长度才能是 $m$（否则长度就小于 $m$ 了）。

我们只需关注首尾两个数，于是问题转化成：

- $\textit{nums}$ 的任意下标相差至少为 $m-1$ 的**两数之积**的最大值。

枚举 $\textit{nums}[i]$，为了让乘积最大，贪心地，维护 $[0,i-m+1]$ 中的最小值和最大值。维护最小值是因为负负得正，答案可以来自两个负数相乘。

注意最终答案可能是负数，要把答案初始化成 $-\infty$。

注意无需特判 $m=1$ 的情况，此时答案来自 $\textit{nums}$ 的最大值与自己相乘，或者最小值与自己相乘，这一定可以被我们枚举到。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1qsMxz6EEd/?t=20m04s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maximumProduct(self, nums: List[int], m: int) -> int:
        ans = mx = -inf
        mn = inf
        for i in range(m - 1, len(nums)):
            # 维护左边 [0,i-m+1] 中的最小值和最大值
            y = nums[i - m + 1]
            mn = min(mn, y)
            mx = max(mx, y)
            # 枚举右
            x = nums[i]
            ans = max(ans, x * mn, x * mx)
        return ans
```

```java [sol-Java]
class Solution {
    public long maximumProduct(int[] nums, int m) {
        long ans = Long.MIN_VALUE;
        int mn = Integer.MAX_VALUE;
        int mx = Integer.MIN_VALUE;
        for (int i = m - 1; i < nums.length; i++) {
            // 维护左边 [0,i-m+1] 中的最小值和最大值
            int y = nums[i - m + 1];
            mn = Math.min(mn, y);
            mx = Math.max(mx, y);
            // 枚举右
            long x = nums[i];
            ans = Math.max(ans, Math.max(x * mn, x * mx));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maximumProduct(vector<int>& nums, int m) {
        long long ans = LLONG_MIN;
        int mn = INT_MAX, mx = INT_MIN;
        for (int i = m - 1; i < nums.size(); i++) {
            // 维护左边 [0,i-m+1] 中的最小值和最大值
            int y = nums[i - m + 1];
            mn = min(mn, y);
            mx = max(mx, y);
            // 枚举右
            long long x = nums[i];
            ans = max({ans, x * mn, x * mx});
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumProduct(nums []int, m int) int64 {
	ans := math.MinInt
	mn, mx := math.MaxInt, math.MinInt
	for i := m - 1; i < len(nums); i++ {
		// 维护左边 [0,i-m+1] 中的最小值和最大值
		y := nums[i-m+1]
		mn = min(mn, y)
		mx = max(mx, y)
		// 枚举右
		x := nums[i]
		ans = max(ans, x*mn, x*mx)
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n-m)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

1. 改成任意非空**子序列**的最大乘积。
2. 改成长为 $m$ 的**子序列**的最大乘积。
3. 改成长为 $m$ 的**子数组**的最大乘积。
4. 改成任意非空**子数组**的最大乘积。见 [152. 乘积最大子数组](https://leetcode.cn/problems/maximum-product-subarray/)。

欢迎在评论区发表你的思路/代码。

## 相似题目

见下面数据结构题单的「**§0.1 枚举右，维护左**」。

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

本题来自 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
