# 1878. 矩阵中最大的三个菱形和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/get-biggest-three-rhombus-sums-in-a-grid/
- 题目 slug：`get-biggest-three-rhombus-sums-in-a-grid`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.5 进阶
- 难度分：1898
- 外部题解来源：https://leetcode.cn/problems/get-biggest-three-rhombus-sums-in-a-grid/solutions/803561/go-qian-zhui-he-by-endlesscheng-30l0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[斜向前缀和（Python/Java/C++/Go）](https://leetcode.cn/problems/get-biggest-three-rhombus-sums-in-a-grid/solutions/803561/go-qian-zhui-he-by-endlesscheng-30l0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`go-qian-zhui-he-by-endlesscheng-30l0`
- topic id：`803561`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

如何枚举菱形？描述一个菱形需要几个变量？

如果确定了菱形的正中心 $(i,j)$，以及菱形顶点到正中心的距离 $k$，就确定了菱形的四个顶点的位置。

下图的蓝色菱形，正中心在 $(1,1)$，顶点到正中心的距离为 $1$。

![lc1878.png](https://pic.leetcode.cn/1773277698-BpGMad-lc1878.png){:width=170px}

如何快速计算菱形四条边的元素和？

可以用**斜向前缀和**加速：

- 定义 $\textit{diagSum}[i+1][j+1]$ 表示从矩阵最上边或最左边出发，向右下↘到 $(i,j)$，这条线段的元素和。
- 定义 $\textit{antiSum}[i+1][j]$ 表示从矩阵最上边或最右边出发，向左下↙到 $(i,j)$，这条线段的元素和。

为什么这里有一些 $+1$？原理在 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/) 中讲了，类似一维数组的子数组和，即使子数组是前缀，我们仍然可以用两个前缀和之差算出子数组和，无需特判。

根据定义：

- 从 $(x,y)$ 开始，向右下↘连续 $k$ 个数的和为 $\textit{diagSum}[x+k][y+k] - \textit{diagSum}[x][y]$。
- 从 $(x,y)$ 开始，向左下↙连续 $k$ 个数的和为 $\textit{antiSum}[x+k][y+1-k] - \textit{antiSum}[x][y+1]$。

```py [sol-Python3]
class Solution:
    def getBiggestThree(self, grid: List[List[int]]) -> List[int]:
        m, n = len(grid), len(grid[0])
        diag_sum = [[0] * (n + 1) for _ in range(m + 1)]  # ↘ 前缀和
        anti_sum = [[0] * (n + 1) for _ in range(m + 1)]  # ↙ 前缀和
        for i, row in enumerate(grid):
            for j, v in enumerate(row):
                diag_sum[i + 1][j + 1] = diag_sum[i][j] + v
                anti_sum[i + 1][j] = anti_sum[i][j + 1] + v

        # 从 (x,y) 开始，向 ↘，连续 k 个数的和
        def query_diagonal(x: int, y: int, k: int) -> int:
            return diag_sum[x + k][y + k] - diag_sum[x][y]

        # 从 (x,y) 开始，向 ↙，连续 k 个数的和
        def query_anti_diagonal(x: int, y: int, k: int) -> int:
            return anti_sum[x + k][y + 1 - k] - anti_sum[x][y + 1]

        x = y = z = 0  # 最大，次大，第三大

        def update(v: int) -> None:
            nonlocal x, y, z
            if v > x:
                x, y, z = v, x, y
            elif x > v > y:
                y, z = v, y
            elif y > v > z:
                z = v

        # 枚举菱形正中心 (i,j)
        for i, row in enumerate(grid):
            for j, v in enumerate(row):
                update(v)  # 一个数也算菱形
                # 枚举菱形顶点到正中心的距离 k，注意菱形顶点不能出界
                # i-k >= 0 且 i+k <= m-1，所以 k <= min(i, m-1-i)，对于 j 同理
                mx = min(i, m - 1 - i, j, n - 1 - j)
                for k in range(1, mx + 1):
                    a = query_diagonal(i - k, j, k)                   # 菱形右上的边
                    b = query_diagonal(i, j - k, k)                   # 菱形左下的边
                    c = query_anti_diagonal(i - k + 1, j - 1, k - 1)  # 菱形左上的边
                    d = query_anti_diagonal(i, j + k, k + 1)          # 菱形右下的边
                    update(a + b + c + d)

        ans = [x, y, z]
        while ans[-1] == 0:  # 不同的和少于三个
            ans.pop()
        return ans
```

