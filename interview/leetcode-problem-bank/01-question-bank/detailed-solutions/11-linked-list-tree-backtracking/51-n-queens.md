# 51. N 皇后

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/n-queens/
- 题目 slug：`n-queens`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.5 排列型回溯
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/n-queens/solutions/2079586/hui-su-tao-lu-miao-sha-nhuang-hou-shi-pi-mljv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频讲解】排列型回溯，简洁高效！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/n-queens/solutions/2079586/hui-su-tao-lu-miao-sha-nhuang-hou-shi-pi-mljv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hui-su-tao-lu-miao-sha-nhuang-hou-shi-pi-mljv`
- topic id：`2079586`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 视频讲解

请看[【基础算法精讲 16】](https://www.bilibili.com/video/BV1mY411D7f6/)，制作不易，欢迎点赞关注~

## 答疑

**问**：本题和 [46. 全排列](https://leetcode.cn/problems/permutations/) 的关系是什么？

**答**：由于每行恰好放一个皇后，记录每行的皇后放在哪一列，可以得到一个 $[0,n-1]$ 的排列 $\textit{queens}$。示例 1 的两个图，分别对应排列 $[1, 3, 0, 2]$ 和 $[2, 0, 3, 1]$。所以我们本质上是在**枚举列号的全排列**。

**问**：如何 $\mathcal{O}(1)$ 判断两个皇后互相攻击？

**答**：由于我们保证了每行每列恰好放一个皇后，所以只需检查斜方向。对于 ↗ 方向的格子，行号加列号是不变的。对于 ↖ 方向的格子，行号减列号是不变的。如果两个皇后，行号加列号相同，或者行号减列号相同，那么这两个皇后互相攻击。

**问**：如何 $\mathcal{O}(1)$ 判断当前位置被之前放置的某个皇后攻击到？

**答**：额外用两个数组 $\textit{diag}_1$ 和 $\textit{diag}_2$ 分别标记之前放置的皇后的行号加列号，以及行号减列号。如果当前位置的行号加列号在 $\textit{diag}_1$ 中（标记为 $\texttt{true}$），或者当前位置的行号减列号在 $\textit{diag}_2$ 中（标记为 $\texttt{true}$），那么当前位置被之前放置的皇后攻击到，不能放皇后。

```py [sol-Python3]
class Solution:
    def solveNQueens(self, n: int) -> List[List[str]]:
        ans = []
        queens = [0] * n  # 皇后放在 (r,queens[r])
        col = [False] * n
        diag1 = [False] * (n * 2 - 1)
        diag2 = [False] * (n * 2 - 1)
        def dfs(r: int) -> None:
            if r == n:
                ans.append(['.' * c + 'Q' + '.' * (n - 1 - c) for c in queens])
                return
            # 在 (r,c) 放皇后
            for c, ok in enumerate(col):
                if not ok and not diag1[r + c] and not diag2[r - c]:  # 判断能否放皇后
                    queens[r] = c  # 直接覆盖，无需恢复现场
                    col[c] = diag1[r + c] = diag2[r - c] = True  # 皇后占用了 c 列和两条斜线
                    dfs(r + 1)
                    col[c] = diag1[r + c] = diag2[r - c] = False  # 恢复现场
        dfs(0)
        return ans
```

```java [sol-Java]
class Solution {
    public List<List<String>> solveNQueens(int n) {
        List<List<String>> ans = new ArrayList<>();
        int[] queens = new int[n]; // 皇后放在 (r,queens[r])
        boolean[] col = new boolean[n];
        boolean[] diag1 = new boolean[n * 2 - 1];
        boolean[] diag2 = new boolean[n * 2 - 1];
        dfs(0, queens, col, diag1, diag2, ans);
        return ans;
    }

