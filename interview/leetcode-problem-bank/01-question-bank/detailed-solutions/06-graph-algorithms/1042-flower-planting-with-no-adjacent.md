# 1042. 不邻接植花

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/flower-planting-with-no-adjacent/
- 题目 slug：`flower-planting-with-no-adjacent`
- 来源专题：图论算法
- 来源分类路径：九、其他
- 难度分：1712
- 外部题解来源：https://leetcode.cn/problems/flower-planting-with-no-adjacent/solutions/2227318/liang-chong-xie-fa-ha-xi-biao-shu-zu-wei-7hm8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种写法：哈希表（数组）/位运算（Python/Java/C++/Go）](https://leetcode.cn/problems/flower-planting-with-no-adjacent/solutions/2227318/liang-chong-xie-fa-ha-xi-biao-shu-zu-wei-7hm8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-xie-fa-ha-xi-biao-shu-zu-wei-7hm8`
- topic id：`2227318`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

出题人可能是受到 [四色定理](https://baike.baidu.com/item/%E5%9B%9B%E8%89%B2%E5%AE%9A%E7%90%86/805159) 的启发出的题。

问题相当于用 $4$ 种颜色给图中的每个节点染色，要求相邻节点颜色不同。而「所有花园最多有 $3$ 条路径可以进入或离开」，这相当于图中每个点的度数至多为 $3$，那么只要选一个和邻居不同的颜色即可。

## 哈希表（数组）实现

```py [sol-Python3]
class Solution:
    def gardenNoAdj(self, n: int, paths: List[List[int]]) -> List[int]:
        g = [[] for _ in range(n)]
        for u, v in paths:
            g[u - 1].append(v - 1)
            g[v - 1].append(u - 1)  # 建图

        color = [0] * n
        for i, nodes in enumerate(g):
            color[i] = (set(range(1, 5)) - {color[j] for j in nodes}).pop()
        return color
```

```java [sol-Java]
class Solution {
    public int[] gardenNoAdj(int n, int[][] paths) {
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int[] e : paths) {
            int x = e[0] - 1; // 编号改从 0 开始
            int y = e[1] - 1;
            g[x].add(y);
            g[y].add(x); // 建图
        }

        int[] color = new int[n];
        for (int i = 0; i < n; i++) {
            boolean[] used = new boolean[5];
            for (int j : g[i]) {
                used[color[j]] = true;
            }
            while (used[++color[i]]);
        }
        return color;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> gardenNoAdj(int n, vector<vector<int>>& paths) {
        vector<vector<int>> g(n);
        for (auto& e : paths) {
            int x = e[0] - 1, y = e[1] - 1; // 编号改从 0 开始
            g[x].push_back(y);
            g[y].push_back(x); // 建图
        }

        vector<int> color(n);
        for (int i = 0; i < n; i++) {
            bool used[5]{};
            for (int j : g[i]) {
                used[color[j]] = true;
            }
            while (used[++color[i]]);
        }
        return color;
    }
};
```

```go [sol-Go]
func gardenNoAdj(n int, paths [][]int) []int {
    g := make([][]int, n)
    for _, e := range paths {
        x, y := e[0]-1, e[1]-1 // 编号改从 0 开始
        g[x] = append(g[x], y)
        g[y] = append(g[y], x) // 建图
    }

    color := make([]int, n)
    for i, nodes := range g {
        used := [5]bool{}
        for _, j := range nodes {
            used[color[j]] = true
        }
        for color[i]++; used[color[i]]; color[i]++ {
        }
    }
    return color
}
```

## 位运算实现

集合（或者布尔数组）可以用二进制表示，二进制从低到高第 $i$ 位为 $1$ 表示 $i$ 在集合中，为 $0$ 表示 $i$ 不在集合中。例如集合 $\{0,2,3\}$ 对应的二进制数为 $1101_{(2)}$。

下面代码用到的位运算技巧：

1. 把 $x$ 添加到 $\textit{mask}$ 中：将 `mask` 更新为 `mask | (1 << x)`。
2. 找到 $\textit{mask}$ 从低到高第一个 $0$ 的位置：计算 $\textit{mask}$ 取反后的尾零个数。例如 $\textit{mask}=10111_{(2)}$，取反后变为 $1000_{(2)}$（实际前导零也取反了，但不影响计算），尾零个数为 $3$，这恰好就是从低到高第一个 $0$ 的位置。

```py [sol-Python3]
class Solution:
    def gardenNoAdj(self, n: int, paths: List[List[int]]) -> List[int]:
        g = [[] for _ in range(n)]
        for u, v in paths:
            g[u - 1].append(v - 1)
            g[v - 1].append(u - 1)  # 建图

        color = [0] * n
        for i, nodes in enumerate(g):
            mask = 1  # 由于颜色是 1~4，把 0 加入 mask 保证下面不会算出 0
            for j in g[i]:
                mask |= 1 << color[j]
            mask = ~mask
            # Python 没有统计尾零的库函数，可以枚举，或者求 lowbit 的二进制长度减一
            color[i] = (mask & -mask).bit_length() - 1
        return color
```

```java [sol-Java]
class Solution {
    public int[] gardenNoAdj(int n, int[][] paths) {
        List<Integer>[] g = new ArrayList[n];
        Arrays.setAll(g, e -> new ArrayList<>());
        for (int[] e : paths) {
            int x = e[0] - 1; // 编号改从 0 开始
            int y = e[1] - 1;
            g[x].add(y);
            g[y].add(x); // 建图
        }

        int[] color = new int[n];
        for (int i = 0; i < n; i++) {
            int mask = 1; // 由于颜色是 1~4，把 0 加入 mask 保证下面不会算出 0
            for (int j : g[i]) {
                mask |= 1 << color[j];
            }
            color[i] = Integer.numberOfTrailingZeros(~mask);
        }
        return color;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> gardenNoAdj(int n, vector<vector<int>>& paths) {
        vector<vector<int>> g(n);
        for (auto& e : paths) {
            int x = e[0] - 1, y = e[1] - 1; // 编号改从 0 开始
            g[x].push_back(y);
            g[y].push_back(x); // 建图
        }

        vector<int> color(n);
        for (int i = 0; i < n; i++) {
            unsigned mask = 1; // 由于颜色是 1~4，把 0 加入 mask 保证下面不会算出 0
            for (int j : g[i]) {
                mask |= 1 << color[j];
            }
            color[i] = countr_zero(~mask);
        }
        return color;
    }
};
```

```go [sol-Go]
func gardenNoAdj(n int, paths [][]int) []int {
    g := make([][]int, n)
    for _, e := range paths {
        x, y := e[0]-1, e[1]-1 // 编号改从 0 开始
        g[x] = append(g[x], y)
        g[y] = append(g[y], x) // 建图
    }

    color := make([]int, n)
    for i, nodes := range g {
        mask := uint8(1) // 由于颜色是 1~4，把 0 加入 mask 保证下面不会算出 0
        for _, j := range nodes {
            mask |= 1 << color[j]
        }
        color[i] = bits.TrailingZeros8(^mask)
    }
    return color
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $m$ 为 $\textit{paths}$ 的长度。
- 空间复杂度：$\mathcal{O}(n+m)$。

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

本题来自 `九、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
