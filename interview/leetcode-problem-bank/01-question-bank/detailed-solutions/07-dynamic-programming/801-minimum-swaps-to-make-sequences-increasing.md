# 801. 使序列递增的最小交换次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-swaps-to-make-sequences-increasing/
- 题目 slug：`minimum-swaps-to-make-sequences-increasing`
- 来源专题：动态规划
- 来源分类路径：六、状态机 DP / §6.3 进阶
- 难度分：2066
- 外部题解来源：https://leetcode.cn/problems/minimum-swaps-to-make-sequences-increasing/solutions/1880884/dong-tai-gui-hua-kao-lu-xiang-lin-yuan-s-ni0p/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划：考虑相邻元素的大小关系（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-swaps-to-make-sequences-increasing/solutions/1880884/dong-tai-gui-hua-kao-lu-xiang-lin-yuan-s-ni0p/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-kao-lu-xiang-lin-yuan-s-ni0p`
- topic id：`1880884`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

#### 提示 1

把**严格递增**换一种方式表述：对所有 $i\ (i>0)$，有 $\textit{nums}_1[i-1]<\textit{nums}_1[i]$ 且 $\textit{nums}_2[i-1]<\textit{nums}_2[i]$。

从这一表述可以发现，我们只需要考虑**相邻**元素间的大小关系。这意味着我们可以从左到右**递推**计算出答案。

#### 提示 2

考虑状态如何定义。由于当前元素的「交换」与「不交换」会影响到后续元素的「交换」与「不交换」，因此需要在下标 $i$ 的基础上，**额外加上一个维度表示是否发生交换**。

具体地，定义 $f[i][0/1]$ 表示让 $\textit{nums}_1$ 和 $\textit{nums}_2$ 的前 $i$ 个元素严格递增所需操作的最小次数，其中 $f[i][0]$ 表示不交换 $\textit{nums}_1[i]$ 和 $\textit{nums}_2[i]$，$f[i][1]$ 表示交换 $\textit{nums}_1[i]$ 和 $\textit{nums}_2[i]$。

记 $a_1=\textit{nums}_1[i-1],\ a_2=\textit{nums}_1[i],\ b_1=\textit{nums}_2[i-1],\ b_2=\textit{nums}_2[i]$。在计算 $f[i][0/1]$ 时，根据这四个数的大小关系来决定转移来源，分类讨论如下：

- 如果 $a_1 < a_2$ 且 $b_1 < b_2$，那么这两对数可以都不交换，即 $f[i][0] = f[i-1][0]$；也可以都交换，即 $f[i][1] = f[i-1][1] + 1$。
- 如果 $b_1 < a_2$ 且 $a_1 < b_2$，那么可以交换其中一对，即 $f[i][0] = f[i-1][1]$，$f[i][1] = f[i-1][0] + 1$。
- 如果同时满足上述两种情况，则在转移时取较小值。

最后答案为 $\min(f[n-1][0], f[n-1][1])$。

```py [sol1-Python3]
class Solution:
    def minSwap(self, nums1: List[int], nums2: List[int]) -> int:
        n = len(nums1)
        # f[i][0/1] 表示让 nums1 和 nums2 的前 i 个数严格递增所需操作的最小次数
        # 其中 f[i][0] 不交换 nums1[i] 和 nums2[i]，f[i][1] 交换 nums1[i] 和 nums2[i]
        f = [[inf, inf] for _ in range(n)]
        f[0] = [0, 1]
        for i in range(1, n):
            if nums1[i - 1] < nums1[i] and nums2[i - 1] < nums2[i]:
                f[i][0] = f[i - 1][0]
                f[i][1] = f[i - 1][1] + 1
            if nums2[i - 1] < nums1[i] and nums1[i - 1] < nums2[i]:
                f[i][0] = min(f[i][0], f[i - 1][1])
                f[i][1] = min(f[i][1], f[i - 1][0] + 1)
        return min(f[-1])
```

```java [sol1-Java]
class Solution {
    public int minSwap(int[] nums1, int[] nums2) {
        // f[i][0/1] 表示让 nums1 和 nums2 的前 i 个数严格递增所需操作的最小次数
        // 其中 f[i][0] 不交换 nums1[i] 和 nums2[i]，f[i][1] 交换 nums1[i] 和 nums2[i]
        var n = nums1.length;
        var f = new int[n][2];
        f[0][1] = 1;
        for (var i = 1; i < n; i++) {
            f[i][0] = n; // 答案不会超过 n，故初始化成 n 方便后面取 min
            f[i][1] = n;
            if (nums1[i - 1] < nums1[i] && nums2[i - 1] < nums2[i]) {
                f[i][0] = f[i - 1][0];
                f[i][1] = f[i - 1][1] + 1;
            }
            if (nums2[i - 1] < nums1[i] && nums1[i - 1] < nums2[i]) {
                f[i][0] = Math.min(f[i][0], f[i - 1][1]);
                f[i][1] = Math.min(f[i][1], f[i - 1][0] + 1);
            }
        }
        return Math.min(f[n - 1][0], f[n - 1][1]);
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int minSwap(vector<int> &nums1, vector<int> &nums2) {
        // f[i][0/1] 表示让 nums1 和 nums2 的前 i 个数严格递增所需操作的最小次数
        // 其中 f[i][0] 不交换 nums1[i] 和 nums2[i]，f[i][1] 交换 nums1[i] 和 nums2[i]
        int n = nums1.size(), f[n][2];
        memset(f, 0x3f, sizeof(f));
        f[0][0] = 0, f[0][1] = 1;
        for (int i = 1; i < n; i++) {
            if (nums1[i - 1] < nums1[i] && nums2[i - 1] < nums2[i]) {
                f[i][0] = f[i - 1][0];
                f[i][1] = f[i - 1][1] + 1;
            }
            if (nums2[i - 1] < nums1[i] && nums1[i - 1] < nums2[i]) {
                f[i][0] = min(f[i][0], f[i - 1][1]);
                f[i][1] = min(f[i][1], f[i - 1][0] + 1);
            }
        }
        return min(f[n - 1][0], f[n - 1][1]);
    }
};
```

