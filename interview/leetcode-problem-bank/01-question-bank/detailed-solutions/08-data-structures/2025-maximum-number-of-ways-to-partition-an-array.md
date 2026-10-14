# 2025. 分割数组的最多方案数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-ways-to-partition-an-array/
- 题目 slug：`maximum-number-of-ways-to-partition-an-array`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.2 前缀和与哈希表
- 难度分：2218
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-ways-to-partition-an-array/solutions/1028621/qian-zhui-he-ha-xi-biao-mei-ju-xiu-gai-y-l546/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和+双哈希表+枚举修改元素](https://leetcode.cn/problems/maximum-number-of-ways-to-partition-an-array/solutions/1028621/qian-zhui-he-ha-xi-biao-mei-ju-xiu-gai-y-l546/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-ha-xi-biao-mei-ju-xiu-gai-y-l546`
- topic id：`1028621`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

计算出 $\textit{nums}$ 的前缀和 $\textit{sum}$，记所有元素的和为 $\textit{tot}$。

对于不修改的情况，合法分割相当于要满足 $\textit{sum}[i] = \textit{tot}-\textit{sum}[i]$，即 $\textit{sum}[i]=\dfrac{\textit{tot}}{2}$。

对于修改的情况，枚举修改的元素，记变化量 $d=k-\textit{nums}[i]$，这一修改操作对于 $i$ 左侧的前缀和是没有影响的，因此合法分割相当于要满足 $\textit{sum}[i] = \textit{tot}+d-\textit{sum}[i]$，即 $\textit{sum}[i]=\dfrac{\textit{tot}+d}{2}$；而对于 $i$ 右侧的前缀和，每个前缀和都增加了 $d$，因此合法分割相当于要满足 $\textit{sum}[i]+d = \textit{tot}+d-(\textit{sum}[i]+d)$，即 $\textit{sum}[i]=\dfrac{\textit{tot}-d}{2}$。

我们可以在枚举 $\textit{nums}[i]$ 的同时，用两个哈希表动态维护 $i$ 左右前缀和的个数，从而做到对每个 $\textit{nums}[i]$ 在 $O(1)$ 的时间计算出合法分割数，因此总的时间复杂度为 $O(n)$。

```go
func waysToPartition(nums []int, k int) (ans int) {
	n := len(nums)
	sum := make([]int, n)
	sum[0] = nums[0]
	cntR := map[int]int{}
	for i := 1; i < n; i++ {
		sum[i] = sum[i-1] + nums[i]
		cntR[sum[i-1]]++
	}
	tot := sum[n-1]
	if tot%2 == 0 {
		ans = cntR[tot/2] // 不修改
	}
	cntL := map[int]int{}
	for i, s := range sum {
		if d := k - nums[i]; (tot+d)%2 == 0 {
			ans = max(ans, cntL[(tot+d)/2]+cntR[(tot-d)/2]) // 修改 nums[i]
		}
		cntL[s]++
		cntR[s]--
	}
	return
}

func max(a, b int) int { if b > a { return b }; return a }
```
