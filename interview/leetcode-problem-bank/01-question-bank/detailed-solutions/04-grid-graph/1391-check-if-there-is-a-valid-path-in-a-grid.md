# 1391. 检查网格中是否存在有效路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/check-if-there-is-a-valid-path-in-a-grid/
- 题目 slug：`check-if-there-is-a-valid-path-in-a-grid`
- 来源专题：网格图
- 来源分类路径：一、网格图 DFS
- 难度分：1746
- 外部题解来源：https://leetcode.cn/problems/check-if-there-is-a-valid-path-in-a-grid/solutions/3950394/li-yong-fang-xiang-xiang-liang-jian-hua-nzn2m/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[利用方向向量简化代码逻辑（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/check-if-there-is-a-valid-path-in-a-grid/solutions/3950394/li-yong-fang-xiang-xiang-liang-jian-hua-nzn2m/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`li-yong-fang-xiang-xiang-liang-jian-hua-nzn2m`
- topic id：`3950394`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

我们需要解决两个关键问题：

1. 站在 $(x,y)$，下一步可以往哪些方向移动？
2. 如何判断相邻街道是否连通？示例 2 的街道不是连通的，无法移动。

对于第一个问题，我们可以创建一个方向向量数组，保存每种街道的移动方向。例如：

- 站在街道 1，我们可以往左或者往右移动，对应的方向向量分别为 $(0,-1)$ 和 $(0,1)$。
- 站在街道 3，我们可以往左或者往下移动，对应的方向向量分别为 $(0,-1)$ 和 $(1,0)$。

对于第二个问题，如果两条相邻街道可以**互相到达**，那么这两条街道就是连通的。

- 如果街道 1 的右边是街道 3，我们可以从街道 1 往右移动到街道 3，也可以从街道 3 往左移动到街道 1。
- 如果要从街道 1 往右移动，那么只要右边相邻街道能往左移动就行，也就是包含往左的方向向量 $(0,-1)$。
- 一般地，如果从当前位置往 $(\textit{dx},\textit{dy})$ 方向移动到相邻街道，那么相邻街道必须包含相反的方向向量 $(-\textit{dx},-\textit{dy})$。

```py [sol-Python3]
DIRS = (
    (),
    ((0, -1), (0, 1)),  # 站在街道 1，可以往左或者往右
    ((-1, 0), (1, 0)),  # 站在街道 2，可以往上或者往下
    ((0, -1), (1, 0)),  # 站在街道 3，可以往左或者往下
    ((0, 1), (1, 0)),   # 站在街道 4，可以往右或者往下
    ((0, -1), (-1, 0)), # 站在街道 5，可以往左或者往上
    ((0, 1), (-1, 0)),  # 站在街道 6，可以往右或者往上
)

class Solution:
    def hasValidPath(self, grid: list[list[int]]) -> bool:
        m, n = len(grid), len(grid[0])
        vis = [[False] * n for _ in range(m)]

        def dfs(x: int, y: int) -> bool:
            if x == m - 1 and y == n - 1:
                return True
            vis[x][y] = True  # 标记 (x, y) 访问过，从而避免重复访问
            for dx, dy in DIRS[grid[x][y]]:  # 枚举下一步往哪走
                i, j = x + dx, y + dy
                if 0 <= i < m and 0 <= j < n and not vis[i][j] and \
                   (-dx, -dy) in DIRS[grid[i][j]] and dfs(i, j):
                    return True
            return False

        return dfs(0, 0)
```

