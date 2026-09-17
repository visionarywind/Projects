# 1877. 数组中最大数对和的最小值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimize-maximum-pair-sum-in-array/
- 题目 slug：`minimize-maximum-pair-sum-in-array`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.2 单序列配对
- 难度分：1301
- 外部题解来源：https://leetcode.cn/problems/minimize-maximum-pair-sum-in-array/solutions/802347/tan-xin-by-endlesscheng-fwk8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[证明，交换论证法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimize-maximum-pair-sum-in-array/solutions/802347/tan-xin-by-endlesscheng-fwk8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-by-endlesscheng-fwk8`
- topic id：`802347`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

你可能猜了一个结论：**把最小的数和最大的数配对，把第二小的数和第二大的数配对，依此类推**。注意题目保证 $n$ 是偶数。

但是，如何证明这样做是对的？

设 $\textit{nums}$ 排序后的结果为 $a_1\le a_2\le \cdots \le a_n$。

首先证明，$a_1$ 与 $a_n$ 配对是最优的。

设 $a_i$ 和 $a_j$ 是数组中的另外两个数，考虑如下两种配对方案：

- $(a_1,a_i)$ 和 $(a_j,a_n)$。最大数对和为 $M_1 = \max(a_1+a_i,a_j+a_n,其他数对和)$。
- $(a_1,a_n)$ 和 $(a_i,a_j)$。最大数对和为 $M_2 = \max(a_1+a_n,a_i+a_j,其他数对和)$。

由于 $a_1\le a_j$，$a_i\le a_n$，所以 $a_1+a_i\le a_j+a_n$，所以 $M_1 = \max(a_j+a_n,其他数对和)$。

由于 $a_1+a_n\le a_j+a_n$ 且 $a_i+a_j\le a_j+a_n$，所以 $\max(a_1+a_n,a_i+a_j)\le a_j+a_n$，所以 $M_2\le M_1$。

这意味着，对于任意最优配对方案，将其调整为 $a_1$ 和 $a_n$ 配对，**不会让最大数对和变得更大**。所以存在最优配对方案，其中 $a_1$ 和 $a_n$ 是配对的。

去掉 $a_1$ 和 $a_n$（已配对），问题变成一个规模更小的子问题（$n-2$ 个数），同理可得其他数的配对方式。

```py [sol-Python3]
class Solution:
    def minPairSum(self, nums: List[int]) -> int:
        nums.sort()
        n = len(nums)
        return max(nums[i] + nums[-1 - i] for i in range(n // 2))
```

```java [sol-Java]
class Solution {
    public int minPairSum(int[] nums) {
        Arrays.sort(nums);
        int n = nums.length;
        int ans = 0;
        for (int i = 0; i < n / 2; i++) {
            ans = Math.max(ans, nums[i] + nums[n - 1 - i]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minPairSum(vector<int>& nums) {
        ranges::sort(nums);
        int n = nums.size();
        int ans = 0;
        for (int i = 0; i < n / 2; i++) {
            ans = max(ans, nums[i] + nums[n - 1 - i]);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int minPairSum(int* nums, int numsSize) {
    qsort(nums, numsSize, sizeof(int), cmp);
    int ans = 0;
    for (int i = 0; i < numsSize / 2; i++) {
        ans = MAX(ans, nums[i] + nums[numsSize - 1 - i]);
    }
    return ans;
}
```

```go [sol-Go]
func minPairSum(nums []int) (ans int) {
	slices.Sort(nums)
	n := len(nums)
	for i, x := range nums[:n/2] {
		ans = max(ans, x+nums[n-1-i])
	}
	return
}
```

```js [sol-JavaScript]
var minPairSum = function(nums) {
    nums.sort((a, b) => a - b);
    const n = nums.length;
    let ans = 0;
    for (let i = 0; i < n / 2; i++) {
        ans = Math.max(ans, nums[i] + nums[n - 1 - i]);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_pair_sum(mut nums: Vec<i32>) -> i32 {
        nums.sort_unstable();
        let n = nums.len();
        let mut ans = 0;
        for i in 0..n / 2 {
            ans = ans.max(nums[i] + nums[n - 1 - i]);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 专题训练

见下面贪心题单的「**§1.2 单序列配对**」和「**§1.7 交换论证法**」。

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

本题来自 `一、贪心策略 / §1.2 单序列配对`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.2 单序列配对`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
