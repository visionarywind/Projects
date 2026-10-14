# 1839. 所有元音按顺序排布的最长子字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-substring-of-all-vowels-in-order/
- 题目 slug：`longest-substring-of-all-vowels-in-order`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：1580
- 外部题解来源：https://leetcode.cn/problems/longest-substring-of-all-vowels-in-order/solutions/742586/mo-ni-by-endlesscheng-otyy/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(N) 模拟，或正则表达式](https://leetcode.cn/problems/longest-substring-of-all-vowels-in-order/solutions/742586/mo-ni-by-endlesscheng-otyy/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ni-by-endlesscheng-otyy`
- topic id：`742586`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

#### 解法一：模拟

根据题意，美丽子字符串必须是 $aa\cdots aee\cdots eii\cdots ioo\cdots ouu\cdots u$ 的形式，每个元音至少出现一次。

采用分组遍历的写法来寻找美丽子字符串。（每次遍历处理一组相同的连续字母）

用 $\textit{cur}$ 表示当前对应的元音下标，$\textit{sum}$ 表示当前美丽子字符串的长度。

若遍历到的字母与 $\textit{vowel}[\textit{cur}]$ 不匹配则 $\textit{cur}$ 和 $\textit{sum}$ 置零，重新寻找美丽子字符串；否则累加长度至 $\textit{sum}$，同时 $\textit{cur}$ 指向下一个元音。若 $\textit{cur}=5$ 则说明找到了一个美丽子字符串，更新答案并将 $\textit{cur}$ 和 $\textit{sum}$ 置零。

```go
func longestBeautifulSubstring(s string) (ans int) {
	const vowel = "aeiou"
	cur, sum := 0, 0
	for i, n := 0, len(s); i < n; {
		start := i
		ch := s[start]
		for i < n && s[i] == ch {
			i++
		}

		if ch != vowel[cur] {
			cur, sum = 0, 0
			if ch != vowel[0] {
				continue
			}
		}

		sum += i - start
		cur++
		if cur == 5 {
			if sum > ans {
				ans = sum
			}
			cur, sum = 0, 0
		}
	}
	return
}
```

时间复杂度为下标 $i$ 的增加次数，即 $O(N)$。

#### 解法二：正则表达式

```go
var re = regexp.MustCompile("a+e+i+o+u+")

func longestBeautifulSubstring(s string) (ans int) {
	for _, t := range re.FindAllString(s, -1) {
		if len(t) > ans {
			ans = len(t)
		}
	}
	return
}
```
