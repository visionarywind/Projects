# 2002. 两个回文子序列长度的最大乘积

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-product-of-the-length-of-two-palindromic-subsequences/
- 题目 slug：`maximum-product-of-the-length-of-two-palindromic-subsequences`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.6 SOS DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-product-of-the-length-of-two-palindromic-subsequences/solutions/992922/go-bao-sou-by-endlesscheng-ivn0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Go 暴搜](https://leetcode.cn/problems/maximum-product-of-the-length-of-two-palindromic-subsequences/solutions/992922/go-bao-sou-by-endlesscheng-ivn0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-bao-sou-by-endlesscheng-ivn0`
- topic id：`992922`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

根据题意，由于两个子序列不能相交，因此每个字符只有三种选择：不选、放到其中一个子序列中、放到另一个子序列中。

由于字符串很短，我们对每个字符枚举这三种情况，暴力搜索。

时间复杂度：$O(3^n)$。

```go
func maxProduct(s string) (ans int) {
	var a, b []byte
	var f func(int)
	f = func(i int) {
		if i == len(s) {
			if len(a)*len(b) > ans && isPalindromic(a) && isPalindromic(b) {
				ans = len(a) * len(b)
			}
			return
		}

		// 不选
		f(i + 1)

		// 放入 a
		a = append(a, s[i])
		f(i + 1)
		a = a[:len(a)-1]

		// 放入 b
		b = append(b, s[i])
		f(i + 1)
		b = b[:len(b)-1]
	}
	f(0)
	return
}

func isPalindromic(a []byte) bool {
	for i, n := 0, len(a); i < n/2; i++ {
		if a[i] != a[n-1-i] {
			return false
		}
	}
	return true
}
```
