# 994. 腐烂的橘子

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/rotting-oranges/
- 题目 slug：`rotting-oranges`
- 来源专题：网格图
- 来源分类路径：二、网格图 BFS
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/rotting-oranges/solutions/2773461/duo-yuan-bfsfu-ti-dan-pythonjavacgojsrus-yfmh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[多源 BFS，附变形题（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/rotting-oranges/solutions/2773461/duo-yuan-bfsfu-ti-dan-pythonjavacgojsrus-yfmh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`duo-yuan-bfsfu-ti-dan-pythonjavacgojsrus-yfmh`
- topic id：`2773461`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![lc994.png](https://pic.leetcode.cn/1715154469-vIjjoj-lc994.png)

看示例 1：

1. 统计所有初始就腐烂的橘子的位置，加到列表 $q$ 中，现在 $q = [(0,0)]$。
2. 初始化答案 $\textit{ans}=0$。模拟橘子腐烂的过程，不断循环，直到没有新鲜橘子，或者 $q$ 为空。
3. 答案加一，在第 $\textit{ans}=1$ 分钟，遍历 $q$ 中橘子的四方向相邻的新鲜橘子，把这些橘子腐烂，$q$ 更新为这些橘子的位置，现在 $q = [(0,1),(1,0)]$。
4. 答案加一，在第 $\textit{ans}=2$ 分钟，遍历 $q$ 中橘子的四方向相邻的新鲜橘子，把这些橘子腐烂，$q$ 更新为这些橘子的位置，现在 $q = [(0,2),(1,1)]$。
5. 答案加一，在第 $\textit{ans}=3$ 分钟，遍历 $q$ 中橘子的四方向相邻的新鲜橘子，把这些橘子腐烂，$q$ 更新为这些橘子的位置，现在 $q = [(2,1)]$。
6. 答案加一，在第 $\textit{ans}=4$ 分钟，遍历 $q$ 中橘子的四方向相邻的新鲜橘子，把这些橘子腐烂，$q$ 更新为这些橘子的位置，现在 $q = [(2,2)]$。
7. 由于没有新鲜橘子，退出循环。

为了判断是否有永远不会腐烂的橘子（如示例 2），我们可以统计初始新鲜橘子的个数 $\textit{fresh}$。在 BFS 中，每有一个新鲜橘子被腐烂，就把 $\textit{fresh}$ 减一，这样最后如果发现 $\textit{fresh}>0$，就意味着有橘子永远不会腐烂，返回 $-1$。

代码实现时，在 BFS 中要将 $\textit{grid}[i][j]=1$ 的橘子修改成 $2$（或者其它不等于 $1$ 的数），这可以保证每个橘子加入 $q$ 中至多一次。如果不修改，我们就无法知道哪些橘子被腐烂过了，比如示例 1 中 $(0,1)$ 去腐烂 $(1,1)$，而 $(1,1)$ 在此之后又重新腐烂 $(0,1)$，如此反复，程序就会陷入死循环。读者可以注释掉下面代码中的 `grid[i][j] = 2` 这行代码试试。

关于 BFS 的原理，请看[【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)。

### 答疑

**问**：如果代码不在 `while` 中判断 $\textit{fresh} > 0$，会发生什么？

**答**：会在腐烂完所有新鲜橘子后，多循环一次。这会导致 $\textit{ans}$ 比实际多 $1$。

```py [sol-Python3]
class Solution:
    def orangesRotting(self, grid: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])
        fresh = 0
        q = []
        for i, row in enumerate(grid):
            for j, x in enumerate(row):
                if x == 1:
                    fresh += 1  # 统计新鲜橘子个数
                elif x == 2:
                    q.append((i, j))  # 一开始就腐烂的橘子

        ans = 0
        while q and fresh:
            ans += 1  # 经过一分钟
            tmp = q
            q = []
            for x, y in tmp:  # 已经腐烂的橘子
                for i, j in (x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1):  # 四方向
                    if 0 <= i < m and 0 <= j < n and grid[i][j] == 1:  # 新鲜橘子
                        fresh -= 1
                        grid[i][j] = 2  # 变成腐烂橘子
                        q.append((i, j))

        return -1 if fresh else ans
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRECTIONS = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // 四方向

    public int orangesRotting(int[][] grid) {
        int m = grid.length;
        int n = grid[0].length;
        int fresh = 0;
        List<int[]> q = new ArrayList<>();
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 1) {
                    fresh++; // 统计新鲜橘子个数
                } else if (grid[i][j] == 2) {
                    q.add(new int[]{i, j}); // 一开始就腐烂的橘子
                }
            }
        }

        int ans = 0;
        while (fresh > 0 && !q.isEmpty()) {
            ans++; // 经过一分钟
            List<int[]> tmp = q;
            q = new ArrayList<>();
            for (int[] pos : tmp) { // 已经腐烂的橘子
                for (int[] d : DIRECTIONS) { // 四方向
                    int i = pos[0] + d[0];
                    int j = pos[1] + d[1];
                    if (0 <= i && i < m && 0 <= j && j < n && grid[i][j] == 1) { // 新鲜橘子
                        fresh--;
                        grid[i][j] = 2; // 变成腐烂橘子
                        q.add(new int[]{i, j});
                    }
                }
            }
        }

        return fresh > 0 ? -1 : ans;
    }
}
```

```cpp [sol-C++]
class Solution {
    int DIRECTIONS[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}}; // 四方向

public:
    int orangesRotting(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        int fresh = 0;
        vector<pair<int, int>> q;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == 1) {
                    fresh++; // 统计新鲜橘子个数
                } else if (grid[i][j] == 2) {
                    q.emplace_back(i, j); // 一开始就腐烂的橘子
                }
            }
        }

        int ans = 0;
        while (fresh && !q.empty()) {
            ans++; // 经过一分钟
            auto tmp = move(q); // move 后 q 为空
            for (auto& [x, y] : tmp) { // 已经腐烂的橘子
                for (auto& d : DIRECTIONS) { // 四方向
                    int i = x + d[0], j = y + d[1];
                    if (0 <= i && i < m && 0 <= j && j < n && grid[i][j] == 1) { // 新鲜橘子
                        fresh--;
                        grid[i][j] = 2; // 变成腐烂橘子
                        q.emplace_back(i, j);
                    }
                }
            }
        }

        return fresh ? -1 : ans;
    }
};
```

```go [sol-Go]
type pair struct{ x, y int }
var directions = []pair{{-1, 0}, {1, 0}, {0, -1}, {0, 1}} // 四方向

func orangesRotting(grid [][]int) int {
    m, n := len(grid), len(grid[0])
    fresh := 0
    q := []pair{}
    for i, row := range grid {
        for j, x := range row {
            if x == 1 {
                fresh++ // 统计新鲜橘子个数
            } else if x == 2 {
                q = append(q, pair{i, j}) // 一开始就腐烂的橘子
            }
        }
    }

    ans := 0
    for fresh > 0 && len(q) > 0 {
        ans++ // 经过一分钟
        tmp := q
        q = []pair{}
        for _, p := range tmp { // 已经腐烂的橘子
            for _, d := range directions { // 四方向
                i, j := p.x+d.x, p.y+d.y
                if 0 <= i && i < m && 0 <= j && j < n && grid[i][j] == 1 { // 新鲜橘子
                    fresh--
                    grid[i][j] = 2 // 变成腐烂橘子
                    q = append(q, pair{i, j})
                }
            }
        }
    }

    if fresh > 0 {
        return -1
    }
    return ans
}
```

```js [sol-JavaScript]
var orangesRotting = function(grid) {
    const m = grid.length, n = grid[0].length;
    let fresh = 0;
    let q = [];
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (grid[i][j] === 1) {
                fresh++; // 统计新鲜橘子个数
            } else if (grid[i][j] === 2) {
                q.push([i, j]); // 一开始就腐烂的橘子
            }
        }
    }

    let ans = 0;
    while (fresh && q.length) {
        ans++; // 经过一分钟
        const tmp = q;
        q = [];
        for (const [x, y] of tmp) { // 已经腐烂的橘子
            for (const [i, j] of [[x - 1, y], [x + 1, y], [x, y - 1], [x, y + 1]]) { // 四方向
                if (0 <= i && i < m && 0 <= j && j < n && grid[i][j] === 1) { // 新鲜橘子
                    fresh--;
                    grid[i][j] = 2; // 变成腐烂橘子
                    q.push([i, j]);
                }
            }
        }
    }

    return fresh ? -1 : ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn oranges_rotting(mut grid: Vec<Vec<i32>>) -> i32 {
        let m = grid.len();
        let n = grid[0].len();
        let mut fresh = 0;
        let mut q = vec![];
        for (i, row) in grid.iter().enumerate() {
            for (j, &x) in row.iter().enumerate() {
                if x == 1 {
                    fresh += 1; // 统计新鲜橘子个数
                } else if x == 2 {
                    q.push((i, j)); // 一开始就腐烂的橘子
                }
            }
        }

        let mut ans = 0;
        while fresh > 0 && !q.is_empty() {
            ans += 1; // 经过一分钟
            let mut nxt = vec![];
            for (x, y) in q { // 已经腐烂的橘子
                for (i, j) in [(x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)] { // 四方向
                    if i < m && j < n && grid[i][j] == 1 { // 新鲜橘子
                        fresh -= 1;
                        grid[i][j] = 2; // 变成腐烂橘子
                        nxt.push((i, j));
                    }
                }
            }
            q = nxt;
        }

        if fresh == 0 { ans } else { -1 }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(mn)$。

## 变形题（2025.7.16 添加）

1. 你可以把至多 $1$ 个 $1$ 改成 $2$，最小化最终新鲜橘子个数。
2. 你可以把至多 $k$ 个 $1$ 改成 $2$，最小化最终新鲜橘子个数。
3. 你可以把至多 $1$ 个 $2$ 改成 $1$，最大化最终新鲜橘子个数。
4. 你可以把至多 $k$ 个 $2$ 改成 $1$，最大化最终新鲜橘子个数。

欢迎在评论区分享你的思路/代码。

更多相似题目，见下面网格图题单中的 **BFS**。

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
