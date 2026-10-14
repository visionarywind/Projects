# 2027. 转换字符串的最少操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-moves-to-convert-string/
- 题目 slug：`minimum-moves-to-convert-string`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.4 从最左/最右开始贪心
- 难度分：1346
- 外部题解来源：https://leetcode.cn/problems/minimum-moves-to-convert-string/solutions/1029071/tan-xin-by-endlesscheng-8jk9/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心模拟 简洁写法](https://leetcode.cn/problems/minimum-moves-to-convert-string/solutions/1029071/tan-xin-by-endlesscheng-8jk9/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-by-endlesscheng-8jk9`
- topic id：`1029071`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

遍历 $s$，遇到 $\texttt{X}$ 就将其与后面两个字符改为 $\texttt{O}$，操作次数加一，然后跳过后面两个字符。

```go
func minimumMoves(s string) (ans int) {
	for i := 0; i < len(s); i++ {
		if s[i] == 'X' {
			ans++
			i += 2 // 跳过两个字符
		}
	}
	return
}
```
