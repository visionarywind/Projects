# 2876. 有向图访问计数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-visited-nodes-in-a-directed-graph/
- 题目 slug：`count-visited-nodes-in-a-directed-graph`
- 来源专题：图论算法
- 来源分类路径：二、拓扑排序 / §2.3 基环树
- 难度分：2210
- 外部题解来源：https://leetcode.cn/problems/count-visited-nodes-in-a-directed-graph/solutions/2464852/nei-xiang-ji-huan-shu-pythonjavacgo-by-e-zrzh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板】内向基环树，附题单（Python/Java/C++/Go）](https://leetcode.cn/problems/count-visited-nodes-in-a-directed-graph/solutions/2464852/nei-xiang-ji-huan-shu-pythonjavacgo-by-e-zrzh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nei-xiang-ji-huan-shu-pythonjavacgo-by-e-zrzh`
- topic id：`2464852`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题给出的图叫做**内向基环树**。

之前写过一篇题解，介绍了处理基环树问题的一些通用技巧，请看 [内向基环树：拓扑排序+分类讨论](https://leetcode.cn/problems/maximum-employees-to-be-invited-to-a-meeting/solution/nei-xiang-ji-huan-shu-tuo-bu-pai-xu-fen-c1i1b/)

对于本题来说：

- 对于在基环上的点，其可以访问到的节点数，就是基环的大小。
- 对于不在基环上的点 $x$，其可以访问到的节点数，是基环的大小，再加上点 $x$ 的深度。这里的深度是指以基环上的点 $\textit{root}$ 为根的树枝作为一棵树，点 $x$ 在这棵树中的深度。这可以从 $\textit{root}$ 出发，在反图上 DFS 得到。

注意题目给出的图可能不是连通的，可能有多棵内向基环树。

[本题视频讲解](https://www.bilibili.com/video/BV18j411b7v4/) 第四题。

```py [sol-Python3]
class Solution:
    def countVisitedNodes(self, g: List[int]) -> List[int]:
        n = len(g)
        rg = [[] for _ in range(n)]  # 反图
        deg = [0] * n
        for x, y in enumerate(g):
            rg[y].append(x)
            deg[y] += 1

        # 拓扑排序，剪掉 g 上的所有树枝
        # 拓扑排序后，deg 值为 1 的点必定在基环上，为 0 的点必定在树枝上
        q = deque(i for i, d in enumerate(deg) if d == 0)
        while q:
            x = q.popleft()
            y = g[x]
            deg[y] -= 1
            if deg[y] == 0:
                q.append(y)

        ans = [0] * n
        # 在反图上遍历树枝
        def rdfs(x: int, depth: int) -> None:
            ans[x] = depth
            for y in rg[x]:
                if deg[y] == 0:  # 树枝上的点在拓扑排序后，入度均为 0
                    rdfs(y, depth + 1)
        for i, d in enumerate(deg):
            if d <= 0:
                continue
            ring = []
            x = i
            while True:
                deg[x] = -1  # 将基环上的点的入度标记为 -1，避免重复访问
                ring.append(x)  # 收集在基环上的点
                x = g[x]
                if x == i:
                    break
            for x in ring:
                rdfs(x, len(ring))  # 为方便计算，以 len(ring) 作为初始深度
        return ans
```

```java [sol-Java]
class Solution {
    public int[] countVisitedNodes(List<Integer> edges) {
        int[] g = edges.stream().mapToInt(i -> i).toArray();
        int n = g.length;
        List<Integer>[] rg = new ArrayList[n]; // 反图
        Arrays.setAll(rg, e -> new ArrayList<>());
        int[] deg = new int[n];
        for (int x = 0; x < n; x++) {
            int y = g[x];
            rg[y].add(x);
            deg[y]++;
        }

        // 拓扑排序，剪掉 g 上的所有树枝
        // 拓扑排序后，deg 值为 1 的点必定在基环上，为 0 的点必定在树枝上
        var q = new ArrayDeque<Integer>();
        for (int i = 0; i < n; i++) {
            if (deg[i] == 0) {
                q.add(i);
            }
        }
        while (!q.isEmpty()) {
            int x = q.poll();
            int y = g[x];
            if (--deg[y] == 0) {
                q.add(y);
            }
        }

        int[] ans = new int[n];
        for (int i = 0; i < n; i++) {
            if (deg[i] <= 0) {
                continue;
            }
            var ring = new ArrayList<Integer>();
            for (int x = i; ; x = g[x]) {
                deg[x] = -1; // 将基环上的点的入度标记为 -1，避免重复访问
                ring.add(x); // 收集在基环上的点
                if (g[x] == i) {
                    break;
                }
            }
            for (int r : ring) {
                rdfs(r, ring.size(), rg, deg, ans); // 为方便计算，以 ring.size() 作为初始深度
            }
        }
        return ans;
    }

    // 在反图上遍历树枝
    private void rdfs(int x, int depth, List<Integer>[] rg, int[] deg, int[] ans) {
        ans[x] = depth;
        for (int y : rg[x]) {
            if (deg[y] == 0) { // 树枝上的点在拓扑排序后，入度均为 0
                rdfs(y, depth + 1, rg, deg, ans);
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> countVisitedNodes(vector<int>& g) {
        int n = g.size();
        vector<vector<int>> rg(n); // 反图
        vector<int> deg(n);
        for (int x = 0; x < n; x++) {
            int y = g[x];
            rg[y].push_back(x);
            deg[y]++;
        }

        // 拓扑排序，剪掉 g 上的所有树枝
        // 拓扑排序后，deg 值为 1 的点必定在基环上，为 0 的点必定在树枝上
        queue<int> q;
        for (int i = 0; i < n; i++) {
            if (deg[i] == 0) {
                q.push(i);
            }
        }
        while (!q.empty()) {
            int x = q.front(); q.pop();
            int y = g[x];
            deg[y]--;
            if (deg[y] == 0) {
                q.push(y);
            }
        }

        vector<int> ans(n);

        // 在反图上遍历树枝
        auto rdfs = [&](this auto&& rdfs, int x, int depth) -> void {
            ans[x] = depth;
            for (int y : rg[x]) {
                if (deg[y] == 0) { // 树枝上的点在拓扑排序后，入度均为 0
                    rdfs(y, depth + 1);
                }
            }
        };

        for (int i = 0; i < n; i++) {
            if (deg[i] <= 0) {
                continue;
            }
            vector<int> ring;
            for (int x = i; ; x = g[x]) {
                deg[x] = -1; // 将基环上的点的入度标记为 -1，避免重复访问
                ring.push_back(x); // 收集在基环上的点
                if (g[x] == i) {
                    break;
                }
            }
            for (int x : ring) {
                rdfs(x, ring.size()); // 为方便计算，以 ring.size() 作为初始深度
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
func countVisitedNodes(g []int) []int {
	n := len(g)
	rg := make([][]int, n) // 反图
	deg := make([]int, n)
	for x, y := range g {
		rg[y] = append(rg[y], x)
		deg[y]++
	}

	// 拓扑排序，剪掉 g 上的所有树枝
	// 拓扑排序后，deg 值为 1 的点必定在基环上，为 0 的点必定在树枝上
	q := []int{}
	for i, d := range deg {
		if d == 0 {
			q = append(q, i)
		}
	}
	for len(q) > 0 {
		x := q[0]
		q = q[1:]
		y := g[x]
		deg[y]--
		if deg[y] == 0 {
			q = append(q, y)
		}
	}

	ans := make([]int, n)
	// 在反图上遍历树枝
	var rdfs func(int, int)
	rdfs = func(x, depth int) {
		ans[x] = depth
		for _, y := range rg[x] {
			if deg[y] == 0 { // 树枝上的点在拓扑排序后，入度均为 0
				rdfs(y, depth+1)
			}
		}
	}
	for i, d := range deg {
		if d <= 0 {
			continue
		}
		ring := []int{}
		for x := i; ; x = g[x] {
			deg[x] = -1 // 将基环上的点的入度标记为 -1，避免重复访问
			ring = append(ring, x) // 收集在基环上的点
			if g[x] == i {
				break
			}
		}
		for _, x := range ring {
			rdfs(x, len(ring)) // 为方便计算，以 len(ring) 作为初始深度
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{edges}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

如果输入的是一个 $n$ 个点 $m$ 条边的一般有向图，要怎么做呢？

请学习 SCC 强连通分量。

## 专题训练

见下面图论题单的「**§2.3 基环树**」。

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
