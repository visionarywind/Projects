# 417. 太平洋大西洋水流问题

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/pacific-atlantic-water-flow/
- 题目 slug：`pacific-atlantic-water-flow`
- 来源专题：网格图
- 来源分类路径：一、网格图 DFS
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/pacific-atlantic-water-flow/solutions/3798401/ni-xiang-si-wei-pythonjavaccgojsrust-by-zbz19/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[逆向思维（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/pacific-atlantic-water-flow/solutions/3798401/ni-xiang-si-wei-pythonjavaccgojsrust-by-zbz19/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ni-xiang-si-wei-pythonjavaccgojsrust-by-zbz19`
- topic id：`3798401`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**核心想法**：本题起点（答案）不明确，但终点（边界）明确，所以从边界出发能方便地找到答案。

![lc417.jpg](https://pic.leetcode.cn/1759617243-vYcvRK-lc417.jpg){:width=400px}

什么是边界？$\textit{heights}$ 中的 $i=0$ 或者 $i=m-1$ 或者 $j=0$ 或者 $j=n-1$ 的格子。

什么是答案？既可流向太平洋也可流向大西洋的格子。

1. 对于可流向太平洋的格子，我们从上边界（$i=0$）和左边界（$j=0$）倒着**往高处走**，所有能访问到的格子，就是可以流向太平洋的格子。
2. 对于可流向大西洋的格子，我们从下边界（$i=m-1$）和右边界（$j=n-1$）倒着**往高处走**，所有能访问到的格子，就是可以流向大西洋的格子。
3. 计算这两类格子的**交集**，即为既可流向太平洋也可流向大西洋的格子。

关于网格图 DFS 的原理，见 [200. 岛屿数量](https://leetcode.cn/problems/number-of-islands/)，[我的题解](https://leetcode.cn/problems/number-of-islands/solutions/2965773/ba-fang-wen-guo-de-ge-zi-cha-shang-qi-zi-9gs0/)。

```py [sol-Python3]
class Solution:
    def pacificAtlantic(self, heights: List[List[int]]) -> List[List[int]]:
        m, n = len(heights), len(heights[0])

        def search(cells: List[Tuple[int, int]]) -> Set[Tuple[int, int]]:
            def dfs(i: int, j: int) -> None:
                if (i, j) in vis:  # 避免重复访问，避免反复横跳无限递归
                    return
                vis.add((i, j))  # 标记 (i,j) 已访问
                for x, y in (i, j - 1), (i, j + 1), (i - 1, j), (i + 1, j):  # 枚举相邻格子
                    if 0 <= x < m and 0 <= y < n and heights[x][y] >= heights[i][j]:  # 往高处走
                        dfs(x, y)

            vis = set()
            for i, j in cells:
                dfs(i, j)
            return vis

        pacific = [(0, j) for j in range(n)] + [(i, 0) for i in range(1, m)]
        atlantic = [(m - 1, j) for j in range(n)] + [(i, n - 1) for i in range(m - 1)]
        return list(search(pacific) & search(atlantic))  # 交集即为答案
```

```java [sol-Java]
class Solution {
    // 左右上下
    private static final int[][] DIRS = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    public List<List<Integer>> pacificAtlantic(int[][] heights) {
        int m = heights.length, n = heights[0].length;

        // 从太平洋边界出发
        boolean[][] pacificVis = new boolean[m][n];
        for (int j = 0; j < n; j++) {
            dfs(0, j, pacificVis, heights); // 上边界
        }
        for (int i = 1; i < m; i++) {
            dfs(i, 0, pacificVis, heights); // 左边界
        }

        // 从大西洋边界出发
        boolean[][] atlanticVis = new boolean[m][n];
        for (int j = 0; j < n; j++) {
            dfs(m - 1, j, atlanticVis, heights); // 下边界
        }
        for (int i = 0; i < m - 1; i++) {
            dfs(i, n - 1, atlanticVis, heights); // 右边界
        }

        // 交集即为答案
        List<List<Integer>> ans = new ArrayList<>();
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (pacificVis[i][j] && atlanticVis[i][j]) {
                    ans.add(List.of(i, j));
                }
            }
        }
        return ans;
    }

    private void dfs(int i, int j, boolean[][] vis, int[][] heights) {
        if (vis[i][j]) { // 避免重复访问，避免反复横跳无限递归
            return;
        }
        vis[i][j] = true; // 标记 (i,j) 已访问
        for (int[] d : DIRS) { // 枚举相邻格子
            int x = i + d[0], y = j + d[1];
            if (0 <= x && x < heights.length && 0 <= y && y < heights[x].length && heights[x][y] >= heights[i][j]) { // 往高处走
                dfs(x, y, vis, heights);
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
    // 左右上下
    static constexpr int DIRS[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

public:
    vector<vector<int>> pacificAtlantic(vector<vector<int>>& heights) {
        int m = heights.size(), n = heights[0].size();

        // lambda 递归
        auto dfs = [&](this auto&& dfs, int i, int j, vector<vector<int8_t>>& vis) -> void {
            if (vis[i][j]) { // 避免重复访问，避免反复横跳无限递归
                return;
            }
            vis[i][j] = true; // 标记 (i,j) 已访问
            for (auto& [dx, dy] : DIRS) { // 枚举相邻格子
                int x = i + dx, y = j + dy;
                if (0 <= x && x < m && 0 <= y && y < n && heights[x][y] >= heights[i][j]) { // 往高处走
                    dfs(x, y, vis);
                }
            }
        };

        // 从太平洋边界出发
        vector pacific_vis(m, vector<int8_t>(n));
        for (int j = 0; j < n; j++) {
            dfs(0, j, pacific_vis); // 上边界
        }
        for (int i = 1; i < m; i++) {
            dfs(i, 0, pacific_vis); // 左边界
        }

        // 从大西洋边界出发
        vector atlantic_vis(m, vector<int8_t>(n));
        for (int j = 0; j < n; j++) {
            dfs(m - 1, j, atlantic_vis); // 下边界
        }
        for (int i = 0; i < m - 1; i++) {
            dfs(i, n - 1, atlantic_vis); // 右边界
        }

        // 交集即为答案
        vector<vector<int>> ans;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (pacific_vis[i][j] && atlantic_vis[i][j]) {
                    ans.push_back({i, j});
                }
            }
        }
        return ans;
    }
};
```

```c [sol-C]
// 左右上下
static const int DIRS[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

void dfs(int i, int j, bool** vis, int** heights, int m, int n) {
    if (vis[i][j]) { // 避免重复访问，避免反复横跳无限递归
        return;
    }
    vis[i][j] = true; // 标记 (i,j) 已访问
    for (int k = 0; k < 4; k++) { // 枚举相邻格子
        int x = i + DIRS[k][0], y = j + DIRS[k][1];
        if (0 <= x && x < m && 0 <= y && y < n && heights[x][y] >= heights[i][j]) { // 往高处走
            dfs(x, y, vis, heights, m, n);
        }
    }
}

int** pacificAtlantic(int** heights, int heightsSize, int* heightsColSize, int* returnSize, int** returnColumnSizes) {
    int m = heightsSize, n = heightsColSize[0];

    // 从太平洋边界出发
    bool** pacific_vis = malloc(m * sizeof(bool*));
    for (int i = 0; i < m; i++) {
        pacific_vis[i] = calloc(n, sizeof(bool));
    }
    for (int j = 0; j < n; j++) {
        dfs(0, j, pacific_vis, heights, m, n); // 上边界
    }
    for (int i = 1; i < m; i++) {
        dfs(i, 0, pacific_vis, heights, m, n); // 左边界
    }

    // 从大西洋边界出发
    bool** atlantic_vis = malloc(m * sizeof(bool*));
    for (int i = 0; i < m; i++) {
        atlantic_vis[i] = calloc(n, sizeof(bool));
    }
    for (int j = 0; j < n; j++) {
        dfs(m - 1, j, atlantic_vis, heights, m, n); // 下边界
    }
    for (int i = 0; i < m - 1; i++) {
        dfs(i, n - 1, atlantic_vis, heights, m, n); // 右边界
    }

    // 交集即为答案
    int** ans = malloc(m * n * sizeof(int*));
    *returnColumnSizes = malloc(m * n * sizeof(int));
    *returnSize = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (pacific_vis[i][j] && atlantic_vis[i][j]) {
                (*returnColumnSizes)[*returnSize] = 2;
                ans[*returnSize] = malloc(2 * sizeof(int));
                ans[*returnSize][0] = i;
                ans[*returnSize][1] = j;
                (*returnSize)++;
            }
        }
    }
    
    free(pacific_vis);
    free(atlantic_vis);
    return ans;
}
```

```go [sol-Go]
var dirs = [][2]int{{0, -1}, {0, 1}, {-1, 0}, {1, 0}} // 左右上下

