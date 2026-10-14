# 1866. 恰有 K 根木棍可以看到的排列数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-ways-to-rearrange-sticks-with-k-sticks-visible/
- 题目 slug：`number-of-ways-to-rearrange-sticks-with-k-sticks-visible`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.7 计数 DP
- 难度分：2333
- 外部题解来源：https://leetcode.cn/problems/number-of-ways-to-rearrange-sticks-with-k-sticks-visible/solutions/779216/zhuan-huan-cheng-di-yi-lei-si-te-lin-shu-2y1k/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[可以转换成第一类斯特林数的理由](https://leetcode.cn/problems/number-of-ways-to-rearrange-sticks-with-k-sticks-visible/solutions/779216/zhuan-huan-cheng-di-yi-lei-si-te-lin-shu-2y1k/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuan-huan-cheng-di-yi-lei-si-te-lin-shu-2y1k`
- topic id：`779216`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

将每根能看到的木棍及其后面被挡住的木棍看作一个整体，则这 $n$ 根木棍被划分成了 $k$ 个部分，每个部分的第一根木棍即为可以看到的木棍。

对于每个长为 $m$ 的部分，对于其任意排列，我们总是可以将排列中最大的元素当作可以看到的木棍，移到该部分的开头，则剩余的木棍可以任意排列，因此每个部分的方案数为 $(m-1)!$，即为一个长为 $m$ 的圆排列的方案数。

因此原问题本质上就是在问长为 $n$ 的排列划分成 $k$ 个非空圆排列的方案数，这就是第一类斯特林数。

```go
var f [1001][1001]int

func init() {
	f[0][0] = 1
	for i := 1; i <= 1000; i++ {
		for j := 1; j <= i; j++ {
			f[i][j] = (f[i-1][j-1] + (i-1)*f[i-1][j]) % (1e9 + 7)
		}
	}
}

func rearrangeSticks(n, k int) int {
	return f[n][k]
}
```
