# 3938. 矩阵中最大共享路径和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-path-intersection-sum-in-a-grid/
- 题目 slug：`maximum-path-intersection-sum-in-a-grid`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.3 最大子数组和（最大子段和）
- 难度分：2252
- 外部题解来源：https://leetcode.cn/problems/maximum-path-intersection-sum-in-a-grid/solutions/3973275/zui-da-zi-shu-zu-he-pythonjavacgo-by-end-f0xr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[最大子数组和（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-path-intersection-sum-in-a-grid/solutions/3973275/zui-da-zi-shu-zu-he-pythonjavacgo-by-end-f0xr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zui-da-zi-shu-zu-he-pythonjavacgo-by-end-f0xr`
- topic id：`3973275`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

两人的移动路径，可以分别视作一个减函数和一个增函数（非严格）。所以交集要么是水平的一段，要么是垂直的一段。

所以问题等价于每行每列的 [53. 最大子数组和](https://leetcode.cn/problems/maximum-subarray/)，[我的题解](https://leetcode.cn/problems/maximum-subarray/solutions/2533977/qian-zhui-he-zuo-fa-ben-zhi-shi-mai-mai-abu71/)。

需要特别注意子数组长度为 $1$ 的情况，两人在交点处的移动路径只能形如佛教符号（如下图）。这意味着，交点不能在 $\textit{grid}$ 的边界上。所以边界上的子数组长度至少为 $2$（由示例 2 可知，子数组长度为 $2$ 的情况是存在的）。

![lc3938.png](https://pic.leetcode.cn/1779581072-bPmSGA-lc3938.png){:width=50px}

我们可以先计算不在边界上的 $\textit{grid}[i][j]$ 的最大值，然后就只需考虑子数组长度至少为 $2$ 的情况了，无需特判边界。

长度至少为 $2$ 的最大子数组和，可以用**前缀和**或者 **DP** 解决。

我们可以在 [DP 做法](https://leetcode.cn/problems/maximum-subarray/solutions/2533977/qian-zhui-he-zuo-fa-ben-zhi-shi-mai-mai-abu71/) 上略作修改，先计算 `ans = max(ans, f + nums[i])`，再更新 `f = max(f, 0) + nums[i]`，这样 `f + nums[i]` 就可以保证子数组至少有两个数了。

[本题视频讲解](https://www.bilibili.com/video/BV1iuG76VEXy/?t=15m42s)，欢迎点赞关注~

```py [sol-Python3]
# 手写 max 更快
fmax = lambda a, b: b if b > a else a

class Solution:
    # 53. 最大子数组和（子数组长度 >= 2）
    def maxSubArray(self, nums: list[int]) -> int:
        ans = -inf  # 注意答案可以是负数，不能初始化成 0
        f = nums[0]
        for x in nums[1:]:
            ans = fmax(ans, f + x)  # f+x 保证子数组至少有两个数
            f = fmax(f, 0) + x
        return ans

    def maxScore(self, grid: list[list[int]]) -> int:
        ans = -inf

        # 单独计算子数组长为 1 的情况，此时子数组不能在 grid 的边界上
        if len(grid) > 2 and len(grid[0]) > 2:
            ans = max(max(row[1: -1]) for row in grid[1: -1])

        # 每行的最大子数组和（子数组长度 >= 2）
        for row in grid:
            ans = fmax(ans, self.maxSubArray(row))

        # 每列的最大子数组和（子数组长度 >= 2）
        for col in zip(*grid):
            ans = fmax(ans, self.maxSubArray(list(col)))

        return ans
```

```java [sol-Java]
class Solution {
    public int maxScore(int[][] grid) {
        int m = grid.length;
        int n = grid[0].length;
        int ans = Integer.MIN_VALUE;

        // 单独计算子数组长为 1 的情况，此时子数组不能在 grid 的边界上
        for (int i = 1; i < m - 1; i++) {
            for (int j = 1; j < n - 1; j++) {
                ans = Math.max(ans, grid[i][j]);
            }
        }

        // 每行的最大子数组和（子数组长度 >= 2）
        for (int[] row : grid) {
            ans = Math.max(ans, maxSubArray(row));
        }

        // 每列的最大子数组和（子数组长度 >= 2）
        int[] col = new int[m];
        for (int j = 0; j < n; j++) {
            for (int i = 0; i < m; i++) {
                col[i] = grid[i][j];
            }
            ans = Math.max(ans, maxSubArray(col));
        }

        return ans;
    }

    // 53. 最大子数组和（子数组长度 >= 2）
    private int maxSubArray(int[] nums) {
        int ans = Integer.MIN_VALUE; // 注意答案可以是负数，不能初始化成 0
        int f = nums[0];
        for (int i = 1; i < nums.length; i++) {
            int x = nums[i];
            ans = Math.max(ans, f + x); // f+x 保证子数组至少有两个数
            f = Math.max(f, 0) + x;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 53. 最大子数组和（子数组长度 >= 2）
    int maxSubArray(vector<int>& nums) {
        int ans = INT_MIN; // 注意答案可以是负数，不能初始化成 0
        int f = nums[0];
        for (int i = 1; i < nums.size(); i++) {
            int x = nums[i];
            ans = max(ans, f + x); // f+x 保证子数组至少有两个数
            f = max(f, 0) + x;
        }
        return ans;
    }

public:
    int maxScore(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        int ans = INT_MIN;

        // 单独计算子数组长为 1 的情况，此时子数组不能在 grid 的边界上
        for (int i = 1; i < m - 1; i++) {
            for (int j = 1; j < n - 1; j++) {
                ans = max(ans, grid[i][j]);
            }
        }

        // 每行的最大子数组和（子数组长度 >= 2）
        for (auto& row : grid) {
            ans = max(ans, maxSubArray(row));
        }

        // 每列的最大子数组和（子数组长度 >= 2）
        vector<int> col(m);
        for (int j = 0; j < n; j++) {
            for (int i = 0; i < m; i++) {
                col[i] = grid[i][j];
            }
            ans = max(ans, maxSubArray(col));
        }

        return ans;
    }
};
```

```go [sol-Go]
// 53. 最大子数组和（子数组长度 >= 2）
func maxSubArray(nums []int) int {
	ans := math.MinInt // 注意答案可以是负数，不能初始化成 0
	f := nums[0]
	for _, x := range nums[1:] {
		ans = max(ans, f+x) // f+x 保证子数组至少有两个数
		f = max(f, 0) + x
	}
	return ans
}

func maxScore(grid [][]int) int {
	m, n := len(grid), len(grid[0])
	ans := math.MinInt

	// 单独计算子数组长为 1 的情况，此时子数组不能在 grid 的边界上
	if m > 2 && n > 2 {
		for _, row := range grid[1 : m-1] {
			ans = max(ans, slices.Max(row[1:n-1]))
		}
	}

	// 每行的最大子数组和（子数组长度 >= 2）
	for _, row := range grid {
		ans = max(ans, maxSubArray(row))
	}

	// 每列的最大子数组和（子数组长度 >= 2）
	col := make([]int, m)
	for j := range n {
		for i, row := range grid {
			col[i] = row[j]
		}
		ans = max(ans, maxSubArray(col))
	}

	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{grid}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(m)$ 或 $\mathcal{O}(1)$，取决于是否使用辅助数组。

## 专题训练

见下面动态规划题单的「**§1.3 最大子数组和**」。

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

本题来自 `一、入门 DP / §1.3 最大子数组和（最大子段和）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、入门 DP / §1.3 最大子数组和（最大子段和）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
