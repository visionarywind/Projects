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

## 本地原创解析

### 1. 题意重述

本题来自 `二、拓扑排序 / §2.1 拓扑排序`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

### 2. 暴力思路与瓶颈

直接枚举所有候选并逐个重新计算属性，通常会产生 $O(nk)$、$O(n^2)$ 或更高复杂度。瓶颈在于相邻候选之间有大量重复计算。

### 3. 关键观察

相邻状态通常只差少量元素或一个转移边界。只要把重复计算沉淀为可增量维护的统计量、单调结构、状态转移或图搜索标记，就能显著降低复杂度。

### 4. 算法设计

1. 根据题目约束确定窗口、前缀、二分、栈、图搜索、动态规划或数学变换的核心状态。
2. 初始化边界状态。
3. 按来源分类的套路推进枚举或转移，并在状态合法时更新答案。
4. 对边界不足、空状态、重复元素、负数、溢出、取模和不可达状态单独处理。

### 5. 正确性说明

枚举或转移过程覆盖所有合法候选；维护量在每一步与当前候选状态保持一致；答案只在候选合法或状态最优性成立时更新，因此最终结果等于所有合法候选的最优值、计数或可行性判断。

### 6. 复杂度分析

- 时间复杂度：依据具体题解正文确认；常见为 $O(n)$、$O(n\log n)$、$O(nm)$ 或状态数乘转移数。
- 空间复杂度：依据维护状态确认；常见为 $O(1)$、$O(k)$、$O(n)$ 或 DP/图状态规模。

### 7. C++17 实现

```cpp
#include <bits/stdc++.h>
using namespace std;

class Solution {
public:
    // TODO: 根据题目签名补全。当前批次先建立详解结构，代码需按题面签名复核。
};
```

### 8. 样例推演

当前本地层不复制题面样例。导入授权题解正文后，应结合正文中的示例或手工构造小样例，列出状态变化和答案更新时机。

### 9. 易错点

- 更新答案前必须确认当前状态已经合法。
- 删除、回退或转移状态时不要漏更新计数、和、频率表、访问标记或单调结构。
- 若题目含负数、重复值、空集合、取模、长整型溢出或特殊图结构，需单独核对边界。

### 10. 扩展解析

同一分类下的题目通常共享维护框架，差异主要在状态定义和合法性条件。复盘时应总结“状态是什么、何时合法、如何转移、答案如何更新”。

### 11. 同类题迁移

回到来源分类 `二、拓扑排序 / §2.1 拓扑排序`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
