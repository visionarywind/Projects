# 200. 岛屿数量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-islands/
- 题目 slug：`number-of-islands`
- 来源专题：网格图
- 来源分类路径：一、网格图 DFS
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/number-of-islands/solutions/2965773/ba-fang-wen-guo-de-ge-zi-cha-shang-qi-zi-9gs0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[把访问过的格子插上旗子🚩（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/number-of-islands/solutions/2965773/ba-fang-wen-guo-de-ge-zi-cha-shang-qi-zi-9gs0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ba-fang-wen-guo-de-ge-zi-cha-shang-qi-zi-9gs0`
- topic id：`2965773`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

先说总体思路，然后说 DFS 的细节。

## 总体思路

看示例 2：

```
1 1 0 0 0
1 1 0 0 0
0 0 1 0 0
0 0 0 1 1
```

假设你是哥伦布。先从左上角开始，把第一个岛全部插上旗子🚩，这里用 $2$ 表示。插满旗子后，把答案（岛屿个数）加一。

⚠**注意**：在岛上，你只能左右上下走，不能斜方向走。

```
2 2 0 0 0
2 2 0 0 0
0 0 1 0 0
0 0 0 1 1
```

继续遍历，寻找其他的岛屿，也就是 $1$。找到 $1$ 意味着发现了一个新的岛，继续插上旗子🚩，把答案加一。

```
2 2 0 0 0
2 2 0 0 0
0 0 2 0 0
0 0 0 1 1
```

继续遍历，寻找其他的岛屿。找到 $1$ 意味着发现了一个新的岛，插满旗子🚩，把答案加一。

```
2 2 0 0 0
2 2 0 0 0
0 0 2 0 0
0 0 0 2 2
```

如果没有 $1$ 了，算法就结束了，返回答案（岛屿个数）。

## 如何实现？

一旦我们发现 $(i,j)$ 是 $1$，就从 $(i,j)$ 开始，DFS 这个岛。

每一步可以往左右上下四个方向走，也就是

$$
(i,j-1),(i,j+1),(i-1,j),(i+1,j)
$$

这四个格子。

如果到达一块未发现的陆地格子，就插上旗子🚩，把 $\textit{grid}[i][j]$ 改成 $2$。

如果 $(i,j)$ 出界，或者 $(i,j)$ 是水，或者 $(i,j)$ 已发现（已插上旗子🚩），就不再继续往下递归。

⚠**注意**：DFS 的过程中，最重要的是**不能重复访问之前访问过的格子**。

比如从左上角 $(0,0)$ 向右移动到 $(0,1)$，然后从 $(0,1)$ 又向左移动到 $(0,0)$，再从 $(0,0)$ 向右移动到 $(0,1)$，如此往复，就无限递归下去了。

怎么避免重复访问？本题的做法是把访问过的格子都插上旗子🚩。例如从 $(0,1)$ 往左走，发现 $(0,0)$ 是插过旗子的格子，就不继续走了。

## 答疑

**问**：二叉树的递归和网格图的递归有何区别？

**答**：列表总结如下。

|   | **二叉树**  |  **网格图** |
|---|---|---|
| **递归入口**  |  根节点  |  岛屿第一行最左边的陆地  |
| **递归方向**  |  左儿子和右儿子 |  左右上下的相邻陆地  |
| **递归边界** |  空节点（或者叶节点） | 出界、遇到水或者旗子  |

**问**：如何理解递归？

**答**：递归的思想是，假设你是一家公司的老板，你不需要万事亲力亲为，而是拆解问题，交给下属去做。对于这题来说，就是从岛屿的某个位置登陆插旗，然后一分为四，把探索整个岛屿的任务交给其他人去处理，自己只需处理好第一步就行。

**问**：我可以先判断 $\textit{grid}[i][j]$ 是否等于 $1$，再判断 $(i,j)$ 是否出界吗？

**问**：不行，比如 $i=-1$，获取 $\textit{grid}[i][j]$ 就下标越界了。要先保证下标在范围中，再去获取数组元素值。

**问**：我可以把访问过的 $\textit{grid}[i][j]$ 改成 $0$ 吗？

**答**：可以。相当于把访问过的位置变成水。其实，改成除了 $1$ 以外的任何值都行。

**问**：我可以把这行代码 `grid[i][j] = '2'` 写在 $\textit{dfs}$ 的最后一行吗？

**答**：不行。这样写会先执行递归，比如从左上角 $(0,0)$ 向右移动到 $(0,1)$，内部继续递归，从 $(0,1)$ 又向左移动到 $(0,0)$，导致反复横跳，无限递归。

**问**：DFS 中能否只考虑往右走和往下走？

**答**：不行，比如蚊香形的岛屿，在探索的时候必须具备左右上下走的能力。

```py [sol-Python3]
class Solution:
    def numIslands(self, grid: List[List[str]]) -> int:
        m, n = len(grid), len(grid[0])

        def dfs(i: int, j: int) -> None:
            # 出界，或者不是 '1'，就不再往下递归
            if i < 0 or i >= m or j < 0 or j >= n or grid[i][j] != '1':
                return
            grid[i][j] = '2'  # 插旗！避免来回横跳无限递归
            dfs(i, j - 1)  # 往左走
            dfs(i, j + 1)  # 往右走
            dfs(i - 1, j)  # 往上走
            dfs(i + 1, j)  # 往下走

        ans = 0
        for i, row in enumerate(grid):
            for j, c in enumerate(row):
                if c == '1':  # 找到了一个新的岛
                    dfs(i, j)  # 把这个岛插满旗子，这样后面遍历到的 '1' 一定是新的岛
                    ans += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int numIslands(char[][] grid) {
        int ans = 0;
        for (int i = 0; i < grid.length; i++) {
            for (int j = 0; j < grid[i].length; j++) {
                if (grid[i][j] == '1') { // 找到了一个新的岛
                    dfs(grid, i, j); // 把这个岛插满旗子，这样后面遍历到的 '1' 一定是新的岛
                    ans++;
                }
            }
        }
        return ans;
    }

    private void dfs(char[][] grid, int i, int j) {
        // 出界，或者不是 '1'，就不再往下递归
        if (i < 0 || i >= grid.length || j < 0 || j >= grid[0].length || grid[i][j] != '1') {
            return;
        }
        grid[i][j] = '2'; // 插旗！避免来回横跳无限递归
        dfs(grid, i, j - 1); // 往左走
        dfs(grid, i, j + 1); // 往右走
        dfs(grid, i - 1, j); // 往上走
        dfs(grid, i + 1, j); // 往下走
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numIslands(vector<vector<char>>& grid) {
        int m = grid.size(), n = grid[0].size();
        auto dfs = [&](this auto&& dfs, int i, int j) -> void {
            // 出界，或者不是 '1'，就不再往下递归
            if (i < 0 || i >= m || j < 0 || j >= n || grid[i][j] != '1') {
                return;
            }
            grid[i][j] = '2'; // 插旗！避免来回横跳无限递归
            dfs(i, j - 1); // 往左走
            dfs(i, j + 1); // 往右走
            dfs(i - 1, j); // 往上走
            dfs(i + 1, j); // 往下走
        };

        int ans = 0;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (grid[i][j] == '1') { // 找到了一个新的岛
                    dfs(i, j); // 把这个岛插满旗子，这样后面遍历到的 '1' 一定是新的岛
                    ans++;
                }
            }
        }
        return ans;
    }
};
```

```c [sol-C]
void dfs(char** grid, int m, int n, int i, int j) {
    // 出界，或者不是 '1'，就不再往下递归
    if (i < 0 || i >= m || j < 0 || j >= n || grid[i][j] != '1') {
        return;
    }
    grid[i][j] = '2'; // 插旗！避免来回横跳无限递归
    dfs(grid, m, n, i, j - 1); // 往左走
    dfs(grid, m, n, i, j + 1); // 往右走
    dfs(grid, m, n, i - 1, j); // 往上走
    dfs(grid, m, n, i + 1, j); // 往下走
}

