# 3752. 字典序最小和为目标值且绝对值是排列的数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/lexicographically-smallest-negated-permutation-that-sums-to-target/
- 题目 slug：`lexicographically-smallest-negated-permutation-that-sums-to-target`
- 来源专题：贪心与思维
- 来源分类路径：三、字符串贪心 / §3.1 字典序最小/最大
- 难度分：1827
- 外部题解来源：https://leetcode.cn/problems/lexicographically-smallest-negated-permutation-that-sums-to-target/solutions/3839568/tan-xin-on-zuo-fa-pythonjavacgo-by-endle-7ojw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[字典序贪心 + 可行性证明，O(n) 做法（Python/Java/C++/Go）](https://leetcode.cn/problems/lexicographically-smallest-negated-permutation-that-sums-to-target/solutions/3839568/tan-xin-on-zuo-fa-pythonjavacgo-by-endle-7ojw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-on-zuo-fa-pythonjavacgo-by-endle-7ojw`
- topic id：`3839568`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

## 题意

有一个 $1$ 到 $n$ 的排列（顺序由你指定），你需要给其中的某些元素添加负号（变成相反数），得到一个数组 $a$，满足 $a$ 的所有元素之和恰好等于 $\textit{target}$，且字典序尽量小。

## 分析

设 $S = 1+2+\cdots +n = \dfrac{n(n+1)}{2}$，没有添加负号的元素之和为 $\textit{posS}$，添加负号的元素（的绝对值）之和为 $\textit{negS}$，那么有

$$
\textit{posS} + \textit{negS} = S
$$

又因为所有元素之和等于 $\textit{target}$，所以有

$$
\textit{posS} - \textit{negS} = \textit{target}
$$

解得

$$
\textit{negS} = \dfrac{S - \textit{target}}{2}
$$

所以 $S - \textit{target}$ 必须是偶数。

又由于 $0\le \textit{negS}\le S$，所以还要满足 $-S\le \textit{target} \le S$，即 $|\textit{target}|\le S$。

⚠**注意**：要让字典序最小，第一个数越小越好。所以负数要填在前面，所以本题是**负数主导**的，应围绕 $\textit{negS}$ 贪心，而不是 $\textit{posS}$。

## 字典序贪心

如果 $\textit{negS} > 0$，那么第一个数可以选 $\le \textit{negS}$ 的最大的数 $x$，这样 $-x$ 就是最小的。

然后把 $\textit{negS}$ 减少 $x$，重复上述步骤，直到 $\textit{negS}=0$ 为止。可以倒着枚举 $x = n,n-1,\ldots,1$ 实现。

剩余未选元素（不添加负号）从小到大排列。

## 答疑

**问**：在 $0\le \textit{negS}\le S$ 的情况下，为什么一定可以从 $1,2,\ldots,n$ 中选出元素和恰好等于 $\textit{negS}$ 的子集？

**答**：从 $1,2,\ldots,n$ 中选一些数（也可以不选），可以得到 $[0,S]$ 中的任意整数。我们可以从 $1$ 开始分析：

- 一开始只有 $1$，能得到的元素和为 $0,1$。
- 考虑 $2$ 选或不选：不选，能得到的元素和为 $0,1$；选，能得到的元素和为 $2,3$。合并得 $0,1,2,3$。
- 考虑 $3$ 选或不选：不选，能得到的元素和为 $0,1,2,3$；选，能得到的元素和为 $3,4,5,6$。合并得 $0,1,2,3,4,5,6$。
- 依此类推。
- 注意每次考虑一个数 $x\ (x\ge 2)$ 选或不选的时候，一定满足 $x-1\le$ 前面所有元素之和 $1+2+\cdots + x-1 = \dfrac{x(x-1)}{2}$，所以合并之后元素和范围仍然是连续的。

一般地，可以用**数学归纳法**证明，我们可以得到 $[0,S]$ 中的任意整数。

