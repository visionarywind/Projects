# 2587. 重排数组以得到最大前缀分数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/rearrange-array-to-maximize-prefix-score/
- 题目 slug：`rearrange-array-to-maximize-prefix-score`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：1337
- 外部题解来源：https://leetcode.cn/problems/rearrange-array-to-maximize-prefix-score/solutions/2163137/pai-xu-tan-xin-by-endlesscheng-14ea/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序+贪心](https://leetcode.cn/problems/rearrange-array-to-maximize-prefix-score/solutions/2163137/pai-xu-tan-xin-by-endlesscheng-14ea/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-tan-xin-by-endlesscheng-14ea`
- topic id：`2163137`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

[视频讲解](https://www.bilibili.com/video/BV1d54y1M7Qg/) 第二题。

对于一个负数来说，它后面的前缀和都会把这个负数加进去。

由于要统计的是正数前缀和，那么把负数尽量放在后面，能统计到尽量多的正数前缀和。

同时，绝对值小的负数应该排在负数的前面，尽量在前缀和减为负数前还能多统计一些正数。

```py [sol1-Python3]
class Solution:
    def maxScore(self, nums: List[int]) -> int:
        nums.sort(reverse=True)
        return sum(s > 0 for s in accumulate(nums))
```

```go [sol1-Go]
func maxScore(nums []int) (ans int) {
	sort.Sort(sort.Reverse(sort.IntSlice(nums)))
	sum := 0
	for _, x := range nums {
		sum += x
		if sum <= 0 {
			break
		}
		ans++
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$O(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$O(1)$。忽略排序时的栈开销，仅用到若干额外变量。