func pacificAtlantic(heights [][]int) (ans [][]int) {
    m, n := len(heights), len(heights[0])

    var dfs func(int, int, [][]bool)
    dfs = func(i, j int, vis [][]bool) {
        if vis[i][j] { // 避免重复访问，避免反复横跳无限递归
            return
        }
        vis[i][j] = true // 标记 (i,j) 已访问
        for _, d := range dirs { // 枚举相邻格子
            x, y := i+d[0], j+d[1]
            if 0 <= x && x < m && 0 <= y && y < n && heights[x][y] >= heights[i][j] { // 往高处走
                dfs(x, y, vis)
            }
        }
    }

    // 从太平洋边界出发
    pacificVis := make([][]bool, m)
    for i := range pacificVis {
        pacificVis[i] = make([]bool, n)
    }
    for j := range n {
        dfs(0, j, pacificVis) // 上边界
    }
    for i := 1; i < m; i++ {
        dfs(i, 0, pacificVis) // 左边界
    }

    // 从大西洋边界出发
    atlanticVis := make([][]bool, m)
    for i := range atlanticVis {
        atlanticVis[i] = make([]bool, n)
    }
    for j := range n {
        dfs(m-1, j, atlanticVis) // 下边界
    }
    for i := range m - 1 {
        dfs(i, n-1, atlanticVis) // 右边界
    }

    // 交集即为答案
    for i, row := range pacificVis {
        for j, ok := range row {
            if ok && atlanticVis[i][j] {
                ans = append(ans, []int{i, j})
            }
        }
    }
    return
}
```

```js [sol-JavaScript]
const DIRS = [[0, -1], [0, 1], [-1, 0], [1, 0]]; // 左右上下

