# 网格图

- 来源 URL：https://leetcode.cn/circle/discuss/YiXPXW/
- 来源标题：分享丨【算法题单】网格图（DFS/BFS/综合应用）
- 抓取时间：2026-09-17 13:14:52 +0800
- 授权状态：authorized-by-user-confirmation
- 导入状态：preview

## 授权导入：专题/分类解析

> 本节按用户确认的授权导入前提保存来源专题中的分类说明、套路说明和学习建议。题目清单本身仍以 `01-question-bank/by-source-category/` 为准。

### 网格图

> 小小贪吃蛇，移动花样多。
平移加旋转，为把迷宫破。
代码复代码，bug 何其多。
六步化一步，AC 定我夺。
—— [1210. 穿过迷宫的最少移动次数](https://leetcode.cn/problems/minimum-moves-to-reach-target-with-rotations/)

### 一、网格图 DFS

适用于需要计算连通块个数、大小的题目。
部分题目做法不止一种，也可以用 BFS 或并查集解决。
二叉树 DFS 与网格图 DFS 的区别：
|   | **二叉树**  |  **网格图** |
|---|---|---|
| **递归入口**  |  根节点  |  网格图的某个格子  |
| **递归方向**  |  左儿子和右儿子 |  一般为左右上下的相邻格子  |
| **递归边界**  |  空节点（或者叶节点） | 出界、遇到障碍或者已访问  |
模板（计算每个连通块的大小）：
```py [sol-Python3]
# 返回网格图 grid 每个连通块的大小
# 时间复杂度 O(mn)

### 二、网格图 BFS

适用于需要计算最短距离（最短路）的题目。
DFS 是不撞南墙不回头；BFS 是往水塘中扔石头（起点），荡起一圈圈涟漪（先访问近的，再访问远的）。
模板（单源最短路）：
```py [sol-Python3]
# 返回从 (start_x, start_y) 出发，到其余格子的最短距离
# 时间复杂度 O(mn)
def bfsGrid(grid: List[List[str]], start_x: int, start_y: int) -> List[List[int]]:
    m, n = len(grid), len(grid[0])
    dis = [[-1] * n for _ in range(m)]
    dis[start_x][start_y] = 0
    q = deque([(start_x, start_y)])
    while q:

### 三、网格图 0-1 BFS

边权只有 $0$ 和 $1$ 的题目，也可以用 BFS 做。

### 四、网格图 Dijkstra

见 [图论题单](https://leetcode.cn/circle/discuss/01LUak/) 中的「**§3.1 单源最短路：Dijkstra 算法**」，我标记了网格图的题目。

### 思考题

欢迎在评论区发表你的思路。

### 算法题单

如果你发现有题目可以补充进来，欢迎评论反馈。

## 授权题解链接索引

| 分类路径 | 标题 | 链接 | 导入状态 |
|---|---|---|---|
| - | - | - | no-solution-link-detected |

## 本地原创增强解析

网格图 应优先沿用来源页的标题层级学习：先做低难度分题目，用同一套路反复观察边界条件；再回到进阶题处理多约束、多状态或跨专题组合。

### 分类层级说明

本专题的本地分类路径完全来自来源页面的 Markdown 标题层级，后续单题详解会回链到这些路径。

### 建议刷题节奏

先完成每个小节前 3–5 题，确认模板和边界；再按难度分上升补齐。遇到看完题解仍无法复述关键观察的题，先标记复盘，不阻塞下一小节。
