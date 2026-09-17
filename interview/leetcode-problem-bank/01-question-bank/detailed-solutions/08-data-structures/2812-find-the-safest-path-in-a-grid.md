# 2812. 找出最安全路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-safest-path-in-a-grid/
- 题目 slug：`find-the-safest-path-in-a-grid`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.2 进阶
- 难度分：2154
- 外部题解来源：https://leetcode.cn/problems/find-the-safest-path-in-a-grid/solutions/2375565/jie-jin-on2-de-zuo-fa-duo-yuan-bfsdao-xu-r5um/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[接近 O(n^2) 的做法：多源BFS+倒序枚举答案+并查集（Python/Java/C++/Go）](https://leetcode.cn/problems/find-the-safest-path-in-a-grid/solutions/2375565/jie-jin-on2-de-zuo-fa-duo-yuan-bfsdao-xu-r5um/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jie-jin-on2-de-zuo-fa-duo-yuan-bfsdao-xu-r5um`
- topic id：`2375565`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

设 $\textit{dis}[i][j]$ 是单元格 $(i,j)$ 到最近的 $1$ 的曼哈顿距离。

安全系数即从 $(0, 0)$ 到 $(n-1,n-1)$ 的路径上的最小的 $\textit{dis}[i][j]$。

**从大到小枚举安全系数**。假设安全系数为 $d$，那么把所有 $\textit{dis}[i][j]\ge d$ 的格子与其四周 $\ge d$ 的格子用**并查集**连起来。如果 $(0,0)$ 和 $(n-1,n-1)$ 连通，那么答案为 $d$。

代码实现时，不需要对每个 $d$ 都遍历整个 $\textit{dis}$，而是**增量地**把恰好等于 $d$ 的 $\textit{dis}[i][j]$ 与其四周 $\ge d$ 的格子用并查集连起来。我们可以把相同的 $\textit{dis}[i][j]$ 分组（记在 $\textit{groups}$ 中），从而避免反复遍历整个 $\textit{dis}$。

