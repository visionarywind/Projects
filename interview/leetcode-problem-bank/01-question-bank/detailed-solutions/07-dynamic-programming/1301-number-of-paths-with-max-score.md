# 1301. 最大得分的路径数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-paths-with-max-score/
- 题目 slug：`number-of-paths-with-max-score`
- 来源专题：动态规划
- 来源分类路径：二、网格图 DP / §2.2 进阶
- 难度分：1853
- 外部题解来源：https://leetcode.cn/problems/number-of-paths-with-max-score/solutions/3986980/wang-ge-tu-dppythonjavacgo-by-endlessche-rzxm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[网格图 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-paths-with-max-score/solutions/3986980/wang-ge-tu-dppythonjavacgo-by-endlessche-rzxm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`wang-ge-tu-dppythonjavacgo-by-endlessche-rzxm`
- topic id：`3986980`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

**前置题目**：

- 如果只算路径和，则本题类似 [64. 最小路径和](https://leetcode.cn/problems/minimum-path-sum/)，[我的题解](https://leetcode.cn/problems/minimum-path-sum/solutions/3045828/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-zfb2/)。
- 如果只算方案数，则本题类似 [63. 不同路径 II](https://leetcode.cn/problems/unique-paths/)，[我的题解](https://leetcode.cn/problems/unique-paths-ii/solutions/3059862/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-451i/)。

本题是前置题目的综合题。

- 如果往左走的路径和，严格大于往上走的路径和，那么计算方案数时，只需考虑往左走的方案数。
- 如果往上走的路径和，严格大于往左走的路径和，那么计算方案数时，只需考虑往上走的方案数。
- 如果二者一样大，那么根据**加法原理**，要把两种方案数相加。

本题还可以往左上移动，要额外考虑往左上走的路径和以及方案数。

注意取模。为什么可以在**中途取模**？原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

## 优化前

```py [sol-Python3]
class Solution:
    def pathsWithMaxScore(self, board: List[str]) -> List[int]:
        MOD = 1_000_000_007
        m, n = len(board), len(board[0])
        max_sum = [[-inf] * (n + 1) for _ in range(m + 1)]  # 定义同 64 题（改成最大路径和）
        ways = [[0] * (n + 1) for _ in range(m + 1)]  # 定义同 63 题
        max_sum[0][0] = 0
        ways[0][0] = 1

        for i, row in enumerate(board):
            for j, ch in enumerate(row):
                if ch == 'X':
                    continue
                # 左上、正上、正左
                max_sum[i + 1][j + 1] = s = max(max_sum[i][j], max_sum[i][j + 1], max_sum[i + 1][j])
                # 如果路径和相同，则累加方案数（加法原理）
                if max_sum[i][j] == s:
                    ways[i + 1][j + 1] += ways[i][j]
                if max_sum[i][j + 1] == s:
                    ways[i + 1][j + 1] += ways[i][j + 1]
                if max_sum[i + 1][j] == s:
                    ways[i + 1][j + 1] += ways[i + 1][j]
                ways[i + 1][j + 1] %= MOD
                if ch.isdigit():
                    max_sum[i + 1][j + 1] += int(ch)  # 加上当前格子的值

        return [max_sum[m][n], ways[m][n]] if max_sum[m][n] != -inf else [0, 0]
```

```java [sol-Java]
class Solution {
    public int[] pathsWithMaxScore(List<String> board) {
        final int MOD = 1_000_000_007;
        int m = board.size();
        int n = board.get(0).length();
        int[][] maxSum = new int[m + 1][n + 1]; // 定义同 64 题（改成最大路径和）
        int[][] ways = new int[m + 1][n + 1]; // 定义同 63 题
        for (int[] row : maxSum) {
            Arrays.fill(row, Integer.MIN_VALUE);
        }
        maxSum[0][0] = 0;
        ways[0][0] = 1;

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                char ch = board.get(i).charAt(j);
                if (ch == 'X') {
                    continue;
                }
                // 左上、正上、正左
                maxSum[i + 1][j + 1] = Math.max(Math.max(maxSum[i][j], maxSum[i][j + 1]), maxSum[i + 1][j]);
                int s = maxSum[i + 1][j + 1];
                long w = 0;
                // 如果路径和相同，则累加方案数（加法原理）
                if (maxSum[i][j] == s) {
                    w += ways[i][j];
                }
                if (maxSum[i][j + 1] == s) {
                    w += ways[i][j + 1];
                }
                if (maxSum[i + 1][j] == s) {
                    w += ways[i + 1][j];
                }
                ways[i + 1][j + 1] = (int) (w % MOD);
                if ('1' <= ch && ch <= '9') {
                    maxSum[i + 1][j + 1] += ch - '0'; // 加上当前格子的值
                }
            }
        }

        return maxSum[m][n] < 0 ? new int[]{0, 0} : new int[]{maxSum[m][n], ways[m][n]};
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> pathsWithMaxScore(vector<string>& board) {
        constexpr int MOD = 1'000'000'007;
        int m = board.size(), n = board[0].size();
        vector max_sum(m + 1, vector<int>(n + 1, INT_MIN)); // 定义同 64 题（改成最大路径和）
        vector ways(m + 1, vector<int>(n + 1)); // 定义同 63 题
        max_sum[0][0] = 0;
        ways[0][0] = 1;

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                char ch = board[i][j];
                if (ch == 'X') {
                    continue;
                }
                // 左上、正上、正左
                max_sum[i + 1][j + 1] = max({max_sum[i][j], max_sum[i][j + 1], max_sum[i + 1][j]});
                int s = max_sum[i + 1][j + 1];
                long long w = 0;
                // 如果路径和相同，则累加方案数（加法原理）
                if (max_sum[i][j] == s) {
                    w += ways[i][j];
                }
                if (max_sum[i][j + 1] == s) {
                    w += ways[i][j + 1];
                }
                if (max_sum[i + 1][j] == s) {
                    w += ways[i + 1][j];
                }
                ways[i + 1][j + 1] = w % MOD;
                if ('1' <= ch && ch <= '9') {
                    max_sum[i + 1][j + 1] += ch - '0'; // 加上当前格子的值
                }
            }
        }

        return max_sum[m][n] < 0 ? vector{0, 0} : vector{max_sum[m][n], ways[m][n]};
    }
};
```

```go [sol-Go]
func pathsWithMaxScore(board []string) []int {
	const mod = 1_000_000_007
	m, n := len(board), len(board[0])
	maxSum := make([][]int, m+1) // 定义同 64 题（改成最大路径和）
	ways := make([][]int, m+1)   // 定义同 63 题
	for i := range maxSum {
		maxSum[i] = make([]int, n+1)
		for j := range maxSum[i] {
			maxSum[i][j] = math.MinInt
		}
		ways[i] = make([]int, n+1)
	}
	maxSum[0][0] = 0
	ways[0][0] = 1

	for i, row := range board {
		for j, ch := range row {
			if ch == 'X' {
				continue
			}
			// 左上、正上、正左
			maxSum[i+1][j+1] = max(maxSum[i][j], maxSum[i][j+1], maxSum[i+1][j])
			s := maxSum[i+1][j+1]
			w := 0
			// 如果路径和相同，则累加方案数（加法原理）
			if maxSum[i][j] == s {
				w += ways[i][j]
			}
			if maxSum[i][j+1] == s {
				w += ways[i][j+1]
			}
			if maxSum[i+1][j] == s {
				w += ways[i+1][j]
			}
			ways[i+1][j+1] = w % mod
			if '1' <= ch && ch <= '9' {
				maxSum[i+1][j+1] += int(ch - '0') // 加上当前格子的值
			}
		}
	}

	if maxSum[m][n] < 0 {
		return []int{0, 0}
	}
	return []int{maxSum[m][n], ways[m][n]}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{board}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(mn)$。

## 空间优化

```py [sol-Python3]
class Solution:
    def pathsWithMaxScore(self, board: List[str]) -> List[int]:
        MOD = 1_000_000_007
        n = len(board[0])
        max_sum = [0] + [-inf] * n  # 定义同 64 题（改成最大路径和）
        ways = [1] + [0] * n  # 定义同 63 题

        for row in board:
            # 计算 max_sum[i+1][j+1] 时，pre_s 表示 max_sum[i][j]，pre_w 表示 ways[i][j]
            pre_s, pre_w = max_sum[0], ways[0]
            max_sum[0], ways[0] = -inf, 0
            for j, ch in enumerate(row):
                if ch == 'X':
                    pre_s, pre_w = max_sum[j + 1], ways[j + 1]
                    max_sum[j + 1], ways[j + 1] = -inf, 0
                    continue
                tmp_s, tmp_w = max_sum[j + 1], ways[j + 1]
                # 左上、正上、正左
                s = max(pre_s, max_sum[j + 1], max_sum[j])
                # 如果路径和相同，则累加方案数（加法原理）
                w = 0
                if pre_s == s:
                    w += pre_w
                if max_sum[j + 1] == s:
                    w += ways[j + 1]
                if max_sum[j] == s:
                    w += ways[j]
                ways[j + 1] = w % MOD
                max_sum[j + 1] = s
                if ch.isdigit():
                    max_sum[j + 1] += int(ch)  # 加上当前格子的值
                pre_s, pre_w = tmp_s, tmp_w

        return [max_sum[n], ways[n]] if max_sum[n] != -inf else [0, 0]
```

```java [sol-Java]
class Solution {
    public int[] pathsWithMaxScore(List<String> board) {
        final int MOD = 1_000_000_007;
        int n = board.get(0).length();
        int[] maxSum = new int[n + 1]; // 定义同 64 题（改成最大路径和）
        int[] ways = new int[n + 1]; // 定义同 63 题
        Arrays.fill(maxSum, Integer.MIN_VALUE);
        maxSum[0] = 0;
        ways[0] = 1;

        for (String row : board) {
            // 计算 maxSum[i+1][j+1] 时，preS 表示 maxSum[i][j]，preW 表示 ways[i][j]
            int preS = maxSum[0];
            int preW = ways[0];
            maxSum[0] = Integer.MIN_VALUE;
            ways[0] = 0;
            for (int j = 0; j < n; j++) {
                char ch = row.charAt(j);
                if (ch == 'X') {
                    preS = maxSum[j + 1];
                    preW = ways[j + 1];
                    maxSum[j + 1] = Integer.MIN_VALUE;
                    ways[j + 1] = 0;
                    continue;
                }
                int tmpS = maxSum[j + 1];
                int tmpW = ways[j + 1];
                // 左上、正上、正左
                int s = Math.max(Math.max(preS, maxSum[j + 1]), maxSum[j]);
                // 如果路径和相同，则累加方案数（加法原理）
                long w = 0;
                if (preS == s) {
                    w += preW;
                }
                if (maxSum[j + 1] == s) {
                    w += ways[j + 1];
                }
                if (maxSum[j] == s) {
                    w += ways[j];
                }
                ways[j + 1] = (int) (w % MOD);
                maxSum[j + 1] = s;
                if ('1' <= ch && ch <= '9') {
                    maxSum[j + 1] += ch - '0'; // 加上当前格子的值
                }
                preS = tmpS;
                preW = tmpW;
            }
        }

        return maxSum[n] < 0 ? new int[]{0, 0} : new int[]{maxSum[n], ways[n]};
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> pathsWithMaxScore(vector<string>& board) {
        constexpr int MOD = 1'000'000'007;
        int n = board[0].size();
        vector<int> max_sum(n + 1, INT_MIN); // 定义同 64 题（改成最大路径和）
        vector<int> ways(n + 1); // 定义同 63 题
        max_sum[0] = 0;
        ways[0] = 1;

        for (auto& row : board) {
            // 计算 max_sum[i+1][j+1] 时，pre_s 表示 max_sum[i][j]，pre_w 表示 ways[i][j]
            int pre_s = max_sum[0];
            int pre_w = ways[0];
            max_sum[0] = INT_MIN;
            ways[0] = 0;
            for (int j = 0; j < n; j++) {
                char ch = row[j];
                if (ch == 'X') {
                    pre_s = max_sum[j + 1];
                    pre_w = ways[j + 1];
                    max_sum[j + 1] = INT_MIN;
                    ways[j + 1] = 0;
                    continue;
                }
                int tmp_s = max_sum[j + 1];
                int tmp_w = ways[j + 1];
                // 左上、正上、正左
                int s = max({pre_s, max_sum[j + 1], max_sum[j]});
                // 如果路径和相同，则累加方案数（加法原理）
                long long w = 0;
                if (pre_s == s) {
                    w += pre_w;
                }
                if (max_sum[j + 1] == s) {
                    w += ways[j + 1];
                }
                if (max_sum[j] == s) {
                    w += ways[j];
                }
                ways[j + 1] = w % MOD;
                max_sum[j + 1] = s;
                if ('1' <= ch && ch <= '9') {
                    max_sum[j + 1] += ch - '0'; // 加上当前格子的值
                }
                pre_s = tmp_s;
                pre_w = tmp_w;
            }
        }

        return max_sum[n] < 0 ? vector{0, 0} : vector{max_sum[n], ways[n]};
    }
};
```

```go [sol-Go]
func pathsWithMaxScore(board []string) []int {
	const mod = 1_000_000_007
	n := len(board[0])
	maxSum := make([]int, n+1) // 定义同 64 题（改成最大路径和）
	for i := 1; i <= n; i++ {
		maxSum[i] = math.MinInt
	}
	ways := make([]int, n+1) // 定义同 63 题
	ways[0] = 1

	for _, row := range board {
		// 计算 maxSum[i+1][j+1] 时，preS 表示 maxSum[i][j]，preW 表示 ways[i][j]
		preS, preW := maxSum[0], ways[0]
		maxSum[0], ways[0] = math.MinInt, 0
		for j, ch := range row {
			if ch == 'X' {
				preS, preW = maxSum[j+1], ways[j+1]
				maxSum[j+1], ways[j+1] = math.MinInt, 0
				continue
			}
			tmpS, tmpW := maxSum[j+1], ways[j+1]
			// 左上、正上、正左
			s := max(preS, maxSum[j+1], maxSum[j])
			// 如果路径和相同，则累加方案数（加法原理）
			w := 0
			if preS == s {
				w += preW
			}
			if maxSum[j+1] == s {
				w += ways[j+1]
			}
			if maxSum[j] == s {
				w += ways[j]
			}
			ways[j+1] = w % mod
			maxSum[j+1] = s
			if '1' <= ch && ch <= '9' {
				maxSum[j+1] += int(ch - '0') // 加上当前格子的值
			}
			preS, preW = tmpS, tmpW
		}
	}

	if maxSum[n] < 0 {
		return []int{0, 0}
	}
	return []int{maxSum[n], ways[n]}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$，其中 $m$ 和 $n$ 分别是 $\textit{board}$ 的行数和列数。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

[1976. 到达目的地的方案数](https://leetcode.cn/problems/number-of-ways-to-arrive-at-destination/)

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
