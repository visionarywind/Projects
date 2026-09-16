# 3806. 增加操作后最大按位与的结果

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-bitwise-and-after-increment-operations/
- 题目 slug：`maximum-bitwise-and-after-increment-operations`
- 来源专题：位运算
- 来源分类路径：五、试填法
- 难度分：2259
- 外部题解来源：https://leetcode.cn/problems/maximum-bitwise-and-after-increment-operations/solutions/3877680/shi-tian-fa-pai-xu-kuai-su-xuan-ze-pytho-3fyt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[试填法 + 排序/快速选择（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-bitwise-and-after-increment-operations/solutions/3877680/shi-tian-fa-pai-xu-kuai-su-xuan-ze-pytho-3fyt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-tian-fa-pai-xu-kuai-su-xuan-ze-pytho-3fyt`
- topic id：`3877680`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 核心思路

1. 从高到低构建答案的每一位是 $1$ 还是 $0$。
2. 遍历 $\textit{nums}$，计算把 $\textit{nums}[i]$ 的某些比特位都变成 $1$ 的最小操作次数。
3. 计算前 $m$ 小的操作次数之和 $s$。如果 $s\le k$，那么答案这一位是 $1$，否则是 $0$。

## 具体思路

从高到低构建答案的每一位是 $1$ 还是 $0$。

> 由于 AND 结果不超过 $\max(\textit{nums})+ \left\lfloor\dfrac{k}{m}\right\rfloor$，从 $\max(\textit{nums})+\left\lfloor\dfrac{k}{m}\right\rfloor$ 的二进制长度减一开始枚举。

设现在要判断 AND 结果是否包含 $\textit{target}$。如果 $\textit{target} = 0100$（二进制数），那么 $x=\textit{nums}[i]$ 的从低到高第三位必须是 $1$，才能保证 AND 结果从低到高第三位是 $1$。

举几个例子：

- $\textit{target} = 0100$，$x = 0001$，那么 $x$ 要增大到 $0100$。
- $\textit{target} = 0100$，$x = 1010$，那么 $x$ 要增大到 $1100$。
- $\textit{target} = 10110$，$x = 11010$，那么 $x$ 要增大到 $11110$。

一般地，找到 $x$ 的**最高缺失位**，即从高到低第一个 $\textit{target}$ 是 $1$ 且 $x$ 是 $0$ 的比特位 $j$。$x$ 二进制中的高于 $j$ 的位不变（已满足要求），低 $j$ 位增大到等于 $\textit{target}$ 的低 $j$ 位。

按照这个方法，计算每个 $\textit{nums}[i]$ 的操作次数，然后计算前 $m$ 小的操作次数之和 $s$。如果 $s\le k$，那么答案这一位填 $1$，否则填 $0$。

