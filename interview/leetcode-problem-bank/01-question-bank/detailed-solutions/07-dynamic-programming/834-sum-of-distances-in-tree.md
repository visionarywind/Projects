# 834. 树中距离之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-distances-in-tree/
- 题目 slug：`sum-of-distances-in-tree`
- 来源专题：动态规划
- 来源分类路径：十二、树形 DP / §12.4 换根 DP
- 难度分：2197
- 外部题解来源：https://leetcode.cn/problems/sum-of-distances-in-tree/solutions/2345592/tu-jie-yi-zhang-tu-miao-dong-huan-gen-dp-6bgb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】一张图秒懂换根 DP！（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/sum-of-distances-in-tree/solutions/2345592/tu-jie-yi-zhang-tu-miao-dong-huan-gen-dp-6bgb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-yi-zhang-tu-miao-dong-huan-gen-dp-6bgb`
- topic id：`2345592`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

暴力做法是，以点 $i$ 为树根，从 $i$ 出发 DFS 这棵树，那么 $i$ 到 $j$ 的距离就是 $j$ 在这棵树的深度。所有点的深度之和就是 $\textit{answer}[i]$（简称为 $\textit{ans}[i]$）。

但这样做，DFS 一次的时间是 $\mathcal{O}(n)$，$n$ 个点各 DFS 一次，总时间就是 $\mathcal{O}(n^2)$，会超时。如何优化呢？

