# 2260. 必须拿起的最小连续卡牌数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-consecutive-cards-to-pick-up/
- 题目 slug：`minimum-consecutive-cards-to-pick-up`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.1 枚举右，维护左 / §0.1.1 基础
- 难度分：1365
- 外部题解来源：https://leetcode.cn/problems/minimum-consecutive-cards-to-pick-up/solutions/1461623/by-endlesscheng-rv10/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心+哈希表](https://leetcode.cn/problems/minimum-consecutive-cards-to-pick-up/solutions/1461623/by-endlesscheng-rv10/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-rv10`
- topic id：`1461623`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
func minimumCardPickup(cards []int) int {
	ans := len(cards) + 1
	pos := map[int]int{}
	for i, v := range cards {
		if p, ok := pos[v]; ok && i-p+1 < ans { // 找最近出现的 v
			ans = i - p + 1 
		}
		pos[v] = i
	}
	if ans <= len(cards) {
		return ans
	}
	return -1
}
```
