# 2056. 棋盘上有效移动组合的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-valid-move-combinations-on-chessboard/
- 题目 slug：`number-of-valid-move-combinations-on-chessboard`
- 来源专题：常用数据结构
- 来源分类路径：编程能力强化训练 / Part C
- 难度分：2611
- 外部题解来源：https://leetcode.cn/problems/number-of-valid-move-combinations-on-chessboard/solutions/1075478/go-mo-ni-by-endlesscheng-kjpt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[回溯（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-valid-move-combinations-on-chessboard/solutions/1075478/go-mo-ni-by-endlesscheng-kjpt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-mo-ni-by-endlesscheng-kjpt`
- topic id：`1075478`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 核心思路

1. 预处理每个棋子的所有合法移动。
2. 写一个回溯，暴力枚举每个棋子的每个合法移动，如果这些棋子没有重叠在一起，则答案加一。

## 细节

具体来说，合法移动包含：

- 棋子的初始位置 $(x_0,y_0)$。
- 棋子的移动方向 $(d_x,d_y)$。
- 棋子的移动次数 $\textit{step}$。

在回溯时，可以剪枝：如果当前棋子的当前这个合法移动，与前面的棋子冲突，即同一时刻两个棋子重叠，那么不往下递归，枚举当前棋子的下一个合法移动。

```py [sol-Python3]
class Solution:
    # 计算位于 (x0,y0) 的棋子在 dirs 这些方向上的所有合法移动
    def generate_moves(self, x0: int, y0: int, dirs: List[Tuple[int, int]]) -> List[Tuple[int, int, int, int, int]]:
        SIZE = 8
        moves = [(x0, y0, 0, 0, 0)]  # 原地不动
        for dx, dy in dirs:
            # 往 (dx,dy) 方向走 1,2,3,... 步
            x, y = x0 + dx, y0 + dy
            step = 1
            while 0 < x <= SIZE and 0 < y <= SIZE:
                moves.append((x0, y0, dx, dy, step))
                x += dx
                y += dy
                step += 1
        return moves

    # 判断两个移动是否合法，即不存在同一时刻两个棋子重叠的情况
    def is_valid(self, move1: Tuple[int, int, int, int, int], move2: Tuple[int, int, int, int, int]) -> bool:
        x1, y1, dx1, dy1, step1 = move1
        x2, y2, dx2, dy2, step2 = move2
        for i in range(max(step1, step2)):
            # 每一秒走一步
            if i < step1:
                x1 += dx1
                y1 += dy1
            if i < step2:
                x2 += dx2
                y2 += dy2
            if x1 == x2 and y1 == y2:  # 重叠
                return False
        return True

    def countCombinations(self, pieces: List[str], positions: List[List[int]]) -> int:
        rook_dirs = (-1, 0), (1, 0), (0, -1), (0, 1)  # 上下左右
        bishop_dirs = (1, 1), (-1, 1), (-1, -1), (1, -1)  # 斜向
        piece_dirs = {'r': rook_dirs, 'b': bishop_dirs, 'q': rook_dirs + bishop_dirs}
        # 预处理所有合法移动
        all_moves = [self.generate_moves(x, y, piece_dirs[piece[0]])
                     for piece, (x, y) in zip(pieces, positions)]

        n = len(pieces)
        path = [None] * n  # 注意 path 的长度是固定的
        ans = 0
        def dfs(i: int) -> None:
            if i == n:
                nonlocal ans
                ans += 1
                return
            # 枚举当前棋子的所有合法移动
            for move1 in all_moves[i]:
                # 判断合法移动 move1 是否有效
                if all(self.is_valid(move1, move2) for move2 in path[:i]):
                    path[i] = move1  # 直接覆盖，无需恢复现场
                    dfs(i + 1)  # 枚举后续棋子的所有合法移动组合
        dfs(0)
        return ans
```

