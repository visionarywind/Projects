# 3824. 减小数组使其满足条件的最小 K 值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-k-to-reduce-array-within-limit/
- 题目 slug：`minimum-k-to-reduce-array-within-limit`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.1 求最小 / 答疑
- 难度分：1531
- 外部题解来源：https://leetcode.cn/problems/minimum-k-to-reduce-array-within-limit/solutions/3893557/er-fen-da-an-pythonjavacgo-by-endlessche-qt0r/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分答案 + 上下界优化（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-k-to-reduce-array-within-limit/solutions/3893557/er-fen-da-an-pythonjavacgo-by-endlessche-qt0r/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-pythonjavacgo-by-endlessche-qt0r`
- topic id：`3893557`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 转化

对于不等式

$$
\text{nonPositive}(\textit{nums}, k) \le k^2
$$

当 $k$ 逐渐增大时，操作次数会变小（或者不变），所以 $\text{nonPositive}(\textit{nums}, k)$ 会变小（或者不变）；另一方面，$k^2$ 会随着 $k$ 的增大而增大。

所以当 $k$ 较小时，不等式不成立；当 $k$ 较大时，不等式成立。

据此，可以**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

现在问题转化成一个判定性问题：

- 给定 $k$，计算每个数的操作次数，判断不等式是否成立。

如果成立，说明答案 $\le k$，否则答案 $> k$。

## 思路

对于 $x = \textit{nums}[i]$，设需要操作 $t$ 次，那么有

$$
k\cdot t \ge x
$$

解得

$$
t\ge \dfrac{x}{k}
$$

由于 $t$ 是整数，所以最小操作次数为

$$
\left\lceil\dfrac{x}{k}\right\rceil
$$

所以有

$$
\text{nonPositive}(\textit{nums}, k) = \sum_{i=0}^{n-1}\left\lceil\dfrac{\textit{nums}[i]}{k}\right\rceil
$$

## 细节

### 1)

下面代码采用开区间二分。使用闭区间或者半闭半开区间也是可以的，喜欢哪种写法就用哪种。

- 开区间左端点初始值：$0$。无法满足题目要求。
- 开区间左端点初始值（优化）：$\left\lceil\sqrt n\right\rceil - 1$。由于 $\textit{nums}$ 中的元素都是正数，**每个数都至少要操作一次**，所以 $\text{nonPositive}(\textit{nums}, k) \ge n$，所以 $k$ 必须满足 $k^2\ge n$，即 $k\ge \left\lceil\sqrt n\right\rceil$。减一后，一定无法满足题目要求。
- 开区间右端点初始值：$M$，其中 $M = \max(\textit{nums})$。此时 $\text{nonPositive}(\textit{nums}, M)=n$。如果 $n \le M^2$，那么满足题目要求。这引出了一个**特殊情况**：如果 $M\le \left\lceil\sqrt n\right\rceil$，那么答案就是理论最小值 $\left\lceil\sqrt n\right\rceil$，此时 $\text{nonPositive}(\textit{nums}, k) \le k^2$ 为 $n\le \left\lceil\sqrt n\right\rceil^2$，一定成立，可以提前返回 $\left\lceil\sqrt n\right\rceil$，无需二分。

### 2)

关于上取整的计算，当 $a$ 为整数，$b$ 为正整数时，有恒等式

$$
\left\lceil\dfrac{a}{b}\right\rceil = \left\lfloor\dfrac{a+b-1}{b}\right\rfloor = \left\lfloor\dfrac{a-1}{b}\right\rfloor + 1
$$