    private void dfs(int r, int[] queens, boolean[] col, boolean[] diag1, boolean[] diag2, List<List<String>> ans) {
        int n = col.length;
        if (r == n) {
            List<String> board = new ArrayList<>(n); // 预分配空间
            for (int c : queens) {
                char[] row = new char[n];
                Arrays.fill(row, '.');
                row[c] = 'Q';
                board.add(new String(row));
            }
            ans.add(board);
            return;
        }
        // 在 (r,c) 放皇后
        for (int c = 0; c < n; c++) {
            int rc = r - c + n - 1;
            if (!col[c] && !diag1[r + c] && !diag2[rc]) { // 判断能否放皇后
                queens[r] = c; // 直接覆盖，无需恢复现场
                col[c] = diag1[r + c] = diag2[rc] = true; // 皇后占用了 c 列和两条斜线
                dfs(r + 1, queens, col, diag1, diag2, ans);
                col[c] = diag1[r + c] = diag2[rc] = false; // 恢复现场
            }
        }
    }
}
```

```cpp [sol-C++ 写法一]
class Solution {
public:
    vector<vector<string>> solveNQueens(int n) {
        vector<vector<string>> ans;
        vector board(n, string(n, '.')); // 一开始棋盘是空的，没有皇后
        vector<uint8_t> col(n), diag1(n * 2 - 1), diag2(n * 2 - 1); // vector<uint8_t> 效率比 vector<bool> 高
        auto dfs = [&](this auto&& dfs, int r) {
            if (r == n) {
                ans.push_back(board); // 复制整个棋盘
                return;
            }
            // 在 (r,c) 放皇后
            for (int c = 0; c < n; c++) {
                int rc = r - c + n - 1;
                if (!col[c] && !diag1[r + c] && !diag2[rc]) { // 判断能否放皇后
                    board[r][c] = 'Q'; // 放皇后
                    col[c] = diag1[r + c] = diag2[rc] = true; // 皇后占用了 c 列和两条斜线
                    dfs(r + 1);
                    col[c] = diag1[r + c] = diag2[rc] = false; // 恢复现场
                    board[r][c] = '.';
                }
            }
        };
        dfs(0);
        return ans;
    }
};
```

```cpp [sol-C++ 写法二]
class Solution {
public:
    vector<vector<string>> solveNQueens(int n) {
        vector<vector<string>> ans;
        vector<int> queens(n); // 皇后放在 (r,queens[r])
        vector<uint8_t> col(n), diag1(n * 2 - 1), diag2(n * 2 - 1); // vector<uint8_t> 效率比 vector<bool> 高
        auto dfs = [&](this auto&& dfs, int r) {
            if (r == n) {
                vector<string> board(n);
                for (int i = 0; i < n; i++) {
                    board[i] = string(queens[i], '.') + 'Q' + string(n - 1 - queens[i], '.');
                }
                ans.push_back(board);
                return;
            }
            // 在 (r,c) 放皇后
            for (int c = 0; c < n; c++) {
                int rc = r - c + n - 1;
                if (!col[c] && !diag1[r + c] && !diag2[rc]) { // 判断能否放皇后
                    queens[r] = c; // 直接覆盖，无需恢复现场
                    col[c] = diag1[r + c] = diag2[rc] = true; // 皇后占用了 c 列和两条斜线
                    dfs(r + 1);
                    col[c] = diag1[r + c] = diag2[rc] = false; // 恢复现场
                }
            }
        };
        dfs(0);
        return ans;
    }
};
```

```c [sol-C]
void dfs(int r, int n, int* queens, bool* col, bool* diag1, bool* diag2, char*** ans, int* returnSize, int* returnColumnSizes) {
    if (r == n) {
        char** board = malloc(n * sizeof(char*));
        for (int i = 0; i < n; i++) {
            board[i] = malloc((n + 1) * sizeof(char));
            memset(board[i], '.', n);
            board[i][queens[i]] = 'Q';
            board[i][n] = '\0';
        }
        ans[*returnSize] = board;
        returnColumnSizes[(*returnSize)++] = n;
        return;
    }
    // 在 (r,c) 放皇后
    for (int c = 0; c < n; c++) {
        int rc = r - c + n - 1;
        if (!col[c] && !diag1[r + c] && !diag2[rc]) { // 判断能否放皇后
            queens[r] = c; // 直接覆盖，无需恢复现场
            col[c] = diag1[r + c] = diag2[rc] = true; // 皇后占用了 c 列和两条斜线
            dfs(r + 1, n, queens, col, diag1, diag2, ans, returnSize, returnColumnSizes);
            col[c] = diag1[r + c] = diag2[rc] = false; // 恢复现场
        }
    }
}

