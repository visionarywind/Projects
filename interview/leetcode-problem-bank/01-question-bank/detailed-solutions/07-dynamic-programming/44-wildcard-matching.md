# 44. 通配符匹配

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/wildcard-matching/
- 题目 slug：`wildcard-matching`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.1 最长公共子序列（LCS） / §4.1.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/wildcard-matching/solutions/1492176/go-ku-han-shu-zhengz-by-endlesscheng-8c1u/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Go 库函数正则匹配](https://leetcode.cn/problems/wildcard-matching/solutions/1492176/go-ku-han-shu-zhengz-by-endlesscheng-8c1u/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-ku-han-shu-zhengz-by-endlesscheng-8c1u`
- topic id：`1492176`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

将题目中的两种通配符转换成标准正则写法。

```go
func isMatch(s, p string) bool {
	p = strings.ReplaceAll(p, "?", ".")
	p = strings.ReplaceAll(p, "*", ".*")
	return regexp.MustCompile("^" + p + "$").MatchString(s)
}
```
