# 53. 最大子数组和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-subarray/
- 题目 slug：`maximum-subarray`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.3 最大子数组和（最大子段和）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-subarray/solutions/2533977/qian-zhui-he-zuo-fa-ben-zhi-shi-mai-mai-abu71/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：前缀和+贪心 / 动态规划，附变形题（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-subarray/solutions/2533977/qian-zhui-he-zuo-fa-ben-zhi-shi-mai-mai-abu71/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-zuo-fa-ben-zhi-shi-mai-mai-abu71`
- topic id：`2533977`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：前缀和 + 贪心

**前置知识**：[前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)

由于子数组的元素和等于两个前缀和的差，所以求出 $\textit{nums}$ 的前缀和，问题就变成 [121. 买卖股票的最佳时机](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock/) 了。本题子数组不能为空，相当于一定要交易一次。

我们可以一边遍历数组计算前缀和，一边维护前缀和的最小值（相当于股票最低价格），用当前的前缀和（卖出价格）减去前缀和的最小值（买入价格），就得到了以当前元素结尾的子数组和的最大值（利润），用它来更新答案的最大值（最大利润）。

请注意，由于题目要求子数组不能为空，应当**先计算前缀和-最小前缀和，再更新最小前缀和**。相当于不能在同一天买入股票又卖出股票。

如果先更新最小前缀和，再计算前缀和-最小前缀和，就会把空数组的元素和 $0$ 算入答案。

示例 1 $[-2,1,-3,4,-1,2,1,-5,4]$ 的计算流程如下，可以对照代码理解。注意计算顺序。

| 元素值 | 前缀和 | 最小前缀和 | 前缀和-最小前缀和 |
|---|---|---|---|
| $-2$ | $-2$ | $0$  | $-2$ |
| $1$ | $-1$ | $-2$ | $1$ |
| $-3$ | $-4$ | $-2$ | $-2$ |
| $4$ | $0$ | $-4$  | $4$ |
| $-1$ | $-1$ | $-4$ | $3$ |
| $2$ | $1$ | $-4$  | $5$ |
| $1$ | $2$ | $-4$  | $6$ |
| $-5$ | $-3$ | $-4$ | $1$ |
| $4$ | $1$ | $-4$  | $5$ |

前缀和-最小前缀和的最大值等于 $6$，即为答案。

### 答疑

**问**：为什么不能直接计算出最大前缀和与最小前缀和，二者相减不就是答案吗？

**答**：这是错的。子数组的和必须是右边的前缀和减去左边的前缀和。如果最大前缀和在左边，最小前缀和在右边，就不符合要求。例如 $\textit{nums}=[1,-2]$，最大子数组和是 $1$，如果用最大前缀和 $1$ 减去最小前缀和 $-1$，结果是错误的 $2$。

```py [sol-Python3]
class Solution:
    def maxSubArray(self, nums: List[int]) -> int:
        ans = -inf
        min_pre_sum = pre_sum = 0
        for x in nums:
            pre_sum += x  # 当前的前缀和
            ans = max(ans, pre_sum - min_pre_sum)  # 减去前缀和的最小值
            min_pre_sum = min(min_pre_sum, pre_sum)  # 维护前缀和的最小值
        return ans
