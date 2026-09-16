# 152. 乘积最大子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-product-subarray/
- 题目 slug：`maximum-product-subarray`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.3 最大子数组和（最大子段和）
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/maximum-product-subarray/solutions/2968916/dong-tai-gui-hua-jian-ji-gao-xiao-python-i778/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划，简洁高效！（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-product-subarray/solutions/2968916/dong-tai-gui-hua-jian-ji-gao-xiao-python-i778/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-jian-ji-gao-xiao-python-i778`
- topic id：`2968916`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题是 [53. 最大子数组和](https://leetcode.cn/problems/maximum-subarray/) 的乘法版本，推荐先完成 53 题。

## 寻找子问题

例如 $\textit{nums}=[-2,1,-3,4]$，讨论右端点为 $\textit{nums}[3]=4$ 的子数组的最大乘积：

- $4$ 单独组成一个子数组。
- $4$ 和前面的子数组拼起来，也就是在右端点为 $\textit{nums}[2]=-3$ 的乘积最大子数组之后添加 $4$。

又例如 $\textit{nums}=[-2,1,-3,-4]$，讨论右端点为 $\textit{nums}[3]=-4$ 的子数组的最大乘积：

- $-4$ 单独组成一个子数组。
- $-4$ 和前面的子数组拼起来，由于 $-4$ 是负数，要想得到最大的乘积，根据**负负得正**，我们可以在右端点为 $\textit{nums}[2]=-3$ 的乘积**最小**子数组之后添加 $-4$。

## 状态定义与状态转移方程

上面两个例子启发我们，需要在遍历 $\textit{nums}$ 的同时，维护两个信息：

- 右端点下标为 $i$ 的子数组的最大乘积，记作 $f_{\textit{max}}[i]$。
- 右端点下标为 $i$ 的子数组的最小乘积，记作 $f_{\textit{min}}[i]$。

设 $x=\textit{nums}[i]$，分类讨论：

- $x$ 单独组成一个子数组，那么 $f_{\textit{max}}[i] = x$。
- $x$ 和前面的子数组拼起来，也就是在右端点下标为 $i-1$ 的乘积最大子数组之后添加 $x$，那么 $f_{\textit{max}}[i] = f_{\textit{max}}[i-1]\cdot x$；也可以在右端点下标为 $i-1$ 的乘积最小子数组之后添加 $x$，那么 $f_{\textit{max}}[i] = f_{\textit{min}}[i-1]\cdot x$。把这两种都算一下，这样我们就无需判断 $x$ 到底是正还是负了。

三种情况取最大值，得

$$
f_{\textit{max}}[i] = \max(f_{\textit{max}}[i-1]\cdot x, f_{\textit{min}}[i-1]\cdot x, x)
$$

同理得

$$
f_{\textit{min}}[i] = \min(f_{\textit{max}}[i-1]\cdot x, f_{\textit{min}}[i-1]\cdot x, x)
$$

由于以 $\textit{nums}[0]$ 为右端点的子数组乘积只能是 $\textit{nums}[0]$，所以初始值为 $f_{\textit{max}}[0] = f_{\textit{min}}[0] = \textit{nums}[0]$。这是一种初始化的方法，下文会讲另外一种。

答案为 $\max(f_{\textit{max}})$。

## 写法一

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        n = len(nums)
        f_max = [0] * n
        f_min = [0] * n
        f_max[0] = f_min[0] = nums[0]
        for i in range(1, n):
            x = nums[i]
            # 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
            # 或者单独组成一个子数组，只有 x 一个元素
            f_max[i] = max(f_max[i - 1] * x, f_min[i - 1] * x, x)
            f_min[i] = min(f_max[i - 1] * x, f_min[i - 1] * x, x)
        return max(f_max)
```

