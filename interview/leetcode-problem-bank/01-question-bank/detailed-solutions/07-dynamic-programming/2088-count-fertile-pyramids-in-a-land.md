# 2088. 统计农场中肥沃金字塔的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-fertile-pyramids-in-a-land/
- 题目 slug：`count-fertile-pyramids-in-a-land`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.5 子矩形 DP
- 难度分：2105
- 外部题解来源：https://leetcode.cn/problems/count-fertile-pyramids-in-a-land/solutions/1125747/dong-tai-gui-hua-by-endlesscheng-depp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(mn) 动态规划](https://leetcode.cn/problems/count-fertile-pyramids-in-a-land/solutions/1125747/dong-tai-gui-hua-by-endlesscheng-depp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-by-endlesscheng-depp`
- topic id：`1125747`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

先求正金字塔。

定义 $\textit{dp}[i][j]$ 表示金字塔顶端位于 $(i,j)$ 时的最大层数（$1$ 层也算）。如果顶端在 $(i,j)$ 的金字塔最大能有 $x$ 层，那么顶端在 $(i,j)$ 的金字塔也可以有 $x-1,x-2,\cdots,1$ 层。由于要求区域内格子数目大于 $1$，统计答案的时候把 $1$ 层去掉，因此有 $x-1$ 个以 $(i,j)$ 为顶端的金字塔。

我们从 $\textit{grid}$ 的最后一行开始往上递推。转移的策略是在 $(i+1,j)$ 处的最大金字塔上套一层 **∧ 型的「外壳」**。具体来说，从 $(i,j)$ 出发向左下方向前进，求出能达到的最长连续 $1$ 的个数，根据 $\textit{dp}$ 的定义，这就是 $\textit{dp}[i+1][j-1]+1$；同理，向右下方向前进，能达到的最长连续 $1$ 的个数为 $\textit{dp}[i+1][j+1]+1$。取左右最长连续 $1$ 的个数的最小值即为「外壳」的高度。那么 $(i,j)$ 处的最大金字塔的高度为「外壳」的高度与 $(i+1,j)$ 处最大金字塔高度 $+1$ 的最小值。

即

$$
\textit{dp}[i][j] = \min(\textit{dp}[i+1][j-1]+1, \textit{dp}[i+1][j+1]+1, \textit{dp}[i+1][j]+1)
$$

倒金字塔可以将 $\textit{grid}$ 上下颠倒后再求一遍正金字塔即可。

```go
func countPyramids(grid [][]int) (ans int) {
	m, n := len(grid), len(grid[0])
	dp := make([][]int, m)
	for i := range dp {
		dp[i] = make([]int, n)
	}

	f := func() {
		dp[m-1] = grid[m-1]
		for i := m - 2; i >= 0; i-- {
			dp[i][0] = grid[i][0]
			dp[i][n-1] = grid[i][n-1]
			for j := 1; j < n-1; j++ {
				if grid[i][j] == 0 {
					dp[i][j] = 0
				} else {
					dp[i][j] = min(dp[i+1][j-1], dp[i+1][j+1], dp[i+1][j]) + 1
					ans += dp[i][j] - 1
				}
			}
		}
	}
	f() // 求正金字塔个数
	for i := 0; i < m/2; i++ { // 上下颠倒
		grid[i], grid[m-1-i] = grid[m-1-i], grid[i]
	}
	f() // 再求一遍正金字塔个数
	return
}
```

相似题目：

- [CF1393D - Rarity and New Dress](https://codeforces.com/problemset/problem/1393/D)

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.5 子矩形 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.5 子矩形 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
