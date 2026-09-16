# 3243. 新增道路查询后的最短距离 I

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/shortest-distance-after-road-addition-queries-i/
- 题目 slug：`shortest-distance-after-road-addition-queries-i`
- 来源专题：图论算法
- 来源分类路径：一、图的遍历 / §1.2 广度优先搜索（BFS）
- 难度分：1568
- 外部题解来源：https://leetcode.cn/problems/shortest-distance-after-road-addition-queries-i/solutions/2869215/liang-chong-fang-fa-bfs-dppythonjavacgo-mgunf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：BFS / DP（Python/Java/C++/Go）](https://leetcode.cn/problems/shortest-distance-after-road-addition-queries-i/solutions/2869215/liang-chong-fang-fa-bfs-dppythonjavacgo-mgunf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-bfs-dppythonjavacgo-mgunf`
- topic id：`2869215`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：BFS

暴力。每次加边后，重新跑一遍 BFS，求出从 $0$ 到 $n-1$ 的最短路。

代码采用双数组实现 BFS，原理请看[【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)。

### 细节

为避免反复创建 $\textit{vis}$ 数组，可以在 $\textit{vis}$ 中保存当前节点是第几次询问访问的。

```py [sol-Python3]
class Solution:
    def shortestDistanceAfterQueries(self, n: int, queries: List[List[int]]) -> List[int]:
        g = [[i + 1] for i in range(n - 1)]
        vis = [-1] * (n - 1)

        def bfs(i: int) -> int:
            q = [0]
            for step in count(1):
                tmp = q
                q = []
                for x in tmp:
                    for y in g[x]:
                        if y == n - 1:
                            return step
                        if vis[y] != i:
                            vis[y] = i
                            q.append(y)
            return -1

        ans = [0] * len(queries)
        for i, (l, r) in enumerate(queries):
            g[l].append(r)
            ans[i] = bfs(i)
        return ans
```

```java [sol-Java]
class Solution {
    public int[] shortestDistanceAfterQueries(int n, int[][] queries) {
        List<Integer>[] g = new ArrayList[n - 1];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int i = 0; i < n - 1; i++) {
            g[i].add(i + 1);
        }

        int[] ans = new int[queries.length];
        int[] vis = new int[n - 1];
        for (int i = 0; i < queries.length; i++) {
            g[queries[i][0]].add(queries[i][1]);
            ans[i] = bfs(i + 1, g, vis, n);
        }
        return ans;
    }

    private int bfs(int i, List<Integer>[] g, int[] vis, int n) {
        List<Integer> q = new ArrayList<>();
        q.add(0);
        for (int step = 1; ; step++) {
            List<Integer> tmp = q;
            q = new ArrayList<>();
            for (int x : tmp) {
                for (int y : g[x]) {
                    if (y == n - 1) {
                        return step;
                    }
                    if (vis[y] != i) {
                        vis[y] = i;
                        q.add(y);
                    }
                }
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> shortestDistanceAfterQueries(int n, vector<vector<int>>& queries) {
        vector<vector<int>> g(n - 1);
        for (int i = 0; i < n - 1; i++) {
            g[i].push_back(i + 1);
        }
        vector<int> vis(n - 1, -1);

        auto bfs = [&](int i) -> int {
            vector<int> q = {0};
            for (int step = 1; ; step++) {
                vector<int> nxt;
                for (int x : q) {
                    for (int y : g[x]) {
                        if (y == n - 1) {
                            return step;
                        }
                        if (vis[y] != i) {
                            vis[y] = i;
                            nxt.push_back(y);
                        }
                    }
                }
                q = move(nxt);
            }
        };

        vector<int> ans(queries.size());
        for (int i = 0; i < queries.size(); i++) {
            g[queries[i][0]].push_back(queries[i][1]);
            ans[i] = bfs(i);
        }
        return ans;
    }
};
```

