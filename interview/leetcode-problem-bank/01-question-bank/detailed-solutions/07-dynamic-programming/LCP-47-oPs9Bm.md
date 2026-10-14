# LCP 47. 入场安检

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/oPs9Bm/
- 题目 slug：`oPs9Bm`
- 来源专题：动态规划
- 来源分类路径：三、背包 / §3.1 0-1 背包
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/oPs9Bm/solutions/1016968/zhuan-huan-wei-01-bei-bao-fang-an-shu-by-1dax/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[转换为 01 背包方案数](https://leetcode.cn/problems/oPs9Bm/solutions/1016968/zhuan-huan-wei-01-bei-bao-fang-an-shu-by-1dax/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuan-huan-wei-01-bei-bao-fang-an-shu-by-1dax`
- topic id：`1016968`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

根据题意：

- 对于类型为先进先出（队列）的安检室，第一个进入的人总是第一个出去的；
- 对于类型为后进先出（栈）的安检室，当栈填满时，最后一个进入的人才可以最先出去。

我们可以对栈填充 $x_i=\textit{capacities}_i-1$ 个人，这样可以将栈转换成一个大小为 $1$ 的队列。

因此，为了使编号为 $k$ 的观众第一个通过最后一个安检室，我们需要将其中一些安检室设定为栈类型，且这些安检室的 $x_i$ 之和恰好为 $k$。当这些安检室都各自填充了 $x_i$ 个人之后，所有安检室可以串成一个队列，此时编号为 $k$ 的观众就自然地成为第一个通过最后一个安检室的人。

把 $x_i$ 看成物品大小，$k$ 看成背包容量，则该问题就转换成了一个经典模型——求 01 背包的方案数。

```go
func securityCheck(capacities []int, k int) int {
	dp := make([]int, k+1)
	dp[0] = 1
	for _, x := range capacities {
		x--
		for s := k; s >= x; s-- {
			dp[s] = (dp[s] + dp[s-x]) % (1e9 + 7)
		}
	}
	return dp[k]
}
```
