# 2086. 喂食仓鼠的最小食物桶数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-food-buckets-to-feed-the-hamsters/
- 题目 slug：`minimum-number-of-food-buckets-to-feed-the-hamsters`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1623
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-food-buckets-to-feed-the-hamsters/solutions/1125700/tan-xin-on-yi-ci-bian-li-o1-kong-jian-by-hshg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心：O(n) 一次遍历 + O(1) 额外空间](https://leetcode.cn/problems/minimum-number-of-food-buckets-to-feed-the-hamsters/solutions/1125700/tan-xin-on-yi-ci-bian-li-o1-kong-jian-by-hshg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-on-yi-ci-bian-li-o1-kong-jian-by-hshg`
- topic id：`1125700`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

```go
func minimumBuckets(street string) (ans int) {
	bucketPos := -2 // 上一个水桶的放置位置
	for i, ch := range street {
		if ch == 'H' { // 遍历每个房屋
			if bucketPos == i-1 { // 左侧已有水桶，不做任何处理
			} else if i+1 < len(street) && street[i+1] == '.' { // 贪心：先考虑在右侧放水桶
				bucketPos = i + 1
				ans++
			} else if i > 0 && street[i-1] == '.' { // 再考虑在左侧放水桶
				ans++
			} else { // 左右均无空位，无解
				return -1
			}
		}
	}
	return
}
```
