# 35. 搜索插入位置

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/search-insert-position/
- 题目 slug：`search-insert-position`
- 来源专题：二分算法
- 来源分类路径：一、二分查找 / §1.1 基础
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/search-insert-position/solutions/2023391/er-fen-cha-zhao-zong-shi-xie-bu-dui-yi-g-nq23/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分查找总是写不对？一个视频讲透！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/search-insert-position/solutions/2023391/er-fen-cha-zhao-zong-shi-xie-bu-dui-yi-g-nq23/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-cha-zhao-zong-shi-xie-bu-dui-yi-g-nq23`
- topic id：`2023391`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意概括

返回 $\textit{nums}$ 中的第一个（最左边的）**大于或等于** $\textit{target}$ 的数的下标。如果所有数都小于 $\textit{target}$，返回 $\textit{nums}$ 的长度。

## 视频讲解

请看[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。制作不易，欢迎点赞~

下面的代码包含**闭区间**、**左闭右开区间**和**开区间**三种写法。

选择自己喜欢的一种写法即可。

## 答疑

**问**：为什么不在二分的过程中，找到 $\textit{target}$ 就立刻返回？

**答**：这种写法适用范围很窄。在有多个等于 $\textit{target}$ 的数的情况下，如果在二分的过程中立刻返回，我们得到的可能不是第一个等于 $\textit{target}$ 的数的下标。为什么返回第一个等于 $\textit{target}$ 的数的下标更好呢？因为这可以解决更复杂的题目，比如给你一个有序数组 $[1,1,3,3,3,3,5]$，让你计算有多少个数小于 $3$。我们可以二分查找**第一个**等于 $3$ 的数的下标 $2$，那么下标小于 $2$ 的数就是元素值小于 $3$ 的数，这有 $2$ 个。对比可以发现，如果在二分中途就立刻返回，我们不一定找到的是**第一个**等于 $3$ 的数的下标，所以不一定能算出正确答案 $2$。

**问**：为什么代码没有特判所有数都小于 $\textit{target}$ 的情况？

**答**：如果所有数都小于 $\textit{target}$，那么循环中更新的只有 $\textit{left}$，无论下面哪种二分写法，最后都一定会返回数组长度，所以无需特判这种情况。

**问**：如果所有数都大于 $\textit{target}$ 呢？

**答**：代码会返回 $0$。

**问**：是否需要特判 $\textit{nums}[\textit{mid}]=\textit{target}$ 的情况？

**答**：可以，但没必要。

```py [sol-Python3]
# lower_bound 返回最小的满足 nums[i] >= target 的 i
# 如果数组为空，或者所有数都 < target，则返回 len(nums)
# 要求 nums 是非递减的，即 nums[i] <= nums[i + 1]

# 闭区间写法
def lower_bound(nums: List[int], target: int) -> int:
    left, right = 0, len(nums) - 1  # 闭区间 [left, right]
    while left <= right:  # 区间不为空
        # 循环不变量：
        # nums[left-1] < target
        # nums[right+1] >= target
        mid = (left + right) // 2
        if nums[mid] < target:
            left = mid + 1  # 范围缩小到 [mid+1, right]
        else:
            right = mid - 1  # 范围缩小到 [left, mid-1]
    return left

# 左闭右开区间写法
def lower_bound2(nums: List[int], target: int) -> int:
    left = 0
    right = len(nums)  # 左闭右开区间 [left, right)
    while left < right:  # 区间不为空
        # 循环不变量：
        # nums[left-1] < target
        # nums[right] >= target
        mid = (left + right) // 2
        if nums[mid] < target:
            left = mid + 1  # 范围缩小到 [mid+1, right)
        else:
            right = mid  # 范围缩小到 [left, mid)
    return left  # 或者 right

# 开区间写法
def lower_bound3(nums: List[int], target: int) -> int:
    left, right = -1, len(nums)  # 开区间 (left, right)
    while left + 1 < right:  # 区间不为空
        mid = (left + right) // 2
        # 循环不变量：
        # nums[left] < target
        # nums[right] >= target
        if nums[mid] < target:
            left = mid  # 范围缩小到 (mid, right)
        else:
            right = mid  # 范围缩小到 (left, mid)
    return right

