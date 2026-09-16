# 827. 最大人工岛

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/making-a-large-island/
- 题目 slug：`making-a-large-island`
- 来源专题：网格图
- 来源分类路径：一、网格图 DFS
- 难度分：1934
- 外部题解来源：https://leetcode.cn/problems/making-a-large-island/solutions/2808887/jian-ji-gao-xiao-ji-suan-dao-yu-de-mian-ab4h7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[计算岛屿的面积+每个格子属于哪个岛，附变形题（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/making-a-large-island/solutions/2808887/jian-ji-gao-xiao-ji-suan-dao-yu-de-mian-ab4h7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-ji-gao-xiao-ji-suan-dao-yu-de-mian-ab4h7`
- topic id：`2808887`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 核心思路

1. 遍历每个水域格子（$0$），把水域填成陆地后，上下左右的岛（如果有）会连在一起。可以直接累加上下左右的岛屿面积吗？
2. 不行。比如在示例 2 中，$0$ 的左边和上边的相邻格子属于同一个岛，**如果直接累加面积，会重复统计**，算出比实际更大的答案。
3. 解决办法：首先 DFS 各个岛屿，计算岛屿面积，并标记每个陆地格子（$1$）属于哪个岛（记录岛屿编号）。然后再去遍历每个水域格子，把上下左右的岛（如果有）的编号**去重**后，累加去重后的这些岛的面积。

## 算法

