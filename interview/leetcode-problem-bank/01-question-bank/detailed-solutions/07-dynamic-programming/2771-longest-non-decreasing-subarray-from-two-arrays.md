# 2771. 构造最长非递减子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-non-decreasing-subarray-from-two-arrays/
- 题目 slug：`longest-non-decreasing-subarray-from-two-arrays`
- 来源专题：动态规划
- 来源分类路径：六、状态机 DP / §6.3 进阶
- 难度分：1792
- 外部题解来源：https://leetcode.cn/problems/longest-non-decreasing-subarray-from-two-arrays/solutions/2336749/dong-tai-gui-hua-cong-ji-yi-hua-sou-suo-hxalb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划：从记忆化搜索到递推，再到空间优化](https://leetcode.cn/problems/longest-non-decreasing-subarray-from-two-arrays/solutions/2336749/dong-tai-gui-hua-cong-ji-yi-hua-sou-suo-hxalb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-cong-ji-yi-hua-sou-suo-hxalb`
- topic id：`2336749`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识：动态规划入门

详见 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)

## 思路

为方便后面翻译成递推，这里从右往左递归。

定义 $\textit{dfs}(i,j)$ 表示以 $\textit{nums}_j[i]$ 结尾的最长非递减子数组的长度。

用「枚举选哪个」来思考：

- 如果 $\textit{nums}_1[i-1]\le \textit{nums}_j[i]$，那么下一步选 $\textit{nums}_1[i-1]$，有 $\textit{dfs}(i,j) = \textit{dfs}(i-1,0)+1$。
- 如果 $\textit{nums}_2[i-1]\le \textit{nums}_j[i]$，那么下一步选 $\textit{nums}_2[i-1]$，有 $\textit{dfs}(i,j) = \textit{dfs}(i-1,1)+1$。
- 如果都不成立，那么 $\textit{dfs}(i,j)=1$。

这几种情况取最大值，即为 $\textit{dfs}(i,j)$。

递归边界：$\textit{dfs}(0)=1$。

递归入口：$\textit{dfs}(i,j)$。遍历所有 $i,j$ 取 $\textit{dfs}(i,j)$ 的最大值，即为答案。

[视频讲解](https://www.bilibili.com/video/BV1XW4y1f7Wv/) 第三题。

```py [sol-Python3]
class Solution:
    def maxNonDecreasingLength(self, nums1: List[int], nums2: List[int]) -> int:
        nums = (nums1, nums2)
        @cache
        def dfs(i: int, j: int) -> int:
            if i == 0: return 1
            res = 1
            if nums1[i - 1] <= nums[j][i]:
                res = dfs(i - 1, 0) + 1
            if nums2[i - 1] <= nums[j][i]:
                res = max(res, dfs(i - 1, 1) + 1)
            return res
        return max(dfs(i, j) for j in range(2) for i in range(len(nums1)))
```

```go [sol-Go]
func maxNonDecreasingLength(nums1, nums2 []int) (ans int) {
	n := len(nums1)
	nums := [2][]int{nums1, nums2}
	memo := make([][2]int, n)
	for i := range memo {
		memo[i] = [2]int{-1, -1} // -1 表示没有计算过
	}
	var dfs func(int, int) int
	dfs = func(i, j int) int {
		if i == 0 {
			return 1
		}
		p := &memo[i][j]
		if *p != -1 { // 之前计算过
			return *p
		}
		res := 1
		if nums1[i-1] <= nums[j][i] {
			res = dfs(i-1, 0) + 1
		}
		if nums2[i-1] <= nums[j][i] {
			res = max(res, dfs(i-1, 1)+1)
		}
		*p = res // 记忆化
		return res
	}
	for j := 0; j < 2; j++ {
		for i := 0; i < n; i++ {
			ans = max(ans, dfs(i, j))
		}
	}
	return
}
```

然后按照 [视频](https://www.bilibili.com/video/BV1Xj411K7oF/) 中讲的，1:1 翻译成递推。

```py [sol-Python3]
class Solution:
    def maxNonDecreasingLength(self, nums1: List[int], nums2: List[int]) -> int:
        n = len(nums1)
        nums = (nums1, nums2)
        f = [[1, 1] for _ in range(n)]
        for i in range(1, n):
            for j in range(2):
                if nums1[i - 1] <= nums[j][i]:
                    f[i][j] = f[i - 1][0] + 1
                if nums2[i - 1] <= nums[j][i]:
                    f[i][j] = max(f[i][j], f[i - 1][1] + 1)
        return max(map(max, f))
```

```go [sol-Go]
func maxNonDecreasingLength(nums1, nums2 []int) int {
	ans, n := 1, len(nums1)
	nums := [2][]int{nums1, nums2}
	f := make([][2]int, n)
	f[0] = [2]int{1, 1}
	for i := 1; i < n; i++ {
		f[i] = [2]int{1, 1}
		for j := 0; j < 2; j++ {
			if nums1[i-1] <= nums[j][i] {
				f[i][j] = f[i-1][0] + 1
			}
			if nums2[i-1] <= nums[j][i] {
				f[i][j] = max(f[i][j], f[i-1][1]+1)
			}
		}
		ans = max(ans, max(f[i][0], f[i][1]))
	}
	return ans
}
```

由于 $f[i]$ 只用到 $f[i-1]$，所以可以去掉第一个维度。

```py [sol-Python3]
class Solution:
    def maxNonDecreasingLength(self, nums1: List[int], nums2: List[int]) -> int:
        ans = f0 = f1 = 1
        for (x0, y0), (x1, y1) in pairwise(zip(nums1, nums2)):
            f = g = 1
            if x0 <= x1: f = f0 + 1
            if y0 <= x1: f = max(f, f1 + 1)
            if x0 <= y1: g = f0 + 1
            if y0 <= y1: g = max(g, f1 + 1)
            f0, f1 = f, g
            ans = max(ans, f0, f1)
        return ans
```

```go [sol-Go]
func maxNonDecreasingLength(nums1, nums2 []int) int {
	ans, n := 1, len(nums1)
	f0, f1 := 1, 1
	for i := 1; i < n; i++ {
		f, g := 1, 1
		if nums1[i-1] <= nums1[i] {
			f = f0 + 1
		}
		if nums2[i-1] <= nums1[i] {
			f = max(f, f1+1)
		}
		if nums1[i-1] <= nums2[i] {
			g = f0 + 1
		}
		if nums2[i-1] <= nums2[i] {
			g = max(g, f1+1)
		}
		f0, f1 = f, g
		ans = max(ans, max(f0, f1))
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}_1$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 思考题

如果把「子数组」改成「子序列」呢？

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

本题来自 `六、状态机 DP / §6.3 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、状态机 DP / §6.3 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
