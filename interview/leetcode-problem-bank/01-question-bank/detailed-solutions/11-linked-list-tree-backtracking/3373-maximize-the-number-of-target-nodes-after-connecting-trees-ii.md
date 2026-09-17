# 3373. 连接两棵树后最大目标节点数目 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximize-the-number-of-target-nodes-after-connecting-trees-ii/
- 题目 slug：`maximize-the-number-of-target-nodes-after-connecting-trees-ii`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.2 自顶向下 DFS
- 难度分：2162
- 外部题解来源：https://leetcode.cn/problems/maximize-the-number-of-target-nodes-after-connecting-trees-ii/solutions/3006331/an-qi-ou-fen-lei-pythonjavacgo-by-endles-dweg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[黑白染色（Python/Java/C++/Go）](https://leetcode.cn/problems/maximize-the-number-of-target-nodes-after-connecting-trees-ii/solutions/3006331/an-qi-ou-fen-lei-pythonjavacgo-by-endles-dweg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`an-qi-ou-fen-lei-pythonjavacgo-by-endles-dweg`
- topic id：`3006331`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

## 分析

对于一棵树，我们把这棵树的所有节点染成黑色或者白色，规则如下：

- 黑色节点的所有邻居都是白色。
- 白色节点的所有邻居都是黑色。

> 这个想法来自国际象棋的棋盘：所有黑色格子的四方向邻居都是白色格子，所有白色格子的四方向邻居都是黑色格子。也可以从图论的角度理解，因为树一定是二分图。

染色后，从任意节点出发，每走一步，节点的颜色都会改变。所以：

- 从某个节点走奇数步之后，一定会走到异色节点上。
- 从某个节点走偶数步之后，一定会走到同色节点上。

所以从**任意**黑色节点出发，所找到的目标节点，一定都是黑色；从**任意**白色节点出发，所找到的目标节点，一定都是白色。

不妨从节点 $0$ 开始 DFS。（你想从其他节点开始 DFS 也可以。）

## 第二棵树

对于第二棵树，我们把其中的节点分成两个集合：

- 集合 $A$：到节点 $0$ 的距离是偶数的点。其大小记作 $\textit{cnt}_2[0]$。
- 集合 $B$：到节点 $0$ 的距离是奇数的点。其大小记作 $\textit{cnt}_2[1]$。

分类讨论：

- 如果 $\textit{cnt}_2[0] > \textit{cnt}_2[1]$ ，那么第一棵树的节点 $i$ 应当连到集合 $B$ 中的任意节点，这样节点 $i$ 在第二棵树中的目标节点的个数为 $\textit{cnt}_2[0]$。
- 否则，第一棵树的节点 $i$ 应当连到集合 $A$ 中的任意节点，这样节点 $i$ 在第二棵树中的目标节点的个数为 $\textit{cnt}_2[1]$。

所以节点 $i$ 在第二棵树中，最多有

$$
\textit{max}_2 =  \max(\textit{cnt}_2[0],\textit{cnt}_2[1])
$$

个目标节点。

> 注意本题保证 $n\ge 2$ 且 $m\ge 2$。如果 $n=1$ 且 $m=1$，则不能用上式计算，需要特判这种情况。

## 第一棵树

对于第一棵树，我们把其中的节点分成两个集合：

- 集合 $A$：到节点 $0$ 的距离是偶数的点。
- 集合 $B$：到节点 $0$ 的距离是奇数的点。

分类讨论：

- 如果节点 $i$ 在集合 $A$ 中，那么它的目标节点也必然在集合 $A$ 中。
- 如果节点 $i$ 在集合 $B$ 中，那么它的目标节点也必然在集合 $B$ 中。

