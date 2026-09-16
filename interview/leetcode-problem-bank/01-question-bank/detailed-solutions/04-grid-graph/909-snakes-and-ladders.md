# 909. 蛇梯棋

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/snakes-and-ladders/
- 题目 slug：`snakes-and-ladders`
- 来源专题：网格图
- 来源分类路径：二、网格图 BFS
- 难度分：2020
- 外部题解来源：https://leetcode.cn/problems/snakes-and-ladders/solutions/3690137/bfszhu-yi-ke-neng-you-huan-pythonjavacgo-xcy7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[BFS，注意可能有环（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/snakes-and-ladders/solutions/3690137/bfszhu-yi-ke-neng-you-huan-pythonjavacgo-xcy7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bfszhu-yi-ke-neng-you-huan-pythonjavacgo-xcy7`
- topic id：`3690137`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 思路

**起点**：$1$。

**每一步怎么走**？假设当前在编号 $x$，那么可以移动到 $[x+1,\min(x+6,n^2)]$ 中的编号 $y$。如果编号 $y$ 对应的 $\textit{board}[r][c] > 0$，那么继续移动到编号 $\textit{board}[r][c]$。

**终点**：$n^2$。

如果无法到达终点（有环），返回 $-1$。

关于 BFS 双列表的写法，见[【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)。

## 细节

如何把编号 $y$ 转成行号 $r$ 和列号 $c$？

先确定行号 $r$，再确定列号 $c$。

把编号**减一**可以看得更清楚，比如示例 1：

- $n=6$。
- 编号 $[0,5]$ 在倒数 $0$ 行。
- 编号 $[6,11]$ 在倒数 $1$ 行。
- 编号 $[12,17]$ 在倒数 $2$ 行。
- ……

一般地，$y-1$ 在倒数 $r'=\left\lfloor\dfrac{y-1}{n}\right\rfloor$ 行，即正数 $r=n-1-r'$ 行。

在 $y-1$ 下面有完整的 $r'$ 行，所以在 $r'$ 行还剩下 $c' = y-1-r'\cdot n = (y-1)\bmod n$：

- 如果 $r'$ 是偶数，那么列号 $c = c'$。
- 如果 $r'$ 是奇数，那么列号 $c = n-1-c'$。

```py [sol-Python3]
class Solution:
    def snakesAndLadders(self, board: List[List[int]]) -> int:
        n = len(board)
        vis = [False] * (n * n + 1)
        vis[1] = True  # 题目保证起点没有蛇梯，不写也可以
        q = [1]  # 起点
        step = 0
        while q:
            tmp = q
            q = []
            for x in tmp:
                if x == n * n:  # 终点
                    return step
                for y in range(x + 1, min(x + 6, n * n) + 1):
                    r, c = divmod(y - 1, n)
                    if r % 2:
                        c = n - 1 - c  # 奇数行从右到左
                    nxt = board[-1 - r][c]
                    if nxt < 0:
                        nxt = y
                    if not vis[nxt]:
                        vis[nxt] = True  # 有环的情况下，避免死循环
                        q.append(nxt)
            step += 1
        return -1  # 无法到达终点
```

```java [sol-Java]
// 更快的写法见【Java 数组】
class Solution {
    public int snakesAndLadders(int[][] board) {
        int n = board.length;
        boolean[] vis = new boolean[n * n + 1];
        vis[1] = true; // 题目保证起点没有蛇梯，不写也可以
        List<Integer> q = new ArrayList<>();
        q.add(1); // 起点
        for (int step = 0; !q.isEmpty(); step++) {
            List<Integer> tmp = q;
            q = new ArrayList<>();
            for (int x : tmp) {
                if (x == n * n) { // 终点
                    return step;
                }
                for (int y = x + 1; y <= Math.min(x + 6, n * n); y++) {
                    int r = (y - 1) / n;
                    int c = (y - 1) % n;
                    if (r % 2 > 0) {
                        c = n - 1 - c; // 奇数行从右到左
                    }
                    int nxt = board[n - 1 - r][c];
                    if (nxt < 0) {
                        nxt = y;
                    }
                    if (!vis[nxt]) {
                        vis[nxt] = true; // 有环的情况下，避免死循环
                        q.add(nxt);
                    }
                }
            }
        }
        return -1; // 无法到达终点
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int snakesAndLadders(int[][] board) {
        int n = board.length;
        boolean[] vis = new boolean[n * n + 1];
        vis[1] = true;
        int[] q = new int[n * n];
        int head = 0;
        int tail = 0;
        q[tail++] = 1; // 起点
        for (int step = 0; head < tail; step++) {
            for (int size = tail - head; size > 0; size--) {
                int x = q[head++];
                if (x == n * n) {
                    return step; // 终点
                }
                for (int y = x + 1; y <= Math.min(x + 6, n * n); y++) {
                    int r = (y - 1) / n;
                    int c = (y - 1) % n;
                    if (r % 2 > 0) {
                        c = n - 1 - c; // 奇数行从右到左
                    }
                    int nxt = board[n - 1 - r][c];
                    if (nxt < 0) {
                        nxt = y;
                    }
                    if (!vis[nxt]) {
                        vis[nxt] = true; // 有环的情况下，避免死循环
                        q[tail++] = nxt;
                    }
                }
            }
        }
        return -1; // 无法到达终点
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int snakesAndLadders(vector<vector<int>>& board) {
        int n = board.size();
        vector<int8_t> vis(n * n + 1);
        vis[1] = true; // 题目保证起点没有蛇梯，不写也可以
        vector<int> q = {1}; // 起点
        for (int step = 0; !q.empty(); step++) {
            auto tmp = move(q); // move 后 q 为空
            for (int x : tmp) {
                if (x == n * n) { // 终点
                    return step;
                }
                for (int y = x + 1; y <= min(x + 6, n * n); y++) {
                    int r = (y - 1) / n, c = (y - 1) % n;
                    if (r % 2) {
                        c = n - 1 - c; // 奇数行从右到左
                    }
                    int nxt = board[n - 1 - r][c];
                    if (nxt < 0) {
                        nxt = y;
                    }
                    if (!vis[nxt]) {
                        vis[nxt] = true; // 有环的情况下，避免死循环
                        q.push_back(nxt);
                    }
                }
            }
        }
        return -1; // 无法到达终点
    }
};
```

