# 1905. 统计子岛屿

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-sub-islands/
- 题目 slug：`count-sub-islands`
- 来源专题：网格图
- 来源分类路径：一、网格图 DFS
- 难度分：1679
- 外部题解来源：https://leetcode.cn/problems/count-sub-islands/solutions/836310/bu-cun-zai-shui-yu-ji-wei-zi-dao-yu-by-e-wbz5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[不存在水域即为子岛屿](https://leetcode.cn/problems/count-sub-islands/solutions/836310/bu-cun-zai-shui-yu-ji-wei-zi-dao-yu-by-e-wbz5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bu-cun-zai-shui-yu-ji-wei-zi-dao-yu-by-e-wbz5`
- topic id：`836310`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

一开始想复杂了，给 $\textit{grid1}$ 的每个岛屿都求了个连通分量，写了一长串代码。

其实注意到只要 $\textit{grid2}$ 的岛屿在 $\textit{grid1}$ 对应位置上不存在水域，那其就是子岛屿。

```go
var dir4 = []struct{ x, y int }{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}
var ok bool

func countSubIslands(grid1, grid2 [][]int) (ans int) {
	n, m := len(grid2), len(grid2[0])
	var dfs func(int, int)
	dfs = func(x, y int) {
		grid2[x][y] = 0
		if grid1[x][y] == 0 { // 找到水域
			ok = false
		}
		for _, d := range dir4 {
			if x, y := x+d.x, y+d.y; 0 <= x && x < n && 0 <= y && y < m && grid2[x][y] > 0 {
				dfs(x, y)
			}
		}
	}
	for i, row := range grid2 {
		for j, v := range row {
			if v > 0 {
				ok = true // 重置
				dfs(i, j)
				if ok {
					ans++
				}
			}
		}
	}
	return
}
```
