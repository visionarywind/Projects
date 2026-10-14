# 2049. 统计最高分的节点数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-nodes-with-the-highest-score/
- 题目 slug：`count-nodes-with-the-highest-score`
- 来源专题：链表、树与回溯
- 来源分类路径：二、二叉树 / §2.16 其他
- 难度分：1912
- 外部题解来源：https://leetcode.cn/problems/count-nodes-with-the-highest-score/solutions/1063923/liang-bian-dfs-by-endlesscheng-utf8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[DFS 的同时计算分数](https://leetcode.cn/problems/count-nodes-with-the-highest-score/solutions/1063923/liang-bian-dfs-by-endlesscheng-utf8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-bian-dfs-by-endlesscheng-utf8`
- topic id：`1063923`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

对于一个点 $v$，删除与 $v$ 相连的边，剩余部分可以分为两类：

- 以 $v$ 的子节点为根的子树；
- 整棵树去掉以 $v$ 为根的子树后，剩余的部分。

我们可以从 $0$ 出发，对整棵树跑一次 DFS，在算出子树大小的同时，根据题目要求计算出分数。

```go 
func countHighestScoreNodes(parents []int) (ans int) {
	n := len(parents)
	g := make([][]int, n)
	for w := 1; w < n; w++ {
		v := parents[w]
		g[v] = append(g[v], w) // 建树
	}

	maxScore := 0
	var dfs func(int) int
	dfs = func(v int) int {
		size, score := 1, 1
		for _, w := range g[v] {
			sz := dfs(w)
			size += sz // 统计以 v 为根的子树的大小
			score *= sz // 由于是二叉树所以 score 最大约为 (1e5/3)^3，在 64 位整数范围内
		}
		if v > 0 {
			score *= n - size // 如果不是根节点，还要乘上「整棵树去掉以 v 为根的子树后，剩余的部分」
		}
		if score > maxScore {
			maxScore, ans = score, 1
		} else if score == maxScore {
			ans++
		}
		return size
	}
	dfs(0)
	return
}
```
