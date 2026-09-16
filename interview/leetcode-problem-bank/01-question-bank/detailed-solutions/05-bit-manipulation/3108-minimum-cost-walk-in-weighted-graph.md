# 3108. 带权图里旅途的最小代价

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-walk-in-weighted-graph/
- 题目 slug：`minimum-cost-walk-in-weighted-graph`
- 来源专题：位运算
- 来源分类路径：三、与或（AND/OR）的性质
- 难度分：2109
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-walk-in-weighted-graph/solutions/2727290/xian-xing-zuo-fa-dfspythonjavacgo-by-end-i0gg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：DFS / 并查集（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-cost-walk-in-weighted-graph/solutions/2727290/xian-xing-zuo-fa-dfspythonjavacgo-by-end-i0gg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xian-xing-zuo-fa-dfspythonjavacgo-by-end-i0gg`
- topic id：`2727290`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

分类讨论：

- $s$ 和 $t$ 不在同一个连通块中。答案是 $-1$。
- $s$ 和 $t$ 在同一个连通块中。由于 AND 的性质是 **AND 的数字越多，结果越小**。在可以重复经过边的前提下，最优方案是把 $s$ 所在连通块内的边都走一遍。

所以我们需要知道 $s$ 和 $t$ 在哪个连通块，以及连通块内边权的 AND 是多少。

这可以用 DFS 或者并查集实现。请看 [视频讲解](https://www.bilibili.com/video/BV1ut421H7Wv/) 第四题，欢迎点赞关注！

代码实现时，可以把 AND 的初始值设为 $-1$，因为其二进制中的数都是 $1$，与任何 $x$ 求 AND 的结果都是 $x$。

## 方法一：DFS

```py [sol-Python3]
class Solution:
    def minimumCost(self, n: int, edges: List[List[int]], query: List[List[int]]) -> List[int]:
        g = [[] for _ in range(n)]
        for x, y, w in edges:
            g[x].append((y, w))
            g[y].append((x, w))

        def dfs(x: int) -> int:
            and_ = -1
            ids[x] = len(cc_and)  # 记录每个点所在连通块的编号
            for y, w in g[x]:
                and_ &= w
                if ids[y] < 0:  # 没有访问过
                    and_ &= dfs(y)
            return and_

        ids = [-1] * n  # 记录每个点所在连通块的编号
        cc_and = []  # 记录每个连通块的边权的 AND
        for i in range(n):
            if ids[i] < 0:
                cc_and.append(dfs(i))

        return [-1 if ids[s] != ids[t] else cc_and[ids[s]]
                for s, t in query]
