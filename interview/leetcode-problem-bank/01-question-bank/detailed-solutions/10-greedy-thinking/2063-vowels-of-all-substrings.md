# 2063. 所有子字符串中的元音

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/vowels-of-all-substrings/
- 题目 slug：`vowels-of-all-substrings`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.5 贡献法
- 难度分：1663
- 外部题解来源：https://leetcode.cn/problems/vowels-of-all-substrings/solutions/1088411/dan-du-ji-suan-mei-ge-yuan-yin-de-chu-xi-eyjf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[单独计算每个元音的出现次数](https://leetcode.cn/problems/vowels-of-all-substrings/solutions/1088411/dan-du-ji-suan-mei-ge-yuan-yin-de-chu-xi-eyjf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dan-du-ji-suan-mei-ge-yuan-yin-de-chu-xi-eyjf`
- topic id：`1088411`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

遍历 $\textit{word}$，若 $\textit{word}[i]$ 是元音，我们考察它能出现在多少个子字符串中。

设 $\textit{word}$ 的长度为 $n$。子字符串 $\textit{word}[l..r]$ 若要包含 $\textit{word}[i]$，则必须满足

- $0\le l\le i$
- $i\le r\le n-1$

这样的 $l$ 有 $i+1$ 个，$r$ 有 $n-i$ 个，因此有 $(i+1)(n-i)$ 个子字符串，所以 $\textit{word}[i]$ 在所有子字符串中一共出现了 $(i+1)(n-i)$ 次。

累加所有出现次数即为答案。

```go
func countVowels(word string) (ans int64) {
	for i, ch := range word {
		if strings.ContainsRune("aeiou", ch) {
			ans += int64(i+1) * int64(len(word)-i)
		}
	}
	return
}
```
