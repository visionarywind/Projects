# 3727. 最大交替平方和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-alternating-sum-of-squares/
- 题目 slug：`maximum-alternating-sum-of-squares`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：1455
- 外部题解来源：https://leetcode.cn/problems/maximum-alternating-sum-of-squares/solutions/3815653/tan-xin-pai-xu-kuai-su-xuan-ze-pythonjav-qfmr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心：排序 / 快速选择（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-alternating-sum-of-squares/solutions/3815653/tan-xin-pai-xu-kuai-su-xuan-ze-pythonjav-qfmr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-pai-xu-kuai-su-xuan-ze-pythonjav-qfmr`
- topic id：`3815653`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

把 $\textit{nums}$ 每一项平方，然后从小到大排序。

设 $m = \left\lfloor\dfrac{n}{2}\right\rfloor$。贪心地，把前 $m$ 小作为奇数项（减去），后 $n-m$ 大作为偶数项（加上），这样算出来的交替和是最大的。

[本题视频讲解](https://www.bilibili.com/video/BV1eqxNzXE8v/?t=4m35s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxAlternatingSum(self, nums: List[int]) -> int:
        nums = sorted(x * x for x in nums)  # 原地写法见另一份代码

        m = len(nums) // 2
        # 交替和：减去小的，加上大的
        return sum(nums[m:]) - sum(nums[:m])
```

```py [sol-Python3 原地]
class Solution:
    def maxAlternatingSum(self, nums: List[int]) -> int:
        for i, x in enumerate(nums):
            nums[i] *= x
        nums.sort()

        m = len(nums) // 2
        # 交替和：减去小的，加上大的
        return sum(nums[m:]) - sum(nums[:m])
```

```java [sol-Java]
class Solution {
    public long maxAlternatingSum(int[] nums) {
        int n = nums.length;
        for (int i = 0; i < n; i++) {
            nums[i] *= nums[i];
        }
        Arrays.sort(nums);

        int m = n / 2;
        // 交替和：减去小的，加上大的
        long ans = 0;
        for (int i = 0; i < m; i++) {
            ans -= nums[i];
        }
        for (int i = m; i < n; i++) {
            ans += nums[i];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxAlternatingSum(vector<int>& nums) {
        for (int& x : nums) {
            x *= x;
        }
        int m = nums.size() / 2;
        ranges::nth_element(nums, nums.begin() + m);

        // 交替和：减去小的，加上大的
        return -accumulate(nums.begin(), nums.begin() + m, 0LL)
               +accumulate(nums.begin() + m, nums.end(), 0LL);
    }
};
```

```cpp [sol-C++ 写法二]
class Solution {
public:
    long long maxAlternatingSum(vector<int>& nums) {
        for (int& x : nums) {
            x *= x;
        }
        int m = nums.size() / 2;
        ranges::nth_element(nums, nums.begin() + m);

        // 交替和：减去小的，加上大的
        return -reduce(nums.begin(), nums.begin() + m, 0LL, plus<long long>())
               +reduce(nums.begin() + m, nums.end(), 0LL, plus<long long>());
    }
};
```

```go [sol-Go]
func maxAlternatingSum(nums []int) (ans int64) {
	for i, x := range nums {
		nums[i] *= x
	}
	slices.Sort(nums)

	// 交替和：减去小的，加上大的
	m := len(nums) / 2
	for _, x := range nums[:m] {
		ans -= int64(x)
	}
	for _, x := range nums[m:] {
		ans += int64(x)
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$ 或 $\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。瓶颈在排序上。用**快速选择算法**可以做到 $\mathcal{O}(n)$，见 C++ 代码。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 专题训练

见下面贪心题单的「**§1.1 从最小/最大开始贪心**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.1 从最小/最大开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.1 从最小/最大开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
