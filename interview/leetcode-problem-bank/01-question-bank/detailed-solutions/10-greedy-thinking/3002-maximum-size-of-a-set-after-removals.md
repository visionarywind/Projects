# 3002. 移除后集合的最多元素数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-size-of-a-set-after-removals/
- 题目 slug：`maximum-size-of-a-set-after-removals`
- 来源专题：贪心与思维
- 来源分类路径：八、其他
- 难度分：1917
- 外部题解来源：https://leetcode.cn/problems/maximum-size-of-a-set-after-removals/solutions/2594380/tan-xin-pythonjavacgo-by-endlesscheng-ymuh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种思考问题的角度：移除元素/添加元素（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-size-of-a-set-after-removals/solutions/2594380/tan-xin-pythonjavacgo-by-endlesscheng-ymuh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-pythonjavacgo-by-endlesscheng-ymuh`
- topic id：`2594380`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

[本题视频讲解](https://www.bilibili.com/video/BV1ae411e7fn/)

## 方法一：从「移除」的角度思考

#### 提示 1

设 $\textit{nums}_1$ 中有 $n_1$ 个不同元素，$\textit{nums}_2$ 中有 $n_2$ 个不同元素，它们的交集有 $\textit{common}$ 个元素。

如果不移除任何元素，根据容斥原理，$\textit{nums}_1$ 和 $\textit{nums}_2$ 的并集一共有

$$
\textit{ans} = n_1+n_2-\textit{common}
$$

个不同元素。

#### 提示 2

我们可以先移除每个数组中的重复元素，再考虑从剩下的数中移除元素。

设 $m = n/2$。对于 $\textit{nums}_1$ 来说，如果 $n_1 > m$，**先从交集中移除元素**：

- 如果交集元素少，那么全部移除，即移除 $\textit{common}$ 个元素。
- 如果交集元素多，那么移除交集中的 $n_1-m$ 个元素，就可以让 $n_1=m$。

所以要从交集中移除

$$
\min(n_1-m, \textit{common})
$$

个元素。

移除后，如果 $n_1$ 仍然大于 $m$，那么必须把 $\textit{ans}$ 减少 $n_1-m$。

对于 $\textit{nums}_2$ 也同理。

```py [sol-Python3]
class Solution:
    def maximumSetSize(self, nums1: List[int], nums2: List[int]) -> int:
        set1 = set(nums1)
        set2 = set(nums2)
        common = len(set1 & set2)

        n1 = len(set1)
        n2 = len(set2)
        ans = n1 + n2 - common

        m = len(nums1) // 2
        if n1 > m:
            mn = min(n1 - m, common)
            ans -= n1 - mn - m
            common -= mn

        if n2 > m:
            n2 -= min(n2 - m, common)
            ans -= n2 - m

        return ans
```

```java [sol-Java]
class Solution {
    public int maximumSetSize(int[] nums1, int[] nums2) {
        Set<Integer> set1 = new HashSet<>();
        for (int x : nums1) {
            set1.add(x);
        }
        Set<Integer> set2 = new HashSet<>();
        for (int x : nums2) {
            set2.add(x);
        }
        int common = 0;
        for (int x : set1) {
            if (set2.contains(x)) {
                common++;
            }
        }

        int n1 = set1.size();
        int n2 = set2.size();
        int ans = n1 + n2 - common;

        int m = nums1.length / 2;
        if (n1 > m) {
            int mn = Math.min(n1 - m, common);
            ans -= n1 - mn - m;
            common -= mn;
        }

        if (n2 > m) {
            n2 -= Math.min(n2 - m, common);
            ans -= n2 - m;
        }

        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSetSize(vector<int> &nums1, vector<int> &nums2) {
        unordered_set<int> set1(nums1.begin(), nums1.end());
        unordered_set<int> set2(nums2.begin(), nums2.end());
        int common = 0;
        for (int x : set1) {
            common += set2.count(x);
        }

        int n1 = set1.size();
        int n2 = set2.size();
        int ans = n1 + n2 - common;

        int m = nums1.size() / 2;
        if (n1 > m) {
            int mn = min(n1 - m, common);
            ans -= n1 - mn - m;
            common -= mn;
        }

        if (n2 > m) {
            n2 -= min(n2 - m, common);
            ans -= n2 - m;
        }

        return ans;
    }
};
```

```go [sol-Go]
func maximumSetSize(nums1, nums2 []int) int {
	set1 := map[int]bool{}
	for _, x := range nums1 {
		set1[x] = true
	}
	set2 := map[int]bool{}
	for _, x := range nums2 {
		set2[x] = true
	}
	common := 0
	for x := range set1 {
		if set2[x] {
			common++
		}
	}

	n1 := len(set1)
	n2 := len(set2)
	ans := n1 + n2 - common

	m := len(nums1) / 2
	if n1 > m {
		mn := min(n1-m, common)
		ans -= n1 - mn - m
		common -= mn
	}

	if n2 > m {
		n2 -= min(n2-m, common)
		ans -= n2 - m
	}

	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 为 $\textit{nums}_1$ 的长度，$m$ 为 $\textit{nums}_2$ 的长度。
- 空间复杂度：$\mathcal{O}(n+m)$。

## 方法二：从「添加」的角度思考

设 $\textit{nums}_1$ 中有 $n_1$ 个不同元素，$\textit{nums}_2$ 中有 $n_2$ 个不同元素，它们的交集有 $\textit{common}$ 个元素。

考虑怎么从两个数组中选择不同元素，添加到集合 $s$ 中，使 $s$ 的大小最大：

- 对于 $\textit{nums}_1$，优先选**不在**交集中的元素，这可以选 $n_1-\textit{common}$ 个，但不能超过题目规定的 $n/2$，所以至多选 $c_1 = \min(n_1-\textit{common}, n/2)$ 个不在交集中的元素。
- 对于 $\textit{nums}_2$，优先选**不在**交集中的元素，同理，至多选 $c_2 = \min(n_2-\textit{common}, n/2)$ 个。
- 由于都和 $n/2$ 取最小值，所以 $c_1 + c_2 \le n/2 + n/2 = n$。
- 如果 $c_1 + c_2 < n$，那么还可以再选 $n-c_1-c_2$ 个数，且这些数只能从交集中选，所以不能超过 $\textit{common}$ 个，所以还可以再选 $\min(n-c_1-c_2, \textit{common})$ 个数。

最终答案为

$$
\begin{aligned}
&c_1 + c_2 + \min(n-c_1-c_2, \textit{common})\\
=\ &\min(n, c_1 + c_2 + \textit{common})
\end{aligned}
$$

```py [sol-Python3]
class Solution:
    def maximumSetSize(self, nums1: List[int], nums2: List[int]) -> int:
        set1 = set(nums1)
        set2 = set(nums2)
        common = len(set1 & set2)
        n = len(nums1)
        c1 = min(len(set1) - common, n // 2)
        c2 = min(len(set2) - common, n // 2)
        return min(n, c1 + c2 + common)
```

```java [sol-Java]
class Solution {
    public int maximumSetSize(int[] nums1, int[] nums2) {
        Set<Integer> set1 = new HashSet<>();
        for (int x : nums1) {
            set1.add(x);
        }
        Set<Integer> set2 = new HashSet<>();
        int common = 0;
        for (int x : nums2) {
            if (set2.contains(x)) {
                continue;
            }
            set2.add(x);
            // 相比方法一，这样写会略快一点
            if (set1.contains(x)) {
                common++;
            }
        }

        int n = nums1.length;
        int c1 = Math.min(set1.size() - common, n / 2);
        int c2 = Math.min(set2.size() - common, n / 2);
        return Math.min(n, c1 + c2 + common);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSetSize(vector<int> &nums1, vector<int> &nums2) {
        unordered_set<int> set1(nums1.begin(), nums1.end());
        unordered_set<int> set2(nums2.begin(), nums2.end());
        int common = 0;
        for (int x : set1) {
            common += set2.count(x);
        }

        int n = nums1.size();
        int c1 = min((int) set1.size() - common, n / 2);
        int c2 = min((int) set2.size() - common, n / 2);
        return min(n, c1 + c2 + common);
    }
};
```

```go [sol-Go]
func maximumSetSize(nums1, nums2 []int) int {
	set1 := map[int]bool{}
	for _, x := range nums1 {
		set1[x] = true
	}
	set2 := map[int]bool{}
	common := 0
	for _, x := range nums2 {
		if set2[x] {
			continue
		}
		set2[x] = true
		// 另一种求 common 的写法
		if set1[x] {
			common++
		}
	}

	n := len(nums1)
	c1 := min(len(set1)-common, n/2)
	c2 := min(len(set2)-common, n/2)
	return min(n, c1+c2+common)
}
```

还有一种理解方式：

- 从 $\textit{nums}_1$ 中选择不超过 $n/2$ 个元素，并让这些元素尽量与 $\textit{nums}_2$ 没有交集。这可以选 $c_1=\min(n_1, n/2)$ 个。
- 从 $\textit{nums}_2$ 中选择不超过 $n/2$ 个元素，并让这些元素尽量与 $\textit{nums}_1$ 没有交集。这可以选 $c_2=\min(n_2, n/2)$ 个。
- 如果这两部分没有交集，那么答案就是 $c_1+c_2$。
- 如果这两部分有交集，那么答案就是 $\textit{nums}_1$ 和 $\textit{nums}_2$ 的**并集**的大小。
- 这两种情况取最小值。

```py [sol-Python3]
class Solution:
    def maximumSetSize(self, nums1: List[int], nums2: List[int]) -> int:
        set1 = set(nums1)
        set2 = set(nums2)
        n = len(nums1)
        c1 = min(len(set1), n // 2)
        c2 = min(len(set2), n // 2)
        return min(len(set1 | set2), c1 + c2)
```

```java [sol-Java]
class Solution {
    public int maximumSetSize(int[] nums1, int[] nums2) {
        Set<Integer> set1 = new HashSet<>();
        for (int x : nums1) {
            set1.add(x);
        }
        int all = set1.size();
        Set<Integer> set2 = new HashSet<>();
        for (int x : nums2) {
            if (set2.contains(x)) {
                continue;
            }
            set2.add(x);
            if (!set1.contains(x)) {
                all++;
            }
        }

        int n = nums1.length;
        int c1 = Math.min(set1.size(), n / 2);
        int c2 = Math.min(set2.size(), n / 2);
        return Math.min(all, c1 + c2);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSetSize(vector<int> &nums1, vector<int> &nums2) {
        unordered_set<int> set1(nums1.begin(), nums1.end());
        unordered_set<int> set2(nums2.begin(), nums2.end());
        int all = set1.size() + set2.size();
        for (int x : set1) {
            all -= set2.count(x); // 去掉重复的
        }

        int n = nums1.size();
        int c1 = min((int) set1.size(), n / 2);
        int c2 = min((int) set2.size(), n / 2);
        return min(all, c1 + c2);
    }
};
```

```go [sol-Go]
func maximumSetSize(nums1, nums2 []int) int {
	set1 := map[int]bool{}
	for _, x := range nums1 {
		set1[x] = true
	}
	all := len(set1)
	set2 := map[int]bool{}
	for _, x := range nums2 {
		if set2[x] {
			continue
		}
		set2[x] = true
		if !set1[x] {
			all++
		}
	}

	n := len(nums1)
	c1 := min(len(set1), n/2)
	c2 := min(len(set2), n/2)
	return min(all, c1+c2)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+m)$，其中 $n$ 为 $\textit{nums}_1$ 的长度，$m$ 为 $\textit{nums}_2$ 的长度。
- 空间复杂度：$\mathcal{O}(n+m)$。

周赛总结更新啦！请看 [2023 下半年周赛题目总结](https://leetcode.cn/circle/discuss/lUu0KB/)

## 本地原创解析

### 1. 题意重述

本题来自 `八、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
