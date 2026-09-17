# 2780. 合法分割的最小下标

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-index-of-a-valid-split/
- 题目 slug：`minimum-index-of-a-valid-split`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.8 摩尔投票法
- 难度分：1550
- 外部题解来源：https://leetcode.cn/problems/minimum-index-of-a-valid-split/solutions/2345798/jie-lun-zheng-ming-mei-ju-by-endlesschen-w156/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[结论证明+枚举](https://leetcode.cn/problems/minimum-index-of-a-valid-split/solutions/2345798/jie-lun-zheng-ming-mei-ju-by-endlesschen-w156/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jie-lun-zheng-ming-mei-ju-by-endlesschen-w156`
- topic id：`2345798`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

[视频讲解](https://www.bilibili.com/video/BV1DM4y1x7bR/) 第三题。

**证明**：分割出的两个数组的支配元素就是原数组的支配元素。

设这两个数组的支配元素为 $y$（题目要求支配元素相同），那么对于第一个数组有

$$
\text{freq}_1(y) \cdot 2 > i+1
$$

对于第二个数组有

$$
\text{freq}_2(y) \cdot 2 > n-i-1
$$

由于这两个数组合并之后就是原数组，所以

$$
\text{freq}(y) \cdot 2 = \text{freq}_1(y) \cdot 2 + \text{freq}_2(y) \cdot 2 > (i+1) + (n-i-1) = n
$$

上式表明，$y$ 就是原数组的支配元素，证毕。

### 算法

首先求出 $\textit{nums}$ 的支配元素（绝对众数）$\textit{mode}$ 及其出现次数 $\textit{total}$。

然后枚举 $i$，一边枚举一边统计 $\text{freq}_1(\textit{mode})$，那么 $\text{freq}_2(\textit{mode}) =\textit{total} -\text{freq}_1(\textit{mode})$。

只要满足 $\text{freq}_1(\textit{mode}) \cdot 2 > i+1$ 且 $\text{freq}_2(\textit{mode}) \cdot 2 > n-i-1$，就返回 $i$。

如果没有这样的 $i$，返回 $-1$。

```py [sol-Python3]
class Solution:
    def minimumIndex(self, nums: List[int]) -> int:
        mode, total = Counter(nums).most_common(1)[0]
        freq1 = 0
        for i, x in enumerate(nums):
            freq1 += x == mode
            if freq1 * 2 > i + 1 and (total - freq1) * 2 > len(nums) - i - 1:
                return i
        return -1
```

```go [sol-Go]
func minimumIndex(nums []int) int {
	// 也可以用摩尔投票法实现
	freq := map[int]int{}
	mode := nums[0]
	for _, x := range nums {
		freq[x]++
		if freq[x] > freq[mode] {
			mode = x
		}
	}

	total := freq[mode]
	freq1 := 0
	for i, x := range nums {
		if x == mode {
			freq1++
		}
		if freq1*2 > i+1 && (total-freq1)*2 > len(nums)-i-1 {
			return i
		}
	}
	return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。用摩尔投票法可以做到 $\mathcal{O}(1)$ 额外空间，具体见 [169. 多数元素](https://leetcode.cn/problems/majority-element/)。

## 本地原创解析

### 1. 题意重述

本题来自 `七、杂项 / §7.8 摩尔投票法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、杂项 / §7.8 摩尔投票法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
