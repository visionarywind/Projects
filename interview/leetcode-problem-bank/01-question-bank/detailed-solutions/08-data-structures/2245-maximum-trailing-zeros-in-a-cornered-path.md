# 2245. 转角路径的乘积中最多能有几个尾随零

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-trailing-zeros-in-a-cornered-path/
- 题目 slug：`maximum-trailing-zeros-in-a-cornered-path`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.5 进阶
- 难度分：2037
- 外部题解来源：https://leetcode.cn/problems/maximum-trailing-zeros-in-a-cornered-path/solutions/1427617/by-endlesscheng-7z5a/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和 + 从上往下/从下往上 + 左拐/右拐（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-trailing-zeros-in-a-cornered-path/solutions/1427617/by-endlesscheng-7z5a/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-7z5a`
- topic id：`1427617`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

尾零的个数就是路径上的数的因子 $2$ 的个数和，与因子 $5$ 的个数之和的较小值。
 
那么数越多越好，路径的起点和终点都应该在边界上。

预处理每一行的因子的前缀和，然后枚举所有的路径：

- 从上往下走，枚举左拐/右拐；
- 从下往上走，枚举左拐/右拐。

所有路径上的 $\min(s_2,s_5)$ 的最大值即为答案，这里 $s_2$ 为路径上的因子 $2$ 的个数之和，$s_5$ 为路径上的因子 $5$ 的个数之和。

```python [sol1-Python3]
c2, c5 = [0] * 1001, [0] * 1001
for i in range(2, 1001):  # 预处理：递推算出每个数的因子 2 的个数和因子 5 的个数
    if i % 2 == 0: c2[i] = c2[i // 2] + 1
    if i % 5 == 0: c5[i] = c5[i // 5] + 1

class Solution:
    def maxTrailingZeros(self, grid: List[List[int]]) -> int:
        m, n = len(grid), len(grid[0])
        s = [[None] * (n + 1) for _ in range(m)]
        for i, row in enumerate(grid):
            s[i][0] = (0, 0)
            for j, v in enumerate(row):  # 计算 grid 每行因子 2 和 5 的前缀和
                s[i][j + 1] = (s[i][j][0] + c2[v], s[i][j][1] + c5[v])

        ans = 0
        for j, col in enumerate(zip(*grid)):
            s2 = s5 = 0
            for i, v in enumerate(col):  # 从上往下，枚举左拐还是右拐
                s2 += c2[v]
                s5 += c5[v]
                ans = max(ans, min(s2 + s[i][j][0], s5 + s[i][j][1]),
                               min(s2 + s[i][n][0] - s[i][j + 1][0], s5 + s[i][n][1] - s[i][j + 1][1]))
            s2 = s5 = 0
            for i in range(m - 1, -1, -1):  # 从下往上，枚举左拐还是右拐
                s2 += c2[col[i]]
                s5 += c5[col[i]]
                ans = max(ans, min(s2 + s[i][j][0], s5 + s[i][j][1]),
                               min(s2 + s[i][n][0] - s[i][j + 1][0], s5 + s[i][n][1] - s[i][j + 1][1]))
        return ans
```