```java [sol-Java]
class Solution {
    public int maxProduct(int[] nums) {
        int n = nums.length;
        int[] fMax = new int[n];
        int[] fMin = new int[n];
        fMax[0] = fMin[0] = nums[0];
        for (int i = 1; i < n; i++) {
            int x = nums[i];
            // 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
            // 或者单独组成一个子数组，只有 x 一个元素
            fMax[i] = Math.max(Math.max(fMax[i - 1] * x, fMin[i - 1] * x), x);
            fMin[i] = Math.min(Math.min(fMax[i - 1] * x, fMin[i - 1] * x), x);
        }
        return Arrays.stream(fMax).max().getAsInt();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProduct(vector<int>& nums) {
        int n = nums.size();
        vector<int> f_max(n), f_min(n);
        f_max[0] = f_min[0] = nums[0];
        for (int i = 1; i < n; i++) {
            int x = nums[i];
            // 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
            // 或者单独组成一个子数组，只有 x 一个元素
            f_max[i] = max({f_max[i - 1] * x, f_min[i - 1] * x, x});
            f_min[i] = min({f_max[i - 1] * x, f_min[i - 1] * x, x});
        }
        return ranges::max(f_max);
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int maxProduct(int* nums, int numsSize) {
    int* f_max = malloc(numsSize * sizeof(int));
    int* f_min = malloc(numsSize * sizeof(int));
    f_max[0] = f_min[0] = nums[0];
    int ans = nums[0];

    for (int i = 1; i < numsSize; i++) {
        int x = nums[i];
        // 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
        // 或者单独组成一个子数组，只有 x 一个元素
        f_max[i] = MAX(MAX(f_max[i - 1] * x, f_min[i - 1] * x), x);
        f_min[i] = MIN(MIN(f_max[i - 1] * x, f_min[i - 1] * x), x);
        ans = MAX(ans, f_max[i]);
    }

    free(f_max);
    free(f_min);
    return ans;
}
```

```go [sol-Go]
func maxProduct(nums []int) int {
    n := len(nums)
    fMax := make([]int, n)
    fMin := make([]int, n)
    fMax[0], fMin[0] = nums[0], nums[0]
    for i := 1; i < n; i++ {
        x := nums[i]
        // 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
        // 或者单独组成一个子数组，只有 x 一个元素
        fMax[i] = max(fMax[i-1]*x, fMin[i-1]*x, x)
        fMin[i] = min(fMax[i-1]*x, fMin[i-1]*x, x)
    }
    return slices.Max(fMax)
}
```

```js [sol-JavaScript]
var maxProduct = function(nums) {
    const n = nums.length;
    const fMax = new Array(n);
    const fMin = new Array(n);
    fMax[0] = fMin[0] = nums[0];
    for (let i = 1; i < n; i++) {
        const x = nums[i];
        // 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
        // 或者单独组成一个子数组，只有 x 一个元素
        fMax[i] = Math.max(fMax[i - 1] * x, fMin[i - 1] * x, x);
        fMin[i] = Math.min(fMax[i - 1] * x, fMin[i - 1] * x, x);
    }
    return Math.max(...fMax);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_product(nums: Vec<i32>) -> i32 {
        let n = nums.len();
        let mut f_max = vec![0; n];
        let mut f_min = vec![0; n];
        f_max[0] = nums[0];
        f_min[0] = nums[0];
        for i in 1..n {
            let x = nums[i];
            // 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
            // 或者单独组成一个子数组，只有 x 一个元素
            f_max[i] = x.max(f_max[i - 1] * x).max(f_min[i - 1] * x);
            f_min[i] = x.min(f_max[i - 1] * x).min(f_min[i - 1] * x);
        }
        *f_max.iter().max().unwrap()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 写法二（空间优化）

由于计算 $f_{\textit{max}}[i]$ 和 $f_{\textit{min}}[i]$ 只会用到 $f_{\textit{max}}[i-1]$ 和 $f_{\textit{min}}[i-1]$，不会用到更早的状态，所以可以用两个变量 $f_{\textit{max}}$ 和 $f_{\textit{min}}$ 滚动计算。具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)。

状态转移方程简化为：

$$
\begin{aligned}
f_{\textit{max}} &= \max(f_{\textit{max}}\cdot x, f_{\textit{min}}\cdot x, x) \\
f_{\textit{min}} &= \min(f_{\textit{max}}\cdot x, f_{\textit{min}}\cdot x, x) \\
\end{aligned}
$$

注意这两个式子要同时计算。

代码实现时，可以初始化 $f_{\textit{max}} = f_{\textit{min}} = 1$，因为 $1$ 乘以 $\textit{nums}[0]$ 等于 $\textit{nums}[0]$，这样我们可以从下标 $0$ 开始遍历 $\textit{nums}$，代码写起来更简单。

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        ans = -inf  # 注意答案可能是负数
        f_max = f_min = 1
        for x in nums:
            f_max, f_min = max(f_max * x, f_min * x, x), \
                           min(f_max * x, f_min * x, x)
            ans = max(ans, f_max)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxProduct(int[] nums) {
        int ans = Integer.MIN_VALUE; // 注意答案可能是负数
        int fMax = 1;
        int fMin = 1;
        for (int x : nums) {
            int mx = fMax;
            fMax = Math.max(Math.max(fMax * x, fMin * x), x);
            fMin = Math.min(Math.min(mx * x, fMin * x), x);
            ans = Math.max(ans, fMax);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProduct(vector<int>& nums) {
        int ans = INT_MIN; // 注意答案可能是负数
        int f_max = 1, f_min = 1;
        for (int x : nums) {
            int mx = f_max;
            f_max = max({f_max * x, f_min * x, x});
            f_min = min({mx * x, f_min * x, x});
            ans = max(ans, f_max);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int maxProduct(int* nums, int numsSize) {
    int ans = INT_MIN; // 注意答案可能是负数
    int f_max = 1, f_min = 1;
    for (int i = 0; i < numsSize; i++) {
        int x = nums[i];
        int mx = f_max;
        f_max = MAX(MAX(f_max * x, f_min * x), x);
        f_min = MIN(MIN(mx * x, f_min * x), x);
        ans = MAX(ans, f_max);
    }
    return ans;
}
```

