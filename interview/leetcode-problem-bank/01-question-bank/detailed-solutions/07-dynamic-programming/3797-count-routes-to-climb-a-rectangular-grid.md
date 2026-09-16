# 3797. 统计在矩形格子里移动的路径数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-routes-to-climb-a-rectangular-grid/
- 题目 slug：`count-routes-to-climb-a-rectangular-grid`
- 来源专题：动态规划
- 来源分类路径：十一、优化 DP / §11.1 前缀和优化 DP
- 难度分：2376
- 外部题解来源：https://leetcode.cn/problems/count-routes-to-climb-a-rectangular-grid/solutions/3872180/qian-zhui-he-you-hua-dppythonjavacgo-by-w3ve2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和优化 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/count-routes-to-climb-a-rectangular-grid/solutions/3872180/qian-zhui-he-you-hua-dppythonjavacgo-by-w3ve2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-you-hua-dppythonjavacgo-by-w3ve2`
- topic id：`3872180`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

为方便计算，考虑从上往下移动。

在移动的过程中，我们需要知道：

- 当前位置 $(i,j)$。
- 上一步的状态：是在 $i-1$ 行，还是在 $i$ 行？

定义 $f[i][j]$ 表示从 $0$ 行移动到 $(i,j)$，且上一步在 $i-1$ 行的方案数。

定义 $g[i][j]$ 表示从 $0$ 行移动到 $(i,j)$，且上一步也在 $i$ 行的方案数。

从 $0$ 行移动到 $(i,j)$ 的方案数为

$$
f[i][j] + g[i][j]
$$

如果 $(i,j)$ 被阻塞，那么上式为 $0$。

设上一步在 $(i-1,k)$，那么必须满足

$$
(k-j)^2 + 1 \le d^2
$$

即 

$$
|k-j|\le \left\lfloor \sqrt {d^2-1} \right\rfloor = d-1
$$

> 注：题目保证 $d\ge 1$，所以 $(d-1)^2 = d^2-2d+1\le d^2-1$。

对于 $f[i][j]$，枚举上一步在 $(i-1,k)$，从 $0$ 行移动到 $(i-1,k)$ 的方案数为

$$
f[i-1][k] + g[i-1][k]
$$

累加得

$$
f[i][j] = \sum_{k=j-d+1}^{j+d-1} f[i-1][k] + g[i-1][k]
$$

上式可以用 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/) 优化。

对于 $g[i][j]$，枚举上一步在 $(i,k)$，那么有

$$
g[i][j] = \left(\sum_{k=j-d}^{j+d} f[i][k]\right) - f[i][j]
$$

注意不能原地不动，所以要减去 $f[i][j]$。

上式也可以用 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/) 优化。

初始值：$f[0][j] = 1$。

答案：$\displaystyle\sum_{j=0}^{m-1} f[n-1][j] + g[n-1][j]$。

