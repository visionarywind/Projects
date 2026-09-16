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

## 本地原创解析

### 1. 题意重述

本题来自 `五、欧拉路径/欧拉回路`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、欧拉路径/欧拉回路`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
