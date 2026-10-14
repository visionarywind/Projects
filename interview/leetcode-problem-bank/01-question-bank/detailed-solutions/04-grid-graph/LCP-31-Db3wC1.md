# LCP 31. 变换的迷宫

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/Db3wC1/
- 题目 slug：`Db3wC1`
- 来源专题：网格图
- 来源分类路径：五、综合应用
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/Db3wC1/solutions/702755/dfs-48ms-by-endlesscheng-ckzn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[DFS 48ms](https://leetcode.cn/problems/Db3wC1/solutions/702755/dfs-48ms-by-endlesscheng-ckzn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dfs-48ms-by-endlesscheng-ckzn`
- topic id：`702755`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

```
var dir4 = []struct{ x, y int }{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}

func escapeMaze(g [][]string) bool {
	k, n, m := len(g), len(g[0]), len(g[0][0])
	vis := make([][][][6]bool, k)
	for i := range vis {
		vis[i] = make([][][6]bool, n)
		for j := range vis[i] {
			vis[i][j] = make([][6]bool, m)
		}
	}
	// s 的最低位：是否使用了临时消除术
	// s 的其余位：0-未使用永久消除术，1-当前正位于永久消除的位置，2-已使用永久消除术
	var f func(t, x, y, s int) bool
	f = func(t, x, y, s int) bool {
		if x < 0 || x >= n || y < 0 || y >= m || t+n-1-x+m-1-y >= k || vis[t][x][y][s] {
			return false
		}
		if x == n-1 && y == m-1 {
			return true
		}
		vis[t][x][y][s] = true
		if s>>1 == 1 {
			for _, d := range dir4 {
				if f(t+1, x+d.x, y+d.y, s^6) { // 跳出永久消除的位置
					return true
				}
			}
			return f(t+1, x, y, s)
		}
		if s>>1 == 0 && g[t][x][y] == '#' && f(t, x, y, s|2) { // 尝试使用永久消除术
			return true
		}
		// 使用永久消除术已无法走到终点，遇到障碍只能使用临时消除术
		if g[t][x][y] == '#' {
			if s&1 == 1 {
				return false
			}
			s |= 1
		}
		for _, d := range dir4 {
			if f(t+1, x+d.x, y+d.y, s) {
				return true
			}
		}
		return f(t+1, x, y, s)
	}
	return f(0, 0, 0, 0)
}
```
