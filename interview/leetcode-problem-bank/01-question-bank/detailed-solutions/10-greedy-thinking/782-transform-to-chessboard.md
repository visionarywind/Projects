# 782. 变为棋盘

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/transform-to-chessboard/
- 题目 slug：`transform-to-chessboard`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.4 逆向思维
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/transform-to-chessboard/solutions/2997293/tu-jie-ni-xiang-si-wei-pythonjavaccgojsr-mixb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】逆向思维（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/transform-to-chessboard/solutions/2997293/tu-jie-ni-xiang-si-wei-pythonjavaccgojsr-mixb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-ni-xiang-si-wei-pythonjavaccgojsr-mixb`
- topic id：`2997293`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

$\textit{board}$ 要能变成棋盘，有哪些必要条件？

## 分析性质

![lc782-c.png](https://pic.leetcode.cn/1732265312-EypwGk-lc782-c.png)

根据上图，$\textit{board}$ 要能变成棋盘，有两个**必要条件**：

1. 只有两种行：$A$ 行是从 $0101\cdots$ 交换得到的，也就是说，$A$ 行中的 $0$ 的个数和 $1$ 的个数相差至多为 $1$；$B$ 行是从 $1010\cdots$ 交换得到的，和 $A$ 行完全相反。
2. 这两种行的个数相差至多为 $1$。

## 不能变成棋盘的情况

**核心思路**：统计 $A$ 行和 $B$ 行的个数。统计的过程中，如果发现有一行既不是 $A$ 行也不是 $B$ 行，直接返回 $-1$。统计结束后，如果发现 $A$ 行和 $B$ 行的个数相差超过 $1$，也返回 $-1$。

这可以用哈希表统计。但是，没有必要。

以 $\textit{board}$ 第一行（或者任意一行）为**参照物**。

统计 $\textit{board}$ 第一行中的 $0$ 和 $1$ 的个数，如果 $0$ 和 $1$ 的个数相差超过 $1$，说明这一行既不是 $A$ 行也不是 $B$ 行，无论如何交换，绝不可能得到棋盘，返回 $-1$。

对于其余行 $\textit{board}[i]$，首先比较 $\textit{board}[i][0]$ 和 $\textit{board}[0][0]$：

- 相同：那么 $\textit{board}[i]$ 必须和 $\textit{board}[0]$ 完全相同，即对于任意 $j$，$\textit{board}[i][j] = \textit{board}[0][j]$ 成立。若不满足，返回 $-1$。
- 不同：那么 $\textit{board}[i]$ 必须和 $\textit{board}[0]$ 完全不同，即对于任意 $j$，$\textit{board}[i][j]\ne \textit{board}[0][j]$ 成立。若不满足，返回 $-1$。

如果没有返回 $-1$，那么说明只有 $A$ 行和 $B$ 行。我们也只需统计 $\textit{board}[i][0]$（第一列）的 $0$ 和 $1$ 的个数，就知道有多少个 $A$ 行和 $B$ 行。如果 $0$ 和 $1$ 的个数相差超过 $1$，那么返回 $-1$。（代码实现时，关于第一列的判断可以放在前面）

如果上述情况都没有返回 $-1$，那么 $\textit{board}$ 一定可以变成棋盘，方法如下。

## 最小交换次数

比如把 $s=001110$ 通过交换元素，变成 $t=010101$。这其中 $s[i]\ne t[i]$ 出现了 $4$ 次。我们需要让 $s$ 中的 $0$ 在偶数下标上，$1$ 在奇数下标上。那么把奇数上的 $0$ 和偶数上的 $1$ 交换，就可以满足要求，所以只需要 $\dfrac{4}{2}=2$ 次交换。一般地，有如下定理。

**定理**：如果有 $\textit{diff}$ 个位置与目标值不同，那么只需交换 $\dfrac{\textit{diff}}{2}$ 次。

**证明**：首先 $\dfrac{\textit{diff}}{2}$ 是交换次数的下界。下面证明可以只用 $\dfrac{\textit{diff}}{2}$ 次交换。

不失一般性，假设要把 $s$ 通过交换，变成 $t=010101\cdots$。注意这意味着当 $n$ 是奇数时，$0$ 的个数更多。

- 如果 $n$ 是偶数，那么恰好有 $\dfrac{n}{2}$ 个 $0$，$\dfrac{n}{2}$ 个 $1$，$\dfrac{n}{2}$ 个偶数下标，$\dfrac{n}{2}$ 个奇数下标。设有 $k$ 个 $0$ 在奇数下标上，那么有 $\dfrac{n}{2}-k$ 个 $1$ 在奇数下标上，所以有 $\dfrac{n}{2}-\left(\dfrac{n}{2}-k\right)=k$ 个 $1$ 在偶数下标上，所以只需 $k$ 次交换。注意这意味着 $\textit{diff}=2k$，所以 $\textit{diff}$ 必定是偶数。
- 如果 $n$ 是奇数，$0$ 的个数比 $1$ 的多 $1$，那么有 $\dfrac{n-1}{2}+1$ 个 $0$ 和 $\dfrac{n-1}{2}$ 个奇数下标，所以必然存在一个 $0$ 在偶数下标上（也就是在正确的位置上），去掉这个 $0$，变成 $n$ 是偶数的情况，结论同上。

根据该定理，得到如下计算方法：

- 如果 $n$ 是偶数，那么可以交换成 $010101\cdots$，也可以交换成 $101010\cdots$，设交换成 $t=010101\cdots$ 的 $s[i]\ne t[i]$ 的个数为 $\textit{diff}$，那么交换成 $t'=101010\cdots$ 的 $s[i]\ne t'[i]$ 的个数为 $n-\textit{diff}$，二者取最小值，最小交换次数为 $\dfrac{\min(\textit{diff},n-\textit{diff})}{2}$。
- 如果 $n$ 是奇数，那么 $t$ 是唯一的，最小交换次数为 $\dfrac{\textit{diff}}{2}$。

答案是第一行的最小交换次数，加上第一列的最小交换次数。

交换完成后，就得到了棋盘。

代码实现时，可以利用异或运算（$\oplus$），累加 $s[i]\oplus t[i]$，就相当于统计 $s[i]\ne t[i]$ 的个数。

如果 $t=010101\cdots$，那么 $t[i] = i\bmod 2$。

如果 $t=101010\cdots$，那么 $t[i] = (i\bmod 2)\oplus 1$。

```py [sol-Python3]
class Solution:
    def movesToChessboard(self, board: List[List[int]]) -> int:
        # 第一行，0 和 1 的个数之差不能超过 1
        first_row = board[0]
        row_cnt = Counter(first_row)
        if abs(row_cnt[0] - row_cnt[1]) > 1:
            return -1

        # 第一列，0 和 1 的个数之差不能超过 1
        first_col = list(next(zip(*board)))
        col_cnt = Counter(first_col)
        if abs(col_cnt[0] - col_cnt[1]) > 1:
            return -1

        # 每一行和第一行比较，要么完全相同，要么完全不同
        for row in board:
            same = row[0] == first_row[0]
            for x, y in zip(row, first_row):
                if (x == y) != same:
                    return -1

        # 计算最小交换次数
        def min_swap(s: List[int], cnt: Counter) -> int:
            n = len(s)
            x0 = 1 if cnt[1] > cnt[0] else 0  # 如果 n 是偶数，x0 是 0
            # 计算规则见题解正文最后一段，这里 t[i] = i % 2 ^ x0
            diff = sum(x ^ i % 2 ^ x0 for i, x in enumerate(s))
            return diff // 2 if n % 2 else min(diff, n - diff) // 2

        return min_swap(first_row, row_cnt) + min_swap(first_col, col_cnt)
