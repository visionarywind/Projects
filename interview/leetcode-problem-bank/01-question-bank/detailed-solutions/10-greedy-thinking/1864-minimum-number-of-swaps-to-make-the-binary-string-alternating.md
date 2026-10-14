# 1864. 构成交替字符串需要的最小交换次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-swaps-to-make-the-binary-string-alternating/
- 题目 slug：`minimum-number-of-swaps-to-make-the-binary-string-alternating`
- 来源专题：贪心与思维
- 来源分类路径：八、其他
- 难度分：1601
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-swaps-to-make-the-binary-string-alternating/solutions/779054/bi-jiao-1010-huo-0101-de-zui-xiao-bu-ton-1xpx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[比较 1010... 或 0101... 的最小不同值](https://leetcode.cn/problems/minimum-number-of-swaps-to-make-the-binary-string-alternating/solutions/779054/bi-jiao-1010-huo-0101-de-zui-xiao-bu-ton-1xpx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bi-jiao-1010-huo-0101-de-zui-xiao-bu-ton-1xpx`
- topic id：`779054`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

最终得到的字符串一定是 $\texttt{1010}\cdots$ 或者 $\texttt{0101}\cdots$，直接比较 $s$ 与这两个字符串的不同值即可。

```go
func minSwaps(s string) int {
	ans := int(1e9)
	f := func(tar string) {
		if strings.Count(tar, "0") != strings.Count(s, "0") {
			return
		}
		c := 0
		for i := range s {
			if s[i] == '1' && tar[i] == '0' {
				c++
			}
		}
		if c < ans {
			ans = c
		}
	}
	n := len(s)
	f(strings.Repeat("10", n)[:n])
	f(strings.Repeat("01", n)[:n])
	if ans == 1e9 {
		return -1
	}
	return ans
}
```