```

```java [sol-Java]
class Solution {
    public int maxSubArray(int[] nums) {
        int ans = Integer.MIN_VALUE;
        int minPreSum = 0;
        int preSum = 0;
        for (int x : nums) {
            preSum += x; // 当前的前缀和
            ans = Math.max(ans, preSum - minPreSum); // 减去前缀和的最小值
            minPreSum = Math.min(minPreSum, preSum); // 维护前缀和的最小值
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int ans = INT_MIN;
        int min_pre_sum = 0;
        int pre_sum = 0;
        for (int x : nums) {
            pre_sum += x; // 当前的前缀和
            ans = max(ans, pre_sum - min_pre_sum); // 减去前缀和的最小值
            min_pre_sum = min(min_pre_sum, pre_sum); // 维护前缀和的最小值
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int maxSubArray(int* nums, int numsSize) {
    int ans = INT_MIN;
    int min_pre_sum = 0;
    int pre_sum = 0;
    for (int i = 0; i < numsSize; i++) {
        pre_sum += nums[i]; // 当前的前缀和
        ans = MAX(ans, pre_sum - min_pre_sum); // 减去前缀和的最小值
        min_pre_sum = MIN(min_pre_sum, pre_sum); // 维护前缀和的最小值
    }
    return ans;
}
```

```go [sol-Go]
func maxSubArray(nums []int) int {
    ans := math.MinInt
    minPreSum := 0
    preSum := 0
    for _, x := range nums {
        preSum += x // 当前的前缀和
        ans = max(ans, preSum-minPreSum)   // 减去前缀和的最小值
        minPreSum = min(minPreSum, preSum) // 维护前缀和的最小值
    }
    return ans
}
```

```js [sol-JavaScript]
var maxSubArray = function(nums) {
    let ans = -Infinity;
    let minPreSum = 0;
    let preSum = 0;
    for (const x of nums) {
        preSum += x; // 当前的前缀和
        ans = Math.max(ans, preSum - minPreSum); // 减去前缀和的最小值
        minPreSum = Math.min(minPreSum, preSum); // 维护前缀和的最小值
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_sub_array(nums: Vec<i32>) -> i32 {
        let mut ans = i32::MIN;
        let mut min_pre_sum = 0;
        let mut pre_sum = 0;
        for x in nums {
            pre_sum += x; // 当前的前缀和
            ans = ans.max(pre_sum - min_pre_sum); // 减去前缀和的最小值
            min_pre_sum = min_pre_sum.min(pre_sum); // 维护前缀和的最小值
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 方法二：动态规划

定义 $f[i]$ 表示以 $\textit{nums}[i]$ 结尾的最大子数组和。

分类讨论：

- $\textit{nums}[i]$ 单独组成一个子数组，那么 $f[i] = \textit{nums}[i]$。
- $\textit{nums}[i]$ 和前面的子数组拼起来，也就是在以 $\textit{nums}[i-1]$ 结尾的最大子数组和之后添加 $\textit{nums}[i]$，那么 $f[i] = f[i-1] + \textit{nums}[i]$。

两种情况取最大值，得

$$
f[i] =
\begin{cases}
\textit{nums}[i],\ &i=0\\
\max(f[i-1], 0) + \textit{nums}[i],\ &i\ge 1
\end{cases}
$$

简单地说，如果 $\textit{nums}[i]$ 左边的子数组元素和是负的，就不用和左边的子数组拼在一起了。

答案为 $\max(f)$。

⚠**注意**：答案不是 $f[n-1]$，这仅仅表示以 $\textit{nums}[n-1]$ 结尾的最大子数组和。或者说 $f[n-1]$ 意味着 $\textit{nums}[n-1]$ 一定要选，但这不一定正确。

### 答疑

**问**：为什么不能用「选或不选 $\textit{nums}[i]$」的思路做？

**答**：选或不选无法保证子数组是连续的。选或不选适用于子序列问题（例如 0-1 背包问题），对于子数组问题，更适合用「拼接」的思路，即如果 $\textit{nums}[i]$ 左边的子数组元素和是负的，就不用和左边的子数组拼在一起了。

### 优化前

```py [sol-Python3]
class Solution:
    def maxSubArray(self, nums: List[int]) -> int:
        f = [0] * len(nums)
        f[0] = nums[0]
        for i in range(1, len(nums)):
            f[i] = max(f[i - 1], 0) + nums[i]
        return max(f)
```

```java [sol-Java]
class Solution {
    public int maxSubArray(int[] nums) {
        int[] f = new int[nums.length];
        f[0] = nums[0];
        int ans = f[0];
        for (int i = 1; i < nums.length; i++) {
            f[i] = Math.max(f[i - 1], 0) + nums[i];
            ans = Math.max(ans, f[i]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        vector<int> f(nums.size());
        f[0] = nums[0];
        for (int i = 1; i < nums.size(); i++) {
            f[i] = max(f[i - 1], 0) + nums[i];
        }
        return ranges::max(f);
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxSubArray(int* nums, int numsSize) {
    int* f = malloc(numsSize * sizeof(int));
    f[0] = nums[0];
    int ans = f[0];
    for (int i = 1; i < numsSize; i++) {
        f[i] = MAX(f[i - 1], 0) + nums[i];
        ans = MAX(ans, f[i]);
    }
    free(f);
    return ans;
}
```

```go [sol-Go]
func maxSubArray(nums []int) int {
    f := make([]int, len(nums))
    f[0] = nums[0]
    for i := 1; i < len(nums); i++ {
        f[i] = max(f[i-1], 0) + nums[i]
    }
    return slices.Max(f)
}
```

```js [sol-JavaScript]
var maxSubArray = function(nums) {
    const f = Array(nums.length);
    f[0] = nums[0];
    for (let i = 1; i < nums.length; i++) {
        f[i] = Math.max(f[i - 1], 0) + nums[i];
    }
    return Math.max(...f);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_sub_array(nums: Vec<i32>) -> i32 {
        let mut f = vec![0; nums.len()];
        f[0] = nums[0];
        for i in 1..nums.len() {
            f[i] = f[i - 1].max(0) + nums[i];
        }
        *f.iter().max().unwrap()
    }
}
```

### 空间优化

由于计算 $f[i]$ 只会用到 $f[i-1]$，不会用到更早的状态，所以可以用一个变量滚动计算。具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)。

状态转移方程简化为：

$$
f = \max(f, 0) + \textit{nums}[i]
$$

$f$ 可以初始化成 $0$ 或者任意负数。

```py [sol-Python3]
class Solution:
    def maxSubArray(self, nums: List[int]) -> int:
        ans = -inf  # 注意答案可以是负数，不能初始化成 0
        f = 0
        for x in nums:
            f = max(f, 0) + x
            ans = max(ans, f)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxSubArray(int[] nums) {
        int ans = Integer.MIN_VALUE; // 注意答案可以是负数，不能初始化成 0
        int f = 0;
        for (int x : nums) {
            f = Math.max(f, 0) + x;
            ans = Math.max(ans, f);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int ans = INT_MIN; // 注意答案可以是负数，不能初始化成 0
        int f = 0;
        for (int x : nums) {
            f = max(f, 0) + x;
            ans = max(ans, f);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxSubArray(int* nums, int numsSize) {
    int ans = INT_MIN; // 注意答案可以是负数，不能初始化成 0
    int f = 0;
    for (int i = 0; i < numsSize; i++) {
        f = MAX(f, 0) + nums[i];
        ans = MAX(ans, f);
    }
    return ans;
}
```

```go [sol-Go]
func maxSubArray(nums []int) int {
    ans := math.MinInt // 注意答案可以是负数，不能初始化成 0
    f := 0
    for _, x := range nums {
        f = max(f, 0) + x
        ans = max(ans, f)
    }
    return ans
}
```

```js [sol-JavaScript]
var maxSubArray = function(nums) {
    let ans = -Infinity; // 注意答案可以是负数，不能初始化成 0
    let f = 0;
    for (const x of nums) {
        f = Math.max(f, 0) + x;
        ans = Math.max(ans, f);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_sub_array(nums: Vec<i32>) -> i32 {
        let mut ans = i32::MIN; // 注意答案可以是负数，不能初始化成 0
        let mut f = 0;
        for x in nums {
            f = f.max(0) + x;
            ans = ans.max(f);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 思考题

1. 改成求最小子数组和，应当如何修改代码？
2. 改成返回和最大的子数组，要怎么做？注意返回的是子数组，不是元素和。
3. 如果子数组的长度有下界，要怎么做？
4. 如果子数组的长度有上界，要怎么做？
5. 如果要求子数组的长度必须是奇数，要怎么做？

欢迎在评论区发表你的思路/代码。

## 变形题

1. 子数组的元素和有上界。见 [363. 矩形区域不超过 K 的最大数值和](https://leetcode.cn/problems/max-sum-of-rectangle-no-larger-than-k/)。提示：枚举上下边界，转成一维数组。
2. 拼接 $k$ 个相同的 $\textit{nums}$，得到一个长为 $nk$ 的大数组，求这个大数组的最大子数组和。见 [1191. K 次串联后最大子数组之和](https://leetcode.cn/problems/k-concatenation-maximum-sum/)。
3. $\textit{nums}$ 是个环形数组。见 [918. 环形子数组的最大和](https://leetcode.cn/problems/maximum-sum-circular-subarray/)。
4. 删除 $\textit{nums}$ 中的至多一个数，再求最大子数组和。见 [1186. 删除一次得到子数组最大和](https://leetcode.cn/problems/maximum-subarray-sum-with-one-deletion/)。

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
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 方法一：前缀和 + 贪心

**前置知识**：[前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)

由于子数组的元素和等于两个前缀和的差，所以求出 $\textit{nums}$ 的前缀和，问题就变成 [121. 买卖股票的最佳时机](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock/) 了。本题子数组不能为空，相当于一定要交易一次。

我们可以一边遍历数组计算前缀和，一边维护前缀和的最小值（相当于股票最低价格），用当前的前缀和（卖出价格）减去前缀和的最小值（买入价格），就得到了以当前元素结尾的子数组和的最大值（利润），用它来更新答案的最大值（最大利润）。

请注意，由于题目要求子数组不能为空，应当**先计算前缀和-最小前缀和，再更新最小前缀和**。相当于不能在同一天买入股票又卖出股票。

如果先更新最小前缀和，再计算前缀和-最小前缀和，就会把空数组的元素和 $0$ 算入答案。

示例 1 $[-2,1,-3,4,-1,2,1,-5,4]$ 的计算流程如下，可以对照代码理解。注意计算顺序。

| 元素值 | 前缀和 | 最小前缀和 | 前缀和-最小前缀和 |
|---|---|---|---|
| $-2$ | $-2$ | $0$  | $-2$ |
| $1$ | $-1$ | $-2$ | $1$ |
| $-3$ | $-4$ | $-2$ | $-2$ |
| $4$ | $0$ | $-4$  | $4$ |
| $-1$ | $-1$ | $-4$ | $3$ |
| $2$ | $1$ | $-4$  | $5$ |
| $1$ | $2$ | $-4$  | $6$ |
| $-5$ | $-3$ | $-4$ | $1$ |
| $4$ | $1$ | $-4$  | $5$ |

前缀和-最小前缀和的最大值等于 $6$，即为答案。

### 答疑

**问**：为什么不能直接计算出最大前缀和与最小前缀和，二者相减不就是答案吗？

**答**：这是错的。子数组的和必须是右边的前缀和减去左边的前缀和。如果最大前缀和在左边，最小前缀和在右边，就不符合要求。例如 $\textit{nums}=[1,-2]$，最大子数组和是 $1$，如果用最大前缀和 $1$ 减去最小前缀和 $-1$，结果是错误的 $2$。

```py [sol-Python3]
class Solution:
    def maxSubArray(self, nums: List[int]) -> int:
        ans = -inf
        min_pre_sum = pre_sum = 0
        for x in nums:
            pre_sum += x  # 当前的前缀和
            ans = max(ans, pre_sum - min_pre_sum)  # 减去前缀和的最小值
            min_pre_sum = min(min_pre_sum, pre_sum)  # 维护前缀和的最小值
        return ans
```

```java [sol-Java]
class Solution {
    public int maxSubArray(int[] nums) {
        int ans = Integer.MIN_VALUE;
        int minPreSum = 0;
        int preSum = 0;
        for (int x : nums) {
            preSum += x; // 当前的前缀和
            ans = Math.max(ans, preSum - minPreSum); // 减去前缀和的最小值
            minPreSum = Math.min(minPreSum, preSum); // 维护前缀和的最小值
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int ans = INT_MIN;
        int min_pre_sum = 0;
        int pre_sum = 0;
        for (int x : nums) {
            pre_sum += x; // 当前的前缀和
            ans = max(ans, pre_sum - min_pre_sum); // 减去前缀和的最小值
            min_pre_sum = min(min_pre_sum, pre_sum); // 维护前缀和的最小值
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int maxSubArray(int* nums, int numsSize) {
    int ans = INT_MIN;
    int min_pre_sum = 0;
    int pre_sum = 0;
    for (int i = 0; i < numsSize; i++) {
        pre_sum += nums[i]; // 当前的前缀和
        ans = MAX(ans, pre_sum - min_pre_sum); // 减去前缀和的最小值
        min_pre_sum = MIN(min_pre_sum, pre_sum); // 维护前缀和的最小值
    }
    return ans;
}
```

```go [sol-Go]
func maxSubArray(nums []int) int {
    ans := math.MinInt
    minPreSum := 0
    preSum := 0
    for _, x := range nums {
        preSum += x // 当前的前缀和
        ans = max(ans, preSum-minPreSum)   // 减去前缀和的最小值
        minPreSum = min(minPreSum, preSum) // 维护前缀和的最小值
    }
    return ans
}
```

```js [sol-JavaScript]
var maxSubArray = function(nums) {
    let ans = -Infinity;
    let minPreSum = 0;
    let preSum = 0;
    for (const x of nums) {
        preSum += x; // 当前的前缀和
        ans = Math.max(ans, preSum - minPreSum); // 减去前缀和的最小值
        minPreSum = Math.min(minPreSum, preSum); // 维护前缀和的最小值
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_sub_array(nums: Vec<i32>) -> i32 {
        let mut ans = i32::MIN;
        let mut min_pre_sum = 0;
        let mut pre_sum = 0;
        for x in nums {
            pre_sum += x; // 当前的前缀和
            ans = ans.max(pre_sum - min_pre_sum); // 减去前缀和的最小值
            min_pre_sum = min_pre_sum.min(pre_sum); // 维护前缀和的最小值
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 方法二：动态规划

定义 $f[i]$ 表示以 $\textit{nums}[i]$ 结尾的最大子数组和。

分类讨论：

- $\textit{nums}[i]$ 单独组成一个子数组，那么 $f[i] = \textit{nums}[i]$。
- $\textit{nums}[i]$ 和前面的子数组拼起来，也就是在以 $\textit{nums}[i-1]$ 结尾的最大子数组和之后添加 $\textit{nums}[i]$，那么 $f[i] = f[i-1] + \textit{nums}[i]$。

两种情况取最大值，得

$$
f[i] =
\begin{cases}
\textit{nums}[i],\ &i=0\\
\max(f[i-1], 0) + \textit{nums}[i],\ &i\ge 1
\end{cases}
$$

简单地说，如果 $\textit{nums}[i]$ 左边的子数组元素和是负的，就不用和左边的子数组拼在一起了。

答案为 $\max(f)$。

⚠**注意**：答案不是 $f[n-1]$，这仅仅表示以 $\textit{nums}[n-1]$ 结尾的最大子数组和。或者说 $f[n-1]$ 意味着 $\textit{nums}[n-1]$ 一定要选，但这不一定正确。

### 答疑

**问**：为什么不能用「选或不选 $\textit{nums}[i]$」的思路做？

**答**：选或不选无法保证子数组是连续的。选或不选适用于子序列问题（例如 0-1 背包问题），对于子数组问题，更适合用「拼接」的思路，即如果 $\textit{nums}[i]$ 左边的子数组元素和是负的，就不用和左边的子数组拼在一起了。

### 优化前

```py [sol-Python3]
class Solution:
    def maxSubArray(self, nums: List[int]) -> int:
        f = [0] * len(nums)
        f[0] = nums[0]
        for i in range(1, len(nums)):
            f[i] = max(f[i - 1], 0) + nums[i]
        return max(f)
```

```java [sol-Java]
class Solution {
    public int maxSubArray(int[] nums) {
        int[] f = new int[nums.length];
        f[0] = nums[0];
        int ans = f[0];
        for (int i = 1; i < nums.length; i++) {
            f[i] = Math.max(f[i - 1], 0) + nums[i];
            ans = Math.max(ans, f[i]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        vector<int> f(nums.size());
        f[0] = nums[0];
        for (int i = 1; i < nums.size(); i++) {
            f[i] = max(f[i - 1], 0) + nums[i];
        }
        return ranges::max(f);
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxSubArray(int* nums, int numsSize) {
    int* f = malloc(numsSize * sizeof(int));
    f[0] = nums[0];
    int ans = f[0];
    for (int i = 1; i < numsSize; i++) {
        f[i] = MAX(f[i - 1], 0) + nums[i];
        ans = MAX(ans, f[i]);
    }
    free(f);
    return ans;
}
```

```go [sol-Go]
func maxSubArray(nums []int) int {
    f := make([]int, len(nums))
    f[0] = nums[0]
    for i := 1; i < len(nums); i++ {
        f[i] = max(f[i-1], 0) + nums[i]
    }
    return slices.Max(f)
}
```

```js [sol-JavaScript]
var maxSubArray = function(nums) {
    const f = Array(nums.length);
    f[0] = nums[0];
    for (let i = 1; i < nums.length; i++) {
        f[i] = Math.max(f[i - 1], 0) + nums[i];
    }
    return Math.max(...f);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_sub_array(nums: Vec<i32>) -> i32 {
        let mut f = vec![0; nums.len()];
        f[0] = nums[0];
        for i in 1..nums.len() {
            f[i] = f[i - 1].max(0) + nums[i];
        }
        *f.iter().max().unwrap()
    }
}
```

### 空间优化

由于计算 $f[i]$ 只会用到 $f[i-1]$，不会用到更早的状态，所以可以用一个变量滚动计算。具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)。

状态转移方程简化为：

$$
f = \max(f, 0) + \textit{nums}[i]
$$

$f$ 可以初始化成 $0$ 或者任意负数。

```py [sol-Python3]
class Solution:
    def maxSubArray(self, nums: List[int]) -> int:
        ans = -inf  # 注意答案可以是负数，不能初始化成 0
        f = 0
        for x in nums:
            f = max(f, 0) + x
            ans = max(ans, f)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxSubArray(int[] nums) {
        int ans = Integer.MIN_VALUE; // 注意答案可以是负数，不能初始化成 0
        int f = 0;
        for (int x : nums) {
            f = Math.max(f, 0) + x;
            ans = Math.max(ans, f);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int ans = INT_MIN; // 注意答案可以是负数，不能初始化成 0
        int f = 0;
        for (int x : nums) {
            f = max(f, 0) + x;
            ans = max(ans, f);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maxSubArray(int* nums, int numsSize) {
    int ans = INT_MIN; // 注意答案可以是负数，不能初始化成 0
    int f = 0;
    for (int i = 0; i < numsSize; i++) {
        f = MAX(f, 0) + nums[i];
        ans = MAX(ans, f);
    }
    return ans;
}
```

```go [sol-Go]
func maxSubArray(nums []int) int {
    ans := math.MinInt // 注意答案可以是负数，不能初始化成 0
    f := 0
    for _, x := range nums {
        f = max(f, 0) + x
        ans = max(ans, f)
    }
    return ans
}
```

```js [sol-JavaScript]
var maxSubArray = function(nums) {
    let ans = -Infinity; // 注意答案可以是负数，不能初始化成 0
    let f = 0;
    for (const x of nums) {
        f = Math.max(f, 0) + x;
        ans = Math.max(ans, f);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_sub_array(nums: Vec<i32>) -> i32 {
        let mut ans = i32::MIN; // 注意答案可以是负数，不能初始化成 0
        let mut f = 0;
        for x in nums {
            f = f.max(0) + x;
            ans = ans.max(f);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。仅用到若干额外变量。

## 思考题

1. 改成求最小子数组和，应当如何修改代码？
2. 改成返回和最大的子数组，要怎么做？注意返回的是子数组，不是元素和。
3. 如果子数组的长度有下界，要怎么做？
4. 如果子数组的长度有上界，要怎么做？
5. 如果要求子数组的长度必须是奇数，要怎么做？

欢迎在评论区发表你的思路/代码。

## 变形题

1. 子数组的元素和有上界。见 [363. 矩形区域不超过 K 的最大数值和](https://leetcode.cn/problems/max-sum-of-rectangle-no-larger-than-k/)。提示：枚举上下边界，转成一维数组。
2. 拼接 $k$ 个相同的 $\textit{nums}$，得到一个长为 $nk$ 的大数组，求这个大数组的最大子数组和。见 [1191. K 次串联后最大子数组之和](https://leetcode.cn/problems/k-concatenation-maximum-sum/)。
3. $\textit{nums}$ 是个环形数组。见 [918. 环形子数组的最大和](https://leetcode.cn/problems/maximum-sum-circular-subarray/)。
4. 删除 $\textit{nums}$ 中的至多一个数，再求最大子数组和。见 [1186. 删除一次得到子数组最大和](https://leetcode.cn/problems/maximum-subarray-sum-with-one-deletion/)。

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
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
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
