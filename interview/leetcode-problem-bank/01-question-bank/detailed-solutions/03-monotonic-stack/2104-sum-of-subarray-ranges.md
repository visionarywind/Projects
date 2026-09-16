# 2104. 子数组范围和（最大值-最小值）

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-subarray-ranges/
- 题目 slug：`sum-of-subarray-ranges`
- 来源专题：单调栈
- 来源分类路径：三、贡献法
- 难度分：2000
- 外部题解来源：https://leetcode.cn/problems/sum-of-subarray-ranges/solutions/1153054/cong-on2-dao-ondan-diao-zhan-ji-suan-mei-o1op/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从 O(n^2) 到 O(n)：单调栈+计算每个元素对答案的贡献](https://leetcode.cn/problems/sum-of-subarray-ranges/solutions/1153054/cong-on2-dao-ondan-diao-zhan-ji-suan-mei-o1op/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-on2-dao-ondan-diao-zhan-ji-suan-mei-o1op`
- topic id：`1153054`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：暴力枚举所有子数组

写一个二重循环，外层循环枚举子数组的左边界，内层循环枚举子数组的右边界，同时维护当前子数组的最小值和最大值。

```go 
func subArrayRanges(nums []int) (ans int64) {
	for i, num := range nums {
		min, max := num, num
		for _, v := range nums[i+1:] {
			if v < min {
				min = v
			} else if v > max {
				max = v
			}
			ans += int64(max - min)
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是数组 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$，我们只需要常数的空间保存若干变量。

## 方法二：单调栈 + 计算每个元素对答案的贡献

不了解单调栈的同学请看 [单调栈【基础算法精讲 26】](https://www.bilibili.com/video/BV1VN411J7S7/)。

我们可以考虑每个元素作为最大值出现在了多少子数组中，以及作为最小值出现在了多少子数组中。

以最大值为例。我们可以求出 $\textit{nums}[i]$ 左侧**严格大于**它的最近元素位置 $\textit{left}[i]$，以及右侧**大于等于**它的最近元素位置 $\textit{right}[i]$。注意 $\textit{nums}$ 中可能有重复元素，所以这里右侧取大于等于，这样可以避免在有重复元素的情况下，重复统计相同的子数组。

设以 $\textit{nums}[i]$ 为最大值的子数组为 $\textit{nums}[l..r]$，则有

- $\textit{left}[i]<l\le i$
- $i\le r<\textit{right}[i]$

所以 $\textit{nums}[i]$ 可以作为最大值出现在

$$
(i-\textit{left}[i])\cdot (\textit{right}[i]-i)
$$

个子数组中，这对答案产生的贡献是

$$
(i-\textit{left}[i])\cdot(\textit{right}[i]-i)\cdot \textit{nums}[i]
$$

最小值的做法同理（贡献为负数）。

累加所有贡献即为答案。

```go
func solve(nums []int) (ans int64) {
	n := len(nums)
	left := make([]int, n)  // left[i] 为左侧严格大于 num[i] 的最近元素位置（不存在时为 -1）
	right := make([]int, n) // right[i] 为右侧大于等于 num[i] 的最近元素位置（不存在时为 n）
	for i := range right { right[i] = n }
	st := []int{-1}
	for i, v := range nums {
		for len(st) > 1 && nums[st[len(st)-1]] <= v {
			right[st[len(st)-1]] = i
			st = st[:len(st)-1]
		}
		left[i] = st[len(st)-1]
		st = append(st, i)
	}
	for i, v := range nums {
		ans += int64(i-left[i]) * int64(right[i]-i) * int64(v)
	}
	return
}

func subArrayRanges(nums []int) int64 {
	ans := solve(nums)
	for i, v := range nums { // 小技巧：所有元素取反后算的就是最小值的贡献
		nums[i] = -v
	}
	return ans + solve(nums)
}

```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是数组 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

把子数组改成**子序列**，要怎么做？

这题是 [891. 子序列宽度之和](https://leetcode.cn/problems/sum-of-subsequence-widths/)。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `三、贡献法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、贡献法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
