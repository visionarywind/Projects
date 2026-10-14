# 2405. 子字符串的最优划分

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/optimal-partition-of-string/
- 题目 slug：`optimal-partition-of-string`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.5 划分型贪心
- 难度分：1355
- 外部题解来源：https://leetcode.cn/problems/optimal-partition-of-string/solutions/1816269/wei-yun-suan-o1-kong-jian-by-endlesschen-m0q5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心 + 位运算 O(1) 空间](https://leetcode.cn/problems/optimal-partition-of-string/solutions/1816269/wei-yun-suan-o1-kong-jian-by-endlesschen-m0q5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`wei-yun-suan-o1-kong-jian-by-endlesschen-m0q5`
- topic id：`1816269`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1it4y1L7kL) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~
 
---

贪心：从左往右遍历，没出现过就分到同一组。

```go
func partitionString(s string) int {
	ans, vis := 1, 0
	for _, c := range s {
		if vis>>(c&31)&1 > 0 {
			vis = 0
			ans++
		}
		vis |= 1 << (c & 31)
	}
	return ans
}
```
