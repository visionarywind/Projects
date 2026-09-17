# 79. 单词搜索

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/word-search/
- 题目 slug：`word-search`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.7 搜索
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/word-search/solutions/2927294/liang-ge-you-hua-rang-dai-ma-ji-bai-jie-g3mmm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[极致优化！代码击败接近 100%！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/word-search/solutions/2927294/liang-ge-you-hua-rang-dai-ma-ji-bai-jie-g3mmm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-ge-you-hua-rang-dai-ma-ji-bai-jie-g3mmm`
- topic id：`2927294`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 前置知识

做本题前，你需要有一些网格图 DFS 的经验和回溯的经验。

- 关于网格图 DFS，可以做做 [200. 岛屿数量](https://leetcode.cn/problems/number-of-islands/)。
- 关于回溯，可以看[【基础算法精讲 14】](https://www.bilibili.com/video/BV1mG4y1A7Gu/)。

## 基本思路（优化前）

枚举 $i=0,1,2,\ldots,m-1$ 和 $j=0,1,2,\ldots,n-1$，以 $(i,j)$ 为起点开始搜索。

同时，我们还需要知道当前匹配到了 $\textit{word}$ 的第几个字母，所以还需要一个参数 $k$。

定义 $\textit{dfs}(i,j,k)$ 表示当前在 $\textit{board}[i][j]$ 这个格子，要匹配 $\textit{word}[k]$，返回在这个状态下最终能否匹配成功（搜索成功）。

分类讨论：

- 如果 $\textit{board}[i][j] \ne \textit{word}[k]$，匹配失败，返回 $\texttt{false}$。
- 否则，如果 $k=\text{len}(\textit{word})-1$，匹配成功，返回 $\texttt{true}$。
- 否则，枚举 $(i,j)$ 周围的四个相邻格子 $(x,y)$，如果 $(x,y)$ 没有出界，则递归 $\textit{dfs}(x,y,k+1)$，如果其返回 $\texttt{true}$，则 $\textit{dfs}(i,j,k)$ 也返回 $\texttt{true}$。
- 如果递归周围的四个相邻格子都没有返回 $\texttt{true}$，则最后返回 $\texttt{false}$，表示没有搜到。

细节：

- 递归过程中，为了避免重复访问同一个格子，可以用 $\textit{vis}$ 数组标记。更简单的做法是，直接修改 $\textit{board}[i][j]$，将其置为空（或者 $0$），返回 $\texttt{false}$ 前再恢复成原来的值（恢复现场）。注意返回 $\texttt{true}$ 的时候就不用恢复现场了，因为已经成功搜到 $\textit{word}$ 了。

```py [sol-Python3]
class Solution:
    def exist(self, board: List[List[str]], word: str) -> bool:
        m, n = len(board), len(board[0])
        def dfs(i: int, j: int, k: int) -> bool:
            if board[i][j] != word[k]:  # 匹配失败
                return False
            if k == len(word) - 1:  # 匹配成功！
                return True
            board[i][j] = ''  # 标记访问过
            for x, y in (i, j - 1), (i, j + 1), (i - 1, j), (i + 1, j):  # 相邻格子
                if 0 <= x < m and 0 <= y < n and dfs(x, y, k + 1):
                    return True  # 搜到了！
            board[i][j] = word[k]  # 恢复现场
            return False  # 没搜到
        return any(dfs(i, j, 0) for i in range(m) for j in range(n))
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    public boolean exist(char[][] board, String word) {
        char[] w = word.toCharArray();
        for (int i = 0; i < board.length; i++) {
            for (int j = 0; j < board[i].length; j++) {
                if (dfs(i, j, 0, board, w)) {
                    return true; // 搜到了！
                }
            }
        }
        return false; // 没搜到
    }

    private boolean dfs(int i, int j, int k, char[][] board, char[] word) {
        if (board[i][j] != word[k]) { // 匹配失败
            return false;
        }
        if (k == word.length - 1) { // 匹配成功！
            return true;
        }
        board[i][j] = 0; // 标记访问过
        for (int[] d : DIRS) {
            int x = i + d[0];
            int y = j + d[1]; // 相邻格子
            if (0 <= x && x < board.length && 0 <= y && y < board[x].length && dfs(x, y, k + 1, board, word)) {
                return true; // 搜到了！
            }
        }
        board[i][j] = word[k]; // 恢复现场
        return false; // 没搜到
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int DIRS[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
public:
    bool exist(vector<vector<char>>& board, string word) {
        int m = board.size(), n = board[0].size();
        auto dfs = [&](this auto&& dfs, int i, int j, int k) -> bool {
            if (board[i][j] != word[k]) { // 匹配失败
                return false;
            }
            if (k + 1 == word.length()) { // 匹配成功！
                return true;
            }
            board[i][j] = 0; // 标记访问过
            for (auto& [dx, dy] : DIRS) {
                int x = i + dx, y = j + dy; // 相邻格子
                if (0 <= x && x < m && 0 <= y && y < n && dfs(x, y, k + 1)) { // 没超过边界，并且后续字母都成功匹配
                    return true;
                }
            }
            board[i][j] = word[k]; // 恢复现场
            return false; // 没搜到
        };
        // 每个格子都可以作为起点
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (dfs(i, j, 0)) {
                    return true; // 搜到了！
                }
            }
        }
        return false; // 没搜到
    }
};
```

```c [sol-C]
int DIRS[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

bool dfs(char** board, char* word, int m, int n, int i, int j, int k) {
    if (board[i][j] != word[k]) {
        return false; // 匹配失败
    }
    if (word[k + 1] == '\0') {
        return true; // 匹配成功！
    }
    board[i][j] = 0; // 标记访问过
    for (int d = 0; d < 4; d++) {
        int x = i + DIRS[d][0], y = j + DIRS[d][1]; // 相邻格子
        if (0 <= x && x < m && 0 <= y && y < n && dfs(board, word, m, n, x, y, k + 1)) {
            return true; // 搜到了！
        }
    }
    board[i][j] = word[k]; // 恢复现场
    return false; // 没搜到
}

bool exist(char** board, int boardSize, int* boardColSize, char* word) {
    int m = boardSize, n = boardColSize[0];
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (dfs(board, word, m, n, i, j, 0)) {
                return true; // 搜到了！
            }
        }
    }
    return false; // 没搜到
}
```

```go [sol-Go]
var dirs = []struct{ x, y int }{{0, -1}, {0, 1}, {-1, 0}, {1, 0}}