```go [sol-Go]
func maxProduct(nums []int) int {
    ans := math.MinInt // 注意答案可能是负数
    fMax, fMin := 1, 1
    for _, x := range nums {
        fMax, fMin = max(fMax*x, fMin*x, x),
                     min(fMax*x, fMin*x, x)
        ans = max(ans, fMax)
    }
    return ans
}
```

```js [sol-JavaScript]
var maxProduct = function(nums) {
    let ans = -Infinity; // 注意答案可能是负数
    let fMax = 1, fMin = 1;
    for (const x of nums) {
        const mx = fMax;
        fMax = Math.max(fMax * x, fMin * x, x);
        fMin = Math.min(mx * x, fMin * x, x);
        ans = Math.max(ans, fMax);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_product(nums: Vec<i32>) -> i32 {
        let mut ans = i32::MIN; // 注意答案可能是负数
        let mut f_max = 1;
        let mut f_min = 1;
        for x in nums {
            let mx = f_max;
            f_max = x.max(f_max * x).max(f_min * x);
            f_min = x.min(mx * x).min(f_min * x);
            ans = ans.max(f_max);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 写法三

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        ans = -inf
        f_max = f_min = 1
        for x in nums:
            if x < 0:
                # 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
                # 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 max 和 min 可以少一项
                f_max, f_min = f_min, f_max
            f_max = max(f_max * x, x)
            f_min = min(f_min * x, x)
            ans = max(ans, f_max)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxProduct(int[] nums) {
        int ans = Integer.MIN_VALUE;
        int fMax = 1;
        int fMin = 1;
        for (int x : nums) {
            if (x < 0) {
                // 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
                // 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 max 和 min 可以少一项
                int tmp = fMax;
                fMax = fMin;
                fMin = tmp;
            }
            fMax = Math.max(fMax * x, x);
            fMin = Math.min(fMin * x, x);
            ans = Math.max(ans, fMax);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProduct(vector<int>& nums) {
        int ans = INT_MIN;
        int f_max = 1, f_min = 1;
        for (int x : nums) {
            if (x < 0) {
                // 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
                // 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 max 和 min 可以少一项
                swap(f_max, f_min);
            }
            f_max = max(f_max * x, x);
            f_min = min(f_min * x, x);
            ans = max(ans, f_max);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int maxProduct(int* nums, int numsSize) {
    int ans = INT_MIN;
    int f_max = 1, f_min = 1;
    for (int i = 0; i < numsSize; i++) {
        int x = nums[i];
        if (x < 0) {
            // 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
            // 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 MAX 和 MIN 可以少一项
            int tmp = f_max;
            f_max = f_min;
            f_min = tmp;
        }
        f_max = MAX(f_max * x, x);
        f_min = MIN(f_min * x, x);
        ans = MAX(ans, f_max);
    }
    return ans;
}
```

