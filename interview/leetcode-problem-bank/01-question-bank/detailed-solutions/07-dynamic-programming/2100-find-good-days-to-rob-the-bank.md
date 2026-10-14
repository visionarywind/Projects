# 2100. 适合野炊的日子

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-good-days-to-rob-the-bank/
- 题目 slug：`find-good-days-to-rob-the-bank`
- 来源专题：动态规划
- 来源分类路径：专题：前后缀分解
- 难度分：1702
- 外部题解来源：https://leetcode.cn/problems/find-good-days-to-rob-the-bank/solutions/1152456/liang-ci-bian-li-qian-hou-zhui-by-endles-pq67/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两次遍历 + 前后缀分解](https://leetcode.cn/problems/find-good-days-to-rob-the-bank/solutions/1152456/liang-ci-bian-li-qian-hou-zhui-by-endles-pq67/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-ci-bian-li-qian-hou-zhui-by-endles-pq67`
- topic id：`1152456`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
func goodDaysToRobBank(security []int, time int) (ans []int) {
	n := len(security)
	if time*2 >= n { // i 最小为 time，而且需要满足 i+time<n，即 time*2<n，不满足则直接返回空数组
		return
	}

	suf := make([]int, n) // suf[i] 表示从 security[i+1] 向后的最长连续非递减长度
	for i := n - 2; i >= time; i-- {
		if security[i] <= security[i+1] {
			suf[i] = suf[i+1] + 1
		}
	}

	pre := 0 // pre 表示从 security[i-1] 向前的最长连续非递增长度
	for i, v := range security[:n-time] {
		if i > 0 && v <= security[i-1] {
			pre++
		} else {
			pre = 0
		}
		if pre >= time && suf[i] >= time {
			ans = append(ans, i)
		}
	}
	return
}
```
