# 1376. 通知所有员工所需的时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/time-needed-to-inform-all-employees/
- 题目 slug：`time-needed-to-inform-all-employees`
- 来源专题：链表、树与回溯
- 来源分类路径：三、一般树 / §3.2 自顶向下 DFS
- 难度分：1561
- 外部题解来源：https://leetcode.cn/problems/time-needed-to-inform-all-employees/solutions/2251986/shen-ru-li-jie-di-gui-zi-ding-xiang-xia-ps0mm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[深入理解递归：自顶向下/自底向上/记忆化搜索（附 O(1) 空间迭代写法！）](https://leetcode.cn/problems/time-needed-to-inform-all-employees/solutions/2251986/shen-ru-li-jie-di-gui-zi-ding-xiang-xia-ps0mm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shen-ru-li-jie-di-gui-zi-ding-xiang-xia-ps0mm`
- topic id：`2251986`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:49:15 +0800

## 方法一：一般树递归

本质是 [104. 二叉树的最大深度](https://leetcode.cn/problems/maximum-depth-of-binary-tree/)，把递归中的 $1$ 替换成 $\textit{informTime}[x]$。

关于 104 题的做法，可以看[【基础算法精讲 09】](https://www.bilibili.com/video/BV1UD4y1Y769/)。

相比二叉树，本题是一般树，需要先通过 $\textit{manager}$ 数组把每个点的儿子预处理出来，存储在 $g$ 数组中。然后在递归中遍历当前节点的儿子，向下递归。

和视频中讲的一样，有如下两种写法。

### 写法一：自底向上

```py [sol-Python3]
class Solution:
    def numOfMinutes(self, n: int, headID: int, manager: List[int], informTime: List[int]) -> int:
        g = [[] for _ in range(n)]
        for i, m in enumerate(manager):
            if m >= 0:
                g[m].append(i)  # 建树
        def dfs(x: int) -> int:
            max_path_sum = 0
            for y in g[x]:  # 遍历 x 的儿子 y（如果没有儿子就不会进入循环）
                max_path_sum = max(max_path_sum, dfs(y))
            # 这和 104 题代码中的 max(l_depth, r_depth) + 1 是一个意思
            return max_path_sum + informTime[x]
        return dfs(headID)  # 从根节点 headID 开始递归
```

```java [sol-Java]
class Solution {
    public int numOfMinutes(int n, int headID, int[] manager, int[] informTime) {
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int i = 0; i < n; i++) {
            if (manager[i] >= 0) {
                g[manager[i]].add(i); // 建树
            }
        }
        return dfs(g, informTime, headID); // 从根节点 headID 开始递归
    }

    private int dfs(List<Integer>[] g, int[] informTime, int x) {
        int maxPathSum = 0;
        for (int y : g[x]) { // 遍历 x 的儿子 y（如果没有儿子就不会进入循环）
            maxPathSum = Math.max(maxPathSum, dfs(g, informTime, y));
        }
        // 这和 104 题代码中的 return max(lDepth, rDepth) + 1; 是一个意思
        return maxPathSum + informTime[x];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numOfMinutes(int n, int headID, vector<int>& manager, vector<int>& informTime) {
        vector<vector<int>> g(n);
        for (int i = 0; i < n; i++) {
            if (manager[i] >= 0) {
                g[manager[i]].push_back(i); // 建树
            }
        }
        auto dfs = [&](this auto&& dfs, int x) -> int {
            int maxPathSum = 0;
            for (int y : g[x]) { // 遍历 x 的儿子 y（如果没有儿子就不会进入循环）
                maxPathSum = max(maxPathSum, dfs(y));
            }
            return maxPathSum + informTime[x];
        };
        return dfs(headID); // 从根节点 headID 开始递归
    }
};
```

```go [sol-Go]
func numOfMinutes(n, headID int, manager, informTime []int) (ans int) {
    g := make([][]int, n)
    for i, m := range manager {
        if m >= 0 {
            g[m] = append(g[m], i) // 建树
        }
    }
    var dfs func(int) int
    dfs = func(x int) (maxPathSum int) {
        for _, y := range g[x] { // 遍历 x 的儿子 y（如果没有儿子就不会进入循环）
            maxPathSum = max(maxPathSum, dfs(y))
        }
        // 这和 104 题代码中的 return max(lDepth, rDepth) + 1 是一个意思
        return maxPathSum + informTime[x]
    }
    return dfs(headID) // 从根节点 headID 开始递归
}
```

### 写法二：自顶向下

```py [sol-Python3]
class Solution:
    def numOfMinutes(self, n: int, headID: int, manager: List[int], informTime: List[int]) -> int:
        g = [[] for _ in range(n)]
        for i, m in enumerate(manager):
            if m >= 0:
                g[m].append(i)  # 建树
        ans = 0
        def dfs(x: int, path_sum: int) -> None:
            path_sum += informTime[x]  # 累加递归路径上的 informTime[x]
            nonlocal ans
            ans = max(ans, path_sum)  # 更新答案的最大值
            for y in g[x]:  # 遍历 x 的儿子 y（如果没有儿子就不会进入循环）
                dfs(y, path_sum)  # 继续递归
        dfs(headID, 0)  # 从根节点 headID 开始递归
        return ans
```

```java [sol-Java]
class Solution {
    private int ans;

    public int numOfMinutes(int n, int headID, int[] manager, int[] informTime) {
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int i = 0; i < n; i++) {
            if (manager[i] >= 0) {
                g[manager[i]].add(i); // 建树
            }
        }
        dfs(g, informTime, headID, 0); // 从根节点 headID 开始递归
        return ans;
    }

    private void dfs(List<Integer>[] g, int[] informTime, int x, int pathSum) {
        pathSum += informTime[x]; // 累加递归路径上的 informTime[x]
        ans = Math.max(ans, pathSum); // 更新答案的最大值
        for (int y : g[x]) { // 遍历 x 的儿子 y（如果没有儿子就不会进入循环）
            dfs(g, informTime, y, pathSum); // 继续递归
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numOfMinutes(int n, int headID, vector<int>& manager, vector<int>& informTime) {
        vector<vector<int>> g(n);
        for (int i = 0; i < n; i++) {
            if (manager[i] >= 0) {
                g[manager[i]].push_back(i); // 建树
            }
        }
        int ans = 0;
        auto dfs = [&](this auto&& dfs, int x, int pathSum) -> void {
            pathSum += informTime[x]; // 累加递归路径上的 informTime[x]
            ans = max(ans, pathSum); // 更新答案的最大值
            for (int y: g[x]) { // 遍历 x 的儿子 y（如果没有儿子就不会进入循环）
                dfs(y, pathSum); // 继续递归
            }
        };
        dfs(headID, 0); // 从根节点 headID 开始递归
        return ans;
    }
};
```

```go [sol-Go]
func numOfMinutes(n, headID int, manager, informTime []int) (ans int) {
    g := make([][]int, n)
    for i, m := range manager {
        if m >= 0 {
            g[m] = append(g[m], i) // 建树
        }
    }
    var dfs func(int, int)
    dfs = func(x, pathSum int) {
        pathSum += informTime[x] // 累加递归路径上的 informTime[x]
        ans = max(ans, pathSum)  // 更新答案的最大值
        for _, y := range g[x] { // 遍历 x 的儿子 y（如果没有儿子就不会进入循环）
            dfs(y, pathSum) // 继续递归
        }
    }
    dfs(headID, 0) // 从根节点 headID 开始递归
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。每个节点都恰好访问一次。
- 空间复杂度：$\mathcal{O}(n)$。最坏情况下，树退化成一条链，递归需要 $\mathcal{O}(n)$ 的栈空间。

## 方法二：记忆化搜索

由于 $\textit{manager}$ 数组中保存了每个节点的父节点，无需建树，直接顺着父节点，一路向上，同时累加路径上的 $\textit{informTime}[x]$。

如果暴力枚举每个点，取所有累加值中的最大值作为答案，时间复杂度是 $\mathcal{O}(n^2)$ 的。如何优化？

使用 [记忆化搜索](https://www.bilibili.com/video/BV1Xj411K7oF/) 这一思想，把从 $x$ 向上得到的累加值记录到一个 $\textit{memo}$ 数组中，如果下次再递归到 $x$，就直接返回 $\textit{memo}$ 数组中保存的累加值。（Python 可以用 `@cache` 装饰器）

```py [sol-Python3]
class Solution:
    def numOfMinutes(self, n: int, headID: int, manager: List[int], informTime: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(x: int) -> int:
            if manager[x] < 0:
                return informTime[x]
            return dfs(manager[x]) + informTime[x]
        return max(dfs(i) for i in range(n))
```

```java [sol-Java]
class Solution {
    public int numOfMinutes(int n, int headID, int[] manager, int[] informTime) {
        var memo = new int[n];
        Arrays.fill(memo, -1); // -1 表示还没有计算过
        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans = Math.max(ans, dfs(manager, informTime, memo, i));
        }
        return ans;
    }

    private int dfs(int[] manager, int[] informTime, int[] memo, int x) {
        if (manager[x] < 0) {
            return informTime[x];
        }
        if (memo[x] >= 0) {
            return memo[x]; // 之前计算过了
        }
        return memo[x] = dfs(manager, informTime, memo, manager[x]) + informTime[x];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numOfMinutes(int n, int _, vector<int>& manager, vector<int>& informTime) {
        vector<int> memo(n, -1); // -1 表示还没有计算过
        auto dfs = [&](this auto&& dfs, int x) -> int {
            if (manager[x] < 0) {
                return informTime[x];
            }
            int& res = memo[x]; // 注意这里是引用
            if (res >= 0) {
                return res; // 之前计算过了
            }
            return res = dfs(manager[x]) + informTime[x];
        };
        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans = max(ans, dfs(i));
        }
        return ans;
    }
};
```

```go [sol-Go]
func numOfMinutes(n, _ int, manager, informTime []int) (ans int) {
    memo := make([]int, n)
    for i := range memo {
        memo[i] = -1 // -1 表示还没有计算过
    }
    var dfs func(int) int
    dfs = func(x int) int {
        if manager[x] < 0 {
            return informTime[x]
        }
        if memo[x] >= 0 { // 之前计算过了
            return memo[x]
        }
        res := dfs(manager[x]) + informTime[x]
        memo[x] = res // 记忆化
        return res
    }
    for i := range manager {
        ans = max(ans, dfs(i))
    }
    return
}
```

### 空间优化·其一

把计算结果直接保存到 $\textit{informTime}$ 中。

如何判断之前是否计算过呢？利用 $\textit{manager}$ 数组，如果 $x$ 计算过，就把 $\textit{manager}[x]$ 置为 $-1$。

```py [sol-Python3]
class Solution:
    def numOfMinutes(self, n: int, headID: int, manager: List[int], informTime: List[int]) -> int:
        def dfs(x: int) -> int:
            if manager[x] >= 0:
                informTime[x] += dfs(manager[x])
                manager[x] = -1  # 标记 x 计算过
            return informTime[x]
        return max(dfs(i) for i in range(n))
```

```java [sol-Java]
class Solution {
    public int numOfMinutes(int n, int headID, int[] manager, int[] informTime) {
        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans = Math.max(ans, dfs(manager, informTime, i));
        }
        return ans;
    }

    private int dfs(int[] manager, int[] informTime, int x) {
        if (manager[x] >= 0) {
            informTime[x] += dfs(manager, informTime, manager[x]);
            manager[x] = -1; // 标记 x 计算过
        }
        return informTime[x];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numOfMinutes(int n, int _, vector<int>& manager, vector<int>& informTime) {
        auto dfs = [&](this auto&& dfs, int x) -> int {
            if (manager[x] >= 0) {
                informTime[x] += dfs(manager[x]);
                manager[x] = -1; // 标记 x 计算过
            }
            return informTime[x];
        };
        int ans = 0;
        for (int i = 0; i < n; i++) {
            ans = max(ans, dfs(i));
        }
        return ans;
    }
};
```

```go [sol-Go]
func numOfMinutes(_, _ int, manager, informTime []int) (ans int) {
    var dfs func(int) int
    dfs = func(x int) int {
        if manager[x] >= 0 {
            informTime[x] += dfs(manager[x])
            manager[x] = -1 // 标记 x 计算过
        }
        return informTime[x]
    }
    for i := range manager {
        ans = max(ans, dfs(i))
    }
    return
}
```

### 空间优化·其二

进一步地，把上面的代码改成两次迭代：

- 第一次迭代，仅累加，不更新，计算从当前节点往上的 $\textit{informTime}$ 的累加值 $s$。
- 第二次迭代，更新从当前节点向上的每个未被计算的节点值的对应累加值。在向上移动之前，从 $s$ 中减去当前节点的 $\textit{informTime}$ 值，同时设置当前节点的 $\textit{manger}$ 值为 $-1$。

> 如果你学过并查集，可以试试利用这个技巧，写出 $\texttt{find}$ 函数的非递归版本。

```py [sol-Python3]
class Solution:
    def numOfMinutes(self, n: int, headID: int, manager: List[int], informTime: List[int]) -> int:
        for i, m in enumerate(manager):
            if m < 0: continue

            # 计算从 i 向上的累加值
            s = 0
            x = i
            while manager[x] >= 0:
                s += informTime[x]
                x = manager[x]
            # 此时 x 要么是 headID，要么是一个计算过的节点
            s += informTime[x]

            # 记录从 i 向上的每个未被计算的节点值的对应累加值
            x = i
            while manager[x] >= 0:  # 未被计算
                informTime[x], s = s, s - informTime[x]
                manager[x], x = -1, manager[x]
        return max(informTime)
```

```java [sol-Java]
class Solution {
    public int numOfMinutes(int n, int headID, int[] manager, int[] informTime) {
        int ans = 0;
        for (int i = 0; i < n; i++) {
            if (manager[i] < 0) {
                continue;
            }

            // 计算从 i 向上的累加值
            int s = 0;
            int x = i;
            for (; manager[x] >= 0; x = manager[x]) {
                s += informTime[x];
            }
            // 此时 x 要么是 headID，要么是一个计算过的节点
            s += informTime[x];
            ans = Math.max(ans, s);

            // 记录从 i 向上的每个未被计算的节点值的对应累加值
            for (x = i; manager[x] >= 0;) {
                int t = informTime[x];
                informTime[x] = s;
                s -= t;
                int m = manager[x];
                manager[x] = -1;
                x = m;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numOfMinutes(int n, int _, vector<int>& manager, vector<int>& informTime) {
        int ans = 0;
        for (int i = 0; i < n; i++) {
            if (manager[i] < 0) {
                continue;
            }

            // 计算从 i 向上的累加值
            int s = 0, x = i;
            for (; manager[x] >= 0; x = manager[x]) {
                s += informTime[x];
            }
            // 此时 x 要么是 headID，要么是一个计算过的节点
            s += informTime[x];
            ans = max(ans, s);

            // 记录从 i 向上的每个未被计算的节点值的对应累加值
            for (int x = i; manager[x] >= 0;) {
                int t = informTime[x];
                informTime[x] = s;
                s -= t;
                int m = manager[x];
                manager[x] = -1;
                x = m;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func numOfMinutes(_, _ int, manager, informTime []int) (ans int) {
    for i, m := range manager {
        if m < 0 {
            continue
        }

        // 计算从 i 向上的累加值
        s, x := 0, i
        for ; manager[x] >= 0; x = manager[x] {
            s += informTime[x]
        }
        // 此时 x 要么是 headID，要么是一个计算过的节点
        s += informTime[x]
        ans = max(ans, s)

        // 记录从 i 向上的每个未被计算的节点值的对应累加值
        for x = i; manager[x] >= 0; {
            informTime[x], s = s, s-informTime[x]
            manager[x], x = -1, manager[x]
        }
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。没有建图，实际运行速度比方法一要快一些。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

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

本题来自 `三、一般树 / §3.2 自顶向下 DFS`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、一般树 / §3.2 自顶向下 DFS`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