class Solution:
    def searchInsert(self, nums: List[int], target: int) -> int:
        return lower_bound(nums, target)  # 选择其中一种写法即可
```

```java [sol-Java]
class Solution {
    public int searchInsert(int[] nums, int target) {
        return lowerBound(nums, target); // 选择其中一种写法即可
    }

    // lowerBound 返回最小的满足 nums[i] >= target 的 i
    // 如果数组为空，或者所有数都 < target，则返回 nums.length
    // 要求 nums 是非递减的，即 nums[i] <= nums[i + 1]

    // 闭区间写法
    private int lowerBound(int[] nums, int target) {
        int left = 0;
        int right = nums.length - 1; // 闭区间 [left, right]
        while (left <= right) { // 区间不为空
            // 循环不变量：
            // nums[left-1] < target
            // nums[right+1] >= target
            int mid = left + (right - left) / 2;
            if (nums[mid] < target) {
                left = mid + 1; // 范围缩小到 [mid+1, right]
            } else {
                right = mid - 1; // 范围缩小到 [left, mid-1]
            }
        }
        return left;
    }

    // 左闭右开区间写法
    private int lowerBound2(int[] nums, int target) {
        int left = 0;
        int right = nums.length; // 左闭右开区间 [left, right)
        while (left < right) { // 区间不为空
            // 循环不变量：
            // nums[left-1] < target
            // nums[right] >= target
            int mid = left + (right - left) / 2;
            if (nums[mid] < target) {
                left = mid + 1; // 范围缩小到 [mid+1, right)
            } else {
                right = mid; // 范围缩小到 [left, mid)
            }
        }
        return left; // 或者 right
    }

    // 开区间写法
    private int lowerBound3(int[] nums, int target) {
        int left = -1;
        int right = nums.length; // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = left + (right - left) / 2;
            if (nums[mid] < target) {
                left = mid; // 范围缩小到 (mid, right)
            } else {
                right = mid; // 范围缩小到 (left, mid)
            }
        }
        return right;
    }
}
```

```cpp [sol-C++]
class Solution {
    // lower_bound 返回最小的满足 nums[i] >= target 的 i
    // 如果数组为空，或者所有数都 < target，则返回 nums.size()
    // 要求 nums 是非递减的，即 nums[i] <= nums[i + 1]

    // 闭区间写法
    int lower_bound(vector<int>& nums, int target) {
        int left = 0, right = (int) nums.size() - 1; // 闭区间 [left, right]
        while (left <= right) { // 区间不为空
            // 循环不变量：
            // nums[left-1] < target
            // nums[right+1] >= target
            int mid = left + (right - left) / 2;
            if (nums[mid] < target) {
                left = mid + 1; // 范围缩小到 [mid+1, right]
            } else {
                right = mid - 1; // 范围缩小到 [left, mid-1]
            }
        }
        return left;
    }

    // 左闭右开区间写法
    int lower_bound2(vector<int>& nums, int target) {
        int left = 0, right = nums.size(); // 左闭右开区间 [left, right)
        while (left < right) { // 区间不为空
            // 循环不变量：
            // nums[left-1] < target
            // nums[right] >= target
            int mid = left + (right - left) / 2;
            if (nums[mid] < target) {
                left = mid + 1; // 范围缩小到 [mid+1, right)
            } else {
                right = mid; // 范围缩小到 [left, mid)
            }
        }
        return left;
    }

    // 开区间写法
    int lower_bound3(vector<int>& nums, int target) {
        int left = -1, right = nums.size(); // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = left + (right - left) / 2;
            if (nums[mid] < target) {
                left = mid; // 范围缩小到 (mid, right)
            } else {
                right = mid; // 范围缩小到 (left, mid)
            }
        }
        return right;
    }

