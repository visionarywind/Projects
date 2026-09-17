# 1493. 删掉一个元素以后全为 1 的最长子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/longest-subarray-of-1s-after-deleting-one-element/
- 题目 slug：`longest-subarray-of-1s-after-deleting-one-element`
- 来源专题：动态规划
- 来源分类路径：专题：前后缀分解
- 难度分：1423
- 外部题解来源：https://leetcode.cn/problems/longest-subarray-of-1s-after-deleting-one-element/solutions/3702202/hua-dong-chuang-kou-qiu-zui-chang-python-fa07/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[滑动窗口（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/longest-subarray-of-1s-after-deleting-one-element/solutions/3702202/hua-dong-chuang-kou-qiu-zui-chang-python-fa07/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`hua-dong-chuang-kou-qiu-zui-chang-python-fa07`
- topic id：`3702202`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

删除后，子数组只有 $1$，也就是没有 $0$。那么删除之前呢？至多有一个 $0$。

所以问题相当于：

- 求最长子数组的长度（减一），满足子数组至多有一个 $0$。

子数组越短，包含的 $0$ 越少，越能满足要求；子数组越长，包含的 $0$ 越多，越无法满足要求。所以**当子数组的右端点增大时，左端点也随之增大（或者不变），这符合滑动窗口模型**。如果你不了解滑动窗口，请看视频[【基础算法精讲 03】](https://www.bilibili.com/video/BV1hd4y1r7Gq/)。

这个方法对于至多有 $k$ 个 $0$ 的题目，也是适用的。

⚠**注意**：删除之前的子数组长度是 $\textit{right}-\textit{left}+1$，由于题目一定要删除一个数（尤其是在全为 $1$ 的情况下，一定要删除一个 $1$），所以删除后的子数组长度是 $\textit{right}-\textit{left}$，用其更新答案的最大值。

```py [sol-Python3]
class Solution:
    def longestSubarray(self, nums: List[int]) -> int:
        ans = cnt0 = left = 0
        for right, x in enumerate(nums):
            # 1. 入，nums[right] 进入窗口
            cnt0 += 1 - x  # 维护窗口中的 0 的个数
            while cnt0 > 1:  # 不符合题目要求
                # 2. 出，nums[left] 离开窗口
                cnt0 -= 1 - nums[left]  # 维护窗口中的 0 的个数
                left += 1
            # 3. 更新答案，注意不是 right-left+1，因为我们要删掉一个数
            ans = max(ans, right - left)
        return ans
```

```java [sol-Java]
class Solution {
    public int longestSubarray(int[] nums) {
        int ans = 0;
        int cnt0 = 0;
        int left = 0;
        for (int right = 0; right < nums.length; right++) {
            // 1. 入，nums[right] 进入窗口
            cnt0 += 1 - nums[right]; // 维护窗口中的 0 的个数
            while (cnt0 > 1) { // 不符合题目要求
                // 2. 出，nums[left] 离开窗口
                cnt0 -= 1 - nums[left]; // 维护窗口中的 0 的个数
                left++;
            }
            // 3. 更新答案，注意不是 right-left+1，因为我们要删掉一个数
            ans = Math.max(ans, right - left);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int longestSubarray(vector<int>& nums) {
        int ans = 0, cnt0 = 0, left = 0;
        for (int right = 0; right < nums.size(); right++) {
            // 1. 入，nums[right] 进入窗口
            cnt0 += 1 - nums[right]; // 维护窗口中的 0 的个数
            while (cnt0 > 1) { // 不符合题目要求
                // 2. 出，nums[left] 离开窗口
                cnt0 -= 1 - nums[left]; // 维护窗口中的 0 的个数
                left++;
            }
            // 3. 更新答案，注意不是 right-left+1，因为我们要删掉一个数
            ans = max(ans, right - left);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int longestSubarray(int* nums, int numsSize) {
    int ans = 0, cnt0 = 0, left = 0;
    for (int right = 0; right < numsSize; right++) {
        // 1. 入，nums[right] 进入窗口
        cnt0 += 1 - nums[right]; // 维护窗口中的 0 的个数
        while (cnt0 > 1) { // 不符合题目要求
            // 2. 出，nums[left] 离开窗口
            cnt0 -= 1 - nums[left]; // 维护窗口中的 0 的个数
            left++;
        }
        // 3. 更新答案，注意不是 right-left+1，因为我们要删掉一个数
        ans = MAX(ans, right - left);
    }
    return ans;
}
```

```go [sol-Go]
func longestSubarray(nums []int) (ans int) {
    cnt0, left := 0, 0
    for right, x := range nums {
        // 1. 入，nums[right] 进入窗口
        cnt0 += 1 - x  // 维护窗口中的 0 的个数
        for cnt0 > 1 { // 不符合题目要求
            // 2. 出，nums[left] 离开窗口
            cnt0 -= 1 - nums[left] // 维护窗口中的 0 的个数
            left++
        }
        // 3. 更新答案，注意不是 right-left+1，因为我们要删掉一个数
        ans = max(ans, right-left)
    }
    return
}
```

```js [sol-JavaScript]
var longestSubarray = function(nums) {
    let ans = 0, cnt0 = 0, left = 0;
    for (let right = 0; right < nums.length; right++) {
        // 1. 入，nums[right] 进入窗口
        cnt0 += 1 - nums[right]; // 维护窗口中的 0 的个数
        while (cnt0 > 1) { // 不符合题目要求
            // 2. 出，nums[left] 离开窗口
            cnt0 -= 1 - nums[left]; // 维护窗口中的 0 的个数
            left++;
        }
        // 3. 更新答案，注意不是 right-left+1，因为我们要删掉一个数
        ans = Math.max(ans, right - left);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn longest_subarray(nums: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut cnt0 = 0;
        let mut left = 0;
        for (right, &x) in nums.iter().enumerate() {
            // 1. 入，nums[right] 进入窗口
            cnt0 += 1 - x; // 维护窗口中的 0 的个数
            while cnt0 > 1 {
                // 2. 出，nums[left] 离开窗口
                cnt0 -= 1 - nums[left]; // 维护窗口中的 0 的个数
                left += 1;
            }
            // 3. 更新答案，注意不是 right-left+1，因为我们要删掉一个数
            ans = ans.max(right - left);
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。虽然写了个二重循环，但是内层循环中对 $\textit{left}$ 加一的**总**执行次数不会超过 $n$ 次，所以总的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

见下面滑动窗口题单的「**§2.1 越短越合法/求最长/最大**」。

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
