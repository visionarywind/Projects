# 2060. 同源字符串检测

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/check-if-an-original-string-exists-given-two-encoded-strings/
- 题目 slug：`check-if-an-original-string-exists-given-two-encoded-strings`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：2804
- 外部题解来源：https://leetcode.cn/problems/check-if-an-original-string-exists-given-two-encoded-strings/solutions/1075925/ji-yi-hua-sou-suo-by-endlesscheng-ll3r/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[DFS + 分类讨论](https://leetcode.cn/problems/check-if-an-original-string-exists-given-two-encoded-strings/solutions/1075925/ji-yi-hua-sou-suo-by-endlesscheng-ll3r/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-yi-hua-sou-suo-by-endlesscheng-ll3r`
- topic id：`1075925`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

$\textit{dfs}(i,j,d)$ 表示 $s1[:i]$ 的原始字符串的长度减去 $s2[:j]$ 的原始字符串的长度等于 $d$ 时，能否匹配（即可以由同一个原始字符串转换得到）。

按照 $d$ 的大小分类讨论：

- 若 $d=0$，表示 $s1[:i]$ 与 $s2[:j]$ 的原始字符串长度相等，此时若 $s1[i]$ 与 $s2[j]$ 相同，则可以继续向后匹配；若 $s1[i]$ 为数字，则 $s1$ 的原始字符串可以向后扩展；若 $s2[j]$ 为数字，则 $s2$ 的原始字符串可以向后扩展。**这一规则同时意味着，当 $d\ne 0$ 时，较长的原始字符串多出的那部分必然对应着一串数字。**
- 若 $d<0$，则 $s2[:j]$ 的原始字符串比 $s1[:i]$ 的原始字符串长，且长出的那部分在 $s2$ 中是数字，因此 $s1[:i]$ 可以用数字或字母来向后扩展，且无需关心是否需要匹配字母；
- 若 $d>0$，则 $s1[:i]$ 的原始字符串比 $s2[:j]$ 的原始字符串长，且长出的那部分在 $s1$ 中是数字，因此 $s2[:j]$ 可以用数字或字母来向后扩展，且无需关心是否需要匹配字母；

最后，若 $s1$ 与 $s2$ 均能匹配到字符串末尾，且此时 $d=0$，则二者可以由同一个原始字符串转换得到。

代码实现时，由于连续数字至多有 $3$ 个，按照上面的匹配规则，$d$ 的范围可以保证在 $(-1000,1000)$ 之内，因此我们可以简单地用数组来标记访问过的状态。

```go
func possiblyEquals(s1, s2 string) bool {
	n, m := len(s1), len(s2)
	const mx, bias = 2000, 1000
	vis := make([][][mx]bool, n+1)
	for i := range vis {
		vis[i] = make([][mx]bool, m+1)
	}
	var dfs func(i, j, d int) bool
	dfs = func(i, j, d int) bool {
		if i == n && j == m {
			return d == 0 // 匹配成功
		}
		if vis[i][j][d+bias] {
			return false
		}
		vis[i][j][d+bias] = true
		// 原始字符串长度相同时，若 s1[i] == s2[j]，则 s1[:i] 和 s2[:j] 均可以向后扩展一个字母
		if d == 0 && i < n && j < m && s1[i] == s2[j] && dfs(i+1, j+1, 0) {
			return true
		}
		if d <= 0 && i < n { // s1[:i] 的原始字符串长度不超过 s2[:j] 的原始字符串长度时，扩展 s1[:i]
			if unicode.IsDigit(rune(s1[i])) { // 数字
				for p, v := i, 0; p < n && unicode.IsDigit(rune(s1[p])); p++ {
					v = v*10 + int(s1[p]&15)
					if dfs(p+1, j, d+v) {
						return true
					}
				}
			} else if d < 0 && dfs(i+1, j, d+1) { // 字符，扩展一位，注意这里 d 不能为 0
				return true
			}
		}
		if d >= 0 && j < m { // s2[:j] 的原始字符串长度不超过 s1[:i] 的原始字符串长度时，扩展 s2[:j]
			if unicode.IsDigit(rune(s2[j])) { // 数字
				for q, v := j, 0; q < m && unicode.IsDigit(rune(s2[q])); q++ {
					v = v*10 + int(s2[q]&15)
					if dfs(i, q+1, d-v) {
						return true
					}
				}
			} else if d > 0 && dfs(i, j+1, d-1) { // 字符，扩展一位，注意这里 d 不能为 0
				return true
			}
		}
		return false
	}
	return dfs(0, 0, 0)
}
```
