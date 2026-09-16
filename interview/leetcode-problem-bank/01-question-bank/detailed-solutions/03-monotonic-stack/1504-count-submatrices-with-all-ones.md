# 1504. 统计全 1 子矩形

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-submatrices-with-all-ones/
- 题目 slug：`count-submatrices-with-all-ones`
- 来源专题：单调栈
- 来源分类路径：二、矩形
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/count-submatrices-with-all-ones/solutions/3704971/omn-dan-diao-zhan-pythonjavacgo-by-endle-jf8l/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】两种方法：枚举上下边界 / 单调栈（Python/Java/C++/Go）](https://leetcode.cn/problems/count-submatrices-with-all-ones/solutions/3704971/omn-dan-diao-zhan-pythonjavacgo-by-endle-jf8l/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`omn-dan-diao-zhan-pythonjavacgo-by-endle-jf8l`
- topic id：`3704971`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：枚举子矩形的上下边界

枚举子矩形的上下边界，统计每一列的 $1$ 的个数，把原问题「压缩」为一维数组上的问题。

![lc1504.jpg](https://pic.leetcode.cn/1750468652-DSwIKo-lc1504.jpg)

在示例 2 中，假设现在枚举到子矩形的上边界为 $0$ 行，下边界为 $1$ 行，即 $\textit{mat}$ 的前两行。子矩形的高 $h=2$。

统计每一列的 $1$ 的个数，得到一个一维数组 $a=[0,2,2,1]$。我们要找的子矩形，就是 $a$ 的子数组。由于全 $1$ 子矩形的每一列都是 $1$，所以子数组的每一项都得是子矩形的高 $h=2$。问题变成：

- 统计 $a$ 的全 $h$ 子数组的数目。

做法同 [2348. 全 0 子数组的数目](https://leetcode.cn/problems/number-of-zero-filled-subarrays/)。

代码实现时，外层循环枚举上边界，内层循环枚举下边界。当下边界 $\textit{bottom}$ 加一时，只需把每个 $a[j]$ 都增加相应的 $\textit{mat}[\textit{bottom}][j]$，无需整个重新统计。

```py [sol-Python3]
class Solution:
    def numSubmat(self, mat: List[List[int]]) -> int:
        m, n = len(mat), len(mat[0])
        ans = 0
        for top in range(m):  # 枚举上边界
            a = [0] * n
            for bottom in range(top, m):  # 枚举下边界
                h = bottom - top + 1  # 高
                # 2348. 全 h 子数组的数目
                last = -1
                for j in range(n):
                    a[j] += mat[bottom][j]  # 把 bottom 这一行的值加到 a 中
                    if a[j] != h:
                        last = j  # 记录上一个非 h 元素的位置
                    else:
                        ans += j - last
        return ans
```

```java [sol-Java]
class Solution {
    public int numSubmat(int[][] mat) {
        int m = mat.length;
        int n = mat[0].length;
        int ans = 0;
        for (int top = 0; top < m; top++) { // 枚举上边界
            int[] a = new int[n];
            for (int bottom = top; bottom < m; bottom++) { // 枚举下边界
                int h = bottom - top + 1; // 高
                // 2348. 全 h 子数组的数目
                int last = -1;
                for (int j = 0; j < n; j++) {
                    a[j] += mat[bottom][j]; // 把 bottom 这一行的值加到 a 中
                    if (a[j] != h) {
                        last = j; // 记录上一个非 h 元素的位置
                    } else {
                        ans += j - last;
                    }
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numSubmat(vector<vector<int>>& mat) {
        int m = mat.size(), n = mat[0].size();
        int ans = 0;
        for (int top = 0; top < m; top++) { // 枚举上边界
            vector<int> a(n);
            for (int bottom = top; bottom < m; bottom++) { // 枚举下边界
                int h = bottom - top + 1; // 高
                // 2348. 全 h 子数组的数目
                int last = -1;
                for (int j = 0; j < n; j++) {
                    a[j] += mat[bottom][j]; // 把 bottom 这一行的值加到 a 中
                    if (a[j] != h) {
                        last = j; // 记录上一个非 h 元素的位置
                    } else {
                        ans += j - last;
                    }
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func numSubmat(mat [][]int) (ans int) {
    m, n := len(mat), len(mat[0])
    for top := range m { // 枚举上边界
        a := make([]int, n)
        for bottom := top; bottom < m; bottom++ { // 枚举下边界
            h := bottom - top + 1 // 高
            // 2348. 全 h 子数组的数目
            last := -1
            for j := range n {
                a[j] += mat[bottom][j] // 把 bottom 这一行的值加到 a 中
                if a[j] != h {
                    last = j // 记录上一个非 h 元素的位置
                } else {
                    ans += j - last
                }
            }
        }
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m^2n)$，其中 $m$ 和 $n$ 分别为 $\textit{mat}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：单调栈

**前置题目**：[85. 最大矩形](https://leetcode.cn/problems/maximal-rectangle/)，接着 [我的题解](https://leetcode.cn/problems/maximal-rectangle/solutions/3704011/zhi-jie-diao-yong-84-ti-dai-ma-jie-jue-p-49at/) 继续讲。

枚举子矩形的**右下角**（枚举底边，枚举右边界），有多少个对应的子矩形？

![lc1504-c.png](https://pic.leetcode.cn/1755747882-QHVXIH-lc1504-c.png)

举个例子。

![lc1504.jpg](https://pic.leetcode.cn/1750468652-DSwIKo-lc1504.jpg)

在示例 2 中，当我们枚举到最后一行时，柱子高度为 $\textit{heights}=[1,3,3,0]$。然后枚举子矩形的右边界：

- 右边界为 $j=0$，子矩形只有 $1$ 个。
- 右边界为 $j=1$，子矩形分成两类：
  - 左边界 $<1$。在右边界为 $j=0$ 时我们算出这有 $1$ 个，现在将其右边界扩展到 $j=1$，得到 $1$ 个新的子矩形。
  - 左边界 $\ge 1$。左边界只能是 $1$，矩形高度有 $1,2,3$ 共 $3$ 种，有 $1\times 3=3$ 个子矩形。
- 右边界为 $j=2$，子矩形分成两类：
    - 左边界 $<1$。在右边界为 $j=0$ 时我们算出这有 $1$ 个，现在将其右边界扩展到 $j=2$，得到 $1$ 个新的子矩形。
    - 左边界 $\ge 1$。左边界可以是 $1,2$ 共 $2$ 种，矩形高度有 $1,2,3$ 共 $3$ 种，有 $2\times 3=6$ 个子矩形。
- 右边界为 $j=3$，高度为 $0$，没有子矩形。

一般地，用 [单调栈](https://www.bilibili.com/video/BV1VN411J7S7/) 计算小于 $\textit{heights}[j]$ 的左边最近柱子的位置 $\textit{left}$，把子矩形分成两类：

- 左边界 $\le \textit{left}$。假设在右边界为 $j=\textit{left}$ 时我们算出这有 $f$ 个，现在将其右边界扩展到 $j$，得到 $f$ 个新的子矩形。
- 左边界 $> \textit{left}$。矩形左边界可以是 $\textit{left}+1,\textit{left}+2,\ldots,j$，共 $j-\textit{left}$ 种；矩形高度可以是 $1,2,\ldots,\textit{heights}[j]$，共 $\textit{heights}[j]$ 种。所以有 $(j-\textit{left})\cdot \textit{heights}[j]$ 个子矩形。
- 二者相加，就是右边界在 $j$ 的子矩形个数。加到答案中。

```py [sol-Python3]
class Solution:
    def numSubmat(self, mat: List[List[int]]) -> int:
        heights = [0] * len(mat[0])
        ans = 0
        for row in mat:
            for j, x in enumerate(row):
                if x == 0:
                    heights[j] = 0
                else:
                    heights[j] += 1

            # (j, f, heights[j])
            st = [(-1, 0, -1)]  # 哨兵，方便处理 left=-1 的情况
            for j, h in enumerate(heights):
                while st[-1][2] >= h:
                    st.pop()
                left, f, _ = st[-1]
                # 计算底边为 row，右边界为 j 的子矩形个数
                # 左边界 <= left 的矩形，每个矩形的右边界都可以扩展到 j，一共有 f 个
                # 左边界 >  left 的矩形，左边界有 j-left 种，高度有 h 种，一共有 (j-left)*h 个
                f += (j - left) * h
                ans += f
                st.append((j, f, h))
        return ans
```

```java [sol-Java]
class Solution {
    public int numSubmat(int[][] mat) {
        int n = mat[0].length;
        int[] heights = new int[n];
        int ans = 0;

        int[][] st = new int[n + 1][3]; // (j, f, heights[j])
        for (int[] row : mat) {
            for (int j = 0; j < n; j++) {
                if (row[j] == 0) {
                    heights[j] = 0;
                } else {
                    heights[j]++;
                }
            }

            st[0][0] = st[0][2] = -1; // 哨兵，方便处理 left=-1 的情况
            int top = 0;
            for (int j = 0; j < n; j++) {
                int h = heights[j];
                while (st[top][2] >= h) {
                    top--; // 出栈
                }
                int left = st[top][0];
                int f = st[top][1];
                // 计算底边为 row，右边界为 j 的子矩形个数
                // 左边界 <= left 的矩形，每个矩形的右边界都可以扩展到 j，一共有 f 个
                // 左边界 >  left 的矩形，左边界有 j-left 种，高度有 h 种，一共有 (j-left)*h 个
                f += (j - left) * h;
                ans += f;
                top++;
                st[top][0] = j; // 入栈
                st[top][1] = f;
                st[top][2] = h;
            }
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numSubmat(vector<vector<int>>& mat) {
        int n = mat[0].size();
        vector<int> heights(n);
        int ans = 0;

        for (auto& row : mat) {
            for (int j = 0; j < n; j++) {
                if (row[j] == 0) {
                    heights[j] = 0;
                } else {
                    heights[j]++;
                }
            }

            stack<tuple<int, int, int>> st; // (j, f, heights[j])
            st.emplace(-1, 0, -1); // 哨兵，方便处理 left=-1 的情况
            for (int j = 0; j < n; j++) {
                int h = heights[j];
                while (get<2>(st.top()) >= h) {
                    st.pop();
                }
                auto [left, f, _] = st.top();
                // 计算底边为 row，右边界为 j 的子矩形个数
                // 左边界 <= left 的矩形，每个矩形的右边界都可以扩展到 j，一共有 f 个
                // 左边界 >  left 的矩形，左边界有 j-left 种，高度有 h 种，一共有 (j-left)*h 个
                f += (j - left) * h;
                ans += f;
                st.emplace(j, f, h);
            }
        }

        return ans;
    }
};
```

```go [sol-Go]
func numSubmat(mat [][]int) (ans int) {
    heights := make([]int, len(mat[0]))
    for _, row := range mat {
        for j, x := range row {
            if x == 0 {
                heights[j] = 0
            } else {
                heights[j]++
            }
        }

        type tuple struct{ j, f, h int }
        st := []tuple{{-1, 0, -1}} // 哨兵，方便处理 left=-1 的情况
        for j, h := range heights {
            for st[len(st)-1].h >= h {
                st = st[:len(st)-1]
            }
            p := st[len(st)-1]
            left, f := p.j, p.f
            // 计算底边为 row，右边界为 j 的子矩形个数
            // 左边界 <= left 的矩形，每个矩形的右边界都可以扩展到 j，一共有 f 个
            // 左边界 >  left 的矩形，左边界有 j-left 种，高度有 h 种，一共有 (j-left)*h 个
            f += (j - left) * h
            ans += f
            st = append(st, tuple{j, f, h})
        }
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别为 $\textit{mat}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

见下面单调栈题单的「**二、矩形**」。

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

本题来自 `二、矩形`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、矩形`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
