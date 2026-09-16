# 2009. 使数组连续的最少操作数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-operations-to-make-array-continuous/
- 题目 slug：`minimum-number-of-operations-to-make-array-continuous`
- 来源专题：滑动窗口与双指针
- 来源分类路径：二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）
- 难度分：2084
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-operations-to-make-array-continuous/solutions/1005398/on-zuo-fa-by-endlesscheng-l7yi/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[正难则反 + 滑动窗口（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-number-of-operations-to-make-array-continuous/solutions/1005398/on-zuo-fa-by-endlesscheng-l7yi/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on-zuo-fa-by-endlesscheng-l7yi`
- topic id：`1005398`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

正难则反，考虑最多**保留**多少个元素不变。

根据题意，$[1,2,3]$ 和 $[2,3,1]$ 都被认为是连续的，所以元素位置不影响答案，我们可以先把 $\textit{nums}$ 从小到大排序，方便思考。

题目要求所有元素互不相同，对于 $\textit{nums}$ 中的相同元素，至多保留其中一个。

设 $a$ 为 $\textit{nums}$ 排序去重后的数组。把 $a[i]$ 画在数轴上，由于最终形成的连续数组的长度为 $n$，所以相当于有一个长度为 $n$ 的滑动窗口，**窗口内的点保留不变，窗口外的点需要改变，填在窗口内的空位上**。

考虑最多保留多少个元素不变，问题为：

- 计算长度固定为 $n$ 的窗口内最多可以包含多少个点。

然而，任意整数都可以是窗口的右端点，我们该如何枚举？

![lc2009.png](https://pic.leetcode.cn/1711951521-kMTJHu-lc2009.png)

**定理**：只需要枚举 $a[i]$ 作为窗口的右端点。

**证明**：在窗口从左向右滑动的过程中，如果窗口右端点处没有点，那么继续滑动，在滑到下一个点之前，窗口内包含的点的个数是不会增多的。

为了算出窗口内有多少个点，我们需要知道窗口包含的最左边的点在哪，设这个点的位置是 $\textit{a}[\textit{left}]$，则它必须大于等于窗口的左边界，即

$$
a[\textit{left}]\ge a[\textit{i}]-n+1
$$

此时窗口内有 $\textit{i}-\textit{left}+1$ 个点，取其最大值，就得到了最多保留不变的元素个数。最后用 $n$ 减去保留不变的元素个数，就得到了答案。

关于滑动窗口算法，请看视频[【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

关于原地去重算法，请参考 [26. 删除有序数组中的重复项](https://leetcode.cn/problems/remove-duplicates-from-sorted-array/)。

```py [sol-Python3]
class Solution:
    def minOperations(self, nums: List[int]) -> int:
        n = len(nums)
        nums = sorted(set(nums))  # 去重排序

        ans = left = 0
        for i, x in enumerate(nums):
            while nums[left] < x - n + 1:  # nums[left] 不在窗口中
                left += 1
            ans = max(ans, i - left + 1)
        return n - ans
```

```java [sol-Java]
class Solution {
    public int minOperations(int[] nums) {
        Arrays.sort(nums);

        // 26. 删除有序数组中的重复项
        int n = nums.length;
        int m = 1;
        for (int i = 1; i < n; i++) {
            if (nums[i] != nums[i - 1]) {
                nums[m++] = nums[i]; // 原地去重
            }
        }

        int ans = 0;
        int left = 0;
        for (int i = 0; i < m; i++) {
            while (nums[left] < nums[i] - n + 1) { // nums[left] 不在窗口中
                left++;
            }
            ans = Math.max(ans, i - left + 1);
        }
        return n - ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOperations(vector<int>& nums) {
        ranges::sort(nums);
        int n = nums.size();
        int m = ranges::unique(nums).begin() - nums.begin(); // 原地去重

        int ans = 0, left = 0;
        for (int i = 0; i < m; i++) {
            while (nums[left] < nums[i] - n + 1) { // nums[left] 不在窗口中
                left++;
            }
            ans = max(ans, i - left + 1);
        }
        return n - ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int minOperations(int* nums, int numsSize) {
    qsort(nums, numsSize, sizeof(int), cmp);

    // 26. 删除有序数组中的重复项
    int m = 1;
    for (int i = 1; i < numsSize; i++) {
        if (nums[i] != nums[i - 1]) {
            nums[m++] = nums[i]; // 原地去重
        }
    }

    int ans = 0, left = 0;
    for (int i = 0; i < m; i++) {
        while (nums[left] < nums[i] - numsSize + 1) { // nums[left] 不在窗口内
            left++;
        }
        ans = MAX(ans, i - left + 1);
    }
    return numsSize - ans;
}
```

```go [sol-Go]
func minOperations(nums []int) int {
    n := len(nums)
    slices.Sort(nums)
    nums = slices.Compact(nums) // 原地去重

    ans, left := 0, 0
    for i, x := range nums {
        for nums[left] < x-n+1 { // nums[left] 不在窗口中
            left++
        }
        ans = max(ans, i-left+1)
    }
    return n - ans
}
```

```js [sol-JavaScript]
var minOperations = function(nums) {
    nums.sort((a, b) => a - b);

    // 26. 删除有序数组中的重复项
    const n = nums.length;
    let m = 1;
    for (let i = 1; i < n; i++) {
        if (nums[i] !== nums[i - 1]) {
            nums[m++] = nums[i]; // 原地去重
        }
    }

    let ans = 0, left = 0;
    for (let i = 0; i < m; i++) {
        while (nums[left] < nums[i] - n + 1) { // nums[left] 不在窗口中
            left++;
        }
        ans = Math.max(ans, i - left + 1);
    }
    return n - ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_operations(mut nums: Vec<i32>) -> i32 {
        let n = nums.len() as i32;
        nums.sort_unstable();
        nums.dedup(); // 原地去重

        let mut ans = 0;
        let mut left = 0;
        for (i, &x) in nums.iter().enumerate() {
            while nums[left] < x - n + 1 { // nums[left] 不在窗口中
                left += 1;
            }
            ans = ans.max(i - left + 1);
        }
        n - ans as i32
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。瓶颈在排序上。
- 空间复杂度：使用哈希表去重为 $\mathcal{O}(n)$，原地去重为 $\mathcal{O}(1)$（忽略排序时的栈开销）。

## 相似题目

- [1040. 移动石子直到连续 II](https://leetcode.cn/problems/moving-stones-until-consecutive-ii/)，[我的图解](https://leetcode.cn/problems/moving-stones-until-consecutive-ii/solution/tu-jie-xia-tiao-qi-pythonjavacgo-by-endl-r1eb/)

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

本题来自 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、不定长滑动窗口 / §2.1 越短越合法/求最长/最大 / §2.1.2 进阶（选做）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
