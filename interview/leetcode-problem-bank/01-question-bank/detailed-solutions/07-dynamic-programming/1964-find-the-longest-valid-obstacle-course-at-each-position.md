# 1964. 找出到每个位置为止最长的有效障碍赛跑路线

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-longest-valid-obstacle-course-at-each-position/
- 题目 slug：`find-the-longest-valid-obstacle-course-at-each-position`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.1 基础
- 难度分：1933
- 外部题解来源：https://leetcode.cn/problems/find-the-longest-valid-obstacle-course-at-each-position/solutions/922738/ji-suan-mei-ge-qian-zhui-de-zui-chang-sh-d2vd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[计算每个前缀的最长非降子序列的长度](https://leetcode.cn/problems/find-the-longest-valid-obstacle-course-at-each-position/solutions/922738/ji-suan-mei-ge-qian-zhui-de-zui-chang-sh-d2vd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-suan-mei-ge-qian-zhui-de-zui-chang-sh-d2vd`
- topic id：`922738`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

简化后的题意就是标题。

对于最长上升子序列，可以去 [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/) 了解，此处不再赘述。

相比最长**上升**子序列，对于**非降**的情况，需要二分大于该元素的第一个位置。

用二分得到的位置当作答案。

```go
func longestObstacleCourseAtEachPosition(obstacles []int) []int {
	ans := make([]int, len(obstacles))
	dp := []int{}
	for i, v := range obstacles {
		p := sort.SearchInts(dp, v+1) // 等价写法是二分不小于 v+1 的第一个位置
		if p < len(dp) {
			dp[p] = v
		} else {
			dp = append(dp, v)
		}
		ans[i] = p + 1
	}
	return ans
}
```
