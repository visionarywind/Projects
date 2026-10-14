# 2059. 转化数字的最小运算数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-convert-number/
- 题目 slug：`minimum-operations-to-convert-number`
- 来源专题：图论算法
- 来源分类路径：一、图的遍历 / §1.3 图论建模 + BFS 最短路
- 难度分：1850
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-convert-number/solutions/1076009/go-bfs-by-endlesscheng-kymv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[BFS 简洁写法](https://leetcode.cn/problems/minimum-operations-to-convert-number/solutions/1076009/go-bfs-by-endlesscheng-kymv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-bfs-by-endlesscheng-kymv`
- topic id：`1076009`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

问题可以抽象成一张图，一次运算对应着图中的一条边。我们要从 $\textit{start}$ 出发，求到 $\textit{goal}$ 的最短路，这可以用 BFS 来做。

```go
func minimumOperations(nums []int, start, goal int) int {
	vis := [1001]bool{}
	vis[start] = true
	q := []int{start}
	for step := 1; q != nil; step++ {
		tmp := q
		q = nil
		for _, v := range tmp {
			for _, num := range nums {
				for _, x := range []int{v + num, v - num, v ^ num} {
					if x == goal {
						return step
					}
					// 由于超过范围无法继续运算，故不入队
					if 0 <= x && x <= 1000 && !vis[x] {
						vis[x] = true
						q = append(q, x)
					}
				}
			}
		}
	}
	return -1
}
```
