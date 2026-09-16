# 1824. 最少侧跳次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-sideway-jumps/
- 题目 slug：`minimum-sideway-jumps`
- 来源专题：动态规划
- 来源分类路径：二、网格图 DP / §2.1 基础
- 难度分：1778
- 外部题解来源：https://leetcode.cn/problems/minimum-sideway-jumps/solutions/2071617/cong-0-dao-1-de-0-1-bfspythonjavacgo-by-1m8z4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从 0 到 1 的 0-1 BFS（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-sideway-jumps/solutions/2071617/cong-0-dao-1-de-0-1-bfspythonjavacgo-by-1m8z4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cong-0-dao-1-de-0-1-bfspythonjavacgo-by-1m8z4`
- topic id：`2071617`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 转化

把 $3$ 条跑道看成是一个 $n+1$ 列 $3$ 行的网格图。设列号为 $i\in [0,n]$，行号为 $j\in [0,2]$：

- 如果 $(i,j)$ 和 $(i+1,j)$ 都没有障碍，则连一条边权为 $0$ 的边，表示跑道不变；
- 如果 $(i,j)$ 和 $(i,k)\ (k\ne j)$ 都没有障碍，则连一条边权为 $1$ 的边，表示侧跳。

原问题可以转换成在该网格图上，求 $(0,1)$ 到 $(n,j)$ 的最短路长度。

这个问题你可以直接用 Dijkstra 算法解决，但还有更快的做法。

## 线性！我要线性时间复杂度！

