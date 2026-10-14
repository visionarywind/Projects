# 2115. 从给定原材料中找到所有可以做出的菜

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-all-possible-recipes-from-given-supplies/
- 题目 slug：`find-all-possible-recipes-from-given-supplies`
- 来源专题：图论算法
- 来源分类路径：二、拓扑排序 / §2.1 拓扑排序
- 难度分：1679
- 外部题解来源：https://leetcode.cn/problems/find-all-possible-recipes-from-given-supplies/solutions/1176904/go-tuo-bu-pai-xu-by-endlesscheng-lnlg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简洁写法：哈希表+拓扑排序](https://leetcode.cn/problems/find-all-possible-recipes-from-given-supplies/solutions/1176904/go-tuo-bu-pai-xu-by-endlesscheng-lnlg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-tuo-bu-pai-xu-by-endlesscheng-lnlg`
- topic id：`1176904`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

直接用输入的 $\textit{supplies}$ 当作队列的初始元素。

```py [sol1-Python3]
class Solution:
    def findAllRecipes(self, recipes: List[str], ingredients: List[List[str]], supplies: List[str]) -> List[str]:
        g = defaultdict(list)
        deg = {}
        for r, ing in zip(recipes, ingredients):
            for s in ing:
                g[s].append(r)  # 从这道菜的原材料向这道菜连边
            deg[r] = len(ing)
        ans = []
        q = deque(supplies)  # 拓扑排序（用初始原材料当队列）
        while q:
            for r in g[q.popleft()]:
                deg[r] -= 1
                if deg[r] == 0:  # 这道菜的所有原材料我们都有
                    q.append(r)
                    ans.append(r)
        return ans
```

```go [sol1-Go]
func findAllRecipes(recipes []string, ingredients [][]string, q []string) (ans []string) {
	g := map[string][]string{}
	deg := make(map[string]int, len(recipes))
	for i, r := range recipes {
		for _, s := range ingredients[i] {
			g[s] = append(g[s], r) // 从这道菜的原材料向这道菜连边
		}
		deg[r] = len(ingredients[i])
	}
	for len(q) > 0 { // 拓扑排序（这里我们直接用初始原材料当队列）
		s := q[0]
		q = q[1:]
		for _, r := range g[s] {
			if deg[r]--; deg[r] == 0 { // 这道菜的所有原材料我们都有
				q = append(q, r)
				ans = append(ans, r)
			}
		}
	}
	return
}
```
