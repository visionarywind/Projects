# 2222. 选择建筑的方案数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-ways-to-select-buildings/
- 题目 slug：`number-of-ways-to-select-buildings`
- 来源专题：动态规划
- 来源分类路径：六、状态机 DP / §6.2 基础
- 难度分：1657
- 外部题解来源：https://leetcode.cn/problems/number-of-ways-to-select-buildings/solutions/1390143/mei-ju-by-endlesscheng-xp5r/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[计算 010 和 101 的出现次数](https://leetcode.cn/problems/number-of-ways-to-select-buildings/solutions/1390143/mei-ju-by-endlesscheng-xp5r/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-by-endlesscheng-xp5r`
- topic id：`1390143`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

根据题意，只有两种情况：

- 010：对每个 1，统计左边 0 的个数和右边 0 的个数；
- 101：对每个 0，统计左边 1 的个数和右边 1 的个数。

```go
func numberOfWays(s string) int64 {
	tot0 := strings.Count(s, "0")
	ans, c0 := 0, 0
	for i, ch := range s {
		if ch == '1' {
			ans += c0 * (tot0 - c0) // 对每个 1，统计左边 0 的个数和右边 0 的个数
		} else {
			c1 := i - c0
			ans += c1 * (len(s) - tot0 - c1) // 对每个 0，统计左边 1 的个数和右边 1 的个数
			c0++
		}
	}
	return int64(ans)
}
```
