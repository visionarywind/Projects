# 2202. K 次操作后最大化顶端元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximize-the-topmost-element-after-k-moves/
- 题目 slug：`maximize-the-topmost-element-after-k-moves`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.1 从特殊到一般
- 难度分：1717
- 外部题解来源：https://leetcode.cn/problems/maximize-the-topmost-element-after-k-moves/solutions/1332973/by-endlesscheng-vmtr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论](https://leetcode.cn/problems/maximize-the-topmost-element-after-k-moves/solutions/1332973/by-endlesscheng-vmtr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-vmtr`
- topic id：`1332973`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

### 提示 1

考虑哪些数能在 $k$ 次操作后成为栈顶，答案是这些数的最大值。

### 提示 2

从特殊位置 $\textit{nums}[0]$ 入手，思考如何让 $\textit{nums}[0]$ 成为栈顶。（尝试枚举 $k=0,1,2,3,\cdots$）

### 提示 3

推广到其余 $\textit{nums}[i]$。

### 思路

设 $\textit{nums}$ 的长度为 $n$，根据题意：

- 如果 $n=1$，那么我们只能在栈不为空时删除栈顶，栈为空时将 $\textit{nums}[0]$ 入栈。因此 $k$ 为奇数时，$k$ 次操作后栈为空，返回 $-1$；$k$ 为偶数时则返回 $\textit{nums}[0]$。
- 如果 $k=0$，无法执行任何操作，直接返回 $\textit{nums}[0]$。

其余情况，按数组元素的下标 $i$ 分类讨论：

- 如果 $i=0$，我们可以不断地删除-添加 $\textit{nums}[0]$，如果 $k$ 为偶数，那么最后栈顶为 $\textit{nums}[0]$；如果 $k$ 为奇数（这里要求 $k>1$），我们可以在倒数第二步删除 $\textit{nums}[1]$，最后一步将 $\textit{nums}[0]$ 入栈，从而保证 $\textit{nums}[0]$ 可以为栈顶。 
- 如果 $0<i<k-1$，我们仍然可以仿造上述流程操作。
- 如果 $i=k-1$，最后一步操作只能删除 $\textit{nums}[i]$，所以无法将 $\textit{nums}[i]$ 置于栈顶。
- 如果 $i=k$，那么可以删除前 $k$ 个元素，将 $\textit{nums}[i]$ 置于栈顶。
- 如果 $i>k$，$\textit{nums}[i]$ 前面的元素无法删除，所以无法将 $\textit{nums}[i]$ 置于栈顶。

综上所述，我们可以让 $i<k-1$ 或 $i=k$ 的数组元素作为 $k$ 次操作后的栈顶。这些元素的最大值即为答案。

```Python [sol-Python3]
class Solution:
    def maximumTop(self, nums: List[int], k: int) -> int:
        if k % 2 and len(nums) == 1:
            return -1
        return max(num for i, num in enumerate(nums)
                       if i < k - 1 or i == k)
```

```go [sol-Go]
func maximumTop(a []int, k int) int {
	n := len(a)
	if n == 1 || k == 0 {
		if k%2 == 1 { return -1 }
		return a[0]
	}
	// 删除 a[k-1] 以及 a[k+1:]，下面直接取 a 的最大值
	if k < n {
		a = append(a[:k-1], a[k]) 
	} else if k == n {
		a = a[:n-1]
	}
	return slices.Max(a)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 分类题单

以下题单没有特定的顺序，可以按照个人喜好刷题。

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.1 从特殊到一般`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.1 从特殊到一般`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
