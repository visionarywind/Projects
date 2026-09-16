# 2926. 平衡子序列的最大和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-balanced-subsequence-sum/
- 题目 slug：`maximum-balanced-subsequence-sum`
- 来源专题：动态规划
- 来源分类路径：十一、优化 DP / §11.4 树状数组/线段树优化 DP
- 难度分：2448
- 外部题解来源：https://leetcode.cn/problems/maximum-balanced-subsequence-sum/solutions/2513121/shu-zhuang-shu-zu-you-hua-dp-by-endlessc-3zf4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[树状数组优化 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-balanced-subsequence-sum/solutions/2513121/shu-zhuang-shu-zu-you-hua-dp-by-endlessc-3zf4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shu-zhuang-shu-zu-you-hua-dp-by-endlessc-3zf4`
- topic id：`2513121`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

请看 [视频讲解](https://www.bilibili.com/video/BV1Fc411R7xA/) 第四题。

定义 $b[i] = \textit{nums}[i] - i$，问题变成从 $b$ 中选出一个非降子序列，求对应的 $\textit{nums}$ 的元素和的最大值。

如果 $i$ 是子序列最后一个数的下标，考虑倒数第二个数的下标 $j$，如果 $b[j]\le b[i]$，那么就找到了一个子问题：子序列最后一个数的下标是 $j$ 时，对应的 $\textit{nums}$ 的元素和的最大值。

据此，定义 $f[i]$ 表示子序列最后一个数的下标是 $i$ 时，对应的 $\textit{nums}$ 的元素和的最大值。那么答案就是 $\max(f)$。

状态转移方程为

$$
f[i] = \max_{j} \max(f[j],0) + \textit{nums}[i] 
$$

其中 $j$ 满足 $j < i$ 且 $b[j]\le b[i]$。如果 $f[j]<0$，则和 $0$ 取最大值，表示只选 $\textit{nums}[i]$ 一个数，前面的数都不选。

这可以用**权值树状数组**（或者权值线段树）来优化。树状数组用来维护前缀最大值：设下标为 $x=b[i]$，维护的值为 $\max(f[x], f[x-1], f[x-2], \cdots)$。具体请看视频讲解。

代码实现时需要先把 $\textit{nums}[i]-i$ **离散化**，再使用树状数组。

> 离散化：把最小元素映射为 $1$，次小元素映射为 $2$，依此类推。

```py [sol-Python3]
class Solution:
    def maxBalancedSubsequenceSum(self, nums: List[int]) -> int:
        b = sorted(set(x - i for i, x in enumerate(nums)))  # 离散化 nums[i]-i
        t = BIT(len(b) + 1)
        for i, x in enumerate(nums):
            j = bisect_left(b, x - i) + 1  # nums[i]-i 离散化后的值（从 1 开始）
            f = max(t.pre_max(j), 0) + x
            t.update(j, f)
        return t.pre_max(len(b))  # 所有 f 的最大值

# 树状数组模板（维护前缀最大值）
class BIT:
    def __init__(self, n: int):
        self.tree = [-inf] * n

    def update(self, i: int, val: int) -> None:
        while i < len(self.tree):
            self.tree[i] = max(self.tree[i], val)
            i += i & -i

    def pre_max(self, i: int) -> int:
        mx = -inf
        while i > 0:
            mx = max(mx, self.tree[i])
            i &= i - 1
        return mx
```

```java [sol-Java]
class Solution {
    public long maxBalancedSubsequenceSum(int[] nums) {
        int n = nums.length;
        int[] b = new int[n];
        for (int i = 0; i < n; i++) {
            b[i] = nums[i] - i;
        }
        Arrays.sort(b);
 
        BIT t = new BIT(b.length + 1);
        for (int i = 0; i < n; i++) {
            // j 为 nums[i]-i 离散化后的值（从 1 开始）
            int j = Arrays.binarySearch(b, nums[i] - i) + 1;
            long f = Math.max(t.preMax(j), 0) + nums[i];
            t.update(j, f);
        }
        return t.preMax(b.length);
    }
}

// 树状数组模板（维护前缀最大值）
class BIT {
    private long[] tree;

    public BIT(int n) {
        tree = new long[n];
        Arrays.fill(tree, Long.MIN_VALUE);
    }

    public void update(int i, long val) {
        while (i < tree.length) {
            tree[i] = Math.max(tree[i], val);
            i += i & -i;
        }
    }

    public long preMax(int i) {
        long res = Long.MIN_VALUE;
        while (i > 0) {
            res = Math.max(res, tree[i]);
            i &= i - 1;
        }
        return res;
    }
}
```

```cpp [sol-C++]
// 树状数组模板（维护前缀最大值）
class BIT {
    vector<long long> tree;
public:
    BIT(int n) : tree(n, LLONG_MIN) {}

    void update(int i, long long val) {
        while (i < tree.size()) {
            tree[i] = max(tree[i], val);
            i += i & -i;
        }
    }

    long long pre_max(int i) {
        long long res = LLONG_MIN;
        while (i > 0) {
            res = max(res, tree[i]);
            i &= i - 1;
        }
        return res;
    }
};

class Solution {
public:
    long long maxBalancedSubsequenceSum(vector<int> &nums) {
        int n = nums.size();
        // 离散化 nums[i]-i
        auto b = nums;
        for (int i = 0; i < n; i++) {
            b[i] -= i;
        }
        sort(b.begin(), b.end());
        b.erase(unique(b.begin(), b.end()), b.end()); // 去重

        BIT t = BIT(b.size() + 1);
        for (int i = 0; i < n; i++) {
            // j 为 nums[i]-i 离散化后的值（从 1 开始）
            int j = lower_bound(b.begin(), b.end(), nums[i] - i) - b.begin() + 1;
            long long f = max(t.pre_max(j), 0LL) + nums[i];
            t.update(j, f);
        }
        return t.pre_max(b.size());
    }
};
```

```go [sol-Go]
// 树状数组模板（维护前缀最大值）
type fenwick []int

func (f fenwick) update(i, val int) {
	for ; i < len(f); i += i & -i {
		f[i] = max(f[i], val)
	}
}

func (f fenwick) preMax(i int) int {
	mx := math.MinInt
	for ; i > 0; i &= i - 1 {
		mx = max(mx, f[i])
	}
	return mx
}

func maxBalancedSubsequenceSum(nums []int) int64 {
	// 离散化 nums[i]-i
	b := slices.Clone(nums)
	for i := range b {
		b[i] -= i
	}
	slices.Sort(b)
	b = slices.Compact(b) // 去重

	// 初始化树状数组
	t := make(fenwick, len(b)+1)
	for i := range t {
		t[i] = math.MinInt
	}

	for i, x := range nums {
		j := sort.SearchInts(b, x-i) + 1 // nums[i]-i 离散化后的值（从 1 开始）
		f := max(t.preMax(j), 0) + x
		t.update(j, f)
	}
	return int64(t.preMax(len(b)))
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

#### 相似题目

- [2407. 最长递增子序列 II](https://leetcode.cn/problems/longest-increasing-subsequence-ii/)

## 本地原创解析

### 1. 题意重述

本题来自 `十一、优化 DP / §11.4 树状数组/线段树优化 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十一、优化 DP / §11.4 树状数组/线段树优化 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