public:
    int searchInsert(vector<int>& nums, int target) {
        return lower_bound(nums, target); // 选择其中一种写法即可
    }
};
```

```c [sol-C]
// lower_bound 返回最小的满足 nums[i] >= target 的 i
// 如果数组为空，或者所有数都 < target，则返回 nums.size()
// 要求 nums 是非递减的，即 nums[i] <= nums[i + 1]

// 闭区间写法
int lower_bound(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize - 1; // 闭区间 [left, right]
    while (left <= right) { // 区间不为空
        // 循环不变量：
        // nums[left-1] < target
        // nums[right+1] >= target
        int mid = left + (right - left) / 2;
        if (nums[mid] < target) {
            left = mid + 1; // 范围缩小到 [mid+1, right]
        } else {
            right = mid - 1; // 范围缩小到 [left, mid-1]
        }
    }
    return left;
}

// 左闭右开区间写法
int lower_bound2(int* nums, int numsSize, int target) {
    int left = 0, right = numsSize; // 左闭右开区间 [left, right)
    while (left < right) { // 区间不为空
        // 循环不变量：
        // nums[left-1] < target
        // nums[right] >= target
        int mid = left + (right - left) / 2;
        if (nums[mid] < target) {
            left = mid + 1; // 范围缩小到 [mid+1, right)
        } else {
            right = mid; // 范围缩小到 [left, mid)
        }
    }
    return left; // 或者 right
}

// 开区间写法
int lower_bound3(int* nums, int numsSize, int target) {
    int left = -1, right = numsSize; // 开区间 (left, right)
    while (left + 1 < right) { // 区间不为空
        // 循环不变量：
        // nums[left] < target
        // nums[right] >= target
        int mid = left + (right - left) / 2;
        if (nums[mid] < target) {
            left = mid; // 范围缩小到 (mid, right)
        } else {
            right = mid; // 范围缩小到 (left, mid)
        }
    }
    return right;
}

int searchInsert(int* nums, int numsSize, int target) {
    return lower_bound(nums, numsSize, target); // 选择其中一种写法即可
}
```

```go [sol-Go]
// lowerBound 返回最小的满足 nums[i] >= target 的 i
// 如果数组为空，或者所有数都 < target，则返回 nums.length
// 要求 nums 是非递减的，即 nums[i] <= nums[i + 1]

// 闭区间写法
func lowerBound(nums []int, target int) int {
    left, right := 0, len(nums)-1 // 闭区间 [left, right]
    for left <= right {           // 区间不为空
        // 循环不变量：
        // nums[left-1] < target
        // nums[right+1] >= target
        mid := left + (right-left)/2
        if nums[mid] < target {
            left = mid + 1 // 范围缩小到 [mid+1, right]
        } else {
            right = mid - 1 // 范围缩小到 [left, mid-1]
        }
    }
    return left
}

// 左闭右开区间写法
func lowerBound2(nums []int, target int) int {
    left, right := 0, len(nums) // 左闭右开区间 [left, right)
    for left < right {          // 区间不为空
        // 循环不变量：
        // nums[left-1] < target
        // nums[right] >= target
        mid := left + (right-left)/2
        if nums[mid] < target {
            left = mid + 1 // 范围缩小到 [mid+1, right)
        } else {
            right = mid // 范围缩小到 [left, mid)
        }
    }
    return left // 或者 right
}

// 开区间写法
func lowerBound3(nums []int, target int) int {
    left, right := -1, len(nums) // 开区间 (left, right)
    for left+1 < right {         // 区间不为空
        // 循环不变量：
        // nums[left] < target
        // nums[right] >= target
        mid := left + (right-left)/2
        if nums[mid] < target {
            left = mid // 范围缩小到 (mid, right)
        } else {
            right = mid // 范围缩小到 (left, mid)
        }
    }
    return right
}

func searchInsert(nums []int, target int) int {
    return lowerBound(nums, target) // 选择其中一种写法即可
}
```

```js [sol-JavaScript]
// lowerBound 返回最小的满足 nums[i] >= target 的 i
// 如果数组为空，或者所有数都 < target，则返回 nums.length
// 要求 nums 是非递减的，即 nums[i] <= nums[i + 1]

