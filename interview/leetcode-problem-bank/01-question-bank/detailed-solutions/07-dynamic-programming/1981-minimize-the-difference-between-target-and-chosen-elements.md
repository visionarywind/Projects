# 1981. 最小化目标值与所选元素的差

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimize-the-difference-between-target-and-chosen-elements/
- 题目 slug：`minimize-the-difference-between-target-and-chosen-elements`
- 来源专题：动态规划
- 来源分类路径：三、背包 / §3.4 分组背包
- 难度分：2010
- 外部题解来源：https://leetcode.cn/problems/minimize-the-difference-between-target-and-chosen-elements/solutions/951181/fen-zu-bei-bao-by-endlesscheng-fret/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[每组恰好选一个的分组背包 + 常数优化至 40ms](https://leetcode.cn/problems/minimize-the-difference-between-target-and-chosen-elements/solutions/951181/fen-zu-bei-bao-by-endlesscheng-fret/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-zu-bei-bao-by-endlesscheng-fret`
- topic id：`951181`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

把每一行看成一组，物品的体积就是元素值，这样就转化成了一个分组背包的模型。

定义 $f[i][j]$ 表示能否从前 $i$ 组物品中选出恰好为 $j$ 的重量，且每组都恰好选一个物品。

转移时可以倒序转移，从而压缩空间。

注意背包大小至多是 $\textit{target}\cdot 2$：

- 如果每行选最小的，也比 $\textit{target}$ 大，那么最优就是每行选最小的。
- 否则，最小差值至多为 $\textit{target}$，所以无需考虑元素和大于 $\textit{target}\cdot 2$ 的情况。

其余优化点见代码注释。

```go
func minimizeTheDifference(mat [][]int, target int) int {
	dp := make([]bool, min(len(mat)*70, target*2)+1) // 需要枚举的重量不会超过 target*2
	dp[0] = true
	minSum, maxSum := 0, 0
	for _, row := range mat {
		mi, mx := row[0], row[0]
		for _, v := range row[1:] {
			if v > mx {
				mx = v
			} else if v < mi {
				mi = v
			}
		}
		minSum += mi // 求 minSum 是为了防止 target 过小导致 dp 没有记录到
		maxSum = min(maxSum+mx, target*2) // 前 i 组的最大重量，优化枚举时 j 的初始值
		for j := maxSum; j >= 0; j-- {
			dp[j] = false
			for _, v := range row {
				if v <= j && dp[j-v] {
					dp[j] = true
					break
				}
			}
		}
	}
	ans := abs(minSum - target)
	for i, ok := range dp {
		if ok {
			ans = min(ans, abs(i-target))
		}
	}
	return ans
}

func abs(x int) int { if x < 0 { return -x }; return x }
```
