# 2439. 最小化数组中的最大值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimize-maximum-of-array/
- 题目 slug：`minimize-maximum-of-array`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.4 最小化最大值
- 难度分：1965
- 外部题解来源：https://leetcode.cn/problems/minimize-maximum-of-array/solutions/1895244/liang-chong-zuo-fa-er-fen-da-an-fen-lei-qhee6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种做法：二分答案 / 分类讨论（Python/Java/C++/Go）](https://leetcode.cn/problems/minimize-maximum-of-array/solutions/1895244/liang-chong-zuo-fa-er-fen-da-an-fen-lei-qhee6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-zuo-fa-er-fen-da-an-fen-lei-qhee6`
- topic id：`1895244`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：二分答案

「最小化最大值」就是二分答案的代名词。我们猜测一个上界 $\textit{limit}$，即要求操作后所有元素均不超过 $\textit{limit}$。由于 $\textit{limit}$ 越大越能够满足，越小越无法满足，可以**二分猜答案**。

把 $\textit{nums}[i]$ 想象成一个大小为 $\textit{nums}[i]$ 的积木堆，我们可以把一个积木移动到左边相邻的积木堆中。进一步地，多出的积木并不是只能移动一次，而是可以多次向左移动，从 $i$ 移动到 $i-1$，再移动到 $i-2$，依此类推。

我们可以从右往左模拟：如果 $\textit{nums}[i]>\textit{limit}$，那么应当去掉多余的 $\textit{extra}=\textit{nums}[i]-\textit{limit}$ 加到 $\textit{nums}[i-1]$ 上，最后如果 $\textit{nums}[0]\le\textit{limit}$，则二分判定成功。

代码实现时可以不用修改 $\textit{nums}$，而是维护 $\textit{extra}$ 变量。

### 细节

开区间二分下界：$\textit{nums}[0]-1$，无法操作。注意 $\textit{nums}[0]$ 无法减少。

开区间二分上界：$\max(\textit{nums})$，一定可以操作。

[视频讲解](https://www.bilibili.com/video/BV1cV4y157BY) 第三题。

```py [sol-Python3]
class Solution:
    def minimizeArrayValue(self, nums: List[int]) -> int:
        def check(limit: int) -> bool:
            extra = 0
            for i in range(len(nums) - 1, 0, -1):
                new_num = nums[i] + extra  # 把多出的积木堆到 nums[i] 上
                extra = max(new_num - limit, 0)  # 如果 new_num - limit > 0，那么多出的积木继续丢给左边
            return nums[0] + extra <= limit

        return bisect_left(range(max(nums)), True, lo=nums[0], key=check)
```

```java [sol-Java]
class Solution {
    public int minimizeArrayValue(int[] nums) {
        int left = nums[0] - 1;
        int right = 0;
        for (int x : nums) {
            right = Math.max(right, x);
        }

        // 开区间二分，原理见 https://www.bilibili.com/video/BV1AP41137w7/
        while (left + 1 < right) {
            int mid = (left + right) / 2;
            if (check(nums, mid)) {
                right = mid;
            } else {
                left = mid;
            }
        }

        return right;
    }

    private boolean check(int[] nums, int limit) {
        long extra = 0;
        for (int i = nums.length - 1; i > 0; i--) {
            long newNum = nums[i] + extra; // 把多出的积木堆到 nums[i] 上
            extra = Math.max(newNum - limit, 0); // 如果 newNum - limit > 0，那么多出的积木继续丢给左边
        }
        return nums[0] + extra <= limit;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimizeArrayValue(vector<int>& nums) {
        auto check = [&](int limit) -> bool {
            long long extra = 0;
            for (int i = nums.size() - 1; i > 0; i--) {
                long long new_num = nums[i] + extra; // 把多出的积木堆到 nums[i] 上
                extra = max(new_num - limit, 0LL); // 如果 new_num - limit > 0，那么多出的积木继续丢给左边
            }
            return nums[0] + extra <= limit;
        };

        // 开区间二分，原理见 https://www.bilibili.com/video/BV1AP41137w7/
        int left = nums[0] - 1, right = ranges::max(nums);
        while (left + 1 < right) {
            int mid = (left + right) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right;
    }
};
```

```go [sol-Go]
func minimizeArrayValue(nums []int) int {
	// 库函数是左闭右开区间
	left, right := nums[0], slices.Max(nums)
	return left + sort.Search(right-left, func(limit int) bool {
		limit += left
		extra := 0
		for i := len(nums) - 1; i > 0; i-- {
			newNum := nums[i] + extra    // 把多出的积木堆到 nums[i] 上
			extra = max(newNum-limit, 0) // 如果 newNum-limit > 0，那么多出的积木继续丢给左边
		}
		return nums[0]+extra <= limit
	})
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 为 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：分类讨论

从 $\textit{nums}[0]$ 开始讨论：

- 如果数组中只有 $\textit{nums}[0]$，那么最大值为 $\textit{nums}[0]$。
- 再考虑 $\textit{nums}[1]$，如果 $\textit{nums}[1]\le\textit{nums}[0]$，最大值还是 $\textit{nums}[0]$；否则可以平均这两个数，平均后的最大值为平均值的上取整，即 $\left\lceil\dfrac{\textit{nums}[0]+\textit{nums}[1]}{2}\right\rceil$。
- 再考虑 $\textit{nums}[2]$，如果 $\textit{nums}[2]\le$ 前面算出的最大值，或者这三个数的平均值不超过前面算出的最大值，那么最大值不变；否则可以平均这三个数，做法同上。
- 依此类推，直到最后一个数。
- 过程中的最大值为答案。

关于上取整的计算，我们有

$$
\left\lceil\dfrac{a}{b}\right\rceil = \left\lfloor\dfrac{a+b-1}{b}\right\rfloor
$$

证明见 [上取整下取整转换公式的证明](https://zhuanlan.zhihu.com/p/1890356682149838951)。

所以平均值的计算公式为

$$
\left\lceil\dfrac{s}{i+1}\right\rceil = \left\lfloor\dfrac{s+i}{i+1}\right\rfloor
$$

```py [sol-Python3]
class Solution:
    def minimizeArrayValue(self, nums: List[int]) -> int:
        return max((s + i) // (i + 1) for i, s in enumerate(accumulate(nums)))
```

```java [sol-Java]
class Solution {
    public int minimizeArrayValue(int[] nums) {
        long ans = 0;
        long s = 0;
        for (int i = 0; i < nums.length; i++) {
            s += nums[i];
            ans = Math.max(ans, (s + i) / (i + 1));
        }
        return (int) ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimizeArrayValue(vector<int>& nums) {
        long long ans = 0, s = 0;
        for (int i = 0; i < nums.size(); i++) {
            s += nums[i];
            ans = max(ans, (s + i) / (i + 1));
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimizeArrayValue(nums []int) (ans int) {
	s := 0
	for i, x := range nums {
		s += x
		ans = max(ans, (s+i)/(i+1))
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
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

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、二分答案 / §2.4 最小化最大值`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.4 最小化最大值`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
