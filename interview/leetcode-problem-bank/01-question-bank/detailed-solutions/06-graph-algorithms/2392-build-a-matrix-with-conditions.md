# 2392. 给定条件下构造矩阵

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/build-a-matrix-with-conditions/
- 题目 slug：`build-a-matrix-with-conditions`
- 来源专题：图论算法
- 来源分类路径：二、拓扑排序 / §2.1 拓扑排序
- 难度分：1961
- 外部题解来源：https://leetcode.cn/problems/build-a-matrix-with-conditions/solutions/1781092/by-endlesscheng-gpev/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[拓扑排序 + 构造（Python/Java/C++/Go）](https://leetcode.cn/problems/build-a-matrix-with-conditions/solutions/1781092/by-endlesscheng-gpev/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-gpev`
- topic id：`1781092`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

[视频讲解](https://www.bilibili.com/video/BV1mG411V7fj) 已出炉，包括本题**拓扑排序的原理**，欢迎素质三连，在评论区分享你对这场周赛的看法~

---

#### 提示 1

数字之间的约束只发生在行与行、列于列，而行与列之间没有直接约束。

因此我们可以分别处理行与列中数字的相对顺序，如何求出这个相对顺序呢？

#### 提示 2

拓扑排序。

#### 提示 3

对于 $\textit{rowConditions}$，我们可以从 $\textit{above}_i$ 向 $\textit{below}_i$ 连一条有向边，得到一张有向图。在这张图上跑拓扑排序，得到的拓扑序就是行与行中数字的相对顺序，这样我们就知道了每一行要填哪个数字。如果得到的拓扑序长度不足 $k$，说明图中有环，无法构造，答案不存在。

对 $\textit{colConditions}$ 也执行上述过程，得到每一列要填哪个数字，进而得到每个数字要填到哪一列中，这样我们就知道每一行的数字要填到哪一列了。

#### 答疑

Q：下面拓扑排序的代码，是怎么处理孤立点（没有连边的点，对于本题来说是没有受到任何约束的数字）的？
A：孤立点入度为 $0$，在一开始就入队了，进而在后续的循环中加到了拓扑序中。

#### 复杂度分析

- 时间复杂度：$O(k^2+n+m)$，其中 $n$ 为 $\textit{rowConditions}$ 的长度， $m$ 为 $\textit{colConditions}$ 的长度。
- 空间复杂度：$O(k+n+m)$。忽略返回值占用的空间复杂度。



#### 相关题目

- [210. 课程表 II](https://leetcode.cn/problems/course-schedule-ii/)
- [310. 最小高度树](https://leetcode.cn/problems/minimum-height-trees/)
- [更多题目](https://leetcode.cn/tag/topological-sort/)

```py [sol1-Python3]
class Solution:
    def buildMatrix(self, k: int, rowConditions: List[List[int]], colConditions: List[List[int]]) -> List[List[int]]:
        def topo_sort(edges: List[List[int]]) -> List[int]:
            g = [[] for _ in range(k)]
            in_deg = [0] * k
            for x, y in edges:
                g[x - 1].append(y - 1)  # 顶点编号从 0 开始，方便计算
                in_deg[y - 1] += 1
            order = []
            q = deque(i for i, d in enumerate(in_deg) if d == 0)
            while q:
                x = q.popleft()
                order.append(x)
                for y in g[x]:
                    in_deg[y] -= 1
                    if in_deg[y] == 0:
                        q.append(y)
            return order if len(order) == k else None

        if (row := topo_sort(rowConditions)) is None or (col := topo_sort(colConditions)) is None:
            return []
        pos = {x: i for i, x in enumerate(col)}
        ans = [[0] * k for _ in range(k)]
        for i, x in enumerate(row):
            ans[i][pos[x]] = x + 1
        return ans
```

```java [sol1-Java]
class Solution {
    int[] topoSort(int k, int[][] edges) {
        List<Integer>[] g = new ArrayList[k];
        Arrays.setAll(g, e -> new ArrayList<>());
        var inDeg = new int[k];
        for (var e : edges) {
            int x = e[0] - 1, y = e[1] - 1; // 顶点编号从 0 开始，方便计算
            g[x].add(y);
            ++inDeg[y];
        }

        var order = new ArrayList<Integer>();
        var q = new ArrayDeque<Integer>();
        for (var i = 0; i < k; ++i)
            if (inDeg[i] == 0) q.push(i);
        while (!q.isEmpty()) {
            var x = q.pop();
            order.add(x);
            for (var y : g[x])
                if (--inDeg[y] == 0) q.push(y);
        }
        return order.stream().mapToInt(x -> x).toArray();
    }

    public int[][] buildMatrix(int k, int[][] rowConditions, int[][] colConditions) {
        int[] row = topoSort(k, rowConditions), col = topoSort(k, colConditions);
        if (row.length < k || col.length < k) return new int[][]{};
        var pos = new int[k];
        for (var i = 0; i < k; ++i)
            pos[col[i]] = i;
        var ans = new int[k][k];
        for (var i = 0; i < k; ++i)
            ans[i][pos[row[i]]] = row[i] + 1;
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
    vector<int> topo_sort(int k, vector<vector<int>> &edges) {
        vector<vector<int>> g(k);
        vector<int> in_deg(k);
        for (auto &e : edges) {
            int x = e[0] - 1, y = e[1] - 1; // 顶点编号从 0 开始，方便计算
            g[x].push_back(y);
            ++in_deg[y];
        }

        vector<int> order;
        queue<int> q;
        for (int i = 0; i < k; ++i)
            if (in_deg[i] == 0)
                q.push(i);
        while (!q.empty()) {
            int x = q.front();
            q.pop();
            order.push_back(x);
            for (int y : g[x])
                if (--in_deg[y] == 0)
                    q.push(y);
        }
        return order;
    }

public:
    vector<vector<int>> buildMatrix(int k, vector<vector<int>> &rowConditions, vector<vector<int>> &colConditions) {
        auto row = topo_sort(k, rowConditions), col = topo_sort(k, colConditions);
        if (row.size() < k || col.size() < k) return {};
        vector<int> pos(k);
        for (int i = 0; i < k; ++i)
            pos[col[i]] = i;
        vector<vector<int>> ans(k, vector<int>(k));
        for (int i = 0; i < k; ++i)
            ans[i][pos[row[i]]] = row[i] + 1;
        return ans;
    }
};
```

```go [sol1-Go]
func topoSort(k int, edges [][]int) []int {
	g := make([][]int, k)
	inDeg := make([]int, k)
	for _, e := range edges {
		x, y := e[0]-1, e[1]-1 // 顶点编号从 0 开始，方便计算
		g[x] = append(g[x], y)
		inDeg[y]++
	}
	q := make([]int, 0, k)
	orders := q // 复用队列作为拓扑序
	for i, d := range inDeg {
		if d == 0 {
			q = append(q, i)
		}
	}
	for len(q) > 0 {
		x := q[0]
		q = q[1:]
		for _, y := range g[x] {
			if inDeg[y]--; inDeg[y] == 0 {
				q = append(q, y)
			}
		}
	}
	if cap(q) > 0 {
		return nil
	}
	return orders[:k]
}

func buildMatrix(k int, rowConditions, colConditions [][]int) [][]int {
	row := topoSort(k, rowConditions)
	col := topoSort(k, colConditions)
	if row == nil || col == nil {
		return nil
	}
	pos := make([]int, k)
	for i, v := range col {
		pos[v] = i
	}
	ans := make([][]int, k)
	for i, x := range row {
		ans[i] = make([]int, k)
		ans[i][pos[x]] = x + 1
	}
	return ans
}
```

#### 思考题

如果问题变成一个三维的立方格，再添加一个 $z$ 轴上的数字约束，要怎么做？

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
