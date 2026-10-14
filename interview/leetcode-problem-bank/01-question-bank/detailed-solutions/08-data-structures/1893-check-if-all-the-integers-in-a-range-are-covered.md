# 1893. 检查是否区域内所有整数都被覆盖

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/check-if-all-the-integers-in-a-range-are-covered/
- 题目 slug：`check-if-all-the-integers-in-a-range-are-covered`
- 来源专题：常用数据结构
- 来源分类路径：二、差分 / §2.1 一维差分 / §2.1.1 基础
- 难度分：1307
- 外部题解来源：https://leetcode.cn/problems/check-if-all-the-integers-in-a-range-are-covered/solutions/825413/chai-fen-xie-fa-by-endlesscheng-kh8v/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[差分写法](https://leetcode.cn/problems/check-if-all-the-integers-in-a-range-are-covered/solutions/825413/chai-fen-xie-fa-by-endlesscheng-kh8v/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`chai-fen-xie-fa-by-endlesscheng-kh8v`
- topic id：`825413`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
func isCovered(a [][]int, l, r int) bool {
	d := [52]int{}
	for _, p := range a {
		d[p[0]]++
		d[p[1]+1]--
	}
	s := 0
	for _, v := range d[:l] {
		s += v
	}
	for _, v := range d[l : r+1] {
		if s += v; s == 0 {
			return false
		}
	}
	return true
}
```
