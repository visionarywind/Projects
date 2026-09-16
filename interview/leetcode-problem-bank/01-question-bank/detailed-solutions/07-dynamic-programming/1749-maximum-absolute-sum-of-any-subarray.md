# 1749. 任意子数组和的绝对值的最大值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-absolute-sum-of-any-subarray/
- 题目 slug：`maximum-absolute-sum-of-any-subarray`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.3 最大子数组和（最大子段和）
- 难度分：1542
- 外部题解来源：https://leetcode.cn/problems/maximum-absolute-sum-of-any-subarray/solutions/2377930/liang-chong-fang-fa-dong-tai-gui-hua-qia-dczr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：动态规划/前缀和（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-absolute-sum-of-any-subarray/solutions/2377930/liang-chong-fang-fa-dong-tai-gui-hua-qia-dczr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-dong-tai-gui-hua-qia-dczr`
- topic id：`2377930`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 核心思路

要让 $x$ 的绝对值 $|x|$ 尽量大，只有两种情况：

- 如果 $x\ge 0$，那么 $x$ 越大越好，问题转化成 [53. 最大子数组和](https://leetcode.cn/problems/maximum-subarray/)。
- 如果 $x < 0$，那么 $x$ 越小，$-x$ 就越大，问题转化成**最小子数组和**。

我们可以把最大子数组和与最小子数组和（的相反数）都算出来，二者取最大值，即为答案。

## 方法一：动态规划

**前置知识**：[动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

考虑以 $\textit{nums}[i]$ 结尾的最大子数组和：

- 如果子数组只有一个数，那么最大子数组和就是 $\textit{nums}[i]$。
- 如果把 $\textit{nums}[i]$ 和前面的子数组拼起来，那么问题变成求「以 $\textit{nums}[i-1]$ 结尾的最大子数组和」。

这启发我们得到下面的状态定义和状态转移方程。

定义 $f[i]$ 表示以 $\textit{nums}[i]$ 结尾的最大子数组和：

- 如果子数组只有一个数：$f[i]=\textit{nums}[i]$。
- 如果把 $\textit{nums}[i]$ 和前面的子数组拼起来：$f[i]=f[i-1]+\textit{nums}[i]$。
- 这两种情况取最大值，即

$$
f[i] = \max(\textit{nums}[i], f[i-1]+\textit{nums}[i]) = \max(f[i-1], 0) + \textit{nums}[i] 
$$

枚举子数组的最后一个数，最大子数组和就是

$$
\max(\max(f), 0)
$$

这里与 $0$ 取最大值是因为子数组可以为空。

最小子数组和的计算方法与最大子数组和类似。

代码实现时，可以按照 [视频](https://www.bilibili.com/video/BV1Xj411K7oF/) 中讲的，只用几个变量滚动计算。

```py [sol-Python3]
class Solution:
    def maxAbsoluteSum(self, nums: List[int]) -> int:
        ans = f_max = f_min = 0
        for x in nums:
            f_max = max(f_max, 0) + x
            f_min = min(f_min, 0) + x
            ans = max(ans, f_max, -f_min)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxAbsoluteSum(int[] nums) {
        int ans = 0, fMax = 0, fMin = 0;
        for (int x : nums) {
            fMax = Math.max(fMax, 0) + x;
            fMin = Math.min(fMin, 0) + x;
            ans = Math.max(ans, Math.max(fMax, -fMin));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxAbsoluteSum(vector<int>& nums) {
        int ans = 0, f_max = 0, f_min = 0;
        for (int x: nums) {
            f_max = max(f_max, 0) + x;
            f_min = min(f_min, 0) + x;
            ans = max({ans, f_max, -f_min});
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int maxAbsoluteSum(int* nums, int numsSize) {
    int ans = 0, f_max = 0, f_min = 0;
    for (int i = 0; i < numsSize; i++) {
        int x = nums[i];
        f_max = MAX(f_max, 0) + x;
        f_min = MIN(f_min, 0) + x;
        ans = MAX(ans, MAX(f_max, -f_min));
    }
    return ans;
}
```

```go [sol-Go]
func maxAbsoluteSum(nums []int) (ans int) {
    var fMax, fMin int
    for _, x := range nums {
        fMax = max(fMax, 0) + x
        fMin = min(fMin, 0) + x
        ans = max(ans, max(fMax, -fMin))
    }
    return
}
```

```js [sol-JavaScript]
var maxAbsoluteSum = function(nums) {
    let ans = 0, fMax = 0, fMin = 0;
    for (const x of nums) {
        fMax = Math.max(fMax, 0) + x;
        fMin = Math.min(fMin, 0) + x;
        ans = Math.max(ans, fMax, -fMin);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_absolute_sum(nums: Vec<i32>) -> i32 {
        let mut ans = 0;
        let mut f_max = 0;
        let mut f_min = 0;
        for x in nums {
            f_max = f_max.max(0) + x;
            f_min = f_min.min(0) + x;
            ans = ans.max(f_max).max(-f_min);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 方法二：前缀和

**前置知识**：[前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

设 $\textit{nums}$ 的前缀和数组为 $s$，根据前置知识，子数组的和等于两个前缀和的差 $s[j] - s[i]$。所以子数组和的绝对值等于

$$
|s[j] - s[i]|
$$

$s[i]$ 和 $s[j]$ 相差越大，上式也就越大。

给你一堆数，哪两个数相差最大？

这堆数字中的最大值和最小值相差最大。

所以，最大的差来自 $s$ 中的最大值和最小值，所以答案为

$$
\max(s) - \min(s)
$$

你也可以这样理解，在 $s$ 数组中：

- 如果最大前缀和出现在最小前缀和的**右边**，那么上式算的是最大子数组和。
- 如果最大前缀和出现在最小前缀和的**左边**，那么上式算的是最小子数组和的绝对值。

⚠**注意**：由于前缀和的第一项 $s[0]=0$（见 [讲解](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)），所以代码中的 $\textit{mn}$ 和 $\textit{mx}$ 均初始化为 $0$。

```py [sol-Python3]
class Solution:
    def maxAbsoluteSum(self, nums: List[int]) -> int:
        s = list(accumulate(nums, initial=0))  # nums 的前缀和
        return max(s) - min(s)
```

```java [sol-Java]
class Solution {
    public int maxAbsoluteSum(int[] nums) {
        int s = 0, mx = 0, mn = 0;
        for (int x : nums) {
            s += x;
            // mx = Math.max(mx, s);
            // mn = Math.min(mn, s);
            if (s > mx) mx = s;
            else if (s < mn) mn = s; // 效率更高的写法
        }
        return mx - mn;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxAbsoluteSum(vector<int>& nums) {
        int s = 0, mx = 0, mn = 0;
        for (int x : nums) {
            s += x;
            mx = max(mx, s);
            mn = min(mn, s);
        }
        return mx - mn;
    }
};
```

```c [sol-C]
int maxAbsoluteSum(int* nums, int numsSize) {
    int s = 0, mx = 0, mn = 0;
    for (int i = 0; i < numsSize; i++) {
        s += nums[i];
        if (s > mx) {
            mx = s;
        }
        if (s < mn) {
            mn = s;
        }
    }
    return mx - mn;
}
```

```go [sol-Go]
func maxAbsoluteSum(nums []int) int {
    var s, mx, mn int
    for _, x := range nums {
        s += x
        mx = max(mx, s)
        mn = min(mn, s)
    }
    return mx - mn
}
```

```js [sol-JavaScript]
var maxAbsoluteSum = function(nums) {
    let s = 0, mx = 0, mn = 0;
    for (const x of nums) {
        s += x;
        mx = Math.max(mx, s);
        mn = Math.min(mn, s);
    }
    return mx - mn;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_absolute_sum(nums: Vec<i32>) -> i32 {
        let mut s = 0;
        let mut mx = 0;
        let mut mn = 0;
        for x in nums {
            s += x;
            mx = mx.max(s);
            mn = mn.min(s);
        }
        mx - mn
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$ 或 $\mathcal{O}(1)$。Python 为了方便直接生成了前缀和数组，其它语言是一边遍历一边计算的。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. 【本题相关】[动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. 【本题相关】[常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、入门 DP / §1.3 最大子数组和（最大子段和）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、入门 DP / §1.3 最大子数组和（最大子段和）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