```java [sol-Java]
class Solution {
    private static final int[][][] DIRS = {
        {},
        {{0, -1}, {0, 1}},  // 站在街道 1，可以往左或者往右
        {{-1, 0}, {1, 0}},  // 站在街道 2，可以往上或者往下
        {{0, -1}, {1, 0}},  // 站在街道 3，可以往左或者往下
        {{0, 1}, {1, 0}},   // 站在街道 4，可以往右或者往下
        {{0, -1}, {-1, 0}}, // 站在街道 5，可以往左或者往上
        {{0, 1}, {-1, 0}},  // 站在街道 6，可以往右或者往上
    };

    public boolean hasValidPath(int[][] grid) {
        int m = grid.length;
        int n = grid[0].length;
        boolean[][] vis = new boolean[m][n];
        return dfs(0, 0, grid, vis);
    }

    private boolean dfs(int x, int y, int[][] grid, boolean[][] vis) {
        int m = grid.length;
        int n = grid[x].length;
        if (x == m - 1 && y == n - 1) {
            return true;
        }
        vis[x][y] = true; // 标记 (x, y) 访问过，从而避免重复访问
        for (int[] d : DIRS[grid[x][y]]) { // 枚举下一步往哪走
            int i = x + d[0];
            int j = y + d[1];
            if (0 <= i && i < m && 0 <= j && j < n && !vis[i][j] &&
                contains(grid[i][j], -d[0], -d[1]) && dfs(i, j, grid, vis)) {
                return true;
            }
        }
        return false;
    }

    // 判断街道 street 是否包含移动方向 (dx, dy)
    private boolean contains(int street, int dx, int dy) {
        int[][] ds = DIRS[street];
        return ds[0][0] == dx && ds[0][1] == dy ||
               ds[1][0] == dx && ds[1][1] == dy;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int DIRS[7][2][2] = {
        {},
        {{0, -1}, {0, 1}},  // 站在街道 1，可以往左或者往右
        {{-1, 0}, {1, 0}},  // 站在街道 2，可以往上或者往下
        {{0, -1}, {1, 0}},  // 站在街道 3，可以往左或者往下
        {{0, 1}, {1, 0}},   // 站在街道 4，可以往右或者往下
        {{0, -1}, {-1, 0}}, // 站在街道 5，可以往左或者往上
        {{0, 1}, {-1, 0}},  // 站在街道 6，可以往右或者往上
    };

    // 判断街道 street 是否包含移动方向 (dx, dy)
    bool contains(int street, int dx, int dy) {
        auto& ds = DIRS[street];
        return ds[0][0] == dx && ds[0][1] == dy ||
               ds[1][0] == dx && ds[1][1] == dy;
    }

public:
    bool hasValidPath(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        vector vis(m, vector<int8_t>(n));

        auto dfs = [&](this auto&& dfs, int x, int y) -> bool {
            if (x == m - 1 && y == n - 1) {
                return true;
            }
            vis[x][y] = true; // 标记 (x, y) 访问过，从而避免重复访问
            for (auto& [dx, dy] : DIRS[grid[x][y]]) { // 枚举下一步往哪走
                int i = x + dx, j = y + dy;
                if (0 <= i && i < m && 0 <= j && j < n && !vis[i][j] &&
                    contains(grid[i][j], -dx, -dy) && dfs(i, j)) {
                    return true;
                }
            }
            return false;
        };

        return dfs(0, 0);
    }
};
```

```c [sol-C]
static const int DIRS[7][2][2] = {
    {},
    {{0, -1}, {0, 1}},  // 站在街道 1，可以往左或者往右
    {{-1, 0}, {1, 0}},  // 站在街道 2，可以往上或者往下
    {{0, -1}, {1, 0}},  // 站在街道 3，可以往左或者往下
    {{0, 1}, {1, 0}},   // 站在街道 4，可以往右或者往下
    {{0, -1}, {-1, 0}}, // 站在街道 5，可以往左或者往上
    {{0, 1}, {-1, 0}},  // 站在街道 6，可以往右或者往上
};

// 判断街道 street 是否包含移动方向 (dx, dy)
bool contains(int street, int dx, int dy) {
    return DIRS[street][0][0] == dx && DIRS[street][0][1] == dy ||
           DIRS[street][1][0] == dx && DIRS[street][1][1] == dy;
}

bool hasValidPath(int** grid, int gridSize, int* gridColSize) {
    int m = gridSize, n = gridColSize[0];
    bool** vis = malloc(m * sizeof(bool*));
    for (int i = 0; i < m; i++) {
        vis[i] = calloc(n, sizeof(bool));
    }

    bool dfs(int x, int y) {
        if (x == m - 1 && y == n - 1) {
            return true;
        }
        vis[x][y] = true; // 标记 (x, y) 访问过，从而避免重复访问
        for (int k = 0; k < 2; k++) { // 枚举下一步往哪走
            int* dir = DIRS[grid[x][y]][k];
            int i = x + dir[0], j = y + dir[1];
            if (0 <= i && i < m && 0 <= j && j < n && !vis[i][j] &&
                contains(grid[i][j], -dir[0], -dir[1]) && dfs(i, j)) {
                return true;
            }
        }
        return false;
    }

    bool ans = dfs(0, 0);

    for (int i = 0; i < m; i++) {
        free(vis[i]);
    }
    free(vis);

    return ans;
}
```

