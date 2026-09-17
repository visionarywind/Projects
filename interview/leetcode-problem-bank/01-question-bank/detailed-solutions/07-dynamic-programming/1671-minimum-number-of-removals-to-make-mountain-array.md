# 1671. 得到山形数组的最少删除次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-removals-to-make-mountain-array/
- 题目 slug：`minimum-number-of-removals-to-make-mountain-array`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶
- 难度分：1913
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-removals-to-make-mountain-array/solutions/2575527/qian-hou-zhui-fen-jie-zui-chang-di-zeng-9vowl/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前后缀分解+最长递增子序列，附题单（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-number-of-removals-to-make-mountain-array/solutions/2575527/qian-hou-zhui-fen-jie-zui-chang-di-zeng-9vowl/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-hou-zhui-fen-jie-zui-chang-di-zeng-9vowl`
- topic id：`2575527`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 前置知识

关于最长递增子序列的 $\mathcal{O}(n^2)$ 和 $\mathcal{O}(n\log n)$ 做法，请看[【基础算法精讲 20】](https://www.bilibili.com/video/BV1ub411Q7sB/)

## 思路

要使删除次数最少，山形子序列的长度越长越好。最长是多少呢？不妨枚举 $\textit{nums}[i]$，把它当作峰顶，计算此时山形子序列的最长长度。

山形子序列可以看成一个严格递增子序列，拼接一个严格递减子序列：

- 定义 $\textit{pre}[i]$ 表示子序列最后一个数是 $\textit{nums}[i]$ 的最长严格递增子序列的长度。
- 定义 $\textit{suf}[i]$ 表示子序列第一个数是 $\textit{nums}[i]$ 的最长严格递减子序列的长度。

注意本题要求峰顶左右两侧必须有数字，所以在 $\textit{pre}[i] \ge 2$ 且 $\textit{suf}[i] \ge 2$ 的情况下，可以把这两部分拼起来，再去掉中间重复的一个 $\textit{nums}[i]$，得到以 $\textit{nums}[i]$ 为峰顶的最长山形子序列的长度：

$$
\textit{pre}[i] + \textit{suf}[i] - 1
$$

枚举 $i$，取上式取最大值，即为答案。

如何计算 $\textit{pre}$ 和 $\textit{suf}$ 呢？以 $\textit{suf}$ 为例，从右往左遍历 $\textit{nums}$，就相当于是在求最长严格**递增**子序列，可以使用 $\mathcal{O}(n\log n)$ 的做法解决。当我们遍历到 $\textit{nums}[i]$ 时，二分下标加一就是此时 $\textit{suf}[i]$ 的值。

代码实现时，计算 $\textit{pre}$ 的过程可以和计算答案最大值的过程合并，这样只需要用一个变量表示 $\textit{pre}$。

最后用数组长度减去山形子序列的最长长度，即为答案。

```py [sol-Python3]
class Solution:
    def minimumMountainRemovals(self, nums: List[int]) -> int:
        n = len(nums)
        suf = [0] * n
        g = []
        for i in range(n - 1, 0, -1):
            x = nums[i]
            j = bisect_left(g, x)
            if j == len(g):
                g.append(x)
            else:
                g[j] = x
            suf[i] = j + 1  # 从 nums[i] 开始的最长严格递减子序列的长度

        mx = 0  # 最长山形子序列的长度
        g = []
        for i, x in enumerate(nums):
            j = bisect_left(g, x)
            if j == len(g):
                g.append(x)
            else:
                g[j] = x
            pre = j + 1  # 在 nums[i] 结束的最长严格递增子序列的长度
            if pre >= 2 and suf[i] >= 2:
                mx = max(mx, pre + suf[i] - 1)  # 减去重复的 nums[i]
        return n - mx
