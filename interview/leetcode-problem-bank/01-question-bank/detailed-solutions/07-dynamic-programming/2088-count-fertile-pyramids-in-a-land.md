# 2088. 统计农场中肥沃金字塔的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-fertile-pyramids-in-a-land/
- 题目 slug：`count-fertile-pyramids-in-a-land`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.5 子矩形 DP
- 难度分：2105
- 外部题解来源：https://leetcode.cn/problems/count-fertile-pyramids-in-a-land/solutions/1125747/dong-tai-gui-hua-by-endlesscheng-depp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(mn) 动态规划](https://leetcode.cn/problems/count-fertile-pyramids-in-a-land/solutions/1125747/dong-tai-gui-hua-by-endlesscheng-depp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-by-endlesscheng-depp`
- topic id：`1125747`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

先求正金字塔。

定义 $\textit{dp}[i][j]$ 表示金字塔顶端位于 $(i,j)$ 时的最大层数（$1$ 层也算）。如果顶端在 $(i,j)$ 的金字塔最大能有 $x$ 层，那么顶端在 $(i,j)$ 的金字塔也可以有 $x-1,x-2,\cdots,1$ 层。由于要求区域内格子数目大于 $1$，统计答案的时候把 $1$ 层去掉，因此有 $x-1$ 个以 $(i,j)$ 为顶端的金字塔。

我们从 $\textit{grid}$ 的最后一行开始往上递推。转移的策略是在 $(i+1,j)$ 处的最大金字塔上套一层 **∧ 型的「外壳」**。具体来说，从 $(i,j)$ 出发向左下方向前进，求出能达到的最长连续 $1$ 的个数，根据 $\textit{dp}$ 的定义，这就是 $\textit{dp}[i+1][j-1]+1$；同理，向右下方向前进，能达到的最长连续 $1$ 的个数为 $\textit{dp}[i+1][j+1]+1$。取左右最长连续 $1$ 的个数的最小值即为「外壳」的高度。那么 $(i,j)$ 处的最大金字塔的高度为「外壳」的高度与 $(i+1,j)$ 处最大金字塔高度 $+1$ 的最小值。

即

$$
\textit{dp}[i][j] = \min(\textit{dp}[i+1][j-1]+1, \textit{dp}[i+1][j+1]+1, \textit{dp}[i+1][j]+1)
$$

倒金字塔可以将 $\textit{grid}$ 上下颠倒后再求一遍正金字塔即可。

```go
func countPyramids(grid [][]int) (ans int) {
	m, n := len(grid), len(grid[0])
	dp := make([][]int, m)
	for i := range dp {
		dp[i] = make([]int, n)
	}

	f := func() {
		dp[m-1] = grid[m-1]
		for i := m - 2; i >= 0; i-- {
			dp[i][0] = grid[i][0]
			dp[i][n-1] = grid[i][n-1]
			for j := 1; j < n-1; j++ {
				if grid[i][j] == 0 {
					dp[i][j] = 0
				} else {
					dp[i][j] = min(dp[i+1][j-1], dp[i+1][j+1], dp[i+1][j]) + 1
					ans += dp[i][j] - 1
				}
			}
		}
	}
	f() // 求正金字塔个数
	for i := 0; i < m/2; i++ { // 上下颠倒
		grid[i], grid[m-1-i] = grid[m-1-i], grid[i]
	}
	f() // 再求一遍正金字塔个数
	return
}
```

相似题目：

- [CF1393D - Rarity and New Dress](https://codeforces.com/problemset/problem/1393/D)
