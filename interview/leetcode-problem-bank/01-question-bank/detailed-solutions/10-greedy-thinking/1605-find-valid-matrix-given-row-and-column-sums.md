# 1605. 给定行和列的和求可行矩阵

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-valid-matrix-given-row-and-column-sums/
- 题目 slug：`find-valid-matrix-given-row-and-column-sums`
- 来源专题：贪心与思维
- 来源分类路径：六、构造题
- 难度分：1868
- 外部题解来源：https://leetcode.cn/problems/find-valid-matrix-given-row-and-column-sums/solutions/2166773/mei-you-si-lu-yi-ge-dong-hua-miao-dong-f-eezj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[没有思路？一个动画秒懂！附优化写法（Python/Java/C++/Go）](https://leetcode.cn/problems/find-valid-matrix-given-row-and-column-sums/solutions/2166773/mei-you-si-lu-yi-ge-dong-hua-miao-dong-f-eezj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-you-si-lu-yi-ge-dong-hua-miao-dong-f-eezj`
- topic id：`2166773`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

用示例 2 来演示：

<![1605-1-1.png](https://pic.leetcode.cn/1678715751-VTMHhZ-1605-1-1.png),![1605-1-2.png](https://pic.leetcode.cn/1678715751-rgmarG-1605-1-2.png),![1605-1-3.png](https://pic.leetcode.cn/1678715751-gAFjJf-1605-1-3.png),![1605-1-4.png](https://pic.leetcode.cn/1678715751-pslefX-1605-1-4.png),![1605-1-5.png](https://pic.leetcode.cn/1678715751-eRvsYb-1605-1-5.png),![1605-1-6.png](https://pic.leetcode.cn/1678715751-DfWSFv-1605-1-6.png),![1605-1-7.png](https://pic.leetcode.cn/1678715751-aFAGbJ-1605-1-7.png),![1605-1-8.png](https://pic.leetcode.cn/1678715751-Qskoek-1605-1-8.png),![1605-1-9.png](https://pic.leetcode.cn/1678715751-DuNaRa-1605-1-9.png),![1605-1-10.png](https://pic.leetcode.cn/1678716136-UTIqOr-1605-1-10.png)>

### 答疑

**问**：如何证明该构造方案一定能得到满足题目要求的矩阵？

**答**：设生成的矩阵为 $\textit{mat}$。对于只有 $1$ 行的情况，构造 $\textit{mat}[0][j] = \textit{colSum}[j]$，由于题目保证 `sum(rowSum) == sum(colSum)`，所以只有 $1$ 行的 $\textit{mat}$ 是满足题目要求的。

对于一个有 $m$ 行的矩阵 $\textit{mat}$，上述构造方案可以满足 $\textit{mat}$ 第一行的 $\textit{rowSum}$，且构造的数字不超过相应的 $\textit{colSum}$，从而转换成一个 $m-1$ 行的子问题。由于原问题转换成子问题后，`sum(rowSum) == sum(colSum)` 是不变的，只要 $m-1$ 行的 $\textit{mat}$ 是满足题目要求的，那么 $m$ 行的 $\textit{mat}$ 也是满足题目要求的。

根据数学归纳法，$m$ 行的 $\textit{mat}$ 是满足题目要求的。

```py [sol1-Python3]
class Solution:
    def restoreMatrix(self, rowSum: List[int], colSum: List[int]) -> List[List[int]]:
        m, n = len(rowSum), len(colSum)
        mat = [[0] * n for _ in range(m)]
        for i, rs in enumerate(rowSum):
            for j, cs in enumerate(colSum):
                mat[i][j] = x = min(rs, cs)
                rs -= x
                colSum[j] -= x
        return mat
```

```java [sol1-Java]
class Solution {
    public int[][] restoreMatrix(int[] rowSum, int[] colSum) {
        int m = rowSum.length, n = colSum.length;
        var mat = new int[m][n];
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                mat[i][j] = Math.min(rowSum[i], colSum[j]);
                rowSum[i] -= mat[i][j];
                colSum[j] -= mat[i][j];
            }
        }
        return mat;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    vector<vector<int>> restoreMatrix(vector<int> &rowSum, vector<int> &colSum) {
        int m = rowSum.size(), n = colSum.size();
        vector<vector<int>> mat(m, vector<int>(n));
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                mat[i][j] = min(rowSum[i], colSum[j]);
                rowSum[i] -= mat[i][j];
                colSum[j] -= mat[i][j];
            }
        }
        return mat;
    }
};
```

```go [sol1-Go]
func restoreMatrix(rowSum, colSum []int) [][]int {
    mat := make([][]int, len(rowSum))
    for i, rs := range rowSum {
        mat[i] = make([]int, len(colSum))
        for j, cs := range colSum {
            mat[i][j] = min(rs, cs)
            rs -= mat[i][j]
            colSum[j] -= mat[i][j]
        }
    }
    return mat
}
```

### 优化

![1605-2.png](https://pic.leetcode.cn/1678715751-RwBmnc-1605-2.png)

也可以这样理解：从左上角出发，每次要么去掉一行，要么去掉一列。同样根据数学归纳法，可以证明该做法的正确性。

```py [sol2-Python3]
class Solution:
    def restoreMatrix(self, rowSum: List[int], colSum: List[int]) -> List[List[int]]:
        m, n = len(rowSum), len(colSum)
        mat = [[0] * n for _ in range(m)]
        i = j = 0  # 从左上角出发
        while i < m and j < n:
            rs, cs = rowSum[i], colSum[j]
            if rs < cs:
                mat[i][j] = rs  # 去掉第 i 行
                colSum[j] -= rs
                i += 1  # 往下走
            else:
                mat[i][j] = cs  # 去掉第 j 列
                rowSum[i] -= cs
                j += 1  # 往右走
        return mat
```

```java [sol2-Java]
class Solution {
    public int[][] restoreMatrix(int[] rowSum, int[] colSum) {
        int m = rowSum.length, n = colSum.length;
        var mat = new int[m][n];
        for (int i = 0, j = 0; i < m && j < n; ) {
            int rs = rowSum[i], cs = colSum[j];
            if (rs < cs) { // 去掉第 i 行，往下走
                colSum[j] -= rs;
                mat[i++][j] = rs;
            } else { // 去掉第 j 列，往右走
                rowSum[i] -= cs;
                mat[i][j++] = cs;
            }
        }
        return mat;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    vector<vector<int>> restoreMatrix(vector<int> &rowSum, vector<int> &colSum) {
        int m = rowSum.size(), n = colSum.size();
        vector<vector<int>> mat(m, vector<int>(n));
        for (int i = 0, j = 0; i < m && j < n; ) {
            int rs = rowSum[i], cs = colSum[j];
            if (rs < cs) { // 去掉第 i 行，往下走
                colSum[j] -= rs;
                mat[i++][j] = rs;
            } else { // 去掉第 j 列，往右走
                rowSum[i] -= cs;
                mat[i][j++] = cs;
            }
        }
        return mat;
    }
};
```

```go [sol2-Go]
func restoreMatrix(rowSum, colSum []int) [][]int {
    m, n := len(rowSum), len(colSum)
    mat := make([][]int, m)
    for i := range mat {
        mat[i] = make([]int, n)
    }
    for i, j := 0, 0; i < m && j < n; {
        rs, cs := rowSum[i], colSum[j]
        if rs < cs {
            mat[i][j] = rs // 去掉第 i 行
            colSum[j] -= rs
            i++ // 往下走
        } else {
            mat[i][j] = cs // 去掉第 j 列
            rowSum[i] -= cs
            j++ // 往右走
        }
    }
    return mat
}
```

### 复杂度分析

- 时间复杂度：$O(mn)$，其中 $m$ 为矩阵行数，即 $\textit{rowSum}$ 的长度；$n$ 为矩阵列数，即 $\textit{colSum}$ 的长度。如果忽略创建二维数组的时间，时间复杂度为 $O(m+n)$。
- 空间复杂度：$O(1)$。返回值不计入。

## 分类题单

以下题单没有特定的顺序，可以按照个人喜好刷题。

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `六、构造题`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、构造题`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
