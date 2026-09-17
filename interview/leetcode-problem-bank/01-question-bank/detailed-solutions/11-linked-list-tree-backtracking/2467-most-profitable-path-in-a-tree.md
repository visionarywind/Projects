# 2467. 树上最大得分和路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/most-profitable-path-in-a-tree/
- 题目 slug：`most-profitable-path-in-a-tree`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.2 自顶向下 DFS
- 难度分：2053
- 外部题解来源：https://leetcode.cn/problems/most-profitable-path-in-a-tree/solutions/1964916/liang-bian-dfs-by-endlesscheng-da7j/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两遍 DFS（视频讲解）](https://leetcode.cn/problems/most-profitable-path-in-a-tree/solutions/1964916/liang-bian-dfs-by-endlesscheng-da7j/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-bian-dfs-by-endlesscheng-da7j`
- topic id：`1964916`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

[视频讲解](https://www.bilibili.com/video/BV1gd4y1b7qj) 已出炉，欢迎点赞三连，在评论区分享你对这场双周赛的看法~

---

1. DFS，求出 $\textit{bob}$ 到 $0$ 的路径上，Bob 经过每个点的时间 $\textit{bobTime}$。
2. DFS，从 $0$ 到每个叶节点，按照题目要求累加 $\textit{amount}$，在叶子节点上更新答案的最大值。

```py [sol1-Python3]
class Solution:
    def mostProfitablePath(self, edges: List[List[int]], bob: int, amount: List[int]) -> int:
        n = len(edges) + 1
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)  # 建树

        bob_time = [n] * n  # bobTime[x] 表示 bob 访问节点 x 的时间
        def dfs_bob(x: int, fa: int, t: int) -> bool:
            if x == 0:
                bob_time[x] = t
                return True
            for y in g[x]:
                if y != fa and dfs_bob(y, x, t + 1):
                    bob_time[x] = t  # 只有可以到达 0 才标记访问时间
                    return True
            return False
        dfs_bob(bob, -1, 0)

        g[0].append(-1)  # 防止把根节点当作叶子
        ans = -inf
        def dfs_alice(x: int, fa: int, alice_time: int, tot: int) -> None:
            if alice_time < bob_time[x]:
                tot += amount[x]
            elif alice_time == bob_time[x]:
                tot += amount[x] // 2
            if len(g[x]) == 1:  # 叶子
                nonlocal ans
                ans = max(ans, tot)  # 更新答案
                return
            for y in g[x]:
                if y != fa:
                    dfs_alice(y, x, alice_time + 1, tot)
        dfs_alice(0, -1, 0, 0)
        return ans
```

```go [sol1-Go]
func mostProfitablePath(edges [][]int, bob int, amount []int) int {
	n := len(edges) + 1
	g := make([][]int, n)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x) // 建树
	}

	bobTime := make([]int, n) // bobTime[x] 表示 bob 访问节点 x 的时间
	for i := range bobTime {
		bobTime[i] = n // 也可以初始化成 inf
	}
	var dfsBob func(int, int, int) bool
	dfsBob = func(x, fa, t int) bool {
		if x == 0 {
			bobTime[x] = t
			return true
		}
		for _, y := range g[x] {
			if y != fa && dfsBob(y, x, t+1) {
				bobTime[x] = t // 只有可以到达 0 才标记访问时间
				return true
			}
		}
		return false
	}
	dfsBob(bob, -1, 0)

	g[0] = append(g[0], -1) // 防止把根节点当作叶子
	ans := math.MinInt32
	var dfsAlice func(int, int, int, int)
	dfsAlice = func(x, fa, aliceTime, sum int) {
		if aliceTime < bobTime[x] {
			sum += amount[x]
		} else if aliceTime == bobTime[x] {
			sum += amount[x] / 2
		}
		if len(g[x]) == 1 { // 叶子
			ans = max(ans, sum) // 更新答案
			return
		}
		for _, y := range g[x] {
			if y != fa {
				dfsAlice(y, x, aliceTime+1, sum)
			}
		}
	}
	dfsAlice(0, -1, 0, 0)
	return ans
}

func max(a, b int) int { if b > a { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为树的节点个数，即 $\textit{edges}$ 的长度加一。
- 空间复杂度：$O(n)$。

## 本地原创解析

### 1. 题意重述

本题来自 `三、一般树 / §3.2 自顶向下 DFS`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.2 自顶向下 DFS`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