```java [sol1-Java]
class Solution {
    static int[][] c25 = new int[1001][2];

    static {
        // 预处理：递推算出每个数的因子 2 的个数和因子 5 的个数
        for (var i = 2; i <= 1000; i++) {
            if (i % 2 == 0) c25[i][0] = c25[i / 2][0] + 1;
            if (i % 5 == 0) c25[i][1] = c25[i / 5][1] + 1;
        }
    }

    public int maxTrailingZeros(int[][] grid) {
        int m = grid.length, n = grid[0].length, ans = 0;
        var s = new int[m][n + 1][2];
        for (var i = 0; i < m; i++)
            for (var j = 0; j < n; j++) {
                s[i][j + 1][0] = s[i][j][0] + c25[grid[i][j]][0]; // 每行的因子 2 的前缀和
                s[i][j + 1][1] = s[i][j][1] + c25[grid[i][j]][1]; // 每行的因子 5 的前缀和
            }

        for (var j = 0; j < n; j++) {
            for (int i = 0, s2 = 0, s5 = 0; i < m; i++) { // 从上往下，枚举左拐还是右拐
                s2 += c25[grid[i][j]][0];
                s5 += c25[grid[i][j]][1];
                ans = Math.max(ans, Math.max(Math.min(s2 + s[i][j][0], s5 + s[i][j][1]), 
                                             Math.min(s2 + s[i][n][0] - s[i][j + 1][0], s5 + s[i][n][1] - s[i][j + 1][1])));
            }
            for (int i = m - 1, s2 = 0, s5 = 0; i >= 0; i--) { // 从下往上，枚举左拐还是右拐
                s2 += c25[grid[i][j]][0];
                s5 += c25[grid[i][j]][1];
                ans = Math.max(ans, Math.max(Math.min(s2 + s[i][j][0], s5 + s[i][j][1]), 
                                             Math.min(s2 + s[i][n][0] - s[i][j + 1][0], s5 + s[i][n][1] - s[i][j + 1][1])));
            }
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
int c25[1001][2];
int init = []() {
    for (int i = 2; i <= 1000; ++i) { // 预处理：递推算出每个数的因子 2 的个数和因子 5 的个数
        if (i % 2 == 0) c25[i][0] = c25[i / 2][0] + 1;
        if (i % 5 == 0) c25[i][1] = c25[i / 5][1] + 1;
    }
    return 0;
}();

class Solution {
public:
    int maxTrailingZeros(vector<vector<int>> &grid) {
        int m = grid.size(), n = grid[0].size(), ans = 0;
        int s[m][n + 1][2];
        for (int i = 0; i < m; ++i) {
            s[i][0][0] = s[i][0][1] = 0;
            for (int j = 0; j < n; ++j) {
                s[i][j + 1][0] = s[i][j][0] + c25[grid[i][j]][0]; // 每行的因子 2 的前缀和
                s[i][j + 1][1] = s[i][j][1] + c25[grid[i][j]][1]; // 每行的因子 5 的前缀和
            }
        }

        for (int j = 0; j < n; ++j) {
            for (int i = 0, s2 = 0, s5 = 0; i < m; ++i) { // 从上往下，枚举左拐还是右拐
                s2 += c25[grid[i][j]][0];
                s5 += c25[grid[i][j]][1];
                ans = max(ans, max(min(s2 + s[i][j][0], s5 + s[i][j][1]),
                                   min(s2 + s[i][n][0] - s[i][j + 1][0], s5 + s[i][n][1] - s[i][j + 1][1])));
            }
            for (int i = m - 1, s2 = 0, s5 = 0; i >= 0; --i) { // 从下往上，枚举左拐还是右拐
                s2 += c25[grid[i][j]][0];
                s5 += c25[grid[i][j]][1];
                ans = max(ans, max(min(s2 + s[i][j][0], s5 + s[i][j][1]),
                                   min(s2 + s[i][n][0] - s[i][j + 1][0], s5 + s[i][n][1] - s[i][j + 1][1])));
            }
        }
        return ans;
    }
};
```

```go [sol1-Go]
var c25 [1001][2]int

func init() {
	// 预处理：递推算出每个数的因子 2 的个数和因子 5 的个数
	for i := 2; i <= 1000; i++ {
		if i%2 == 0 { c25[i][0] = c25[i/2][0] + 1 }
		if i%5 == 0 { c25[i][1] = c25[i/5][1] + 1 }
	}
}

func maxTrailingZeros(grid [][]int) (ans int) {
	m, n := len(grid), len(grid[0])
	s := make([][][2]int, m)
	for i, row := range grid {
		s[i] = make([][2]int, n+1)
		for j, v := range row {
			s[i][j+1][0] = s[i][j][0] + c25[v][0] // 每行的因子 2 的前缀和
			s[i][j+1][1] = s[i][j][1] + c25[v][1] // 每行的因子 5 的前缀和
		}
	}

	for j := 0; j < n; j++ {
		s2, s5 := 0, 0
		for i, row := range grid { // 从上往下，枚举左拐还是右拐
			s2 += c25[row[j]][0]
			s5 += c25[row[j]][1]
			ans = max(ans, max(min(s2+s[i][j][0], s5+s[i][j][1]), 
                               min(s2+s[i][n][0]-s[i][j+1][0], s5+s[i][n][1]-s[i][j+1][1])))
		}
		s2, s5 = 0, 0
		for i := m - 1; i >= 0; i-- { // 从下往上，枚举左拐还是右拐
			s2 += c25[grid[i][j]][0]
			s5 += c25[grid[i][j]][1]
			ans = max(ans, max(min(s2+s[i][j][0], s5+s[i][j][1]),
                               min(s2+s[i][n][0]-s[i][j+1][0], s5+s[i][n][1]-s[i][j+1][1])))
		}
	}
	return
}

func max(a, b int) int { if a < b { return b }; return a }
func min(a, b int) int { if a > b { return b }; return a }
```

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
