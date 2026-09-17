# 2571. 将整数减少到零需要的最少操作数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-reduce-an-integer-to-0/
- 题目 slug：`minimum-operations-to-reduce-an-integer-to-0`
- 来源专题：位运算
- 来源分类路径：八、思维题
- 难度分：1649
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-reduce-an-integer-to-0/solutions/2120204/ji-yi-hua-sou-suo-by-endlesscheng-cm6l/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[位运算一行搞定！附记忆化搜索做法（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-operations-to-reduce-an-integer-to-0/solutions/2120204/ji-yi-hua-sou-suo-by-endlesscheng-cm6l/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ji-yi-hua-sou-suo-by-endlesscheng-cm6l`
- topic id：`2120204`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

把 $n$ 看成二进制数，那么更高位的比特 $1$ 是会受到更低位的比特 $1$ 的加减影响的，但是，最小的比特 $1$ 没有这个约束。

那么考虑优先消除最小的比特 $1$，设它对应的数字为 $\textit{lowbit}$。

消除方法只能是加上 $\textit{lowbit}$，或者减去 $\textit{lowbit}$。

$\textit{lowbit}$ 的计算方法见本题 [视频讲解](https://www.bilibili.com/video/BV1jM411J7y7/)。

贪心的策略是：如果有多个连续 $1$，那么采用加法是更优的，可以一次消除多个 $1$；否则对于单个 $1$，减法更优。

```py [sol2-Python3]
class Solution:
    def minOperations(self, n: int) -> int:
        ans = 1
        while n & (n - 1):  # 不是 2 的幂次
            lb = n & -n
            if n & (lb << 1): n += lb  # 多个连续 1
            else: n -= lb  # 单个 1
            ans += 1
        return ans
```

```java [sol2-Java]
class Solution {
    public int minOperations(int n) {
        int ans = 1;
        while ((n & (n - 1)) > 0) { // n 不是 2 的幂次
            int lb = n & -n;
            if ((n & (lb << 1)) > 0) n += lb; // 多个连续 1
            else n -= lb; // 单个 1
            ++ans;
        }
        return ans;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int minOperations(int n) {
        int ans = 1;
        while (n & (n - 1)) { // n 不是 2 的幂次
            int lb = n & -n;
            if (n & (lb << 1)) n += lb; // 多个连续 1
            else n -= lb; // 单个 1
            ++ans;
        }
        return ans;
    }
};
```

```go [sol2-Go]
func minOperations(n int) int {
	ans := 1
	for n&(n-1) > 0 { // n 不是 2 的幂次
		lb := n & -n
		if n&(lb<<1) > 0 { // 多个连续 1
			n += lb
		} else {
			n -= lb // 单个 1
		}
		ans++
	}
	return ans
}
```

### 位运算优化

对于多个连续 $1$，如果它和前面的 $1$ 由至少两个 $0$ 隔开的话，那么就需要先加上 $\textit{lowbit}$，产生单个 $1$，再减去 $\textit{lowbit}$ 去掉这个 $1$，那么需要操作两次。

注意到

$$
\begin{aligned} 
n&=00111111\\
3n&=10111101\\
n\oplus 3n&=10000010
\end{aligned}
$$

刚好可以得到两个 $1$（$\oplus$ 表示异或）。

另外，对于单个 $1$，有

$$
\begin{aligned}
n&=0100\\
3n&=1100\\
n\oplus 3n&=1000
\end{aligned}
$$

刚好可以得到一个 $1$。

因此答案就是 $n\oplus 3n$ 二进制中 $1$ 的个数。

```py [sol3-Python3]
class Solution:
    def minOperations(self, n: int) -> int:
        return (3 * n ^ n).bit_count()
```

```java [sol3-Java]
class Solution {
    public int minOperations(int n) {
        return Integer.bitCount(3 * n ^ n);
    }
}
```

```cpp [sol3-C++]
class Solution {
public:
    int minOperations(int n) {
        return __builtin_popcount(3 * n ^ n);
    }
};
```

```go [sol3-Go]
func minOperations(n int) int {
	return bits.OnesCount(uint(3*n ^ n))
}
```

### 复杂度分析

- 时间复杂度：$O(1)$。
- 空间复杂度：$O(1)$。仅用到若干变量。

---

### 附：记忆化搜索写法

理论讲解请看[【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

```py [sol1-Python3]
@cache
def dfs(x: int) -> int:
    if (x & (x - 1)) == 0:  # x 是 2 的幂次
        return 1
    lb = x & -x
    return 1 + min(dfs(x + lb), dfs(x - lb))

class Solution:
    def minOperations(self, n: int) -> int:
        return dfs(n)
```

```go [sol1-Go]
var cache = map[int]int{}

func minOperations(n int) int {
	if n&(n-1) == 0 { // n 是 2 的幂次
		return 1
	}
	if res, ok := cache[n]; ok {
		return res
	}
	lb := n & -n
	res := 1 + min(minOperations(n+lb), minOperations(n-lb))
	cache[n] = res
	return res
}

func min(a, b int) int { if a > b { return b }; return a }
```

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
