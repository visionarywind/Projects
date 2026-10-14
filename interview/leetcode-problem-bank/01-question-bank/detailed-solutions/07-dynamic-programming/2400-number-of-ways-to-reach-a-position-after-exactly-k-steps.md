# 2400. 恰好移动 k 步到达某一位置的方法数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-ways-to-reach-a-position-after-exactly-k-steps/
- 题目 slug：`number-of-ways-to-reach-a-position-after-exactly-k-steps`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：1751
- 外部题解来源：https://leetcode.cn/problems/number-of-ways-to-reach-a-position-after-exactly-k-steps/solutions/1799464/by-endlesscheng-6yvy/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[记忆化搜索](https://leetcode.cn/problems/number-of-ways-to-reach-a-position-after-exactly-k-steps/solutions/1799464/by-endlesscheng-6yvy/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-6yvy`
- topic id：`1799464`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1Dt4y1j7qh) 已出炉，包含组合数学做法，欢迎素质三连，在评论区分享你对这场周赛的看法~

---

定义 $f(x, \textit{left})$ 表示当前在 $x$，还剩 $\textit{left}$ 步时，走到终点的方案数。

枚举下一步往左或者往右，累加即为答案。

注意在递归过程中如果 $|x-\textit{endPos}| > \textit{left}$，由于无法到达 $\textit{endPos}$，可以直接返回 $0$，表示无效方案。

```py [sol1-Python3]
class Solution:
    def numberOfWays(self, startPos: int, endPos: int, k: int) -> int:
        MOD = 10 ** 9 + 7
        @cache
        def f(x: int, left: int) -> int:
            if abs(x - endPos) > left: return 0
            if left == 0: return 1  # 成功到达终点，算一种方案
            return (f(x - 1, left - 1) + f(x + 1, left - 1)) % MOD
        return f(startPos, k)
```

```go [sol1-Go]
func numberOfWays(startPos, endPos, k int) int {
	type pair struct{ x, y int }
	dp := map[pair]int{}
	var f func(int, int) int
	f = func(x, left int) int {
		if abs(x-endPos) > left {
			return 0
		}
		if left == 0 {
			return 1 // 成功到达终点，算一种方案
		}
		p := pair{x, left}
		if v, ok := dp[p]; ok {
			return v
		}
		res := (f(x-1, left-1) + f(x+1, left-1)) % (1e9 + 7)
		dp[p] = res
		return res
	}
	return f(startPos, k)
}
func abs(x int) int { if x < 0 { return -x }; return x }
```
