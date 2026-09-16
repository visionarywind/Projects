# 153. 寻找旋转排序数组中的最小值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-minimum-in-rotated-sorted-array/
- 题目 slug：`find-minimum-in-rotated-sorted-array`
- 来源专题：二分算法
- 来源分类路径：四、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/find-minimum-in-rotated-sorted-array/solutions/1987499/by-endlesscheng-owgd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[和最后一个数比大小，简洁二分（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/find-minimum-in-rotated-sorted-array/solutions/1987499/by-endlesscheng-owgd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-owgd`
- topic id：`1987499`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

设 $x=\textit{nums}[\textit{mid}]$ 是现在二分取到的数。

我们需要判断 $x$ 和数组最小值的位置关系，谁在左边，谁在右边？

把 $x$ 与最后一个数 $\textit{nums}[n-1]$ 比大小：

- 如果 $x > \textit{nums}[n-1]$，那么可以推出以下结论：
    - $\textit{nums}$ 一定被分成左右两个递增段；
    - 第一段的所有元素均大于第二段的所有元素；
    - $x$ 在第一段。
    - 最小值在第二段。
    - 所以 $x$ 一定在最小值的**左边**。
- 如果 $x \le \textit{nums}[n-1]$，那么 $x$ 一定在第二段。（或者 $\textit{nums}$ 就是递增数组，此时只有一段。）
    - $x$ 要么是最小值，要么在最小值**右边**。

所以，只需要比较 $x$ 和 $\textit{nums}[n-1]$ 的大小关系，就**间接地**知道了 $x$ 和数组最小值的位置关系，从而不断地缩小数组最小值所在位置的范围，二分找到数组最小值。

**二分基础知识**：[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

**本题视频讲解**：[【基础算法精讲 05】](https://www.bilibili.com/video/BV1QK411d76w/)。

### 细节

下面代码用的开区间二分，用其他二分写法也是可以的。

二分的范围可以是 $(-1,n-1)$，也就是闭区间 $[0,n-2]$。

这是因为，如果 $\textit{nums}[n-1]$ 是数组最小值，那么 $\textit{nums}$ 分成两段，第一段 $[0,n-2]$，第二段 $[n-1,n-1]$，且第一段的所有数都大于 $\textit{nums}[n-1]$。每次 $x$ 和 $\textit{nums}[n-1]$ 比大小，一定是 $x>\textit{nums}[n-1]$。这意味着每次二分更新的都是 $\textit{left}$，那么循环结束后，答案自然就是 $n-1$ 了。

**注**：这里有两个概念「二分范围」和「答案范围」。答案确实可以等于 $n-1$，但对于二分来说，代码中的 `if (nums[mid] < nums[n - 1])` 在 $\textit{mid}=n-1$ 的时候一定不成立，我们可以直接知道 $n-1$ 是蓝色（根据视频中的红蓝染色法），所以 $n-1$ 无需在二分区间中。

### 答疑

**问**：能否与 $\textit{nums}[0]$ 比大小？

**答**：可以，但要多写一点代码。假设 $\textit{nums}$ 有两段。如果 $\textit{nums}[\textit{mid}] > \textit{nums}[0]$，那么 $\textit{mid}$ 在第一段，在最小值左边；否则，$\textit{mid}$ 要么是最小值，要么在最小值右边。这种写法需要在 $(0,n)$ 中二分，如果二分结果等于 $n$，说明 $\textit{nums}$ 其实只有一段，答案是 $\textit{nums}[0]$。

```py [sol-Python3]
class Solution:
    def findMin(self, nums: List[int]) -> int:
        left, right = -1, len(nums) - 1  # 开区间 (-1, n-1)
        while left + 1 < right:  # 开区间不为空
            mid = (left + right) // 2
            if nums[mid] < nums[-1]:
                right = mid
            else:
                left = mid
        return nums[right]
```

```py [sol-Python3 库函数]
class Solution:
    def findMin(self, nums: List[int]) -> int:
        check = lambda i: nums[i] < nums[-1]
        i = bisect_left(range(len(nums) - 1), True, key=check)
        return nums[i]
```

```java [sol-Java]
class Solution {
    public int findMin(int[] nums) {
        int n = nums.length;
        int left = -1;
        int right = n - 1; // 开区间 (-1, n-1)
        while (left + 1 < right) { // 开区间不为空
            int mid = (left + right) >>> 1;
            if (nums[mid] < nums[n - 1]) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return nums[right];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findMin(vector<int>& nums) {
        int left = -1, right = nums.size() - 1; // 开区间 (-1, n-1)
        while (left + 1 < right) { // 开区间不为空
            int mid = left + (right - left) / 2;
            (nums[mid] < nums.back() ? right : left) = mid;
        }
        return nums[right];
    }
};
```

```c [sol-C]
int findMin(int* nums, int numsSize) {
    int left = -1, right = numsSize - 1; // 开区间 (-1, n-1)
    while (left + 1 < right) { // 开区间不为空
        int mid = left + (right - left) / 2;
        if (nums[mid] < nums[numsSize - 1]) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return nums[right];
}
```

```go [sol-Go]
func findMin(nums []int) int {
    left, right := -1, len(nums)-1 // 开区间 (-1, n-1)
    for left+1 < right { // 开区间不为空
        mid := left + (right-left)/2
        if nums[mid] < nums[len(nums)-1] {
            right = mid
        } else {
            left = mid
        }
    }
    return nums[right]
}
```

```go [sol-Go 库函数]
func findMin(nums []int) int {
    i := sort.Search(len(nums)-1, func(i int) bool {
        return nums[i] < nums[len(nums)-1]
    })
    return nums[i]
}
```

```js [sol-JS]
var findMin = function(nums) {
    let left = -1, right = nums.length - 1; // 开区间 (-1, n-1)
    while (left + 1 < right) { // 开区间不为空
        let mid = Math.floor((left + right) / 2);
        if (nums[mid] < nums[nums.length - 1]) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return nums[right];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn find_min(nums: Vec<i32>) -> i32 {
        let mut left = 0;
        let mut right = nums.len() - 1; // 左闭右开区间 [0, n-1)
        while left < right { // 区间不为空
            let mid = left + (right - left) / 2;
            if nums[mid] < nums[nums.len() - 1] {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        nums[left]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。
  
## 思考题

改成计算 $\textit{nums}$ 的最大值呢？

请读者实现该问题，以加深对本题的理解。

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