```

```py [sol-Python3 更简洁写法]
class Solution:
    def minimumMountainRemovals(self, nums: List[int]) -> int:
        n = len(nums)
        def lis(nums: List[int]) -> List[int]:
            res = [0] * n
            g = []
            for i, x in enumerate(nums):
                j = bisect_left(g, x)
                if j == len(g):
                    g.append(x)
                else:
                    g[j] = x
                res[i] = j + 1
            return res
        pre = lis(nums)
        suf = lis(nums[::-1])[::-1]
        return n - max(p + s for p, s in zip(pre, suf) if p >= 2 and s >= 2) + 1  # -1 提到外面
```

```java [sol-Java]
class Solution {
    public int minimumMountainRemovals(int[] nums) {
        int n = nums.length;
        int[] suf = new int[n];
        List<Integer> g = new ArrayList<>();
        for (int i = n - 1; i > 0; i--) {
            int x = nums[i];
            int j = lowerBound(g, x);
            if (j == g.size()) {
                g.add(x);
            } else {
                g.set(j, x);
            }
            suf[i] = j + 1; // 从 nums[i] 开始的最长严格递减子序列的长度
        }

        int mx = 0;
        g.clear();
        for (int i = 0; i < n - 1; i++) {
            int x = nums[i];
            int j = lowerBound(g, x);
            if (j == g.size()) {
                g.add(x);
            } else {
                g.set(j, x);
            }
            int pre = j + 1; // 在 nums[i] 结束的最长严格递增子序列的长度
            if (pre >= 2 && suf[i] >= 2) {
                mx = Math.max(mx, pre + suf[i] - 1); // 减去重复的 nums[i]
            }
        }
        return n - mx;
    }

