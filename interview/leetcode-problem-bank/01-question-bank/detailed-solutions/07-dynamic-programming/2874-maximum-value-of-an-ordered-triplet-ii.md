# 2874. 有序三元组中的最大值 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-value-of-an-ordered-triplet-ii/
- 题目 slug：`maximum-value-of-an-ordered-triplet-ii`
- 来源专题：动态规划
- 来源分类路径：专题：前后缀分解
- 难度分：1583
- 外部题解来源：https://leetcode.cn/problems/maximum-value-of-an-ordered-triplet-ii/solutions/2464857/mei-ju-jzhao-qian-hou-zui-da-zhi-pythonj-um8q/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：枚举 j / 枚举 k（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-value-of-an-ordered-triplet-ii/solutions/2464857/mei-ju-jzhao-qian-hou-zui-da-zhi-pythonj-um8q/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-jzhao-qian-hou-zui-da-zhi-pythonj-um8q`
- topic id：`2464857`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 方法一：枚举 j

枚举 $j$，为了让 $(\textit{nums}[i] - \textit{nums}[j]) * \textit{nums}[k]$ 尽量大，我们需要知道 $j$ 左侧元素的最大值（让 $\textit{nums}[i] - \textit{nums}[j]$ 尽量大），以及 $j$ 右侧元素的最大值（让乘积尽量大）。

也就是计算 $\textit{nums}$ 的前缀最大值 $\textit{preMax}$ 和后缀最大值 $\textit{sufMax}$，这可以用递推预处理：

- $\textit{preMax}[i] = \max(\textit{preMax}[i-1], \textit{nums}[i])$
- $\textit{sufMax}[i] = \max(\textit{sufMax}[i+1], \textit{nums}[i])$

代码实现时，可以只预处理 $\textit{sufMax}$ 数组，$\textit{preMax}$ 可以在计算答案的同时算出来。

