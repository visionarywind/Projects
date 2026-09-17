# 2967. 使数组成为等数数组的最小代价

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-to-make-array-equalindromic/
- 题目 slug：`minimum-cost-to-make-array-equalindromic`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.1 回文数
- 难度分：2116
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-to-make-array-equalindromic/solutions/2569308/yu-chu-li-hui-wen-shu-zhong-wei-shu-tan-7j0zy/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[预处理回文数+中位数贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-cost-to-make-array-equalindromic/solutions/2569308/yu-chu-li-hui-wen-shu-zhong-wei-shu-tan-7j0zy/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yu-chu-li-hui-wen-shu-zhong-wei-shu-tan-7j0zy`
- topic id：`2569308`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

[本题视频讲解](https://www.bilibili.com/video/BV1994y1A7oo/)

## 前置知识：中位数贪心

为方便描述，将 $\textit{nums}$ 简记为 $a$。

**定理**：将 $a$ 的所有元素变为 $a$ 的**中位数**是最优的。

**证明**：设 $a$ 的长度为 $n$，设要将所有 $a[i]$ 变为 $x$。假设 $a$ 已经从小到大排序。首先，如果 $x$ 取在区间 $[a[0],a[n-1]]$ 之外，那么 $x$ 向区间方向移动可以使距离和变小；同时，如果 $x$ 取在区间 $[a[0],a[n-1]]$ 之内，无论如何移动 $x$，它到 $a[0]$ 和 $a[n-1]$ 的距离和都是一个定值 $a[n-1]-a[0]$，那么去掉 $a[0]$ 和 $a[n-1]$ 这两个最左最右的数，问题规模缩小。不断缩小问题规模，如果最后剩下 $1$ 个数，那么 $x$ 就取它；如果最后剩下 $2$ 个数，那么 $x$ 取这两个数之间的任意值都可以（包括这两个数）。因此 $x$ 可以取 $a[n/2]$。

本题回文数可能取不到中位数，我们可以找离中位数最近的数。

## 前置知识：二分查找

请看[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)

## 思路

首先预处理出 $10^9$ 内的回文数，这可以通过枚举回文数的左半部分得到。

> 注：也可以多预处理一个 $10^9+1$，不过考虑到 $10^9-1$ 也是回文数，这一步可以省略。

然后二分找离 $\textit{nums}$ 的中位数最近的回文数（中位数左右两侧都要找），作为要变成的数字。具体见代码。

```py [sol-Python3]
# 严格按顺序从小到大生成所有回文数（不用字符串转换）
pal = []
base = 1
while base <= 10000:
    # 生成奇数长度回文数
    for i in range(base, base * 10):
        x = i
        t = i // 10
        while t:
            x = x * 10 + t % 10
            t //= 10
        pal.append(x)
    # 生成偶数长度回文数
    if base <= 1000:
        for i in range(base, base * 10):
            x = t = i
            while t:
                x = x * 10 + t % 10
                t //= 10
            pal.append(x)
    base *= 10
pal.append(1_000_000_001)  # 哨兵，防止下面代码中的 i 下标越界

class Solution:
    def minimumCost(self, nums: List[int]) -> int:
        # 注：排序只是为了找中位数，如果用快速选择算法，可以做到 O(n)
        nums.sort()

        # 返回 nums 中的所有数变成 pal[i] 的总代价
        def cost(i: int) -> int:
            target = pal[i]
            return sum(abs(x - target) for x in nums)

        n = len(nums)
        i = bisect_left(pal, nums[(n - 1) // 2])  # 二分找中位数右侧最近的回文数
        if pal[i] <= nums[n // 2]:  # 回文数在中位数范围内
            return cost(i)  # 直接变成 pal[i]
        return min(cost(i - 1), cost(i))  # 枚举离中位数最近的两个回文数 pal[i-1] 和 pal[i]
```

```java [sol-Java]
class Solution {
    private static final int[] pal = new int[109999];

    static {
        // 严格按顺序从小到大生成所有回文数（不用字符串转换）
        int palIdx = 0;
        for (int base = 1; base <= 10000; base *= 10) {
            // 生成奇数长度回文数
            for (int i = base; i < base * 10; i++) {
                int x = i;
                for (int t = i / 10; t > 0; t /= 10) {
                    x = x * 10 + t % 10;
                }
                pal[palIdx++] = x;
            }
            // 生成偶数长度回文数
            if (base <= 1000) {
                for (int i = base; i < base * 10; i++) {
                    int x = i;
                    for (int t = i; t > 0; t /= 10) {
                        x = x * 10 + t % 10;
                    }
                    pal[palIdx++] = x;
                }
            }
        }
        pal[palIdx++] = 1_000_000_001; // 哨兵，防止下面代码中的 i 下标越界
    }

    public long minimumCost(int[] nums) {
        // 注：排序只是为了找中位数，如果用快速选择算法，可以做到 O(n)
        Arrays.sort(nums);
        int n = nums.length;

        // 二分找中位数右侧最近的回文数
        int i = lowerBound(nums[(n - 1) / 2]);

        // 回文数在中位数范围内
        if (pal[i] <= nums[n / 2]) {
            return cost(nums, i); // 直接变成 pal[i]
        }

        // 枚举离中位数最近的两个回文数 pal[i-1] 和 pal[i]
        return Math.min(cost(nums, i - 1), cost(nums, i));
    }

    // 返回 nums 中的所有数变成 pal[i] 的总代价
    private long cost(int[] nums, int i) {
        int target = pal[i];
        long sum = 0;
        for (int x : nums) {
            sum += Math.abs(x - target);
        }
        return sum;
    }

    // 开区间写法
    private int lowerBound(int target) {
        int left = -1, right = pal.length; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // pal[left] < target
            // pal[right] >= target
            int mid = left + (right - left) / 2;
            if (pal[mid] < target)
                left = mid; // 范围缩小到 (mid, right)
            else
                right = mid; // 范围缩小到 (left, mid)
        }
        return right; // 或者 left+1
    }
}
```

```cpp [sol-C++]
vector<int> pal;

auto init = [] {
    // 严格按顺序从小到大生成所有回文数（不用字符串转换）
    for (int base = 1; base <= 10000; base *= 10) {
        // 生成奇数长度回文数
        for (int i = base; i < base * 10; i++) {
            int x = i;
            for (int t = i / 10; t; t /= 10) {
                x = x * 10 + t % 10;
            }
            pal.push_back(x);
        }
        // 生成偶数长度回文数
        if (base <= 1000) {
            for (int i = base; i < base * 10; i++) {
                int x = i;
                for (int t = i; t; t /= 10) {
                    x = x * 10 + t % 10;
                }
                pal.push_back(x);
            }
        }
    }
    pal.push_back(1'000'000'001); // 哨兵，防止下面代码中的 i 下标越界
    return 0;
}();

class Solution {
public:
    long long minimumCost(vector<int> &nums) {
        // 注：排序只是为了找中位数，如果用快速选择算法，可以做到 O(n)
        sort(nums.begin(), nums.end());

        // 返回 nums 中的所有数变成 pal[i] 的总代价
        auto cost = [&](int i) -> long long {
            int target = pal[i];
            long long sum = 0;
            for (int x: nums) {
                sum += abs(x - target);
            }
            return sum;
        };

        int n = nums.size();
        // 二分找中位数右侧最近的回文数
        int i = lower_bound(pal.begin(), pal.end(), nums[(n - 1) / 2]) - pal.begin();
        if (pal[i] <= nums[n / 2]) { // 回文数在中位数范围内
            return cost(i); // 直接变成 pal[i]
        }
        return min(cost(i - 1), cost(i)); // 枚举离中位数最近的两个回文数 pal[i-1] 和 pal[i]
    }
};
```

```cpp [sol-C++ 快速选择]
vector<int> pal;

auto init = [] {
    pal.push_back(0); // 哨兵，防止下面代码中的 i 下标越界
    // 严格按顺序从小到大生成所有回文数（不用字符串转换）
    for (int base = 1; base <= 10000; base *= 10) {
        // 生成奇数长度回文数
        for (int i = base; i < base * 10; i++) {
            int x = i;
            for (int t = i / 10; t; t /= 10) {
                x = x * 10 + t % 10;
            }
            pal.push_back(x);
        }
        // 生成偶数长度回文数
        if (base <= 1000) {
            for (int i = base; i < base * 10; i++) {
                int x = i;
                for (int t = i; t; t /= 10) {
                    x = x * 10 + t % 10;
                }
                pal.push_back(x);
            }
        }
    }
    pal.push_back(1'000'000'001); // 哨兵，防止下面代码中的 i 下标越界
    return 0;
}();

class Solution {
public:
    long long minimumCost(vector<int> &nums) {
        int m = (nums.size() - 1) / 2;
        nth_element(nums.begin(), nums.begin() + m, nums.end());
        int mid = nums[m]; // 中位数

        // 返回 nums 中的所有数变成 pal[i] 的总代价
        auto cost = [&](int i) -> long long {
            int target = pal[i];
            long long sum = 0;
            for (int x: nums) {
                sum += abs(x - target);
            }
            return sum;
        };

        // 二分找中位数右侧最近的回文数
        int i = lower_bound(pal.begin(), pal.end(), mid) - pal.begin();

        // 枚举离中位数最近的两个回文数 pal[i-1] 和 pal[i]
        return min(cost(i - 1), cost(i));
    }
};
```

```go [sol-Go]
var pal = make([]int, 0, 109999)

func init() {
	// 按顺序从小到大生成所有回文数
	for base := 1; base <= 10000; base *= 10 {
		for i := base; i < base*10; i++ {
			x := i
			for t := i / 10; t > 0; t /= 10 {
				x = x*10 + t%10
			}
			pal = append(pal, x)
		}
		if base <= 1000 {
			for i := base; i < base*10; i++ {
				x := i
				for t := i; t > 0; t /= 10 {
					x = x*10 + t%10
				}
				pal = append(pal, x)
			}
		}
	}
	pal = append(pal, 1_000_000_001) // 哨兵，防止下标越界
}

func minimumCost(nums []int) int64 {
	// 注：排序只是为了找中位数，如果用快速选择算法，可以做到 O(n)
	slices.Sort(nums)

	// 返回所有 nums[i] 变成 pal[i] 的总代价
	cost := func(i int) (res int64) {
		target := pal[i]
		for _, x := range nums {
			res += int64(abs(x - target))
		}
		return
	}

	n := len(nums)
	i := sort.SearchInts(pal, nums[(n-1)/2]) // 二分找中位数右侧最近的回文数
	if pal[i] <= nums[n/2] { // 回文数在中位数范围内
		return cost(i) // 直接变成 pal[i]
	}
	return min(cost(i-1), cost(i)) // 枚举离中位数最近的两个回文数 pal[i-1] 和 pal[i]
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + \log U)$ 或 $\mathcal{O}(n + \log U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U$ 为 $10^9+1$ 内的回文数个数。忽略预处理的时间。
- 空间复杂度：$\mathcal{O}(1)$。忽略预处理的空间。

## 思考题

能否直接找离某个数最近的回文数？

这题是 [564. 寻找最近的回文数](https://leetcode.cn/problems/find-the-closest-palindrome/)，注意数据范围。

## 中位数贪心题单（右边数字为难度分）

- [462. 最小操作次数使数组元素相等 II](https://leetcode.cn/problems/minimum-moves-to-equal-array-elements-ii/)
- [2033. 获取单值网格的最小操作数](https://leetcode.cn/problems/minimum-operations-to-make-a-uni-value-grid/) 1672
- [2448. 使数组相等的最小开销](https://leetcode.cn/problems/minimum-cost-to-make-array-equal/) 2005
- [2607. 使子数组元素和相等](https://leetcode.cn/problems/make-k-subarray-sums-equal/) 2071
- [1703. 得到连续 K 个 1 的最少相邻交换次数](https://leetcode.cn/problems/minimum-adjacent-swaps-for-k-consecutive-ones/) 2467

## 本地原创解析

### 1. 题意重述

本题来自 `七、杂项 / §7.1 回文数`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、杂项 / §7.1 回文数`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
