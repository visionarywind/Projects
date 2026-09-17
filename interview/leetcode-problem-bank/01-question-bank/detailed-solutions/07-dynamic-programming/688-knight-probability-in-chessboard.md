# 688. 骑士在棋盘上的概率

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/knight-probability-in-chessboard/
- 题目 slug：`knight-probability-in-chessboard`
- 来源专题：动态规划
- 来源分类路径：十五、概率 DP、期望 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/knight-probability-in-chessboard/solutions/2997395/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-dgt6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推（Python/Java/C++/Go）](https://leetcode.cn/problems/knight-probability-in-chessboard/solutions/2997395/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-dgt6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-dgt6`
- topic id：`2997395`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 一、寻找子问题

在示例 1 中，我们要解决的问题（原问题）是：

- 马从 $(0,0)$ 出发，走 $k=2$ 步后仍然在棋盘上的概率。

枚举马走的八个方向，假设走到了 $(1,2)$，问题变成：

- 马从 $(1,2)$ 出发，走 $k-1=1$ 步后仍然在棋盘上的概率。

这是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

## 二、状态定义与状态转移方程

根据上面的讨论，我们需要在递归过程中跟踪以下信息：

- $k$：还剩下 $k$ 步要走。
- $(i,j)$：马的位置。

因此，定义状态为 $\textit{dfs}(k,i,j)$，表示马从 $(i,j)$ 出发，走 $k$ 步后仍然在棋盘上的概率。

枚举马走的八个方向，其中有 $\dfrac{1}{8}$ 概率走到了 $(x,y)$，问题变成：

- 马从 $(x,y)$ 出发，走 $k-1$ 步后仍然在棋盘上的概率，即 $\textit{dfs}(k-1,x,y)$。

八种情况累加，得

$$
\textit{dfs}(k,i,j) = \dfrac{1}{8}\sum_{(x,y)} \textit{dfs}(k-1,x,y)
$$

**递归边界**：如果马出界，那么在棋盘上的概率为 $0$，即 $\textit{dfs}(k,i,j)=0$。如果 $k=0$ 时马仍然在棋盘上，那么概率为 $1$，即 $\textit{dfs}(0,i,j)=1$。

**递归入口**：$\textit{dfs}(k,\textit{row},\textit{column})$，也就是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(k,i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。本题可以初始化成 $0$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
DIRS = (2, 1), (1, 2), (-1, 2), (-2, 1), (-2, -1), (-1, -2), (1, -2), (2, -1)

class Solution:
    def knightProbability(self, n: int, k: int, row: int, column: int) -> float:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(k: int, i: int, j: int) -> float:
            if not (0 <= i < n and 0 <= j < n):  # 出界
                return 0
            if k == 0:  # 走完了，仍然在棋盘上
                return 1
            return sum(dfs(k - 1, i + dx, j + dy) for dx, dy in DIRS) / 8
        return dfs(k, row, column)
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};

    public double knightProbability(int n, int k, int row, int column) {
        double[][][] memo = new double[k + 1][n][n];
        return dfs(k, row, column, n, memo);
    }

    private double dfs(int k, int i, int j, int n, double[][][] memo) {
        if (i < 0 || j < 0 || i >= n || j >= n) {
            return 0;
        }
        if (k == 0) {
            return 1;
        }
        if (memo[k][i][j] > 0) { // 之前计算过
            return memo[k][i][j];
        }
        double res = 0;
        for (int[] d : DIRS) {
            res += dfs(k - 1, i + d[0], j + d[1], n, memo);
        }
        return memo[k][i][j] = res / DIRS.length; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int DIRS[8][2] = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};
public:
    double knightProbability(int n, int k, int row, int column) {
        vector memo(k + 1, vector(n, vector<double>(n)));
        auto dfs = [&](this auto& dfs, int k, int i, int j) -> double {
            if (i < 0 || i >= n || j < 0 || j >= n) {
                return 0;
            }
            if (k == 0) {
                return 1;
            }
            double& res = memo[k][i][j]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }
            for (auto& [dx, dy] : DIRS) {
                res += dfs(k - 1, i + dx, j + dy);
            }
            res /= 8;
            return res;
        };
        return dfs(k, row, column);
    }
};
```

```go [sol-Go]
var dirs = []struct{ x, y int }{{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}}