```go [sol-Go]
func snakesAndLadders(board [][]int) (step int) {
    n := len(board)
    vis := make([]bool, n*n+1)
    vis[1] = true // 题目保证起点没有蛇梯，不写也可以
    q := []int{1} // 起点
    for len(q) > 0 {
        tmp := q
        q = nil
        for _, x := range tmp {
            if x == n*n { // 终点
                return
            }
            for y := x + 1; y <= min(x+6, n*n); y++ {
                r, c := (y-1)/n, (y-1)%n
                if r%2 > 0 {
                    c = n - 1 - c // 奇数行从右到左
                }
                nxt := board[n-1-r][c]
                if nxt < 0 {
                    nxt = y
                }
                if !vis[nxt] {
                    vis[nxt] = true // 有环的情况下，避免死循环
                    q = append(q, nxt)
                }
            }
        }
        step++
    }
    return -1 // 无法到达终点
}
```

```js [sol-JavaScript]
var snakesAndLadders = function(board) {
    const n = board.length;
    const vis = Array(n * n + 1).fill(false);
    vis[1] = true; // 题目保证起点没有蛇梯，不写也可以
    let q = [1]; // 起点
    for (let step = 0; q.length > 0; step++) {
        const tmp = q;
        q = [];
        for (const x of tmp) {
            if (x === n * n) { // 终点
                return step;
            }
            for (let y = x + 1; y <= Math.min(x + 6, n * n); y++) {
                let r = Math.floor((y - 1) / n);
                let c = (y - 1) % n;
                if (r % 2 > 0) {
                    c = n - 1 - c; // 奇数行从右到左
                }
                let nxt = board[n - 1 - r][c];
                if (nxt < 0) {
                    nxt = y;
                }
                if (!vis[nxt]) {
                    vis[nxt] = true; // 有环的情况下，避免死循环
                    q.push(nxt);
                }
            }
        }
    }
    return -1; // 无法到达终点
};
```

```rust [sol-Rust]
impl Solution {
    pub fn snakes_and_ladders(board: Vec<Vec<i32>>) -> i32 {
        let n = board.len();
        let mut vis = vec![false; n * n + 1];
        vis[1] = true; // 题目保证起点没有蛇梯，不写也可以
        let mut q = vec![1]; // 起点
        for step in 0.. {
            if q.is_empty() {
                break;
            }
            let tmp = q;
            q = vec![];
            for x in tmp {
                if x == n * n { // 终点
                    return step;
                }
                for y in x + 1..=(x + 6).min(n * n) {
                    let mut r = (y - 1) / n;
                    let mut c = (y - 1) % n;
                    if r % 2 > 0 {
                        c = n - 1 - c; // 奇数行从右到左
                    }
                    let mut nxt = board[n - 1 - r][c];
                    if nxt < 0 {
                        nxt = y as i32;
                    }
                    let nxt = nxt as usize;
                    if !vis[nxt] {
                        vis[nxt] = true; // 有环的情况下，避免死循环
                        q.push(nxt);
                    }
                }
            }
        }
        -1 // 无法到达终点
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(Dn^2)$，其中 $n$ 是 $\textit{board}$ 的行数和列数，$D=6$ 是骰子的面数。
- 空间复杂度：$\mathcal{O}(n^2)$。

更多相似题目，见下面网格图题单的 BFS 和图论题单的 BFS。

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
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
