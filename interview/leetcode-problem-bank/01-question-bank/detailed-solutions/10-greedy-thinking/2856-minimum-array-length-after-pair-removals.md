# 2856. 删除数对后的最小数组长度

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-array-length-after-pair-removals/
- 题目 slug：`minimum-array-length-after-pair-removals`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.8 相邻不同
- 难度分：1750
- 外部题解来源：https://leetcode.cn/problems/minimum-array-length-after-pair-removals/solutions/2446146/olog-n-tan-xin-er-fen-cha-zhao-pythonjav-t3qn/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(log n) 贪心+二分查找（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/minimum-array-length-after-pair-removals/solutions/2446146/olog-n-tan-xin-er-fen-cha-zhao-pythonjav-t3qn/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`olog-n-tan-xin-er-fen-cha-zhao-pythonjav-t3qn`
- topic id：`2446146`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

证明过程请看 [视频讲解](https://www.bilibili.com/video/BV1PV411N76R/) 第二题。

假设 $x$ 出现次数最多，其出现次数为 $\textit{maxCnt}$。

分类讨论：

- 如果 $\textit{maxCnt}\cdot 2 > n$，其余所有 $n-\textit{maxCnt}$ 个数都要与 $x$ 消除，所以最后剩下 $\textit{maxCnt}\cdot 2 - n$ 个数。
- 如果 $\textit{maxCnt}\cdot 2 \le n$ 且 $n$ 是偶数，那么可以把其余数消除至剩下 $\textit{maxCnt}$ 个数，然后再和 $x$ 消除，最后剩下 $0$ 个数。
- 如果 $\textit{maxCnt}\cdot 2 \le n$ 且 $n$ 是奇数，同上，最后剩下 $1$ 个数。

所以本题核心是计算 $\textit{maxCnt}$，这可以遍历一遍 $\textit{nums}$ 算出来。

但我们还可以更快！

由于 $\textit{nums}$ 是有序的，如果 $\textit{maxCnt}$ 超过数组长度的一半，那么 $\textit{nums}[n/2]$ 一定是出现次数最多的那个数！

按照 [34. 在排序数组中查找元素的第一个和最后一个位置](https://leetcode.cn/problems/find-first-and-last-position-of-element-in-sorted-array/) 的做法，可以用二分查找在 $\mathcal{O}(\log n)$ 的时间计算 $\textit{nums}[n/2]$ 第一次和最后一次出现的位置，从而算出 $\textit{maxCnt}$。

关于二分的原理，请看视频[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

```py [sol-Python3]
class Solution:
    def minLengthAfterRemovals(self, nums: List[int]) -> int:
        n = len(nums)
        x = nums[n // 2]
        max_cnt = bisect_right(nums, x) - bisect_left(nums, x)
        return max(max_cnt * 2 - n, n % 2)
```

```java [sol-Java]
class Solution {
    public int minLengthAfterRemovals(List<Integer> nums) {
        int n = nums.size();
        int x = nums.get(n / 2);
        int maxCnt = lowerBound(nums, x + 1) - lowerBound(nums, x);
        return Math.max(maxCnt * 2 - n, n % 2);
    }

    // 开区间写法
    private int lowerBound(List<Integer> nums, int target) {
        int left = -1, right = nums.size(); // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = left + (right - left) / 2;
            if (nums.get(mid) < target) {
                left = mid; // 范围缩小到 (mid, right)
            } else {
                right = mid; // 范围缩小到 (left, mid)
            }
        }
        return right; // 或者 left+1
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minLengthAfterRemovals(vector<int>& nums) {
        int n = nums.size();
        int x = nums[n / 2];
        int max_cnt = ranges::upper_bound(nums, x) - ranges::lower_bound(nums, x);
        return max(max_cnt * 2 - n, n % 2);
    }
};
```

```go [sol-Go]
func minLengthAfterRemovals(nums []int) int {
	n := len(nums)
	x := nums[n/2]
	maxCnt := sort.SearchInts(nums, x+1) - sort.SearchInts(nums, x)
	return max(maxCnt*2-n, n%2)
}
```

```js [sol-JavaScript]
var minLengthAfterRemovals = function(nums) {
    const n = nums.length;
    const x = nums[n >> 1];
    const maxCnt = lowerBound(nums, x + 1) - lowerBound(nums, x);
    return Math.max(maxCnt * 2 - n, n % 2);
};

var lowerBound = function(nums, target) {
    let left = -1, right = nums.length; // 开区间 (left, right)
    while (left + 1 < right) { // 区间不为空
        // 循环不变量：
        // nums[left] < target
        // nums[right] >= target
        const mid = left + ((right - left) >> 1);
        if (nums[mid] < target) {
            left = mid; // 范围缩小到 (mid, right)
        } else {
            right = mid; // 范围缩小到 (left, mid)
        }
    }
    return right; // 或者 left+1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.8 相邻不同`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.8 相邻不同`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