```

```java [sol-Java]
class Solution {
    public int movesToChessboard(int[][] board) {
        int n = board.length;
        int[] firstRow = board[0];
        int[] firstCol = new int[n];
        int[] rowCnt = new int[2];
        int[] colCnt = new int[2];
        for (int i = 0; i < n; i++) {
            rowCnt[firstRow[i]]++; // 统计 0 和 1 的个数
            firstCol[i] = board[i][0];
            colCnt[firstCol[i]]++;
        }

        // 第一行，0 和 1 的个数之差不能超过 1
        // 第一列，0 和 1 的个数之差不能超过 1
        if (Math.abs(rowCnt[0] - rowCnt[1]) > 1 || Math.abs(colCnt[0] - colCnt[1]) > 1) {
            return -1;
        }

        // 每一行和第一行比较，要么完全相同，要么完全不同
        for (int[] row : board) {
            boolean same = row[0] == firstRow[0];
            for (int i = 0; i < n; i++) {
                if ((row[i] == firstRow[i]) != same) {
                    return -1;
                }
            }
        }

        return minSwap(firstRow, rowCnt) + minSwap(firstCol, colCnt);
    }

    // 计算最小交换次数
    private int minSwap(int[] s, int[] cnt) {
        int n = s.length;
        int x0 = cnt[1] > cnt[0] ? 1 : 0; // 如果 n 是偶数，x0 是 0
        int diff = 0;
        for (int i = 0; i < n; i++) {
            // 计算规则见题解正文最后一段，这里 t[i] = i % 2 ^ x0
            diff += s[i] ^ i % 2 ^ x0;
        }
        return n % 2 > 0 ? diff / 2 : Math.min(diff, n - diff) / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int movesToChessboard(vector<vector<int>>& board) {
        int n = board.size();
        auto& first_row = board[0];
        vector<int> first_col(n);
        int row_cnt[2]{}, col_cnt[2]{};
        for (int i = 0; i < n; i++) {
            row_cnt[first_row[i]]++; // 统计 0 和 1 的个数
            first_col[i] = board[i][0];
            col_cnt[first_col[i]]++;
        }

        // 第一行，0 和 1 的个数之差不能超过 1
        // 第一列，0 和 1 的个数之差不能超过 1
        if (abs(row_cnt[0] - row_cnt[1]) > 1 || abs(col_cnt[0] - col_cnt[1]) > 1) {
            return -1;
        }

        // 每一行和第一行比较，要么完全相同，要么完全不同
        for (auto& row : board) {
            bool same = row[0] == first_row[0];
            for (int i = 0; i < n; i++) {
                if ((row[i] == first_row[i]) != same) {
                    return -1;
                }
            }
        }

        // 计算最小交换次数
        auto min_swap = [&](vector<int>& s, int cnt[2]) {
            int x0 = cnt[1] > cnt[0]; // 如果 n 是偶数，x0 是 0
            int diff = 0;
            for (int i = 0; i < n; i++) {
                // 计算规则见题解正文最后一段，这里 t[i] = i % 2 ^ x0
                diff += s[i] ^ i % 2 ^ x0;
            }
            return n % 2 ? diff / 2 : min(diff, n - diff) / 2;
        };

        return min_swap(first_row, row_cnt) + min_swap(first_col, col_cnt);
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

// 计算最小交换次数
int minSwap(int* s, int n, int cnt[2]) {
    int x0 = cnt[1] > cnt[0]; // 如果 n 是偶数，x0 是 0
    int diff = 0;
    for (int i = 0; i < n; i++) {
        // 计算规则见题解正文最后一段，这里 t[i] = i % 2 ^ x0
        diff += s[i] ^ i % 2 ^ x0;
    }
    return n % 2 ? diff / 2 : MIN(diff, n - diff) / 2;
}

int movesToChessboard(int** board, int n, int* boardColSize) {
    int* firstRow = board[0];
    int rowCnt[2] = {}, colCnt[2] = {};
    for (int i = 0; i < n; i++) {
        rowCnt[firstRow[i]]++;
        colCnt[board[i][0]]++;
    }
    // 第一行，0 和 1 的个数之差不能超过 1
    // 第一列，0 和 1 的个数之差不能超过 1
    if (abs(rowCnt[0] - rowCnt[1]) > 1 || abs(colCnt[0] - colCnt[1]) > 1) {
        return -1;
    }

    // 每一行和第一行比较，要么完全相同，要么完全不同
    for (int i = 0; i < n; i++) {
        bool same = board[i][0] == firstRow[0];
        for (int j = 0; j < n; j++) {
            if ((board[i][j] == firstRow[j]) != same) {
                return -1;
            }
        }
    }

    int* firstCol = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        firstCol[i] = board[i][0];
    }
    int ans = minSwap(firstRow, n, rowCnt) + minSwap(firstCol, n, colCnt);
    free(firstCol);
    return ans;
}
```

```go [sol-Go]
func movesToChessboard(board [][]int) int {
    n := len(board)
    firstRow := board[0]
    firstCol := make([]int, n)
    var rowCnt, colCnt [2]int
    for i, row := range board {
        rowCnt[firstRow[i]]++ // 统计 0 和 1 的个数
        firstCol[i] = row[0]
        colCnt[firstCol[i]]++
    }

    // 第一行，0 和 1 的个数之差不能超过 1
    // 第一列，0 和 1 的个数之差不能超过 1
    if abs(rowCnt[0]-rowCnt[1]) > 1 || abs(colCnt[0]-colCnt[1]) > 1 {
        return -1
    }

    // 每一行和第一行比较，要么完全相同，要么完全不同
    for _, row := range board {
        same := row[0] == firstRow[0]
        for i, x := range row {
            if (x == firstRow[i]) != same {
                return -1
            }
        }
    }

    return minSwap(firstRow, rowCnt) + minSwap(firstCol, colCnt)
}

// 计算最小交换次数
func minSwap(s []int, cnt [2]int) int {
    x0 := 0 // 如果 n 是偶数，x0 是 0
    if cnt[1] > cnt[0] {
        x0 = 1
    }
    diff := 0
    for i, x := range s {
        // 计算规则见题解正文最后一段，这里 t[i] = i%2 ^ x0
        diff += x ^ i%2 ^ x0;
    }
    n := len(s)
    if n%2 > 0 {
        return diff / 2
    }
    return min(diff, n-diff) / 2
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

```js [sol-JavaScript]
var movesToChessboard = function(board) {
    // 第一行，0 和 1 的个数之差不能超过 1
    const firstRow = board[0];
    const rowCnt = [0, 0];
    firstRow.forEach(x => rowCnt[x]++);
    if (Math.abs(rowCnt[0] - rowCnt[1]) > 1) {
        return -1;
    }

    // 第一列，0 和 1 的个数之差不能超过 1
    const firstCol = board.map(row => row[0]);
    const colCnt = [0, 0];
    firstCol.forEach(x => colCnt[x]++);
    if (Math.abs(colCnt[0] - colCnt[1]) > 1) {
        return -1;
    }

    // 每一行和第一行比较，要么完全相同，要么完全不同
    for (const row of board) {
        const same = row[0] === firstRow[0];
        for (let i = 0; i < row.length; i++) {
            if ((row[i] === firstRow[i]) !== same) {
                return -1;
            }
        }
    }

    return minSwap(firstRow, rowCnt) + minSwap(firstCol, colCnt);
};

// 计算最小交换次数
function minSwap(s, cnt) {
    const n = s.length;
    const x0 = cnt[1] > cnt[0] ? 1 : 0; // 如果 n 是偶数，x0 是 0
    let diff = 0;
    for (let i = 0; i < n; i++) {
        // 计算规则见题解正文最后一段，这里 t[i] = i % 2 ^ x0
        diff += s[i] ^ i % 2 ^ x0;
    }
    return n % 2 ? diff / 2 : Math.min(diff, n - diff) / 2;
}
```

```rust [sol-Rust]
impl Solution {
    pub fn moves_to_chessboard(board: Vec<Vec<i32>>) -> i32 {
        // 第一行，0 和 1 的个数之差不能超过 1
        let first_row = &board[0];
        let mut row_cnt = [0i32; 2];
        for &x in first_row {
            row_cnt[x as usize] += 1;
        }
        if (row_cnt[0] - row_cnt[1]).abs() > 1 {
            return -1;
        }

        // 第一列，0 和 1 的个数之差不能超过 1
        let first_col = board.iter().map(|row| row[0]).collect::<Vec<_>>();
        let mut col_cnt = [0i32; 2];
        for &x in &first_col {
            col_cnt[x as usize] += 1;
        }
        if (col_cnt[0] - col_cnt[1]).abs() > 1 {
            return -1;
        }

        // 每一行和第一行比较，要么完全相同，要么完全不同
        for row in &board {
            let same = row[0] == first_row[0];
            for (x, y) in row.iter().zip(first_row) {
                if (x == y) != same {
                    return -1;
                }
            }
        }

        Self::min_swap(first_row, row_cnt) + Self::min_swap(&first_col, col_cnt)
    }

    // 计算最小交换次数
    fn min_swap(s: &[i32], cnt: [i32; 2]) -> i32 {
        let x0 = if cnt[1] > cnt[0] { 1 } else { 0 }; // 如果 n 是偶数，x0 是 0
        let diff = s.iter()
            .enumerate()
            .map(|(i, &x)| x ^ (i % 2) as i32 ^ x0) // 计算规则见题解正文最后一段，这里 t[i] = i % 2 ^ x0
            .sum::<i32>();
        let n = s.len() as i32;
        if n % 2 > 0 { diff / 2 } else { diff.min(n - diff) / 2 }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{board}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。如果不用 $\textit{firstCol}$ 则为 $\mathcal{O}(1)$。

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
10. 【本题相关】[贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.4 逆向思维`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.4 逆向思维`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
