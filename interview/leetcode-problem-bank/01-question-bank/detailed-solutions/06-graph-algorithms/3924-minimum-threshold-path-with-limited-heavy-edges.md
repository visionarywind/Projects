# 3924. 有限重边的最小阈值路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-threshold-path-with-limited-heavy-edges/
- 题目 slug：`minimum-threshold-path-with-limited-heavy-edges`
- 来源专题：图论算法
- 来源分类路径：三、最短路 / §3.1 单源最短路：Dijkstra 算法
- 难度分：2080
- 外部题解来源：https://leetcode.cn/problems/minimum-threshold-path-with-limited-heavy-edges/solutions/3966077/er-fen-da-an-0-1-bfspythonjavacgo-by-end-meqe/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分答案 + 0-1 BFS（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-threshold-path-with-limited-heavy-edges/solutions/3966077/er-fen-da-an-0-1-bfspythonjavacgo-by-end-meqe/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-0-1-bfspythonjavacgo-by-end-meqe`
- topic id：`3966077`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

$\textit{threshold}$ 越大，重边越少，越能找到满足要求的路径。

$\textit{threshold}$ 越小，重边越多，越无法找到满足要求的路径。

据此，可以**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

现在问题转化成一个判定性问题：

- 给定 $\textit{threshold}$，能否找到一条从 $\textit{source}$ 到 $\textit{target}$ 的路径，其中至多有 $k$ 条边权大于 $\textit{threshold}$ 的边？

如果能找到这样的路径，说明答案 $\le \textit{threshold}$，否则答案 $>\textit{threshold}$。

