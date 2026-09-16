# 2920. 收集所有金币可获得的最大积分

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-points-after-collecting-coins-from-all-nodes/
- 题目 slug：`maximum-points-after-collecting-coins-from-all-nodes`
- 来源专题：动态规划
- 来源分类路径：十二、树形 DP / §12.5 其他树形 DP
- 难度分：2351
- 外部题解来源：https://leetcode.cn/problems/maximum-points-after-collecting-coins-from-all-nodes/solutions/2503152/shu-xing-dp-ji-yi-hua-sou-suo-by-endless-phzx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[树形 DP：记忆化搜索 / 递推（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-points-after-collecting-coins-from-all-nodes/solutions/2503152/shu-xing-dp-ji-yi-hua-sou-suo-by-endless-phzx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-xing-dp-ji-yi-hua-sou-suo-by-endless-phzx`
- topic id：`2503152`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：记忆化搜索

`floor(coins[i] / 2)` 等价于 `coins[i] >> 1`。

右移运算是可以叠加的，即 `(x >> 1) >> 1` 等于 `x >> 2`。

我们可以在递归的过程中，额外记录从根节点递归到当前节点的过程中，一共执行了多少次右移，也就是子树中的每个节点值需要右移的次数。

故定义 $\textit{dfs}(i,j)$ 表示递归到以 $i$ 为根的子树，在上面已经执行了 $j$ 次右移的前提下，我们在这棵子树中最多可以得到多少积分。

用「选或不选」来思考，即是否执行右移：

- 不右移：答案为 $(\textit{coins}[i]\ \texttt{>>}\ j)-k$ 加上 $i$ 的每个子树 $\textit{ch}$ 的 $\textit{dfs}(\textit{ch},j)$。
- 右移：答案为 $\textit{coins}[i]\ \texttt{>>}\ (j+1)$ 加上 $i$ 的每个子树 $\textit{ch}$ 的 $\textit{dfs}(\textit{ch},j+1)$。

两种情况取最大值，得

$$
\textit{dfs}(i,j) = \max
\begin{cases}
(\textit{coins}[i]\ \texttt{>>}\ j)-k + \sum_{\textit{ch}} \textit{dfs}(\textit{ch},j)    \\
(\textit{coins}[i]\ \texttt{>>}\ (j+1)) +  \sum_{\textit{ch}} \textit{dfs}(\textit{ch},j+1)    \\
\end{cases}
$$

递归入口：$\textit{dfs}(0,0)$。其中 $i=0$ 表示根节点。一开始没有执行右移，所以 $j=0$。

### 细节

一个数最多右移多少次，就变成 $0$ 了？

设 $w$ 是 $\textit{coins}[i]$ 的二进制长度，那么 $\textit{coins}[i]$ 右移 $w$ 次后就是 $0$ 了。

在本题的数据范围下，$w\le 14$。

所以如果在递归过程中发现 $j+1 = 14$，就不执行右移，因为此时 $\textit{dfs}(\textit{ch},j+1)$ 子树中的每个节点值都要右移 $14$ 次，算出的结果一定是 $0$。既然都知道递归的结果了，那就不需要递归了。

此外，为避免错把父亲当作儿子，可以额外传入 $\textit{fa}$ 表示父节点，遍历 $i$ 的邻居时，跳过邻居节点是 $\textit{fa}$ 的情况。

关于记忆化搜索的原理，请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