[视频讲解](https://www.bilibili.com/video/BV18j411b7v4/) 第二题。

```py [sol-Python3]
class Solution:
    def maximumTripletValue(self, nums: List[int]) -> int:
        n = len(nums)
        suf_max = [0] * (n + 1)
        for i in range(n - 1, 1, -1):
            suf_max[i] = max(suf_max[i + 1], nums[i])

        ans = pre_max = 0
        for j, x in enumerate(nums):
            ans = max(ans, (pre_max - x) * suf_max[j + 1])
            pre_max = max(pre_max, x)
        return ans
```

```java [sol-Java]
class Solution {
    public long maximumTripletValue(int[] nums) {
        int n = nums.length;
        int[] sufMax = new int[n + 1];
        for (int i = n - 1; i > 1; i--) {
            sufMax[i] = Math.max(sufMax[i + 1], nums[i]);
        }

        long ans = 0;
        int preMax = nums[0];
        for (int j = 1; j < n - 1; j++) {
            ans = Math.max(ans, (long) (preMax - nums[j]) * sufMax[j + 1]);
            preMax = Math.max(preMax, nums[j]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maximumTripletValue(vector<int>& nums) {
        int n = nums.size();
        vector<int> suf_max(n + 1);
        for (int i = n - 1; i > 1; i--) {
            suf_max[i] = max(suf_max[i + 1], nums[i]);
        }

        long long ans = 0;
        int pre_max = nums[0];
        for (int j = 1; j < n - 1; j++) {
            ans = max(ans, 1LL * (pre_max - nums[j]) * suf_max[j + 1]);
            pre_max = max(pre_max, nums[j]);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

long long maximumTripletValue(int* nums, int n) {
    int* suf_max = malloc(n * sizeof(int));
    suf_max[n - 1] = nums[n - 1];
    for (int i = n - 2; i > 1; i--) {
        suf_max[i] = MAX(suf_max[i + 1], nums[i]);
    }

    long long ans = 0;
    int pre_max = nums[0];
    for (int j = 1; j < n - 1; j++) {
        ans = MAX(ans, 1LL * (pre_max - nums[j]) * suf_max[j + 1]);
        pre_max = MAX(pre_max, nums[j]);
    }

    free(suf_max);
    return ans;
}
```

```go [sol-Go]
func maximumTripletValue(nums []int) int64 {
    ans := 0
    n := len(nums)
    sufMax := make([]int, n+1)
    for i := n - 1; i > 1; i-- {
        sufMax[i] = max(sufMax[i+1], nums[i])
    }

    preMax := 0
    for j, x := range nums {
        ans = max(ans, (preMax-x)*sufMax[j+1])
        preMax = max(preMax, x)
    }
    return int64(ans)
}
```

```js [sol-JavaScript]
var maximumTripletValue = function(nums) {
    let n = nums.length;
    let sufMax = Array(n);
    sufMax[n - 1] = nums[n - 1];
    for (let i = n - 2; i > 1; i--) {
        sufMax[i] = Math.max(sufMax[i + 1], nums[i]);
    }

    let ans = 0;
    let preMax = nums[0];
    for (let j = 1; j < n - 1; j++) {
        ans = Math.max(ans, (preMax - nums[j]) * sufMax[j + 1]);
        preMax = Math.max(preMax, nums[j]);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum_triplet_value(nums: Vec<i32>) -> i64 {
        let n = nums.len();
        let mut suf_max = vec![0; n + 1];
        for i in (2..n).rev() {
            suf_max[i] = suf_max[i + 1].max(nums[i]);
        }

        let mut ans = 0;
        let mut pre_max = nums[0];
        for j in 1..n - 1 {
            ans = ans.max((pre_max - nums[j]) as i64 * suf_max[j + 1] as i64);
            pre_max = pre_max.max(nums[j]);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：枚举 k

枚举 $k$，我们需要知道 $k$ 左边 $\textit{nums}[i] - \textit{nums}[j]$ 的最大值。

类似 [121. 买卖股票的最佳时机](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock/)，为了计算 $\textit{nums}[i] - \textit{nums}[j]$ 的最大值，我们需要知道 $j$ 左边的 $\textit{nums}[i]$ 的最大值。

因此，在遍历的过程中：

- 维护 $\textit{nums}[i]$ 的最大值 $\textit{preMax}$。
- 维护 $\textit{preMax} - \textit{nums}[j]$ 的最大值 $\textit{maxDiff}$。
- 计算 $\textit{maxDiff} \cdot \textit{nums}[k]$，更新答案的最大值。

代码实现时，要先更新 $\textit{ans}$，再更新 $\textit{maxDiff}$，最后更新 $\textit{preMax}$。为什么？

这个顺序是精心设置的：

- 首先更新 $\textit{ans}$，此时 $\textit{maxDiff}$ 还没有更新，表示在当前元素**左边**的两个数的最大差值。
- 然后更新 $\textit{maxDiff}$，此时 $\textit{preMax}$ 还没有更新，表示在当前元素**左边**的最大值。
- 最后更新 $\textit{preMax}$。

能否修改更新顺序？

$\textit{ans}$ 依赖 $\textit{maxDiff}$，$\textit{maxDiff}$ 依赖 $\textit{preMax}$。如果修改更新顺序，那么 $\textit{maxDiff}$ 或者 $\textit{preMax}$ 会包含当前元素，就不是**左边**元素的计算结果了，这违反了题目 $i<j<k$ 的规定。

```py [sol-Python3]
class Solution:
    def maximumTripletValue(self, nums: List[int]) -> int:
        ans = max_diff = pre_max = 0
        for x in nums:
            ans = max(ans, max_diff * x)
            max_diff = max(max_diff, pre_max - x)
            pre_max = max(pre_max, x)
        return ans
```

```java [sol-Java]
class Solution {
    public long maximumTripletValue(int[] nums) {
        long ans = 0;
        int maxDiff = 0;
        int preMax = 0;
        for (int x : nums) {
            ans = Math.max(ans, (long) maxDiff * x);
            maxDiff = Math.max(maxDiff, preMax - x);
            preMax = Math.max(preMax, x);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maximumTripletValue(vector<int>& nums) {
        long long ans = 0;
        int max_diff = 0, pre_max = 0;
        for (int x : nums) {
            ans = max(ans, 1LL * max_diff * x);
            max_diff = max(max_diff, pre_max - x);
            pre_max = max(pre_max, x);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

long long maximumTripletValue(int* nums, int n) {
    long long ans = 0;
    int max_diff = 0, pre_max = 0;
    for (int i = 0; i < n; i++) {
        ans = MAX(ans, 1LL * max_diff * nums[i]);
        max_diff = MAX(max_diff, pre_max - nums[i]);
        pre_max = MAX(pre_max, nums[i]);
    }
    return ans;
}
```

```go [sol-Go]
func maximumTripletValue(nums []int) int64 {
    var ans, maxDiff, preMax int
    for _, x := range nums {
        ans = max(ans, maxDiff*x)
        maxDiff = max(maxDiff, preMax-x)
        preMax = max(preMax, x)
    }
    return int64(ans)
}
```

```js [sol-JavaScript]
var maximumTripletValue = function(nums) {
    let ans = 0, maxDiff = 0, preMax = 0;
    for (const x of nums) {
        ans = Math.max(ans, maxDiff * x);
        maxDiff = Math.max(maxDiff, preMax - x);
        preMax = Math.max(preMax, x);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn maximum_triplet_value(nums: Vec<i32>) -> i64 {
        let mut ans = 0;
        let mut max_diff = 0;
        let mut pre_max = 0;
        for x in nums {
            ans = ans.max(max_diff as i64 * x as i64);
            max_diff = max_diff.max(pre_max - x);
            pre_max = pre_max.max(x);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

如果 $\textit{nums}$ 中有负数，要怎么做？

欢迎在评论区分享你的思路/代码。

更多相似题目，见下面数据结构题单中的「**§0.1 枚举右，维护左**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. 【本题相关】[常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `专题：前后缀分解`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `专题：前后缀分解`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
