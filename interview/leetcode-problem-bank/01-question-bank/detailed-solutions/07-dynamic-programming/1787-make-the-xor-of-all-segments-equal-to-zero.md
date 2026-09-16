# 1787. 使所有区间的异或结果为零

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/make-the-xor-of-all-segments-equal-to-zero/
- 题目 slug：`make-the-xor-of-all-segments-equal-to-zero`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：2640
- 外部题解来源：https://leetcode.cn/problems/make-the-xor-of-all-segments-equal-to-zero/solutions/640733/fen-lei-tao-lun-tan-xin-dp-by-endlessche-y14r/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论：贪心+DP](https://leetcode.cn/problems/make-the-xor-of-all-segments-equal-to-zero/solutions/640733/fen-lei-tao-lun-tan-xin-dp-by-endlessche-y14r/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-tan-xin-dp-by-endlessche-y14r`
- topic id：`640733`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**思路**

通过计算最多能保留多少个数不变，来计算最少需要更改的个数。

要使区间异或和为 $0$，数组中的数必须是以 $k$ 为周期的。（从样例中也能猜出这个结论）

将数组按下标模 $k$ 的结果分为 $k$ 组，分两种情况讨论：

1. 有一组所有数都要选择更改

   由于数组元素可以更改为任意值，除了某个组 $X$ 以外，其余组可以选择保留一个数不变，然后将 $X$ 中的元素更改为其余组所保留的数的异或和。

   这种情况下存在一个贪心策略：每个组可以选择出现次数最多的数，那么累加各个组的最大次数，然后减去最大次数中的最小值，就是这种情况下能保留的数最大个数。

2. 所有组都要选择保留一个数不变

   记 $\textit{cnt}[i][v]$ 为第 $i$ 组中元素 $v$ 的出现次数。我们可以写一个二维 $\textit{DP}$，$f(i,j)$ 表示前 $i$ 个组所保留的数的异或和为 $j$ 时，能保留的数的最大个数。那么转移就是：

   $$
   f(i,j) = 
   \begin{cases}
   \textit{cnt}[0][j]&i=0\\
   \max\{\textit{cnt}[i][v]+f(i-1,j\oplus v)\},v\in \textit{group}[i]&i>0
   \end{cases}
   $$

   $f(k-1,0)$ 即为这种情况的答案。

这两种情况取最大值，然后用数组长度 $n$ 减去该最大值即为答案。

**时间复杂度**

记数组的最大元素的二进制长度为 $L$，那么转移中的异或值最大可能为 $2^L-1$。

故状态个数为 $O(k\cdot 2^L)$，每个状态需要计算 $O(\dfrac{n}{k})$ 个转移，因此总的时间复杂度为 $O(n\cdot 2^L)$。


**代码**

```go
func minChanges(a []int, k int) (ans int) {
	cnt := make([][1024]int, k)
	n := len(a)
	minCnt := n
	for i := 0; i < k; i++ {
		maxCnt := 0
		for j := i; j < n; j += k {
			cnt[i][a[j]]++
			maxCnt = max(maxCnt, cnt[i][a[j]])
		}
		ans += maxCnt
		minCnt = min(minCnt, maxCnt)
	}
	ans -= minCnt

	dp := make([][1024]int, k)
	for i := range dp {
		for j := 0; j < 1024; j++ {
			dp[i][j] = -1
		}
	}
	var f func(int, int) int
	f = func(p, xor int) (res int) {
		if p == 0 {
			return cnt[0][xor]
		}
		dv := &dp[p][xor]
		if *dv >= 0 {
			return *dv
		}
		defer func() { *dv = res }()
		for i := p; i < n; i += k {
			res = max(res, cnt[p][a[i]]+f(p-1, xor^a[i]))
		}
		return
	}
	return n - max(ans, f(k-1, 0))
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}

func max(a, b int) int {
	if a > b {
		return a
	}
	return b
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.6 多维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.6 多维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
