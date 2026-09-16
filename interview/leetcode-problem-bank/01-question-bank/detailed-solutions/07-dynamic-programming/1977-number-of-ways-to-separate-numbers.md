# 1977. 划分数字的方案数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-ways-to-separate-numbers/
- 题目 slug：`number-of-ways-to-separate-numbers`
- 来源专题：动态规划
- 来源分类路径：十一、优化 DP / §11.1 前缀和优化 DP
- 难度分：2817
- 外部题解来源：https://leetcode.cn/problems/number-of-ways-to-separate-numbers/solutions/950429/yu-chu-li-dong-tai-gui-hua-by-endlessche-7am2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[预处理 + 动态规划](https://leetcode.cn/problems/number-of-ways-to-separate-numbers/solutions/950429/yu-chu-li-dong-tai-gui-hua-by-endlessche-7am2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yu-chu-li-dong-tai-gui-hua-by-endlessche-7am2`
- topic id：`950429`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

定义 $f[i][j]$ 表示 $\textit{num}$ 的前 $j$ 个字符划分出的最后一个整数的起始位置为 $i$ 时的方案数。

我们所求的答案即为 $\sum\limits_{i=0}^{n-1} f[i][n-1]$。

定义 $\textit{lcp}[i][j]$ 表示后缀 $\textit{num}[i:]$ 和后缀 $\textit{num}[j:]$ 的最长公共前缀的长度。

对于倒数第二个划分出的整数，记其起始位置为 $k$，结束位置为 $i-1$。我们可以通过比较倒数第二个划分出的整数和最后一个划分出的整数的大小，来计算状态转移，也就是在满足题目要求时，将 $f[k][i-1]$ 加到 $f[i][j]$ 上。

具体地，我们按倒数第二个划分出的整数的长度分类讨论：

- 若其长度小于最后一个划分出的整数，则可以将其方案数加到 $f[i][j]$ 上，即 $f[i][j] += \sum f[k][i-1]$，这里 $i-k<j-i+1$；在代码实现时，我们可以从 $i$ 出发，向左向右同时扩展 $k$ 和 $j$，并累加 $f[k][i-1]$；
- 若其长度等于最后一个划分出的整数，则需要比较两个整数的大小，这可以通过比较两个整数（子串）最长公共前缀的下一个字符得出；
- 若其长度大于最后一个划分出的整数，由于不满足题目要求，无法转移。

对于 $\textit{lcp}$，可以用一个 $O(n^2)$ 的简单转移预处理出来，这样就可以做到 $O(1)$ 的状态转移。总的时间复杂度为 $O(n^2)$。

```go
const mod int = 1e9 + 7

func numberOfCombinations(s string) (ans int) {
	if s[0] == '0' {
		return
	}

	n := len(s)
	// 计算 lcp
	lcp := make([][]int, n+1)
	for i := range lcp {
		lcp[i] = make([]int, n+1)
	}
	for i := n - 1; i >= 0; i-- {
		for j := n - 1; j >= 0; j-- {
			if s[i] == s[j] {
				lcp[i][j] = lcp[i+1][j+1] + 1
			}
		}
	}
	// 返回 s[l1:l2] <= s[l2:r2]
	lessEq := func(l1, l2, r2 int) bool {
		l := lcp[l1][l2]
		return l >= r2-l2 || s[l1+l] < s[l2+l]
	}

	f := make([][]int, n)
	for i := range f {
		f[i] = make([]int, n)
	}
	for j := 0; j < n; j++ {
		f[0][j] = 1
	}
	for i := 1; i < n; i++ {
		if s[i] == '0' {
			continue
		}
		// k 和 j 同时向左向右扩展
		for j, k, sum := i, i-1, 0; j < n; j++ {
			f[i][j] = sum // 对应上面所说的长度小于最后一个划分出的整数
			if k < 0 {
				continue
			}
			if s[k] > '0' && lessEq(k, i, j+1) {
				f[i][j] = (f[i][j] + f[k][i-1]) % mod // 对应上面所说的长度等于最后一个划分出的整数
			}
			sum = (sum + f[k][i-1]) % mod
			k--
		}
	}
	for _, row := range f {
		ans = (ans + row[n-1]) % mod
	}
	return
}
```

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