**问**：在有解的情况下，为什么按照贪心算法一定能得到一个解？

**答**：如果 $\textit{negS}\ge n$，我们选 $n$。由于 $\textit{negS}\le \dfrac{n(n+1)}{2}$，所以 $\textit{negS}-n\le 1+2+\cdots +(n-1) = \dfrac{n(n-1)}{2}$ 同样成立，这意味着从剩余元素中可以得到解。如果 $\textit{negS}< n$，即 $\textit{negS}\le n-1$，那么不选 $n$，$\textit{negS}\le n-1\le \dfrac{n(n-1)}{2}$ 也成立。所以贪心算法一定能得到一个解。

[本题视频讲解](https://www.bilibili.com/video/BV1fbUKBqEa7/?t=3m54s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def lexSmallestNegatedPerm(self, n: int, target: int) -> List[int]:
        mx = n * (n + 1) // 2
        if abs(target) > mx or (mx - target) % 2:
            return []
        neg_s = (mx - target) // 2  # 取负号的元素（的绝对值）之和

        ans = [0] * n
        l, r = 0, n - 1
        # 从 1,2,...,n 中选一些数，元素和等于 neg_s
        # 为了让负数部分的字典序尽量小，从大往小选
        for x in range(n, 0, -1):
            if neg_s >= x:
                neg_s -= x
                ans[l] = -x
                l += 1
            else:
                # 大的正数填在末尾
                ans[r] = x
                r -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public int[] lexSmallestNegatedPerm(int n, long target) {
        long mx = (long) n * (n + 1) / 2;
        if (Math.abs(target) > mx || (mx - target) % 2 != 0) {
            return new int[]{};
        }
        long negS = (mx - target) / 2; // 取负号的元素（的绝对值）之和

        int[] ans = new int[n];
        int l = 0;
        int r = n - 1;
        // 从 1,2,...,n 中选一些数，元素和等于 negS
        // 为了让负数部分的字典序尽量小，从大往小选
        for (int x = n; x > 0; x--) {
            if (negS >= x) {
                negS -= x;
                ans[l++] = -x;
            } else {
                // 大的正数填在末尾
                ans[r--] = x;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> lexSmallestNegatedPerm(int n, long long target) {
        long long mx = 1LL * n * (n + 1) / 2;
        if (abs(target) > mx || (mx - target) % 2) {
            return {};
        }
        long long neg_s = (mx - target) / 2; // 取负号的元素（的绝对值）之和

        vector<int> ans(n);
        int l = 0, r = n - 1;
        // 从 1,2,...,n 中选一些数，元素和等于 neg_s
        // 为了让负数部分的字典序尽量小，从大往小选
        for (int x = n; x > 0; x--) {
            if (neg_s >= x) {
                neg_s -= x;
                ans[l++] = -x;
            } else {
                // 大的正数填在末尾
                ans[r--] = x;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func lexSmallestNegatedPerm(n int, target int64) []int {
	t := int(target)
	mx := n * (n + 1) / 2
	if t > mx || -t > mx || (mx-t)%2 != 0 {
		return nil
	}
	negS := (mx - t) / 2 // 取负号的元素（的绝对值）之和

	ans := make([]int, n)
	l, r := 0, n-1
	// 从 1,2,...,n 中选一些数，元素和等于 negS
	// 为了让负数部分的字典序尽量小，从大往小选
	for x := n; x > 0; x-- {
		if negS >= x {
			negS -= x
			ans[l] = -x
			l++
		} else {
			// 大的正数填在末尾
			ans[r] = x
			r--
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。返回值不计入。

## 相似题目

[494. 目标和](https://leetcode.cn/problems/target-sum/)

## 专题训练

见下面贪心与思维题单的「**§3.1 字典序最小/最大**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `三、字符串贪心 / §3.1 字典序最小/最大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、字符串贪心 / §3.1 字典序最小/最大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
