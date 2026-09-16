# 1287. 有序数组中出现次数超过 25% 的元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/element-appearing-more-than-25-in-sorted-array/
- 题目 slug：`element-appearing-more-than-25-in-sorted-array`
- 来源专题：二分算法
- 来源分类路径：一、二分查找 / §1.2 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/element-appearing-more-than-25-in-sorted-array/solutions/3067559/olog-n-er-fen-cha-zhao-zheng-que-xing-zh-5mu9/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(log n) 做法，只需两次二分查找（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/element-appearing-more-than-25-in-sorted-array/solutions/3067559/olog-n-er-fen-cha-zhao-zheng-que-xing-zh-5mu9/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`olog-n-er-fen-cha-zhao-zheng-que-xing-zh-5mu9`
- topic id：`3067559`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

「出现次数**超过**数组元素总数的 $25\%$」等价于出现次数至少为 $\left\lfloor\dfrac{n}{4}\right\rfloor+1$，其中 $n$ 是 $\textit{arr}$ 的长度。

由于 $\textit{arr}$ 是有序的，**相同的元素是连续的**。利用这个性质，我们无需遍历整个数组，而是检查若干下标 $i$，然后 [34. 在排序数组中查找元素的第一个和最后一个位置](https://leetcode.cn/problems/find-first-and-last-position-of-element-in-sorted-array/)，算出等于 $\textit{arr}[i]$ 的元素个数。

需要检查哪些下标呢？好比用网去捕鱼，如果网孔比鱼还大，那么就会出现漏网之鱼。如果检查了下标 $i$ 和 $j$，但在这两个下标之间（不含 $i,j$）的元素个数比 $\left\lfloor\dfrac{n}{4}\right\rfloor$ 还大，那么就可能会错过答案（漏网之鱼）。

设 $m=\left\lfloor\dfrac{n}{4}\right\rfloor$。我们至多检查 $3$ 个下标 $m,2m+1,3m+2$，其中一定有一个 $\textit{arr}[i]$ 的出现次数至少为 $m+1$。

**证明**：用反证法证明。假设 $\textit{arr}[m]$、$\textit{arr}[2m+1]$ 和 $\textit{arr}[3m+2]$ 的出现次数都小于 $m+1$，那么：

- 下标在 $[0,m-1]$ 中的数，出现次数至多为 $m$。（注：这也可以反证，如果出现次数大于 $m$，那么必然包含 $\textit{arr}[m]$，矛盾。）
- 下标在 $[m+1,2m]$ 中的数，出现次数至多为 $m$。
- 下标在 $[2m+2,3m+1]$ 中的数，出现次数至多为 $m$。
- 下标在 $[3m+3,n-1]$ 中的数，出现次数至多为 $n-3m-3$。由于 $4m=4\left\lfloor\dfrac{n}{4}\right\rfloor = n-(n\bmod 4)\ge n-3$，所以移项得 $n-3m-3\le m$。
- 于是整个数组都没有出现次数至少为 $m+1$ 的数，与已知条件矛盾。所以原命题成立。

计算 $\ge \textit{arr}[i]$ 的第一个位置 $p$ 和 $> \textit{arr}[i]$ 的第一个位置 $q$，那么下标在 $[p,q-1]$ 中的数都等于 $\textit{arr}[i]$，所以 $\textit{arr}[i]$ 的出现次数是 $q-p$。如果出现次数 $\ge m+1$，即 $> m$，那么返回 $\textit{arr}[i]$。

由于题目保证答案一定存在，可以只检查 $m$ 和 $2m+1$ 这两个下标，如果都不符合要求，那么答案一定是 $\textit{arr}[3m+2]$。这样可以少跑两次二分。

> **注**：虽然 $n=1$ 的时候 $2m+1=1$ 越界，但 $i=m=0$ 的时候一定会返回 $\textit{arr}[0]$，所以不会出现这种情况。

## 优化前：四次二分

```py [sol-Python3]
class Solution:
    def findSpecialInteger(self, arr: List[int]) -> int:
        m = len(arr) // 4
        for i in (m, m * 2 + 1):
            x = arr[i]
            if bisect_right(arr, x) - bisect_left(arr, x) > m:
                return x
        # 如果答案不是 arr[m] 也不是 arr[2m+1]，那么答案一定是 arr[3m+2]
        return arr[m * 3 + 2]
```

```java [sol-Java]
class Solution {
    public int findSpecialInteger(int[] arr) {
        int m = arr.length / 4;
        for (int i : new int[]{m, m * 2 + 1}) {
            int x = arr[i];
            // > x 等价于 >= x+1
            if (lowerBound(arr, x + 1) - lowerBound(arr, x) > m) {
                return x;
            }
        }
        // 如果答案不是 arr[m] 也不是 arr[2m+1]，那么答案一定是 arr[3m+2]
        return arr[m * 3 + 2];
    }

    // lowerBound 返回最小的满足 nums[i] >= target 的下标 i
    // 如果数组为空，或者所有数都 < target，则返回 nums.length
    // 要求 nums 是非递减的，即 nums[i] <= nums[i + 1]
    // 原理见 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(int[] nums, int target) {
        int left = -1;
        int right = nums.length; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = left + (right - left) / 2;
            if (nums[mid] >= target) {
                right = mid; // 范围缩小到 (left, mid)
            } else {
                left = mid; // 范围缩小到 (mid, right)
            }
        }
        // 循环结束后 left+1 = right
        // 此时 nums[left] < target 而 nums[right] >= target
        // 所以 right 就是第一个 >= target 的元素下标
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findSpecialInteger(vector<int>& arr) {
        int m = arr.size() / 4;
        for (int i : {m, m * 2 + 1}) {
            int x = arr[i];
            if (ranges::upper_bound(arr, x) - ranges::lower_bound(arr, x) > m) {
                return x;
            }
        }
        // 如果答案不是 arr[m] 也不是 arr[2m+1]，那么答案一定是 arr[3m+2]
        return arr[m * 3 + 2];
    }
};
```

```cpp [sol-C++ 写法二]
class Solution {
public:
    int findSpecialInteger(vector<int>& arr) {
        int m = arr.size() / 4;
        for (int i : {m, m * 2 + 1}) {
            int x = arr[i];
            auto [p, q] = ranges::equal_range(arr, x);
            if (q - p > m) {
                return x;
            }
        }
        // 如果答案不是 arr[m] 也不是 arr[2m+1]，那么答案一定是 arr[3m+2]
        return arr[m * 3 + 2];
    }
};
```

```c [sol-C]
// lowerBound 返回最小的满足 nums[i] >= target 的下标 i
// 如果数组为空，或者所有数都 < target，则返回 numsSize
// 要求 nums 是非递减的，即 nums[i] <= nums[i + 1]
// 原理见 https://www.bilibili.com/video/BV1AP41137w7/
int lowerBound(int* nums, int numsSize, int target) {
    int left = -1, right = numsSize; // 开区间 (left, right)
    while (left + 1 < right) { // 区间不为空
        // 循环不变量：
        // nums[left] < target
        // nums[right] >= target
        int mid = left + (right - left) / 2;
        if (nums[mid] >= target) {
            right = mid; // 范围缩小到 (left, mid)
        } else {
            left = mid; // 范围缩小到 (mid, right)
        }
    }
    // 循环结束后 left+1 = right
    // 此时 nums[left] < target 而 nums[right] >= target
    // 所以 right 就是第一个 >= target 的元素下标
    return right;
}

int findSpecialInteger(int* arr, int arrSize) {
    int m = arrSize / 4;
    for (int i = m; i <= m * 2 + 1; i += m + 1) {
        int x = arr[i];
        // > x 等价于 >= x+1
        if (lowerBound(arr, arrSize, x + 1) - lowerBound(arr, arrSize, x) > m) {
            return x;
        }
    }
    // 如果答案不是 arr[m] 也不是 arr[2m+1]，那么答案一定是 arr[3m+2]
    return arr[m * 3 + 2];
}
```

```go [sol-Go]
func findSpecialInteger(arr []int) int {
    m := len(arr) / 4
    for _, i := range []int{m, m*2 + 1} {
        x := arr[i]
        // > x 等价于 >= x+1
        if sort.SearchInts(arr, x+1)-sort.SearchInts(arr, x) > m {
            return x
        }
    }
    // 如果答案不是 arr[m] 也不是 arr[2m+1]，那么答案一定是 arr[3m+2]
    return arr[m*3+2]
}
```

```js [sol-JavaScript]
var findSpecialInteger = function(arr) {
    const m = Math.floor(arr.length / 4);
    for (const i of [m, m * 2 + 1]) {
        const x = arr[i];
        if (_.sortedLastIndex(arr, x) - _.sortedIndex(arr, x) > m) {
            return x;
        }
    }
    // 如果答案不是 arr[m] 也不是 arr[2m+1]，那么答案一定是 arr[3m+2]
    return arr[m * 3 + 2];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_special_integer(arr: Vec<i32>) -> i32 {
        let m = arr.len() / 4;
        for i in [m, m * 2 + 1] {
            let x = arr[i];
            if arr.partition_point(|&y| y <= x) - arr.partition_point(|&y| y < x) > m {
                return x;
            }
        }
        // 如果答案不是 arr[m] 也不是 arr[2m+1]，那么答案一定是 arr[3m+2]
        arr[m * 3 + 2]
    }
}
```

## 优化：两次二分

上面的做法是，直接求出恰好等于 $x=\textit{arr}[i]$ 的元素个数。

但本题只要求判断元素个数是否大于 $m$，我们可以先求出最左边的 $x$ 的下标 $j$，然后判断 $\textit{arr}[j+m]$ 是否等于 $x$。这样又可以少跑两次二分。

**问**：$j+m$ 会不会下标越界？

**答**：不会。反证法，如果下标越界，说明没有符合要求的数，与题目给定条件矛盾。

```py [sol-Python3]
class Solution:
    def findSpecialInteger(self, arr: List[int]) -> int:
        m = len(arr) // 4
        for i in (m, m * 2 + 1):
            x = arr[i]
            j = bisect_left(arr, x)
            if arr[j + m] == x:
                return x
        return arr[m * 3 + 2]
```

```java [sol-Java]
class Solution {
    public int findSpecialInteger(int[] arr) {
        int m = arr.length / 4;
        for (int i : new int[]{m, m * 2 + 1}) {
            int x = arr[i];
            int j = lowerBound(arr, x);
            if (arr[j + m] == x) {
                return x;
            }
        }
        return arr[m * 3 + 2];
    }

    // lowerBound 返回最小的满足 nums[i] >= target 的下标 i
    // 如果数组为空，或者所有数都 < target，则返回 nums.length
    // 要求 nums 是非递减的，即 nums[i] <= nums[i + 1]
    // 原理见 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(int[] nums, int target) {
        int left = -1;
        int right = nums.length; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = left + (right - left) / 2;
            if (nums[mid] >= target) {
                right = mid; // 范围缩小到 (left, mid)
            } else {
                left = mid; // 范围缩小到 (mid, right)
            }
        }
        // 循环结束后 left+1 = right
        // 此时 nums[left] < target 而 nums[right] >= target
        // 所以 right 就是第一个 >= target 的元素下标
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findSpecialInteger(vector<int>& arr) {
        int m = arr.size() / 4;
        for (int i : {m, m * 2 + 1}) {
            int x = arr[i];
            int j = ranges::lower_bound(arr, x) - arr.begin();
            if (arr[j + m] == x) {
                return x;
            }
        }
        return arr[m * 3 + 2];
    }
};
```

```c [sol-C]
// lowerBound 返回最小的满足 nums[i] >= target 的下标 i
// 如果数组为空，或者所有数都 < target，则返回 numsSize
// 要求 nums 是非递减的，即 nums[i] <= nums[i + 1]
// 原理见 https://www.bilibili.com/video/BV1AP41137w7/
int lowerBound(int* nums, int numsSize, int target) {
    int left = -1, right = numsSize; // 开区间 (left, right)
    while (left + 1 < right) { // 区间不为空
        // 循环不变量：
        // nums[left] < target
        // nums[right] >= target
        int mid = left + (right - left) / 2;
        if (nums[mid] >= target) {
            right = mid; // 范围缩小到 (left, mid)
        } else {
            left = mid; // 范围缩小到 (mid, right)
        }
    }
    // 循环结束后 left+1 = right
    // 此时 nums[left] < target 而 nums[right] >= target
    // 所以 right 就是第一个 >= target 的元素下标
    return right;
}

int findSpecialInteger(int* arr, int arrSize) {
    int m = arrSize / 4;
    for (int i = m; i <= m * 2 + 1; i += m + 1) {
        int x = arr[i];
        int j = lowerBound(arr, arrSize, x);
        if (arr[j + m] == x) {
            return x;
        }
    }
    return arr[m * 3 + 2];
}
```

```go [sol-Go]
func findSpecialInteger(arr []int) int {
    m := len(arr) / 4
    for _, i := range []int{m, m*2 + 1} {
        x := arr[i]
        j := sort.SearchInts(arr, x)
        if arr[j+m] == x {
            return x
        }
    }
    return arr[m*3+2]
}
```

```js [sol-JavaScript]
var findSpecialInteger = function(arr) {
    const m = Math.floor(arr.length / 4);
    for (const i of [m, m * 2 + 1]) {
        const x = arr[i];
        const j = _.sortedIndex(arr, x);
        if (arr[j + m] == x) {
            return x;
        }
    }
    return arr[m * 3 + 2];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_special_integer(arr: Vec<i32>) -> i32 {
        let m = arr.len() / 4;
        for i in [m, m * 2 + 1] {
            let x = arr[i];
            let j = arr.partition_point(|&y| y < x);
            if arr[j + m] == x {
                return x;
            }
        }
        arr[m * 3 + 2]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$，其中 $n$ 是 $\textit{arr}$ 的长度。
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

本题来自 `一、二分查找 / §1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、二分查找 / §1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