![lc834.png](https://pic.leetcode.cn/1689398667-omjvbD-lc834.png)

### 答疑

**问**：子树大小是怎么算的？

**答**：先说二叉树，子树 $x$ 的大小等于左子树的大小，加上右子树的大小，再加上 $1$（节点 $x$ 本身），那么后序遍历这棵树，就可以算出每棵子树的大小。不清楚该过程的同学，需要学习「递归」「子问题」等概念，具体请看[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。然后推广到一般树，子树 $x$ 的大小，等于 $x$ 的所有儿子的子树大小之和，再加上 $1$（节点 $x$ 本身）。

**问**：在 DFS 中，如何保证每个节点只递归访问一次？

**答**：通用做法是用一个 $\textit{vis}$ 数组标记访问过的点，如果某个点之前访问过，就不再递归访问。但对于树来说，一直向下递归，并不会遇到之前访问过的点，所以不需要 $\textit{vis}$ 数组。本题是无向树，除了根节点以外，其余每个点的邻居都包含其父节点，所以要避免访问父节点。我们可以定义 $\textit{dfs}(x,\textit{fa})$ 表示递归到节点 $x$ 且 $x$ 的父节点为 $\textit{fa}$。只要 $x$ 的邻居 $y\ne \textit{fa}$，就可以 $\textit{dfs}(y,x)$ 向下递归了。

**问**：这种算法的**本质**是什么？

**答**：以图中的这棵树为例，从「以 $0$ 为根」换到「以 $2$ 为根」时，原来 $2$ 的子节点还是 $2$ 的子节点，原来 $1$ 的子节点还是 $1$ 的子节点，**唯一改变的是** $0$ **和** $2$ **的父子关系**。由此可见，一对节点的距离的「变化量」应该是很小的，那么找出「变化量」的规律，就可以基于 $\textit{ans}[0]$ 算出 $\textit{ans}[2]$ 了。这种算法叫做**换根 DP**。

```py [sol-Python3]
class Solution:
    def sumOfDistancesInTree(self, n: int, edges: List[List[int]]) -> List[int]:
        g = [[] for _ in range(n)]  # g[x] 表示 x 的所有邻居
        for x, y in edges:
            g[x].append(y)
            g[y].append(x)

        ans = [0] * n
        size = [1] * n  # 注意这里初始化成 1 了，下面只需要累加儿子的子树大小
        def dfs(x: int, fa: int, depth: int) -> None:
            ans[0] += depth  # depth 为 0 到 x 的距离
            for y in g[x]:  # 遍历 x 的邻居 y
                if y != fa:  # 避免访问父节点
                    dfs(y, x, depth + 1)  # x 是 y 的父节点
                    size[x] += size[y]  # 累加 x 的儿子 y 的子树大小
        dfs(0, -1, 0)  # 0 没有父节点

        def reroot(x: int, fa: int) -> None:
            for y in g[x]:  # 遍历 x 的邻居 y
                if y != fa:  # 避免访问父节点
                    ans[y] = ans[x] + n - 2 * size[y]
                    reroot(y, x)  # x 是 y 的父节点
        reroot(0, -1)  # 0 没有父节点
        return ans
```

```java [sol-Java]
class Solution {
    private List<Integer>[] g;
    private int[] ans;
    private int[] size;

    public int[] sumOfDistancesInTree(int n, int[][] edges) {
        g = new ArrayList[n]; // g[x] 表示 x 的所有邻居
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int[] e : edges) {
            int x = e[0];
            int y = e[1];
            g[x].add(y);
            g[y].add(x);
        }

        ans = new int[n];
        size = new int[n];
        dfs(0, -1, 0); // 0 没有父节点
        reroot(0, -1); // 0 没有父节点
        return ans;
    }

    private void dfs(int x, int fa, int depth) {
        ans[0] += depth; // depth 为 0 到 x 的距离
        size[x] = 1;
        for (int y : g[x]) { // 遍历 x 的邻居 y
            if (y != fa) { // 避免访问父节点
                dfs(y, x, depth + 1); // x 是 y 的父节点
                size[x] += size[y]; // 累加 x 的儿子 y 的子树大小
            }
        }
    }

    private void reroot(int x, int fa) {
        for (int y : g[x]) { // 遍历 x 的邻居 y
            if (y != fa) { // 避免访问父节点
                ans[y] = ans[x] + g.length - 2 * size[y];
                reroot(y, x); // x 是 y 的父节点
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> sumOfDistancesInTree(int n, vector<vector<int>>& edges) {
        vector<vector<int>> g(n); // g[x] 表示 x 的所有邻居
        for (auto& e: edges) {
            int x = e[0], y = e[1];
            g[x].push_back(y);
            g[y].push_back(x);
        }

        vector<int> ans(n);
        vector<int> size(n, 1); // 注意这里初始化成 1 了，下面只需要累加儿子的子树大小
        auto dfs = [&](auto&& dfs, int x, int fa, int depth) -> void {
            ans[0] += depth; // depth 为 0 到 x 的距离
            for (int y: g[x]) { // 遍历 x 的邻居 y
                if (y != fa) { // 避免访问父节点
                    dfs(dfs, y, x, depth + 1); // x 是 y 的父节点
                    size[x] += size[y]; // 累加 x 的儿子 y 的子树大小
                }
            }
        };
        dfs(dfs, 0, -1, 0); // 0 没有父节点

        auto reroot = [&](auto&& reroot, int x, int fa) -> void {
            for (int y: g[x]) { // 遍历 x 的邻居 y
                if (y != fa) { // 避免访问父节点
                    ans[y] = ans[x] + n - 2 * size[y];
                    reroot(reroot, y, x); // x 是 y 的父节点
                }
            }
        };
        reroot(reroot, 0, -1); // 0 没有父节点
        return ans;
    }
};
```

```go [sol-Go]
func sumOfDistancesInTree(n int, edges [][]int) []int {
    g := make([][]int, n) // g[x] 表示 x 的所有邻居
    for _, e := range edges {
        x, y := e[0], e[1]
        g[x] = append(g[x], y)
        g[y] = append(g[y], x)
    }

    ans := make([]int, n)
    size := make([]int, n)
    var dfs func(int, int, int)
    dfs = func(x, fa, depth int) {
        ans[0] += depth // depth 为 0 到 x 的距离
        size[x] = 1
        for _, y := range g[x] { // 遍历 x 的邻居 y
            if y != fa { // 避免访问父节点
                dfs(y, x, depth+1) // x 是 y 的父节点
                size[x] += size[y] // 累加 x 的儿子 y 的子树大小
            }
        }
    }
    dfs(0, -1, 0) // 0 没有父节点

    var reroot func(int, int)
    reroot = func(x, fa int) {
        for _, y := range g[x] { // 遍历 x 的邻居 y
            if y != fa { // 避免访问父节点
                ans[y] = ans[x] + n - 2*size[y]
                reroot(y, x) // x 是 y 的父节点
            }
        }
    }
    reroot(0, -1) // 0 没有父节点
    return ans
}
```

```js [sol-JavaScript]
var sumOfDistancesInTree = function(n, edges) {
    const g = Array.from({length: n}, () => []); // g[x] 表示 x 的所有邻居
    for (const [x, y] of edges) {
        g[x].push(y);
        g[y].push(x);
    }

    const ans = Array(n).fill(0);
    const size = Array(n).fill(1); // 注意这里初始化成 1 了，下面只需要累加儿子的子树大小
    function dfs(x, fa, depth) {
        ans[0] += depth; // depth 为 0 到 x 的距离
        for (const y of g[x]) { // 遍历 x 的邻居 y
            if (y !== fa) { // 避免访问父节点
                dfs(y, x, depth + 1); // x 是 y 的父节点
                size[x] += size[y]; // 累加 x 的儿子 y 的子树大小
            }
        }
    }
    dfs(0, -1, 0); // 0 没有父节点

    function reroot(x, fa) {
        for (const y of g[x]) { // 遍历 x 的邻居 y
            if (y !== fa) { // 避免访问父节点
                ans[y] = ans[x] + n - 2 * size[y];
                reroot(y, x); // x 是 y 的父节点
            }
        }
    }
    reroot(0, -1); // 0 没有父节点
    return ans;
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。DFS 两次，每次 DFS 会递归访问每个节点恰好一次，所以时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

1. 如果只需要算所有点对的距离之和，你能想出一个只需要一次 DFS 的算法吗？

   提示：我在前天的每日一题中讲解了一种技巧，请看[【图解】没有思路？进来秒懂！](https://leetcode.cn/problems/distribute-coins-in-binary-tree/solution/tu-jie-mei-you-si-lu-jin-lai-miao-dong-p-vrni/)

2. 把题目中的「距离之和」改成「距离的平方和」要怎么做？

   提示 1：换根，思考「变化量」是什么。

   提示 2：除了计算子树大小，还需要计算子树中的每个节点的深度之和。

3. 改成「距离的立方和」要怎么做？

欢迎在评论区发表你的思路/代码。

更多相似题目，见下面动态规划题单中的「**§12.4 换根 DP**」。

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

本题来自 `十二、树形 DP / §12.4 换根 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十二、树形 DP / §12.4 换根 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