[本题视频讲解](https://www.bilibili.com/video/BV1tw411q7VZ/?t=45m18s)

```py [sol-Python3]
class Solution:
    def maximumPoints(self, edges: List[List[int]], coins: List[int], k: int) -> int:
        g = [[] for _ in coins]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int, j: int, fa: int) -> int:
            res1 = (coins[i] >> j) - k
            res2 = coins[i] >> (j + 1)
            for ch in g[i]:
                if ch != fa:
                    res1 += dfs(ch, j, i)  # 不右移
                    if j < 13:  # j+1 >= 14 相当于 res2 += 0，无需递归
                        res2 += dfs(ch, j + 1, i)  # 右移
            return max(res1, res2)

        return dfs(0, 0, -1)
```

```java [sol-Java]
class Solution {
    public int maximumPoints(int[][] edges, int[] coins, int k) {
        int n = coins.length;
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            g[x].add(y);
            g[y].add(x);
        }

        int[][] memo = new int[n][14];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return dfs(0, 0, -1, memo, g, coins, k);
    }

    private int dfs(int i, int j, int fa, int[][] memo, List<Integer>[] g, int[] coins, int k) {
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }
        int res1 = (coins[i] >> j) - k;
        int res2 = coins[i] >> (j + 1);
        for (int ch : g[i]) {
            if (ch == fa) continue;
            res1 += dfs(ch, j, i, memo, g, coins, k); // 不右移
            if (j < 13) { // j+1 >= 14 相当于 res2 += 0，无需递归
                res2 += dfs(ch, j + 1, i, memo, g, coins, k); // 右移
            }
        }
        return memo[i][j] = Math.max(res1, res2); // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumPoints(vector<vector<int>>& edges, vector<int>& coins, int k) {
        int n = coins.size();
        vector<vector<int>> g(n);
        for (auto& e: edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        array<int, 14> init_val;
        ranges::fill(init_val, -1); // -1 表示没有计算过
        vector memo(n, init_val);
        auto dfs = [&](this auto&& dfs, int i, int j, int fa) {
            int& res = memo[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            int res1 = (coins[i] >> j) - k;
            int res2 = coins[i] >> (j + 1);
            for (int ch : g[i]) {
                if (ch == fa) continue;
                res1 += dfs(ch, j, i); // 不右移
                if (j < 13) { // j+1 >= 14 相当于 res2 += 0，无需递归
                    res2 += dfs(ch, j + 1, i); // 右移
                }
            }
            return res = max(res1, res2); // 记忆化
        };
        return dfs(0, 0, -1);
    }
};
```

```go [sol-Go]
func maximumPoints(edges [][]int, coins []int, k int) int {
    n := len(coins)
    g := make([][]int, n)
    for _, e := range edges {
        x, y := e[0], e[1]
        g[x] = append(g[x], y)
        g[y] = append(g[y], x)
    }

    memo := make([][14]int, n)
    for i := range memo {
        for j := range memo[i] {
            memo[i][j] = -1
        }
    }
    var dfs func(int, int, int) int
    dfs = func(i, j, fa int) (res int) {
        p := &memo[i][j]
        if *p != -1 {
            return *p
        }
        defer func() { *p = res }()
        res1 := coins[i]>>j - k
        res2 := coins[i] >> (j + 1)
        for _, ch := range g[i] {
            if ch != fa {
                res1 += dfs(ch, j, i) // 不右移
                if j < 13 { // j+1 >= 14 相当于 res2 += 0 无需递归
                    res2 += dfs(ch, j+1, i) // 右移
                }
            }
        }
        return max(res1, res2)
    }
    return dfs(0, 0, -1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{coins}$ 的长度，$U=\max(\textit{coins})$。
- 空间复杂度：$\mathcal{O}(n\log U)$。

## 方法二：递推

类似把记忆化搜索 1:1 翻译成递推的过程，我们也可以从下往上算。

去掉参数 $j$，改成每个节点 $i$ 返回一个长为 $14$ 的列表 $f_i$，其中 $f_i[j]$ 对应上面 $\textit{dfs}(i,j)$ 的计算结果。

递推式为

$$
f_i[j] = \max
\begin{cases}
(\textit{coins}[i]\ \texttt{>>}\ j)-k + \sum_{\textit{ch}} f_{\textit{ch}}[j]   \\
(\textit{coins}[i]\ \texttt{>>}\ (j+1)) +  \sum_{\textit{ch}} f_{\textit{ch}}[j+1]    \\
\end{cases}
$$

把 $\sum_{\textit{ch}} f_{\textit{ch}}[j]$ 累加到 $s[j]$ 中，上式为

$$
f_i[j] = \max
\begin{cases}
(\textit{coins}[i]\ \texttt{>>}\ j)-k + s[j]   \\
(\textit{coins}[i]\ \texttt{>>}\ (j+1)) +  s[j+1]    \\
\end{cases}
$$

特判 $j=13$ 的情况，上式为

$$
f_i[13] = \max((\textit{coins}[i]\ \texttt{>>}\ 13)-k + s[13],0)
$$

代码实现时，可以直接把算出的结果原地保存到 $s$ 数组中。

```py [sol-Python3]
class Solution:
    def maximumPoints(self, edges: List[List[int]], coins: List[int], k: int) -> int:
        g = [[] for _ in coins]
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        def dfs(x: int, fa: int) -> List[int]:
            s = [0] * 14
            for y in g[x]:
                if y != fa:
                    fy = dfs(y, x)
                    for j, v in enumerate(fy):
                        s[j] += v
            for j in range(13):
                s[j] = max((coins[x] >> j) - k + s[j], (coins[x] >> (j + 1)) + s[j + 1])
            s[13] = max(s[13] + (coins[x] >> 13) - k, 0)
            return s

        return dfs(0, -1)[0]
```

```java [sol-Java]
class Solution {
    public int maximumPoints(int[][] edges, int[] coins, int k) {
        List<Integer>[] g = new ArrayList[coins.length];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            g[x].add(y);
            g[y].add(x);
        }
        return dfs(0, -1, g, coins, k)[0];
    }

    private int[] dfs(int x, int fa, List<Integer>[] g, int[] coins, int k) {
        int[] s = new int[14];
        for (int y : g[x]) {
            if (y == fa) continue;
            int[] fy = dfs(y, x, g, coins, k);
            for (int j = 0; j < 14; j++) {
                s[j] += fy[j];
            }
        }
        for (int j = 0; j < 13; j++) {
            s[j] = Math.max((coins[x] >> j) - k + s[j], (coins[x] >> (j + 1)) + s[j + 1]);
        }
        s[13] = Math.max(s[13] + (coins[x] >> 13) - k, 0);
        return s;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumPoints(vector<vector<int>>& edges, vector<int>& coins, int k) {
        vector<vector<int>> g(coins.size());
        for (auto& e : edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        auto dfs = [&](this auto&& dfs, int x, int fa) -> array<int, 14> {
            array<int, 14> s{};
            for (int y : g[x]) {
                if (y == fa) continue;
                auto fy = dfs(y, x);
                for (int j = 0; j < 14; j++) {
                    s[j] += fy[j];
                }
            }
            for (int j = 0; j < 13; j++) {
                s[j] = max((coins[x] >> j) - k + s[j], (coins[x] >> (j + 1)) + s[j + 1]);
            }
            s[13] = max(s[13] + (coins[x] >> 13) - k, 0);
            return s;
        };
        return dfs(0, -1)[0];
    }
};
```

```go [sol-Go]
func maximumPoints(edges [][]int, coins []int, k int) int {
    n := len(coins)
    g := make([][]int, n)
    for _, e := range edges {
        x, y := e[0], e[1]
        g[x] = append(g[x], y)
        g[y] = append(g[y], x)
    }

    var dfs func(int, int) [14]int
    dfs = func(x, fa int) (s [14]int) {
        for _, y := range g[x] {
            if y != fa {
                fy := dfs(y, x)
                for j, v := range fy {
                    s[j] += v
                }
            }
        }
        for j := range 13 {
            s[j] = max(coins[x]>>j-k+s[j], coins[x]>>(j+1)+s[j+1])
        }
        s[13] = max(s[13]+coins[x]>>13-k, 0)
        return
    }
    return dfs(0, -1)[0]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{coins}$ 的长度，$U=\max(\textit{coins})$。
- 空间复杂度：$\mathcal{O}(n\log U)$。

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

本题来自 `十二、树形 DP / §12.5 其他树形 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十二、树形 DP / §12.5 其他树形 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