```go [sol-Go]
func maxProduct(nums []int) int {
    ans := math.MinInt
    fMax, fMin := 1, 1
    for _, x := range nums {
        if x < 0 {
            // 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
            // 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 max 和 min 可以少一项
            fMax, fMin = fMin, fMax
        }
        fMax = max(fMax*x, x)
        fMin = min(fMin*x, x)
        ans = max(ans, fMax)
    }
    return ans
}
```

```js [sol-JavaScript]
var maxProduct = function(nums) {
    let ans = -Infinity;
    let fMax = 1, fMin = 1;
    for (const x of nums) {
        if (x < 0) {
            // 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
            // 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 max 和 min 可以少一项
            [fMax, fMin] = [fMin, fMax];
        }
        fMax = Math.max(fMax * x, x);
        fMin = Math.min(fMin * x, x);
        ans = Math.max(ans, fMax);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_product(nums: Vec<i32>) -> i32 {
        let mut ans = i32::MIN;
        let mut f_max = 1;
        let mut f_min = 1;
        for x in nums {
            if x < 0 {
                // 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
                // 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 max 和 min 可以少一项
                (f_max, f_min) = (f_min, f_max);
            }
            f_max = x.max(f_max * x);
            f_min = x.min(f_min * x);
            ans = ans.max(f_max);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 变形题

把子数组改成子序列，要怎么做？

- [2708. 一个小组的最大实力值](https://leetcode.cn/problems/maximum-strength-of-a-group/description/)

更多相似题目，见下面动态规划题单中的「**§1.3 最大子数组和**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
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

## 寻找子问题

例如 $\textit{nums}=[-2,1,-3,4]$，讨论右端点为 $\textit{nums}[3]=4$ 的子数组的最大乘积：

- $4$ 单独组成一个子数组。
- $4$ 和前面的子数组拼起来，也就是在右端点为 $\textit{nums}[2]=-3$ 的乘积最大子数组之后添加 $4$。

又例如 $\textit{nums}=[-2,1,-3,-4]$，讨论右端点为 $\textit{nums}[3]=-4$ 的子数组的最大乘积：

- $-4$ 单独组成一个子数组。
- $-4$ 和前面的子数组拼起来，由于 $-4$ 是负数，要想得到最大的乘积，根据**负负得正**，我们可以在右端点为 $\textit{nums}[2]=-3$ 的乘积**最小**子数组之后添加 $-4$。

## 状态定义与状态转移方程

上面两个例子启发我们，需要在遍历 $\textit{nums}$ 的同时，维护两个信息：

- 右端点下标为 $i$ 的子数组的最大乘积，记作 $f_{\textit{max}}[i]$。
- 右端点下标为 $i$ 的子数组的最小乘积，记作 $f_{\textit{min}}[i]$。

设 $x=\textit{nums}[i]$，分类讨论：

- $x$ 单独组成一个子数组，那么 $f_{\textit{max}}[i] = x$。
- $x$ 和前面的子数组拼起来，也就是在右端点下标为 $i-1$ 的乘积最大子数组之后添加 $x$，那么 $f_{\textit{max}}[i] = f_{\textit{max}}[i-1]\cdot x$；也可以在右端点下标为 $i-1$ 的乘积最小子数组之后添加 $x$，那么 $f_{\textit{max}}[i] = f_{\textit{min}}[i-1]\cdot x$。把这两种都算一下，这样我们就无需判断 $x$ 到底是正还是负了。

三种情况取最大值，得

$$
f_{\textit{max}}[i] = \max(f_{\textit{max}}[i-1]\cdot x, f_{\textit{min}}[i-1]\cdot x, x)
$$

同理得

$$
f_{\textit{min}}[i] = \min(f_{\textit{max}}[i-1]\cdot x, f_{\textit{min}}[i-1]\cdot x, x)
$$

由于以 $\textit{nums}[0]$ 为右端点的子数组乘积只能是 $\textit{nums}[0]$，所以初始值为 $f_{\textit{max}}[0] = f_{\textit{min}}[0] = \textit{nums}[0]$。这是一种初始化的方法，下文会讲另外一种。

答案为 $\max(f_{\textit{max}})$。

## 写法一

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        n = len(nums)
        f_max = [0] * n
        f_min = [0] * n
        f_max[0] = f_min[0] = nums[0]
        for i in range(1, n):
            x = nums[i]
            # 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
            # 或者单独组成一个子数组，只有 x 一个元素
            f_max[i] = max(f_max[i - 1] * x, f_min[i - 1] * x, x)
            f_min[i] = min(f_max[i - 1] * x, f_min[i - 1] * x, x)
        return max(f_max)
```

