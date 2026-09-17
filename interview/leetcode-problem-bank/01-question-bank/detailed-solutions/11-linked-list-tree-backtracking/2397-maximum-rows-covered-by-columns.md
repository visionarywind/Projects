# 2397. 被列覆盖的最多行数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-rows-covered-by-columns/
- 题目 slug：`maximum-rows-covered-by-columns`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.2 子集型回溯
- 难度分：1719
- 外部题解来源：https://leetcode.cn/problems/maximum-rows-covered-by-columns/solutions/1798794/by-endlesscheng-dvxe/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：二进制枚举 / Gosper's Hack（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-rows-covered-by-columns/solutions/1798794/by-endlesscheng-dvxe/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-dvxe`
- topic id：`1798794`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 方法一：二进制枚举

本文会用到很多位运算技巧，请先阅读：[从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

枚举 $\{0,1,2,\cdots, n-1\}$ 的所有大小为 $\textit{numSelect}$ 的子集 $\textit{subset}$，表示我们所选的列。

对于每个 $\textit{subset}$，遍历 $\textit{mat}$ 的每一行，看这一行的 $1$ 的列号集合是否为 $\textit{subset}$ 的子集。统计是子集的行的个数，更新答案的最大值。

```py [sol-Python3]
class Solution:
    def maximumRows(self, mat: List[List[int]], numSelect: int) -> int:
        mask = [sum(x << j for j, x in enumerate(row)) for i, row in enumerate(mat)]
        ans = 0
        for subset in range(1 << len(mat[0])):
            if subset.bit_count() == numSelect:  # subset 的大小等于 numSelect
                covered_rows = sum(row & subset == row for row in mask)
                ans = max(ans, covered_rows)
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumRows(int[][] mat, int numSelect) {
        int m = mat.length, n = mat[0].length;
        int[] mask = new int[m];
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                mask[i] |= mat[i][j] << j;
            }
        }

        int ans = 0;
        for (int subset = 0; subset < (1 << n); subset++) {
            if (Integer.bitCount(subset) == numSelect) {
                int coveredRows = 0;
                for (int row : mask) {
                    if ((row & subset) == row) {
                        coveredRows++;
                    }
                }
                ans = Math.max(ans, coveredRows);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumRows(vector<vector<int>> &mat, int numSelect) {
        int m = mat.size(), n = mat[0].size();
        vector<int> mask(m);
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                mask[i] |= mat[i][j] << j;
            }
        }

        int ans = 0;
        for (int subset = 0; subset < (1 << n); subset++) {
            if (__builtin_popcount(subset) == numSelect) {
                int covered_rows = 0;
                for (int row : mask) {
                    if ((row & subset) == row) {
                        covered_rows++;
                    }
                }
                ans = max(ans, covered_rows);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumRows(mat [][]int, numSelect int) (ans int) {
    m, n := len(mat), len(mat[0])
    mask := make([]int, m)
    for i, row := range mat {
        for j, x := range row {
            mask[i] |= x << j
        }
    }

    for subset := 0; subset < 1<<n; subset++ {
        if bits.OnesCount(uint(subset)) != numSelect {
            continue
        }
        coveredRows := 0
        for _, row := range mask {
            if row&subset == row {
                coveredRows++
            }
        }
        ans = max(ans, coveredRows)
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(2^n + m\cdot C_{n}^{\textit{numSelect}})$。其中 $m$ 和 $n$ 分别为 $\textit{mat}$ 的行数和列数。枚举了 $2^n$ 个子集，其中有 $C_{n}^{\textit{numSelect}}$ 个子集是符合要求的，统计 $\textit{coveredRows}$ 需要 $\mathcal{O}(m)$ 的时间。
- 空间复杂度：$\mathcal{O}(m)$。

## 方法二：Gosper's Hack

上面的代码有很多无效枚举，即大小不等于 $\textit{numSelect}$ 的集合，如何优化呢？

通过使用 Gosper's Hack，我们可以在 $\mathcal{O}(1)$ 的时间内找到下一个大小为 $\textit{numSelect}$ 的集合。

[本题视频讲解](https://www.bilibili.com/video/BV1na41137jv) 中介绍了这个算法。

```py [sol-Python3]
class Solution:
    def maximumRows(self, mat: List[List[int]], numSelect: int) -> int:
        mask = [sum(x << j for j, x in enumerate(row)) for i, row in enumerate(mat)]
        ans = 0
        u = 1 << len(mat[0])
        subset = (1 << numSelect) - 1
        while subset < u:
            covered_rows = sum(row & subset == row for row in mask)
            ans = max(ans, covered_rows)
            lb = subset & -subset
            x = subset + lb
            subset = ((subset ^ x) // lb >> 2) | x
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumRows(int[][] mat, int numSelect) {
        int m = mat.length, n = mat[0].length;
        int[] mask = new int[m];
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                mask[i] |= mat[i][j] << j;
            }
        }

        int ans = 0;
        int subset = (1 << numSelect) - 1;
        while (subset < (1 << n)) {
            int coveredRows = 0;
            for (int row : mask) {
                if ((row & subset) == row) {
                    coveredRows++;
                }
            }
            ans = Math.max(ans, coveredRows);
            int lb = subset & -subset;
            int x = subset + lb;
            subset = ((subset ^ x) / lb >> 2) | x;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumRows(vector<vector<int>> &mat, int numSelect) {
        int m = mat.size(), n = mat[0].size();
        vector<int> mask(m);
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                mask[i] |= mat[i][j] << j;
            }
        }

        int ans = 0;
        int subset = (1 << numSelect) - 1;
        while (subset < (1 << n)) {
            int coveredRows = 0;
            for (int row : mask) {
                if ((row & subset) == row) {
                    coveredRows++;
                }
            }
            ans = max(ans, coveredRows);
            int lb = subset & -subset;
            int x = subset + lb;
            subset = ((subset ^ x) / lb >> 2) | x;
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumRows(mat [][]int, numSelect int) (ans int) {
    m, n := len(mat), len(mat[0])
    mask := make([]int, m)
    for i, row := range mat {
        for j, x := range row {
            mask[i] |= x << j
        }
    }

    subset := 1<<numSelect - 1
    for subset < 1<<n {
        coveredRows := 0
        for _, row := range mask {
            if row&subset == row {
                coveredRows++
            }
        }
        ans = max(ans, coveredRows)
        lb := subset & -subset
        x := subset + lb
        subset = (subset^x)/lb>>2 | x
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\cdot C_{n}^{\textit{numSelect}})$。其中 $m$ 为 $\textit{mat}$ 的行数，$n$ 为 $\textit{mat}$ 的列数。
- 空间复杂度：$\mathcal{O}(m)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、回溯 / §4.2 子集型回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.2 子集型回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