把边权 $\le \textit{threshold}$ 的边视作 $0$，边权 $> \textit{threshold}$ 的边视作 $1$，问题变成 [3286. 穿越网格图的安全路径](https://leetcode.cn/problems/find-a-safe-walk-through-a-grid/)。这可以用 **0-1 BFS** 解决，见 [我的题解](https://leetcode.cn/problems/find-a-safe-walk-through-a-grid/solutions/2917627/0-1-bfs-xian-xing-zuo-fa-pythonjavacgo-b-zlzq/)。

### 细节

下面代码采用开区间二分。使用闭区间或者半闭半开区间也是可以的，喜欢哪种写法就用哪种。

- 开区间左端点初始值：$-1$。题目要求 $\textit{threshold}$ 是非负数。
- 开区间右端点初始值：$\max(w_i)$。此时所有边都是轻边，只要起点终点是连通的，就一定能找到路径。

为了兼容起点终点不连通的情况，可以把开区间右端点初始化为 $\max(w_i)+1$。如果二分结果为 $\max(w_i)+1$，说明起点终点不连通，返回 $-1$。

> 对于开区间写法，简单来说 `check(mid) == true` 时更新的是谁，最后就返回谁。相比其他二分写法，开区间写法不需要思考加一减一等细节，更简单。推荐使用开区间写二分。

[本题视频讲解](https://www.bilibili.com/video/BV1uB5L6YEeX/?t=26m46s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minimumThreshold(self, n: int, edges: List[List[int]], source: int, target: int, k: int) -> int:
        g = [[] for _ in range(n)]
        max_wt = 0
        for x, y, wt in edges:
            g[x].append((y, wt))
            g[y].append((x, wt))
            max_wt = max(max_wt, wt)

        def check(threshold: int) -> bool:
            dis = [inf] * n
            dis[source] = 0
            q = deque([(source, 0)])

            while q:
                x, d = q.popleft()
                if x == target:
                    return True
                if d > dis[x]:
                    continue
                for y, w in g[x]:
                    wt = 1 if w > threshold else 0
                    new_dis = d + wt
                    if new_dis < dis[y]:
                        dis[y] = new_dis
                        if wt == 0:
                            q.appendleft((y, new_dis))
                        elif new_dis <= k:
                            q.append((y, new_dis))

            return False

        ans = bisect_left(range(max_wt + 1), True, key=check)
        return -1 if ans > max_wt else ans
```

```py [sol-Python3 写法二]
class Solution:
    def minimumThreshold(self, n: int, edges: List[List[int]], source: int, target: int, k: int) -> int:
        g = [[] for _ in range(n)]
        for x, y, wt in edges:
            g[x].append((y, wt))
            g[y].append((x, wt))

        def check(threshold: int) -> bool:
            dis = [inf] * n
            dis[source] = 0
            q = deque([(source, 0)])

            while q:
                x, d = q.popleft()
                if x == target:
                    return True
                if d > dis[x]:
                    continue
                for y, w in g[x]:
                    wt = 1 if w > threshold else 0
                    new_dis = d + wt
                    if new_dis < dis[y]:
                        dis[y] = new_dis
                        if wt == 0:
                            q.appendleft((y, new_dis))
                        elif new_dis <= k:
                            q.append((y, new_dis))

            return False

        # 用反证法可以证明，（如果起点终点连通）答案一定在 weights 中
        weights = [0] + sorted(set(e[2] for e in edges))
        i = bisect_left(weights, True, key=check)
        return -1 if i == len(weights) else weights[i]
```

```java [sol-Java]
class Solution {
    public int minimumThreshold(int n, int[][] edges, int source, int target, int k) {
        List<int[]>[] g = new ArrayList[n];
        Arrays.setAll(g, _ -> new ArrayList<>());
        int maxWt = 0;
        for (int[] e : edges) {
            int x = e[0], y = e[1], wt = e[2];
            g[x].add(new int[]{y, wt});
            g[y].add(new int[]{x, wt});
            maxWt = Math.max(maxWt, wt);
        }

        int left = -1, right = maxWt + 1;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            if (check(mid, g, source, target, k)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right > maxWt ? -1 : right;
    }

    private boolean check(int threshold, List<int[]>[] g, int source, int target, int k) {
        int[] dis = new int[g.length];
        Arrays.fill(dis, Integer.MAX_VALUE);
        dis[source] = 0;
        Deque<int[]> q = new ArrayDeque<>();
        q.offerLast(new int[]{source, 0});

        while (!q.isEmpty()) {
            int[] p = q.pollFirst();
            int x = p[0], d = p[1];
            if (x == target) {
                return true;
            }
            if (d > dis[x]) {
                continue;
            }
            for (int[] e : g[x]) {
                int y = e[0];
                int wt = e[1] > threshold ? 1 : 0;
                int newDis = d + wt;
                if (newDis < dis[y]) {
                    dis[y] = newDis;
                    if (wt == 0) {
                        q.offerFirst(new int[]{y, newDis});
                    } else if (newDis <= k) {
                        q.offerLast(new int[]{y, newDis});
                    }
                }
            }
        }

        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumThreshold(int n, vector<vector<int>>& edges, int source, int target, int k) {
        vector<vector<pair<int, int>>> g(n);
        int max_wt = 0;
        for (auto& e : edges) {
            int x = e[0], y = e[1], wt = e[2];
            g[x].emplace_back(y, wt);
            g[y].emplace_back(x, wt);
            max_wt = max(max_wt, wt);
        }

        auto check = [&](int threshold) -> bool {
            vector<int> dis(n, INT_MAX);
            dis[source] = 0;
            deque<pair<int, int>> q = {{source, 0}};

            while (!q.empty()) {
                auto [x, d] = q.front();
                q.pop_front();
                if (x == target) {
                    return true;
                }
                if (d > dis[x]) {
                    continue;
                }
                for (auto [y, w] : g[x]) {
                    int wt = w > threshold;
                    int new_dis = d + wt;
                    if (new_dis < dis[y]) {
                        dis[y] = new_dis;
                        if (wt == 0) {
                            q.emplace_front(y, new_dis);
                        } else if (new_dis <= k) {
                            q.emplace_back(y, new_dis);
                        }
                    }
                }
            }

            return false;
        };

        int left = -1, right = max_wt + 1;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right > max_wt ? -1 : right;
    }
};
```

```go [sol-Go]
func minimumThreshold(n int, edges [][]int, source int, target int, k int) int {
	type edge struct{ to, wt int }
	g := make([][]edge, n)
	maxWt := 0
	for _, e := range edges {
		x, y, wt := e[0], e[1], e[2]
		g[x] = append(g[x], edge{y, wt})
		g[y] = append(g[y], edge{x, wt})
		maxWt = max(maxWt, wt)
	}

	dis := make([]int, n)
	ans := sort.Search(maxWt+1, func(threshold int) bool {
		for i := range dis {
			dis[i] = math.MaxInt
		}
		dis[source] = 0

		type pair struct{ x, d int }
		ql, qr := []pair{{source, dis[source]}}, []pair{} // 模拟双端队列

		for len(ql) > 0 || len(qr) > 0 {
			var p pair
			if len(ql) > 0 {
				ql, p = ql[:len(ql)-1], ql[len(ql)-1] // 队首出
			} else {
				p, qr = qr[0], qr[1:] // 队尾出
			}

			x := p.x
			if x == target {
				return true
			}

			if p.d > dis[x] {
				continue
			}

			for _, e := range g[x] {
				y := e.to
				wt := 0
				if e.wt > threshold {
					wt = 1
				}
				newDis := p.d + wt
				if newDis < dis[y] {
					dis[y] = newDis
					if wt == 0 {
						ql = append(ql, pair{y, newDis}) // 加到队首
					} else if newDis <= k {
						qr = append(qr, pair{y, newDis}) // 加到队尾
					}
				}
			}
		}

		return false
	})

	if ans > maxWt { // 图不连通
		return -1
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+m)\log U)$ 或 $\mathcal{O}((n+m)\log m)$，其中 $m$ 是 $\textit{edges}$ 的长度，$U=\max(w_i)$。普通写法二分 $\mathcal{O}(\log U)$ 次，每次需要 $\mathcal{O}(n+m)$ 的时间。$\mathcal{O}((n+m)\log m)$ 的写法见【Python3 写法二】。
- 空间复杂度：$\mathcal{O}(n+m)$。

## 专题训练

1. 二分题单的「**§2.1 求最小**」。
2. 网格图题单的「**三、网格图 0-1 BFS**」。
3. 图论题单的「**§3.1 单源最短路：Dijkstra 算法**」。

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

本题来自 `三、最短路 / §3.1 单源最短路：Dijkstra 算法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、最短路 / §3.1 单源最短路：Dijkstra 算法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
