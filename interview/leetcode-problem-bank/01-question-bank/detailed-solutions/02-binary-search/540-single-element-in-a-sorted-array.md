# 540. 有序数组中的单一元素

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/single-element-in-a-sorted-array/
- 题目 slug：`single-element-in-a-sorted-array`
- 来源专题：二分算法
- 来源分类路径：四、其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/single-element-in-a-sorted-array/solutions/2983333/er-fen-xing-zhi-fen-xi-jian-ji-xie-fa-py-0rng/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分，性质分析+简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/single-element-in-a-sorted-array/solutions/2983333/er-fen-xing-zhi-fen-xi-jian-ji-xie-fa-py-0rng/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-xing-zhi-fen-xi-jian-ji-xie-fa-py-0rng`
- topic id：`2983333`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 分析

题目有两个已知条件：

1. 数组是有序的。
2. 除了一个数出现一次外，其余每个数都出现两次。

第二个条件意味着，数组的长度一定是**奇数**。

第一个条件意味着，出现两次的数，必然**相邻**，不可能出现 $1,2,1$ 这样的顺序。

这也意味着，**只出现一次的那个数，一定位于偶数下标上**。

这启发我们去检查偶数下标 $2k$。

示例 1 的 $\textit{nums} = [1,1,2,3,3,4,4,8,8]$：

- 如果 $\textit{nums}[2k] = \textit{nums}[2k+1]$，说明只出现一次的数的下标 $>2k$。
- 如果 $\textit{nums}[2k] \ne \textit{nums}[2k+1]$，说明只出现一次的数的下标 $\le 2k$。

也就是说，随着 $k$ 的变大，不等式 $\textit{nums}[2k] \ne \textit{nums}[2k+1]$ 越可能满足，有单调性，可以二分。

## 二分

关于二分的原理，请看视频[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

讨论二分的上下界。**本文用开区间二分，其他二分写法也是可以的。**

二分 $k$，其中 $k$ 的范围是 $0\le k \le \left\lfloor\dfrac{n}{2}\right\rfloor - 1$。

⚠**注意**：$k=\left\lfloor\dfrac{n}{2}\right\rfloor$，也就是 $2k=n-1$ 的位置是不需要检查的，如果我们在 $<n-1$ 的位置都没有找到只出现一次的数，那么只出现一次的数必然位于 $n-1$。

所以开区间二分的左右边界分别为 $-1$ 和 $\left\lfloor\dfrac{n}{2}\right\rfloor$。

如果你要用闭区间二分，左右边界分别为 $0$ 和 $\left\lfloor\dfrac{n}{2}\right\rfloor - 1$。

```py [sol-Python3]
class Solution:
    def singleNonDuplicate(self, nums: List[int]) -> int:
        left, right = -1, len(nums) // 2
        while left + 1 < right:
            mid = (left + right) // 2
            if nums[mid * 2] != nums[mid * 2 + 1]:
                right = mid
            else:
                left = mid
        return nums[right * 2]
```

```py [sol-Python3 库函数]
class Solution:
    def singleNonDuplicate(self, nums: List[int]) -> int:
        check = lambda k: nums[k * 2] != nums[k * 2 + 1]
        k = bisect_left(range(len(nums) // 2), True, key=check)
        return nums[k * 2]
```

```java [sol-Java]
class Solution {
    public int singleNonDuplicate(int[] nums) {
        int left = -1;
        int right = nums.length / 2;
        while (left + 1 < right) {
            int mid = (left + right) >>> 1;
            if (nums[mid * 2] != nums[mid * 2 + 1]) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return nums[right * 2];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int singleNonDuplicate(vector<int>& nums) {
        int left = -1, right = nums.size() / 2;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (nums[mid * 2] != nums[mid * 2 + 1] ? right : left) = mid;
        }
        return nums[right * 2];
    }
};
```

```c [sol-C]
int singleNonDuplicate(int* nums, int numsSize) {
    int left = -1, right = numsSize / 2;
    while (left + 1 < right) {
        int mid = left + (right - left) / 2;
        if (nums[mid * 2] != nums[mid * 2 + 1]) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return nums[right * 2];
}
```

```go [sol-Go]
func singleNonDuplicate(nums []int) int {
    left, right := -1, len(nums)/2
    for left+1 < right {
        mid := left + (right-left)/2
        if nums[mid*2] != nums[mid*2+1] {
            right = mid
        } else {
            left = mid
        }
    }
    return nums[right*2]
}
```

```go [sol-Go 库函数]
func singleNonDuplicate(nums []int) int {
    k := sort.Search(len(nums)/2, func(k int) bool {
        return nums[k*2] != nums[k*2+1]
    })
    return nums[k*2]
}
```

```js [sol-JS]
var singleNonDuplicate = function(nums) {
    let left = -1, right = Math.floor(nums.length / 2);
    while (left + 1 < right) {
        const mid = Math.floor((left + right) / 2);
        if (nums[mid * 2] !== nums[mid * 2 + 1]) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return nums[right * 2];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn single_non_duplicate(nums: Vec<i32>) -> i32 {
        // 左闭右开区间
        let mut left = 0;
        let mut right = nums.len() / 2;
        while left < right {
            let mid = (left + right) / 2;
            if nums[mid * 2] != nums[mid * 2 + 1] {
                right = mid;
            } else {
                left = mid + 1;
            }
        }
        nums[right * 2]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

想一想，改成除了一个数出现一次外，其余每个数都出现 $3$ 次呢？$m$ 次呢？

本题还有一种 `nums[mid] != nums[mid ^ 1]` 的做法，这种做法的时间复杂度为 $\mathcal{O}(\log n)$。我的做法不仅适用于 $m=2$ 的情况，还适用于 $m>2$ 的情况，且时间复杂度为 $\mathcal{O}(\log(n/m))$，效率更高。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. 【本题相关】[二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
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
