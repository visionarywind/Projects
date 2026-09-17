# 2568. 最小无法得到的或值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-impossible-or/
- 题目 slug：`minimum-impossible-or`
- 来源专题：位运算
- 来源分类路径：八、思维题
- 难度分：1754
- 外部题解来源：https://leetcode.cn/problems/minimum-impossible-or/solutions/2119478/nao-jin-ji-zhuan-wan-pythonjavacgo-by-en-7j89/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[脑筋急转弯+lowbit优化（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-impossible-or/solutions/2119478/nao-jin-ji-zhuan-wan-pythonjavacgo-by-en-7j89/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`nao-jin-ji-zhuan-wan-pythonjavacgo-by-en-7j89`
- topic id：`2119478`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

### 提示 1

暴力做法是从小到大枚举答案，但这显然会超时。

如何利用「$x$ 不是答案」这一信息呢？

### 提示 2

如果 $1$ 不是答案，说明 $1$ 在 $\textit{nums}$ 中，因为 $1$ 只有一个比特是 $1$（下同）。

继续枚举，如果 $2$ 不是答案，说明 $2$ 在 $\textit{nums}$ 中。

那么 $3$ 肯定不是答案，因为 $1$ 和 $2$ 都在 $\textit{nums}$ 中，且 $1|2=3$。

继续枚举，如果 $4$ 不是答案，说明 $4$ 在 $\textit{nums}$ 中。

那么 $5,6,7$ 肯定不是答案，因为 $1,2,4$ 都在 $\textit{nums}$ 中，它们可以通过或运算组成 $1$ 到 $7$ 中的任意数字。

### 提示 3

因此，我们只需要从小到大挨个判断 $2^i$ 是否在 $\textit{nums}$ 中，第一个不在 $\textit{nums}$ 中的就是答案。

代码实现时，可以用哈希表可以加速这个判断过程。

附：[视频讲解](https://www.bilibili.com/video/BV15D4y1G7ms/)。

```py [sol1-Python3]
class Solution:
    def minImpossibleOR(self, nums: List[int]) -> int:
        s = set(nums)
        return next(1 << i for i in count() if 1 << i not in s)
```

```java [sol1-Java]
class Solution {
    public int minImpossibleOR(int[] nums) {
        var s = new HashSet<Integer>();
        for (int x : nums) s.add(x);
        for (int i = 1; ; i <<= 1)
            if (!s.contains(i))
                return i;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int minImpossibleOR(vector<int> &nums) {
        unordered_set s(nums.begin(), nums.end());
        for (int i = 1;; i <<= 1)
            if (!s.count(i))
                return i;
    }
};
```

```go [sol1-Go]
func minImpossibleOR(a []int) (ans int) {
	has := map[int]bool{}
	for _, v := range a {
		has[v] = true
	}
	for i := 1; ; i <<= 1 {
		if !has[i] {
			return i
		}
	}
}
```

### 复杂度分析

- 时间复杂度：$O(n+\log U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$O(n)$。

### 进一步优化

由于只需要看 $2$ 的幂次，我们可以用一个 $\textit{mask}$ 记录 $\textit{nums}$ 中是 $2$ 的幂次的数。

那么答案就是 $\textit{mask}$ 中的最低比特 $0$。这可以取反后，用 $\textit{lowbit}$ 得到，具体见 [视频讲解](https://www.bilibili.com/video/BV15D4y1G7ms/)。

```py [sol2-Python3]
class Solution:
    def minImpossibleOR(self, nums: List[int]) -> int:
        mask = 0
        for x in nums:
            if (x & (x - 1)) == 0:  # x 是 2 的幂次
                mask |= x
        mask = ~mask
        return mask & -mask  # lowbit
```

```java [sol2-Java]
class Solution {
    public int minImpossibleOR(int[] nums) {
        int mask = 0;
        for (int x : nums)
            if ((x & (x - 1)) == 0) // x 是 2 的幂次
                mask |= x;
        mask = ~mask;
        return mask & -mask; // lowbit
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int minImpossibleOR(vector<int> &nums) {
        int mask = 0;
        for (int x : nums)
            if ((x & (x - 1)) == 0) // x 是 2 的幂次
                mask |= x;
        mask = ~mask;
        return mask & -mask; // lowbit
    }
};
```

```go [sol2-Go]
func minImpossibleOR(nums []int) int {
	mask := 0
	for _, x := range nums {
		if x&(x-1) == 0 { // x 是 2 的幂次
			mask |= x
		}
	}
	mask = ^mask
	return mask & -mask // lowbit
}
```

### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$O(1)$。仅用到若干额外变量。

### 可以用到这个优化技巧的题目

- [2154. 将找到的值乘以 2](https://leetcode.cn/problems/keep-multiplying-found-values-by-two/)

---

如果你觉得自己的思维能力有些薄弱，可以做做 [从周赛中学算法 - 2022 年周赛题目总结（下篇）](https://leetcode.cn/circle/discuss/WR1MJP/) 中的「思维题」这节，所有题目我都写了题解。

## 本地原创解析

### 1. 题意重述

本题来自 `八、思维题`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、思维题`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