我在[【基础算法精讲 13】](https://www.bilibili.com/video/BV1hG4y1277i/)中讲解了树上的 BFS，简单解释了 BFS 为什么要用队列，以及为什么用队列能得到正确结果。这里额外补充两点：

1. 对于一棵边权均为 $1$ 的**树**来说，BFS 算法可以求出起点到其余点的最短路的长度，我们把它记录到一个 $\textit{dis}$ 数组中，$\textit{dis}[x]$ 表示起点到 $x$ 的最短路的长度。$x$ 出队时，对于一条 $x\rightarrow y$ 的边，更新 $\textit{dis}[y]=\textit{dis}[x]+1$，然后把 $y$ 入队。
2. 在任何情况下，队列中只会含有 $\textit{dis}$ 值为 $d$ 和 $d+1$ 的点，且 $\textit{dis}$ **值为 $d$ 的点一定排在值为 $d+1$ 的点前面**（这个叫 **BFS 的两段性**）。

如果你只会树上的 BFS（边权均为 $1$），那么还需要解决两个问题：

1. 怎么处理图上的 BFS（边权均为 $1$）。
2. 怎么处理图上还有边权为 $0$ 的情况。

对于一条 $x\rightarrow y$ 的边，如果 $\textit{dis}[y]\le\textit{dis}[x]+1$，那么无需更新 $\textit{dis}[y]$，也无需把 $y$ 入队了，这样就解决了上面的问题 1。

如果图上还有边权为 $0$ 的边，问题就变得复杂了，可能会出现下面的情况：

![1824.png](https://pic.leetcode.cn/1674230786-NNhevq-1824.png){:width=500}

## 总结

1. 对于边权为 $0$ 的边 $x\rightarrow y$，如果 $\textit{dis}[x]<dis[y]$，更新 $\textit{dis}[y]=\textit{dis}[x]$，把 $y$ 加到队首。
2. 对于边权为 $1$ 的边 $x\rightarrow y$，如果 $\textit{dis}[x]+1<dis[y]$，更新 $\textit{dis}[y]=\textit{dis}[x]+1$，把 $y$ 加到队尾。

这个算法叫 0-1 BFS。

> 注：如果边权不止 $0$ 和 $1$，把双端队列换成最小堆，就得到了 Dijkstra 算法。

代码实现时，只要我们到达了第 $n$ 列中的某一行，就可以直接返回答案，因为到第 $n$ 列另外两行的最短路不会比当前的小。

## 答疑

**问**：为什么青蛙一定能到达终点？

**答**：你想把青蛙堵住是吧？由于有 $3$ 条跑道，且每列至多有一个障碍，那么唯一的**堵住**方式就是示例 $1$ 所展示的，$3$ 个障碍斜着摆放。青蛙不屑地看了看你，侧着跳走了。

**问**：为什么代码中没有使用 $\textit{vis}$ 数组？

**答**：对于节点 $x$，它第一次出队后，会去更新 $x$ 的邻居的 $\textit{dis}$ 值。它第二次出队时，由于 $x$ 的邻居的 $\textit{dis}$ 值已经被更新过，所以此时 $x$ 必不能更新邻居的 $\textit{dis}$ 值，无法产生任何影响，所以 $\textit{vis}$ 数组是多余的。

> Dijkstra 算法也可以不要 $\textit{vis}$ 数组，在出堆时判断下，如果堆中存的 $\textit{dis}$ 超过了实际的 $\textit{dis}$ 值，可以直接 continue。

```py [sol-Python3]
class Solution:
    def minSideJumps(self, obstacles: List[int]) -> int:
        n = len(obstacles)
        dis = [[n] * 3 for _ in range(n)]
        dis[0][1] = 0
        q = deque([(0, 1)])  # 起点
        while True:
            i, j = q.popleft()
            d = dis[i][j]
            if i == n - 1: return d  # 到达终点
            if obstacles[i + 1] != j + 1 and d < dis[i + 1][j]:  # 向右
                dis[i + 1][j] = d
                q.appendleft((i + 1, j))  # 加到队首
            for k in (j + 1) % 3, (j + 2) % 3:  # 枚举另外两条跑道（向上/向下）
                if obstacles[i] != k + 1 and d + 1 < dis[i][k]:
                    dis[i][k] = d + 1
                    q.append((i, k))  # 加到队尾
```

```java [sol-Java]
class Solution {
    public int minSideJumps(int[] obstacles) {
        int n = obstacles.length;
        var dis = new int[n][3];
        for (int i = 0; i < n; ++i)
            Arrays.fill(dis[i], n);
        dis[0][1] = 0;
        var q = new ArrayDeque<int[]>();
        q.add(new int[]{0, 1}); // 起点
        for (;;) {
            var p = q.pollFirst();
            int i = p[0], j = p[1], d = dis[i][j];
            if (i == n - 1) return d; // 到达终点
            if (obstacles[i + 1] != j + 1 && d < dis[i + 1][j]) { // 向右
                dis[i + 1][j] = d;
                q.addFirst(new int[]{i + 1, j}); // 加到队首
            }
            for (int k : new int[]{(j + 1) % 3, (j + 2) % 3}) // 枚举另外两条跑道（向上/向下）
                if (obstacles[i] != k + 1 && d + 1 < dis[i][k]) {
                    dis[i][k] = d + 1;
                    q.addLast(new int[]{i, k}); // 加到队尾
                }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSideJumps(vector<int> &obstacles) {
        int n = obstacles.size(), dis[n][3];
        memset(dis, 0x3f, sizeof(dis));
        dis[0][1] = 0;
        deque<pair<int, int>> q;
        q.emplace_back(0, 1); // 起点
        for (;;) {
            auto[i, j] = q.front(); q.pop_front();
            int d = dis[i][j];
            if (i == n - 1) return d; // 到达终点
            if (obstacles[i + 1] != j + 1 && d < dis[i + 1][j]) { // 向右
                dis[i + 1][j] = d;
                q.emplace_front(i + 1, j); // 加到队首
            }
            for (int k : {(j + 1) % 3, (j + 2) % 3}) // 枚举另外两条跑道（向上/向下）
                if (obstacles[i] != k + 1 && d + 1 < dis[i][k]) {
                    dis[i][k] = d + 1;
                    q.emplace_back(i, k); // 加到队尾
                }
        }
    }
};
```

```go [sol-Go]
func minSideJumps(obstacles []int) int {
    n := len(obstacles)
    dis := make([][3]int, n)
    for i := range dis {
        for j := range dis[i] {
            dis[i][j] = n
        }
    }
    dis[0][1] = 0
    type pair struct{ i, j int }
    var ql, qr []pair // 两个 slice 头对头来实现 deque
    ql = append(ql, pair{0, 1}) // 起点
    for {
        var p pair
        if len(ql) > 0 {
            p, ql = ql[len(ql)-1], ql[:len(ql)-1]
        } else {
            p, qr = qr[0], qr[1:]
        }
        i, j := p.i, p.j
        d := dis[i][j]
        if i == n-1 { // 到达终点
            return d
        }
        if obstacles[i+1] != j+1 && d < dis[i+1][j] { // 向右
            dis[i+1][j] = d
            ql = append(ql, pair{i + 1, j}) // 加到队首
        }
        for _, k := range []int{(j + 1) % 3, (j + 2) % 3} { // 枚举另外两条跑道（向上/向下）
            if obstacles[i] != k+1 && d+1 < dis[i][k] {
                dis[i][k] = d + 1
                qr = append(qr, pair{i, k}) // 加到队尾
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{obstacles}$ 的长度。根据 BFS 的两段性，每个点至多入队两次，一次在队尾，另一次在队首。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

- [1368. 使网格图至少有一条有效路径的最小代价](https://leetcode.cn/problems/minimum-cost-to-make-at-least-one-valid-path-in-a-grid/) 2069
- [2290. 到达角落需要移除障碍物的最小数目](https://leetcode.cn/problems/minimum-obstacle-removal-to-reach-corner/) 2138

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

本题来自 `二、网格图 DP / §2.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、网格图 DP / §2.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
