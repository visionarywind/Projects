# 3854. 使数组奇偶交替的最少操作

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-make-array-parity-alternating/
- 题目 slug：`minimum-operations-to-make-array-parity-alternating`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：2095
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-make-array-parity-alternating/solutions/3910700/tan-xin-pythonjavacgo-by-endlesscheng-8ic0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-operations-to-make-array-parity-alternating/solutions/3910700/tan-xin-pythonjavacgo-by-endlesscheng-8ic0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-pythonjavacgo-by-endlesscheng-8ic0`
- topic id：`3910700`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

把 $\textit{nums}$ 变成奇偶交替，只有两种情况：

- 偶奇偶奇偶奇……
- 奇偶奇偶奇偶……

枚举这两种情况。

遍历 $\textit{nums}$，如果 $\textit{nums}[i]$ 的奇偶性不等于目标奇偶性，那么操作一次后，$\textit{nums}[i]$ 的奇偶性一定会变，一定等于目标奇偶性。所以**每个元素至多操作一次**。

这引出了如下**通用做法**：

- 对于 $x = \textit{nums}[i]$，如果不操作 $x$，视作列表 $[x]$，否则视作列表 $[x-1,x+1]$，于是我们得到了 $n$ 个列表。问题相当于找到一个最短的**值域范围** $[a,b]$，使得每个列表都至少有一个数在 $[a,b]$ 中。这题是 [632. 最小区间](https://leetcode.cn/problems/smallest-range-covering-elements-from-k-lists/)，[我的题解](https://leetcode.cn/problems/smallest-range-covering-elements-from-k-lists/solutions/2982588/liang-chong-fang-fa-dui-pai-xu-hua-dong-luih5/)。

下面介绍针对本题的**特殊做法**。

设全局最小值 $\textit{gMin} = \min(\textit{nums})$，全局最大值 $\textit{gMax} = \max(\textit{nums})$。分类讨论：

- 如果 $n=1$，无需修改，返回 $[0,0]$ 即可。下面讨论 $n\ge 2$ 的情况，对于奇偶交替数组，由于相邻元素一定不同，所以极差至少是 $1$。
- 如果 $\textit{gMin} = \textit{gMax}$，规定对于要修改的数，统一加一。在修改了元素的情况下，最终极差是 $1$。如果不这样做，某些数加一，另一些数减一，那么极差会是 $2$。
- 如果 $\textit{gMin} + 1 = \textit{gMax}$，对于要修改的数，如果其等于 $\textit{gMin}$，那么加一；否则其等于 $\textit{gMax}$，那么减一。这样最终极差是 $1$。
- 如果 $\textit{gMin} + 1 < \textit{gMax}$，对于要修改的数，如果其等于 $\textit{gMin}$，那么加一；如果其等于 $\textit{gMax}$，那么减一；其余数呢？由于修改 $\textit{gMin}$ 和 $\textit{gMax}$ 后极差仍然 $\ge 1$（注意 $n\ge 2$ 的奇偶交替数组的极差至少是 $1$），所以原来在 $[\textit{gMin} + 1, \textit{gMax}-1]$ 中的数，总是可以改成在新的最小值和最大值之间的数，从而**不影响极差**。比如新的最小值是 $\textit{gMin} + 1$，那么新的最大值至少是 $\textit{gMin} + 2$，原来等于 $\textit{gMin} + 1$ 的数可以加一，不会影响极差。

综上所述，对于要修改的数，如果其等于 $\textit{gMin}$，那么加一；否则如果其等于 $\textit{gMax}$，那么减一；其他情况不修改。

[本题视频讲解](https://www.bilibili.com/video/BV1VvABz9EGz/?t=12m13s)，欢迎点赞关注~

如何构造一个具体的操作方案？见视频讲解。

```py [sol-Python3]
class Solution:
    def makeParityAlternating(self, nums: List[int]) -> List[int]:
        if len(nums) == 1:
            return [0, 0]

        g_min = min(nums)
        g_max = max(nums)

        def calc(target: int) -> List[int]:
            op = 0
            mn, mx = inf, -inf
            for i, x in enumerate(nums):
                # 如果 target = 0，那么操作后，nums 每个数的奇偶性必须分别等于 0,1,0,1,... 即 target ^ (i%2)
                if (x - i) & 1 != target:  # 等价于 x&1 != target ^ (i%2)
                    op += 1
                    if x == g_min:
                        x += 1
                    elif x == g_max:
                        x -= 1
                mn = min(mn, x)
                mx = max(mx, x)
            return [op, max(mx - mn, 1)]  # 在 n >= 2 的情况下，极差至少是 1

        return min(calc(0), calc(1))