// 闭区间写法
var lowerBound = function(nums, target) {
    let left = 0, right = nums.length - 1; // 闭区间 [left, right]
    while (left <= right) { // 区间不为空
        // 循环不变量：
        // nums[left-1] < target
        // nums[right+1] >= target
        const mid = Math.floor((left + right) / 2);
        if (nums[mid] < target) {
            left = mid + 1; // 范围缩小到 [mid+1, right]
        } else {
            right = mid - 1; // 范围缩小到 [left, mid-1]
        }
    }
    return left;
}

// 左闭右开区间写法
var lowerBound2 = function(nums, target) {
    let left = 0, right = nums.length; // 左闭右开区间 [left, right)
    while (left < right) { // 区间不为空
        // 循环不变量：
        // nums[left-1] < target
        // nums[right] >= target
        const mid = Math.floor((left + right) / 2);
        if (nums[mid] < target) {
            left = mid + 1; // 范围缩小到 [mid+1, right)
        } else {
            right = mid; // 范围缩小到 [left, mid)
        }
    }
    return left; // 或者 right
}

// 开区间写法
var lowerBound3 = function(nums, target) {
    let left = -1, right = nums.length; // 开区间 (left, right)
    while (left + 1 < right) { // 区间不为空
        // 循环不变量：
        // nums[left] < target
        // nums[right] >= target
        const mid = Math.floor((left + right) / 2);
        if (nums[mid] < target) {
            left = mid; // 范围缩小到 (mid, right)
        } else {
            right = mid; // 范围缩小到 (left, mid)
        }
    }
    return right;
}

var searchInsert = function(nums, target) {
    return lowerBound(nums, target) // 选择其中一种写法即可
};
```

```rust [sol-Rust]
// 注：为避免出现负数，使用左闭右开区间是最方便的
impl Solution {
    // lowerBound 返回最小的满足 nums[i] >= target 的 i
    // 如果数组为空，或者所有数都 < target，则返回 nums.length
    // 要求 nums 是非递减的，即 nums[i] <= nums[i + 1]

    // 左闭右开区间写法
    fn lower_bound(nums: &[i32], target: i32) -> usize {
        let mut left = 0;
        let mut right = nums.len(); // 左闭右开区间 [left, right)
        while left < right { // 区间不为空
            // 循环不变量：
            // nums[left-1] < target
            // nums[right] >= target
            let mid = left + (right - left) / 2;
            if nums[mid] < target {
                left = mid + 1; // 范围缩小到 [mid+1, right)
            } else {
                right = mid; // 范围缩小到 [left, mid)
            }
        }
        left // 或者 right
    }

    pub fn search_insert(nums: Vec<i32>, target: i32) -> i32 {
        Self::lower_bound(&nums, target) as _
    }
}
```

## 库函数写法

```py [sol-Python3]
class Solution:
    def searchInsert(self, nums: List[int], target: int) -> int:
        return bisect_left(nums, target)
```

```java [sol-Java]
// 注意：只能在没有重复元素的时候使用
// 如果 nums 有多个值为 target 的数，返回值不一定是第一个 >= target 的数的下标
class Solution {
    public int searchInsert(int[] nums, int target) {
        int i = Arrays.binarySearch(nums, target);
        return i >= 0 ? i : ~i; // ~i = -i-1
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int searchInsert(vector<int>& nums, int target) {
        return ranges::lower_bound(nums, target) - nums.begin();
    }
};
```

```go [sol-Go]
func searchInsert(nums []int, target int) int {
    return sort.SearchInts(nums, target)
}
```

```js [sol-JavaScript]
var searchInsert = function(nums, target) {
    return _.sortedIndex(nums, target);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn search_insert(nums: Vec<i32>, target: i32) -> i32 {
        nums.partition_point(|&x| x < target) as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$，仅用到若干额外变量。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、二分查找 / §1.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、二分查找 / §1.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