[本题视频讲解](https://www.bilibili.com/video/BV1mUijBnEda/?t=20m25s)，欢迎点赞关注~

## 写法一

```py [sol-Python3]
class Solution:
    def numberOfRoutes(self, grid: List[str], d: int) -> int:
        MOD = 1_000_000_007
        m = len(grid[0])
        sum_f = [0] * (m + 1)
        sum_fg = [0] * (m + 1)

        for i, row in enumerate(grid):
            # 从 i-1 行移动到 i 行的方案数
            f = [0] * m
            for j, ch in enumerate(row):
                if ch == '#':
                    continue
                if i == 0:  # 第一行（起点）
                    f[j] = 1  # DP 初始值
                else:
                    f[j] = sum_fg[min(j + d, m)] - sum_fg[max(j - d + 1, 0)]

            # f 的前缀和
            for j, v in enumerate(f):
                sum_f[j + 1] = (sum_f[j] + v) % MOD

            # 从 i 行移动到 i 行的方案数
            g = [0] * m
            for j, ch in enumerate(row):
                if ch == '#':
                    continue
                # 不能原地不动，减去 f[j]
                g[j] = sum_f[min(j + d + 1, m)] - sum_f[max(j - d, 0)] - f[j]

            # f[j] + g[j] 的前缀和
            for j, (fj, gj) in enumerate(zip(f, g)):
                sum_fg[j + 1] = (sum_fg[j] + fj + gj) % MOD

        return sum_fg[m]
```

```java [sol-Java]
class Solution {
    public int numberOfRoutes(String[] grid, int d) {
        final int MOD = 1_000_000_007;
        int m = grid[0].length();
        long[] sumF = new long[m + 1];
        long[] sum = new long[m + 1];

        for (int i = 0; i < grid.length; i++) {
            char[] row = grid[i].toCharArray();
            // 从 i-1 行移动到 i 行的方案数
            long[] f = new long[m];
            for (int j = 0; j < m; j++) {
                if (row[j] == '#') {
                    continue;
                }
                if (i == 0) { // 第一行（起点）
                    f[j] = 1; // DP 初始值
                } else {
                    f[j] = sum[Math.min(j + d, m)] - sum[Math.max(j - d + 1, 0)];
                }
            }

            // f 的前缀和
            for (int j = 0; j < m; j++) {
                sumF[j + 1] = (sumF[j] + f[j]) % MOD;
            }

            // 从 i 行移动到 i 行的方案数
            long[] g = new long[m];
            for (int j = 0; j < m; j++) {
                if (row[j] == '#') {
                    continue;
                }
                // 不能原地不动，减去 f[j]
                g[j] = sumF[Math.min(j + d + 1, m)] - sumF[Math.max(j - d, 0)] - f[j];
            }

            // f[j] + g[j] 的前缀和
            for (int j = 0; j < m; j++) {
                sum[j + 1] = (sum[j] + f[j] + g[j]) % MOD;
            }
        }

        return (int) ((sum[m] + MOD) % MOD); // +MOD 保证结果非负
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numberOfRoutes(vector<string>& grid, int d) {
        constexpr int MOD = 1'000'000'007;
        int m = grid[0].size();
        vector<long long> sum_f(m + 1);
        vector<long long> sum(m + 1);

        for (int i = 0; i < grid.size(); i++) {
            auto& row = grid[i];
            // 从 i-1 行移动到 i 行的方案数
            vector<long long> f(m);
            for (int j = 0; j < m; j++) {
                if (row[j] == '#') {
                    continue;
                }
                if (i == 0) { // 第一行（起点）
                    f[j] = 1; // DP 初始值
                } else {
                    f[j] = sum[min(j + d, m)] - sum[max(j - d + 1, 0)];
                }
            }

            // f 的前缀和
            for (int j = 0; j < m; j++) {
                sum_f[j + 1] = (sum_f[j] + f[j]) % MOD;
            }

            // 从 i 行移动到 i 行的方案数
            vector<long long> g(m);
            for (int j = 0; j < m; j++) {
                if (row[j] == '#') {
                    continue;
                }
                // 不能原地不动，减去 f[j]
                g[j] = sum_f[min(j + d + 1, m)] - sum_f[max(j - d, 0)] - f[j];
            }

            // f[j] + g[j] 的前缀和
            for (int j = 0; j < m; j++) {
                sum[j + 1] = (sum[j] + f[j] + g[j]) % MOD;
            }
        }

        return (sum[m] + MOD) % MOD; // +MOD 保证结果非负
    }
};
```

```go [sol-Go]
func numberOfRoutes(grid []string, d int) int {
	const mod = 1_000_000_007
	m := len(grid[0])
	sumF := make([]int, m+1)
	sum := make([]int, m+1)

	for i, row := range grid {
		// 从 i-1 行移动到 i 行的方案数
		f := make([]int, m)
		for j, ch := range row {
			if ch == '#' {
				continue
			}
			if i == 0 { // 第一行（起点）
				f[j] = 1 // DP 初始值
			} else {
				f[j] = sum[min(j+d, m)] - sum[max(j-d+1, 0)]
			}
		}

		// f 的前缀和
		for j, v := range f {
			sumF[j+1] = (sumF[j] + v) % mod
		}

		// 从 i 行移动到 i 行的方案数
		g := make([]int, m)
		for j, ch := range row {
			if ch == '#' {
				continue
			}
			// 不能原地不动，减去 f[j]
			g[j] = sumF[min(j+d+1, m)] - sumF[max(j-d, 0)] - f[j]
		}

		// f[j] + g[j] 的前缀和
		for j, fj := range f {
			sum[j+1] = (sum[j] + fj + g[j]) % mod
		}
	}

	return (sum[m] + mod) % mod // +mod 保证结果非负
}
```

## 写法二（进一步优化）

计算 $f$ 的同时，计算 $f$ 的前缀和。

计算 $g$ 的同时，计算 $f[j]+g[j]$ 的前缀和。

```py [sol-Python3]
# 手写 min max 更快
min = lambda a, b: b if b < a else a
max = lambda a, b: b if b > a else a

class Solution:
    def numberOfRoutes(self, grid: List[str], d: int) -> int:
        MOD = 1_000_000_007
        m = len(grid[0])
        sum_f = [0] * (m + 1)
        sum_fg = [0] * (m + 1)

        for i, row in enumerate(grid):
            # f 的前缀和
            for j, ch in enumerate(row):
                if ch == '#':
                    sum_f[j + 1] = sum_f[j]
                elif i == 0:  # 第一行（起点）
                    sum_f[j + 1] = sum_f[j] + 1  # DP 初始值
                else:
                    sum_f[j + 1] = (sum_f[j] + sum_fg[min(j + d, m)] - sum_fg[max(j - d + 1, 0)]) % MOD

            # f[j] + g[j] 的前缀和
            for j, ch in enumerate(row):
                if ch == '#':
                    sum_fg[j + 1] = sum_fg[j]
                else:
                    # -f[j] 和 +f[j] 抵消了
                    sum_fg[j + 1] = (sum_fg[j] + sum_f[min(j + d + 1, m)] - sum_f[max(j - d, 0)]) % MOD

        return sum_fg[m]
```

```java [sol-Java]
class Solution {
    public int numberOfRoutes(String[] grid, int d) {
        final int MOD = 1_000_000_007;
        int m = grid[0].length();
        long[] sumF = new long[m + 1];
        long[] sum = new long[m + 1];

        for (int i = 0; i < grid.length; i++) {
            char[] row = grid[i].toCharArray();
            // f 的前缀和
            for (int j = 0; j < m; j++) {
                if (row[j] == '#') {
                    sumF[j + 1] = sumF[j];
                } else if (i == 0) { // 第一行（起点）
                    sumF[j + 1] = sumF[j] + 1; // DP 初始值
                } else {
                    sumF[j + 1] = (sumF[j] + sum[Math.min(j + d, m)] - sum[Math.max(j - d + 1, 0)]) % MOD;
                }
            }

            // f[j] + g[j] 的前缀和
            for (int j = 0; j < m; j++) {
                if (row[j] == '#') {
                    sum[j + 1] = sum[j];
                } else {
                    // -f[j] 和 +f[j] 抵消了
                    sum[j + 1] = (sum[j] + sumF[Math.min(j + d + 1, m)] - sumF[Math.max(j - d, 0)]) % MOD;
                }
            }
        }

        return (int) ((sum[m] + MOD) % MOD); // +MOD 保证结果非负
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numberOfRoutes(vector<string>& grid, int d) {
        constexpr int MOD = 1'000'000'007;
        int m = grid[0].size();
        vector<long long> sum_f(m + 1);
        vector<long long> sum(m + 1);

        for (int i = 0; i < grid.size(); i++) {
            auto& row = grid[i];
            // f 的前缀和
            for (int j = 0; j < m; j++) {
                if (row[j] == '#') {
                    sum_f[j + 1] = sum_f[j];
                } else if (i == 0) { // 第一行（起点）
                    sum_f[j + 1] = sum_f[j] + 1; // DP 初始值
                } else {
                    sum_f[j + 1] = (sum_f[j] + sum[min(j + d, m)] - sum[max(j - d + 1, 0)]) % MOD;
                }
            }

            // f[j] + g[j] 的前缀和
            for (int j = 0; j < m; j++) {
                if (row[j] == '#') {
                    sum[j + 1] = sum[j];
                } else {
                    // -f[j] 和 +f[j] 抵消了
                    sum[j + 1] = (sum[j] + sum_f[min(j + d + 1, m)] - sum_f[max(j - d, 0)]) % MOD;
                }
            }
        }

        return (sum[m] + MOD) % MOD; // +MOD 保证结果非负
    }
};
```

```go [sol-Go]
func numberOfRoutes(grid []string, d int) int {
	const mod = 1_000_000_007
	m := len(grid[0])
	sumF := make([]int, m+1)
	sum := make([]int, m+1)

	for i, row := range grid {
		// f 的前缀和
		for j, ch := range row {
			if ch == '#' {
				sumF[j+1] = sumF[j]
			} else if i == 0 { // 第一行（起点）
				sumF[j+1] = sumF[j] + 1 // DP 初始值
			} else {
				sumF[j+1] = (sumF[j] + sum[min(j+d, m)] - sum[max(j-d+1, 0)]) % mod
			}
		}

		// f[j] + g[j] 的前缀和
		for j, ch := range row {
			if ch == '#' {
				sum[j+1] = sum[j]
			} else {
				// -f[j] 和 +f[j] 抵消了
				sum[j+1] = (sum[j] + sumF[min(j+d+1, m)] - sumF[max(j-d, 0)]) % mod
			}
		}
	}

	return (sum[m] + mod) % mod // +mod 保证结果非负
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 和 $m$ 分别是 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(m)$。

## 专题训练

见下面动态规划题单的「**§11.1 前缀和优化 DP**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `十一、优化 DP / §11.1 前缀和优化 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十一、优化 DP / §11.1 前缀和优化 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