```

```java [sol-Java]
class Solution {
    public int[] makeParityAlternating(int[] nums) {
        if (nums.length == 1) {
            return new int[]{0, 0};
        }

        int gMin = Integer.MAX_VALUE;
        int gMax = Integer.MIN_VALUE;
        for (int x : nums) {
            gMin = Math.min(gMin, x);
            gMax = Math.max(gMax, x);
        }

        int[] res1 = calc(0, nums, gMin, gMax);
        int[] res2 = calc(1, nums, gMin, gMax);

        int op1 = res1[0], minD1 = res1[1];
        int op2 = res2[0], minD2 = res2[1];
        if (op1 < op2 || op1 == op2 && minD1 < minD2) {
            return new int[]{op1, minD1};
        }
        return new int[]{op2, minD2};
    }

    private int[] calc(int target, int[] nums, int gMin, int gMax) {
        int op = 0;
        int mn = Integer.MAX_VALUE;
        int mx = Integer.MIN_VALUE;
        for (int i = 0; i < nums.length; i++) {
            int x = nums[i];
            // 如果 target = 0，那么操作后，nums 每个数的奇偶性必须分别等于 0,1,0,1,... 即 target ^ (i%2)
            if (((x - i) & 1) != target) { // 等价于 (x&1) != (target ^ (i%2))
                op++;
                if (x == gMin) {
                    x++;
                } else if (x == gMax) {
                    x--;
                }
            }
            mn = Math.min(mn, x);
            mx = Math.max(mx, x);
        }
        return new int[]{op, Math.max(mx - mn, 1)}; // 在 n >= 2 的情况下，极差至少是 1
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> makeParityAlternating(vector<int>& nums) {
        if (nums.size() == 1) {
            return {0, 0};
        }

        int g_min = ranges::min(nums);
        int g_max = ranges::max(nums);

        auto calc = [&](int target) -> vector<int> {
            int op = 0;
            int mn = INT_MAX, mx = INT_MIN;
            for (int i = 0; i < nums.size(); i++) {
                int x = nums[i];
                // 如果 target = 0，那么操作后，nums 每个数的奇偶性必须分别等于 0,1,0,1,... 即 target ^ (i%2)
                if (((x - i) & 1) != target) { // 等价于 (x&1) != (target ^ (i%2))
                    op++;
                    if (x == g_min) {
                        x++;
                    } else if (x == g_max) {
                        x--;
                    }
                }
                mn = min(mn, x);
                mx = max(mx, x);
            }
            return {op, max(mx - mn, 1)}; // 在 n >= 2 的情况下，极差至少是 1
        };

        return min(calc(0), calc(1));
    }
};
```

```go [sol-Go]
func makeParityAlternating(nums []int) []int {
	if len(nums) == 1 {
		return []int{0, 0}
	}

	gMin := slices.Min(nums)
	gMax := slices.Max(nums)

	calc := func(target int) (int, int) {
		op, mn, mx := 0, math.MaxInt, math.MinInt
		for i, x := range nums {
			// 如果 target = 0，那么操作后，nums 每个数的奇偶性必须分别等于 0,1,0,1,... 即 target ^ i%2
			if (x-i)&1 != target { // 等价于 x&1 != target ^ i%2
				op++
				if x == gMin {
					x++
				} else if x == gMax {
					x--
				}
			}
			mn = min(mn, x)
			mx = max(mx, x)
		}
		return op, max(mx-mn, 1) // 在 n >= 2 的情况下，极差至少是 1
	}

	op1, minD1 := calc(0)
	op2, minD2 := calc(1)

	if op1 < op2 || op1 == op2 && minD1 < minD2 {
		return []int{op1, minD1}
	}
	return []int{op2, minD2}
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.8 分类讨论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.8 分类讨论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