func exist(board [][]byte, word string) bool {
    m, n := len(board), len(board[0])
    var dfs func(int, int, int) bool
    dfs = func(i, j, k int) bool {
        if board[i][j] != word[k] { // 匹配失败
            return false
        }
        if k == len(word)-1 { // 匹配成功
            return true
        }
        board[i][j] = 0 // 标记访问过
        for _, d := range dirs {
            x, y := i+d.x, j+d.y // 相邻格子
            if 0 <= x && x < m && 0 <= y && y < n && dfs(x, y, k+1) {
                return true // 搜到了！
            }
        }
        board[i][j] = word[k] // 恢复现场
        return false // 没搜到
    }
    for i := 0; i < m; i++ {
        for j := 0; j < n; j++ {
            if dfs(i, j, 0) {
                return true // 搜到了！
            }
        }
    }
    return false // 没搜到
}
```

```js [sol-JavaScript]
var exist = function(board, word) {
    const m = board.length, n = board[0].length;
    function dfs(i, j, k) {
        if (board[i][j] !== word[k]) {
            return false; // 匹配失败
        }
        if (k + 1 === word.length) {
            return true; // 匹配成功！
        }
        board[i][j] = 0; // 标记访问过
        for (const [x, y] of [[i, j - 1], [i, j + 1], [i - 1, j], [i + 1, j]]) { // 相邻格子
            if (0 <= x && x < m && 0 <= y && y < n && dfs(x, y, k + 1)) {
                return true; // 搜到了！
            }
        }
        board[i][j] = word[k]; // 恢复现场
        return false; // 没搜到
    }
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (dfs(i, j, 0)) {
                return true; // 搜到了！
            }
        }
    }
    return false; // 没搜到
};
```

```rust [sol-Rust]
impl Solution {
    pub fn exist(mut board: Vec<Vec<char>>, word: String) -> bool {
        fn dfs(board: &mut Vec<Vec<char>>, word: &[u8], i: usize, j: usize, k: usize, m: usize, n: usize) -> bool {
            if board[i][j] != word[k] as char {
                return false; // 匹配失败
            }
            if k + 1 == word.len() {
                return true; // 匹配成功！
            }
            board[i][j] = '\0'; // 标记访问过
            for (x, y) in [(i, j - 1), (i, j + 1), (i - 1, j), (i + 1, j)] {
                if x < m && y < n && dfs(board, word, x, y, k + 1, m, n) {
                    return true; // 搜到了！
                }
            }
            board[i][j] = word[k] as char; // 恢复现场
            false // 没搜到
        }
        let m = board.len();
        let n = board[0].len();
        for i in 0..m {
            for j in 0..n {
                if dfs(&mut board, word.as_bytes(), i, j, 0, m, n) {
                    return true; // 搜到了！
                }
            }
        }
        false // 没搜到
    }
}
```

## 第一个优化

![lc79-1.jpg](https://pic.leetcode.cn/1727067229-ZlSuon-lc79-1.jpg)

比如示例 3，$\textit{word}=\texttt{ABCB}$，其中字母 $\texttt{B}$ 出现了 $2$ 次，但 $\textit{board}$ 中只有 $1$ 个字母 $\texttt{B}$，所以肯定搜不到 $\textit{word}$，直接返回 $\texttt{false}$。

一般地，如果 $\textit{word}$ 的某个字母的出现次数，比 $\textit{board}$ 中的这个字母的出现次数还要多，可以直接返回 $\texttt{false}$。

## 第二个优化

**启发**：如果 $\textit{word}=\texttt{abcd}$ 但 $\textit{board}$ 中的 $\texttt{a}$ 很多，$\texttt{d}$ 很少（比如只有一个），那么从 $\texttt{d}$ 开始搜索，能更快地找到答案。（即使我们肉眼去找，这种方法也是更快的）

设 $\textit{word}$ 的第一个字母在 $\textit{board}$ 中出现了 $x$ 次，$\textit{word}$ 的最后一个字母在 $\textit{board}$ 中出现了 $y$ 次。

如果 $y<x$，我们可以把 $\textit{word}$ 反转，相当于从 $\textit{word}$ 的最后一个字母开始搜索，这样更容易在一开始就满足 `board[i][j] != word[k]`，不会往下递归，递归的总次数更少。

加上这两个优化，就可以击败接近 $100\%$ 了！其中 Java、C++、Go 和 Rust 都可以跑到 0ms。

```py [sol-Python3]
class Solution:
    def exist(self, board: List[List[str]], word: str) -> bool:
        cnt = Counter(c for row in board for c in row)
        if not cnt >= Counter(word):  # 优化一
            return False
        if cnt[word[-1]] < cnt[word[0]]:  # 优化二
            word = word[::-1]

        m, n = len(board), len(board[0])
        def dfs(i: int, j: int, k: int) -> bool:
            if board[i][j] != word[k]:  # 匹配失败
                return False
            if k == len(word) - 1:  # 匹配成功！
                return True
            board[i][j] = ''  # 标记访问过
            for x, y in (i, j - 1), (i, j + 1), (i - 1, j), (i + 1, j):  # 相邻格子
                if 0 <= x < m and 0 <= y < n and dfs(x, y, k + 1):
                    return True  # 搜到了！
            board[i][j] = word[k]  # 恢复现场
            return False  # 没搜到
        return any(dfs(i, j, 0) for i in range(m) for j in range(n))
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    public boolean exist(char[][] board, String word) {
        // 为了方便，直接用数组代替哈希表
        int[] cnt = new int[128];
        for (char[] row : board) {
            for (char c : row) {
                cnt[c]++;
            }
        }

        // 优化一
        char[] w = word.toCharArray();
        int[] wordCnt = new int[128];
        for (char c : w) {
            if (++wordCnt[c] > cnt[c]) {
                return false;
            }
        }

        // 优化二
        if (cnt[w[w.length - 1]] < cnt[w[0]]) {
            w = new StringBuilder(word).reverse().toString().toCharArray();
        }

        for (int i = 0; i < board.length; i++) {
            for (int j = 0; j < board[i].length; j++) {
                if (dfs(i, j, 0, board, w)) {
                    return true; // 搜到了！
                }
            }
        }
        return false; // 没搜到
    }

    private boolean dfs(int i, int j, int k, char[][] board, char[] word) {
        if (board[i][j] != word[k]) { // 匹配失败
            return false;
        }
        if (k == word.length - 1) { // 匹配成功！
            return true;
        }
        board[i][j] = 0; // 标记访问过
        for (int[] d : DIRS) {
            int x = i + d[0];
            int y = j + d[1]; // 相邻格子
            if (0 <= x && x < board.length && 0 <= y && y < board[x].length && dfs(x, y, k + 1, board, word)) {
                return true; // 搜到了！
            }
        }
        board[i][j] = word[k]; // 恢复现场
        return false; // 没搜到
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int DIRS[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
public:
    bool exist(vector<vector<char>>& board, string word) {
        unordered_map<char, int> cnt;
        for (auto& row : board) {
            for (char c : row) {
                cnt[c]++;
            }
        }

        // 优化一
        unordered_map<char, int> word_cnt;
        for (char c : word) {
            if (++word_cnt[c] > cnt[c]) {
                return false;
            }
        }

        // 优化二
        if (cnt[word.back()] < cnt[word[0]]) {
            ranges::reverse(word);
        }

        int m = board.size(), n = board[0].size();
        auto dfs = [&](this auto&& dfs, int i, int j, int k) -> bool {
            if (board[i][j] != word[k]) { // 匹配失败
                return false;
            }
            if (k + 1 == word.length()) { // 匹配成功！
                return true;
            }
            board[i][j] = 0; // 标记访问过
            for (auto& [dx, dy] : DIRS) {
                int x = i + dx, y = j + dy; // 相邻格子
                if (0 <= x && x < m && 0 <= y && y < n && dfs(x, y, k + 1)) {
                    return true; // 搜到了！
                }
            }
            board[i][j] = word[k]; // 恢复现场
            return false; // 没搜到
        };
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (dfs(i, j, 0)) {
                    return true; // 搜到了！
                }
            }
        }
        return false; // 没搜到
    }
};
```

```c [sol-C]
int DIRS[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

bool dfs(char** board, char* word, int m, int n, int i, int j, int k) {
    if (board[i][j] != word[k]) {
        return false; // 匹配失败
    }
    if (word[k + 1] == '\0') {
        return true; // 匹配成功！
    }
    board[i][j] = 0; // 标记访问过
    for (int d = 0; d < 4; d++) {
        int x = i + DIRS[d][0], y = j + DIRS[d][1]; // 相邻格子
        if (0 <= x && x < m && 0 <= y && y < n && dfs(board, word, m, n, x, y, k + 1)) {
            return true; // 搜到了！
        }
    }
    board[i][j] = word[k]; // 恢复现场
    return false; // 没搜到
}

bool exist(char** board, int boardSize, int* boardColSize, char* word) {
    int m = boardSize, n = boardColSize[0];
    // 为了方便，用数组代替哈希表
    int cnt[128] = {};
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            cnt[board[i][j]]++;
        }
    }

    // 优化一
    int word_cnt[128] = {};
    int k = 0;
    for (; word[k]; k++) {
        if (++word_cnt[word[k]] > cnt[word[k]]) {
            return false;
        }
    }

    // 优化二
    if (cnt[word[k - 1]] < cnt[word[0]]) {
        // 反转 word
        for (int i = 0; i < k / 2; i++) {
            char tmp = word[i];
            word[i] = word[k - i - 1];
            word[k - i - 1] = tmp;
        }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (dfs(board, word, m, n, i, j, 0)) {
                return true; // 搜到了！
            }
        }
    }
    return false; // 没搜到
}
```

```go [sol-Go]
var dirs = []struct{ x, y int }{{0, -1}, {0, 1}, {-1, 0}, {1, 0}}