int numIslands(char** grid, int gridSize, int* gridColSize) {
    int m = gridSize, n = gridColSize[0];
    int ans = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (grid[i][j] == '1') { // 找到了一个新的岛
                dfs(grid, m, n, i, j); // 把这个岛插满旗子，这样后面遍历到的 '1' 一定是新的岛
                ans++;
            }
        }
    }
    return ans;
}
```

```go [sol-Go]
func numIslands(grid [][]byte) (ans int) {
    m, n := len(grid), len(grid[0])
    var dfs func(int, int)
    dfs = func(i, j int) {
        // 出界，或者不是 '1'，就不再往下递归
        if i < 0 || i >= m || j < 0 || j >= n || grid[i][j] != '1' {
            return
        }
        grid[i][j] = '2' // 插旗！避免来回横跳无限递归
        dfs(i, j-1)      // 往左走
        dfs(i, j+1)      // 往右走
        dfs(i-1, j)      // 往上走
        dfs(i+1, j)      // 往下走
    }

    for i, row := range grid {
        for j, c := range row {
            if c == '1' { // 找到了一个新的岛
                dfs(i, j) // 把这个岛插满旗子，这样后面遍历到的 '1' 一定是新的岛
                ans++
            }
        }
    }
    return
}
```

```js [sol-JavaScript]
var numIslands = function(grid) {
    const m = grid.length, n = grid[0].length;
    function dfs(i, j) {
        // 出界，或者不是 '1'，就不再往下递归
        if (i < 0 || i >= m || j < 0 || j >= n || grid[i][j] !== '1') {
            return;
        }
        grid[i][j] = '2'; // 插旗！避免来回横跳无限递归
        dfs(i, j - 1); // 往左走
        dfs(i, j + 1); // 往右走
        dfs(i - 1, j); // 往上走
        dfs(i + 1, j); // 往下走
    }

    let ans = 0;
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (grid[i][j] === '1') { // 找到了一个新的岛
                dfs(i, j); // 把这个岛插满旗子，这样后面遍历到的 '1' 一定是新的岛
                ans++;
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn num_islands(mut grid: Vec<Vec<char>>) -> i32 {
        fn dfs(grid: &mut Vec<Vec<char>>, i: usize, j: usize) {
            // 出界，或者不是 '1'，就不再往下递归
            if i >= grid.len() || j >= grid[i].len() || grid[i][j] != '1' {
                return;
            }
            grid[i][j] = '2'; // 插旗！避免来回横跳无限递归
            dfs(grid, i, j - 1); // 往左走
            dfs(grid, i, j + 1); // 往右走
            dfs(grid, i - 1, j); // 往上走
            dfs(grid, i + 1, j); // 往下走
        }

        let mut ans = 0;
        for i in 0..grid.len() {
            for j in 0..grid[i].len() {
                if grid[i][j] == '1' { // 找到了一个新的岛
                    dfs(&mut grid, i, j); // 把这个岛插满旗子，这样后面遍历到的 '1' 一定是新的岛
                    ans += 1;
                }
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{grid}$ 的行数和列数。由于 DFS 中修改了 `grid[i][j] = '2'`，当我们访问一个访问过的格子时，会触发 `if grid[i][j] != '1': return`。只有首次访问一个格子时，才会继续递归，其余情况不会继续递归。每次插上一个旗子只需要 $\mathcal{O}(1)$ 的时间，插上至多 $mn$ 个旗子，就需要 $\mathcal{O}(mn)$ 的时间。
- 空间复杂度：$\mathcal{O}(mn)$。最坏情况下，对于蛇形陆地、蚊香型陆地等，递归需要 $\mathcal{O}(mn)$ 的栈空间。

## 思考题

1. 构造一个 $\textit{grid}$，让上面代码的递归深度达到最大。
2. 如果调整四个方向的递归顺序（比如顺序为右下左上），递归路径的形状会发生什么变化？

欢迎在评论区分享你的思路。

## 专题训练

见下面的网格图题单。

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