见 [上取整下取整转换公式的证明](https://zhuanlan.zhihu.com/p/1890356682149838951)。

所以

$$
\text{nonPositive}(\textit{nums}, k) = n + \sum_{i=0}^{n-1}\left\lfloor\dfrac{\textit{nums}[i]-1}{k}\right\rfloor
$$

这样做可以避免浮点运算，避免浮点数的舍入误差导致计算错误。

## 优化前

```py [sol-Python3]
class Solution:
    def minimumK(self, nums: List[int]) -> int:
        n = len(nums)

        def check(k: int) -> bool:
            return n + sum((x - 1) // k for x in nums) <= k * k

        left = ceil(sqrt(n))  # 答案的下界
        right = max(nums)
        return bisect_left(range(right), True, left, key=check)
```

```py [sol-Python3 手写二分]
class Solution:
    def minimumK(self, nums: List[int]) -> int:
        n = len(nums)

        def check(k: int) -> bool:
            return n + sum((x - 1) // k for x in nums) <= k * k

        left = ceil(sqrt(n)) - 1
        right = max(nums)
        while left + 1 < right:
            mid = (left + right) // 2
            if check(mid):
                right = mid
            else:
                left = mid
        return left + 1
```

```java [sol-Java]
class Solution {
    public int minimumK(int[] nums) {
        int n = nums.length;
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int left = (int) Math.ceil(Math.sqrt(n)) - 1;
        int right = mx;
        while (left + 1 < right) {
            int mid = (left + right) / 2;
            if (check(mid, nums)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return left + 1;
    }

    private boolean check(int k, int[] nums) {
        long sum = nums.length;
        for (int x : nums) {
            sum += (x - 1) / k;
        }
        return sum <= (long) k * k;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumK(vector<int>& nums) {
        int n = nums.size();

        auto check = [&](int k) -> bool {
            long long sum = n;
            for (int x : nums) {
                sum += (x - 1) / k;
            }
            return sum <= 1LL * k * k;
        };

        int left = ceil(sqrt(n)) - 1;
        int right = ranges::max(nums);
        while (left + 1 < right) {
            int mid = (left + right) / 2;
            (check(mid) ? right : left) = mid;
        }
        return left + 1;
    }
};
```

```go [sol-Go]
func minimumK(nums []int) int {
	n := len(nums)
	left := int(math.Ceil(math.Sqrt(float64(n)))) // 答案的下界
	right := slices.Max(nums)
	ans := left + sort.Search(right-left, func(k int) bool {
		k += left
		sum := n
		for _, x := range nums {
			sum += (x - 1) / k
		}
		return sum <= k*k
	})
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log (U - \sqrt n))$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(1)$。

## 优化

由于

$$
\sum_{i=0}^{n-1}\dfrac{\textit{nums}[i]}{k} \le \sum_{i=0}^{n-1}\left\lceil\dfrac{\textit{nums}[i]}{k}\right\rceil \le k^2
$$

解得

$$
k\ge \sqrt[3] S
$$

其中 $S = \sum\limits_{i=0}^{n-1}\textit{nums}[i]$。

所以答案的下界，可以改进为

$$
\textit{low} = \max\left(\left\lceil \sqrt n \right\rceil, \left\lceil \sqrt[3] S \right\rceil\right)
$$

设

$$
\textit{high} = \left\lceil \sqrt {\text{nonPositive}(\textit{nums}, \textit{low})} \right\rceil
$$

如果 $\textit{high} \ge \textit{low}$，根据 $\text{nonPositive}$ 的单调性，我们有

$$
\text{nonPositive}(\textit{nums}, \textit{high}) \le \text{nonPositive}(\textit{nums}, \textit{low}) \le \textit{high}^2
$$

所以 $\textit{high}$ 是答案的上界。

> 如果 $\textit{high} < \textit{low}$ 呢？此时 $\sqrt {\text{nonPositive}(\textit{nums}, \textit{low})} \le  \left\lceil \sqrt {\text{nonPositive}(\textit{nums}, \textit{low})} \right\rceil < \textit{low}$，得 $\text{nonPositive}(\textit{nums}, \textit{low}) < \textit{low}^2$，所以答案就是 $\textit{low}$。

```py [sol-Python3]
class Solution:
    def minimumK(self, nums: List[int]) -> int:
        n = len(nums)

        def non_positive(k: int) -> int:
            return n + sum((x - 1) // k for x in nums)

        left = max(ceil(sqrt(n)), ceil(cbrt(sum(nums))))  # 答案的下界
        right = ceil(sqrt(non_positive(left)))  # 答案的上界
        return bisect_left(range(right), True, left, key=lambda k: non_positive(k) <= k * k)
```

```java [sol-Java]
class Solution {
    public int minimumK(int[] nums) {
        int n = nums.length;
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        long total = 0;
        for (int x : nums) {
            total += x;
        }

        int left = Math.max((int) Math.ceil(Math.sqrt(n)), (int) Math.ceil(Math.cbrt(total))) - 1;
        int right = (int) Math.ceil(Math.sqrt(nonPositive(nums, left + 1)));
        while (left + 1 < right) {
            int k = (left + right) / 2;
            if (nonPositive(nums, k) <= (long) k * k) {
                right = k;
            } else {
                left = k;
            }
        }
        return left + 1;
    }

    private long nonPositive(int[] nums, int k) {
        long sum = nums.length;
        for (int x : nums) {
            sum += (x - 1) / k;
        }
        return sum;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumK(vector<int>& nums) {
        int n = nums.size();
        int mx = ranges::max(nums);
        long long total = reduce(nums.begin(), nums.end(), 0LL);

        auto non_positive = [&](int k) -> long long {
            long long sum = n;
            for (int x : nums) {
                sum += (x - 1) / k;
            }
            return sum;
        };

        int left = max(ceil(sqrt(n)), ceil(cbrt(total))) - 1;
        int right = ceil(sqrt(non_positive(left + 1)));
        while (left + 1 < right) {
            int k = (left + right) / 2;
            (non_positive(k) <= 1LL * k * k ? right : left) = k;
        }
        return left + 1;
    }
};
```

```go [sol-Go]
func minimumK(nums []int) int {
	n := len(nums)
	nonPositive := func(k int) int {
		sum := n
		for _, x := range nums {
			sum += (x - 1) / k
		}
		return sum
	}

	sum := 0
	for _, x := range nums {
		sum += x
	}

	left := max(int(math.Ceil(math.Sqrt(float64(n)))), int(math.Ceil(math.Cbrt(float64(sum))))) // 答案的下界
	right := int(math.Ceil(math.Sqrt(float64(nonPositive(left)))))                              // 答案的上界
	ans := left + sort.Search(right-left, func(k int) bool {
		k += left
		return nonPositive(k) <= k*k
	})
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}\left(n\log \dfrac{n}{\sqrt[3] S}\right)$，其中 $n$ 是 $\textit{nums}$ 的长度，$S$ 是 $\textit{nums}$ 的元素和。由于 $\sum\limits_{i=0}^{n-1}\left\lceil\dfrac{\textit{nums}[i]}{k}\right\rceil < \sum\limits_{i=0}^{n-1}\left(\dfrac{\textit{nums}[i]}{k}+1\right) = \dfrac{S}{k} + n$，所以当 $\textit{low} = \sqrt[3] S$ 时，$\textit{high} = \sqrt{\dfrac{S}{\sqrt[3] S} + n} = \sqrt{S^{2/3} + n}$。根据泰勒展开 $\sqrt{x^2+y} - x\approx \dfrac{y}{2x}$，得到 $\textit{high} - \textit{low} \approx \dfrac{n}{2\sqrt[3] S}$。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面二分题单的「**§2.1 求最小**」。

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

本题来自 `二、二分答案 / §2.1 求最小 / 答疑`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.1 求最小 / 答疑`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