```go [sol1-Go]
func minSwap(nums1, nums2 []int) int {
	n := len(nums1)
	// f[i][0/1] 表示让 nums1 和 nums2 的前 i 个数严格递增所需操作的最小次数
	// 其中 f[i][0] 不交换 nums1[i] 和 nums2[i]，f[i][1] 交换 nums1[i] 和 nums2[i]
	f := make([][2]int, n)
	f[0][1] = 1
	for i := 1; i < n; i++ {
		f[i] = [2]int{n, n} // 答案不会超过 n，故初始化成 n 方便后面取 min
		if nums1[i-1] < nums1[i] && nums2[i-1] < nums2[i] {
			f[i][0] = f[i-1][0]
			f[i][1] = f[i-1][1] + 1
		}
		if nums2[i-1] < nums1[i] && nums1[i-1] < nums2[i] {
			f[i][0] = min(f[i][0], f[i-1][1])
			f[i][1] = min(f[i][1], f[i-1][0]+1)
		}
	}
	return min(f[n-1][0], f[n-1][1])
}

func min(a, b int) int { if a > b { return b }; return a }
```

由于状态转移只发生在 $i-1$ 和 $i$ 之间，因此可以只用两个变量表示上述状态转移过程。

```py [sol2-Python3]
class Solution:
    def minSwap(self, nums1: List[int], nums2: List[int]) -> int:
        not_swap, swap = 0, 1
        for (a1, b1), (a2, b2) in pairwise(zip(nums1, nums2)):  # 3.10 from itertools import pairwise
            ns = s = inf
            if a1 < a2 and b1 < b2:
                ns, s = not_swap, swap + 1
            if b1 < a2 and a1 < b2:
                ns, s = min(ns, swap), min(s, not_swap + 1)
            not_swap, swap = ns, s
        return min(not_swap, swap)
```

```java [sol2-Java]
class Solution {
    public int minSwap(int[] nums1, int[] nums2) {
        int n = nums1.length, notSwap = 0, swap = 1;
        for (var i = 1; i < n; ++i) {
            int ns = n, s = n; // 答案不会超过 n，故初始化成 n 方便后面取 min
            if (nums1[i - 1] < nums1[i] && nums2[i - 1] < nums2[i]) {
                ns = notSwap;
                s = swap + 1;
            }
            if (nums2[i - 1] < nums1[i] && nums1[i - 1] < nums2[i]) {
                ns = Math.min(ns, swap);
                s = Math.min(s, notSwap + 1);
            }
            notSwap = ns;
            swap = s;
        }
        return Math.min(notSwap, swap);
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int minSwap(vector<int> &nums1, vector<int> &nums2) {
        int n = nums1.size(), not_swap = 0, swap = 1;
        for (int i = 1; i < n; ++i) {
            int ns = n, s = n; // 答案不会超过 n，故初始化成 n 方便后面取 min
            if (nums1[i - 1] < nums1[i] && nums2[i - 1] < nums2[i]) {
                ns = not_swap;
                s = swap + 1;
            }
            if (nums2[i - 1] < nums1[i] && nums1[i - 1] < nums2[i]) {
                ns = min(ns, swap);
                s = min(s, not_swap + 1);
            }
            not_swap = ns;
            swap = s;
        }
        return min(not_swap, swap);
    }
};
```

```go [sol2-Go]
func minSwap(nums1, nums2 []int) int {
	n, notSwap, swap := len(nums1), 0, 1
	for i := 1; i < n; i++ {
		ns, s := n, n // 答案不会超过 n，故初始化成 n 方便后面取 min
		if nums1[i-1] < nums1[i] && nums2[i-1] < nums2[i] {
			ns, s = notSwap, swap+1
		}
		if nums2[i-1] < nums1[i] && nums1[i-1] < nums2[i] {
			ns, s = min(ns, swap), min(s, notSwap+1)
		}
		notSwap, swap = ns, s
	}
	return min(notSwap, swap)
}

func min(a, b int) int { if a > b { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$O(n)$，其中 $n$ 是 $\textit{nums}_1$ 的长度。
- 空间复杂度：$O(1)$，仅用到若干额外变量。

---

欢迎关注我的B站频道：[灵茶山艾府](https://space.bilibili.com/206214)，定期更新算法讲解视频哦~

## 本地原创解析

### 1. 题意重述

本题来自 `六、状态机 DP / §6.3 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、状态机 DP / §6.3 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
