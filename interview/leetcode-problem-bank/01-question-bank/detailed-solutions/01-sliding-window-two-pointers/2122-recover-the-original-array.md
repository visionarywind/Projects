# 2122. 还原原数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/recover-the-original-array/
- 题目 slug：`recover-the-original-array`
- 来源专题：滑动窗口与双指针
- 来源分类路径：三、单序列双指针 / §3.3 同向双指针
- 难度分：2159
- 外部题解来源：https://leetcode.cn/problems/recover-the-original-array/solutions/1177346/mei-ju-higher0-shuang-zhi-zhen-by-endles-ic64/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举 higher[0] + 双指针](https://leetcode.cn/problems/recover-the-original-array/solutions/1177346/mei-ju-higher0-shuang-zhi-zhen-by-endles-ic64/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-higher0-shuang-zhi-zhen-by-endles-ic64`
- topic id：`1177346`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

将 $\textit{nums}$ 排序后，$\textit{lower}[0]$ 必然是 $\textit{nums}[0]$。我们可以在 $\textit{nums}$ 中枚举 $\textit{higher}[0]$ 的值，从而得到 $k=\dfrac{\textit{higher}[0]-\textit{lower}[0]}{2}$。

由于 $\textit{higher}[i]-\textit{lower}[i]=2k$ 是个定值，我们可以用双指针去遍历 $\textit{nums}$，计算其余的 $\textit{lower}$ 和 $\textit{higher}$ 的元素值。

具体来说，用一个布尔数组 $\textit{vis}$ 标记出现在 $\textit{higher}$ 中的数，每次循环先在 $\textit{nums}$ 中找到下一个不在 $\textit{higher}$ 中的数，作为本次循环的 $\textit{lower}$ 值，然后通过 $\textit{higher}[i]-\textit{lower}[i]=2k$ 这个等式来找到本次循环的 $\textit{higher}$ 值。

细节见代码注释。

```go
func recoverArray(nums []int) []int {
	sort.Ints(nums)
	for i, n := 1, len(nums); ; i++ {
		if nums[i] == nums[i-1] { continue } // 优化：如果与上一个元素相同，那么我们会得到同样的 k，同样找不到原数组，此时应直接跳过
		d := nums[i] - nums[0] // 此时 d > 0 必然成立
		if d&1 > 0 { continue } // k 必须是整数
		k := d / 2
		vis := make([]bool, n) // 用来标记出现在 higher 中的数（用 nums 的下标）
		vis[i] = true
		ans := []int{(nums[0] + nums[i]) / 2}
		for lo, hi := 0, i+1; hi < n; hi++ { // 双指针：lo 指向 lower，hi 指向 higher
			for lo++; vis[lo]; lo++ {} // 找 lower：跳过出现在 higher 中的数
			for ; hi < n && nums[hi]-nums[lo] < 2*k; hi++ {} // 找 higher
			if hi == n || nums[hi]-nums[lo] > 2*k { break } // 不存在满足等式的 higher 值
			vis[hi] = true
			ans = append(ans, (nums[lo]+nums[hi])/2) // 找到一对满足等式的 (lower, higher)
		}
		if len(ans) == n/2 { return ans }
	}
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `三、单序列双指针 / §3.3 同向双指针`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、单序列双指针 / §3.3 同向双指针`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
