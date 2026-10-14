# 1987. 不同的好子序列数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-unique-good-subsequences/
- 题目 slug：`number-of-unique-good-subsequences`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.4 合法子序列 DP
- 难度分：2422
- 外部题解来源：https://leetcode.cn/problems/number-of-unique-good-subsequences/solutions/965042/jian-ji-xie-fa-by-endlesscheng-bvrx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[倒序动态规划](https://leetcode.cn/problems/number-of-unique-good-subsequences/solutions/965042/jian-ji-xie-fa-by-endlesscheng-bvrx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-xie-fa-by-endlesscheng-bvrx`
- topic id：`965042`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

我们将 $s$ 反转，在反转后的字符串 $s'$ 上，定义 $f[i][0/1]$ 表示前 $i$ 个字符中，以 $\texttt{0}$ 或 $\texttt{1}$ 结尾的不同子序列的个数，这样最后取 $f[n-1][1]$ 就可以避免前导零的影响。

转移时，我们可以直接在 $f[i-1][0/1]$ 对应的子序列末尾加上 $s'[i]$，由于 $f[i-1][0/1]$ 统计的是不同子序列的个数，因此这些子序列加上 $s'[i]$ 后，互相之间仍然是不同的；同时，$s'[i]$ 也可以单独形成一个子序列，因此有转移：

$$
f[i][s'[i]] = f[i-1][0] + f[i-1][1] + 1
$$

最后，若 $s$ 含有字符 $\texttt{0}$，则答案需要额外加上一。

代码实现时，第一维可以压缩掉。

相似题目：

- [940. 不同的子序列 II](https://leetcode.cn/problems/distinct-subsequences-ii/)

```go
func numberOfUniqueGoodSubsequences(s string) int {
	const mod int = 1e9 + 7
	f := [2]int{}
	for i := len(s) - 1; i >= 0; i-- {
		f[s[i]&1] = (f[0] + f[1] + 1) % mod
	}
	ans := f[1]
	if strings.Contains(s, "0") {
		ans = (ans + 1) % mod
	}
	return ans
}
```
