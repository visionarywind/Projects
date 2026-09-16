# 2750. 将数组划分成若干好子数组的方式

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/ways-to-split-array-into-good-subarrays/
- 题目 slug：`ways-to-split-array-into-good-subarrays`
- 来源专题：数学算法
- 来源分类路径：二、组合数学 / §2.1 乘法原理
- 难度分：1598
- 外部题解来源：https://leetcode.cn/problems/ways-to-split-array-into-good-subarrays/solutions/2319642/cheng-fa-yuan-li-by-endlesscheng-hypt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[乘法原理（Python/Java/C++/Go）](https://leetcode.cn/problems/ways-to-split-array-into-good-subarrays/solutions/2319642/cheng-fa-yuan-li-by-endlesscheng-hypt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`cheng-fa-yuan-li-by-endlesscheng-hypt`
- topic id：`2319642`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

根据题意，需要在**每**两个 $1$ 之间画一条分割线，有 $x$ 个 $0$ 就可以画 $x+1$ 条分割线。

根据乘法原理，答案为所有分割线的方案数的乘积。这里讲解见[【周赛 351】](https://www.bilibili.com/video/BV1du41187ZN/)第三题。

特别地，如果数组中没有 $1$，那么答案为 $0$。如果数组只有一个 $1$，那么答案为 $1$。

如果你对取模有疑问，可以看文末的讲解。

```py [sol-Python3]
class Solution:
    def numberOfGoodSubarraySplits(self, nums: List[int]) -> int:
        MOD = 10 ** 9 + 7
        ans, pre = 1, -1
        for i, x in enumerate(nums):
            if x == 0: continue
            if pre >= 0:
                ans = ans * (i - pre) % MOD
            pre = i
        return 0 if pre < 0 else ans
```

```java [sol-Java]
class Solution {
    public int numberOfGoodSubarraySplits(int[] nums) {
        final long MOD = (long) 1e9 + 7;
        long ans = 1;
        int pre = -1, n = nums.length;
        for (int i = 0; i < n; i++) {
            if (nums[i] == 0) continue;
            if (pre >= 0) ans = ans * (i - pre) % MOD;
            pre = i;
        }
        return pre < 0 ? 0 : (int) ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numberOfGoodSubarraySplits(vector<int> &nums) {
        const int MOD = 1e9 + 7;
        int ans = 1, pre = -1, n = nums.size();
        for (int i = 0; i < n; i++) {
            if (nums[i] == 0) continue;
            if (pre >= 0) ans = (long) ans * (i - pre) % MOD;
            pre = i;
        }
        return pre < 0 ? 0 : ans;
    }
};
```

```go [sol-Go]
func numberOfGoodSubarraySplits(nums []int) int {
	const mod int = 1e9 + 7
	ans, pre := 1, -1
	for i, x := range nums {
		if x > 0 {
			if pre >= 0 {
				ans = ans * (i - pre) % mod
			}
			pre = i
		}
	}
	if pre < 0 { // 整个数组都是 0，没有好子数组
		return 0
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

#### 相似题目

- [2147. 分隔长廊的方案数](https://leetcode.cn/problems/number-of-ways-to-divide-a-long-corridor/)

#### 算法小课堂：模运算

如果让你计算 $1234\cdot 6789$ 的**个位数**，你会如何计算？

由于只有个位数会影响到乘积的个位数，那么 $4\cdot 9=36$ 的个位数 $6$ 就是答案。

对于 $1234+6789$ 的个位数，同理，$4+9=13$ 的个位数 $3$ 就是答案。

你能把这个结论抽象成数学等式吗？

一般地，涉及到取模的题目，通常会用到如下等式（上面计算的是 $m=10$）：

$$
(a+b)\bmod m = ((a\bmod m) + (b\bmod m)) \bmod m
$$

$$
(a\cdot b) \bmod m=((a\bmod m)\cdot  (b\bmod m)) \bmod m
$$

证明：根据**带余除法**，任意整数 $a$ 都可以表示为 $a=km+r$，这里 $r$ 相当于 $a\bmod m$。那么设 $a=k_1m+r_1,\ b=k_2m+r_2$。

第一个等式：

$$
\begin{aligned}
&\ (a+b) \bmod m\\
=&\ ((k_1+k_2) m+r_1+r_2)\bmod m\\
=&\ (r_1+r_2)\bmod m\\
=&\ ((a\bmod m) + (b\bmod m)) \bmod m
\end{aligned}
$$

第二个等式：

$$
\begin{aligned}
&\ (a\cdot b) \bmod m\\
=&\ (k_1k_2m^2+(k_1r_2+k_2r_1)m+r_1r_2)\bmod m\\
=&\ (r_1r_2)\bmod m\\
=&\ ((a\bmod m)\cdot  (b\bmod m)) \bmod m
\end{aligned}
$$

**根据这两个恒等式，可以随意地对代码中的加法和乘法的结果取模**。

## 本地原创解析

### 1. 题意重述

本题来自 `二、组合数学 / §2.1 乘法原理`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、组合数学 / §2.1 乘法原理`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