1. DFS 计算每个岛的面积，加到一个列表 $\textit{area}$ 中。做法同 [695. 岛屿的最大面积](https://leetcode.cn/problems/max-area-of-island/)，[我的题解](https://leetcode.cn/problems/max-area-of-island/solutions/3756169/wang-ge-tu-dfspythonjavaccgojsrust-by-en-2hsc/)。
2. 在 DFS 的过程中，对于访问到的格子，标记这个格子属于哪个岛。我们可以用此时 $\textit{area}$ 列表的**长度**，当作岛屿的编号。
3. 遍历 $\textit{grid}$ 中的 $0$，用一个哈希集合记录其上下左右相邻格子所属岛屿（如果有）的编号。然后遍历哈希集合，根据编号去 $\textit{area}$ 中获取到对应的面积，累加面积，更新答案的最大值。也可以在哈希集合记录的同时累加面积。

## 细节

为了简化代码逻辑，在记录岛屿编号时，可以把 $\textit{area}$ 的长度加 $2$ 记录到 $\textit{grid}[i][j]$ 中。加 $2$ 是为了和 $\textit{grid}$ 原有的值区分开。

> 如果不想修改 $\textit{grid}$，也可以创建额外空间记录岛屿编号。

```py [sol-Python3]
class Solution:
    def largestIsland(self, grid: List[List[int]]) -> int:
        n = len(grid)
        def dfs(i: int, j: int) -> int:
            size = 1
            grid[i][j] = len(area) + 2  # 记录 (i,j) 属于哪个岛
            for x, y in (i - 1, j), (i + 1, j), (i, j - 1), (i, j + 1):
                if 0 <= x < n and 0 <= y < n and grid[x][y] == 1:
                    size += dfs(x, y)
            return size

        # DFS 每个岛，统计各个岛的面积，记录到 area 列表中
        area = []
        for i, row in enumerate(grid):
            for j, x in enumerate(row):
                if x == 1:
                    area.append(dfs(i, j))

        # 加上这个特判，可以快很多
        if not area:  # 没有岛
            return 1

        ans = 0
        for i, row in enumerate(grid):
            for j, x in enumerate(row):
                if x: continue
                s = set()
                for x, y in (i - 1, j), (i + 1, j), (i, j - 1), (i, j + 1):
                    if 0 <= x < n and 0 <= y < n and grid[x][y]:
                        s.add(grid[x][y])  # 记录上下左右格子所属岛屿编号
                ans = max(ans, sum(area[idx - 2] for idx in s) + 1)  # 累加面积

        # 如果最后 ans 仍然为 0，说明所有格子都是 1，返回 n^2
        return ans if ans else n * n
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    public int largestIsland(int[][] grid) {
        int n = grid.length;
        List<Integer> area = new ArrayList<>();
        // DFS 每个岛，统计各个岛的面积，记录到 area 列表中
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 1) {
                    area.add(dfs(grid, i, j, area.size() + 2));
                }
            }
        }

        // 特判没有岛的情况
        if (area.isEmpty()) {
            return 1;
        }

        int ans = 0;
        Set<Integer> s = new HashSet<>();
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] != 0) {
                    continue;
                }
                s.clear();
                int newArea = 1;
                for (int[] dir : DIRS) {
                    int x = i + dir[0];
                    int y = j + dir[1];
                    if (0 <= x && x < n && 0 <= y && y < n && grid[x][y] != 0 && s.add(grid[x][y])) {
                        newArea += area.get(grid[x][y] - 2); // 累加面积
                    }
                }
                ans = Math.max(ans, newArea);
            }
        }

        // 如果最后 ans 仍然为 0，说明所有格子都是 1，返回 n^2
        return ans == 0 ? n * n : ans;
    }

    private int dfs(int[][] grid, int i, int j, int id) {
        grid[i][j] = id; // 记录 (i,j) 属于哪个岛
        int size = 1;
        for (int[] dir : DIRS) {
            int x = i + dir[0];
            int y = j + dir[1];
            if (0 <= x && x < grid.length && 0 <= y && y < grid.length && grid[x][y] == 1) {
                size += dfs(grid, x, y, id);
            }
        }
        return size;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

public:
    int largestIsland(vector<vector<int>>& grid) {
        int n = grid.size();
        vector<int> area;
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            grid[i][j] = area.size() + 2; // 记录 (i,j) 属于哪个岛
            int size = 1;
            for (auto& [dx, dy] : dirs) {
                int x = i + dx, y = j + dy;
                if (0 <= x && x < n && 0 <= y && y < n && grid[x][y] == 1) {
                    size += dfs(x, y);
                }
            }
            return size;
        };

        // DFS 每个岛，统计各个岛的面积，记录到 area 列表中
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 1) {
                    area.push_back(dfs(i, j));
                }
            }
        }

        // 特判没有岛的情况
        if (area.empty()) {
            return 1;
        }

        int ans = 0;
        unordered_set<int> s;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j]) continue;
                s.clear();
                int new_area = 1;
                for (auto& [dx, dy] : dirs) {
                    int x = i + dx, y = j + dy;
                    if (0 <= x && x < n && 0 <= y && y < n && grid[x][y] && s.insert(grid[x][y]).second) {
                        new_area += area[grid[x][y] - 2]; // 累加面积
                    }
                }
                ans = max(ans, new_area);
            }
        }

        // 如果最后 ans 仍然为 0，说明所有格子都是 1，返回 n^2
        return ans ? ans : n * n;
    }
};
```

```go [sol-Go]
func largestIsland(grid [][]int) int {
    dirs := [][2]int{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}
    n := len(grid)
    area := []int{}

    var dfs func(int, int) int
    dfs = func(i, j int) int {
        grid[i][j] = len(area) + 2 // 记录 (i,j) 属于哪个岛
        size := 1
        for _, dir := range dirs {
            x, y := i+dir[0], j+dir[1]
            if 0 <= x && x < n && 0 <= y && y < n && grid[x][y] == 1 {
                size += dfs(x, y)
            }
        }
        return size
    }

    // DFS 每个岛，统计各个岛的面积，记录到 area 列表中
    for i, row := range grid {
        for j, x := range row {
            if x == 1 {
                area = append(area, dfs(i, j))
            }
        }
    }

    // 特判没有岛的情况
    if len(area) == 0 {
        return 1
    }

    ans := 0
    s := map[int]bool{}
    for i, row := range grid {
        for j, x := range row {
            if x != 0 {
                continue
            }
            clear(s)
            newArea := 1
            for _, dir := range dirs {
                x, y := i+dir[0], j+dir[1]
                if 0 <= x && x < n && 0 <= y && y < n && grid[x][y] != 0 && !s[grid[x][y]] {
                    s[grid[x][y]] = true
                    newArea += area[grid[x][y]-2] // 累加面积
                }
            }
            ans = max(ans, newArea)
        }
    }

    // 如果最后 ans 仍然为 0，说明所有格子都是 1，返回 n^2
    if ans == 0 {
        return n * n
    }
    return ans
}
```

```js [sol-JavaScript]
var largestIsland = function(grid) {
    const dirs = [[-1, 0], [1, 0], [0, -1], [0, 1]];
    const n = grid.length;
    const area = [];

    function dfs(i, j) {
        grid[i][j] = area.length + 2; // 记录 (i,j) 属于哪个岛
        let size = 1;
        for (const [dx, dy] of dirs) {
            const x = i + dx, y = j + dy;
            if (0 <= x && x < n && 0 <= y && y < n && grid[x][y] === 1) {
                size += dfs(x, y);
            }
        }
        return size;
    }

    // DFS 每个岛，统计各个岛的面积，记录到 area 列表中
    for (let i = 0; i < n; i++) {
        for (let j = 0; j < n; j++) {
            if (grid[i][j] === 1) {
                area.push(dfs(i, j));
            }
        }
    }

    // 特判没有岛的情况
    if (area.length === 0) {
        return 1;
    }

    let ans = 0;
    const s = new Set();
    for (let i = 0; i < n; i++) {
        for (let j = 0; j < n; j++) {
            if (grid[i][j]) continue;
            s.clear();
            let newArea = 1;
            for (const [dx, dy] of dirs) {
                const x = i + dx, y = j + dy;
                if (0 <= x && x < n && 0 <= y && y < n && grid[x][y] !== 0 && !s.has(grid[x][y])) {
                    s.add(grid[x][y]);
                    newArea += area[grid[x][y] - 2]; // 累加面积
                }
            }
            ans = Math.max(ans, newArea);
        }
    }

    // 如果最后 ans 仍然为 0，说明所有格子都是 1，返回 n^2
    return ans || n * n;
};
```

```rust [sol-Rust]
use std::collections::HashSet;

impl Solution {
    pub fn largest_island(mut grid: Vec<Vec<i32>>) -> i32 {
        fn dfs(grid: &mut Vec<Vec<i32>>, i: usize, j: usize, id: i32) -> i32 {
            grid[i][j] = id; // 记录 (i,j) 属于哪个岛
            let mut size = 1;
            for (x, y) in [(i.saturating_sub(1), j), (i + 1, j), (i, j.saturating_sub(1)), (i, j + 1)] {
                if 0 <= x && x < grid.len() && 0 <= y && y < grid[0].len() && grid[x][y] == 1 {
                    size += dfs(grid, x, y, id);
                }
            }
            size
        }

        let n = grid.len();
        let mut area = vec![];
        // DFS 每个岛，统计各个岛的面积，记录到 area 列表中
        for i in 0..n {
            for j in 0..n {
                if grid[i][j] == 1 {
                    area.push(dfs(&mut grid, i, j, area.len() as i32 + 2));
                }
            }
        }

        // 特判没有岛的情况
        if area.is_empty() {
            return 1;
        }

        let mut ans = 0;
        let mut s = HashSet::new();
        for (i, row) in grid.iter().enumerate() {
            for (j, &x) in row.iter().enumerate() {
                if x != 0 {
                    continue;
                }
                s.clear();
                let mut new_area = 1;
                for (x, y) in [(i.saturating_sub(1), j), (i + 1, j), (i, j.saturating_sub(1)), (i, j + 1)] {
                    if 0 <= x && x < n && 0 <= y && y < n && grid[x][y] != 0 && s.insert(grid[x][y]) {
                        new_area += area[(grid[x][y] - 2) as usize]; // 累加面积
                    }
                }
                ans = ans.max(new_area);
            }
        }

        // 如果最后 ans 仍然为 0，说明所有格子都是 1，返回 n^2
        if ans == 0 { (n * n) as _ } else { ans }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{grid}$ 的长度。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 变形题

1. 改成把第 $i$ 排 **或** 第 $j$ 列的 $0$ 都变成 $1$，要怎么做？这题是 [CF1985H1](https://codeforces.com/problemset/problem/1985/H1)。
2. 改成把第 $i$ 排 **和** 第 $j$ 列的 $0$ 都变成 $1$，要怎么做？这题是 [CF1985H2](https://codeforces.com/problemset/problem/1985/H2)。
3. 改成把某个 $k\times k$ 的区域都变成 $1$，要怎么做？本题相当于 $k=1$。这题是 [CF679C](https://codeforces.com/problemset/problem/679/C)。

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
