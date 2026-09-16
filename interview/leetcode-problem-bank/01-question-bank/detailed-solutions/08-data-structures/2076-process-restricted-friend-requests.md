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

## 本地原创解析

### 1. 题意重述

本题来自 `七、并查集 / §7.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
