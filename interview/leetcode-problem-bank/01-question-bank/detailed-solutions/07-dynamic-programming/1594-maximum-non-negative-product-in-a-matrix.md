# 1594. 矩阵的最大非负积

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-non-negative-product-in-a-matrix/
- 题目 slug：`maximum-non-negative-product-in-a-matrix`
- 来源专题：动态规划
- 来源分类路径：二、网格图 DP / §2.2 进阶
- 难度分：1807
- 外部题解来源：https://leetcode.cn/problems/maximum-non-negative-product-in-a-matrix/solutions/3923633/ji-yi-hua-sou-suo-di-tui-kong-jian-you-h-hz2m/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[记忆化搜索 -> 递推 -> 空间优化（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-non-negative-product-in-a-matrix/solutions/3923633/ji-yi-hua-sou-suo-di-tui-kong-jian-you-h-hz2m/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-yi-hua-sou-suo-di-tui-kong-jian-you-h-hz2m`
- topic id：`3923633`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 前置题目

1. [152. 乘积最大子数组](https://leetcode.cn/problems/maximum-product-subarray/)，[我的题解](https://leetcode.cn/problems/maximum-product-subarray/solutions/2968916/dong-tai-gui-hua-jian-ji-gao-xiao-python-i778/)。
2. [64. 最小路径和](https://leetcode.cn/problems/minimum-path-sum/)，[我的题解](https://leetcode.cn/problems/minimum-path-sum/solutions/3045828/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-zfb2/)。

## 状态定义与状态转移方程

思路和 152 题是一样的，除了计算最大路径乘积，还要计算最小路径乘积（因为负负得正）。

定义 $\textit{dfs}(i,j)$ 表示从左上角 $(0,0)$ 到 $(i,j)$ 的最小路径乘积以及最大路径乘积（$\textit{dfs}$ 返回两个数）。

设 $x = \textit{grid}[i][j]$。分类讨论如何到达 $(i,j)$：

- 如果是从上边过来，那么必须先到达 $(i-1,j)$，我们需要知道从 $(0,0)$ 到 $(i-1,j)$ 的最小路径乘积 $\textit{mn}$ 以及最大路径乘积 $\textit{mx}$，这可以从 $\textit{dfs}(i-1,j)$ 获取到。从左上角 $(0,0)$ 到 $(i,j)$ 的最小路径乘积为 $\min(\textit{mn}\cdot x, \textit{mx}\cdot x)$，从左上角 $(0,0)$ 到 $(i,j)$ 的最大路径乘积为 $\max(\textit{mn}\cdot x, \textit{mx}\cdot x)$。理由同 152 题。
- 如果是从左边过来，那么必须先到达 $(i,j-1)$，我们需要知道从 $(0,0)$ 到 $(i,j-1)$ 的最小路径乘积以及最大路径乘积，这可以从 $\textit{dfs}(i,j-1)$ 获取到。计算方法同上。

两种情况取最小值（最大值），即为 $\textit{dfs}(i,j)$ 的返回值。

递归边界：$\textit{dfs}(0,0) = (x,x)$。

递归入口：$\textit{dfs}(m-1,n-1)$。取返回值中的最大路径乘积作为答案。如果答案是负数，返回 $-1$；否则返回答案模 $10^9+7$ 的结果。

⚠**注意**：题目要求**算完了再取模**。如果在中途取模，可能会把一个很大的数模成很小的数，导致计算错误。比如两个数 $10^9+8$ 和 $10^9$，取模之前是 $10^9+8$ 更大，但取模后这两个数分别变成 $1$ 和 $10^9$，后者更大。

```py [sol-Python3]
class Solution:
    def maxProductPath(self, grid: List[List[int]]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int, j: int) -> Tuple[int, int]:
            x = grid[i][j]
            if i == j == 0:
                return x, x

            res_min, res_max = inf, -inf
            if i > 0:
                mn, mx = dfs(i - 1, j)
                res_min = min(mn * x, mx * x)
                res_max = max(mn * x, mx * x)
            if j > 0:
                mn, mx = dfs(i, j - 1)
                res_min = min(res_min, mn * x, mx * x)
                res_max = max(res_max, mn * x, mx * x)

            return res_min, res_max

        ans = dfs(len(grid) - 1, len(grid[0]) - 1)[1]
        return -1 if ans < 0 else ans % 1_000_000_007
```

```java [sol-Java]
class Solution {
    public int maxProductPath(int[][] grid) {
        int m = grid.length, n = grid[0].length;
        long[][][] memo = new long[m][n][2];
        for (long[][] row : memo) {
            for (long[] p : row) {
                p[0] = p[1] = Long.MIN_VALUE;
            }
        }

        long ans = dfs(m - 1, n - 1, grid, memo)[1];
        return ans < 0 ? -1 : (int) (ans % 1_000_000_007);
    }

    private long[] dfs(int i, int j, int[][] grid, long[][][] memo) {
        long x = grid[i][j];
        if (i == 0 && j == 0) {
            return new long[]{x, x};
        }

        long[] p = memo[i][j];
        if (p[0] != Long.MIN_VALUE) { // 之前计算过
            return p;
        }

        long resMin = Long.MAX_VALUE;
        long resMax = Long.MIN_VALUE;
        if (i > 0) {
            long[] res = dfs(i - 1, j, grid, memo);
            long mn = res[0], mx = res[1];
            resMin = Math.min(mn * x, mx * x);
            resMax = Math.max(mn * x, mx * x);
        }
        if (j > 0) {
            long[] res = dfs(i, j - 1, grid, memo);
            long mn = res[0], mx = res[1];
            resMin = Math.min(resMin, Math.min(mn * x, mx * x));
            resMax = Math.max(resMax, Math.max(mn * x, mx * x));
        }

        p[0] = resMin;
        p[1] = resMax; // 记忆化
        return p;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProductPath(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        vector memo(m, vector<array<long long, 2>>(n, {LLONG_MIN, LLONG_MIN}));

        auto dfs = [&](this auto&& dfs, int i, int j) -> array<long long, 2> {
            long long x = grid[i][j];
            if (i == 0 && j == 0) {
                return {x, x};
            }

            auto& res = memo[i][j]; // 注意这里是引用
            if (res[0] != LLONG_MIN) { // 之前计算过
                return res;
            }

            long long res_min = LLONG_MAX;
            long long res_max = LLONG_MIN;
            if (i > 0) {
                auto [mn, mx] = dfs(i - 1, j);
                res_min = min(mn * x, mx * x);
                res_max = max(mn * x, mx * x);
            }
            if (j > 0) {
                auto [mn, mx] = dfs(i, j - 1);
                res_min = min(res_min, min(mn * x, mx * x));
                res_max = max(res_max, max(mn * x, mx * x));
            }

            res = {res_min, res_max}; // 记忆化
            return res;
        };

        long long ans = dfs(m - 1, n - 1)[1];
        return ans < 0 ? -1 : ans % 1'000'000'007;
    }
};
```

```go [sol-Go]
func maxProductPath(grid [][]int) int {
	m, n := len(grid), len(grid[0])
	memo := make([][][2]int, m)
	for i := range memo {
		memo[i] = make([][2]int, n)
		for j := range memo[i] {
			memo[i][j] = [2]int{math.MinInt, math.MinInt}
		}
	}

	var dfs func(int, int) (int, int)
	dfs = func(i, j int) (int, int) {
		x := grid[i][j]
		if i == 0 && j == 0 {
			return x, x
		}

		p := &memo[i][j]
		if p[0] != math.MinInt { // 之前计算过
			return p[0], p[1]
		}

		resMin := math.MaxInt
		resMax := math.MinInt
		if i > 0 {
			mn, mx := dfs(i-1, j)
			resMin = min(mn*x, mx*x)
			resMax = max(mn*x, mx*x)
		}
		if j > 0 {
			mn, mx := dfs(i, j-1)
			resMin = min(resMin, mn*x, mx*x)
			resMax = max(resMax, mn*x, mx*x)
		}

		p[0], p[1] = resMin, resMax // 记忆化
		return resMin, resMax
	}

	_, ans := dfs(m-1, n-1)
	if ans < 0 {
		return -1
	}
	return ans % 1_000_000_007
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{grid}$ 的行数和列数。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(mn)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(mn)$。
- 空间复杂度：$\mathcal{O}(mn)$。保存多少状态，就需要多少空间。

## 1:1 翻译成递推

把 $\textit{dfs}(i,j)$ 改成 $f[i][j]$。

```py [sol-Python3]
class Solution:
    def maxProductPath(self, grid: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])
        f = [[None] * n for _ in range(m)]

        for i, row in enumerate(grid):
            for j, x in enumerate(row):
                if i == j == 0:
                    f[0][0] = (x, x)
                    continue

                res_min, res_max = inf, -inf
                if i > 0:
                    mn, mx = f[i - 1][j]
                    res_min = min(mn * x, mx * x)
                    res_max = max(mn * x, mx * x)
                if j > 0:
                    mn, mx = f[i][j - 1]
                    res_min = min(res_min, mn * x, mx * x)
                    res_max = max(res_max, mn * x, mx * x)

                f[i][j] = (res_min, res_max)

        ans = f[-1][-1][1]
        return -1 if ans < 0 else ans % 1_000_000_007
```

```java [sol-Java]
class Solution {
    public int maxProductPath(int[][] grid) {
        int m = grid.length, n = grid[0].length;
        long[][][] f = new long[m][n][2];

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                long x = grid[i][j];
                if (i == 0 && j == 0) {
                    f[0][0][0] = x;
                    f[0][0][1] = x;
                    continue;
                }

                long resMin = Long.MAX_VALUE;
                long resMax = Long.MIN_VALUE;
                if (i > 0) {
                    long mn = f[i - 1][j][0], mx = f[i - 1][j][1];
                    resMin = Math.min(mn * x, mx * x);
                    resMax = Math.max(mn * x, mx * x);
                }
                if (j > 0) {
                    long mn = f[i][j - 1][0], mx = f[i][j - 1][1];
                    resMin = Math.min(resMin, Math.min(mn * x, mx * x));
                    resMax = Math.max(resMax, Math.max(mn * x, mx * x));
                }

                f[i][j][0] = resMin;
                f[i][j][1] = resMax;
            }
        }

        long ans = f[m - 1][n - 1][1];
        return ans < 0 ? -1 : (int) (ans % 1_000_000_007);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProductPath(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        vector f(m, vector<array<long long, 2>>(n));

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                long long x = grid[i][j];
                if (i == 0 && j == 0) {
                    f[0][0] = {x, x};
                    continue;
                }

                long long res_min = LLONG_MAX;
                long long res_max = LLONG_MIN;
                if (i > 0) {
                    auto [mn, mx] = f[i - 1][j];
                    res_min = min(mn * x, mx * x);
                    res_max = max(mn * x, mx * x);
                }
                if (j > 0) {
                    auto [mn, mx] = f[i][j - 1];
                    res_min = min(res_min, min(mn * x, mx * x));
                    res_max = max(res_max, max(mn * x, mx * x));
                }

                f[i][j] = {res_min, res_max};
            }
        }

        long long ans = f[m - 1][n - 1][1];
        return ans < 0 ? -1 : ans % 1'000'000'007;
    }
};
```

```go [sol-Go]
func maxProductPath(grid [][]int) int {
	m, n := len(grid), len(grid[0])
	f := make([][][2]int, m)
	for i := range f {
		f[i] = make([][2]int, n)
	}

	for i, row := range grid {
		for j, x := range row {
			if i == 0 && j == 0 {
				f[0][0] = [2]int{x, x}
				continue
			}

			resMin := math.MaxInt
			resMax := math.MinInt
			if i > 0 {
				mn, mx := f[i-1][j][0], f[i-1][j][1]
				resMin = min(mn*x, mx*x)
				resMax = max(mn*x, mx*x)
			}
			if j > 0 {
				mn, mx := f[i][j-1][0], f[i][j-1][1]
				resMin = min(resMin, mn*x, mx*x)
				resMax = max(resMax, mn*x, mx*x)
			}

			f[i][j] = [2]int{resMin, resMax}
		}
	}

	ans := f[m-1][n-1][1]
	if ans < 0 {
		return -1
	}
	return ans % 1_000_000_007
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(mn)$。

## 空间优化

原理见 64 题 [我的题解](https://leetcode.cn/problems/minimum-path-sum/solutions/3045828/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-zfb2/)。

```py [sol-Python3]
class Solution:
    def maxProductPath(self, grid: List[List[int]]) -> int:
        n = len(grid[0])
        f_min = [0] * n
        f_max = [0] * n

        for i, row in enumerate(grid):
            for j, x in enumerate(row):
                if i == j == 0:
                    f_min[0] = f_max[0] = x
                    continue

                res_min, res_max = inf, -inf
                if i > 0:
                    mn, mx = f_min[j], f_max[j]
                    res_min = min(mn * x, mx * x)
                    res_max = max(mn * x, mx * x)
                if j > 0:
                    mn, mx = f_min[j - 1], f_max[j - 1]
                    res_min = min(res_min, mn * x, mx * x)
                    res_max = max(res_max, mn * x, mx * x)

                f_min[j] = res_min
                f_max[j] = res_max

        ans = f_max[-1]
        return -1 if ans < 0 else ans % 1_000_000_007
```

```java [sol-Java]
class Solution {
    public int maxProductPath(int[][] grid) {
        int m = grid.length, n = grid[0].length;
        long[] fMin = new long[n];
        long[] fMax = new long[n];

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                long x = grid[i][j];
                if (i == 0 && j == 0) {
                    fMin[0] = fMax[0] = x;
                    continue;
                }

                long resMin = Long.MAX_VALUE;
                long resMax = Long.MIN_VALUE;
                if (i > 0) {
                    long mn = fMin[j], mx = fMax[j];
                    resMin = Math.min(mn * x, mx * x);
                    resMax = Math.max(mn * x, mx * x);
                }
                if (j > 0) {
                    long mn = fMin[j - 1], mx = fMax[j - 1];
                    resMin = Math.min(resMin, Math.min(mn * x, mx * x));
                    resMax = Math.max(resMax, Math.max(mn * x, mx * x));
                }

                fMin[j] = resMin;
                fMax[j] = resMax;
            }
        }

        long ans = fMax[n - 1];
        return ans < 0 ? -1 : (int) (ans % 1_000_000_007);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProductPath(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        vector<long long> f_min(n), f_max(n);

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                long long x = grid[i][j];
                if (i == 0 && j == 0) {
                    f_min[0] = f_max[0] = x;
                    continue;
                }

                long long res_min = LLONG_MAX;
                long long res_max = LLONG_MIN;
                if (i > 0) {
                    long long mn = f_min[j], mx = f_max[j];
                    res_min = min(mn * x, mx * x);
                    res_max = max(mn * x, mx * x);
                }
                if (j > 0) {
                    long long mn = f_min[j - 1], mx = f_max[j - 1];
                    res_min = min(res_min, min(mn * x, mx * x));
                    res_max = max(res_max, max(mn * x, mx * x));
                }

                f_min[j] = res_min;
                f_max[j] = res_max;
            }
        }

        long long ans = f_max[n - 1];
        return ans < 0 ? -1 : ans % 1'000'000'007;
    }
};
```

```go [sol-Go]
func maxProductPath(grid [][]int) int {
	n := len(grid[0])
	fMin := make([]int, n)
	fMax := make([]int, n)

	for i, row := range grid {
		for j, x := range row {
			if i == 0 && j == 0 {
				fMin[0], fMax[0] = x, x
				continue
			}

			resMin := math.MaxInt
			resMax := math.MinInt
			if i > 0 {
				mn, mx := fMin[j], fMax[j]
				resMin = min(mn*x, mx*x)
				resMax = max(mn*x, mx*x)
			}
			if j > 0 {
				mn, mx := fMin[j-1], fMax[j-1]
				resMin = min(resMin, mn*x, mx*x)
				resMax = max(resMax, mn*x, mx*x)
			}

			fMin[j] = resMin
			fMax[j] = resMax
		}
	}

	ans := fMax[n-1]
	if ans < 0 {
		return -1
	}
	return ans % 1_000_000_007
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面动态规划题单的「**二、网格图 DP**」。

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

本题来自 `二、网格图 DP / §2.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、网格图 DP / §2.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