所以 $\textit{answer}[i]$ 等于节点 $i$ 所属集合的大小，加上 $\textit{max}_2$。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1tAzoY1EUN/?t=32m17s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def count(self, edges: List[List[int]]) -> Tuple[List[List[int]], List[int]]:
        g = [[] for _ in range(len(edges) + 1)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        cnt = [0, 0]
        def dfs(x: int, fa: int, d: int) -> None:
            cnt[d] += 1
            for y in g[x]:
                if y != fa:
                    dfs(y, x, d ^ 1)
        dfs(0, -1, 0)
        return g, cnt

    def maxTargetNodes(self, edges1: List[List[int]], edges2: List[List[int]]) -> List[int]:
        _, cnt2 = self.count(edges2)
        max2 = max(cnt2)

        g, cnt1 = self.count(edges1)
        ans = [max2] * len(g)
        def dfs(x: int, fa: int, d: int) -> None:
            ans[x] += cnt1[d]
            for y in g[x]:
                if y != fa:
                    dfs(y, x, d ^ 1)
        dfs(0, -1, 0)
        return ans
```

```java [sol-Java]
class Solution {
    public int[] maxTargetNodes(int[][] edges1, int[][] edges2) {
        List<Integer>[] g2 = buildTree(edges2);
        int[] cnt2 = new int[2];
        dfs(0, -1, 0, g2, cnt2);
        int max2 = Math.max(cnt2[0], cnt2[1]);

        List<Integer>[] g1 = buildTree(edges1);
        int[] cnt1 = new int[2];
        dfs(0, -1, 0, g1, cnt1);

        int[] ans = new int[g1.length];
        Arrays.fill(ans, max2);
        dfs1(0, -1, 0, g1, cnt1, ans);
        return ans;
    }

    private List<Integer>[] buildTree(int[][] edges) {
        List<Integer>[] g = new ArrayList[edges.length + 1];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            g[x].add(y);
            g[y].add(x);
        }
        return g;
    }

    private void dfs(int x, int fa, int d, List<Integer>[] g, int[] cnt) {
        cnt[d]++;
        for (int y : g[x]) {
            if (y != fa) {
                dfs(y, x, d ^ 1, g, cnt);
            }
        }
    }

    private void dfs1(int x, int fa, int d, List<Integer>[] g, int[] cnt1, int[] ans) {
        ans[x] += cnt1[d];
        for (int y : g[x]) {
            if (y != fa) {
                dfs1(y, x, d ^ 1, g, cnt1, ans);
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> maxTargetNodes(vector<vector<int>>& edges1, vector<vector<int>>& edges2) {
        auto count = [](vector<vector<int>>& edges) {
            vector<vector<int>> g(edges.size() + 1);
            for (auto& e : edges) {
                int x = e[0], y = e[1];
                g[x].push_back(y);
                g[y].push_back(x);
            }

            array<int, 2> cnt{};
            auto dfs = [&](this auto&& dfs, int x, int fa, int d) -> void {
                cnt[d]++;
                for (int y : g[x]) {
                    if (y != fa) {
                        dfs(y, x, d ^ 1);
                    }
                }
            };
            dfs(0, -1, 0);
            return pair(g, cnt);
        };

        auto [_, cnt2] = count(edges2);
        int max2 = max(cnt2[0], cnt2[1]);

        auto [g, cnt1] = count(edges1);
        vector<int> ans(g.size(), max2);
        auto dfs = [&](this auto&& dfs, int x, int fa, int d) -> void {
            ans[x] += cnt1[d];
            for (int y : g[x]) {
                if (y != fa) {
                    dfs(y, x, d ^ 1);
                }
            }
        };
        dfs(0, -1, 0);
        return ans;
    }
};
```

```go [sol-Go]
func count(edges [][]int) (g [][]int, cnt [2]int) {
	g = make([][]int, len(edges)+1)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x)
	}

	var dfs func(int, int, int)
	dfs = func(x, fa, d int) {
		cnt[d]++
		for _, y := range g[x] {
			if y != fa {
				dfs(y, x, d^1)
			}
		}
	}
	dfs(0, -1, 0)
	return
}

func maxTargetNodes(edges1, edges2 [][]int) []int {
	_, cnt2 := count(edges2)
	max2 := max(cnt2[0], cnt2[1])

	g, cnt1 := count(edges1)
	ans := make([]int, len(g))
	var dfs func(int, int, int)
	dfs = func(x, fa, d int) {
		ans[x] = cnt1[d] + max2
		for _, y := range g[x] {
			if y != fa {
				dfs(y, x, d^1)
			}
		}
	}
	dfs(0, -1, 0)
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 是 $\textit{edges}_1$ 的长度，$m$ 是 $\textit{edges}_2$ 的长度。
- 空间复杂度：$\mathcal{O}(n+m)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. 【本题相关】[链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

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