计算 $\textit{dis}$ 可以从所有 $1$ 出发，跑一个**多源 BFS**。做法类似 [542. 01 矩阵](https://leetcode.cn/problems/01-matrix/)（那题是从所有 $0$ 出发）。

[视频讲解](https://www.bilibili.com/video/BV1Yr4y1o7aP/) 第三题。

```py [sol-Python3]
class Solution:
    def maximumSafenessFactor(self, grid: List[List[int]]) -> int:
        n = len(grid)
        dis = [[-1] * n for _ in range(n)]
        q = []
        for i, row in enumerate(grid):
            for j, x in enumerate(row):
                if x == 1:
                    dis[i][j] = 0
                    q.append((i, j))

        groups = [q]
        # 多源 BFS
        while q:  
            tmp = q
            q = []
            for i, j in tmp:
                for x, y in (i + 1, j), (i - 1, j), (i, j + 1), (i, j - 1):
                    if 0 <= x < n and 0 <= y < n and dis[x][y] < 0:
                        dis[x][y] = len(groups)
                        q.append((x, y))
            groups.append(q)  # 相同 dis 分组记录

        # 并查集模板
        fa = list(range(n * n))
        def find(x: int) -> int:
            if fa[x] != x:
                fa[x] = find(fa[x])
            return fa[x]

        # 从大到小枚举答案
        for d in range(len(groups) - 2, 0, -1):
            for i, j in groups[d]:
                for x, y in (i + 1, j), (i - 1, j), (i, j + 1), (i, j - 1):
                    if 0 <= x < n and 0 <= y < n and dis[x][y] >= d:
                        fa[find(x * n + y)] = find(i * n + j)
            if find(0) == find(n * n - 1):  # 写这里判断更快些
                return d
        return 0
```

```java [sol-Java]
class Solution {
    private final static int[][] DIRS = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    public int maximumSafenessFactor(List<List<Integer>> grid) {
        int n = grid.size();
        int[][] dis = new int[n][n];
        List<int[]> q = new ArrayList<int[]>();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (grid.get(i).get(j) > 0) {
                    q.add(new int[]{i, j});
                } else {
                    dis[i][j] = -1;
                }
            }
        }

        List<List<int[]>> groups = new ArrayList<List<int[]>>();
        groups.add(q);
        // 多源 BFS
        while (!q.isEmpty()) { 
            List<int[]> tmp = q;
            q = new ArrayList<>();
            for (int[] p : tmp) {
                for (int[] d : DIRS) {
                    int x = p[0] + d[0], y = p[1] + d[1];
                    if (0 <= x && x < n && 0 <= y && y < n && dis[x][y] < 0) {
                        dis[x][y] = groups.size();
                        q.add(new int[]{x, y});
                    }
                }
            }
            groups.add(q); // 相同 dis 分组记录
        }

        // 初始化并查集
        fa = new int[n * n];
        for (int i = 0; i < n * n; i++) {
            fa[i] = i;
        }

        // 从大到小枚举答案
        for (int ans = groups.size() - 2; ans > 0; ans--) {
            for (int[] p : groups.get(ans)) {
                int i = p[0], j = p[1];
                for (int[] d : DIRS) {
                    int x = i + d[0], y = j + d[1];
                    if (0 <= x && x < n && 0 <= y && y < n && dis[x][y] >= ans) {
                        fa[find(x * n + y)] = find(i * n + j);
                    }
                }
            }
            if (find(0) == find(n * n - 1)) { // 写这里判断更快些
                return ans;
            }
        }
        return 0;
    }

    // 并查集模板
    private int[] fa;

    private int find(int x) {
        if (fa[x] != x) {
            fa[x] = find(fa[x]);
        }
        return fa[x];
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

public:
    int maximumSafenessFactor(vector<vector<int>>& grid) {
        int n = grid.size();
        vector dis(n, vector<int>(n, -1));
        vector<pair<int, int>> q;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j]) {
                    dis[i][j] = 0;
                    q.emplace_back(i, j);
                }
            }
        }

        vector<vector<pair<int, int>>> groups = {q};
        // 多源 BFS
        while (!q.empty()) {
            auto tmp = move(q);
            for (auto& [i, j] : tmp) {
                for (auto& [dx, dy] : dirs) {
                    int x = i + dx, y = j + dy;
                    if (0 <= x && x < n && 0 <= y && y < n && dis[x][y] < 0) {
                        dis[x][y] = groups.size();
                        q.emplace_back(x, y);
                    }
                }
            }
            groups.push_back(q); // 相同 dis 分组记录
        }

        // 并查集模板
        vector<int> fa(n * n);
        ranges::iota(fa, 0);
        auto find = [&](this auto&& find, int x) -> int { 
            return fa[x] == x ? x : fa[x] = find(fa[x]);
        };

        // 从大到小枚举答案
        for (int ans = (int) groups.size() - 2; ans > 0; ans--) {
            for (auto& [i, j] : groups[ans]) {
                for (auto& [dx, dy] : dirs) {
                    int x = i + dx, y = j + dy;
                    if (0 <= x && x < n && 0 <= y && y < n && dis[x][y] >= ans) {
                        fa[find(x * n + y)] = find(i * n + j);
                    }
                }
            }
            if (find(0) == find(n * n - 1)) { // 写这里判断更快些
                return ans;
            }
        }
        return 0;
    }
};
```

```go [sol-Go]
type pair struct{ x, y int }
var dir4 = []pair{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}

func maximumSafenessFactor(grid [][]int) int {
	n := len(grid)
	dis := make([][]int, n)
	q := []pair{}
	for i, row := range grid {
		dis[i] = make([]int, n)
		for j, x := range row {
			if x > 0 {
				q = append(q, pair{i, j})
			} else {
				dis[i][j] = -1
			}
		}
	}

	groups := [][]pair{q}
	// 多源 BFS
	for len(q) > 0 {
		tmp := q
		q = nil
		for _, p := range tmp {
			for _, d := range dir4 {
				x, y := p.x+d.x, p.y+d.y
				if 0 <= x && x < n && 0 <= y && y < n && dis[x][y] < 0 {
					dis[x][y] = len(groups)
					q = append(q, pair{x, y})
				}
			}
		}
		groups = append(groups, q) // 相同 dis 分组记录
	}

	// 并查集模板
	fa := make([]int, n*n)
	for i := range fa {
		fa[i] = i
	}
	var find func(int) int
	find = func(x int) int {
		if fa[x] != x {
			fa[x] = find(fa[x])
		}
		return fa[x]
	}

	// 从大到小枚举答案
	for ans := len(groups) - 2; ans > 0; ans-- {
		for _, p := range groups[ans] {
			i, j := p.x, p.y
			for _, d := range dir4 {
				x, y := p.x+d.x, p.y+d.y
				if 0 <= x && x < n && 0 <= y && y < n && dis[x][y] >= ans {
					fa[find(x*n+y)] = find(i*n + j)
				}
			}
		}
		if find(0) == find(n*n-1) { // 写这里判断更快些
			return ans
		}
	}
	return 0
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2\log n)$ 或者 $\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{grid}$ 的长度。时间复杂度取决于并查集的实现，加上按秩合并的话，均摊地说并查集的操作可视作 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 专题训练

1. 网格图题单的「**二、网格图 BFS**」。
2. 数据结构题单的「**七、并查集**」。
3. 二分题单的「**§2.5 最大化最小值**」。

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

本题来自 `七、并查集 / §7.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