```go [sol-Go]
var dirs = [7][2][2]int{
	{},
	{{0, -1}, {0, 1}},  // 站在街道 1，可以往左或者往右
	{{-1, 0}, {1, 0}},  // 站在街道 2，可以往上或者往下
	{{0, -1}, {1, 0}},  // 站在街道 3，可以往左或者往下
	{{0, 1}, {1, 0}},   // 站在街道 4，可以往右或者往下
	{{0, -1}, {-1, 0}}, // 站在街道 5，可以往左或者往上
	{{0, 1}, {-1, 0}},  // 站在街道 6，可以往右或者往上
}

// 判断街道 street 是否包含移动方向 dir
func contains(street int, dir [2]int) bool {
	// 也可以写 slices.Contains(dirs[street][:], dir)
	return dirs[street][0] == dir || dirs[street][1] == dir
}

func hasValidPath(grid [][]int) bool {
	m, n := len(grid), len(grid[0])
	vis := make([][]bool, m)
	for i := range vis {
		vis[i] = make([]bool, n)
	}

	var dfs func(int, int) bool
	dfs = func(x, y int) bool {
		if x == m-1 && y == n-1 {
			return true
		}
		vis[x][y] = true // 标记 (x, y) 访问过，从而避免重复访问
		for _, d := range dirs[grid[x][y]] { // 枚举下一步往哪走
			i, j := x+d[0], y+d[1]
			if 0 <= i && i < m && 0 <= j && j < n && !vis[i][j] &&
				contains(grid[i][j], [2]int{-d[0], -d[1]}) && dfs(i, j) {
				return true
			}
		}
		return false
	}

	return dfs(0, 0)
}
```

```js [sol-JavaScript]
const DIRS = [
    [],
    [[0, -1], [0, 1]],  // 站在街道 1，可以往左或者往右
    [[-1, 0], [1, 0]],  // 站在街道 2，可以往上或者往下
    [[0, -1], [1, 0]],  // 站在街道 3，可以往左或者往下
    [[0, 1], [1, 0]],   // 站在街道 4，可以往右或者往下
    [[0, -1], [-1, 0]], // 站在街道 5，可以往左或者往上
    [[0, 1], [-1, 0]],  // 站在街道 6，可以往右或者往上
];

// 判断街道 street 是否包含移动方向 (dx, dy)
function contains(street, dx, dy) {
    const ds = DIRS[street];
    return ds[0][0] === dx && ds[0][1] === dy ||
           ds[1][0] === dx && ds[1][1] === dy;
}

var hasValidPath = function(grid) {
    const m = grid.length, n = grid[0].length;
    const vis = Array.from({ length: m }, () => Array(n).fill(false));

    function dfs(x, y) {
        if (x === m - 1 && y === n - 1) {
            return true;
        }
        vis[x][y] = true; // 标记 (x, y) 访问过，从而避免重复访问
        for (const [dx, dy] of DIRS[grid[x][y]]) { // 枚举下一步往哪走
            const i = x + dx, j = y + dy;
            if (0 <= i && i < m && 0 <= j && j < n && !vis[i][j] &&
                contains(grid[i][j], -dx, -dy) && dfs(i, j)) {
                return true;
            }
        }
        return false;
    }

    return dfs(0, 0);
};
```

```rust [sol-Rust]
impl Solution {
    const DIRS: [[(i32, i32); 2]; 7] = [
        [(0, 0), (0, 0)],
        [(0, -1), (0, 1)],  // 站在街道 1，可以往左或者往右
        [(-1, 0), (1, 0)],  // 站在街道 2，可以往上或者往下
        [(0, -1), (1, 0)],  // 站在街道 3，可以往左或者往下
        [(0, 1), (1, 0)],   // 站在街道 4，可以往右或者往下
        [(0, -1), (-1, 0)], // 站在街道 5，可以往左或者往上
        [(0, 1), (-1, 0)],  // 站在街道 6，可以往右或者往上
    ];

    // 判断街道 street 是否包含移动方向 dir
    fn contains(street: i32, dir: (i32, i32)) -> bool {
        let ds = Self::DIRS[street as usize];
        ds[0] == dir || ds[1] == dir
    }

    pub fn has_valid_path(grid: Vec<Vec<i32>>) -> bool {
        fn dfs(x: usize, y: usize, grid: &[Vec<i32>], vis: &mut [Vec<bool>]) -> bool {
            let m = grid.len();
            let n = grid[x].len();
            if x == m - 1 && y == n - 1 {
                return true;
            }
            vis[x][y] = true; // 标记 (x, y) 访问过，从而避免重复访问
            for &(dx, dy) in Solution::DIRS[grid[x][y] as usize].iter() { // 枚举下一步往哪走
                let i = x + dx as usize;
                let j = y + dy as usize;
                if i < m && j < n && !vis[i][j] &&
                   Solution::contains(grid[i][j], (-dx, -dy)) && dfs(i, j, grid, vis) {
                    return true;
                }
            }
            false
        }

        let m = grid.len();
        let n = grid[0].len();
        let mut vis = vec![vec![false; n]; m];
        dfs(0, 0, &grid, &mut vis)
    }
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