char*** solveNQueens(int n, int* returnSize, int** returnColumnSizes) {
    *returnSize = 0;
    // 本题数据范围下至多 352 个解
    char*** ans = malloc(352 * sizeof(char**));
    *returnColumnSizes = malloc(352 * sizeof(int));

    int* queens = malloc(n * sizeof(int)); // 皇后放在 (r,queens[r])
    bool* col = calloc(n, sizeof(bool));
    bool* diag1 = calloc(2 * n - 1, sizeof(bool));
    bool* diag2 = calloc(2 * n - 1, sizeof(bool));
    dfs(0, n, queens, col, diag1, diag2, ans, returnSize, *returnColumnSizes);

    free(queens);
    free(col);
    free(diag1);
    free(diag2);
    return ans;
}
```

```go [sol-Go]
func solveNQueens(n int) (ans [][]string) {
    queens := make([]int, n) // 皇后放在 (r,queens[r])
    col := make([]bool, n)
    diag1 := make([]bool, n*2-1)
    diag2 := make([]bool, n*2-1)
    var dfs func(int)
    dfs = func(r int) {
        if r == n {
            board := make([]string, n)
            for i, c := range queens {
                board[i] = strings.Repeat(".", c) + "Q" + strings.Repeat(".", n-1-c)
            }
            ans = append(ans, board)
            return
        }
        // 在 (r,c) 放皇后
        for c, ok := range col {
            rc := r - c + n - 1
            if !ok && !diag1[r+c] && !diag2[rc] { // 判断能否放皇后
                queens[r] = c // 直接覆盖，无需恢复现场
                col[c], diag1[r+c], diag2[rc] = true, true, true // 皇后占用了 c 列和两条斜线
                dfs(r + 1)
                col[c], diag1[r+c], diag2[rc] = false, false, false // 恢复现场
            }
        }
    }
    dfs(0)
    return
}
```

```js [sol-JavaScript]
var solveNQueens = function(n) {
    const ans = [];
    const queens = Array(n).fill(0); // 皇后放在 (r,queens[r])
    const col = Array(n).fill(false);
    const diag1 = Array(n * 2 - 1).fill(false);
    const diag2 = Array(n * 2 - 1).fill(false);
    function dfs(r) {
        if (r === n) {
            ans.push(queens.map(c => '.'.repeat(c) + 'Q' + '.'.repeat(n - 1 - c)));
            return;
        }
        // 在 (r,c) 放皇后
        for (let c = 0; c < n; c++) {
            const rc = r - c + n - 1;
            if (!col[c] && !diag1[r + c] && !diag2[rc]) { // 判断能否放皇后
                queens[r] = c; // 直接覆盖，无需恢复现场
                col[c] = diag1[r + c] = diag2[rc] = true; // 皇后占用了 c 列和两条斜线
                dfs(r + 1);
                col[c] = diag1[r + c] = diag2[rc] = false; // 恢复现场
            }
        }
    }
    dfs(0);
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn solve_n_queens(n: i32) -> Vec<Vec<String>> {
        fn dfs(r: usize, queens: &mut [usize], col: &mut [bool], diag1: &mut [bool], diag2: &mut [bool], ans: &mut Vec<Vec<String>>) {
            let n = col.len();
            if r == n {
                let board = queens.iter()
                    .map(|&c| {
                        let mut row = vec!['.'; n];
                        row[c] = 'Q';
                        row.into_iter().collect()
                    })
                    .collect::<Vec<_>>();
                ans.push(board);
                return;
            }
            // 在 (r,c) 放皇后
            for c in 0..n {
                let rc = n + r - c - 1;
                if !col[c] && !diag1[r + c] && !diag2[rc] { // 判断能否放皇后
                    queens[r] = c; // 直接覆盖，无需恢复现场
                    // 皇后占用了 c 列和两条斜线
                    col[c] = true;
                    diag1[r + c] = true;
                    diag2[rc] = true;
                    dfs(r + 1, queens, col, diag1, diag2, ans);
                    col[c] = false;
                    diag1[r + c] = false;
                    diag2[rc] = false; // 恢复现场
                }
            }
        }

        let n = n as usize;
        let mut ans = vec![];
        let mut queens = vec![0; n]; // 皇后放在 (r,queens[r])
        let mut col = vec![false; n];
        let mut diag1 = vec![false; n * 2 - 1];
        let mut diag2 = vec![false; n * 2 - 1];
        dfs(0, &mut queens, &mut col, &mut diag1, &mut diag2, &mut ans);
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2\cdot n!)$。搜索树中至多有 $\mathcal{O}(n!)$ 个叶子，每个叶子生成答案每次需要 $\mathcal{O}(n^2)$ 的时间，所以时间复杂度为 $\mathcal{O}(n^2\cdot n!)$。实际上搜索树中远没有这么多叶子，$n=9$ 时只有 $352$ 种放置方案，远远小于 $9!=362880$。更加准确的方案数可以参考 [OEIS A000170](https://oeis.org/A000170)，为 $\mathcal{O}\left(\dfrac{n!}{2.54^n}\right)$。
- 空间复杂度：$\mathcal{O}(n)$。返回值的空间不计入。

更多相似题目，见下面回溯题单的「**§4.5 排列型回溯**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. 【本题相关】[链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、回溯 / §4.5 排列型回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.5 排列型回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