[本题视频讲解](https://www.bilibili.com/video/BV1tv6dBME7K/?t=22m3s)，欢迎点赞关注~

## 答疑

**问**：在处理答案的每一位时，我们所选择的 $m$ 个数，并没有考虑在上一轮循环中选择的 $m$ 个数，万一和上一轮循环选的 $m$ 个数不同呢？

**答**：这个算法每轮循环，判断的是「能否选出 $m$ 个满足要求的数」，只要能选出（$s\le k$），就说明答案这一位填 $1$（注意算法没有修改 $\textit{nums}$）。最后一次填 $1$ 时选的 $m$ 个数就是最终选的 $m$ 个数（记作序列 $A$）。序列 $A$ 中的元素不仅满足当前这一位可以填 $1$，$\textit{target}$ 更高位的 $1$ 也满足。可以把序列 $A$ 当作每轮循环填 $1$ 时选的 $m$ 个数。

```py [sol-Python3]
class Solution:
    def maximumAND(self, nums: List[int], k: int, m: int) -> int:
        ops = [0] * len(nums)  # 每个数的操作次数
        ans = 0
        max_width = (max(nums) + k // m).bit_length()
        for bit in range(max_width - 1, -1, -1):
            target = ans | (1 << bit)  # 注意 target 要带着 ans 已经填好的 1
            for i, x in enumerate(nums):
                j = (target & ~x).bit_length()
                # j-1 是从高到低第一个 target 是 1 且 x 是 0 的比特位
                # target = 10110
                #      x = 11010
                #            ^
                #           j-1
                # x 二进制中的高于 j-1 的位不变，其余位增大到和 target 一样
                # 上面的例子要把 010 变成 110
                mask = (1 << j) - 1
                ops[i] = (target & mask) - (x & mask)

            # 贪心，取前 m 小的操作次数
            ops.sort()
            if sum(ops[:m]) <= k:
                ans = target  # 答案的 bit 位可以填 1
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumAND(int[] nums, int k, int m) {
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int n = nums.length;
        int[] ops = new int[n]; // 每个数的操作次数
        int ans = 0;

        int maxWidth = 32 - Integer.numberOfLeadingZeros(mx + k / m);
        for (int bit = maxWidth - 1; bit >= 0; bit--) {
            int target = ans | (1 << bit); // 注意 target 要带着 ans 已经填好的 1
            for (int i = 0; i < n; i++) {
                int x = nums[i];
                int j = 32 - Integer.numberOfLeadingZeros(target & ~x);
                // j-1 是从高到低第一个 target 是 1 且 x 是 0 的比特位
                // target = 10110
                //      x = 11010
                //            ^
                //           j-1
                // x 二进制中的高于 j-1 的位不变，其余位增大到和 target 一样
                // 上面的例子要把 010 变成 110
                int mask = (1 << j) - 1;
                ops[i] = (target & mask) - (x & mask);
            }

            // 贪心，取前 m 小的操作次数
            Arrays.sort(ops);
            long sum = 0;
            for (int i = 0; i < m; i++) {
                sum += ops[i];
            }
            if (sum <= k) {
                ans = target; // 答案的 bit 位可以填 1
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumAND(vector<int>& nums, int k, int m) {
        int n = nums.size();
        vector<int> ops(n); // 每个数的操作次数
        int ans = 0;

        int max_width = bit_width((uint32_t) ranges::max(nums) + k / m);
        for (int bit = max_width - 1; bit >= 0; bit--) {
            int target = ans | (1 << bit); // 注意 target 要带着 ans 已经填好的 1
            for (int i = 0; i < n; i++) {
                int x = nums[i];
                int j = bit_width((uint32_t) target & ~x);
                // j-1 是从高到低第一个 target 是 1 且 x 是 0 的比特位
                // target = 10110
                //      x = 11010
                //            ^
                //           j-1
                // x 二进制中的高于 j-1 的位不变，其余位增大到和 target 一样
                // 上面的例子要把 010 变成 110
                int mask = (1u << j) - 1;
                ops[i] = (target & mask) - (x & mask);
            }

            // 贪心，取前 m 小的操作次数
            // ranges::sort(ops);
            ranges::nth_element(ops, ops.begin() + m);
            if (reduce(ops.begin(), ops.begin() + m, 0LL) <= k) {
                ans = target; // 答案的 bit 位可以填 1
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumAND(nums []int, k, m int) (ans int) {
	ops := make([]int, len(nums)) // 每个数的操作次数
	maxWidth := bits.Len(uint(slices.Max(nums) + k/m))
	for bit := maxWidth - 1; bit >= 0; bit-- {
		target := ans | 1<<bit // 注意 target 要带着 ans 已经填好的 1
		for i, x := range nums {
			j := bits.Len(uint(target &^ x))
			// j-1 是从高到低第一个 target 是 1 且 x 是 0 的比特位
			// target = 10110
			//      x = 11010
			//            ^
			//           j-1
			// x 二进制中的高于 j-1 的位不变，其余位增大到和 target 一样
			// 上面的例子要把 010 变成 110
			mask := 1<<j - 1
			ops[i] = target&mask - x&mask
		}

		// 贪心，取前 m 小的操作次数
		slices.Sort(ops)
		sum := 0
		for _, x := range ops[:m] {
			sum += x
		}
		if sum <= k {
			ans = target // 答案的 bit 位可以填 1
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n\log U)$ 或者 $\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})+k/m$。瓶颈在排序上，用快速选择算法可以做到 $\mathcal{O}(n\log U)$，见 C++ 代码。
- 空间复杂度：$\mathcal{O}(n)$。

## 优化

如果 $\textit{nums}$ 最大的 $m$ 个数都相等，那么把最大的 $m$ 个数都增加 $\left\lfloor\dfrac{k}{m}\right\rfloor$，便可让答案等于理论最大值 $\max(\textit{nums}) + \left\lfloor\dfrac{k}{m}\right\rfloor$。

```py [sol-Python3]
class Solution:
    def maximumAND(self, nums: List[int], k: int, m: int) -> int:
        nums.sort()
        max_ans = nums[-1] + k // m
        if nums[-m] == nums[-1]:  # 最大的 m 个数都相等
            return max_ans

        ops = [0] * len(nums)  # 每个数的操作次数
        ans = 0
        max_width = max_ans.bit_length()
        for bit in range(max_width - 1, -1, -1):
            target = ans | (1 << bit)  # 注意 target 要带着 ans 已经填好的 1
            for i, x in enumerate(nums):
                j = (target & ~x).bit_length()
                # j-1 是从高到低第一个 target 是 1 且 x 是 0 的比特位
                # target = 10110
                #      x = 11010
                #            ^
                #           j-1
                # x 二进制中的高于 j-1 的位不变，其余位增大到和 target 一样
                # 上面的例子要把 010 变成 110
                mask = (1 << j) - 1
                ops[i] = (target & mask) - (x & mask)

            # 贪心，取前 m 小的操作次数
            ops.sort()
            if sum(ops[:m]) <= k:
                ans = target  # 答案的 bit 位可以填 1
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumAND(int[] nums, int k, int m) {
        Arrays.sort(nums);
        int n = nums.length;
        int maxAns = nums[n - 1] + k / m;
        if (nums[n - m] == nums[n - 1]) { // 最大的 m 个数都相等
            return maxAns;
        }

        int[] ops = new int[n]; // 每个数的操作次数
        int ans = 0;

        int maxWidth = 32 - Integer.numberOfLeadingZeros(maxAns);
        for (int bit = maxWidth - 1; bit >= 0; bit--) {
            int target = ans | (1 << bit); // 注意 target 要带着 ans 已经填好的 1
            for (int i = 0; i < n; i++) {
                int x = nums[i];
                int j = 32 - Integer.numberOfLeadingZeros(target & ~x);
                // j-1 是从高到低第一个 target 是 1 且 x 是 0 的比特位
                // target = 10110
                //      x = 11010
                //            ^
                //           j-1
                // x 二进制中的高于 j-1 的位不变，其余位增大到和 target 一样
                // 上面的例子要把 010 变成 110
                int mask = (1 << j) - 1;
                ops[i] = (target & mask) - (x & mask);
            }

            // 贪心，取前 m 小的操作次数
            Arrays.sort(ops);
            long sum = 0;
            for (int i = 0; i < m; i++) {
                sum += ops[i];
            }
            if (sum <= k) {
                ans = target; // 答案的 bit 位可以填 1
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumAND(vector<int>& nums, int k, int m) {
        ranges::nth_element(nums, nums.end() - m);
        int n = nums.size();
        int base = nums.back();
        bool same = true;
        for (int i = n - m; i < n - 1; i++) {
            if (nums[i] != base) {
                same = false;
            }
        }
        if (same) { // 最大的 m 个数都相等
            return base + k / m;
        }

        vector<int> ops(n); // 每个数的操作次数
        int ans = 0;

        int max_width = bit_width((uint32_t) ranges::max(nums) + k / m);
        for (int bit = max_width - 1; bit >= 0; bit--) {
            int target = ans | (1 << bit); // 注意 target 要带着 ans 已经填好的 1
            for (int i = 0; i < n; i++) {
                int x = nums[i];
                int j = bit_width((uint32_t) target & ~x);
                // j-1 是从高到低第一个 target 是 1 且 x 是 0 的比特位
                // target = 10110
                //      x = 11010
                //            ^
                //           j-1
                // x 二进制中的高于 j-1 的位不变，其余位增大到和 target 一样
                // 上面的例子要把 010 变成 110
                int mask = (1u << j) - 1;
                ops[i] = (target & mask) - (x & mask);
            }

            // 贪心，取前 m 小的操作次数
            // ranges::sort(ops);
            ranges::nth_element(ops, ops.begin() + m);
            if (reduce(ops.begin(), ops.begin() + m, 0LL) <= k) {
                ans = target; // 答案的 bit 位可以填 1
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumAND(nums []int, k, m int) (ans int) {
	slices.Sort(nums)
	n := len(nums)
	maxAns := nums[n-1] + k/m
	if nums[n-m] == nums[n-1] { // 最大的 m 个数都相等
		return maxAns
	}

	ops := make([]int, n) // 每个数的操作次数
	maxWidth := bits.Len(uint(maxAns))
	for bit := maxWidth - 1; bit >= 0; bit-- {
		target := ans | 1<<bit // 注意 target 要带着 ans 已经填好的 1
		for i, x := range nums {
			j := bits.Len(uint(target &^ x))
			// j-1 是从高到低第一个 target 是 1 且 x 是 0 的比特位
			// target = 10110
			//      x = 11010
			//            ^
			//           j-1
			// x 二进制中的高于 j-1 的位不变，其余位增大到和 target 一样
			// 上面的例子要把 010 变成 110
			mask := 1<<j - 1
			ops[i] = target&mask - x&mask
		}

		// 贪心，取前 m 小的操作次数
		slices.Sort(ops)
		sum := 0
		for _, x := range ops[:m] {
			sum += x
		}
		if sum <= k {
			ans = target // 答案的 bit 位可以填 1
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n\log U)$ 或者 $\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})+k/m$。瓶颈在排序上，用快速选择算法可以做到 $\mathcal{O}(n\log U)$，见 C++ 代码。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

把 AND 改成 OR（按位或）怎么做？

欢迎在评论区分享你的思路/代码。

## 专题训练

见下面位运算题单的「**五、试填法**」。

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

本题来自 `五、试填法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、试填法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
