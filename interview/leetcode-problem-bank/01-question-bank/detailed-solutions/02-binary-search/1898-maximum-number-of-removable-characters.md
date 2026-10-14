# 1898. 可移除字符的最大数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-removable-characters/
- 题目 slug：`maximum-number-of-removable-characters`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.2 求最大
- 难度分：1913
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-removable-characters/solutions/825874/go-er-fen-by-endlesscheng-yal5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Go 二分](https://leetcode.cn/problems/maximum-number-of-removable-characters/solutions/825874/go-er-fen-by-endlesscheng-yal5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-er-fen-by-endlesscheng-yal5`
- topic id：`825874`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
func maximumRemovals(s, p string, id []int) int {
	return sort.Search(len(id), func(k int) bool {
		del := make([]bool, len(s))
		for _, i := range id[:k+1] {
			del[i] = true
		}
		j := 0
		for i := range s {
			if !del[i] && s[i] == p[j] {
				if j++; j == len(p) {
					return false
				}
			}
		}
		return true
	})
}
```
