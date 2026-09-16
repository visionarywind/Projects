# 2146. 价格范围内最高排名的 K 样物品

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/k-highest-ranked-items-within-a-price-range/
- 题目 slug：`k-highest-ranked-items-within-a-price-range`
- 来源专题：网格图
- 来源分类路径：二、网格图 BFS
- 难度分：1837
- 外部题解来源：https://leetcode.cn/problems/k-highest-ranked-items-within-a-price-range/solutions/1226661/fen-ceng-bfs-de-tong-shi-pai-xu-by-endle-ash6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分层 BFS 的同时排序（Python/Java/C++/Go）](https://leetcode.cn/problems/k-highest-ranked-items-within-a-price-range/solutions/1226661/fen-ceng-bfs-de-tong-shi-pai-xu-by-endle-ash6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-ceng-bfs-de-tong-shi-pai-xu-by-endle-ash6`
- topic id：`1226661`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

不了解网格图 BFS 的同学可以先看看 [1765. 地图中的最高点](https://leetcode.cn/problems/map-of-highest-peak/)。

对于此题，我们可以直接 BFS 然后按照题目要求排序。但由于排序时距离为第一关键字，因此我们可以采用分层 BFS 的做法，即每次 BFS 时向外扩展一层，然后对扩展出的这些位置排序，取将价格在 $[\textit{low},\textit{high}]$ 中的物品位置加入答案。

```py [sol1-Python3]
class Solution:
    def highestRankedKItems(self, grid: List[List[int]], pricing: List[int], start: List[int], k: int) -> List[List[int]]:
        ans = []
        m, n = len(grid), len(grid[0])
        low, high = pricing
        sx, sy = start
        vis = {(sx, sy)}
        q = [(sx, sy)]
        while q:  # 分层 BFS
            # 此时 q 内所有元素到起点的距离均相同，因此按照题目中的第 2~4 关键字排序后，就可以将价格在 [low,high] 内的位置加入答案
            q.sort(key=lambda p: (grid[p[0]][p[1]], p))
            ans.extend(p for p in q if low <= grid[p[0]][p[1]] <= high)
            if len(ans) >= k: return ans[:k]
            tmp = q
            q = []
            for i, j in tmp:
                for x, y in (i, j - 1), (i, j + 1), (i - 1, j), (i + 1, j):
                    if 0 <= x < m and 0 <= y < n and grid[x][y] and (x, y) not in vis:
                        vis.add((x, y))
                        q.append((x, y))
        return ans
```

```java [sol1-Java]
class Solution {
    static final int[][] dirs = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    public List<List<Integer>> highestRankedKItems(int[][] grid, int[] pricing, int[] start, int k) {
        var ans = new ArrayList<List<Integer>>();
        int m = grid.length, n = grid[0].length;
        var vis = new boolean[m][n];
        vis[start[0]][start[1]] = true;
        var q = new ArrayList<int[]>();
        q.add(start);
        while (!q.isEmpty()) { // 分层 BFS
            // 此时 q 内所有元素到起点的距离均相同，因此按照题目中的第 2~4 关键字排序后，就可以将价格在 [low,high] 内的位置加入答案
            q.sort((a, b) -> {
                int pa = grid[a[0]][a[1]], pb = grid[b[0]][b[1]];
                return pa != pb ? pa - pb : a[0] != b[0] ? a[0] - b[0] : a[1] - b[1];
            });
            for (var p : q)
                if (pricing[0] <= grid[p[0]][p[1]] && grid[p[0]][p[1]] <= pricing[1]) {
                    ans.add(List.of(p[0], p[1]));
                    if (ans.size() == k) return ans;
                }
            var tmp = q;
            q = new ArrayList<>();
            for (var p : tmp)
                for (var d : dirs) {
                    int x = p[0] + d[0], y = p[1] + d[1];
                    if (0 <= x && x < m && 0 <= y && y < n && !vis[x][y] && grid[x][y] > 0) {
                        vis[x][y] = true;
                        q.add(new int[]{x, y});
                    }
                }
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

class Solution {
public:
    vector<vector<int>>
    highestRankedKItems(vector<vector<int>> &grid, vector<int> &pricing, vector<int> &start, int k) {
        vector<vector<int>> ans;
        int m = grid.size(), n = grid[0].size();
        int low = pricing[0], high = pricing[1];
        int sx = start[0], sy = start[1];
        bool vis[m][n];
        memset(vis, 0, sizeof(vis));
        vis[sx][sy] = true;
        vector<pair<int, int>> q = {{sx, sy}};
        while (!q.empty()) { // 分层 BFS
            // 此时 q 内所有元素到起点的距离均相同，因此按照题目中的第 2~4 关键字排序后，就可以将价格在 [low,high] 内的位置加入答案
            sort(q.begin(), q.end(), [&](auto &a, auto &b) {
                int pa = grid[a.first][a.second], pb = grid[b.first][b.second];
                return pa < pb || pa == pb && a < b;
            });
            for (auto &p: q) {
                int g = grid[p.first][p.second];
                if (low <= g && g <= high) {
                    ans.push_back({p.first, p.second});
                    if (ans.size() == k) return ans;
                }
            }
            vector<pair<int, int>> qq;
            for (auto &p: q)
                for (auto &d: dirs) {
                    int x = p.first + d[0], y = p.second + d[1];
                    if (0 <= x && x < m && 0 <= y && y < n && !vis[x][y] && grid[x][y]) {
                        vis[x][y] = true;
                        qq.emplace_back(x, y);
                    }
                }
            q = move(qq);
        }
        return ans;
    }
};
```

```go [sol1-Go]
var dirs = []struct{ x, y int }{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}

func highestRankedKItems(grid [][]int, pricing, start []int, k int) (ans [][]int) {
	m, n := len(grid), len(grid[0])
	low, high := pricing[0], pricing[1]
	sx, sy := start[0], start[1]
	vis := make([][]bool, m)
	for i := range vis {
		vis[i] = make([]bool, n)
	}
	vis[sx][sy] = true
	q := [][]int{{sx, sy}}
	for len(q) > 0 { // 分层 BFS
		// 此时 q 内所有元素到起点的距离均相同，因此按照题目中的第 2~4 关键字排序后，就可以将价格在 [low,high] 内的位置加入答案
		sort.Slice(q, func(i, j int) bool {
			ax, ay, bx, by := q[i][0], q[i][1], q[j][0], q[j][1]
			pa, pb := grid[ax][ay], grid[bx][by]
			return pa < pb || pa == pb && (ax < bx || ax == bx && ay < by)
		})
		l := sort.Search(len(q), func(i int) bool { return grid[q[i][0]][q[i][1]] >= low })
		r := sort.Search(len(q), func(i int) bool { return grid[q[i][0]][q[i][1]] > high })
		ans = append(ans, q[l:r]...)
		if len(ans) >= k {
			return ans[:k]
		}
		tmp := q
		q = nil
		for _, p := range tmp {
			for _, d := range dirs {
				if x, y := p[0]+d.x, p[1]+d.y; 0 <= x && x < m && 0 <= y && y < n && !vis[x][y] && grid[x][y] != 0 {
					vis[x][y] = true
					q = append(q, []int{x, y})
				}
			}
		}
	}
	return
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `二、网格图 BFS`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、网格图 BFS`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