func exist(board [][]byte, word string) bool {
    cnt := map[byte]int{}
    for _, row := range board {
        for _, c := range row {
            cnt[c]++
        }
    }

    // 优化一
    w := []byte(word)
    wordCnt := map[byte]int{}
    for _, c := range w {
        wordCnt[c]++
        if wordCnt[c] > cnt[c] {
            return false
        }
    }

    // 优化二
    if cnt[w[len(w)-1]] < cnt[w[0]] {
        slices.Reverse(w)
    }

    m, n := len(board), len(board[0])
    var dfs func(int, int, int) bool
    dfs = func(i, j, k int) bool {
        if board[i][j] != w[k] { // 匹配失败
            return false
        }
        if k == len(w)-1 { // 匹配成功
            return true
        }
        board[i][j] = 0 // 标记访问过
        for _, d := range dirs {
            x, y := i+d.x, j+d.y // 相邻格子
            if 0 <= x && x < m && 0 <= y && y < n && dfs(x, y, k+1) {
                return true // 搜到了！
            }
        }
        board[i][j] = w[k] // 恢复现场
        return false // 没搜到
    }
    for i := 0; i < m; i++ {
        for j := 0; j < n; j++ {
            if dfs(i, j, 0) {
                return true // 搜到了！
            }
        }
    }
    return false // 没搜到
}
```

```js [sol-JavaScript]
var exist = function(board, word) {
    const cnt = new Map();
    for (const row of board) {
        for (const c of row) {
            cnt.set(c, (cnt.get(c) ?? 0) + 1);
        }
    }

    // 优化一
    const wordCnt = new Map();
    for (const c of word) {
        wordCnt.set(c, (wordCnt.get(c) ?? 0) + 1);
        if (wordCnt.get(c) > (cnt.get(c) ?? 0)) {
            return false;
        }
    }

    // 优化二
    if ((cnt.get(word[word.length - 1]) ?? 0) < (cnt.get(word[0]) ?? 0)) {
        word = word.split('').reverse();
    }

    const m = board.length, n = board[0].length;
    function dfs(i, j, k) {
        if (board[i][j] !== word[k]) {
            return false; // 匹配失败
        }
        if (k + 1 === word.length) {
            return true; // 匹配成功！
        }
        board[i][j] = 0; // 标记访问过
        for (const [x, y] of [[i, j - 1], [i, j + 1], [i - 1, j], [i + 1, j]]) { // 相邻格子
            if (0 <= x && x < m && 0 <= y && y < n && dfs(x, y, k + 1)) {
                return true; // 搜到了！
            }
        }
        board[i][j] = word[k]; // 恢复现场
        return false; // 没搜到
    }
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (dfs(i, j, 0)) {
                return true; // 搜到了！
            }
        }
    }
    return false; // 没搜到
};
```

```rust [sol-Rust]
impl Solution {
    pub fn exist(mut board: Vec<Vec<char>>, mut word: String) -> bool {
        // 为了方便，直接用数组代替哈希表
        let mut cnt = [0; 128];
        for row in &board {
            for &c in row {
                cnt[c as usize] += 1;
            }
        }

        // 优化一
        let w = word.as_bytes();
        let mut word_cnt = [0; 128];
        for &c in w {
            let c = c as usize;
            word_cnt[c] += 1;
            if word_cnt[c] > cnt[c] {
                return false;
            }
        }

        // 优化二
        if cnt[w[w.len() - 1] as usize] < cnt[w[0] as usize] {
            word = word.chars().rev().collect();
        }

        fn dfs(board: &mut Vec<Vec<char>>, word: &[u8], i: usize, j: usize, k: usize, m: usize, n: usize) -> bool {
            if board[i][j] != word[k] as char {
                return false; // 匹配失败
            }
            if k + 1 == word.len() {
                return true; // 匹配成功！
            }
            board[i][j] = '\0'; // 标记访问过
            for (x, y) in [(i, j - 1), (i, j + 1), (i - 1, j), (i + 1, j)] { // 相邻格子
                if x < m && y < n && dfs(board, word, x, y, k + 1, m, n) {
                    return true; // 搜到了！
                }
            }
            board[i][j] = word[k] as char; // 恢复现场
            false // 没搜到
        }
        let m = board.len();
        let n = board[0].len();
        for i in 0..m {
            for j in 0..n {
                if dfs(&mut board, word.as_bytes(), i, j, 0, m, n) {
                    return true; // 搜到了！
                }
            }
        }
        false // 没搜到
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn3^k)$，其中 $m$ 和 $n$ 分别为 $\textit{grid}$ 的行数和列数，$k$ 是 $\textit{word}$ 的长度。除了递归入口，其余递归至多有 $3$ 个分支（因为至少有一个方向是之前走过的），所以每次递归（回溯）的时间复杂度为 $\mathcal{O}(3^k)$，一共回溯 $\mathcal{O}(mn)$ 次，所以时间复杂度为 $\mathcal{O}(mn3^k)$。
- 空间复杂度：$\mathcal{O}(|\Sigma| + k)$。其中 $|\Sigma|=52$ 是字符集合的大小。递归需要 $\mathcal{O}(k)$ 的栈空间。部分语言用的数组代替哈希表，可以视作 $|\Sigma|=128$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、回溯 / §4.7 搜索`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.7 搜索`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
