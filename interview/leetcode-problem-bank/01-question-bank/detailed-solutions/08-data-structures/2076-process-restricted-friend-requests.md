# 2076. 处理含限制条件的好友请求

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/process-restricted-friend-requests/
- 题目 slug：`process-restricted-friend-requests`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.2 进阶
- 难度分：2131
- 外部题解来源：https://leetcode.cn/problems/process-restricted-friend-requests/solutions/1100831/bing-cha-ji-by-endlesscheng-8ipg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(nlogn) 做法：用并查集维护朋友关系，同时用哈希表维护代表元之间能否成为朋友](https://leetcode.cn/problems/process-restricted-friend-requests/solutions/1100831/bing-cha-ji-by-endlesscheng-8ipg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bing-cha-ji-by-endlesscheng-8ipg`
- topic id：`1100831`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

用并查集维护朋友关系。同时我们还要处理哪些用户不能成为朋友，这可以用哈希表来维护。更具体地，我们可以在并查集的代表元（即 `find` 的终点）上记录当前集合与哪些用户不能成为朋友。
 
如果两个用户不在同一集合且可以成为朋友，那么就要用并查集合并这两个集合，同时把与其中一个集合的不能成为朋友的用户添加到另一个集合上。这里的一个优化点是，合并集合时，总是从小的集合合并到大的集合上。

时间复杂度：由于在合并集合时，总是从小的合并到大的上，因此时间复杂度与启发式合并相同，为 $O(n\log n)$（这里将输入大小均视作 $O(n)$）。证明[见此](https://oi-wiki.org/graph/dsu-on-tree/#_3)。

![1.png](https://pic.leetcode.cn/1636866538-kxqvVg-1.png)

```go
func friendRequests(n int, restrictions [][]int, requests [][]int) []bool {
	fa := make([]int, n) // 初始化并查集
	for i := range fa {
		fa[i] = i
	}
	var find func(int) int
	find = func(x int) int {
		if fa[x] != x {
			fa[x] = find(fa[x])
		}
		return fa[x]
	}

	cant := make([]map[int]bool, n)
	for i := range cant {
		cant[i] = map[int]bool{}
	}
	for _, r := range restrictions { // 初始化无法成为朋友的用户
		v, w := r[0], r[1]
		cant[v][w] = true
		cant[w][v] = true // 双向
	}

	ans := make([]bool, len(requests))
	for i, r := range requests {
		v, w := find(r[0]), find(r[1])
		if v == w { // 已经是朋友（直接或间接）
			ans[i] = true
			continue
		}
		if cant[v][w] { // 无法成为朋友
			continue
		}
		ans[i] = true
		if len(cant[v]) > len(cant[w]) { // 启发式合并：总是从小的集合合并到大的集合上
			v, w = w, v
		}
		for x := range cant[v] { // 将 cant[v] 中的代表元合并到 cant[w] 上
			x = find(x)
			cant[w][x] = true
			cant[x][w] = true // 双向
		}
		fa[v] = w
	}
	return ans
}
```
