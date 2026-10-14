# 1655. 分配重复整数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/distribute-repeating-integers/
- 题目 slug：`distribute-repeating-integers`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.4 子集状压 DP
- 难度分：2307
- 外部题解来源：https://leetcode.cn/problems/distribute-repeating-integers/solutions/920640/go-zi-ji-zhuang-ya-dp-by-endlesscheng-h8z0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[Go 子集状压 DP](https://leetcode.cn/problems/distribute-repeating-integers/solutions/920640/go-zi-ji-zhuang-ya-dp-by-endlesscheng-h8z0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-zi-ji-zhuang-ya-dp-by-endlesscheng-h8z0`
- topic id：`920640`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
func canDistribute(a []int, quantity []int) bool {
	// 预处理 quantity 每个子集的子集和
	m := 1 << len(quantity)
	sum := make([]int, m)
	for i, v := range quantity {
		bit := 1 << i
		for j := 0; j < bit; j++ {
			sum[bit|j] = sum[j] + v
		}
	}

	cnt := map[int]int{}
	for _, v := range a {
		cnt[v]++
	}
	n := len(cnt)

	// dp[i][j] 表示 cnt 的前 i 个元素能否满足集合为 j 的顾客
	dp := make([][]bool, n+1)
	for i := range dp {
		dp[i] = make([]bool, m)
		dp[i][0] = true
	}
	i := 0
	for _, c := range cnt {
		for j, ok := range dp[i] {
			if ok {
				dp[i+1][j] = true
				continue
			}
			for sub := j; sub > 0; sub = (sub - 1) & j { // 枚举 j 的子集 sub
				if sum[sub] <= c && dp[i][j^sub] { // 判断这 c 个数能否全部分给 sub，并且除了 sub 以外的 j 中的顾客也满足
					dp[i+1][j] = true
					break
				}
			}
		}
		i++
	}
	return dp[n][m-1]
}
```
