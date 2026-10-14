# 2000. 反转单词前缀

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reverse-prefix-of-word/
- 题目 slug：`reverse-prefix-of-word`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.1 反转字符串
- 难度分：1199
- 外部题解来源：https://leetcode.cn/problems/reverse-prefix-of-word/solutions/992912/go-mo-ni-by-endlesscheng-tq1p/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Go 模拟](https://leetcode.cn/problems/reverse-prefix-of-word/solutions/992912/go-mo-ni-by-endlesscheng-tq1p/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-mo-ni-by-endlesscheng-tq1p`
- topic id：`992912`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
func reversePrefix(s string, ch byte) string {
	j := strings.IndexByte(s, ch)
	if j < 0 {
		return s
	}
	t := []byte(s)
	for i := 0; i < j; i++ {
		t[i], t[j] = t[j], t[i]
		j--
	}
	return string(t)
}
```
