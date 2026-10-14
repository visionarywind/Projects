# LCP 43. 十字路口的交通

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/Y1VbOX/
- 题目 slug：`Y1VbOX`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/Y1VbOX/solutions/991709/dong-tai-gui-hua-by-endlesscheng-bcmi/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划](https://leetcode.cn/problems/Y1VbOX/solutions/991709/dong-tai-gui-hua-by-endlesscheng-bcmi/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-by-endlesscheng-bcmi`
- topic id：`991709`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```go
var mp = []int{'E': 0, 'S': 1, 'W': 2, 'N': 3} // 将方向字符映射到 0-3 上
var valid = [4][4][4]int{ // 当前车去方向，另一辆车来方向，另一辆车去方向
	{},
	{
		{},
		{1},
		{0, 0, 0, 1},
		{0, 0, 1},
	},
	{
		{},
		{1},
		{1, 1},
	},
	{
		{},
		{1, 0, 1},
		{1, 1},
		{1, 1, 1},
	},
}

// 两辆车 X 和 Y，将 X 旋转至方向 0 上，Y 跟着 X 一起旋转
// 这样上面就不用写一个很长的 valid 数组
func ok(fromX, toX, fromY, toY int) bool {
	toX = (toX - fromX + 4) % 4
	fromY = (fromY - fromX + 4) % 4
	toY = (toY - fromX + 4) % 4
	return valid[toX][fromY][toY] == 1
}

func trafficCommand(ds []string) int {
	n0, n1, n2, n3 := len(ds[0]), len(ds[1]), len(ds[2]), len(ds[3])
	dp := [21][21][21][21]int{}
	vis := [21][21][21][21]bool{}

	var f func(int, int, int, int) int
	f = func(p0, p1, p2, p3 int) (res int) {
		if p0 == n0 && p1 == n1 && p2 == n2 && p3 == n3 {
			return
		}
		dv := &dp[p0][p1][p2][p3]
		if vis[p0][p1][p2][p3] {
			return *dv
		}
		vis[p0][p1][p2][p3] = true
		defer func() { *dv = res }()
		ps := [4]int{p0, p1, p2, p3}
		res = 1e9
	outer:
		for sub := uint(1); sub < 16; sub++ { // 枚举选择哪几个方向的车通行
			a := [][2]int{}
			for s := sub; s > 0; s &= s - 1 {
				from := bits.TrailingZeros(s)
				if ps[from] == len(ds[from]) {
					continue outer
				}
				to := mp[ds[from][ps[from]]]
				for _, q := range a {
					if !ok(from, to, q[0], q[1]) {
						continue outer
					}
				}
				a = append(a, [2]int{from, to})
			}
			for _, p := range a {
				ps[p[0]]++
			}
			res = min(res, f(ps[0], ps[1], ps[2], ps[3])+1)
			for _, p := range a {
				ps[p[0]]--
			}
		}
		return
	}
	return f(0, 0, 0, 0)
}

func min(a, b int) int {
	if a > b {
		return b
	}
	return a
}
```
