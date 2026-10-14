# 2030. 含特定字母的最小子序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/smallest-k-length-subsequence-with-occurrences-of-a-letter/
- 题目 slug：`smallest-k-length-subsequence-with-occurrences-of-a-letter`
- 来源专题：单调栈
- 来源分类路径：四、最小字典序
- 难度分：2562
- 外部题解来源：https://leetcode.cn/problems/smallest-k-length-subsequence-with-occurrences-of-a-letter/solutions/1029139/go-dan-diao-zhan-by-endlesscheng-4yqf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Go 单调队列](https://leetcode.cn/problems/smallest-k-length-subsequence-with-occurrences-of-a-letter/solutions/1029139/go-dan-diao-zhan-by-endlesscheng-4yqf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-dan-diao-zhan-by-endlesscheng-4yqf`
- topic id：`1029139`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

去掉 $\textit{repetition}$ 的限制就是一个经典问题，求长为 $k$ 的字典序最小子序列，可以使用单调队列求解。

对于本题需要注意的是，在弹出队尾元素时，需要判断后续能否构成 $\textit{repetition}$ 个 $\textit{letter}$，从而及时退出循环。以及在弹出队首元素时，需要判断是否还有空间来放置后续的 $\textit{letter}$。

```go
func smallestSubsequence(s string, k int, letter byte, repetition int) string {
	ans := []byte{}
	unread := strings.Count(s, string(letter)) // 未遍历到的 letter 个数
	inQ := 0 // 在单调队列中的 letter 个数
	q := []byte{}
	for i, n := 0, len(s); len(ans) < k; i++ {
		// 将 s[i] 插入单调队列
		for i < n && len(q) > 0 && s[i] < q[len(q)-1] {
			if q[len(q)-1] == letter { // 特判队尾是 letter 的情况
				if inQ+unread <= repetition { // 弹出队尾会导致无法凑足 repetition 个 letter，及时退出
					break
				}
				inQ--
			}
			q = q[:len(q)-1]
		}
		if i < n {
			if s[i] == letter {
				unread--
				inQ++
			}
			q = append(q, s[i])
		}

		if i >= n-k {
			// 从队首拿到字典序最小的字符
			if q[0] == letter {
				inQ--
				repetition--
				ans = append(ans, q[0])
			} else if len(ans)+repetition < k { // 还有足够空间可以放 letter
				ans = append(ans, q[0])
			}
			q = q[1:]
		}
	}
	return string(ans)
}
```