```java [sol-Java]
class Solution {
    public int maxProduct(int[] nums) {
        int n = nums.length;
        int[] fMax = new int[n];
        int[] fMin = new int[n];
        fMax[0] = fMin[0] = nums[0];
        for (int i = 1; i < n; i++) {
            int x = nums[i];
            // 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
            // 或者单独组成一个子数组，只有 x 一个元素
            fMax[i] = Math.max(Math.max(fMax[i - 1] * x, fMin[i - 1] * x), x);
            fMin[i] = Math.min(Math.min(fMax[i - 1] * x, fMin[i - 1] * x), x);
        }
        return Arrays.stream(fMax).max().getAsInt();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProduct(vector<int>& nums) {
        int n = nums.size();
        vector<int> f_max(n), f_min(n);
        f_max[0] = f_min[0] = nums[0];
        for (int i = 1; i < n; i++) {
            int x = nums[i];
            // 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
            // 或者单独组成一个子数组，只有 x 一个元素
            f_max[i] = max({f_max[i - 1] * x, f_min[i - 1] * x, x});
            f_min[i] = min({f_max[i - 1] * x, f_min[i - 1] * x, x});
        }
        return ranges::max(f_max);
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int maxProduct(int* nums, int numsSize) {
    int* f_max = malloc(numsSize * sizeof(int));
    int* f_min = malloc(numsSize * sizeof(int));
    f_max[0] = f_min[0] = nums[0];
    int ans = nums[0];

    for (int i = 1; i < numsSize; i++) {
        int x = nums[i];
        // 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
        // 或者单独组成一个子数组，只有 x 一个元素
        f_max[i] = MAX(MAX(f_max[i - 1] * x, f_min[i - 1] * x), x);
        f_min[i] = MIN(MIN(f_max[i - 1] * x, f_min[i - 1] * x), x);
        ans = MAX(ans, f_max[i]);
    }

    free(f_max);
    free(f_min);
    return ans;
}
```

```go [sol-Go]
func maxProduct(nums []int) int {
    n := len(nums)
    fMax := make([]int, n)
    fMin := make([]int, n)
    fMax[0], fMin[0] = nums[0], nums[0]
    for i := 1; i < n; i++ {
        x := nums[i]
        // 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
        // 或者单独组成一个子数组，只有 x 一个元素
        fMax[i] = max(fMax[i-1]*x, fMin[i-1]*x, x)
        fMin[i] = min(fMax[i-1]*x, fMin[i-1]*x, x)
    }
    return slices.Max(fMax)
}
```