```

```java [sol-Java]
class Solution {
    public int[] minimumCost(int n, int[][] edges, int[][] query) {
        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0], y = e[1], w = e[2];
            g[x].add(new int[]{y, w});
            g[y].add(new int[]{x, w});
        }

        int[] ids = new int[n]; // 记录每个点所在连通块的编号
        Arrays.fill(ids, -1);
        List<Integer> ccAnd = new ArrayList<>(); // 记录每个连通块的边权的 AND
        for (int i = 0; i < n; i++) {
            if (ids[i] < 0) {
                ccAnd.add(dfs(i, ccAnd.size(), g, ids));
            }
        }

        int[] ans = new int[query.length];
        for (int i = 0; i < query.length; i++) {
            int s = query[i][0], t = query[i][1];
            ans[i] = ids[s] != ids[t] ? -1 : ccAnd.get(ids[s]);
        }
        return ans;
    }

    private int dfs(int x, int curId, List<int[]>[] g, int[] ids) {
        ids[x] = curId; // 记录每个点所在连通块的编号
        int and = -1;
        for (int[] e : g[x]) {
            and &= e[1];
            if (ids[e[0]] < 0) { // 没有访问过
                and &= dfs(e[0], curId, g, ids);
            }
        }
        return and;
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<vector<pair<int, int>>> g;
    vector<int> cc_and, ids;

    int dfs(int x) {
        ids[x] = cc_and.size(); // 记录每个点所在连通块的编号
        int and_ = -1;
        for (auto &[y, w]: g[x]) {
            and_ &= w;
            if (ids[y] < 0) { // 没有访问过
                and_ &= dfs(y);
            }
        }
        return and_;
    }

public:
    vector<int> minimumCost(int n, vector<vector<int>> &edges, vector<vector<int>> &query) {
        g.resize(n);
        for (auto &e: edges) {
            int x = e[0], y = e[1], w = e[2];
            g[x].emplace_back(y, w);
            g[y].emplace_back(x, w);
        }

        ids.resize(n, -1); // 记录每个点所在连通块的编号
        for (int i = 0; i < n; i++) {
            if (ids[i] < 0) { // 没有访问过
                cc_and.push_back(dfs(i)); // 记录每个连通块的边权的 AND
            }
        }

        vector<int> ans;
        ans.reserve(query.size()); // 预分配空间
        for (auto &q: query) {
            int s = q[0], t = q[1];
            ans.push_back(ids[s] != ids[t] ? -1 : cc_and[ids[s]]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumCost(n int, edges, query [][]int) []int {
	type edge struct{ to, w int }
	g := make([][]edge, n)
	for _, e := range edges {
		x, y, w := e[0], e[1], e[2]
		g[x] = append(g[x], edge{y, w})
		g[y] = append(g[y], edge{x, w})
	}

	ids := make([]int, n) // 记录每个点所在连通块的编号
	for i := range ids {
		ids[i] = -1
	}
	ccAnd := []int{} // 记录每个连通块的边权的 AND
	var dfs func(int) int
	dfs = func(x int) int {
		ids[x] = len(ccAnd) // 记录每个点所在连通块的编号
		and := -1
		for _, e := range g[x] {
			and &= e.w
			if ids[e.to] < 0 { // 没有访问过
				and &= dfs(e.to)
			}
		}
		return and
	}
	for i, id := range ids {
		if id < 0 { // 没有访问过
			ccAnd = append(ccAnd, dfs(i)) // 记录每个连通块的边权的 AND
		}
	}

	ans := make([]int, len(query))
	for i, q := range query {
		s, t := q[0], q[1]
		if ids[s] != ids[t] {
			ans[i] = -1
		} else {
			ans[i] = ccAnd[ids[s]]
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m+q)$，其中 $m$ 为 $\textit{edges}$ 的长度，$q$ 为 $\textit{query}$ 的长度。
- 空间复杂度：$\mathcal{O}(n+m)$。返回值不计入。

## 方法二：并查集

```py [sol-Python3]
class Solution:
    def minimumCost(self, n: int, edges: List[List[int]], query: List[List[int]]) -> List[int]:
        fa = list(range(n))
        and_ = [-1] * n

        def find(x: int) -> int:
            if fa[x] != x:
                fa[x] = find(fa[x])
            return fa[x]

        for x, y, w in edges:
            x = find(x)
            y = find(y)
            and_[y] &= w
            if x != y:
                and_[y] &= and_[x]
                fa[x] = y

        return [-1 if find(s) != find(t) else and_[find(s)]
                for s, t in query]
```

```java [sol-Java]
class Solution {
    public int[] minimumCost(int n, int[][] edges, int[][] query) {
        int[] fa = new int[n];
        for (int i = 0; i < n; i++) {
            fa[i] = i;
        }
        int[] and = new int[n];
        Arrays.fill(and, -1);

        for (int[] e : edges) {
            int x = find(e[0], fa);
            int y = find(e[1], fa);
            and[y] &= e[2];
            if (x != y) {
                and[y] &= and[x];
                fa[x] = y;
            }
        }

        int[] ans = new int[query.length];
        for (int i = 0; i < query.length; i++) {
            int s = query[i][0], t = query[i][1];
            ans[i] = find(s, fa) != find(t, fa) ? -1 : and[find(s, fa)];
        }
        return ans;
    }

    private int find(int x, int[] fa) {
        if (fa[x] != x) {
            fa[x] = find(fa[x], fa);
        }
        return fa[x];
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<int> fa, and_;

    int find(int x) {
        return fa[x] == x ? x : fa[x] = find(fa[x]);
    };

public:
    vector<int> minimumCost(int n, vector<vector<int>> &edges, vector<vector<int>> &query) {
        fa.resize(n);
        iota(fa.begin(), fa.end(), 0);
        and_.resize(n, -1);
        for (auto &e: edges) {
            int x = find(e[0]);
            int y = find(e[1]);
            and_[y] &= e[2];
            if (x != y) {
                and_[y] &= and_[x];
                fa[x] = y;
            }
        }

        vector<int> ans;
        ans.reserve(query.size()); // 预分配空间
        for (auto &q: query) {
            int s = q[0], t = q[1];
            ans.push_back(find(s) != find(t) ? -1 : and_[find(s)]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumCost(n int, edges, query [][]int) []int {
	fa := make([]int, n)
	and := make([]int, n)
	for i := range fa {
		fa[i] = i
		and[i] = -1
	}
	var find func(int) int
	find = func(x int) int {
		if fa[x] != x {
			fa[x] = find(fa[x])
		}
		return fa[x]
	}

	for _, e := range edges {
		x, y := find(e[0]), find(e[1])
		and[y] &= e[2]
		if x != y {
			and[y] &= and[x]
			fa[x] = y
		}
	}

	ans := make([]int, len(query))
	for i, q := range query {
		s, t := q[0], q[1]
		if find(s) != find(t) {
			ans[i] = -1
		} else {
			ans[i] = and[find(s)]
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+m+q)\log n)$，其中 $m$ 为 $\textit{edges}$ 的长度，$q$ 为 $\textit{query}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。返回值不计入。

## 相关题目

[图论题单](https://leetcode.cn/circle/discuss/01LUak/) 中的 DFS。

## 其它题单

- [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
- [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
- [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
- [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
- [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
- [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `三、与或（AND/OR）的性质`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、与或（AND/OR）的性质`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
