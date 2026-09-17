# 37. 解数独

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sudoku-solver/
- 题目 slug：`sudoku-solver`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.5 排列型回溯
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/sudoku-solver/solutions/3767438/shu-du-zen-yao-wan-ti-mu-jiu-zen-yao-zuo-ms2q/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[数独怎么玩，题目就怎么做：回溯+堆优化（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/sudoku-solver/solutions/3767438/shu-du-zen-yao-wan-ti-mu-jiu-zen-yao-zuo-ms2q/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-du-zen-yao-wan-ti-mu-jiu-zen-yao-zuo-ms2q`
- topic id：`3767438`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 前置题目

1. [36. 有效的数独](https://leetcode.cn/problems/valid-sudoku/)。学会如何把当前位置转化成「宫」的位置。
2. [46. 全排列](https://leetcode.cn/problems/permutations/)。视频讲解：[排列型回溯【基础算法精讲 16】](https://www.bilibili.com/video/BV1mY411D7f6/)。

## 核心思路

游玩数独的技巧之一是，**优先填待定数字最少的空格子**。

![lc37.png](https://pic.leetcode.cn/1756519012-lBxHRE-lc37.png)

示例 1 如上图。看正中间的空格子 $\textit{board}[4][4]$：

- 横着看，这一行有 $1,3,4,8$。
- 竖着看，这一列有 $1,2,6,7,8,9$。
- 正中间的宫有 $2,3,6,8$。
- 所以我们能填的数字只有 $5$。

填完 $\textit{board}[4][4]=5$ 后，继续找下一个待定数字最少的空格子。重复上述过程，直到所有格子都填完。

如果一个格子有多个数字可以填，那么就像 [46. 全排列](https://leetcode.cn/problems/permutations/) 那样，**枚举填入的数字**。

> 注：题目保证输入的数独**有唯一解**。

## 实现细节

类似 [36. 有效的数独](https://leetcode.cn/problems/valid-sudoku/)，用三组哈希表（布尔数组）维护：

1. $\textit{rowSet}[i]$ 维护第 $i$ 行填入的数字。
2. $\textit{colSet}[j]$ 维护第 $j$ 列填入的数字。
3. $\textit{subBoxSet}[i'][j']$ 维护 $(i',j')$ 宫填入的数字。其中 $\textit{board}[i][j]$ 在 $\left(\left\lfloor\dfrac{i}{3}\right\rfloor, \left\lfloor\dfrac{j}{3}\right\rfloor \right)$ 这个宫。

为了快速找到待定数字最少的空格子，用一个**最小堆**维护三元组 $(\textit{candidates},i,j)$，其中 $\textit{candidates}$ 等于 $(i,j)$ 这个空格子可以填入的数字个数。那么堆顶就是 $\textit{candidates}$ 最少的空格子。

然而，当我们把数字填入一个空格子时，同一行、同一列、同一宫的其余空格子的 $\textit{candidates}$ 都会减少 $1$。难道要动态修改堆中元素？

我们采取一个简单的折中方案：在枚举填入数字的循环中，**重新统计**当前格子 $(i,j)$ 的实际待定数字个数 $\textit{candidates}'$。在恢复现场的时候，把三元组 $(\textit{candidates}',i,j)$ 入堆。

```py [sol-Python3]
class Solution:
    def solveSudoku(self, board: List[List[str]]) -> None:
        row_set = [set() for _ in range(9)]  # 每行填入的数字
        col_set = [set() for _ in range(9)]  # 每列填入的数字
        sub_box_set = [[set() for _ in range(3)] for _ in range(3)]  # 每宫填入的数字
        empty_pos = []  # 空格子的位置

        for i, row in enumerate(board):
            for j, b in enumerate(row):
                if b == '.':
                    empty_pos.append((i, j))  # 记录空格子的位置
                else:
                    x = int(b)
                    # 标记行、列、宫包含数字 x
                    row_set[i].add(x)
                    col_set[j].add(x)
                    sub_box_set[i // 3][j // 3].add(x)

        # get_candidates(i, j) 计算 (i, j) 这个空格子的待定数字个数，最小的在堆顶
        get_candidates = lambda i, j: 9 - len(row_set[i] | col_set[j] | sub_box_set[i // 3][j // 3])
        empty_heap = [(get_candidates(i, j), i, j) for i, j in empty_pos]
        heapify(empty_heap)

        # 每次递归，选一个空格子，枚举填入的数字
        def dfs() -> bool:
            if not empty_heap:  # 所有格子都已填入数字
                return True  # 完成数独

            # 数独玩法：优先考虑待定数字个数最少的空格子
            _, i, j = heappop(empty_heap)

            candidates = 0  # 受之前填入的数字影响，实际待定数字个数可能比入堆时的少，需要重新计算
            # 枚举 1~9 中没填过的数字 x，填入 board[i][j]
            for x in range(1, 10):
                if x in row_set[i] or x in col_set[j] or x in sub_box_set[i // 3][j // 3]:
                    continue  # x 填过了

                # 把数字 x 转成字符，填入 board[i][j]
                board[i][j] = digits[x]
                # 标记行、列、宫包含数字 x
                row_set[i].add(x)
                col_set[j].add(x)
                sub_box_set[i // 3][j // 3].add(x)

                # 填下一个空格子
                if dfs():
                    return True  # 完成数独

                # 恢复现场（撤销）
                # 注意 board[i][j] 无需恢复现场，因为我们会直接覆盖掉之前填入的数字
                row_set[i].remove(x)
                col_set[j].remove(x)
                sub_box_set[i // 3][j // 3].remove(x)

                # 统计待定数字个数
                candidates += 1

            # 恢复现场（撤销）
            heappush(empty_heap, (candidates, i, j))  # 重新入堆（更新待定数字个数）
            # 所有填法都不行，说明之前（祖先节点）的填法是错的
            return False

        dfs()
```

```java [sol-Java]
class Solution {
    public void solveSudoku(char[][] board) {
        boolean[][] rowHas = new boolean[9][9]; // rowHas[i][x] 表示 i 行是否有数字 x
        boolean[][] colHas = new boolean[9][9]; // colHas[j][x] 表示 j 列是否有数字 x
        boolean[][][] subBoxHas = new boolean[3][3][9]; // subBoxHas[i'][j'][x] 表示 (i',j') 宫是否有数字 x
        List<int[]> emptyPos = new ArrayList<>(); // 空格子的位置

        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (board[i][j] == '.') {
                    emptyPos.add(new int[]{i, j}); // 记录空格子的位置
                } else {
                    int x = board[i][j] - '1'; // 字符 '1'~'9' 转成数字 0~8
                    // 标记行、列、宫包含数字 x
                    rowHas[i][x] = colHas[j][x] = subBoxHas[i / 3][j / 3][x] = true;
                }
            }
        }

        PriorityQueue<int[]> emptyPQ = new PriorityQueue<>((a, b) -> a[0] - b[0]);
        for (int[] pos : emptyPos) {
            int i = pos[0];
            int j = pos[1];
            int candidates = getCandidates(i, j, rowHas, colHas, subBoxHas);
            emptyPQ.offer(new int[]{candidates, i, j}); // 待定数字个数最少的在堆顶
        }

        dfs(board, rowHas, colHas, subBoxHas, emptyPQ);
    }

    // 计算 (i, j) 这个空格子的待定数字个数
    private int getCandidates(int i, int j, boolean[][] rowHas, boolean[][] colHas, boolean[][][] subBoxHas) {
        int candidates = 9;
        for (int x = 0; x < 9; x++) {
            if (rowHas[i][x] || colHas[j][x] || subBoxHas[i / 3][j / 3][x]) {
                candidates--;
            }
        }
        return candidates;
    }

    // 每次递归，选一个空格子，枚举填入的数字
    private boolean dfs(char[][] board, boolean[][] rowHas, boolean[][] colHas, boolean[][][] subBoxHas, PriorityQueue<int[]> emptyPQ) {
        if (emptyPQ.isEmpty()) { // 所有格子都已填入数字
            return true; // 完成数独
        }

        // 数独玩法：优先考虑待定数字个数最少的空格子
        int[] top = emptyPQ.poll();
        int i = top[1];
        int j = top[2];

        int candidates = 0; // 受之前填入的数字影响，实际待定数字个数可能比入堆时的少，需要重新计算
        // 枚举没填过的数字 x，填入 board[i][j]
        for (int x = 0; x < 9; x++) {
            if (rowHas[i][x] || colHas[j][x] || subBoxHas[i / 3][j / 3][x]) {
                continue; // x 填过了
            }

            // 填入 board[i][j]
            board[i][j] = (char) ('1' + x);
            // 标记行、列、宫包含数字 x
            rowHas[i][x] = colHas[j][x] = subBoxHas[i / 3][j / 3][x] = true;

            // 填下一个空格子
            if (dfs(board, rowHas, colHas, subBoxHas, emptyPQ)) {
                return true; // 完成数独
            }

            // 恢复现场（撤销）
            // 注意 board[i][j] 无需恢复现场，因为我们会直接覆盖掉之前填入的数字
            rowHas[i][x] = colHas[j][x] = subBoxHas[i / 3][j / 3][x] = false;

            // 统计待定数字个数
            candidates++;
        }

        // 恢复现场（撤销）
        emptyPQ.offer(new int[]{candidates, i, j}); // 重新入堆（更新待定数字个数）
        // 所有填法都不行，说明之前（祖先节点）的填法是错的
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    void solveSudoku(vector<vector<char>>& board) {
        bool row_has[9][9]{}; // row_has[i][x] 表示 i 行是否有数字 x
        bool col_has[9][9]{}; // col_has[j][x] 表示 j 列是否有数字 x
        bool sub_box_has[3][3][9]{}; // sub_box_has[i'][j'][x] 表示 (i',j') 宫是否有数字 x
        vector<pair<int, int>> empty_pos; // 空格子的位置

        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                char b = board[i][j];
                if (b == '.') {
                    empty_pos.emplace_back(i, j); // 记录空格子的位置
                } else {
                    int x = b - '1'; // 字符 '1'~'9' 转成数字 0~8
                    // 标记行、列、宫包含数字 x
                    row_has[i][x] = col_has[j][x] = sub_box_has[i / 3][j / 3][x] = true;
                }
            }
        }

        // 计算 (i, j) 这个空格子的待定数字个数
        auto get_candidates = [&](int i, int j) -> int {
            int candidates = 9;
            for (int x = 0; x < 9; x++) {
                if (row_has[i][x] || col_has[j][x] || sub_box_has[i / 3][j / 3][x]) {
                    candidates--;
                }
            }
            return candidates;
        };

        priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>, greater<>> empty_pq;
        for (auto& [i, j] : empty_pos) {
            empty_pq.emplace(get_candidates(i, j), i, j);
        }

        // 每次递归，选一个空格子，枚举填入的数字
        // lambda 递归
        auto dfs = [&](this auto&& dfs) -> bool {
            if (empty_pq.empty()) { // 所有格子都已填入数字
                return true; // 完成数独
            }

            // 数独玩法：优先考虑待定数字个数最少的空格子
            auto [_, i, j] = empty_pq.top();
            empty_pq.pop();

            int candidates = 0; // 受之前填入的数字影响，实际待定数字个数可能比入堆时的少，需要重新计算
            // 枚举没填过的数字 x，填入 board[i][j]
            for (int x = 0; x < 9; x++) {
                if (row_has[i][x] || col_has[j][x] || sub_box_has[i / 3][j / 3][x]) {
                    continue; // x 填过了
                }

                // 填入 board[i][j]
                board[i][j] = '1' + x;
                // 标记行、列、宫包含数字 x
                row_has[i][x] = col_has[j][x] = sub_box_has[i / 3][j / 3][x] = true;

                // 填下一个空格子
                if (dfs()) {
                    return true; // 完成数独
                }

                // 恢复现场（撤销）
                // 注意 board[i][j] 无需恢复现场，因为我们会直接覆盖掉之前填入的数字
                row_has[i][x] = col_has[j][x] = sub_box_has[i / 3][j / 3][x] = false;

                // 统计待定数字个数
                candidates++;
            }

            // 恢复现场（撤销）
            empty_pq.emplace(candidates, i, j); // 重新入堆（更新待定数字个数）
            // 所有填法都不行，说明之前（祖先节点）的填法是错的
            return false;
        };

        dfs();
    }
};
```

```go [sol-Go]
func solveSudoku(board [][]byte) {
    rowHas := [9][9]bool{}       // rowHas[i][x] 表示 i 行是否有数字 x
    colHas := [9][9]bool{}       // colHas[j][x] 表示 j 列是否有数字 x
    subBoxHas := [3][3][9]bool{} // subBoxHas[i'][j'][x] 表示 (i',j') 宫是否有数字 x
    emptyPos := [][2]int{}       // 空格子的位置

    for i, row := range board {
        for j, b := range row {
            if b == '.' {
                emptyPos = append(emptyPos, [2]int{i, j}) // 记录空格子的位置
            } else {
                x := b - '1' // 字符 '1'~'9' 转成数字 0~8
                // 标记行、列、宫包含数字 x
                rowHas[i][x] = true
                colHas[j][x] = true
                subBoxHas[i/3][j/3][x] = true
            }
        }
    }

    // 计算 (i, j) 这个空格子的待定数字个数
    getCandidates := func(i, j int) int {
        candidates := 9
        for x := range 9 {
            if rowHas[i][x] || colHas[j][x] || subBoxHas[i/3][j/3][x] {
                candidates--
            }
        }
        return candidates
    }

    emptyHeap := make(hp, len(emptyPos))
    for k, pos := range emptyPos {
        i, j := pos[0], pos[1]
        emptyHeap[k] = tuple{getCandidates(i, j), i, j}
    }
    heap.Init(&emptyHeap)

    // 每次递归，选一个空格子，枚举填入的数字
    var dfs func() bool
    dfs = func() bool {
        if len(emptyHeap) == 0 { // 所有格子都已填入数字
            return true // 完成数独
        }

        // 数独玩法：优先考虑待定数字个数最少的空格子
        t := heap.Pop(&emptyHeap).(tuple)
        i, j := t.i, t.j

        candidates := 0 // 受之前填入的数字影响，实际待定数字个数可能比入堆时的少，需要重新计算
        // 枚举没填过的数字 x，填入 board[i][j]
        for x := range 9 {
            if rowHas[i][x] || colHas[j][x] || subBoxHas[i/3][j/3][x] {
                continue // x 填过了
            }

            // 填入 board[i][j]
            board[i][j] = '1' + byte(x)
            // 标记行、列、宫包含数字 x
            rowHas[i][x] = true
            colHas[j][x] = true
            subBoxHas[i/3][j/3][x] = true

            // 填下一个空格子
            if dfs() {
                return true // 完成数独
            }

            // 恢复现场（撤销）
            // 注意 board[i][j] 无需恢复现场，因为我们会直接覆盖掉之前填入的数字
            rowHas[i][x] = false
            colHas[j][x] = false
            subBoxHas[i/3][j/3][x] = false

            // 统计待定数字个数
            candidates++
        }

        // 恢复现场（撤销）
        heap.Push(&emptyHeap, tuple{candidates, i, j}) // 重新入堆（更新待定数字个数）
        // 所有填法都不行，说明之前（祖先节点）的填法是错的
        return false
    }

    dfs()
}

type tuple struct{ candidates, i, j int }
type hp []tuple

func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].candidates < h[j].candidates }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(tuple)) }
func (h *hp) Pop() any          { a := *h; v := a[len(a)-1]; *h = a[:len(a)-1]; return v }
```

```js [sol-JavaScript]
var solveSudoku = function(board) {
    const rowHas = Array.from({length: 9}, () => Array(9).fill(false)); // rowHas[i][x] 表示 i 行是否有数字 x
    const colHas = Array.from({length: 9}, () => Array(9).fill(false)); // colHas[j][x] 表示 j 列是否有数字 x
    const subBoxHas = Array.from({length: 3}, () => Array.from({length: 3}, () => Array(9).fill(false))); // subBoxHas[i'][j'][x] 表示 (i',j') 宫是否有数字 x
    const emptyPos = []; // 空格子的位置

    for (let i = 0; i < 9; i++) {
        for (let j = 0; j < 9; j++) {
            const b = board[i][j];
            if (b === '.') {
                emptyPos.push([i, j]); // 记录空格子的位置
            } else {
                const x = b.charCodeAt(0) - '1'.charCodeAt(0); // 字符 '1'~'9' 转成数字 0~8
                // 标记行、列、宫包含数字 x
                rowHas[i][x] = colHas[j][x] = subBoxHas[Math.floor(i / 3)][Math.floor(j / 3)][x] = true;
            }
        }
    }

    // 计算 (i, j) 这个空格子的待定数字个数
    function getCandidates(i, j) {
        let candidates = 9;
        for (let x = 0; x < 9; x++) {
            if (rowHas[i][x] || colHas[j][x] || subBoxHas[Math.floor(i / 3)][Math.floor(j / 3)][x]) {
                candidates--;
            }
        }
        return candidates;
    }

    const emptyPQ = new MinPriorityQueue(e => e[0]);
    for (const [i, j] of emptyPos) {
        emptyPQ.enqueue([getCandidates(i, j), i, j]);
    }

    // 每次递归，选一个空格子，枚举填入的数字
    function dfs() {
        if (emptyPQ.isEmpty()) { // 所有格子都已填入数字
            return true; // 完成数独
        }

        // 数独玩法：优先考虑待定数字个数最少的空格子
        const [_, i, j] = emptyPQ.dequeue();

        let candidates = 0; // 受之前填入的数字影响，实际待定数字个数可能比入堆时的少，需要重新计算
        // 枚举没填过的数字 x，填入 board[i][j]
        for (let x = 0; x < 9; x++) {
            if (rowHas[i][x] || colHas[j][x] || subBoxHas[Math.floor(i / 3)][Math.floor(j / 3)][x]) {
                continue; // x 填过了
            }

            // 填入 board[i][j]
            board[i][j] = (x + 1).toString(); 
            // 标记行、列、宫包含数字 x
            rowHas[i][x] = colHas[j][x] = subBoxHas[Math.floor(i / 3)][Math.floor(j / 3)][x] = true;

            // 填下一个空格子
            if (dfs()) {
                return true; // 完成数独
            }

            // 恢复现场（撤销）
            // 注意 board[i][j] 无需恢复现场，因为我们会直接覆盖掉之前填入的数字
            rowHas[i][x] = colHas[j][x] = subBoxHas[Math.floor(i / 3)][Math.floor(j / 3)][x] = false;

            // 统计待定数字个数
            candidates++;
        }

        // 恢复现场（撤销）
        emptyPQ.enqueue([candidates, i, j]); // 重新入堆（更新待定数字个数）
        // 所有填法都不行，说明之前（祖先节点）的填法是错的
        return false;
    }

    dfs();
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn solve_sudoku(board: &mut Vec<Vec<char>>) {
        let mut row_has = [[false; 9]; 9]; // row_has[i][x] 表示 i 行是否有数字 x
        let mut col_has = [[false; 9]; 9]; // col_has[j][x] 表示 j 列是否有数字 x
        let mut sub_box_has = [[[false; 9]; 3]; 3]; // sub_box_has[i'][j'][x] 表示 (i',j') 宫是否有数字 x
        let mut empty_pos = vec![]; // 空格子的位置

        for (i, row) in board.iter().enumerate() {
            for (j, &b) in row.iter().enumerate() {
                if b == '.' {
                    empty_pos.push((i, j)); // 记录空格子的位置
                } else {
                    let x = (b as u8 - b'1') as usize; // 字符 '1'~'9' 转成数字 0~8
                    // 标记行、列、宫包含数字 x
                    row_has[i][x] = true;
                    col_has[j][x] = true;
                    sub_box_has[i / 3][j / 3][x] = true;
                }
            }
        }

        // 计算 (i, j) 这个空格子的待定数字个数
        let get_candidates = |i: usize, j: usize| -> i8 {
            let mut candidates = 9;
            for x in 0..9 {
                if row_has[i][x] || col_has[j][x] || sub_box_has[i / 3][j / 3][x] {
                    candidates -= 1;
                }
            }
            candidates
        };

        let mut empty_heap = BinaryHeap::new();
        for (i, j) in empty_pos {
            // 取相反数，把 empty_pq 当作最小堆
            empty_heap.push((-get_candidates(i, j), i, j));
        }

        // 每次递归，选一个空格子，枚举填入的数字
        fn dfs(board: &mut [Vec<char>], row_has: &mut [[bool; 9]; 9], col_has: &mut [[bool; 9]; 9], sub_box_has: &mut [[[bool; 9]; 3]; 3], empty_heap: &mut BinaryHeap<(i8, usize, usize)>) -> bool {
            if empty_heap.is_empty() { // 所有格子都已填入数字
                return true; // 完成数独
            }

            // 数独玩法：优先考虑待定数字个数最少的空格子
            let (_, i, j) = empty_heap.pop().unwrap();

            let mut candidates = 0; // 受之前填入的数字影响，实际待定数字个数可能比入堆时的少，需要重新计算
            // 枚举没填过的数字 x，填入 board[i][j]
            for x in 0..9 {
                if row_has[i][x] || col_has[j][x] || sub_box_has[i / 3][j / 3][x] {
                    continue; // x 填过了
                }

                // 填入 board[i][j]
                board[i][j] = (b'1' + x as u8) as char;
                // 标记行、列、宫包含数字 x
                row_has[i][x] = true;
                col_has[j][x] = true;
                sub_box_has[i / 3][j / 3][x] = true;

                // 填下一个空格子
                if dfs(board, row_has, col_has, sub_box_has, empty_heap) {
                    return true; // 完成数独
                }

                // 恢复现场（撤销）
                // 注意 board[i][j] 无需恢复现场，因为我们会直接覆盖掉之前填入的数字
                row_has[i][x] = false;
                col_has[j][x] = false;
                sub_box_has[i / 3][j / 3][x] = false;

                // 统计待定数字个数
                candidates += 1;
            }

            // 恢复现场（撤销）
            empty_heap.push((-candidates, i, j)); // 重新入堆（更新待定数字个数）
            // 所有填法都不行，说明之前（祖先节点）的填法是错的
            false
        }

        dfs(board, &mut row_has, &mut col_has, &mut sub_box_has, &mut empty_heap);
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(9^m\log m)$，其中 $m\le 81$ 是空格的个数。搜索树的高度为 $m$，每个节点至多有 $9$ 个儿子，所以搜索树有 $\mathcal{O}(9^m)$ 个节点。每个节点需要 $\mathcal{O}(\log m)$ 的实际操作堆。注意这只是一个上界，实际复杂度远小于上界。
- 空间复杂度：$\mathcal{O}(m)$。

注：还有一些更高级的算法，如 [Dancing Links](https://oi-wiki.org/search/dlx/)。

## 专题训练

见下面回溯题单的「**§4.5 排列型回溯**」。

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
