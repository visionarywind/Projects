# 162. 寻找峰值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-peak-element/
- 题目 slug：`find-peak-element`
- 来源专题：二分算法
- 来源分类路径：四、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-peak-element/solutions/1987497/by-endlesscheng-9ass/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[严格证明，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/find-peak-element/solutions/1987497/by-endlesscheng-9ass/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-9ass`
- topic id：`1987497`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意解读

首先读懂题目，为什么要规定 $\textit{nums}[-1] = \textit{nums}[n] = -\infty$？也就是假设 $\textit{nums}[0]$ 的左边还有一个 $-\infty$，$\textit{nums}[n-1]$ 的右边还有一个 $-\infty$。

因为这可以**保证数组一定有峰值**。比如数组是严格递减的，那么 $\textit{nums}[0]$ 就是（唯一的）峰值。为什么？因为此时 $\textit{nums}[-1] < \textit{nums}[0] > \textit{nums}[1]$。注意 $-\infty$ 可以保证 $\textit{nums}[0]$ 一定比它左边的数大。同理，如果数组是严格递增的，那么 $\textit{nums}[n-1]$ 就是（唯一的）峰值。

## 性质分析

**定理**：如果 $i<n-1$ 且 $\textit{nums}[i] < \textit{nums}[i+1]$，那么在下标 $[i+1,n-1]$ 中一定存在峰值。

**证明**：反证法，假设下标 $[i+1,n-1]$ 中没有峰值。

- 由于 $i+1$ 不是峰值且 $\textit{nums}[i] < \textit{nums}[i+1]$，所以一定有 $\textit{nums}[i+1] < \textit{nums}[i+2]$ 成立，否则 $i+1$ 就是峰值了。注意题目保证相邻元素不同，不存在相邻元素相等的情况。
- 由于 $i+2$ 不是峰值且 $\textit{nums}[i+1] < \textit{nums}[i+2]$，所以一定有 $\textit{nums}[i+2] < \textit{nums}[i+3]$ 成立，否则 $i+2$ 就是峰值了。
- 依此类推，得
    $$
    \textit{nums}[i] < \textit{nums}[i+1] < \textit{nums}[i+2] < \cdots < \textit{nums}[n-1] > \textit{nums}[n] = -\infty
    $$
    这意味着 $\textit{nums}[n-1]$ 是峰值，矛盾，所以原命题成立。

同理可得，如果 $i<n-1$ 且 $\textit{nums}[i] > \textit{nums}[i+1]$，那么在 $[0,i]$ 中一定存在峰值。

所以，通过比较 $\textit{nums}[i]$ 和 $\textit{nums}[i+1]$ 的大小关系，可以二分找到峰值。

## 二分定义

下面代码采用**开区间二分**，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的，喜欢哪种写法就用哪种。

**循环不变量**：

- $> \textit{left}$ 的下标中存在峰值下标。
- $\le \textit{right}$ 的下标中存在峰值下标。
- 换句话说，任意时刻均满足 $(\textit{left},\textit{right}]$ 中存在峰值下标。

在二分过程中，我们能确定的是区间 $(\textit{left},\textit{right}]$ 里面**一定**有峰值。区间外面有没有峰值？可能有，也可能没有。

⚠**常见误区**：很多同学会把二分范围与答案范围混为一谈。请注意，二分范围 $(\textit{left},\textit{right})$ 与答案范围 $(\textit{left},\textit{right}]$ 是不一样的。比如二分循环结束的时候，二分范围是空的，你总不能说答案在空区间里面吧！

开区间二分循环结束后 $\textit{left}+1=\textit{right}$，由于 $(\textit{left},\textit{right}]$ 中只有 $\textit{right}$ 一个数，所以答案是 $\textit{right}$。

⚠**常见误区**：如果有多个峰值，我们无法在一开始、以及二分过程中就确定哪个峰值最终会成为答案。二分的思路只是不断地缩小范围，并最终找到其中的一个峰值。尤其在二分过程中，$\textit{nums}[i] < \textit{nums}[i+1]$ 并不意味着 $i$ 右边的第一个峰值一定会是最终答案。

## 细节

二分范围（注意是二分范围不是答案范围）是开区间 $(-1,n-1)$，也就是闭区间 $[0,n-2]$。

为什么二分范围不用包含 $n-1$？

这是因为，如果有且仅有一个峰值，且其下标是 $n-1$，那么一定有

$$
\textit{nums}[0] < \textit{nums}[1] < \textit{nums}[2] < \cdots < \textit{nums}[n-1]
$$

这意味着每次二分更新的都是 $\textit{left}$，最终答案自然就是 $n-1$。

> 注意本题答案是存在的，即 $[0,n-1]$ 中一定存在峰值，这可以用上文中的反证法证明。

**二分基础知识**：[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

**本题视频讲解**：[【基础算法精讲 05】](https://www.bilibili.com/video/BV1QK411d76w/)。

```py [sol-Python3]
class Solution:
    def findPeakElement(self, nums: List[int]) -> int:
        left, right = -1, len(nums) - 1  # 开区间 (-1, n-1)
        while left + 1 < right:  # 开区间不为空
            mid = (left + right) // 2
            if nums[mid] > nums[mid + 1]:  # 下坡，峰顶位置 <= mid
                right = mid
            else:  # 上坡，峰顶位置 > mid
                left = mid
        return right
```

```py [sol-Python3 库函数]
class Solution:
    def findPeakElement(self, nums: List[int]) -> int:
        check = lambda i: nums[i] > nums[i + 1]
        return bisect_left(range(len(nums) - 1), True, key=check)
```

```java [sol-Java]
class Solution {
    public int findPeakElement(int[] nums) {
        int left = -1;
        int right = nums.length - 1; // 开区间 (-1, n-1)
        while (left + 1 < right) { // 开区间不为空
            int mid = left + (right - left) / 2;
            if (nums[mid] > nums[mid + 1]) { // 下坡，峰顶位置 <= mid
                right = mid;
            } else { // 上坡，峰顶位置 > mid
                left = mid;
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findPeakElement(vector<int>& nums) {
        int left = -1, right = nums.size() - 1; // 开区间 (-1, n-1)
        while (left + 1 < right) { // 开区间不为空
            int mid = left + (right - left) / 2;
            if (nums[mid] > nums[mid + 1]) { // 下坡，峰顶位置 <= mid
                right = mid;
            } else { // 上坡，峰顶位置 > mid
                left = mid;
            }
        }
        return right;
    }
};
```

```c [sol-C]
int findPeakElement(int* nums, int numsSize) {
    int left = -1, right = numsSize - 1; // 开区间 (-1, n-1)
    while (left + 1 < right) { // 开区间不为空
        int mid = left + (right - left) / 2;
        if (nums[mid] > nums[mid + 1]) { // 下坡，峰顶位置 <= mid
            right = mid;
        } else { // 上坡，峰顶位置 > mid
            left = mid;
        }
    }
    return right;
}
```

```go [sol-Go]
func findPeakElement(nums []int) int {
    left, right := -1, len(nums)-1 // 开区间 (-1, n-1)
    for left+1 < right { // 开区间不为空
        mid := left + (right-left)/2
        if nums[mid] > nums[mid+1] { // 下坡，峰顶位置 <= mid
            right = mid
        } else { // 上坡，峰顶位置 > mid
            left = mid
        }
    }
    return right
}
```

```go [sol-Go 库函数]
func findPeakElement(nums []int) int {
    return sort.Search(len(nums)-1, func(i int) bool {
        return nums[i] > nums[i+1]
    })
}
```

```js [sol-JS]
var findPeakElement = function(nums) {
    let left = -1, right = nums.length - 1; // 开区间 (-1, n-1)
    while (left + 1 < right) { // 开区间不为空
        const mid = Math.floor((left + right) / 2);
        if (nums[mid] > nums[mid + 1]) { // 下坡，峰顶位置 <= mid
            right = mid;
        } else { // 上坡，峰顶位置 > mid
            left = mid;
        }
    }
    return right;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_peak_element(nums: Vec<i32>) -> i32 {
        let mut left = 0; // usize 用 -1 比较麻烦，这里用左闭右开区间
        let mut right = nums.len() - 1;
        while left < right {
            let mid = left + (right - left) / 2;
            if nums[mid] > nums[mid + 1] { // 下坡，峰顶位置 <= mid
                right = mid;
            } else { // 上坡，峰顶位置 > mid
                left = mid + 1;
            }
        }
        left as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

如果本题不保证 $\textit{nums}[-1] = \textit{nums}[n] = -\infty$，你会如何设计算法？

欢迎在评论区分享你的思路。

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

本题来自 `四、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
