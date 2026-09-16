# 1210. 穿过迷宫的最少移动次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-moves-to-reach-target-with-rotations/
- 题目 slug：`minimum-moves-to-reach-target-with-rotations`
- 来源专题：网格图
- 来源分类路径：网格图
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/minimum-moves-to-reach-target-with-rotations/solutions/2093126/huan-zai-if-elseyi-ge-xun-huan-chu-li-li-tw8b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[还在 if-else？一个循环处理六种移动！（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-moves-to-reach-target-with-rotations/solutions/2093126/huan-zai-if-elseyi-ge-xun-huan-chu-li-li-tw8b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`huan-zai-if-elseyi-ge-xun-huan-chu-li-li-tw8b`
- topic id：`2093126`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

### 小小贪吃蛇，移动花样多。
### 平移加旋转，为把迷宫破。
### 代码复代码，bug 何其多。
### 六步化一步，AC 定我夺。

本题相当于在网格图上求起点到终点的最短路长度，这可以用 BFS 解决。相比一般的网格图 BFS（例如 [1926 题](https://leetcode.cn/problems/nearest-exit-from-entrance-in-maze/)），本题多了个「水平／竖直状态」，这可以通过添加一个维度来解决，也就是用 $(x,y,s)$ 表示**蛇尾**在第 $x$ 行第 $y$ 列，$s=0$ 表示水平状态，$s=1$ 表示竖直状态。这样初始位置为 $(0,0,0)$，最终位置为 $(n-1,n-2,0)$。

一些同学在写代码时，会写出很多的 if-else，对这 $6$ 种移动方式分别编程：

- 水平状态：向下移动／向右移动／顺时针旋转 $90$ 度。
- 竖直状态：向下移动／向右移动／逆时针旋转 $90$ 度。

代码越长，就越容易出 bug。能否总结出这 $6$ 种移动方式的异同，用同一份代码解决呢？

无论是水平状态还是竖直状态：

- 向下移动：$x$ 增加 $1$，$y$ 和 $s$ 不变。用三元组 $(1,0,0)$ 表示。
- 向右移动：$y$ 增加 $1$，$x$ 和 $s$ 不变。用三元组 $(0,1,0)$ 表示。
- 旋转：$s$ **切换**，即 $0$ 变为 $1$，$1$ 变为 $0$；$x$ 和 $y$ 不变。用三元组 $(0,0,1)$ 表示。

三元组中的数字表示 $(x,y,s)$ 每个值对应的**变化量**。对于旋转可以用异或运算解决。

这样就能把 $6$ 种移动方式用 $3$ 个三元组表示了。把这 $3$ 个三元组存到数组 $\textit{dirs}$ 中，遍历 $\textit{dirs}$，**用同一份代码处理不同的移动**，这样就不用对每种移动各写一份代码了。

最后，还需要判断：

- 移动后蛇身不能出界。
- 移动后蛇身不能在障碍物上。
- 对于旋转，还需要保证 $(x+1,y+1)$ 没有障碍物。

蛇尾在 $(x,y)$，蛇头呢？如果 $s=0$，蛇头在 $(x,y+1)$；如果 $s=1$，蛇头在 $(x+1,y)$。也可以合并为一个公式表示蛇头：

$$
(x+s,y+(s\oplus 1))
$$

其中 $\oplus$ 表示异或运算。

```py [sol1-Python3]
class Solution:
    def minimumMoves(self, g: List[List[int]]) -> int:
        step, n = 1, len(g)
        vis = {(0, 0, 0)}
        q = [(0, 0, 0)]  # 初始位置
        while q:
            tmp = q
            q = []
            for X, Y, S in tmp:
                for t in (X + 1, Y, S), (X, Y + 1, S), (X, Y, S ^ 1):  # 直接把移动后的位置算出来
                    x, y, s = t
                    x2, y2 = x + s, y + (s ^ 1)  # 蛇头
                    if x2 < n and y2 < n and t not in vis and \
                       g[x][y] == 0 and g[x2][y2] == 0 and (s == S or g[x + 1][y + 1] == 0):
                        if x == n - 1 and y == n - 2:  # 此时蛇头一定在 (n-1,n-1)
                            return step
                        vis.add(t)
                        q.append(t)
            step += 1
        return -1
```

```java [sol1-Java]
class Solution {
    private static int[][] DIRS = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    public int minimumMoves(int[][] g) {
        int n = g.length;
        var vis = new boolean[n][n][2];
        var q = new ArrayList<int[]>();
        vis[0][0][0] = true;
        q.add(new int[]{0, 0, 0}); // 初始位置
        for (int step = 1; !q.isEmpty(); ++step) {
            var tmp = q;
            q = new ArrayList<>();
            for (var t : tmp) {
                for (var d : DIRS) {
                    int x = t[0] + d[0], y = t[1] + d[1], s = t[2] ^ d[2];
                    int x2 = x + s, y2 = y + (s ^ 1); // 蛇头
                    if (x2 < n && y2 < n && !vis[x][y][s] &&
                        g[x][y] == 0 && g[x2][y2] == 0 && (d[2] == 0 || g[x + 1][y + 1] == 0)) {
                        if (x == n - 1 && y == n - 2) return step; // 此时蛇头一定在 (n-1,n-1)
                        vis[x][y][s] = true;
                        q.add(new int[]{x, y, s});
                    }
                }
            }
        }
        return -1;
    }
}
```

```cpp [sol1-C++]
class Solution {
    static constexpr int DIRS[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
public:
    int minimumMoves(vector<vector<int>> &g) {
        int n = g.size();
        bool vis[n][n][2]; memset(vis, 0, sizeof(vis));
        vis[0][0][0] = true;
        vector<tuple<int, int, int>> q = {{0, 0, 0}}; // 初始位置
        for (int step = 1; !q.empty(); ++step) {
            vector<tuple<int, int, int>> nxt;
            for (const auto &[X, Y, S] : q) {
                for (const auto &d : DIRS) {
                    int x = X + d[0], y = Y + d[1], s = S ^ d[2];
                    int x2 = x + s, y2 = y + (s ^ 1); // 蛇头
                    if (x2 < n && y2 < n && !vis[x][y][s] &&
                        g[x][y] == 0 && g[x2][y2] == 0 && (d[2] == 0 || g[x + 1][y + 1] == 0)) {
                        if (x == n - 1 && y == n - 2) return step; // 此时蛇头一定在 (n-1,n-1)
                        vis[x][y][s] = true;
                        nxt.emplace_back(x, y, s);
                    }
                }
            }
            q = move(nxt);
        }
        return -1;
    }
};
```

```go [sol1-Go]
type tuple struct{ x, y, s int }
var dirs = []tuple{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}

func minimumMoves(g [][]int) int {
    n := len(g)
    vis := make([][][2]bool, n)
    for i := range vis {
        vis[i] = make([][2]bool, n)
    }
    vis[0][0][0] = true // 初始位置
    q := []tuple{{}}
    for step := 1; len(q) > 0; step++ {
        tmp := q
        q = nil
        for _, t := range tmp {
            for _, d := range dirs {
                x, y, s := t.x+d.x, t.y+d.y, t.s^d.s
                x2, y2 := x+s, y+(s^1) // 蛇头
                if x2 < n && y2 < n && !vis[x][y][s] &&
                    g[x][y] == 0 && g[x2][y2] == 0 && (d.s == 0 || g[x+1][y+1] == 0) {
                    if x == n-1 && y == n-2 { // 此时蛇头一定在 (n-1,n-1)
                        return step
                    }
                    vis[x][y][s] = true
                    q = append(q, tuple{x, y, s})
                }
            }
        }
    }
    return -1
}
```

### 复杂度分析

- 时间复杂度：$O(n^2)$，其中 $n$ 为 $\textit{grid}$ 的长度。$\textit{vis}$ 保证每个位置至多访问一次。
- 空间复杂度：$O(n^2)$。

### 思考题

如果蛇身还可以朝上／朝左，还可以向上／向右移动，要如何修改呢？

欢迎在评论区发表你的做法。

## 本地原创解析

### 1. 题意重述

本题来自 `网格图`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `网格图`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