```js [sol-JavaScript]
var maxProduct = function(nums) {
    const n = nums.length;
    const fMax = new Array(n);
    const fMin = new Array(n);
    fMax[0] = fMin[0] = nums[0];
    for (let i = 1; i < n; i++) {
        const x = nums[i];
        // 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
        // 或者单独组成一个子数组，只有 x 一个元素
        fMax[i] = Math.max(fMax[i - 1] * x, fMin[i - 1] * x, x);
        fMin[i] = Math.min(fMax[i - 1] * x, fMin[i - 1] * x, x);
    }
    return Math.max(...fMax);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_product(nums: Vec<i32>) -> i32 {
        let n = nums.len();
        let mut f_max = vec![0; n];
        let mut f_min = vec![0; n];
        f_max[0] = nums[0];
        f_min[0] = nums[0];
        for i in 1..n {
            let x = nums[i];
            // 把 x 加到右端点为 i-1 的（乘积最大/最小）子数组后面，
            // 或者单独组成一个子数组，只有 x 一个元素
            f_max[i] = x.max(f_max[i - 1] * x).max(f_min[i - 1] * x);
            f_min[i] = x.min(f_max[i - 1] * x).min(f_min[i - 1] * x);
        }
        *f_max.iter().max().unwrap()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 写法二（空间优化）

由于计算 $f_{\textit{max}}[i]$ 和 $f_{\textit{min}}[i]$ 只会用到 $f_{\textit{max}}[i-1]$ 和 $f_{\textit{min}}[i-1]$，不会用到更早的状态，所以可以用两个变量 $f_{\textit{max}}$ 和 $f_{\textit{min}}$ 滚动计算。具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)。

状态转移方程简化为：

$$
\begin{aligned}
f_{\textit{max}} &= \max(f_{\textit{max}}\cdot x, f_{\textit{min}}\cdot x, x) \\
f_{\textit{min}} &= \min(f_{\textit{max}}\cdot x, f_{\textit{min}}\cdot x, x) \\
\end{aligned}
$$

注意这两个式子要同时计算。

代码实现时，可以初始化 $f_{\textit{max}} = f_{\textit{min}} = 1$，因为 $1$ 乘以 $\textit{nums}[0]$ 等于 $\textit{nums}[0]$，这样我们可以从下标 $0$ 开始遍历 $\textit{nums}$，代码写起来更简单。

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        ans = -inf  # 注意答案可能是负数
        f_max = f_min = 1
        for x in nums:
            f_max, f_min = max(f_max * x, f_min * x, x), \
                           min(f_max * x, f_min * x, x)
            ans = max(ans, f_max)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxProduct(int[] nums) {
        int ans = Integer.MIN_VALUE; // 注意答案可能是负数
        int fMax = 1;
        int fMin = 1;
        for (int x : nums) {
            int mx = fMax;
            fMax = Math.max(Math.max(fMax * x, fMin * x), x);
            fMin = Math.min(Math.min(mx * x, fMin * x), x);
            ans = Math.max(ans, fMax);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProduct(vector<int>& nums) {
        int ans = INT_MIN; // 注意答案可能是负数
        int f_max = 1, f_min = 1;
        for (int x : nums) {
            int mx = f_max;
            f_max = max({f_max * x, f_min * x, x});
            f_min = min({mx * x, f_min * x, x});
            ans = max(ans, f_max);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int maxProduct(int* nums, int numsSize) {
    int ans = INT_MIN; // 注意答案可能是负数
    int f_max = 1, f_min = 1;
    for (int i = 0; i < numsSize; i++) {
        int x = nums[i];
        int mx = f_max;
        f_max = MAX(MAX(f_max * x, f_min * x), x);
        f_min = MIN(MIN(mx * x, f_min * x), x);
        ans = MAX(ans, f_max);
    }
    return ans;
}
```

```go [sol-Go]
func maxProduct(nums []int) int {
    ans := math.MinInt // 注意答案可能是负数
    fMax, fMin := 1, 1
    for _, x := range nums {
        fMax, fMin = max(fMax*x, fMin*x, x),
                     min(fMax*x, fMin*x, x)
        ans = max(ans, fMax)
    }
    return ans
}
```

