# 3241. 标记所有节点需要的时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/time-taken-to-mark-all-nodes/
- 题目 slug：`time-taken-to-mark-all-nodes`
- 来源专题：动态规划
- 来源分类路径：十二、树形 DP / §12.4 换根 DP
- 难度分：2522
- 外部题解来源：https://leetcode.cn/problems/time-taken-to-mark-all-nodes/solutions/2868276/di-er-lei-huan-gen-dppythonjavacgo-by-en-411w/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板】第二类换根 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/time-taken-to-mark-all-nodes/solutions/2868276/di-er-lei-huan-gen-dppythonjavacgo-by-en-411w/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`di-er-lei-huan-gen-dppythonjavacgo-by-en-411w`
- topic id：`2868276`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置知识**：[【图解】一张图秒懂换根 DP！](https://leetcode.cn/problems/sum-of-distances-in-tree/solution/tu-jie-yi-zhang-tu-miao-dong-huan-gen-dp-6bgb/)

本题相当于对每个节点，计算以该节点为根时，树的高度（最大深度）。

其中从 $x\to y$ 的有向边的边权为 $2 - y\bmod 2$，即当 $y$ 是奇数时，边权为 $1$；当 $y$ 是偶数时，边权为 $2$。

⚠**注意**：如果 $x$ 和 $y$ 的奇偶性不同，那么从 $x\to y$ 的有向边和从 $y\to x$ 的有向边的边权是不一样的。

考虑换根 DP。

首先，通过一次 DFS，计算以 $0$ 为根节点时，树的最大深度。

在 DFS 的过程中，额外保存：

- 子树 $x$ 的**最大**深度 $\textit{maxD}$。
- 子树 $x$ 的**次大**深度 $\textit{maxD}_2$。
- 子树 $x$ 通过其儿子 $\textit{my}$ 取到的最大深度。

然后，再通过一次 DFS，计算出本题的答案。

对于节点 $x$，其答案是以下两种情况的最大值：

- 子树 $x$ 的最大深度。
- $x$ 往上走到某个节点（可以再往下拐弯）的路径长度。

对于第二种情况，可以作为 DFS 的一个参数 $\textit{fromUp}$。

如果 $x$ 的儿子 $y = \textit{my}$，那么往下传入的参数更新为

$$
\max(\textit{fromUp}, \textit{maxD}_2) + 2 - x\bmod 2
$$

如果 $x$ 的儿子 $y\ne \textit{my}$，那么往下传入的参数更新为

$$
\max(\textit{fromUp}, \textit{maxD}) + 2 - x\bmod 2
$$

**注**：我把[【图解】一张图秒懂换根 DP](https://leetcode.cn/problems/sum-of-distances-in-tree/solution/tu-jie-yi-zhang-tu-miao-dong-huan-gen-dp-6bgb/) 这题叫做**第一类换根 DP**，本题需要额外维护次大信息，我称其为**第二类换根 DP**。

[本题视频讲解](https://www.bilibili.com/video/BV1F4421S7XU/?t=17m28s) 第四题，欢迎点赞关注~

```py [sol-Python3]
# 手写 max 更快
max = lambda a, b: b if b > a else a

class Solution:
    def timeTaken(self, edges: List[List[int]]) -> List[int]:
        n = len(edges) + 1
        g = [[] for _ in range(n)]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        # sub_res[x] 保存子树 x 的最大深度，次大深度，以及最大深度要往哪个儿子走
        sub_res = [None] * n
        # 计算 sub_res[x]
        def dfs(x: int, fa: int) -> None:
            max_d = max_d2 = my = 0
            for y in g[x]:
                if y == fa:
                    continue
                dfs(y, x)
                w = 2 - y % 2  # 从 x 到 y 的边权
                max_y = sub_res[y][0] + w  # 从 x 出发，往 y 方向的最大深度
                if max_y > max_d:
                    max_d2 = max_d
                    max_d = max_y
                    my = y
                elif max_y > max_d2:
                    max_d2 = max_y
            sub_res[x] = (max_d, max_d2, my)
        dfs(0, -1)

        # ans[x] 表示当 x 是树根时，整棵树的最大深度
        ans = [0] * n
        # 计算 ans[x]
        def reroot(x: int, fa: int, from_up: int) -> None:
            max_d, max_d2, my = sub_res[x]
            ans[x] = max(max_d, from_up)
            for y in g[x]:
                if y == fa:
                    continue
                # 站在 x 的角度，不往 y 走，能走多远？
                # 要么往上走（from_up），要么往除了 y 的其余子树走（mx），二者取最大值
                mx = max_d if y != my else max_d2
                w = 2 - x % 2  # 从 y 到 x 的边权
                reroot(y, x, max(from_up, mx) + w)  # 对于 y 来说，加上从 y 到 x 的边权
        reroot(0, -1, 0)
        return ans
```

```java [sol-Java]
class Solution {
    public int[] timeTaken(int[][] edges) {
        int n = edges.length + 1;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, _ -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            g[x].add(y);
            g[y].add(x);
        }

        // subRes[x] 保存子树 x 的最大深度，次大深度，以及最大深度要往哪个儿子走
        int[][] subRes = new int[n][];
        dfs(0, -1, g, subRes);

        // ans[x] 表示当 x 是树根时，整棵树的最大深度
        int[] ans = new int[n];
        reroot(0, -1, 0, g, subRes, ans);
        return ans;
    }

    // 计算 subRes[x]
    private void dfs(int x, int fa, List<Integer>[] g, int[][] subRes) {
        int maxD = 0;
        int maxD2 = 0;
        int my = 0;
        for (int y : g[x]) {
            if (y == fa) {
                continue;
            }
            dfs(y, x, g, subRes);
            int w = 2 - y % 2; // 从 x 到 y 的边权
            int maxY = subRes[y][0] + w; // 从 x 出发，往 y 方向的最大深度
            if (maxY > maxD) {
                maxD2 = maxD;
                maxD = maxY;
                my = y;
            } else if (maxY > maxD2) {
                maxD2 = maxY;
            }
        }
        subRes[x] = new int[]{maxD, maxD2, my};
    }

    // 计算 ans[x]
    private void reroot(int x, int fa, int fromUp, List<Integer>[] g, int[][] subRes, int[] ans) {
        int maxD = subRes[x][0];
        int maxD2 = subRes[x][1];
        int my = subRes[x][2];
        ans[x] = Math.max(maxD, fromUp);
        for (int y : g[x]) {
            if (y == fa) {
                continue;
            }
            // 站在 x 的角度，不往 y 走，能走多远？
            // 要么往上走（fromUp），要么往除了 y 的其余子树走（mx），二者取最大值
            int mx = y != my ? maxD : maxD2;
            int w = 2 - x % 2; // 从 y 到 x 的边权
            reroot(y, x, Math.max(fromUp, mx) + w, g, subRes, ans);
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> timeTaken(vector<vector<int>>& edges) {
        int n = edges.size() + 1;
        vector<vector<int>> g(n);
        for (auto& e : edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        // sub_res[x] 保存子树 x 的最大深度，次大深度，以及最大深度要往哪个儿子走
        vector<tuple<int, int, int>> sub_res(n);
        // 计算 sub_res[x]
        auto dfs = [&](this auto&& dfs, int x, int fa) -> void {
            int max_d = 0, max_d2 = 0, my = 0;
            for (int y : g[x]) {
                if (y == fa) {
                    continue;
                }
                dfs(y, x);
                int w = 2 - y % 2; // 从 x 到 y 的边权
                int max_y = get<0>(sub_res[y]) + w; // 从 x 出发，往 y 方向的最大深度
                if (max_y > max_d) {
                    max_d2 = max_d;
                    max_d = max_y;
                    my = y;
                } else if (max_y > max_d2) {
                    max_d2 = max_y;
                }
            }
            sub_res[x] = {max_d, max_d2, my};
        };
        dfs(0, -1);

        // ans[x] 表示当 x 是树根时，整棵树的最大深度
        vector<int> ans(n);
        // 计算 ans[x]
        auto reroot = [&](this auto&& reroot, int x, int fa, int from_up) -> void {
            auto [max_d, max_d2, my] = sub_res[x];
            ans[x] = max(max_d, from_up);
            for (int y : g[x]) {
                if (y == fa) {
                    continue;
                }
                // 站在 x 的角度，不往 y 走，能走多远？
                // 要么往上走（from_up），要么往除了 y 的其余子树走（mx），二者取最大值
                int mx = y != my ? max_d : max_d2;
                int w = 2 - x % 2; // 从 y 到 x 的边权
                reroot(y, x, max(from_up, mx) + w); // 对于 y 来说，加上从 y 到 x 的边权
            }
        };
        reroot(0, -1, 0);
        return ans;
    }
};
```

```go [sol-Go]
func timeTaken(edges [][]int) []int {
	n := len(edges) + 1
	g := make([][]int, n)
	for _, e := range edges {
		x, y := e[0], e[1]
		g[x] = append(g[x], y)
		g[y] = append(g[y], x)
	}

	// subRes[x] 保存子树 x 的最大深度 maxD，次大深度 maxD2，以及最大深度要往儿子 y 走
	subRes := make([]struct{ maxD, maxD2, y int }, n)
	// 计算 subRes[x]
	var dfs func(int, int)
	dfs = func(x, fa int) {
		res := &subRes[x]
		for _, y := range g[x] {
			if y == fa {
				continue
			}
			dfs(y, x)
			w := 2 - y%2 // 从 x 到 y 的边权
			maxD := subRes[y].maxD + w // 从 x 出发，往 y 方向的最大深度
			if maxD > res.maxD {
				res.maxD2 = res.maxD
				res.maxD = maxD
				res.y = y
			} else if maxD > res.maxD2 {
				res.maxD2 = maxD
			}
		}
	}
	dfs(0, -1)

	// ans[x] 表示当 x 是树根时，整棵树的最大深度
	ans := make([]int, n)
	// 计算 ans[x]
	var reroot func(int, int, int)
	reroot = func(x, fa, fromUp int) {
		sub := subRes[x]
		ans[x] = max(sub.maxD, fromUp)
		for _, y := range g[x] {
			if y == fa {
				continue
			}
			// 站在 x 的角度，不往 y 走，能走多远？
			// 要么往上走（fromUp），要么往除了 y 的其余子树走（mx），二者取最大值
			mx := sub.maxD
			if y == sub.y { // 对于 y 来说，上面要选次大的
				mx = sub.maxD2
			}
			w := 2 - x%2 // 从 y 到 x 的边权
			reroot(y, x, max(fromUp, mx)+w) // 对于 y 来说，加上从 y 到 x 的边权
		}
	}
	reroot(0, -1, 0)
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{edges}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

- [CF1822F. Gardening Friends](https://codeforces.com/problemset/problem/1822/F)

更多相似题目，见下面 DP 题单中的「**换根 DP**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `十二、树形 DP / §12.4 换根 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十二、树形 DP / §12.4 换根 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