```go [sol-Go]
func shortestDistanceAfterQueries(n int, queries [][]int) []int {
    g := make([][]int, n-1)
    for i := range g {
        g[i] = append(g[i], i+1)
    }

    vis := make([]int, n-1)
    bfs := func(i int) int {
        q := []int{0}
        for step := 1; ; step++ {
            tmp := q
            q = nil
            for _, x := range tmp {
                for _, y := range g[x] {
                    if y == n-1 {
                        return step
                    }
                    if vis[y] != i {
                        vis[y] = i
                        q = append(q, y)
                    }
                }
            }
        }
    }

    ans := make([]int, len(queries))
    for i, q := range queries {
        g[q[0]] = append(g[q[0]], q[1])
        ans[i] = bfs(i + 1)
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(q(n+q))$，其中 $q$ 是 $\textit{queries}$ 的长度。每次 BFS 的时间是 $\mathcal{O}(n+q)$。
- 空间复杂度：$\mathcal{O}(n+q)$。

## 方法二：DP

定义 $f[i]$ 为从 $0$ 到 $i$ 的最短路。

用 $\textit{from}[i]$ 记录额外添加的边的终点是 $i$，起点列表是 $\textit{from}[i]$。

我们可以从 $i-1$ 到 $i$，也可以从 $\textit{from}[i][j]$ 到 $i$，这些位置作为转移来源，用其 $f$ 值 $+1$ 更新 $f[i]$ 的最小值。

初始值：$f[i] = i$。

答案：$f[n-1]$。

### 细节

设添加的边为 $l\to r$，只有当 $f[l]+1 < f[r]$ 时才更新 DP。

```py [sol-Python3]
class Solution:
    def shortestDistanceAfterQueries(self, n: int, queries: List[List[int]]) -> List[int]:
        frm = [[] for _ in range(n)]
        f = list(range(n))
        ans = []
        for l, r in queries:
            frm[r].append(l)
            if f[l] + 1 < f[r]:
                f[r] = f[l] + 1
                for i in range(r + 1, n):
                    f[i] = min(f[i], f[i - 1] + 1, min((f[j] for j in frm[i]), default=inf) + 1)
            ans.append(f[-1])
        return ans
```

```java [sol-Java]
class Solution {
    public int[] shortestDistanceAfterQueries(int n, int[][] queries) {
        List<Integer>[] from = new ArrayList[n];
        Arrays.setAll(from, i -> new ArrayList<>());
        int[] f = new int[n];
        for (int i = 1; i < n; i++) {
            f[i] = i;
        }

        int[] ans = new int[queries.length];
        for (int qi = 0; qi < queries.length; qi++) {
            int l = queries[qi][0];
            int r = queries[qi][1];
            from[r].add(l);
            if (f[l] + 1 < f[r]) {
                f[r] = f[l] + 1;
                for (int i = r + 1; i < n; i++) {
                    f[i] = Math.min(f[i], f[i - 1] + 1);
                    for (int j : from[i]) {
                        f[i] = Math.min(f[i], f[j] + 1);
                    }
                }
            }
            ans[qi] = f[n - 1];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> shortestDistanceAfterQueries(int n, vector<vector<int>>& queries) {
        vector<vector<int>> from(n);
        vector<int> f(n);
        iota(f.begin(), f.end(), 0);

        vector<int> ans(queries.size());
        for (int qi = 0; qi < queries.size(); qi++) {
            int l = queries[qi][0], r = queries[qi][1];
            from[r].push_back(l);
            if (f[l] + 1 < f[r]) {
                f[r] = f[l] + 1;
                for (int i = r + 1; i < n; i++) {
                    f[i] = min(f[i], f[i - 1] + 1);
                    for (int j : from[i]) {
                        f[i] = min(f[i], f[j] + 1);
                    }
                }
            }
            ans[qi] = f[n - 1];
        }
        return ans;
    }
};
```

```go [sol-Go]
func shortestDistanceAfterQueries(n int, queries [][]int) []int {
    from := make([][]int, n)
    f := make([]int, n)
    for i := 1; i < n; i++ {
        f[i] = i
    }

    ans := make([]int, len(queries))
    for qi, q := range queries {
        l, r := q[0], q[1]
        from[r] = append(from[r], l)
        if f[l]+1 < f[r] {
            f[r] = f[l] + 1
            for i := r + 1; i < n; i++ {
                f[i] = min(f[i], f[i-1]+1)
                for _, j := range from[i] {
                    f[i] = min(f[i], f[j]+1)
                }
            }
        }
        ans[qi] = f[n-1]
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(q(n+q))$，其中 $q$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n+q)$。

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

本题来自 `一、图的遍历 / §1.2 广度优先搜索（BFS）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、图的遍历 / §1.2 广度优先搜索（BFS）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
