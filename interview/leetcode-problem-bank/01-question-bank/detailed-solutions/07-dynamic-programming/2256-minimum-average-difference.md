# 2256. 最小平均差

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-average-difference/
- 题目 slug：`minimum-average-difference`
- 来源专题：动态规划
- 来源分类路径：专题：前后缀分解
- 难度分：1395
- 外部题解来源：https://leetcode.cn/problems/minimum-average-difference/solutions/1460905/qian-hou-zhui-fen-jie-by-endlesscheng-a47e/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前后缀分解](https://leetcode.cn/problems/minimum-average-difference/solutions/1460905/qian-hou-zhui-fen-jie-by-endlesscheng-a47e/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-hou-zhui-fen-jie-by-endlesscheng-a47e`
- topic id：`1460905`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
func minimumAverageDifference(nums []int) (ans int) {
	pre, suf, n := 0, 0, len(nums)
	for _, v := range nums { suf += v } // 后缀和
	minDiff := math.MaxInt64
	for i, v := range nums[:n-1] {
		pre += v // 前缀和
		suf -= v // 后缀和
		d := abs(pre/(i+1) - suf/(n-1-i))
		if d < minDiff {
			minDiff, ans = d, i
		}
	}
	if (pre+nums[n-1])/n < minDiff { ans = n - 1 }
	return ans
}

func abs(x int) int { if x < 0 { return -x }; return x }
```
