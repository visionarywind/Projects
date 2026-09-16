# 2389. 和有限的最长子序列

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-subsequence-with-limited-sum/
- 题目 slug：`longest-subsequence-with-limited-sum`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.5 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/longest-subsequence-with-limited-sum/solutions/1781111/fei-bao-li-zuo-fa-qian-zhui-he-er-fen-by-ny4m/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[非暴力做法：前缀和 + 二分查找 + 原地 O(1) 空间（Python/Java/C++/Go）](https://leetcode.cn/problems/longest-subsequence-with-limited-sum/solutions/1781111/fei-bao-li-zuo-fa-qian-zhui-he-er-fen-by-ny4m/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fei-bao-li-zuo-fa-qian-zhui-he-er-fen-by-ny4m`
- topic id：`1781111`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 核心思路

贪心：由于元素和有上限，为了能让子序列尽量长，子序列中的**元素值越小越好**。

对于本题来说，元素在数组中的位置是无关紧要的。因为我们从小到大算数组中的元素和，无论数组是 $[1,2,3]$ 还是 $[3,2,1]$ 还是别的顺序，不影响答案。所以可以排序。

把 $\textit{nums}$ 从小到大排序后，再从小到大选择尽量多的元素（相当于选择一个前缀），使这些元素的和不超过询问值。

## 时间优化

既然求的是前缀的元素和（前缀和），那么干脆把每个前缀和都算出来。

做法是递推：前 $i$ 个数的元素和，等于前 $i-1$ 个数的元素和，加上第 $i$ 个数的值。

例如 $[4,5,2,1]$ 排序后为 $[1,2,4,5]$，从左到右递推计算前缀和，得到 $[1,3,7,12]$。

由于 $\textit{nums}[i]$ 都是正整数，前缀和是**严格单调递增**的，这样就能在前缀和上使用二分查找：找到大于 $\textit{queries}[i]$ 的第一个数的下标，由于下标是从 $0$ 开始的，这个数的下标正好就是前缀和小于等于 $\textit{queries}[i]$ 的最长前缀的长度。

例如在 $[1,3,7,12]$ **二分查找**大于 $3$ 的第一个数（$7$），得到下标 $2$，这正好就是前缀和小于等于 $3$ 的最长前缀长度。对应到 $\textit{nums}$ 中，就是选择了 $2$ 个数（$1$ 和 $2$）作为子序列中的元素。

关于二分查找的原理，请看视频讲解：[二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)

## 空间优化

前缀和可以直接记到 $\textit{nums}$ 中。

答案可以直接记到 $\textit{queries}$ 中。

[本题视频讲解](https://www.bilibili.com/video/BV1mG411V7fj)

```py [sol-Python3]
class Solution:
    def answerQueries(self, nums: List[int], queries: List[int]) -> List[int]:
        nums.sort()
        for i in range(1, len(nums)):
            nums[i] += nums[i - 1]  # 原地求前缀和
        for i, q in enumerate(queries):
            queries[i] = bisect_right(nums, q)  # 复用 queries 作为答案
        return queries
```

```java [sol-Java]
class Solution {
    public int[] answerQueries(int[] nums, int[] queries) {
        Arrays.sort(nums);
        for (int i = 1; i < nums.length; i++) {
            nums[i] += nums[i - 1]; // 原地求前缀和
        }
        for (int i = 0; i < queries.length; i++) {
            queries[i] = upperBound(nums, queries[i]); // 复用 queries 作为答案
        }
        return queries;
    }

    // https://www.bilibili.com/video/BV1AP41137w7/
    // 返回 nums 中第一个大于 target 的数的下标（注意是大于，不是大于等于）
    // 如果这样的数不存在，则返回 nums.length
    // 时间复杂度 O(log nums.length)
    // 采用开区间写法实现
    private int upperBound(int[] nums, int target) {
        int left = -1, right = nums.length; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] <= target
            // nums[right] > target
            int mid = left + (right - left) / 2;
            if (nums[mid] > target) {
                right = mid; // 范围缩小到 (left, mid)
            } else {
                left = mid; // 范围缩小到 (mid, right)
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> answerQueries(vector<int>& nums, vector<int>& queries) {
        ranges::sort(nums);
        partial_sum(nums.begin(), nums.end(), nums.begin()); // 原地求前缀和
        for (int& q : queries) { // 用 queries 保存答案
            q = ranges::upper_bound(nums, q) - nums.begin();
        }
        return queries;
    }
};
```

```go [sol-Go]
func answerQueries(nums, queries []int) []int {
	slices.Sort(nums)
	for i := 1; i < len(nums); i++ {
		nums[i] += nums[i-1] // 原地求前缀和
	}
	for i, q := range queries {
		queries[i] = sort.SearchInts(nums, q+1) // 复用 queries 作为答案
	}
	return queries
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((n+m)\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度，$m$ 为 $\textit{queries}$ 的长度。排序为 $\mathcal{O}(n\log n)$，$m$ 次二分查找为 $\mathcal{O}(m\log n)$。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序时的栈开销。

## 思考题

把子序列改成子数组要怎么做？

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、前缀和 / §1.5 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.5 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