var pacificAtlantic = function(heights) {
    const m = heights.length, n = heights[0].length;

    function dfs(i, j, vis) {
        if (vis[i][j]) { // 避免重复访问，避免反复横跳无限递归
            return;
        }
        vis[i][j] = true; // 标记 (i,j) 已访问
        for (const [dx, dy] of DIRS) { // 枚举相邻格子
            const x = i + dx, y = j + dy;
            if (x >= 0 && x < m && y >= 0 && y < n && heights[x][y] >= heights[i][j]) { // 往高处走
                dfs(x, y, vis);
            }
        }
    }

    // 从太平洋边界出发
    const pacificVis = Array.from({ length: m }, () => Array(n).fill(false));
    for (let j = 0; j < n; j++) {
        dfs(0, j, pacificVis); // 上边界
    }
    for (let i = 1; i < m; i++) {
        dfs(i, 0, pacificVis); // 左边界
    }

    // 从大西洋边界出发
    const atlanticVis = Array.from({ length: m }, () => Array(n).fill(false));
    for (let j = 0; j < n; j++) {
        dfs(m - 1, j, atlanticVis); // 下边界
    }
    for (let i = 0; i < m - 1; i++) {
        dfs(i, n - 1, atlanticVis); // 右边界
    }

    // 交集即为答案
    const ans = [];
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (pacificVis[i][j] && atlanticVis[i][j]) {
                ans.push([i, j]);
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
const DIRS: [(i8, i8); 4] = [(0, -1), (0, 1), (-1, 0), (1, 0)]; // 左右上下

impl Solution {
    pub fn pacific_atlantic(heights: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        fn dfs(i: usize, j: usize, vis: &mut Vec<Vec<bool>>, heights: &Vec<Vec<i32>>) {
            if vis[i][j] { // 避免重复访问，避免反复横跳无限递归
                return;
            }
            vis[i][j] = true; // 标记 (i,j) 已访问
            for (dx, dy) in DIRS { // 枚举相邻格子
                let x = i + dx as usize;
                let y = j + dy as usize;
                if x < heights.len() && y < heights[i].len() && heights[x][y] >= heights[i][j] { // 往高处走
                    dfs(x, y, vis, heights);
                }
            }
        }

        let m = heights.len();
        let n = heights[0].len();

        // 从太平洋边界出发
        let mut pacific_vis = vec![vec![false; n]; m];
        for j in 0..n {
            dfs(0, j, &mut pacific_vis, &heights); // 上边界
        }
        for i in 1..m {
            dfs(i, 0, &mut pacific_vis, &heights); // 左边界
        }

        // 从大西洋边界出发
        let mut atlantic_vis = vec![vec![false; n]; m];
        for j in 0..n {
            dfs(m - 1, j, &mut atlantic_vis, &heights); // 下边界
        }
        for i in 0..m - 1 {
            dfs(i, n - 1, &mut atlantic_vis, &heights); // 右边界
        }

        // 交集即为答案
        let mut ans = vec![];
        for i in 0..m {
            for j in 0..n {
                if pacific_vis[i][j] && atlantic_vis[i][j] {
                    ans.push(vec![i as i32, j as i32]);
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{heights}$ 的行数和列数。
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
