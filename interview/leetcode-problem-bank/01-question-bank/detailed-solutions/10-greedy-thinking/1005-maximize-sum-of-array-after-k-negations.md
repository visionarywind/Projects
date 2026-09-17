# 1005. K 次取反后最大化的数组和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximize-sum-of-array-after-k-negations/
- 题目 slug：`maximize-sum-of-array-after-k-negations`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：1275
- 外部题解来源：https://leetcode.cn/problems/maximize-sum-of-array-after-k-negations/solutions/3752775/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-uj5k/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[简单题，简单做（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximize-sum-of-array-after-k-negations/solutions/3752775/jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-uj5k/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jian-dan-ti-jian-dan-zuo-pythonjavaccgoj-uj5k`
- topic id：`3752775`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

贪心地，优先改更小的负数（绝对值更大的负数），这样可以让元素和更大。

先把数组从小到大排序。分类讨论：

- 如果 $k$ 小于等于负数个数，那么把前 $k$ 个元素取反。
- 如果 $k$ 大于负数个数，那么先把所有负数都取反。然后判断剩余 $k$ 的奇偶性：
  - 如果剩余 $k$ 为偶数，那么随便找一个数取反偶数次。由于一个数取反两次不变，所以这不影响元素和。
  - 如果剩余 $k$ 为奇数，那么必须有一个数要强行取反，那就改当前最小的数。

代码实现时，可以在改完负数后判断：如果此时 $k$ 是奇数，说明我们把负数改完了 $k$ 还有剩下的，且必须强行取反一个数。我们可以在数组元素和的基础上，减去最小值的两倍，即为最终答案。

```py [sol-Python3]
class Solution:
    def largestSumAfterKNegations(self, nums: List[int], k: int) -> int:
        nums.sort()  # 从小到大排序

        for i, x in enumerate(nums):  # 优先改最小的
            if k == 0 or x >= 0:  # 修改次数用尽，或者负数已全部取反
                break
            nums[i] *= -1  # 负数取反
            k -= 1  # 消耗一次修改次数

        # 如果剩余的 k 是奇数，选最小的数取反
        return sum(nums) - (min(nums) * 2 if k % 2 else 0)
```

```py [sol-Python3 写法二]
class Solution:
    def largestSumAfterKNegations(self, nums: List[int], k: int) -> int:
        nums.sort()  # 从小到大排序

        total = 0
        mn = inf
        for x in nums:  # 优先改最小的
            if k > 0 and x < 0:  # 还有修改次数，且 x 是负数
                x = -x  # 取反
                k -= 1  # 消耗一次修改次数
            total += x
            mn = min(mn, x)

        # 如果剩余的 k 是奇数，选最小的数取反
        return total - (mn * 2 if k % 2 else 0)
```

```java [sol-Java]
class Solution {
    public int largestSumAfterKNegations(int[] nums, int k) {
        Arrays.sort(nums); // 从小到大排序

        int sum = 0;
        int mn = Integer.MAX_VALUE;
        for (int x : nums) { // 优先改最小的
            if (k > 0 && x < 0) { // 还有修改次数，且 x 是负数
                x = -x; // 取反
                k--; // 消耗一次修改次数
            }
            sum += x;
            mn = Math.min(mn, x);
        }

        // 如果剩余的 k 是奇数，选最小的数取反
        return sum - (k % 2 > 0 ? mn * 2 : 0);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int largestSumAfterKNegations(vector<int>& nums, int k) {
        if (k < nums.size()) {
            // 无需排序，把最小的 k 个数放前面就行
            ranges::nth_element(nums, nums.begin() + k);
        }

        int sum = 0, mn = INT_MAX;
        for (int x : nums) { // 优先改最小的
            if (k > 0 && x < 0) { // 还有修改次数，且 x 是负数
                x = -x; // 取反
                k--; // 消耗一次修改次数
            }
            sum += x;
            mn = min(mn, x);
        }

        // 如果剩余的 k 是奇数，选最小的数取反
        return sum - (k % 2 ? mn * 2 : 0);
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int cmp(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}

int largestSumAfterKNegations(int* nums, int numsSize, int k) {
    qsort(nums, numsSize, sizeof(int), cmp); // 从小到大排序

    int sum = 0, mn = INT_MAX;
    for (int i = 0; i < numsSize; i++) { // 优先改最小的
        int x = nums[i];
        if (k > 0 && x < 0) { // 还有修改次数，且 x 是负数
            x = -x; // 取反
            k--; // 消耗一次修改次数
        }
        sum += x;
        mn = MIN(mn, x);
    }

    // 如果剩余的 k 是奇数，选最小的数取反
    return sum - (k % 2 ? mn * 2 : 0);
}
```

```go [sol-Go]
func largestSumAfterKNegations(nums []int, k int) (sum int) {
    slices.Sort(nums) // 从小到大排序

    mn := math.MaxInt
    for _, x := range nums { // 优先改最小的
        if k > 0 && x < 0 { // 还有修改次数，且 x 是负数
            x = -x // 取反
            k--    // 消耗一次修改次数
        }
        sum += x
        mn = min(mn, x)
    }

    // 如果剩余的 k 是奇数，选最小的数取反
    if k%2 > 0 {
        sum -= mn * 2
    }
    return
}
```

```go [sol-Go 写法二]
func largestSumAfterKNegations(nums []int, k int) (sum int) {
    slices.Sort(nums) // 从小到大排序

    mn := math.MaxInt
    for _, x := range nums { // 优先改最小的
        if k > 0 && x < 0 { // 还有修改次数，且 x 是负数
            x = -x // 取反
            k--    // 消耗一次修改次数
        }
        sum += x
        mn = min(mn, x)
    }

    return sum - mn*(k%2)*2
}
```

```js [sol-JS]
var largestSumAfterKNegations = function(nums, k) {
    nums.sort((a, b) => a - b); // 从小到大排序

    let sum = 0, mn = Infinity;
    for (let x of nums) { // 优先改最小的
        if (k > 0 && x < 0) { // 还有修改次数，且 x 是负数
            x = -x; // 取反
            k--; // 消耗一次修改次数
        }
        sum += x;
        mn = Math.min(mn, x);
    }

    // 如果剩余的 k 是奇数，选最小的数取反
    return sum - (k % 2 ? mn * 2 : 0);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn largest_sum_after_k_negations(mut nums: Vec<i32>, k: i32) -> i32 {
        // 无需排序，把最小的 k 个数放前面就行
        let mut k = k as usize;
        if k < nums.len() {
            nums.select_nth_unstable(k);
        }

        let mut sum = 0;
        let mut mn = i32::MAX;
        for mut x in nums { // 优先改最小的
            if k > 0 && x < 0 { // 还有修改次数，且 x 是负数
                x = -x; // 取反
                k -= 1; // 消耗一次修改次数
            }
            sum += x;
            mn = mn.min(x);
        }

        // 如果剩余的 k 是奇数，选最小的数取反
        if k % 2 > 0 {
            sum -= mn * 2;
        }
        sum
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$ 或 $\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。用快速选择算法可以做到 $\mathcal{O}(n)$，见 C++ 或 Rust 代码。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 专题训练

见下面贪心题单的「**§1.1 从最小/最大开始贪心**」。

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

本题来自 `一、贪心策略 / §1.1 从最小/最大开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.1 从最小/最大开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
