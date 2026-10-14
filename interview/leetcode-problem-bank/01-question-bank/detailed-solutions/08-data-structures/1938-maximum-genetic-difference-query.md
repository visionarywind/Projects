# 1938. 查询最大基因差

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-genetic-difference-query/
- 题目 slug：`maximum-genetic-difference-query`
- 来源专题：常用数据结构
- 来源分类路径：六、字典树（trie） / §6.4 0-1 字典树（异或字典树）
- 难度分：2503
- 外部题解来源：https://leetcode.cn/problems/maximum-genetic-difference-query/solutions/882220/chi-xian-zi-dian-shu-by-endlesscheng-ojnw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[离线+字典树](https://leetcode.cn/problems/maximum-genetic-difference-query/solutions/882220/chi-xian-zi-dian-shu-by-endlesscheng-ojnw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`chi-xian-zi-dian-shu-by-endlesscheng-ojnw`
- topic id：`882220`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

首先离线询问，将询问按照 $\textit{node}_i$ 分组。

然后根据 $\textit{parents}$ 建树，从根开始 DFS，每访问一个节点 $v$，就将其插入字典树，然后回答当前节点 $v$ 对应的所有询问，最后在递归结束时将 $v$ 从字典树中删去。

如果你不熟悉如何用字典树回答询问，可以先做完这道模板题：[421. 数组中两个数的最大异或值](https://leetcode.cn/problems/maximum-xor-of-two-numbers-in-an-array/)。


```go
type node struct {
	son [2]*node
	cnt int
}
type trie struct{ root *node }

func (t *trie) put(v int) *node {
	o := t.root
	for i := 17; i >= 0; i-- {
		b := v >> i & 1
		if o.son[b] == nil {
			o.son[b] = &node{}
		}
		o = o.son[b]
		o.cnt++
	}
	return o
}

func (t *trie) del(v int) *node {
	o := t.root
	for i := 17; i >= 0; i-- {
		o = o.son[v>>i&1]
		o.cnt-- // 删除操作只需要减少 cnt 就行，cnt 为 0 就视作删掉了该节点
	}
	return o
}

func (t *trie) maxXor(v int) (ans int) {
	o := t.root
	for i := 17; i >= 0; i-- {
		b := v >> i & 1
		if o.son[b^1] != nil && o.son[b^1].cnt > 0 {
			ans |= 1 << i
			b ^= 1
		}
		o = o.son[b]
	}
	return
}

func maxGeneticDifference(parents []int, queries [][]int) []int {
	n := len(parents)
	// 建树
	g := make([][]int, n)
	var root int
	for v, pa := range parents {
		if pa == -1 {
			root = v
		} else {
			g[pa] = append(g[pa], v)
		}
	}

	// 离线，将查询分组
	type query struct{ val, i int }
	qs := make([][]query, n)
	for i, q := range queries {
		qs[q[0]] = append(qs[q[0]], query{q[1], i})
	}

	ans := make([]int, len(queries))
	t := &trie{&node{}}
	// 遍历整棵树，每访问一个节点就将其插入 trie 树，访问结束时将其从 trie 中删去
	var dfs func(int)
	dfs = func(v int) {
		t.put(v)
		for _, q := range qs[v] {
			ans[q.i] = t.maxXor(q.val)
		}
		for _, w := range g[v] {
			dfs(w)
		}
		t.del(v)
	}
	dfs(root)
	return ans
}
```
