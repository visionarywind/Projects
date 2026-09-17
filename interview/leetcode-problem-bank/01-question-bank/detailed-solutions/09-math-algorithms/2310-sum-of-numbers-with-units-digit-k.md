# 2310. 个位数字为 K 的整数之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-numbers-with-units-digit-k/
- 题目 slug：`sum-of-numbers-with-units-digit-k`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：1559
- 外部题解来源：https://leetcode.cn/problems/sum-of-numbers-with-units-digit-k/solutions/1611273/mei-ju-da-an-by-endlesscheng-zh75/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[枚举答案 + 利用同余性质优化至 O(1) 时间复杂度](https://leetcode.cn/problems/sum-of-numbers-with-units-digit-k/solutions/1611273/mei-ju-da-an-by-endlesscheng-zh75/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-da-an-by-endlesscheng-zh75`
- topic id：`1611273`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1CW4y1k7B3) 已出炉，欢迎点赞三连~

---

#### 提示 1

首先分析一下答案至少至多是多少（尤其是和数学相关的题目）。

#### 提示 2

答案的范围比较小，我们可以尝试枚举答案 $n$。

#### 提示 3

把个位数单独拆开看，每个数可以表示成 $10$ 的倍数加上 $k$ 的形式。

由于这 $n$ 个数都以 $k$ 结尾，那么 $\textit{num}-nk$ 必须是 $10$ 的倍数。

从小到大枚举 $n$，找到第一个满足 $\textit{num}-nk$ 是 $10$ 的倍数的 $n$。

由于 $n$ 不会超过 $\textit{num}$，我们至多枚举到 $\textit{num}$ 时停止。

注意特判 $\textit{num}=0$ 的情况，此时返回 $0$。

```Python [sol1-Python3]
class Solution:
    def minimumNumbers(self, num: int, k: int) -> int:
        if num == 0: return 0
        for n in range(1, num + 1):
            if num - n * k < 0: break
            if (num - n * k) % 10 == 0: return n
        return -1
```

```go [sol1-Go]
func minimumNumbers(num, k int) int {
	if num == 0 {
		return 0
	}
	for n := 1; n <= num && num-n*k >= 0; n++ {
		if (num-n*k)%10 == 0 {
			return n
		}
	}
	return -1
}
```

特判 $k=0$ 的情况，还可以减少一部分循环次数：

```Python [sol2-Python3]
class Solution:
    def minimumNumbers(self, num: int, k: int) -> int:
        if num == 0: return 0
        if k == 0: return -1 if num % 10 else 1
        return next((n for n in range(1, num // k + 1) if (num - n * k) % 10 == 0), -1)
```

```go [sol2-Go]
func minimumNumbers(num, k int) int {
	if num == 0 {
		return 0
	}
	if k == 0 {
		if num%10 == 0 {
			return 1
		}
		return -1
	}
	for n := 1; n*k <= num; n++ {
		if (num-n*k)%10 == 0 {
			return n
		}
	}
	return -1
}
```

进一步地，由于

$$
n\cdot k\equiv(n\bmod 10)\cdot k \pmod{10}
$$

枚举到 $n=11$ 时，$(\textit{num}-nk)\bmod 10$ 的结果会和 $n=1$ 时相同，对于更大的 $n$ 也同样会和 $n\bmod 10$ 相同。

因此至多枚举到 $n=10$ 就行了。

#### 复杂度分析

- 时间复杂度：$O(1)$。枚举的次数至多为 $10$。
- 空间复杂度：$O(1)$，仅用到若干变量。

```Python [sol3-Python3]
class Solution:
    def minimumNumbers(self, num, k):
        if num == 0: return 0
        if k == 0: return -1 if num % 10 else 1
        return next((n for n in range(1, min(num // k + 1, 11)) if ((num - n * k) % 10 == 0)), -1)
```

```java [sol3-Java]
class Solution {
    public int minimumNumbers(int num, int k) {
        if (num == 0) return 0;
        for (var n = 1; n <= 10 && num - k * n >= 0; ++n)
            if ((num - k * n) % 10 == 0) return n;
        return -1;
    }
}
```

```cpp [sol3-C++]
class Solution {
public:
    int minimumNumbers(int num, int k) {
        if (num == 0) return 0;
        for (int n = 1; n <= 10 && num - k * n >= 0; ++n)
            if ((num - k * n) % 10 == 0) return n;
        return -1;
    }
};
```

```go [sol3-Go]
func minimumNumbers(num, k int) int {
	if num == 0 {
		return 0
	}
	for n := 1; n <= 10 && n*k <= num; n++ {
		if (num-n*k)%10 == 0 {
			return n
		}
	}
	return -1
}
```

附：Python 记忆化写法

```py
@cache
def dfs(left, k):
    if left == 0: return 0
    res = inf
    for i in range(k, left + 1, 10):
        res = min(res, dfs(left - i, k) + 1)
    return res

class Solution:
    def minimumNumbers(self, num: int, k: int) -> int:
        if num == 0: return 0
        if k == 0: return -1 if num % 10 else 1
        ans = dfs(num, k)
        return ans if ans < inf else -1
```

## 本地原创解析

### 1. 题意重述

本题来自 `七、杂项 / §7.10 其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、杂项 / §7.10 其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
