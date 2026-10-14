# 2045. 到达目的地的第二短时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/second-minimum-time-to-reach-destination/
- 题目 slug：`second-minimum-time-to-reach-destination`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：2202
- 外部题解来源：https://leetcode.cn/problems/second-minimum-time-to-reach-destination/solutions/1051494/bfs-qiu-ci-duan-lu-fu-xiang-xi-zhu-shi-b-igf7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[BFS 求次短路，附详细注释](https://leetcode.cn/problems/second-minimum-time-to-reach-destination/solutions/1051494/bfs-qiu-ci-duan-lu-fu-xiang-xi-zhu-shi-b-igf7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bfs-qiu-ci-duan-lu-fu-xiang-xi-zhu-shi-b-igf7`
- topic id：`1051494`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

简单说一下为什么可以用 BFS。由于 $\textit{time}$ 和 $\textit{change}$ 是固定的，经过多少条边就知道花费了多少时间。因此这题本质上可以看成边权均为 $1$ 的图，我们要求的就是 $1$ 到 $n$ 的严格次短路的长度，知道长度就知道花费的时间。

下面的代码是直接用 BFS 求次短时间。

```go
func secondMinimum(n int, edges [][]int, time, change int) int {
	g := make([][]int, n)
	for _, e := range edges { // 建图
		v, w := e[0]-1, e[1]-1
		g[v] = append(g[v], w)
		g[w] = append(g[w], v)
	}

	// 传入当前时间 d，返回到达下一个节点的时间
	next := func(d int) int {
		if times := d / change; times%2 == 1 { // 如果红绿灯切换次数为奇数，则现在是红灯
			return (times+1)*change + time // 等绿灯再出发
		}
		return d + time // 绿灯，直接出发
	}

	dis := make([][2]int, n) // 距离数组同时存 [最短路, 次短路]
	dis[0][1] = 1e9
	for i := 1; i < n; i++ {
		dis[i] = [2]int{1e9, 1e9}
	}
	type pair struct{ v, d int }
	q := []pair{{}}
	for len(q) > 0 { // BFS 求最短路和次短路
		p := q[0] // 取队首
		q = q[1:]
		for _, w := range g[p.v] {
			d := next(p.d) // 到达节点 w 的时间
			if d < dis[w][0] { // d 比最短路小，就更新最短路
				dis[w][0] = d
				q = append(q, pair{w, d})
			} else if dis[w][0] < d && d < dis[w][1] { // d 比最短路大又比次短路小，就更新次短路
				dis[w][1] = d
				q = append(q, pair{w, d})
			}
		}
	}
	return dis[n-1][1]
}
```