```java [sol-Java]
class Solution {
    // 每种棋子的移动方向
    private static final Map<Character, int[][]> PIECE_DIRS = Map.of(
        'r', new int[][]{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}, // 车
        'b', new int[][]{{1, 1}, {-1, 1}, {-1, -1}, {1, -1}}, // 象
        'q', new int[][]{{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {1, 1}, {-1, 1}, {-1, -1}, {1, -1}} // 皇后
    );

    public int countCombinations(String[] pieces, int[][] positions) {
        int n = pieces.length;
        // 预处理所有合法移动
        Move[][] allMoves = new Move[n][];
        for (int i = 0; i < n; i++) {
            allMoves[i] = generateMoves(positions[i][0], positions[i][1], PIECE_DIRS.get(pieces[i].charAt(0)));
        }

        Move[] path = new Move[n]; // 注意 path 的长度是固定的
        return dfs(0, n, allMoves, path);
    }

    // 起点 (x0,y0)，移动方向 (dx,dy)，移动次数 step
    private record Move(int x0, int y0, int dx, int dy, int step) {
    }

    // 计算位于 (x0,y0) 的棋子在 dirs 这些方向上的所有合法移动
    private Move[] generateMoves(int x0, int y0, int[][] dirs) {
        final int SIZE = 8;
        List<Move> moves = new ArrayList<>();
        moves.add(new Move(x0, y0, 0, 0, 0)); // 原地不动
        for (int[] d : dirs) {
            // 往 d 方向走 1,2,3,... 步
            int x = x0 + d[0], y = y0 + d[1];
            for (int step = 1; 0 < x && x <= SIZE && 0 < y && y <= SIZE; step++) {
                moves.add(new Move(x0, y0, d[0], d[1], step));
                x += d[0];
                y += d[1];
            }
        }
        return moves.toArray(Move[]::new);
    }

    // 判断两个移动是否合法，即不存在同一时刻两个棋子重叠的情况
    private boolean isValid(Move m1, Move m2) {
        int x1 = m1.x0, y1 = m1.y0; // 初始位置
        int x2 = m2.x0, y2 = m2.y0;
        for (int i = 0; i < Math.max(m1.step, m2.step); i++) {
            // 每一秒走一步
            if (i < m1.step) {
                x1 += m1.dx;
                y1 += m1.dy;
            }
            if (i < m2.step) {
                x2 += m2.dx;
                y2 += m2.dy;
            }
            if (x1 == x2 && y1 == y2) { // 重叠
                return false;
            }
        }
        return true;
    }

    private int dfs(int i, int n, Move[][] allMoves, Move[] path) {
        if (i == n) {
            return 1;
        }
        int res = 0;
        outer:
        // 枚举当前棋子的所有合法移动
        for (Move move1 : allMoves[i]) {
            // 判断合法移动 move1 是否有效
            for (int j = 0; j < i; j++) {
                if (!isValid(move1, path[j])) {
                    continue outer; // 无效，枚举下一个 move1
                }
            }
            path[i] = move1; // 直接覆盖，无需恢复现场
            res += dfs(i + 1, n, allMoves, path); // 枚举后续棋子的所有合法移动组合
        }
        return res;
    }
}
```

```cpp [sol-C++]
struct Move {
    int x0, y0; // 起点
    int dx, dy; // 移动方向
    int step;   // 移动次数
};

class Solution {
    vector<pair<int, int>> DIRS = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {1, 1}, {-1, 1}, {-1, -1}, {1, -1}}; // 上下左右 + 斜向
    unordered_map<char, vector<pair<int, int>>> PIECE_DIRS = {
        {'r', {DIRS.begin(), DIRS.begin() + 4}},
        {'b', {DIRS.begin() + 4, DIRS.end()}},
        {'q', DIRS},
    };

    // 计算位于 (x0,y0) 的棋子在 dirs 这些方向上的所有合法移动
    vector<Move> generate_moves(int x0, int y0, vector<pair<int, int>>& dirs) {
        const int SIZE = 8;
        vector<Move> moves = {{x0, y0, 0, 0, 0}}; // 原地不动
        for (auto [dx, dy] : dirs) {
            // 往 d 方向走 1,2,3,... 步
            int x = x0 + dx, y = y0 + dy;
            for (int step = 1; 0 < x && x <= SIZE && 0 < y && y <= SIZE; step++) {
                moves.emplace_back(x0, y0, dx, dy, step);
                x += dx;
                y += dy;
            }
        }
        return moves;
    }

    // 判断两个移动是否合法，即不存在同一时刻两个棋子重叠的情况
    bool is_valid(Move& m1, Move& m2) {
        int x1 = m1.x0, y1 = m1.y0;
        int x2 = m2.x0, y2 = m2.y0;
        for (int i = 0; i < max(m1.step, m2.step); i++) {
            // 每一秒走一步
            if (i < m1.step) {
                x1 += m1.dx;
                y1 += m1.dy;
            }
            if (i < m2.step) {
                x2 += m2.dx;
                y2 += m2.dy;
            }
            if (x1 == x2 && y1 == y2) { // 重叠
                return false;
            }
        }
        return true;
    }

public:
    int countCombinations(vector<string>& pieces, vector<vector<int>>& positions) {
        int n = pieces.size();
        // 预处理所有合法移动
        vector<vector<Move>> all_moves(n);
        for (int i = 0; i < n; i++) {
            all_moves[i] = generate_moves(positions[i][0], positions[i][1], PIECE_DIRS[pieces[i][0]]);
        }

        vector<Move> path(n); // 注意 path 的长度是固定的
        int ans = 0;
        auto dfs = [&](auto& dfs, int i) -> void {
            if (i == n) {
                ans++;
                return;
            }
            // 枚举当前棋子的所有合法移动
            for (Move& move1 : all_moves[i]) {
                // 判断合法移动 move1 是否有效
                bool ok = true;
                for (int j = 0; j < i; j++) {
                    if (!is_valid(move1, path[j])) {
                        ok = false;
                        break;
                    }
                }
                if (ok) {
                    path[i] = move1; // 直接覆盖，无需恢复现场
                    dfs(dfs, i + 1); // 枚举后续棋子的所有合法移动组合
                }
            }
        };
        dfs(dfs, 0);
        return ans;
    }
};
```

