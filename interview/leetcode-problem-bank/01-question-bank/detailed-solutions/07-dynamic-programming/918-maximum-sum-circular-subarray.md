# 918. 环形子数组的最大和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-sum-circular-subarray/
- 题目 slug：`maximum-sum-circular-subarray`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.3 最大子数组和（最大子段和）
- 难度分：1777
- 外部题解来源：https://leetcode.cn/problems/maximum-sum-circular-subarray/solutions/2351107/mei-you-si-lu-yi-zhang-tu-miao-dong-pyth-ilqh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】正难则反，一张图秒懂！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-sum-circular-subarray/solutions/2351107/mei-you-si-lu-yi-zhang-tu-miao-dong-pyth-ilqh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-you-si-lu-yi-zhang-tu-miao-dong-pyth-ilqh`
- topic id：`2351107`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

![lc918-2c.png](https://pic.leetcode.cn/1761701105-HVjEQZ-lc918-2c.png)

如何计算 [53. 最大子数组和](https://leetcode.cn/problems/maximum-subarray/)，请看 [我的题解](https://leetcode.cn/problems/maximum-subarray/solutions/2533977/qian-zhui-he-zuo-fa-ben-zhi-shi-mai-mai-abu71/)，下面代码修改自我题解中的空间优化版本。

```py [sol-Python3]
class Solution:
    def maxSubarraySumCircular(self, nums: List[int]) -> int:
        max_f = 0     # 计算最大子数组和的 DP 数组（空间优化成一个变量）
        max_s = -inf  # 最大子数组和，不能为空
        min_f = 0     # 计算最小子数组和的 DP 数组（空间优化成一个变量）
        min_s = 0     # 最小子数组和，可以为空（元素和为 0）

        for x in nums:
            # 53. 最大子数组和（空间优化写法）
            max_f = max(max_f, 0) + x
            max_s = max(max_s, max_f)
            min_f = min(min_f, 0) + x
            min_s = min(min_s, min_f)

        if max_s < 0:
            return max_s
        return max(max_s, sum(nums) - min_s)
```

```java [sol-Java]
class Solution {
    public int maxSubarraySumCircular(int[] nums) {
        int maxF = 0; // 计算最大子数组和的 DP 数组（空间优化成一个变量）
        int maxS = Integer.MIN_VALUE; // 最大子数组和，不能为空
        int minF = 0; // 计算最小子数组和的 DP 数组（空间优化成一个变量）
        int minS = 0; // 最小子数组和，可以为空（元素和为 0）
        int sum = 0; // nums 的元素和

        for (int x : nums) {
            // 53. 最大子数组和（空间优化写法）
            maxF = Math.max(maxF, 0) + x;
            maxS = Math.max(maxS, maxF);
            minF = Math.min(minF, 0) + x;
            minS = Math.min(minS, minF);
            sum += x;
        }

        return maxS < 0 ? maxS : Math.max(maxS, sum - minS);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSubarraySumCircular(vector<int>& nums) {
        int max_f = 0; // 计算最大子数组和的 DP 数组（空间优化成一个变量）
        int max_s = INT_MIN; // 最大子数组和，不能为空
        int min_f = 0; // 计算最小子数组和的 DP 数组（空间优化成一个变量）
        int min_s = 0; // 最小子数组和，可以为空（元素和为 0）
        int sum = 0; // nums 的元素和

        for (int x : nums) {
            // 53. 最大子数组和（空间优化写法）
            max_f = max(max_f, 0) + x;
            max_s = max(max_s, max_f);
            min_f = min(min_f, 0) + x;
            min_s = min(min_s, min_f);
            sum += x;
        }

        return max_s < 0 ? max_s : max(max_s, sum - min_s);
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxSubarraySumCircular(int* nums, int numsSize) {
    int max_f = 0; // 计算最大子数组和的 DP 数组（空间优化成一个变量）
    int max_s = INT_MIN; // 最大子数组和，不能为空
    int min_f = 0; // 计算最小子数组和的 DP 数组（空间优化成一个变量）
    int min_s = 0; // 最小子数组和，可以为空（元素和为 0）
    int sum = 0; // nums 的元素和

    for (int i = 0; i < numsSize; i++) {
        int x = nums[i];
        // 53. 最大子数组和（空间优化写法）
        max_f = MAX(max_f, 0) + x;
        max_s = MAX(max_s, max_f);
        min_f = MIN(min_f, 0) + x;
        min_s = MIN(min_s, min_f);
        sum += x;
    }

    return max_s < 0 ? max_s : MAX(max_s, sum - min_s);
}
```

```go [sol-Go]
func maxSubarraySumCircular(nums []int) int {
    maxF := 0           // 计算最大子数组和的 DP 数组（空间优化成一个变量）
    maxS := math.MinInt // 最大子数组和，不能为空
    minF := 0           // 计算最小子数组和的 DP 数组（空间优化成一个变量）
    minS := 0           // 最小子数组和，可以为空（元素和为 0）
    sum := 0            // nums 的元素和

    for _, x := range nums {
        // 53. 最大子数组和（空间优化写法）
        maxF = max(maxF, 0) + x
        maxS = max(maxS, maxF)
        minF = min(minF, 0) + x
        minS = min(minS, minF)
        sum += x
    }

    if maxS < 0 {
        return maxS
    }
    return max(maxS, sum-minS)
}
```

```js [sol-JavaScript]
var maxSubarraySumCircular = function(nums) {
    let maxF = 0; // 计算最大子数组和的 DP 数组（空间优化成一个变量）
    let maxS = -Infinity; // 最大子数组和，不能为空
    let minF = 0; // 计算最小子数组和的 DP 数组（空间优化成一个变量）
    let minS = 0; // 最小子数组和，可以为空（元素和为 0）
    let sum = 0; // nums 的元素和

    for (const x of nums) {
        // 53. 最大子数组和（空间优化写法）
        maxF = Math.max(maxF, 0) + x;
        maxS = Math.max(maxS, maxF);
        minF = Math.min(minF, 0) + x;
        minS = Math.min(minS, minF);
        sum += x;
    }

    return maxS < 0 ? maxS : Math.max(maxS, sum - minS);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_subarray_sum_circular(nums: Vec<i32>) -> i32 {
        let mut max_f = 0; // 计算最大子数组和的 DP 数组（空间优化成一个变量）
        let mut max_s = i32::MIN; // 最大子数组和，不能为空
        let mut min_f = 0; // 计算最小子数组和的 DP 数组（空间优化成一个变量）
        let mut min_s = 0; // 最小子数组和，可以为空（元素和为 0）
        let mut sum = 0; // nums 的元素和

        for x in nums {
            // 53. 最大子数组和（空间优化写法）
            max_f = max_f.max(0) + x;
            max_s = max_s.max(max_f);
            min_f = min_f.min(0) + x;
            min_s = min_s.min(min_f);
            sum += x;
        }

        if max_s < 0 {
            max_s
        } else {
            max_s.max(sum - min_s)
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 变形题

如果改成选**两个**不重叠的非空连续子数组，这两个子数组的元素和最大是多少？

- 非环形数组：[P2642 双子序列最大和](https://www.luogu.com.cn/problem/P2642)
- 环形数组：[P1121 环状最大两段子段和](https://www.luogu.com.cn/problem/P1121)

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
