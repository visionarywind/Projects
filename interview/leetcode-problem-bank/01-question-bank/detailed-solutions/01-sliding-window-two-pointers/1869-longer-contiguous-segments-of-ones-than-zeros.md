# 1869. 哪种连续子字符串更长

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longer-contiguous-segments-of-ones-than-zeros/
- 题目 slug：`longer-contiguous-segments-of-ones-than-zeros`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：1205
- 外部题解来源：https://leetcode.cn/problems/longer-contiguous-segments-of-ones-than-zeros/solutions/791226/fen-zu-xun-huan-by-endlesscheng-f2i1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分组循环](https://leetcode.cn/problems/longer-contiguous-segments-of-ones-than-zeros/solutions/791226/fen-zu-xun-huan-by-endlesscheng-f2i1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-zu-xun-huan-by-endlesscheng-f2i1`
- topic id：`791226`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
func checkZeroOnes(s string) bool {
	m1, m0 := 0, 0
	for i, n := 0, len(s); i < n; {
		st := i
		v := s[st]
		for ; i < n && s[i] == v; i++ {
		}
		if v == '1' {
			m1 = max(m1, i-st)
		} else {
			m0 = max(m0, i-st)
		}
	}
	return m1 > m0
}
```