    // 请看 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(List<Integer> g, int target) {
        int left = -1, right = g.size(); // 开区间 (left, right)
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // nums[left] < target
            // nums[right] >= target
            int mid = (left + right) >>> 1;
            if (g.get(mid) < target) {
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
    int minimumMountainRemovals(vector<int> &nums) {
        int n = nums.size();
        vector<int> suf(n), g;
        for (int i = n - 1; i; i--) {
            int x = nums[i];
            auto it = lower_bound(g.begin(), g.end(), x);
            suf[i] = it - g.begin() + 1; // 从 nums[i] 开始的最长严格递减子序列的长度
            if (it == g.end()) {
                g.push_back(x);
            } else {
                *it = x;
            }
        }

        int mx = 0;
        g.clear();
        for (int i = 0; i < n - 1; i++) {
            int x = nums[i];
            auto it = lower_bound(g.begin(), g.end(), x);
            int pre = it - g.begin() + 1; // 在 nums[i] 结束的最长严格递增子序列的长度
            if (it == g.end()) {
                g.push_back(x);
            } else {
                *it = x;
            }
            if (pre >= 2 && suf[i] >= 2) {
                mx = max(mx, pre + suf[i] - 1); // 减去重复的 nums[i]
            }
        }
        return n - mx;
    }
};
```

```go [sol-Go]
func minimumMountainRemovals(nums []int) int {
    n := len(nums)
    suf := make([]int, n)
    g := []int{}
    for i := n - 1; i > 0; i-- {
        x := nums[i]
        j := sort.SearchInts(g, x)
        if j == len(g) {
            g = append(g, x)
        } else {
            g[j] = x
        }
        suf[i] = j + 1 // 从 nums[i] 开始的最长严格递减子序列的长度
    }

    mx := 0
    g = g[:0]
    for i, x := range nums {
        j := sort.SearchInts(g, x)
        if j == len(g) {
            g = append(g, x)
        } else {
            g[j] = x
        }
        pre := j + 1 // 在 nums[i] 结束的最长严格递增子序列的长度
        if pre >= 2 && suf[i] >= 2 {
            mx = max(mx, pre+suf[i]-1) // 减去重复的 nums[i]
        }
    }
    return n - mx
}
```

```js [sol-JavaScript]
var minimumMountainRemovals = function (nums) {
    const n = nums.length;
    const suf = Array(n).fill(0);
    const g = [];
    for (let i = n - 1; i > 0; i--) {
        const x = nums[i];
        const j = lowerBound(g, x);
        if (j === g.length) {
            g.push(x);
        } else {
            g[j] = x;
        }
        suf[i] = j + 1; // 从 nums[i] 开始的最长严格递减子序列的长度
    }

    let mx = 0;
    g.length = 0;
    for (let i = 0; i < n - 1; i++) {
        const x = nums[i];
        const j = lowerBound(g, x);
        if (j === g.length) {
            g.push(x);
        } else {
            g[j] = x;
        }
        const pre = j + 1; // 在 nums[i] 结束的最长严格递增子序列的长度
        if (pre >= 2 && suf[i] >= 2) {
            mx = Math.max(mx, pre + suf[i] - 1); // 减去重复的 nums[i]
        }
    }
    return n - mx;
};

// 请看 https://www.bilibili.com/video/BV1AP41137w7/
var lowerBound = function (nums, target) {
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
    return right; // 或者 left+1
}
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_mountain_removals(nums: Vec<i32>) -> i32 {
        let n = nums.len();
        let mut suf = vec![0; n];
        let mut g = Vec::new();
        for (i, &x) in nums.iter().enumerate().rev() {
            let j = g.partition_point(|&v| v < x);
            if j == g.len() {
                g.push(x);
            } else {
                g[j] = x;
            }
            suf[i] = j + 1; // 从 nums[i] 开始的最长严格递减子序列的长度
        }

        let mut mx = 0;
        g.clear();
        for (i, &x) in nums.iter().enumerate() {
            let j = g.partition_point(|&v| v < x);
            if j == g.len() {
                g.push(x);
            } else {
                g[j] = x;
            }
            let pre = j + 1; // 在 nums[i] 结束的最长严格递增子序列的长度
            if pre >= 2 && suf[i] >= 2 {
                mx = mx.max(pre + suf[i] - 1); // 减去重复的 nums[i]
            }
        }
        (n - mx) as i32
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 变形题

1. 严格递增改成非递减。
2. 添加约束：相邻数字之差不能超过 $k$。
3. 输出具体方案。

提示：结合前后缀分解与 [2407. 最长递增子序列 II](https://leetcode.cn/problems/longest-increasing-subsequence-ii/)

## 题单：前后缀分解（右边数字为难度分）

- [42. 接雨水](https://leetcode.cn/problems/trapping-rain-water/)
- [2256. 最小平均差](https://leetcode.cn/problems/minimum-average-difference/) 1395
- [2909. 元素和最小的山形三元组 II](https://leetcode.cn/problems/minimum-sum-of-mountain-triplets-ii/) 1479
- [2483. 商店的最少代价](https://leetcode.cn/problems/minimum-penalty-for-a-shop/) 1495
- [2874. 有序三元组中的最大值 II](https://leetcode.cn/problems/maximum-value-of-an-ordered-triplet-ii/) 1583
- [2420. 找到所有好下标](https://leetcode.cn/problems/find-all-good-indices/) 1695
- [238. 除自身以外数组的乘积](https://leetcode.cn/problems/product-of-array-except-self/)
- [2906. 构造乘积矩阵](https://leetcode.cn/problems/construct-product-matrix/) 2075
- [2167. 移除所有载有违禁货物车厢所需的最少时间](https://leetcode.cn/problems/minimum-time-to-remove-all-cars-containing-illegal-goods/) 2219
- [2484. 统计回文子序列数目](https://leetcode.cn/problems/count-palindromic-subsequences/) 2223
- [2565. 最少得分子序列](https://leetcode.cn/problems/subsequence-with-the-minimum-score/) 2432
- [2552. 统计上升四元组](https://leetcode.cn/problems/count-increasing-quadruplets/) 2433

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

更多精彩题解，请看 [往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
