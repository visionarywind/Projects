# 695. 岛屿的最大面积

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/max-area-of-island/
- 题目 slug：`max-area-of-island`
- 来源专题：网格图
- 来源分类路径：一、网格图 DFS
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/max-area-of-island/solutions/3756169/wang-ge-tu-dfspythonjavaccgojsrust-by-en-2hsc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[网格图 DFS（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/max-area-of-island/solutions/3756169/wang-ge-tu-dfspythonjavaccgojsrust-by-en-2hsc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`wang-ge-tu-dfspythonjavaccgojsrust-by-en-2hsc`
- topic id：`3756169`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

做法同 [200. 岛屿数量](https://leetcode.cn/problems/number-of-islands/)，请看 [我的题解](https://leetcode.cn/problems/number-of-islands/solutions/2965773/ba-fang-wen-guo-de-ge-zi-cha-shang-qi-zi-9gs0/)，在 DFS 岛屿的同时统计岛屿格子的个数。

**问**：如何理解这个递归？

**答**：递归的思想是，假设你是一家公司的老板，你不需要万事亲力亲为，而是拆解问题，交给下属去做。我们从岛屿的某个位置 $(i,j)$ 登陆，然后一分为四，把统计岛屿面积的任务交给其他人去处理，自己只需累加其他人统计出来的岛屿面积，再加上 $1$，即 $(i,j)$ 这个格子。

```py [sol-Python3]
class Solution:
    def maxAreaOfIsland(self, grid: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])

        def dfs(i: int, j: int) -> int:
            area = 1  # (i,j) 这个格子
            grid[i][j] = 0  # 标记 (i,j) 访问过
            for x, y in (i, j - 1), (i, j + 1), (i - 1, j), (i + 1, j):  # 左右上下
                if 0 <= x < m and 0 <= y < n and grid[x][y]:
                    # 把统计岛屿面积的任务交给其他人去处理，自己只需累加其他人统计出来的岛屿面积
                    area += dfs(x, y)
            return area

        ans = 0
        for i, row in enumerate(grid):
            for j, x in enumerate(row):
                if x:  # 是陆地，且之前没有访问过
                    ans = max(ans, dfs(i, j))
        return ans
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 左右上下

    public int maxAreaOfIsland(int[][] grid) {
        int m = grid.length;
        int n = grid[0].length;
        int ans = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] > 0) { // 是陆地，且之前没有访问过
                    ans = Math.max(ans, dfs(i, j, grid));
                }
            }
        }
        return ans;
    }

    private int dfs(int i, int j, int[][] grid) {
        int area = 1; // (i,j) 这个格子
        grid[i][j] = 0; // 标记 (i,j) 访问过
        for (int[] d : DIRS) {
            int x = i + d[0];
            int y = j + d[1];
            if (0 <= x && x < grid.length && 0 <= y && y < grid[x].length && grid[x][y] > 0) {
                // 把统计岛屿面积的任务交给其他人去处理，自己只需累加其他人统计出来的岛屿面积
                area += dfs(x, y, grid);
            }
        }
        return area;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int DIRS[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 左右上下

public:
    int maxAreaOfIsland(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();

        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            int area = 1; // (i,j) 这个格子
            grid[i][j] = 0; // 标记 (i,j) 访问过
            for (auto [dx, dy] : DIRS) {
                int x = i + dx, y = j + dy;
                if (0 <= x && x < m && 0 <= y && y < n && grid[x][y]) {
                    // 把统计岛屿面积的任务交给其他人去处理，自己只需累加其他人统计出来的岛屿面积
                    area += dfs(x, y);
                }
            }
            return area;
        };

        int ans = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j]) { // 是陆地，且之前没有访问过
                    ans = max(ans, dfs(i, j));
                }
            }
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
const int DIRS[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 左右上下

int dfs(int** grid, int m, int n, int i, int j) {
    int area = 1; // (i,j) 这个格子
    grid[i][j] = 0; // 标记 (i,j) 访问过
    for (int k = 0; k < 4; k++) {
        int x = i + DIRS[k][0];
        int y = j + DIRS[k][1];
        if (0 <= x && x < m && 0 <= y && y < n && grid[x][y]) {
            // 把统计岛屿面积的任务交给其他人去处理，自己只需累加其他人统计出来的岛屿面积
            area += dfs(grid, m, n, x, y);
        }
    }
    return area;
}

int maxAreaOfIsland(int** grid, int gridSize, int* gridColSize) {
    int m = gridSize, n = gridColSize[0];
    int ans = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j]) { // 是陆地，且之前没有访问过
                int area = dfs(grid, m, n, i, j);
                ans = MAX(ans, area);
            }
        }
    }
    return ans;
}
```

```go [sol-Go]
var dirs = []struct{ x, y int }{{0, -1}, {0, 1}, {-1, 0}, {1, 0}} // 左右上下

func maxAreaOfIsland(grid [][]int) (ans int) {
    m, n := len(grid), len(grid[0])

    var dfs func(int, int) int
    dfs = func(i, j int) int {
        area := 1 // (i,j) 这个格子
        grid[i][j] = 0 // 标记 (i,j) 访问过
        for _, d := range dirs {
            x, y := i+d.x, j+d.y
            if 0 <= x && x < m && 0 <= y && y < n && grid[x][y] > 0 {
                // 把统计岛屿面积的任务交给其他人去处理，自己只需累加其他人统计出来的岛屿面积
                area += dfs(x, y)
            }
        }
        return area
    }

    for i, row := range grid {
        for j, x := range row {
            if x > 0 {
                ans = max(ans, dfs(i, j))
            }
        }
    }
    return
}
```

```js [sol-JavaScript]
var maxAreaOfIsland = function(grid) {
    const m = grid.length, n = grid[0].length;

    function dfs(i, j) {
        let area = 1; // (i,j) 这个格子
        grid[i][j] = 0; // 标记 (i,j) 访问过
        for (const [x, y] of [[i, j - 1], [i, j + 1], [i - 1, j], [i + 1, j]]) { // 左右上下
            if (0 <= x && x < m && 0 <= y && y < n && grid[x][y]) {
                // 把统计岛屿面积的任务交给其他人去处理，自己只需累加其他人统计出来的岛屿面积
                area += dfs(x, y);
            }
        }
        return area;
    }

    let ans = 0;
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (grid[i][j]) { // 是陆地，且之前没有访问过
                ans = Math.max(ans, dfs(i, j));
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_area_of_island(mut grid: Vec<Vec<i32>>) -> i32 {
        fn dfs(grid: &mut [Vec<i32>], i: usize, j: usize) -> i32 {
            if i >= grid.len() || j >= grid[i].len() || grid[i][j] == 0 {
                return 0;
            }
            grid[i][j] = 0; // 标记 (i,j) 访问过

            let mut area = 1; // (i,j) 这个格子
            // 把统计岛屿面积的任务交给其他人去处理，自己只需累加其他人统计出来的岛屿面积
            area += dfs(grid, i, j - 1); // 左
            area += dfs(grid, i, j + 1); // 右
            area += dfs(grid, i - 1, j); // 上
            area += dfs(grid, i + 1, j); // 下
            area
        }

        let m = grid.len();
        let n = grid[0].len();
        let mut ans = 0;
        for i in 0..m {
            for j in 0..n {
                ans = ans.max(dfs(&mut grid, i, j));
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(mn)$。最坏情况下，对于之字形（蛇形）岛屿，递归深度为 $\mathcal{O}(mn)$，需要 $\mathcal{O}(mn)$ 的栈空间。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
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