```go [sol-Go]
type move struct {
    x0, y0 int // 起点 
    dx, dy int // 移动方向
    step   int // 移动次数
}

// 计算位于 (x0,y0) 的棋子在 dirs 这些方向上的所有合法移动
func generateMoves(x0, y0 int, dirs []struct{ x, y int }) []move {
    const size = 8
    moves := []move{{x0, y0, 0, 0, 0}} // 原地不动
    for _, d := range dirs {
        // 往 d 方向走 1,2,3,... 步
        x, y := x0+d.x, y0+d.y
        for step := 1; 0 < x && x <= size && 0 < y && y <= size; step++ {
            moves = append(moves, move{x0, y0, d.x, d.y, step})
            x += d.x
            y += d.y
        }
    }
    return moves
}

// 判断两个移动是否合法，即不存在同一时刻两个棋子重叠的情况
func isValid(m1, m2 move) bool {
    x1, y1 := m1.x0, m1.y0
    x2, y2 := m2.x0, m2.y0
    for i := range max(m1.step, m2.step) {
        // 每一秒走一步
        if i < m1.step {
            x1 += m1.dx
            y1 += m1.dy
        }
        if i < m2.step {
            x2 += m2.dx
            y2 += m2.dy
        }
        if x1 == x2 && y1 == y2 { // 重叠
            return false
        }
    }
    return true
}

var dirs = []struct{ x, y int }{{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {1, 1}, {-1, 1}, {-1, -1}, {1, -1}} // 上下左右 + 斜向
var pieceDirs = map[byte][]struct{ x, y int }{'r': dirs[:4], 'b': dirs[4:], 'q': dirs}

func countCombinations(pieces []string, positions [][]int) (ans int) {
    n := len(pieces)
    // 预处理所有合法移动
    allMoves := make([][]move, n)
    for i, pos := range positions {
        allMoves[i] = generateMoves(pos[0], pos[1], pieceDirs[pieces[i][0]])
    }

    path := make([]move, n) // 注意 path 的长度是固定的
    var dfs func(int)
    dfs = func(i int) {
        if i == n {
            ans++
            return
        }
    outer:
        // 枚举当前棋子的所有合法移动
        for _, move1 := range allMoves[i] {
            // 判断合法移动 move1 是否有效
            for _, move2 := range path[:i] {
                if !isValid(move1, move2) {
                    continue outer // 无效，枚举下一个 move1
                }
            }
            path[i] = move1 // 直接覆盖，无需恢复现场
            dfs(i + 1) // 枚举后续棋子的所有合法移动组合
        }
    }
    dfs(0)
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nLM^n)$，其中 $n$ 是 $\textit{pieces}$ 的长度，$L=8$，$M=\mathcal{O}(L)$ 是单个棋子的合法移动个数。搜索树是一棵 $M$ 叉树，深度为 $\mathcal{O}(n)$，一共有 $\mathcal{O}(M^n)$ 个节点。每个节点需要用 $\mathcal{O}(nL)$ 的时间，判断第 $i$ 个棋子的其中一个合法移动是否有效。
- 空间复杂度：$\mathcal{O}(nM)$。

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
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `编程能力强化训练 / Part C`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `编程能力强化训练 / Part C`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
