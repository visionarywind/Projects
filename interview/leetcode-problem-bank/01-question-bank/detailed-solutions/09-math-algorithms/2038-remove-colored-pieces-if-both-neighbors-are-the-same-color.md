# 2038. 如果相邻两个颜色均相同则删除当前颜色

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-colored-pieces-if-both-neighbors-are-the-same-color/
- 题目 slug：`remove-colored-pieces-if-both-neighbors-are-the-same-color`
- 来源专题：数学算法
- 来源分类路径：四、博弈论
- 难度分：1468
- 外部题解来源：https://leetcode.cn/problems/remove-colored-pieces-if-both-neighbors-are-the-same-color/solutions/1050943/tong-ji-lian-xu-xiang-tong-yan-se-de-cha-zynp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[统计连续相同颜色的长度](https://leetcode.cn/problems/remove-colored-pieces-if-both-neighbors-are-the-same-color/solutions/1050943/tong-ji-lian-xu-xiang-tong-yan-se-de-cha-zynp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tong-ji-lian-xu-xiang-tong-yan-se-de-cha-zynp`
- topic id：`1050943`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于删除操作需要两边都有相同颜色，所以对于每一串连续相同的颜色，最边上的两个颜色是不会被删除的。

因此删除一种颜色不会对另一种颜色产生任何影响，我们只需要统计每一串连续相同颜色的长度 $l$，若 $l>2$，则可以删除 $l-2$ 个颜色。将该值按颜色分别累加，记 $\texttt{A}$ 的累加值为 $a$，$\texttt{B}$ 的累加值为 $b$。

Alice 要想获胜，其操作次数必须比 Bob 多，否则 Bob 获胜，因此若 $a>b$ 则返回 $\texttt{true}$，否则返回 $\texttt{false}$。

```go
func winnerOfGame(colors string) bool {
	cnt := [2]int{}
	for i, n := 0, len(colors); i < n; {
		i0 := i
		c := colors[i0]
		for i < n && colors[i] == c {
			i++ // 注意这里 i 就是外层循环的 i，所以复杂度是 O(n) 的
		}
		if l := i - i0; l > 2 {
			cnt[c-'A'] += l - 2
		}
	}
	return cnt[0] > cnt[1]
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `四、博弈论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、博弈论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
