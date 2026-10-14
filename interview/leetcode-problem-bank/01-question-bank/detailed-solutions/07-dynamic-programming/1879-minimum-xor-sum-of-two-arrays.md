# 1879. 两个数组最小的异或值之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-xor-sum-of-two-arrays/
- 题目 slug：`minimum-xor-sum-of-two-arrays`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.1 排列型状压 DP ① 相邻无关
- 难度分：2145
- 外部题解来源：https://leetcode.cn/problems/minimum-xor-sum-of-two-arrays/solutions/802324/zhuang-ya-dp-by-endlesscheng-d8sw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[状压 DP](https://leetcode.cn/problems/minimum-xor-sum-of-two-arrays/solutions/802324/zhuang-ya-dp-by-endlesscheng-d8sw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuang-ya-dp-by-endlesscheng-d8sw`
- topic id：`802324`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
func minimumXORSum(x, y []int) int {
	m := 1 << len(x)
	dp := make([]int, m)
	for i := range dp {
		dp[i] = 2e9
	}
	dp[0] = 0
	for s, dv := range dp[:m-1] {
		v := x[bits.OnesCount(uint(s))]
		for t, lb := s^(m-1), 0; t > 0; t ^= lb {
			lb = t & -t
			w := y[bits.TrailingZeros(uint(lb))]
			dp[s|lb] = min(dp[s|lb], v^w+dv)
		}
	}
	return dp[m-1]
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}
```
