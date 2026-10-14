# 2097. 合法重新排列数对

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/valid-arrangement-of-pairs/
- 题目 slug：`valid-arrangement-of-pairs`
- 来源专题：图论算法
- 来源分类路径：五、欧拉路径/欧拉回路
- 难度分：2651
- 外部题解来源：https://leetcode.cn/problems/valid-arrangement-of-pairs/solutions/1139613/you-xiang-tu-ou-la-lu-jing-by-endlessche-j2i3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[有向图欧拉路径](https://leetcode.cn/problems/valid-arrangement-of-pairs/solutions/1139613/you-xiang-tu-ou-la-lu-jing-by-endlessche-j2i3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`you-xiang-tu-ou-la-lu-jing-by-endlessche-j2i3`
- topic id：`1139613`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题与 [332. 重新安排行程](https://leetcode.cn/problems/reconstruct-itinerary/) 题一样，都可以抽象成如下问题：
 
> 给你一个有向图，在这个有向图中找到一条欧拉路径。

对于此题，由于题目保证有解，所以有向图肯定是欧拉图或半欧拉图。

我们需要找到一个起点，如果是欧拉图，那么随便找一个点都行；如果是半欧拉图，那么找到一个出度比入度大 $1$ 的点，把它当成起点。然后按照 332 题给出的算法来求解。

```go
func validArrangement(pairs [][]int) [][]int {
	g := map[int][]int{}
	inDeg := map[int]int{}
	for _, p := range pairs {
		v, w := p[0], p[1]
		g[v] = append(g[v], w) // 建图（有向图）
		inDeg[w]++ // 统计入度
	}

	start := 0
	for i, es := range g {
		if len(es) == inDeg[i]+1 { // 半欧拉图：存在出度比入度大 1 的点，那就把它当成起点
			start = i
			break
		}
		start = i // 欧拉图：随便找一个点当起点
	}

	m := len(pairs)
	ans := make([][]int, 0, m)
	var dfs func(int)
	dfs = func(v int) {
		for len(g[v]) > 0 {
			w := g[v][0]
			g[v] = g[v][1:]
			dfs(w)
			ans = append(ans, []int{v, w}) // 记录欧拉路径
		}
	}
	dfs(start)
	for i := 0; i < m/2; i++ {
		ans[i], ans[m-1-i] = ans[m-1-i], ans[i]
	}
	return ans
}
```