```js [sol-JavaScript]
var maxProduct = function(nums) {
    let ans = -Infinity; // 注意答案可能是负数
    let fMax = 1, fMin = 1;
    for (const x of nums) {
        const mx = fMax;
        fMax = Math.max(fMax * x, fMin * x, x);
        fMin = Math.min(mx * x, fMin * x, x);
        ans = Math.max(ans, fMax);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_product(nums: Vec<i32>) -> i32 {
        let mut ans = i32::MIN; // 注意答案可能是负数
        let mut f_max = 1;
        let mut f_min = 1;
        for x in nums {
            let mx = f_max;
            f_max = x.max(f_max * x).max(f_min * x);
            f_min = x.min(mx * x).min(f_min * x);
            ans = ans.max(f_max);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 写法三

```py [sol-Python3]
class Solution:
    def maxProduct(self, nums: List[int]) -> int:
        ans = -inf
        f_max = f_min = 1
        for x in nums:
            if x < 0:
                # 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
                # 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 max 和 min 可以少一项
                f_max, f_min = f_min, f_max
            f_max = max(f_max * x, x)
            f_min = min(f_min * x, x)
            ans = max(ans, f_max)
        return ans
```

```java [sol-Java]
class Solution {
    public int maxProduct(int[] nums) {
        int ans = Integer.MIN_VALUE;
        int fMax = 1;
        int fMin = 1;
        for (int x : nums) {
            if (x < 0) {
                // 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
                // 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 max 和 min 可以少一项
                int tmp = fMax;
                fMax = fMin;
                fMin = tmp;
            }
            fMax = Math.max(fMax * x, x);
            fMin = Math.min(fMin * x, x);
            ans = Math.max(ans, fMax);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProduct(vector<int>& nums) {
        int ans = INT_MIN;
        int f_max = 1, f_min = 1;
        for (int x : nums) {
            if (x < 0) {
                // 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
                // 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 max 和 min 可以少一项
                swap(f_max, f_min);
            }
            f_max = max(f_max * x, x);
            f_min = min(f_min * x, x);
            ans = max(ans, f_max);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int maxProduct(int* nums, int numsSize) {
    int ans = INT_MIN;
    int f_max = 1, f_min = 1;
    for (int i = 0; i < numsSize; i++) {
        int x = nums[i];
        if (x < 0) {
            // 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
            // 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 MAX 和 MIN 可以少一项
            int tmp = f_max;
            f_max = f_min;
            f_min = tmp;
        }
        f_max = MAX(f_max * x, x);
        f_min = MIN(f_min * x, x);
        ans = MAX(ans, f_max);
    }
    return ans;
}
```

```go [sol-Go]
func maxProduct(nums []int) int {
    ans := math.MinInt
    fMax, fMin := 1, 1
    for _, x := range nums {
        if x < 0 {
            // 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
            // 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 max 和 min 可以少一项
            fMax, fMin = fMin, fMax
        }
        fMax = max(fMax*x, x)
        fMin = min(fMin*x, x)
        ans = max(ans, fMax)
    }
    return ans
}
```

```js [sol-JavaScript]
var maxProduct = function(nums) {
    let ans = -Infinity;
    let fMax = 1, fMin = 1;
    for (const x of nums) {
        if (x < 0) {
            // 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
            // 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 max 和 min 可以少一项
            [fMax, fMin] = [fMin, fMax];
        }
        fMax = Math.max(fMax * x, x);
        fMin = Math.min(fMin * x, x);
        ans = Math.max(ans, fMax);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_product(nums: Vec<i32>) -> i32 {
        let mut ans = i32::MIN;
        let mut f_max = 1;
        let mut f_min = 1;
        for x in nums {
            if x < 0 {
                // 下面与 x 相乘后，最大的正数变成最小的负数，最小的负数变成最大的正数
                // 提前交换，这样可以把 x < 0 和 x >= 0 的情况合并，合并后，计算 max 和 min 可以少一项
                (f_max, f_min) = (f_min, f_max);
            }
            f_max = x.max(f_max * x);
            f_min = x.min(f_min * x);
            ans = ans.max(f_max);
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 变形题

把子数组改成子序列，要怎么做？

- [2708. 一个小组的最大实力值](https://leetcode.cn/problems/maximum-strength-of-a-group/description/)

更多相似题目，见下面动态规划题单中的「**§1.3 最大子数组和**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
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
