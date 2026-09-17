# 图论算法

- 来源 URL：https://leetcode.cn/circle/discuss/01LUak/
- 来源标题：分享丨【算法题单】图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）
- 抓取时间：2026-09-17 13:14:52 +0800
- 授权状态：authorized-by-user-confirmation
- 导入状态：preview

## 授权导入：专题/分类解析

> 本节按用户确认的授权导入前提保存来源专题中的分类说明、套路说明和学习建议。题目清单本身仍以 `01-question-bank/by-source-category/` 为准。

### 一、图的遍历 / §1.1 深度优先搜索（DFS）

找连通块、判断是否有环（如 207 题）等。部分题目**做法不止一种**。
模板（计算每个连通块的大小）：
```py [sol-Python3]
def solve(n: int, edges: list[list[int]]) -> list[int]:
    # 节点编号从 0 到 n-1
    g = [[] for _ in range(n)]
    for x, y in edges:
        g[x].append(y)
        g[y].append(x)  # 无向图
    vis = [False] * n
    def dfs(x: int) -> int:
        vis[x] = True  # 避免重复访问节点

### 一、图的遍历 / §1.2 广度优先搜索（BFS）

求最短路等。要求边权都是 $1$（或者说都是同一个正数）。
模板（单源最短路）：
```py [sol-Python3]
# 计算从 start 到各个节点的最短路长度
# 如果节点不可达，则最短路长度为 -1
# 节点编号从 0 到 n-1，边权均为 1
def bfs(n: int, edges: list[list[int]], start: int) -> list[int]:
    g = [[] for _ in range(n)]
    for x, y in edges:
        g[x].append(y)
        g[y].append(x)  # 无向图
    dis = [-1] * n  # -1 表示尚未访问到

### 一、图的遍历 / §1.3 图论建模 + BFS 最短路

