# 2055. 蜡烛之间的盘子

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/plates-between-candles/
- 题目 slug：`plates-between-candles`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.5 进阶
- 难度分：1819
- 外部题解来源：https://leetcode.cn/problems/plates-between-candles/solutions/1075392/xian-xing-zuo-fa-qian-zhui-he-mei-ge-wei-b2if/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[线性做法：前缀和+每个位置的最近蜡烛](https://leetcode.cn/problems/plates-between-candles/solutions/1075392/xian-xing-zuo-fa-qian-zhui-he-mei-ge-wei-b2if/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xian-xing-zuo-fa-qian-zhui-he-mei-ge-wei-b2if`
- topic id：`1075392`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
func platesBetweenCandles(s string, queries [][]int) []int {
	n := len(s)
	sum := make([]int, n+1) // sum[i] 表示 s[:i] 中盘子的个数
	left := make([]int, n) // left[i] 表示 i 左侧最近蜡烛位置
	p := -1
	for i, b := range s {
		sum[i+1] = sum[i]
		if b == '|' {
			p = i
		} else {
			sum[i+1]++
		}
		left[i] = p
	}

	right := make([]int, n) // right[i] 表示 i 右侧最近蜡烛位置
	for i, p := n-1, n; i >= 0; i-- {
		if s[i] == '|' {
			p = i
		}
		right[i] = p
	}

	ans := make([]int, len(queries))
	for i, q := range queries {
		l, r := right[q[0]], left[q[1]] // 用最近蜡烛位置来代替查询的范围，从而去掉不符合要求的盘子
		if l < r {
			ans[i] = sum[r] - sum[l]
		}
	}
	return ans
}
```
