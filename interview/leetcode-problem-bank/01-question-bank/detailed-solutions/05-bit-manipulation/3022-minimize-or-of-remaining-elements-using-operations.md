# 3022. 给定操作次数内使剩余元素的或值最小

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimize-or-of-remaining-elements-using-operations/
- 题目 slug：`minimize-or-of-remaining-elements-using-operations`
- 来源专题：位运算
- 来源分类路径：五、试填法
- 难度分：2918
- 外部题解来源：https://leetcode.cn/problems/minimize-or-of-remaining-elements-using-operations/solutions/2622658/shi-tian-fa-pythonjavacgo-by-endlesschen-ysom/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[试填法（Python/Java/C++/Go）](https://leetcode.cn/problems/minimize-or-of-remaining-elements-using-operations/solutions/2622658/shi-tian-fa-pythonjavacgo-by-endlesschen-ysom/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-tian-fa-pythonjavacgo-by-endlesschen-ysom`
- topic id：`2622658`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

[视频讲解](https://www.bilibili.com/video/BV1we411J7Y8/) 第四题。

## 提示 1

从高到低考虑：答案在这一位能不能是 $0$？

## 提示 2

一次操作，用 AND 合并两个相邻数字。

多次操作，相当于把一段**连续子数组**合并成 $0$。

尝试从左到右合并（忽略低位和必须是 $1$ 的位），如果合并出 $0$，就开始合并下一段。

## 思路

以 $[5,2,3,6]$ 为例说明，这四个数的二进制表示如下：

$$
\begin{aligned}
&101\\
&010\\
&011\\
&110
\end{aligned}
$$

设 $k=2$。从高到低考虑：答案在这一位能不能是 $0$？

- 最高位有两个 $1$，合并掉这两个 $1$ 需要操作 $2$ 次（$\le k$），所以答案的最高位可以是 $0$。
- 对于次高位，我们需要通过一连串的合并，让合并结果的最高位和次高位都是 $0$。数组前两个数（只看最高位和次高位）可以合并成 $0$，操作 $1$ 次。数组后两个数（只看最高位和次高位）无法合并成 $0$，那么用前两个数合并出来的 $0$，与后两个数合并，操作 $2$ 次，得到 $0$。所以一共要操作 $3$ 次才能让最高位和次高位都是 $0$，无法做到，所以答案的次高位一定是 $1$。
- 对于最低位，我们需要通过一连串的合并，让合并结果的最高位和最低位都是 $0$。注意我们**无需考虑次高位**，因为前面已经确定答案这一位是 $1$ 了。数组前两个数（只看最高位和最低位）可以合并成 $0$，操作 $1$ 次。数组后两个数（只看最高位和最低位）也可以合并成 $0$，操作 $1$ 次。一共操作 $2$ 次，所以答案的最低位可以是 $0$。
- 综上所述，答案的二进制表示为 $010$，即十进制 $2$。

代码中用到了一些位运算技巧，不了解位运算的同学可以看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

注意，如果整个数组都无法合并成 $0$，那么代码计算出来的操作次数是 $n$，题目保证了这是大于 $k$ 的，无需特判这种情况。

```py [sol-Python3]
class Solution:
    def minOrAfterOperations(self, nums: List[int], k: int) -> int:
        ans = mask = 0
        for b in range(max(nums).bit_length() - 1, -1, -1):
            mask |= 1 << b
            cnt = 0  # 操作次数
            and_res = -1  # -1 的二进制全为 1
            for x in nums:
                and_res &= x & mask
                if and_res:
                    cnt += 1  # 合并 x，操作次数加一
                else:
                    and_res = -1  # 准备合并下一段
            if cnt > k:
                ans |= 1 << b  # 答案的这个比特位必须是 1
                mask ^= 1 << b  # 后面不考虑这个比特位
        return ans
```

```java [sol-Java]
class Solution {
    public int minOrAfterOperations(int[] nums, int k) {
        int ans = 0;
        int mask = 0;
        for (int b = 29; b >= 0; b--) {
            mask |= 1 << b;
            int cnt = 0; // 操作次数
            int and = -1; // -1 的二进制全为 1
            for (int x : nums) {
                and &= x & mask;
                if (and != 0) {
                    cnt++; // 合并 x，操作次数加一
                } else {
                    and = -1; // 准备合并下一段
                }
            }
            if (cnt > k) {
                ans |= 1 << b; // 答案的这个比特位必须是 1
                mask ^= 1 << b; // 后面不考虑这个比特位
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOrAfterOperations(vector<int> &nums, int k) {
        int ans = 0, mask = 0;
        for (int b = 29; b >= 0; b--) {
            mask |= 1 << b;
            int cnt = 0, and_res = -1; // -1 的二进制全为 1
            for (int x : nums) {
                and_res &= x & mask;
                if (and_res) {
                    cnt++; // 合并 x，操作次数加一
                } else {
                    and_res = -1; // 准备合并下一段
                }
            }
            if (cnt > k) {
                ans |= 1 << b; // 答案的这个比特位必须是 1
                mask ^= 1 << b; // 后面不考虑这个比特位
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minOrAfterOperations(nums []int, k int) (ans int) {
	mask := 0
	for b := 29; b >= 0; b-- {
		mask |= 1 << b
		cnt := 0  // 操作次数
		and := -1 // -1 的二进制全为 1
		for _, x := range nums {
			and &= x & mask
			if and != 0 {
				cnt++ // 合并 x，操作次数加一
			} else {
				and = -1 // 准备合并下一段
			}
		}
		if cnt > k {
			ans |= 1 << b  // 答案的这个比特位必须是 1
			mask ^= 1 << b // 后面不考虑这个比特位
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(1)$。

[2023 下半年周赛题目总结](https://leetcode.cn/circle/discuss/lUu0KB/)

## 本地原创解析

### 1. 题意重述

本题来自 `五、试填法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、试填法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
