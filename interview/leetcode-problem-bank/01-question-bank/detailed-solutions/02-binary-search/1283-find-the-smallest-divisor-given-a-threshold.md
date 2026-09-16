# 1283. 使结果不超过阈值的最小除数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-smallest-divisor-given-a-threshold/
- 题目 slug：`find-the-smallest-divisor-given-a-threshold`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.1 求最小 / 答疑
- 难度分：1542
- 外部题解来源：https://leetcode.cn/problems/find-the-smallest-divisor-given-a-threshold/solutions/2989469/mo-ban-er-fen-da-an-qiu-zui-xiao-pythonj-ukwe/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【模板】二分答案求最小（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/find-the-smallest-divisor-given-a-threshold/solutions/2989469/mo-ban-er-fen-da-an-qiu-zui-xiao-pythonj-ukwe/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mo-ban-er-fen-da-an-qiu-zui-xiao-pythonj-ukwe`
- topic id：`2989469`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 思路

假设除数为 $m$。

根据题意，每个数除以 $m$ 再上取整，元素和为

$$
\sum_{i=0}^{n-1} \left\lceil\dfrac{\textit{nums}[i]}{m}\right\rceil
$$

由于 $m$ 越大，上式越小，有**单调性**，可以**二分答案**。关于二分的原理，请看视频[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

最小的满足 $\sum\limits_{i=0}^{n-1} \left\lceil\dfrac{\textit{nums}[i]}{m}\right\rceil \le \textit{threshold}$ 的 $m$ 就是答案。

## 细节

### 1)

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的。

- 开区间左端点初始值：$0$。一定不满足题目要求。
- 开区间右端点初始值：$\max(\textit{nums})$。此时 $\sum\limits_{i=0}^{n-1} \left\lceil\dfrac{\textit{nums}[i]}{m}\right\rceil =n\le \textit{threshold}$ 一定成立。（注意题目数据范围保证 $n\le \textit{threshold}$）

如果你喜欢用闭区间，左右端点可以分别初始化成 $1$ 和 $\max(\textit{nums})-1$。

> 注：左右端点可以继续优化，留给读者思考，欢迎在评论区发表你的优化思路。

### 2)

关于上取整的计算，当 $a$ 和 $b$ 均为正整数时，我们有

$$
\left\lceil\dfrac{a}{b}\right\rceil = \left\lfloor\dfrac{a+b-1}{b}\right\rfloor = \left\lfloor\dfrac{a-1}{b}\right\rfloor + 1
$$

证明见 [上取整下取整转换公式的证明](https://zhuanlan.zhihu.com/p/1890356682149838951)。

此时 $\sum\limits_{i=0}^{n-1} \left\lceil\dfrac{\textit{nums}[i]}{m}\right\rceil \le \textit{threshold}$ 变成

$$
\sum\limits_{i=0}^{n-1} 1+ \left\lfloor\dfrac{\textit{nums}[i] - 1}{m}\right\rfloor \le \textit{threshold}
$$

即

$$
\sum\limits_{i=0}^{n-1} \left\lfloor\dfrac{\textit{nums}[i] - 1}{m}\right\rfloor \le \textit{threshold} - n
$$

### 3)

力扣有多台评测机，如果你发现运行时间长，可能是运行在比较慢的那台机子上，可以尝试多提交几次。

```py [sol-Python3]
class Solution:
    def smallestDivisor(self, nums: List[int], threshold: int) -> int:
        left, right = 0, max(nums)
        while left + 1 < right:
            mid = (left + right) // 2
            if sum((x - 1) // mid for x in nums) <= threshold - len(nums):
                right = mid
            else:
                left = mid
        return right
```

```py [sol-Python3 库函数]
class Solution:
    def smallestDivisor(self, nums: List[int], threshold: int) -> int:
        check = lambda m: sum((x - 1) // m for x in nums) <= threshold - len(nums)
        return bisect_left(range(max(nums)), True, 1, key=check)
```

```java [sol-Java]
class Solution {
    public int smallestDivisor(int[] nums, int threshold) {
        int left = 0;
        int right = 0;
        for (int x : nums) {
            right = Math.max(right, x);
        }
        while (left + 1 < right) {
            int mid = (left + right) >>> 1;
            if (check(nums, mid, threshold)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    private boolean check(int[] nums, int m, int threshold) {
        int sum = 0;
        for (int x : nums) {
            sum += (x + m - 1) / m;
            if (sum > threshold) { // 提前退出循环
                return false;
            }
        }
        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int smallestDivisor(vector<int>& nums, int threshold) {
        auto check = [&](int m) -> bool {
            int sum = 0;
            for (int x : nums) {
                sum += (x + m - 1) / m;
                if (sum > threshold) { // 提前退出循环
                    return false;
                }
            }
            return true;
        };

        int left = 0, right = ranges::max(nums);
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int check(int* nums, int numsSize, int m, int threshold) {
    int sum = 0;
    for (int i = 0; i < numsSize; i++) {
        sum += (nums[i] + m - 1) / m;
        if (sum > threshold) { // 提前退出循环
            return 0;
        }
    }
    return 1;
}

int smallestDivisor(int* nums, int numsSize, int threshold) {
    int left = 0, right = nums[0];
    for (int i = 1; i < numsSize; i++) {
        right = MAX(right, nums[i]);
    }
    while (left + 1 < right) {
        int mid = left + (right - left) / 2;
        if (check(nums, numsSize, mid, threshold)) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right;
}
```

```go [sol-Go]
func smallestDivisor(nums []int, threshold int) int {
    check := func(m int) bool {
        sum := 0
        for _, x := range nums {
            sum += (x + m - 1) / m
            if sum > threshold { // 提前退出循环
                return false
            }
        }
        return true
    }

    left, right := 0, slices.Max(nums)
    for left+1 < right {
        mid := left + (right-left)/2
        if check(mid) {
            right = mid
        } else {
            left = mid
        }
    }
    return right
}
```

```go [sol-Go 库函数]
func smallestDivisor(nums []int, threshold int) int {
    ans := 1 + sort.Search(slices.Max(nums)-1, func(m int) bool {
        m++
        sum := 0
        for _, x := range nums {
            sum += (x + m - 1) / m
            if sum > threshold { // 提前退出循环
                return false
            }
        }
        return true
    })
    return ans
}
```

```js [sol-JS]
var smallestDivisor = function(nums, threshold) {
    function check(m) {
        let sum = 0;
        for (const x of nums) {
            sum += Math.ceil(x / m);
            if (sum > threshold) { // 提前退出循环
                return false;
            }
        }
        return true;
    };

    let left = 0, right = Math.max(...nums);
    while (left + 1 < right) {
        const mid = Math.floor((left + right) / 2);
        if (check(mid)) {
            right = mid;
        } else {
            left = mid;
        }
    }
    return right;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn smallest_divisor(nums: Vec<i32>, threshold: i32) -> i32 {
        let check = |m: i32| -> bool {
            let mut sum = 0;
            for &x in &nums {
                sum += (x + m - 1) / m;
                if sum > threshold { // 提前退出循环
                    return false;
                }
            }
            true
        };

        let mut left = 0;
        let mut right = *nums.iter().max().unwrap();
        while left + 1 < right {
            let mid = left + (right - left) / 2;
            if check(mid) {
                right = mid;
            } else {
                left = mid;
            }
        }
        right
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 是 $\textit{nums}$ 的长度，$U=\max(\textit{nums})$。二分 $\mathcal{O}(\log U)$ 次，每次 $\mathcal{O}(n)$ 遍历 $\textit{nums}$。
- 空间复杂度：$\mathcal{O}(1)$。

## 总结

在练习时，请注意「求最小」和「求最大」的二分写法上的区别。

「求最小」和二分查找求「排序数组中某元素的第一个位置」是类似的，按照红蓝染色法，左边是不满足要求的（红色），右边则是满足要求的（蓝色）。

「求最大」的题目则相反，左边是满足要求的（蓝色），右边是不满足要求的（红色）。这会导致二分写法和上面的「求最小」有一些区别。

以开区间二分为例：

- 求最小：`check(mid) == true` 时更新 `right = mid`，反之更新 `left = mid`，最后返回 `right`。
- 求最大：`check(mid) == true` 时更新 `left = mid`，反之更新 `right = mid`，最后返回 `left`。

对于开区间写法，简单来说 `check(mid) == true` 时更新的是谁，最后就返回谁。相比其他二分写法，开区间写法不需要思考加一减一等细节，个人推荐使用开区间写二分。

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

本题来自 `二、二分答案 / §2.1 求最小 / 答疑`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.1 求最小 / 答疑`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
