# 3439. 重新安排会议得到最多空余时间 I

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reschedule-meetings-for-maximum-free-time-i/
- 题目 slug：`reschedule-meetings-for-maximum-free-time-i`
- 来源专题：滑动窗口与双指针
- 来源分类路径：一、定长滑动窗口 / §1.2 进阶（选做）
- 难度分：1729
- 外部题解来源：https://leetcode.cn/problems/reschedule-meetings-for-maximum-free-time-i/solutions/3061619/zhuan-huan-cheng-ding-chang-hua-dong-chu-1kg1/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[转化成定长滑动窗口（Python/Java/C++/Go）](https://leetcode.cn/problems/reschedule-meetings-for-maximum-free-time-i/solutions/3061619/zhuan-huan-cheng-ding-chang-hua-dong-chu-1kg1/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuan-huan-cheng-ding-chang-hua-dong-chu-1kg1`
- topic id：`3061619`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![lc3439.png](https://pic.leetcode.cn/1738458316-BLfZXa-lc3439.png)

看示例 1：

- 如果把会议区间 $[1,2]$ 向左移动到 $[0,1]$，会产生空余时间段 $[1,3]$。
- 如果把会议区间 $[1,2]$ 向右移动到 $[2,3]$，会产生空余时间段 $[0,2]$。

这两种情况，都相当于把两个**相邻**的长为 $1$ 空余时间段 $[0,1]$ 和 $[2,3]$ **合并**成一个更长的长为 $1+1=2$ 的空余时间段。

如果 $k=1$，那么我们可以合并 $2$ 个相邻的空余时间段。

如果 $k=2$，为了让答案尽量大，合并连续 $3$ 个空余时间段，相比其他方案是最优的。（注意题目要求会议之间的相对顺序必须保持不变）

一般地，最优做法是合并连续 $k+1$ 个空余时间段。

现在问题变成：

- 给你 $n+1$ 个空余时间段，合并其中连续 $k+1$ 个空余时间段，得到的最大长度是多少？

> **注 1**：最左边和最右边各有一个空余时间段，中间有 $n-1$ 个空余时间段夹在两个相邻会议之间，所以有 $n+1$ 个空余时间段。
> 
> **注 2**：空余时间段的长度可以是 $0$。

这可以用**定长滑动窗口**解决，窗口大小为 $k+1$。原理见[【套路】教你解决定长滑窗！适用于所有定长滑窗题目！](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/solutions/2809359/tao-lu-jiao-ni-jie-jue-ding-chang-hua-ch-fzfo/)

[本题视频讲解](https://www.bilibili.com/video/BV1eUF6eaERQ/?t=38s)，欢迎点赞关注~

## 写法一

创建一个长为 $n+1$ 的数组，保存所有空余时间段的长度：

- 最左边的空余时间段：长为 $\textit{startTime}[0]$。
- 中间的空余时间段：长为 $\textit{startTime}[i] - \textit{endTime}[i-1]$。
- 最右边的空余时间段：长为 $\textit{eventTime} - \textit{endTime}[n-1]$。

示例 1 的数组为 $[1, 1, 0]$。

示例 2 的数组为 $[0, 1, 5, 0]$。

示例 3 的数组为 $[0, 0, 0, 0, 0, 0]$。

```py [sol-Python3]
class Solution:
    def maxFreeTime(self, eventTime: int, k: int, startTime: List[int], endTime: List[int]) -> int:
        n = len(startTime)
        free = [0] * (n + 1)
        free[0] = startTime[0]  # 最左边的空余时间段
        for i in range(1, n):
            free[i] = startTime[i] - endTime[i - 1]  # 中间的空余时间段
        free[n] = eventTime - endTime[-1]  # 最右边的空余时间段

        # 套定长滑窗模板（窗口长为 k+1）
        ans = s = 0
        for i, f in enumerate(free):
            s += f
            if i < k:
                continue
            ans = max(ans, s)
            s -= free[i - k]
        return ans
```

```py [sol-Python3 精简写法]
class Solution:
    def maxFreeTime(self, eventTime: int, k: int, startTime: List[int], endTime: List[int]) -> int:
        free = [startTime[0]] + [s - e for s, e in zip(startTime[1:], endTime)] + [eventTime - endTime[-1]]
        ans = s = 0
        for i, f in enumerate(free):
            s += f
            if i < k:
                continue
            ans = max(ans, s)
            s -= free[i - k]
        return ans
```

```java [sol-Java]
class Solution {
    public int maxFreeTime(int eventTime, int k, int[] startTime, int[] endTime) {
        int n = startTime.length;
        int[] free = new int[n + 1];
        free[0] = startTime[0]; // 最左边的空余时间段
        for (int i = 1; i < n; i++) {
            free[i] = startTime[i] - endTime[i - 1]; // 中间的空余时间段
        }
        free[n] = eventTime - endTime[n - 1]; // 最右边的空余时间段

        // 套定长滑窗模板（窗口长为 k+1）
        int ans = 0;
        int s = 0;
        for (int i = 0; i <= n; i++) {
            s += free[i];
            if (i < k) {
                continue;
            }
            ans = Math.max(ans, s);
            s -= free[i - k];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxFreeTime(int eventTime, int k, vector<int>& startTime, vector<int>& endTime) {
        int n = startTime.size();
        vector<int> free(n + 1);
        free[0] = startTime[0]; // 最左边的空余时间段
        for (int i = 1; i < n; i++) {
            free[i] = startTime[i] - endTime[i - 1]; // 中间的空余时间段
        }
        free[n] = eventTime - endTime[n - 1]; // 最右边的空余时间段

        // 套定长滑窗模板（窗口长为 k+1）
        int ans = 0, s = 0;
        for (int i = 0; i <= n; i++) {
            s += free[i];
            if (i < k) {
                continue;
            }
            ans = max(ans, s);
            s -= free[i - k];
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxFreeTime(eventTime, k int, startTime, endTime []int) (ans int) {
	n := len(startTime)
	free := make([]int, n+1)
	free[0] = startTime[0] // 最左边的空余时间段
	for i := 1; i < n; i++ {
		free[i] = startTime[i] - endTime[i-1] // 中间的空余时间段
	}
	free[n] = eventTime - endTime[n-1] // 最右边的空余时间段

	// 套定长滑窗模板（窗口长为 k+1）
	s := 0
	for i, f := range free {
		s += f
		if i < k {
			continue
		}
		ans = max(ans, s)
		s -= free[i-k]
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{startTime}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 写法二：空间优化

改用一个函数计算第 $i$ 个空余时间段的长度，从而省去 $\textit{free}$ 数组。

```py [sol-Python3]
class Solution:
    def maxFreeTime(self, eventTime: int, k: int, startTime: List[int], endTime: List[int]) -> int:
        def get(i: int) -> int:
            if i == 0:
                return startTime[0]  # 最左边的空余时间段
            if i == n:
                return eventTime - endTime[-1]  # 最右边的空余时间段
            return startTime[i] - endTime[i - 1]  # 中间的空余时间段

        n = len(startTime)
        ans = s = 0
        for i in range(n + 1):
            s += get(i)
            if i < k:
                continue
            ans = max(ans, s)
            s -= get(i - k)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxFreeTime(int eventTime, int k, int[] startTime, int[] endTime) {
        int ans = 0;
        int s = 0;
        for (int i = 0; i <= startTime.length; i++) {
            s += get(i, eventTime, startTime, endTime);
            if (i < k) {
                continue;
            }
            ans = Math.max(ans, s);
            s -= get(i - k, eventTime, startTime, endTime);
        }
        return ans;
    }

    private int get(int i, int eventTime, int[] startTime, int[] endTime) {
        if (i == 0) {
            return startTime[0]; // 最左边的空余时间段
        }
        int n = startTime.length;
        if (i == n) {
            return eventTime - endTime[n - 1]; // 最右边的空余时间段
        }
        return startTime[i] - endTime[i - 1]; // 中间的空余时间段
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxFreeTime(int eventTime, int k, vector<int>& startTime, vector<int>& endTime) {
        int n = startTime.size();
        auto get = [&](int i) -> int {
            if (i == 0) {
                return startTime[0]; // 最左边的空余时间段
            }
            if (i == n) {
                return eventTime - endTime[n - 1]; // 最右边的空余时间段
            }
            return startTime[i] - endTime[i - 1]; // 中间的空余时间段
        };

        int s = 0, ans = 0;
        for (int i = 0; i <= n; i++) {
            s += get(i);
            if (i < k) {
                continue;
            }
            ans = max(ans, s);
            s -= get(i - k);
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxFreeTime(eventTime, k int, startTime, endTime []int) (ans int) {
	n := len(startTime)
	get := func(i int) int {
		if i == 0 {
			return startTime[0] // 最左边的空余时间段
		}
		if i == n {
			return eventTime - endTime[n-1] // 最右边的空余时间段
		}
		return startTime[i] - endTime[i-1] // 中间的空余时间段
	}

	s := 0
	for i := range n + 1 {
		s += get(i)
		if i < k {
			continue
		}
		ans = max(ans, s)
		s -= get(i - k)
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{startTime}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、定长滑动窗口 / §1.2 进阶（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、定长滑动窗口 / §1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