func knightProbability(n, k, row, column int) float64 {
    memo := make([][][]float64, k+1)
    for i := range memo {
        memo[i] = make([][]float64, n)
        for j := range memo[i] {
            memo[i][j] = make([]float64, n)
        }
    }
    var dfs func(int, int, int) float64
    dfs = func(k, i, j int) float64 {
        if i < 0 || j < 0 || i >= n || j >= n {
            return 0
        }
        if k == 0 {
            return 1
        }
        p := &memo[k][i][j]
        if *p > 0 {
            return *p
        }
        res := 0.0
        for _, d := range dirs {
            res += dfs(k-1, i+d.x, j+d.y)
        }
        res /= 8
        *p = res
        return res
    }
    return dfs(k, row, column)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(kn^2)$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(kn^2)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(kn^2)$。
- 空间复杂度：$\mathcal{O}(kn^2)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

为避免下标出现负数，把棋盘的边界从 $[0,n-1]$ 调整为 $[2,n+1]$。

具体来说，$f[k][i][j]$ 的定义和 $\textit{dfs}(k,i,j)$ 的定义是一样的，都表示马从 $(i,j)$ 出发，走 $k$ 步后仍然在棋盘上的概率。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[k][i][j] = \dfrac{1}{8}\sum_{(x,y)} f[k-1][x][y]
$$

初始值：如果马出界，那么在棋盘上的概率为 $0$，即 $f[k][i][j]=0$。如果 $k=0$ 时马仍然在棋盘上，那么概率为 $1$，即 $f[0][i][j]=1$。

答案为 $f[k][\textit{row}+2][\textit{column}+2]$，翻译自递归入口 $\textit{dfs}(k,\textit{row},\textit{column})$。这里 $+2$ 是因为我们调整了棋盘的边界，所以起点也一同调整。

```py [sol-Python3]
DIRS = (2, 1), (1, 2), (-1, 2), (-2, 1), (-2, -1), (-1, -2), (1, -2), (2, -1)

class Solution:
    def knightProbability(self, n: int, k: int, row: int, column: int) -> float:
        f = [[[0] * (n + 4) for _ in range(n + 4)] for _ in range(k + 1)]
        for i in range(2, n + 2):
            f[0][i][2: n + 2] = [1] * n
        for step in range(1, k + 1):
            for i in range(2, n + 2):
                for j in range(2, n + 2):
                    f[step][i][j] = sum(f[step - 1][i + dx][j + dy] for dx, dy in DIRS) / 8
        return f[k][row + 2][column + 2]
```

```java [sol-Java]
class Solution {
    private static final int[][] DIRS = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};

    public double knightProbability(int n, int k, int row, int column) {
        double[][][] f = new double[k + 1][n + 4][n + 4];
        for (int i = 2; i < n + 2; i++) {
            Arrays.fill(f[0][i], 2, n + 2, 1);
        }
        for (int step = 1; step <= k; step++) {
            for (int i = 2; i < n + 2; i++) {
                for (int j = 2; j < n + 2; j++) {
                    for (int[] d : DIRS) {
                        f[step][i][j] += f[step - 1][i + d[0]][j + d[1]];
                    }
                    f[step][i][j] /= DIRS.length;
                }
            }
        }
        return f[k][row + 2][column + 2];
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int DIRS[8][2] = {{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}};
public:
    double knightProbability(int n, int k, int row, int column) {
        vector f(k + 1, vector(n + 4, vector<double>(n + 4)));
        for (int i = 2; i < n + 2; i++) {
            fill(f[0][i].begin() + 2, f[0][i].begin() + n + 2, 1);
        }
        for (int step = 1; step <= k; step++) {
            for (int i = 2; i < n + 2; i++) {
                for (int j = 2; j < n + 2; j++) {
                    for (auto& [dx, dy] : DIRS) {
                        f[step][i][j] += f[step - 1][i + dx][j + dy];
                    }
                    f[step][i][j] /= 8;
                }
            }
        }
        return f[k][row + 2][column + 2];
    }
};
```

```go [sol-Go]
var dirs = []struct{ x, y int }{{2, 1}, {1, 2}, {-1, 2}, {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}, {2, -1}}

func knightProbability(n, k, row, column int) float64 {
    f := make([][][]float64, k+1)
    for i := range f {
        f[i] = make([][]float64, n+4)
        for j := range f[i] {
            f[i][j] = make([]float64, n+4)
        }
    }
    for i := 2; i < n+2; i++ {
        for j := 2; j < n+2; j++ {
            f[0][i][j] = 1
        }
    }
    for steps := 1; steps <= k; steps++ {
        for i := 2; i < n+2; i++ {
            for j := 2; j < n+2; j++ {
                for _, d := range dirs {
                    f[steps][i][j] += f[steps-1][i+d.x][j+d.y]
                }
                f[steps][i][j] /= 8
            }
        }
    }
    return f[k][row+2][column+2]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(kn^2)$。
- 空间复杂度：$\mathcal{O}(kn^2)$。若用滚动数组，空间可以优化至 $\mathcal{O}(n^2)$。

更多相似题目，见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**§7.5 多维 DP**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. 【本题相关】[动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `十五、概率 DP、期望 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十五、概率 DP、期望 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