```java [sol-Java]
class Solution {
    private int x, y, z; // 最大，次大，第三大

    public int[] getBiggestThree(int[][] grid) {
        int m = grid.length;
        int n = grid[0].length;
        int[][] diagSum = new int[m + 1][n + 1]; // ↘ 前缀和
        int[][] antiSum = new int[m + 1][n + 1]; // ↙ 前缀和
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                int v = grid[i][j];
                diagSum[i + 1][j + 1] = diagSum[i][j] + v;
                antiSum[i + 1][j] = antiSum[i][j + 1] + v;
            }
        }

        // 枚举菱形正中心 (i,j)
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                update(grid[i][j]); // 一个数也算菱形
                // 枚举菱形顶点到正中心的距离 k，注意菱形顶点不能出界
                // i-k >= 0 且 i+k <= m-1，所以 k <= min(i, m-1-i)，对于 j 同理
                int mx = Math.min(Math.min(i, m - 1 - i), Math.min(j, n - 1 - j));
                for (int k = 1; k <= mx; k++) {
                    int a = queryDiagonal(diagSum, i - k, j, k); // 菱形右上的边
                    int b = queryDiagonal(diagSum, i, j - k, k); // 菱形左下的边
                    int c = queryAntiDiagonal(antiSum, i - k + 1, j - 1, k - 1); // 菱形左上的边
                    int d = queryAntiDiagonal(antiSum, i, j + k, k + 1); // 菱形右下的边
                    update(a + b + c + d);
                }
            }
        }

        int[] ans = new int[]{x, y, z};
        int len = 3;
        while (ans[len - 1] == 0) { // 不同的和少于三个
            len--;
        }
        return Arrays.copyOf(ans, len);
    }

    // 从 (x,y) 开始，向 ↘，连续 k 个数的和
    private int queryDiagonal(int[][] diagSum, int x, int y, int k) {
        return diagSum[x + k][y + k] - diagSum[x][y];
    }

    // 从 (x,y) 开始，向 ↙，连续 k 个数的和
    private int queryAntiDiagonal(int[][] antiSum, int x, int y, int k) {
        return antiSum[x + k][y + 1 - k] - antiSum[x][y + 1];
    }

    private void update(int v) {
        if (v > x) {
            z = y;
            y = x;
            x = v;
        } else if (v < x && v > y) {
            z = y;
            y = v;
        } else if (v < y && v > z) {
            z = v;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> getBiggestThree(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        vector diag_sum(m + 1, vector<int>(n + 1)); // ↘ 前缀和
        vector anti_sum(m + 1, vector<int>(n + 1)); // ↙ 前缀和
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                int v = grid[i][j];
                diag_sum[i + 1][j + 1] = diag_sum[i][j] + v;
                anti_sum[i + 1][j] = anti_sum[i][j + 1] + v;
            }
        }

        // 从 (x,y) 开始，向 ↘，连续 k 个数的和
        auto query_diagonal = [&](int x, int y, int k) -> int {
            return diag_sum[x + k][y + k] - diag_sum[x][y];
        };

        // 从 (x,y) 开始，向 ↙，连续 k 个数的和
        auto query_anti_diagonal = [&](int x, int y, int k) -> int {
            return anti_sum[x + k][y + 1 - k] - anti_sum[x][y + 1];
        };

        int x = 0, y = 0, z = 0; // 最大，次大，第三大

        auto update = [&](int v) -> void {
            if (v > x) {
                z = y;
                y = x;
                x = v;
            } else if (v < x && v > y) {
                z = y;
                y = v;
            } else if (v < y && v > z) {
                z = v;
            }
        };

        // 枚举菱形正中心 (i,j)
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                update(grid[i][j]); // 一个数也算菱形
                // 枚举菱形顶点到正中心的距离 k，注意菱形顶点不能出界
                // i-k >= 0 且 i+k <= m-1，所以 k <= min(i, m-1-i)，对于 j 同理
                int mx = min({i, m - 1 - i, j, n - 1 - j});
                for (int k = 1; k <= mx; k++) {
                    int a = query_diagonal(i - k, j, k); // 菱形右上的边
                    int b = query_diagonal(i, j - k, k); // 菱形左下的边
                    int c = query_anti_diagonal(i - k + 1, j - 1, k - 1); // 菱形左上的边
                    int d = query_anti_diagonal(i, j + k, k + 1); // 菱形右下的边
                    update(a + b + c + d);
                }
            }
        }

        vector<int> ans = {x, y, z};
        while (ans.back() == 0) { // 不同的和少于三个
            ans.pop_back();
        }
        return ans;
    }
};
```

```go [sol-Go]
func getBiggestThree(grid [][]int) []int {
	m, n := len(grid), len(grid[0])
	diagSum := make([][]int, m+1) // ↘ 前缀和
	antiSum := make([][]int, m+1) // ↙ 前缀和
	for i := range diagSum {
		diagSum[i] = make([]int, n+1)
		antiSum[i] = make([]int, n+1)
	}
	for i, row := range grid {
		for j, v := range row {
			diagSum[i+1][j+1] = diagSum[i][j] + v
			antiSum[i+1][j] = antiSum[i][j+1] + v
		}
	}

	// 从 (x,y) 开始，向 ↘，连续 k 个数的和
	queryDiagonal := func(x, y, k int) int { return diagSum[x+k][y+k] - diagSum[x][y] }

	// 从 (x,y) 开始，向 ↙，连续 k 个数的和
	queryAntiDiagonal := func(x, y, k int) int { return antiSum[x+k][y+1-k] - antiSum[x][y+1] }

	var x, y, z int // 最大，次大，第三大
	update := func(v int) {
		if v > x {
			x, y, z = v, x, y
		} else if v < x && v > y {
			y, z = v, y
		} else if v < y && v > z {
			z = v
		}
	}

	// 枚举菱形正中心 (i,j)
	for i, row := range grid {
		for j, v := range row {
			update(v) // 一个数也算菱形
			// 枚举菱形顶点到正中心的距离 k，注意菱形顶点不能出界
			// i-k >= 0 且 i+k <= m-1，所以 k <= min(i, m-1-i)，对于 j 同理
			mx := min(i, m-1-i, j, n-1-j)
			for k := 1; k <= mx; k++ {
				a := queryDiagonal(i-k, j, k)           // 菱形右上的边
				b := queryDiagonal(i, j-k, k)           // 菱形左下的边
				c := queryAntiDiagonal(i-k+1, j-1, k-1) // 菱形左上的边
				d := queryAntiDiagonal(i, j+k, k+1)     // 菱形右下的边
				update(a + b + c + d)
			}
		}
	}

	ans := []int{x, y, z}
	for ans[len(ans)-1] == 0 { // 不同的和少于三个
		ans = ans[:len(ans)-1]
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn\min(m,n))$，其中 $m$ 和 $n$ 分别是 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(mn)$。

## 相似题目

[1895. 最大的幻方](https://leetcode.cn/problems/largest-magic-square/)

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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、前缀和 / §1.5 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.5 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
