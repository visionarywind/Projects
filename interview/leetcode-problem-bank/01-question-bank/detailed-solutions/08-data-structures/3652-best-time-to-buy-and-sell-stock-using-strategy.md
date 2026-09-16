# 3652. 按策略买卖股票的最佳时机

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-using-strategy/
- 题目 slug：`best-time-to-buy-and-sell-stock-using-strategy`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.1 基础
- 难度分：1557
- 外部题解来源：https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-using-strategy/solutions/3755330/liang-chong-fang-fa-qian-zhui-he-ding-ch-uq98/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：前缀和 / 定长滑动窗口（Python/Java/C++/Go）](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-using-strategy/solutions/3755330/liang-chong-fang-fa-qian-zhui-he-ding-ch-uq98/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-qian-zhui-he-ding-ch-uq98`
- topic id：`3755330`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：前缀和

计算两个前缀和数组：

- 定义数组 $c$，其中 $c[i] = \textit{prices}[i]\cdot \textit{strategy}[i]$。计算 $c$ 的前缀和，记作 $\textit{sum}$。
- 计算 $\textit{prices}$ 的前缀和，记作 $\textit{sumSell}$。
- 关于前缀和数组的详细定义，请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

如果不修改，答案为 $\textit{sum}[n]$。

如果修改，枚举修改子数组 $[i-k,i-1]$。修改后的利润由三部分组成：

1. $[0,i-k-1]$ 的 $\textit{prices}[i]\cdot \textit{strategy}[i]$ 之和，即 $\textit{sum}[i-k]$。
2. $[i,n-1]$ 的 $\textit{prices}[i]\cdot \textit{strategy}[i]$ 之和，即 $\textit{sum}[n] - \textit{sum}[i]$。
3. $[i-k/2,i-1]$ 的 $\textit{prices}[i]$ 之和，即 $\textit{sumSell}[i] - \textit{sumSell}[i-k/2]$。

总和为

$$
\textit{sum}[i-k] + \textit{sum}[n] - \textit{sum}[i] + \textit{sumSell}[i] - \textit{sumSell}[i-k/2]
$$

用上式更新答案的最大值。

[本题视频讲解](https://www.bilibili.com/video/BV1kTYyzwEDD/?t=29m23s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxProfit(self, prices: List[int], strategy: List[int], k: int) -> int:
        n = len(prices)
        s = list(accumulate((p * s for p, s in zip(prices, strategy)), initial=0))
        s_sell = list(accumulate(prices, initial=0))

        # 修改一次
        ans = max(s[i - k] + s[n] - s[i] + s_sell[i] - s_sell[i - k // 2] for i in range(k, n + 1))
        return max(ans, s[n])  # 不修改
```

```java [sol-Java]
class Solution {
    public long maxProfit(int[] prices, int[] strategy, int k) {
        int n = prices.length;
        long[] sum = new long[n + 1];
        long[] sumSell = new long[n + 1];
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i] + prices[i] * strategy[i];
            sumSell[i + 1] = sumSell[i] + prices[i];
        }

        long ans = sum[n]; // 不修改
        for (int i = k; i <= n; i++) {
            long res = sum[i - k] + sum[n] - sum[i] + sumSell[i] - sumSell[i - k / 2];
            ans = Math.max(ans, res);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxProfit(vector<int>& prices, vector<int>& strategy, int k) {
        int n = prices.size();
        vector<long long> sum(n + 1), sum_sell(n + 1);
        for (int i = 0; i < n; i++) {
            sum[i + 1] = sum[i] + prices[i] * strategy[i];
            sum_sell[i + 1] = sum_sell[i] + prices[i];
        }

        long long ans = sum[n]; // 不修改
        for (int i = k; i <= n; i++) {
            long long res = sum[i - k] + sum[n] - sum[i] + sum_sell[i] - sum_sell[i - k / 2];
            ans = max(ans, res);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxProfit(prices []int, strategy []int, k int) int64 {
	n := len(prices)
	sum := make([]int, n+1)
	sumSell := make([]int, n+1)
	for i, p := range prices {
		sum[i+1] = sum[i] + p*strategy[i]
		sumSell[i+1] = sumSell[i] + p
	}

	ans := sum[n] // 不修改
	for i := k; i <= n; i++ {
		res := sum[i-k] + sum[n] - sum[i] + sumSell[i] - sumSell[i-k/2]
		ans = max(ans, res)
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{prices}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：定长滑动窗口

**前置知识**：[定长滑动窗口](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)。

设 $\textit{total}$ 为不修改时的总利润。

如果修改，枚举修改的位置，视作两个长度均为 $k/2$ 的**定长滑动窗口**紧挨着同时向右滑动。计算修改操作可以让总利润相比 $\textit{total}$ **额外增加多少**。

- 对于左边的窗口，进入窗口的元素的交易策略从 $\textit{strategy}[i]$ 变成了 $0$，利润增加了 $(0-\textit{strategy}[i]) \cdot \textit{prices}[i]$。
- 对于右边的窗口，进入窗口的元素的交易策略从 $\textit{strategy}[i]$ 变成了 $1$，利润增加了 $(1-\textit{strategy}[i]) \cdot \textit{prices}[i]$。

```py [sol-Python3]
class Solution:
    def maxProfit(self, prices: List[int], strategy: List[int], k: int) -> int:
        total = extra = max_extra = 0
        m = k // 2
        for i, (s, p) in enumerate(zip(strategy, prices)):
            total += s * p
            if i < m:
                continue

            # 1. 入
            extra -= strategy[i - m] * prices[i - m]  # 前 k/2 个元素的窗口
            extra += (1 - s) * p  # 后 k/2 个元素的窗口

            left = i - k + 1
            if left < 0:  # 尚未形成第一个窗口
                continue

            # 2. 更新
            max_extra = max(max_extra, extra)

            # 3. 出（计算方式和入相反）
            extra += strategy[left] * prices[left]  # 前 k/2 个元素的窗口
            extra -= (1 - strategy[left + m]) * prices[left + m]  # 后 k/2 个元素的窗口

        return total + max_extra
```

```java [sol-Java]
class Solution {
    public long maxProfit(int[] prices, int[] strategy, int k) {
        long total = 0;
        long extra = 0;
        long maxExtra = 0;

        for (int i = 0; i < prices.length; i++) {
            total += strategy[i] * prices[i];
            if (i < k / 2) {
                continue;
            }

            // 1. 入
            extra -= strategy[i - k / 2] * prices[i - k / 2]; // 前 k/2 个元素的窗口
            extra += (1 - strategy[i]) * prices[i]; // 后 k/2 个元素的窗口

            int left = i - k + 1;
            if (left < 0) { // 尚未形成第一个窗口
                continue;
            }

            // 2. 更新
            maxExtra = Math.max(maxExtra, extra);

            // 3. 出（计算方式和入相反）
            extra += strategy[left] * prices[left]; // 前 k/2 个元素的窗口
            extra -= (1 - strategy[left + k / 2]) * prices[left + k / 2]; // 后 k/2 个元素的窗口
        }

        return total + maxExtra;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxProfit(vector<int>& prices, vector<int>& strategy, int k) {
        long long total = 0;
        long long extra = 0;
        long long max_extra = 0;

        for (int i = 0; i < prices.size(); i++) {
            total += strategy[i] * prices[i];
            if (i < k / 2) {
                continue;
            }

            // 1. 入
            extra -= strategy[i - k / 2] * prices[i - k / 2]; // 前 k/2 个元素的窗口
            extra += (1 - strategy[i]) * prices[i]; // 后 k/2 个元素的窗口

            int left = i - k + 1;
            if (left < 0) { // 尚未形成第一个窗口
                continue;
            }

            // 2. 更新
            max_extra = max(max_extra, extra);

            // 3. 出（计算方式和入相反）
            extra += strategy[left] * prices[left]; // 前 k/2 个元素的窗口
            extra -= (1 - strategy[left + k / 2]) * prices[left + k / 2]; // 后 k/2 个元素的窗口
        }

        return total + max_extra;
    }
};
```

```go [sol-Go]
func maxProfit(prices, strategy []int, k int) int64 {
	var total, extra, maxExtra int
	for i, p := range prices {
		total += strategy[i] * p
		if i < k/2 {
			continue
		}

		// 1. 入
		extra -= strategy[i-k/2] * prices[i-k/2] // 前 k/2 个元素的窗口
		extra += (1 - strategy[i]) * p           // 后 k/2 个元素的窗口

		left := i - k + 1
		if left < 0 { // 尚未形成第一个窗口
			continue
		}

		// 2. 更新
		maxExtra = max(maxExtra, extra)

		// 3. 出（计算方式和入相反）
		extra += strategy[left] * prices[left]               // 前 k/2 个元素的窗口
		extra -= (1 - strategy[left+k/2]) * prices[left+k/2] // 后 k/2 个元素的窗口
	}
	return int64(total + maxExtra)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{prices}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

1. 数据结构题单的「**一、前缀和**」。
2. 滑动窗口题单的「**一、定长滑动窗口**」。

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

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、前缀和 / §1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
