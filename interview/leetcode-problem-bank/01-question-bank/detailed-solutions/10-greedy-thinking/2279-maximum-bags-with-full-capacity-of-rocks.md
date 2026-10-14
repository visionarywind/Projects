# 2279. 装满石头的背包的最大数量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-bags-with-full-capacity-of-rocks/
- 题目 slug：`maximum-bags-with-full-capacity-of-rocks`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：1249
- 外部题解来源：https://leetcode.cn/problems/maximum-bags-with-full-capacity-of-rocks/solutions/1510441/by-endlesscheng-iik6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序 + 贪心](https://leetcode.cn/problems/maximum-bags-with-full-capacity-of-rocks/solutions/1510441/by-endlesscheng-iik6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-iik6`
- topic id：`1510441`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

```go
func maximumBags(capacity, rocks []int, additionalRocks int) (ans int) {
	for i := range capacity {
		capacity[i] -= rocks[i]
	}
	sort.Ints(capacity) // 先装剩余最小的
	for _, leftSpace := range capacity {
		if leftSpace > additionalRocks { // 无法装满，那后续也无法装满（因为排序了）
			break // 直接退出
		}
		ans++
		additionalRocks -= leftSpace
	}
	return
}
```
