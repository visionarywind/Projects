# 1559. 二维网格图中探测环

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/detect-cycles-in-2d-grid/
- 题目 slug：`detect-cycles-in-2d-grid`
- 来源专题：网格图
- 来源分类路径：一、网格图 DFS
- 难度分：1838
- 外部题解来源：https://leetcode.cn/problems/detect-cycles-in-2d-grid/solutions/3952785/wang-ge-tu-dfspythonjavacgo-by-endlessch-dbxw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[网格图 DFS（Python/Java/C++/Go）](https://leetcode.cn/problems/detect-cycles-in-2d-grid/solutions/3952785/wang-ge-tu-dfspythonjavacgo-by-endlessch-dbxw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`wang-ge-tu-dfspythonjavacgo-by-endlessch-dbxw`
- topic id：`3952785`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

在 DFS 的过程中，如果发现了一个之前访问过的同值格子，那么：

- 如果我们从 $(x,y)$ 移动一步到 $(i,j)$，再从 $(i,j)$ 移动一步到 $(x,y)$，这的确是个环，但长度只有 $2$，不满足要求。所以我们规定，**不能立刻回头**。在 DFS 的过程中，额外传入上一步的位置 $(\textit{px},\textit{py})$，并禁止从当前位置移动到上一步的位置。
- 如果我们从 $(x,y)$ 移动一步到 $(i,j)$，且 $(i,j)\ne (\textit{px},\textit{py})$，且 $(i,j)$ 之前访问过，那么就找到了一个长度至少为 $4$ 的环。

**问**：有没有可能，环长是 $3$？

**答**：对于（四连通的）网格图，这是不可能的。把网格图看成国际象棋棋盘，每走一步，所处格子的颜色切换成另一种颜色。对于一个环，从环上的一点出发，只有走偶数步后，所处格子的颜色才和起始格子的颜色相同。所以对于（四连通的）网格图，**环长一定是偶数**。只要不立刻回头，环长至少为 $4$。

```py [sol-Python3]
class Solution:
    def containsCycle(self, grid: List[List[str]]) -> bool:
        m, n = len(grid), len(grid[0])
        vis = [[False] * n for _ in range(m)]

        def dfs(x: int, y: int, px: int, py: int) -> bool:
            vis[x][y] = True
            for i, j in (x, y - 1), (x, y + 1), (x - 1, y), (x + 1, y):  # 枚举移动方向
                if ((i != px or j != py) and  # (i, j) 不是上一步的格子 (px, py)
                    0 <= i < m and 0 <= j < n and  # (i, j) 没有出界
                    grid[i][j] == grid[x][y] and  # (i, j) 和 (x, y) 的格子值相同
                    (vis[i][j] or dfs(i, j, x, y))):  # 如果之前访问过 (i, j)，那么找到了环，否则继续递归找
                    return True
            return False

        for i in range(m):
            for j in range(n):
                if not vis[i][j] and dfs(i, j, -1, -1):
                    return True
        return False
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 左右上下

    public boolean containsCycle(char[][] grid) {
        int m = grid.length;
        int n = grid[0].length;
        boolean[][] vis = new boolean[m][n];
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (!vis[i][j] && dfs(i, j, -1, -1, grid, vis)) {
                    return true;
                }
            }
        }
        return false;
    }

    private boolean dfs(int x, int y, int px, int py, char[][] grid, boolean[][] vis) {
        vis[x][y] = true;
        for (int[] d : DIRS) { // 枚举移动方向
            int i = x + d[0];
            int j = y + d[1];
            if ((i != px || j != py) && // (i, j) 不是上一步的格子 (px, py)
                0 <= i && i < grid.length && 0 <= j && j < grid[i].length && // (i, j) 没有出界
                grid[i][j] == grid[x][y] && // (i, j) 和 (x, y) 的格子值相同
                (vis[i][j] || dfs(i, j, x, y, grid, vis))) { // 如果之前访问过 (i, j)，那么找到了环，否则继续递归找
                return true;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int DIRS[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 左右上下

public:
    bool containsCycle(vector<vector<char>>& grid) {
        int m = grid.size(), n = grid[0].size();
        vector vis(m, vector<int8_t>(n));

        auto dfs = [&](this auto&& dfs, int x, int y, int px, int py) -> bool {
            vis[x][y] = true;
            for (auto [dx, dy] : DIRS) { // 枚举移动方向
                int i = x + dx, j = y + dy;
                if ((i != px || j != py) && // (i, j) 不是上一步的格子 (px, py)
                    0 <= i && i < m && 0 <= j && j < n && // (i, j) 没有出界
                    grid[i][j] == grid[x][y] && // (i, j) 和 (x, y) 的格子值相同
                    (vis[i][j] || dfs(i, j, x, y))) { // 如果之前访问过 (i, j)，那么找到了环，否则继续递归找
                    return true;
                }
            }
            return false;
        };

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (!vis[i][j] && dfs(i, j, -1, -1)) {
                    return true;
                }
            }
        }
        return false;
    }
};
```

```go [sol-Go]
var dirs = []struct{ x, y int }{{0, -1}, {0, 1}, {-1, 0}, {1, 0}} // 左右上下

func containsCycle(grid [][]byte) bool {
	m, n := len(grid), len(grid[0])
	vis := make([][]bool, m)
	for i := range vis {
		vis[i] = make([]bool, n)
	}

	var dfs func(int, int, int, int) bool
	dfs = func(x, y, px, py int) bool {
		vis[x][y] = true
		for _, d := range dirs { // 枚举移动方向
			i, j := x+d.x, y+d.y
			if (i != px || j != py) && // (i, j) 不是上一步的格子 (px, py)
				0 <= i && i < m && 0 <= j && j < n && // (i, j) 没有出界
				grid[i][j] == grid[x][y] && // (i, j) 和 (x, y) 的格子值相同
				(vis[i][j] || dfs(i, j, x, y)) { // 如果之前访问过 (i, j)，那么找到了环，否则继续递归找
				return true
			}
		}
		return false
	}

	for i, row := range vis {
		for j, b := range row {
			if !b && dfs(i, j, -1, -1) {
				return true
			}
		}
	}
	return false
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(mn)$。

## 专题训练

见下面网格图题单的「**一、网格图 DFS**」。

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

本题来自 `一、网格图 DFS`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、网格图 DFS`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
