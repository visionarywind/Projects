# 1946. 子字符串突变后可能得到的最大整数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/largest-number-after-mutating-substring/
- 题目 slug：`largest-number-after-mutating-substring`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：1445
- 外部题解来源：https://leetcode.cn/problems/largest-number-after-mutating-substring/solutions/895181/go-tan-xin-by-endlesscheng-znuz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Go 贪心](https://leetcode.cn/problems/largest-number-after-mutating-substring/solutions/895181/go-tan-xin-by-endlesscheng-znuz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-tan-xin-by-endlesscheng-znuz`
- topic id：`895181`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

```go
func maximumNumber(num string, change []int) string {
	s := []byte(num)
	i, n := 0, len(s)
	for ; i < n && byte(change[s[i]&15]) <= s[i]&15; i++ {
	}
	for ; i < n && byte(change[s[i]&15]) >= s[i]&15; i++ {
		s[i] = '0' + byte(change[s[i]&15])
	}
	return string(s)
}
```