把状态抽象成图上的点，用 BFS 遍历这张图，计算从初始状态到目标状态的最短路长度。
可以锻炼状态设计能力。
注：关于**网格图**的 DFS 和 BFS，请看 [网格图题单](https://leetcode.cn/circle/discuss/YiXPXW/)。

### 二、拓扑排序

把拓扑排序想象成一个黑盒，给它一堆杂乱的先修课约束，它会给你一个井井有条的课程学习安排。
这一种在图上的「排序」，可以把杂乱的点排成一排。前提条件是图中无环，从而保证每条边都是从排在前面的点，指向排在后面的点。即对于任意有向边 $x\to y$，$x$ 一定在 $y$ 之前。

### 二、拓扑排序 / §2.1 拓扑排序

```py [sol-Python3]
# 返回有向无环图（DAG）的其中一个拓扑序
# 如果图中有环，返回空列表
# 节点编号从 0 到 n-1
def topologicalSort(n: int, edges: list[list[int]]) -> list[int]:
    g = [[] for _ in range(n)]
    in_deg = [0] * n
    for x, y in edges:
        g[x].append(y)
        in_deg[y] += 1  # 统计 y 的先修课数量
    topo_order = []
    q = deque(i for i, d in enumerate(in_deg) if d == 0)  # 没有先修课，可以直接上

### 三、最短路 / §3.1 单源最短路：Dijkstra 算法

[Dijkstra 算法介绍](https://leetcode.cn/problems/network-delay-time/solution/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)
```py [sol-Python3]
# 返回从起点 start 到每个点的最短路长度 dis，如果节点 x 不可达，则 dis[x] = math.inf
# 要求：没有负数边权
# 时间复杂度 O(n + mlogm)，其中 m 是 edges 的长度。注意堆中有 O(m) 个元素
def shortestPathDijkstra(n: int, edges: list[list[int]], start: int) -> list[int]:
    # 注：如果节点编号从 1 开始（而不是从 0 开始），可以把 n 加一
    g = [[] for _ in range(n)]  # 邻接表
    for x, y, wt in edges:
        g[x].append((y, wt))
        # g[y].append((x, wt))  # 无向图加上这行
    dis = [math.inf] * n

### 三、最短路 / §3.2 全源最短路：Floyd 算法

Floyd 算法本质是三维 DP。理解 DP 一定要基于空间优化前的写法。
[带你发明 Floyd 算法：从记忆化搜索到递推](https://leetcode.cn/problems/find-the-city-with-the-smallest-number-of-neighbors-at-a-threshold-distance/solution/dai-ni-fa-ming-floyd-suan-fa-cong-ji-yi-m8s51/)
```py [sol-Python3]
# 返回一个二维列表，其中 (i,j) 这一项表示从 i 到 j 的最短路长度
# 如果无法从 i 到 j，则最短路长度为 math.inf
# 允许负数边权
# 如果计算完毕后，存在 i，使得从 i 到 i 的最短路长度小于 0，说明图中有负环
# 节点编号从 0 到 n-1
# 时间复杂度 O(n^3 + m)，其中 m 是 edges 的长度
def shortestPathFloyd(self, n: int, edges: list[list[int]]) -> list[list[int]]:
    f = [[math.inf] * n for _ in range(n)]
    for i in range(n):

### 四、最小生成树

涉及到 Kruskal 算法和 Prim 算法。前者一般用于稀疏图，后者一般用于稠密图。
> 注：如果要求最大生成树，把边权从大到小排序。
Kruskal 算法模板（用到了并查集，完整模板见 [数据结构题单](https://leetcode.cn/circle/discuss/mOr1u6/)）：
```py [sol-Python3]
class UnionFind:
    def __init__(self, n: int):
        # 一开始有 n 个集合 {0}, {1}, ..., {n-1}
        # 集合 i 的代表元是自己
        self._fa = list(range(n))  # 代表元
        self.cc = n  # 连通块个数
    # 返回 x 所在集合的代表元
    # 同时做路径压缩，也就是把 x 所在集合中的所有元素的 fa 都改成代表元

### 五、欧拉路径/欧拉回路

涉及到 Hierholzer 算法。

### 六、强连通分量/双连通分量

涉及到 Tarjan 算法。

### 七、二分图染色

模板（交替染色法）：
```py [sol-Python3]
# 返回图的二染色
# 如果是二分图，返回每个节点的颜色，用 1 和 2 表示两种颜色
# 如果不是二分图，返回空列表
# 时间复杂度 O(n+m)，n 是点数，m 是边数
def colorBipartite(n: int, edges: list[list[int]]) -> list[int]:
    # 建图（节点编号从 0 到 n-1）
    g = [[] for _ in range(n)]
    for x, y in edges:
        g[x].append(y)
        g[y].append(x)

### 八、网络流

由于有其他做法（比如状压 DP），难度分仅供参考。

### 十、树上算法

见 [链表、树、回溯](https://leetcode.cn/circle/discuss/K0n2gO/) 题单的第三章节。

### 关联题单

- 关于**网格图**的 DFS 和 BFS，见 [网格图题单](https://leetcode.cn/circle/discuss/YiXPXW/)。

### 算法题单

如果你发现有题目可以补充进来，欢迎评论反馈。

## 授权题解链接索引

| 分类路径 | 标题 | 链接 | 导入状态 |
|---|---|---|---|
| - | - | - | no-solution-link-detected |

## 本地原创增强解析

图论算法 应优先沿用来源页的标题层级学习：先做低难度分题目，用同一套路反复观察边界条件；再回到进阶题处理多约束、多状态或跨专题组合。

### 分类层级说明

本专题的本地分类路径完全来自来源页面的 Markdown 标题层级，后续单题详解会回链到这些路径。

### 建议刷题节奏

先完成每个小节前 3–5 题，确认模板和边界；再按难度分上升补齐。遇到看完题解仍无法复述关键观察的题，先标记复盘，不阻塞下一小节。
