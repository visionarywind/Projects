# 3871. 统计范围内的逗号 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-commas-in-range-ii/
- 题目 slug：`count-commas-in-range-ii`
- 来源专题：动态规划
- 来源分类路径：十、数位 DP / §10.2 统计合法元素的价值总和
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/count-commas-in-range-ii/solutions/3925828/heng-kan-cheng-ling-ce-cheng-feng-gong-x-8qaw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[横看成岭侧成峰，贡献法（Python/Java/C++/Go）](https://leetcode.cn/problems/count-commas-in-range-ii/solutions/3925828/heng-kan-cheng-ling-ce-cheng-feng-gong-x-8qaw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`heng-kan-cheng-ling-ce-cheng-feng-gong-x-8qaw`
- topic id：`3925828`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

题目让我们统计

$$
\begin{array}{r}
1\\
2\\
3\\
\vdots\\
999\\
1,000\\
1,001\\
1,002\\
\vdots\\
999,999\\
1,000,000\\
1,000,001\\
1,000,002\\
\vdots \\
999,999,999\\
1,000,000,000\\
1,000,000,001\\
1,000,000,002\\
\vdots \\
n \\
\end{array}
$$

这些数一共有多少个逗号。

横看成岭侧成峰，从逗号的视角看，它出现在多少个数中？

- 最右边的逗号，出现在 $[10^3,n]$ 的每个整数中。这有 $n-10^3+1$ 个。
- 倒数第二个逗号，出现在 $[10^6,n]$ 的每个整数中。这有 $n-10^6+1$ 个。
- 倒数第三个逗号，出现在 $[10^9,n]$ 的每个整数中。这有 $n-10^9+1$ 个。
- 依此类推。

累加这些逗号的个数，即为答案。

[本题视频讲解](https://www.bilibili.com/video/BV1DvwTzbE1n/)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def countCommas(self, n: int) -> int:
        ans = 0
        # 从低到高，枚举逗号的位置
        low = 1000
        while low <= n:
            # [low, n] 中的每个数都在这个位置上有一个逗号
            ans += n - low + 1
            low *= 1000
        return ans
```

```java [sol-Java]
class Solution {
    public long countCommas(long n) {
        long ans = 0;
        // 从低到高，枚举逗号的位置
        for (long low = 1000; low <= n; low *= 1000) {
            // [low, n] 中的每个数都在这个位置上有一个逗号
            ans += n - low + 1;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long countCommas(long long n) {
        long long ans = 0;
        // 从低到高，枚举逗号的位置
        for (long long low = 1000; low <= n; low *= 1000) {
            // [low, n] 中的每个数都在这个位置上有一个逗号
            ans += n - low + 1;
        }
        return ans;
    }
};
```

```go [sol-Go]
func countCommas(n int64) (ans int64) {
	// 从低到高，枚举逗号的位置
	for low := int64(1000); low <= n; low *= 1000 {
		// [low, n] 中的每个数都在这个位置上有一个逗号
		ans += n - low + 1
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 附：数学公式

设 $k$ 是满足 $10^{3k}\le n$ 的最大整数。

根据等比数列求和公式，答案为

$$
\begin{aligned}
    & (n-10^3+1) + (n-10^6+1) + \cdots + (n-10^{3k}+1)      \\
={} & k(n+1) - \dfrac{10^{3k+3}-1000}{999}        \\
\end{aligned}
$$

```go
func countCommas(n int64) (ans int64) {
	k := 5 // n == 1e15 时 Log10(n) 有误差，需要特判
	if n < 1e15 {
		k = int(math.Log10(float64(n))) / 3
	}
	return int64(k)*(n+1) - (int64(math.Pow10(k*3+3))-1000)/999
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面思维题单的「**§5.5 贡献法**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

## 本地原创解析

### 1. 题意重述

本题来自 `十、数位 DP / §10.2 统计合法元素的价值总和`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十、数位 DP / §10.2 统计合法元素的价值总和`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
