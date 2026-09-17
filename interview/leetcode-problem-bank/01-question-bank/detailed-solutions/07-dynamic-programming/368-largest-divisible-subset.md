# 368. 最大整除子集

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/largest-divisible-subset/
- 题目 slug：`largest-divisible-subset`
- 来源专题：动态规划
- 来源分类路径：四、经典线性 DP / §4.2 最长递增子序列（LIS） / §4.2.2 进阶
- 难度分：1800
- 外部题解来源：https://leetcode.cn/problems/largest-divisible-subset/solutions/3641565/san-chong-fang-fa-ji-yi-hua-sou-suo-di-t-pift/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种方法：记忆化搜索/递推/最优性优化（Python/Java/C++/Go）](https://leetcode.cn/problems/largest-divisible-subset/solutions/3641565/san-chong-fang-fa-ji-yi-hua-sou-suo-di-t-pift/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`san-chong-fang-fa-ji-yi-hua-sou-suo-di-t-pift`
- topic id：`3641565`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 一、分析

设子集为 $A$，题目要求对于任意 $(A[i],A[j])$，都满足 $A[i]\bmod A[j] = 0$ 或者 $A[j]\bmod A[i] = 0$，也就是**一个数是另一个数的倍数**。

这里有两个条件，不好处理。我们可以把 $A$ 排序，或者说把 $\textit{nums}$ 排序（从小到大）。由于 $\textit{nums}$ 所有元素互不相同（没有相等的情况），题目要求变成：

- 从（排序后的）$\textit{nums}$ 中选一个**子序列**，在子序列中，右边的数一定是左边的数的倍数。

由于 $x$ 的倍数的倍数仍然是 $x$ 的倍数，只要相邻元素满足倍数关系，那么任意两数一定满足倍数关系。于是题目要求变成：

- 从（排序后的）$\textit{nums}$ 中选一个**子序列**，在子序列中，任意**相邻**的两个数，右边的数一定是左边的数的倍数。

这类似 [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/)，都是相邻元素有约束，且要计算的都是子序列的最长长度。

下文把满足题目要求的子序列叫做合法子序列。

## 二、记忆化搜索

先把 $\textit{nums}$ 从小到大排序。

⚠**注意**：排序不影响答案，因为题目说「如果存在多个有效解子集，返回其中任何一个均可」。如果 $[1,2]$ 合法，那么 $[2,1]$ 也合法。

仿照 300 题，定义 $\textit{dfs}(i)$ 表示以 $\textit{nums}[i]$ 结尾的合法子序列的最长长度。

枚举子序列倒数第二个数是 $\textit{nums}[j]$，如果 $\textit{nums}[i]\bmod \textit{nums}[j] = 0$，那么问题变成以 $\textit{nums}[j]$ 结尾的合法子序列的最长长度，即 $\textit{dfs}(i) = \textit{dfs}(j) + 1$。

取最大值，有

$$
\textit{dfs}(i) = \max_{j=0}^{i-1} \textit{dfs}(j) + 1
$$

其中 $\textit{nums}[i]\bmod \textit{nums}[j] = 0$。如果没有满足要求的 $j$，那么 $\textit{dfs}(i) = 1$，即 $\textit{nums}[i]$ 单独一个数作为子序列。

**递归边界**：$\textit{dfs}(0) = 1$。其实不需要判断递归边界，因为当 $i=0$ 的时候，不会进入循环，直接返回了。

**递归入口**：$\textit{dfs}(i)$。

> 注：从右往左递归，主要是为了方便把递归翻译成递推。从左往右递归也是可以的。

### 如何生成具体方案？

本题可以直接把合法子序列作为 $\textit{dfs}$ 的返回值，但这样做空间复杂度是 $\mathcal{O}(n^2)$ 的，且不是通用做法。

通用做法是，用一个数组 $\textit{from}$ 记录转移来源，初始值 $\textit{from}[i]=-1$。

如果 $\textit{dfs}(j)$ 是 $[0,i-1]$ 中的最大值，那么记录 $\textit{from}[i] = j$。

此外，记录最大的 $\textit{dfs}(i)$ 的下标 $\textit{maxI}$，也就是**最长**合法子序列的最后一个数的下标。

这样我们可以从 $i=\textit{maxI}$ 开始，顺着 $\textit{from}[i]$，找到合法子序列的倒数第二个数，倒数第三个数，……，第一个数。把找到的数记录到一个列表中，最后返回这个列表。

```py [sol-Python3]
class Solution:
    def largestDivisibleSubset(self, nums: List[int]) -> List[int]:
        nums.sort()
        n = len(nums)
        from_ = [-1] * n

        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int) -> int:
            res = 0
            for j in range(i):
                if nums[i] % nums[j]:
                    continue
                f = dfs(j)
                if f > res:
                    res = f
                    from_[i] = j  # 记录最佳转移来源
            return res + 1  # 加上 nums[i] 自己

        max_f = max_i = 0
        for i in range(n):
            f = dfs(i)
            if f > max_f:
                max_f = f
                max_i = i  # 最长合法子序列的最后一个数的下标

        path = []
        i = max_i
        while i >= 0:
            path.append(nums[i])
            i = from_[i]
        return path  # 不需要 reverse，任意顺序返回均可
```

```java [sol-Java]
class Solution {
    public List<Integer> largestDivisibleSubset(int[] nums) {
        Arrays.sort(nums);

        int n = nums.length;
        int[] memo = new int[n];
        int[] from = new int[n];
        Arrays.fill(from, -1);
        int maxF = 0;
        int maxI = 0;

        for (int i = 0; i < n; i++) {
            int f = dfs(i, nums, memo, from);
            if (f > maxF) {
                maxF = f;
                maxI = i; // 最长合法子序列的最后一个数的下标
            }
        }

        List<Integer> path = new ArrayList<>(maxF); // 预分配空间
        for (int i = maxI; i >= 0; i = from[i]) {
            path.add(nums[i]);
        }
        return path; // 不需要 reverse，任意顺序返回均可
    }

    private int dfs(int i, int[] nums, int[] memo, int[] from) {
        if (memo[i] > 0) { // 之前计算过
            return memo[i];
        }
        int res = 0;
        for (int j = 0; j < i; j++) {
            if (nums[i] % nums[j] != 0) {
                continue;
            }
            int f = dfs(j, nums, memo, from);
            if (f > res) {
                res = f;
                from[i] = j; // 记录最佳转移来源
            }
        }
        return memo[i] = res + 1; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> largestDivisibleSubset(vector<int>& nums) {
        ranges::sort(nums);
        int n = nums.size();
        vector<int> memo(n), from(n, -1);

        auto dfs = [&](this auto&& dfs, int i) -> int {
            int& res = memo[i]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }
            for (int j = 0; j < i; j++) {
                if (nums[i] % nums[j]) {
                    continue;
                }
                int f = dfs(j);
                if (f > res) {
                    res = f;
                    from[i] = j; // 记录最佳转移来源
                }
            }
            res++; // 加上 nums[i] 自己
            return res;
        };

        int max_f = 0, max_i = 0;
        for (int i = 0; i < n; i++) {
            int f = dfs(i);
            if (f > max_f) {
                max_f = f;
                max_i = i; // 最长合法子序列的最后一个数的下标
            }
        }

        vector<int> path;
        for (int i = max_i; i >= 0; i = from[i]) {
            path.push_back(nums[i]);
        }
        return path; // 不需要 reverse，任意顺序返回均可
    }
};
```

```go [sol-Go]
func largestDivisibleSubset(nums []int) []int {
    slices.Sort(nums)
    n := len(nums)
    memo := make([]int, n)
    from := make([]int, n)
    for i := range from {
        from[i] = -1
    }

    var dfs func(i int) int
    dfs = func(i int) (res int) {
        p := &memo[i]
        if *p > 0 { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        x := nums[i]
        for j, y := range nums[:i] {
            if x%y != 0 {
                continue
            }
            f := dfs(j)
            if f > res {
                res = f
                from[i] = j // 记录最佳转移来源
            }
        }
        return res + 1 // 加上 nums[i] 自己
    }

    maxF, maxI := 0, 0
    for i := range n {
        f := dfs(i)
        if f > maxF {
            maxF = f
            maxI = i // 最长合法子序列的最后一个数的下标
        }
    }

    path := make([]int, 0, maxF) // 预分配空间
    for i := maxI; i >= 0; i = from[i] {
        path = append(path, nums[i])
    }
    return path // 不需要 reverse，任意顺序返回均可
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{nums}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(n)$，所以总的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n)$。保存多少状态，就需要多少空间。

## 三、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i]$ 的定义和 $\textit{dfs}(i)$ 的定义是完全一样的，都表示以 $\textit{nums}[i]$ 结尾的合法子序列的最长长度。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i] = \max_{j=0}^{i-1} f[j] + 1
$$

其中 $\textit{nums}[i]\bmod \textit{nums}[j] = 0$。如果没有满足要求的 $j$，那么 $f[i] = 1$，即 $\textit{nums}[i]$ 单独一个数作为子序列。

```py [sol-Python3]
class Solution:
    def largestDivisibleSubset(self, nums: List[int]) -> List[int]:
        nums.sort()

        n = len(nums)
        f = [0] * n
        from_ = [-1] * n
        max_i = 0

        for i, x in enumerate(nums):
            for j in range(i):
                if x % nums[j] == 0 and f[j] > f[i]:
                    f[i] = f[j]
                    from_[i] = j  # 记录最佳转移来源
            f[i] += 1
            if f[i] > f[max_i]:
                max_i = i  # 最长合法子序列的最后一个数的下标

        path = []
        i = max_i
        while i >= 0:
            path.append(nums[i])
            i = from_[i]
        return path  # 不需要 reverse，任意顺序返回均可
```

```java [sol-Java]
class Solution {
    public List<Integer> largestDivisibleSubset(int[] nums) {
        Arrays.sort(nums);

        int n = nums.length;
        int[] f = new int[n];
        int[] from = new int[n];
        Arrays.fill(from, -1);
        int maxI = 0;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++) {
                if (nums[i] % nums[j] == 0 && f[j] > f[i]) {
                    f[i] = f[j];
                    from[i] = j; // 记录最佳转移来源
                }
            }
            f[i]++;
            if (f[i] > f[maxI]) {
                maxI = i; // 最长合法子序列的最后一个数的下标
            }
        }

        List<Integer> path = new ArrayList<>(f[maxI]); // 预分配空间
        for (int i = maxI; i >= 0; i = from[i]) {
            path.add(nums[i]);
        }
        return path; // 不需要 reverse，任意顺序返回均可
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> largestDivisibleSubset(vector<int>& nums) {
        ranges::sort(nums);

        int n = nums.size();
        vector<int> f(n), from(n, -1);
        int max_i = 0;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < i; j++) {
                if (nums[i] % nums[j] == 0 && f[j] > f[i]) {
                    f[i] = f[j];
                    from[i] = j; // 记录最佳转移来源
                }
            }
            f[i]++;
            if (f[i] > f[max_i]) {
                max_i = i; // 最长合法子序列的最后一个数的下标
            }
        }

        vector<int> path;
        for (int i = max_i; i >= 0; i = from[i]) {
            path.push_back(nums[i]);
        }
        return path; // 不需要 reverse，任意顺序返回均可
    }
};
```

```go [sol-Go]
func largestDivisibleSubset(nums []int) []int {
    slices.Sort(nums)

    n := len(nums)
    f := make([]int, n)
    from := make([]int, n)
    for i := range from {
        from[i] = -1
    }
    maxI := 0

    for i, x := range nums {
        for j, y := range nums[:i] {
            if x%y == 0 && f[j] > f[i] {
                f[i] = f[j]
                from[i] = j // 记录最佳转移来源
            }
        }
        f[i]++
        if f[i] > f[maxI] {
            maxI = i // 最长合法子序列的最后一个数的下标
        }
    }

    path := make([]int, 0, f[maxI]) // 预分配空间
    for i := maxI; i >= 0; i = from[i] {
        path = append(path, nums[i])
    }
    return path // 不需要 reverse，任意顺序返回均可
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 四、最优性优化

设 $\textit{nums}$ 的最大值为 $\textit{maxNum}$，目前计算出的最大的 $f[i]$ 为 $\textit{maxF}$。

假设 $\textit{maxNum}=16$，目前 $\textit{maxF}=3$，$\textit{nums}[i]=5$，算出的 $f[i]=2$。我们可以断定：这个 $f[i]$ 是**无用数据**，不需要在后续的内层循环中遍历它！

为什么？$\textit{nums}[i]=5$ 的不超过 $\textit{maxNum}$ 的倍数有 $10,15$，这些倍数的倍数，一定会超过 $\textit{maxNum}$，所以 $f[i]$ 的「增长潜力」只有 $1$。由于 $f[i]+1\le \textit{maxF}$，在后续的计算中，不可能更新 $\textit{maxF}$，所以 $f[i]$ 是无用数据。

一般地，把一个数 $x$ 不断地乘 $2$，要求结果 $\le \textit{maxNum}$，最多能乘多少次？

这相当于 $x\cdot 2^k\le \textit{maxNum}$，解得 $k\le \left\lfloor \log_2 \dfrac{\textit{maxNum}}{x}\right\rfloor$。

一般地，如果 

$$
f[i] + \left\lfloor \log_2 \dfrac{\textit{maxNum}}{\textit{nums}[i]}\right\rfloor \le \textit{maxF}
$$

那么这个 $f[i]$ 是无用数据，在后续的循环中，不需要遍历 $i$。

我们可以维护一个需要遍历的下标列表 $\textit{validIdx}$，把不满足上式的 $i$ 加进去。

此外，当 $\textit{maxF}$ 变大时，扫描 $\textit{validIdx}$，去掉无用数据。

> 注：这个做法类似搜索中的「最优性剪枝」：如果发现继续递归下去不可能更新答案，那么不继续往下递归。

```py [sol-Python3]
class Solution:
    def largestDivisibleSubset(self, nums: List[int]) -> List[int]:
        nums.sort()
        max_num = nums[-1]

        n = len(nums)
        f = [0] * n
        from_ = [-1] * n
        max_f = max_i = 0
        valid_idx = []

        for i, x in enumerate(nums):
            for j in valid_idx:  # 只需要遍历在 valid_idx 中的下标
                if x % nums[j] == 0 and f[j] > f[i]:
                    f[i] = f[j]
                    from_[i] = j
            f[i] += 1
            if f[i] > max_f:
                max_f = f[i]
                max_i = i
                # max_f 变大，去掉 valid_idx 中的无用数据（这里直接生成一个新的）
                new_valid_idx = []
                for j in valid_idx:
                    if f[j] + (max_num // nums[j]).bit_length() - 1 > max_f:
                        new_valid_idx.append(j)
                valid_idx = new_valid_idx
            if f[i] + (max_num // x).bit_length() - 1 > max_f:
                valid_idx.append(i)

        path = []
        i = max_i
        while i >= 0:
            path.append(nums[i])
            i = from_[i]
        return path
```

```java [sol-Java]
// 更快的写法见【Java 数组】
class Solution {
    public List<Integer> largestDivisibleSubset(int[] nums) {
        Arrays.sort(nums);
        int n = nums.length;
        int maxNum = nums[nums.length - 1];

        int[] f = new int[n];
        int[] from = new int[n];
        Arrays.fill(from, -1);
        int maxF = 0;
        int maxI = 0;
        List<Integer> validIdx = new ArrayList<>();

        for (int i = 0; i < n; i++) {
            int x = nums[i];
            for (int j : validIdx) { // 只需要遍历在 validIdx 中的下标
                if (x % nums[j] == 0 && f[j] > f[i]) {
                    f[i] = f[j];
                    from[i] = j;
                }
            }
            f[i]++;
            if (f[i] > maxF) {
                maxF = f[i];
                maxI = i;
                // maxF 变大，去掉 validIdx 中的无用数据（这里直接生成一个新的）
                List<Integer> newValidIdx = new ArrayList<>();
                for (int j : validIdx) {
                    if (f[j] + 31 - Integer.numberOfLeadingZeros(maxNum / nums[j]) > maxF) {
                        newValidIdx.add(j);
                    }
                }
                validIdx = newValidIdx;
            }
            if (f[i] + 31 - Integer.numberOfLeadingZeros(maxNum / x) > maxF) {
                validIdx.add(i);
            }
        }

        List<Integer> path = new ArrayList<>(maxF);
        for (int i = maxI; i >= 0; i = from[i]) {
            path.add(nums[i]);
        }
        return path;
    }
}
```

```java [sol-Java 数组]
class Solution {
    public List<Integer> largestDivisibleSubset(int[] nums) {
        Arrays.sort(nums);
        int n = nums.length;
        int maxNum = nums[nums.length - 1];

        int[] f = new int[n];
        int[] from = new int[n];
        Arrays.fill(from, -1);
        int maxF = 0;
        int maxI = 0;
        int[] validIdx = new int[n]; // 改成数组
        int m = 0; // validIdx 的大小

        for (int i = 0; i < n; i++) {
            int x = nums[i];
            for (int k = 0; k < m; k++) {
                int j = validIdx[k];
                if (x % nums[j] == 0 && f[j] > f[i]) {
                    f[i] = f[j];
                    from[i] = j;
                }
            }
            f[i]++;
            if (f[i] > maxF) {
                maxF = f[i];
                maxI = i;
                int m2 = 0;
                for (int k = 0; k < m; k++) {
                    int j = validIdx[k];
                    if (f[j] + 31 - Integer.numberOfLeadingZeros(maxNum / nums[j]) > maxF) {
                        validIdx[m2++] = j; // 原地修改
                    }
                }
                m = m2;
            }
            if (f[i] + 31 - Integer.numberOfLeadingZeros(maxNum / x) > maxF) {
                validIdx[m++] = i;
            }
        }

        List<Integer> path = new ArrayList<>(maxF);
        for (int i = maxI; i >= 0; i = from[i]) {
            path.add(nums[i]);
        }
        return path;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> largestDivisibleSubset(vector<int>& nums) {
        ranges::sort(nums);
        unsigned max_num = nums.back();

        int n = nums.size();
        vector<int> f(n), from(n, -1);
        int max_f = 0, max_i = 0;
        vector<int> valid_idx;

        for (int i = 0; i < n; i++) {
            for (int j : valid_idx) { // 只需要遍历在 valid_idx 中的下标
                if (nums[i] % nums[j] == 0 && f[j] > f[i]) {
                    f[i] = f[j];
                    from[i] = j;
                }
            }
            f[i]++;
            if (f[i] > max_f) {
                max_f = f[i];
                max_i = i;
                // max_f 变大，去掉 valid_idx 中的无用数据（这里直接生成一个新的）
                vector<int> new_valid_idx;
                for (int j : valid_idx) {
                    if (f[j] + bit_width(max_num / nums[j]) - 1 > max_f) {
                        new_valid_idx.push_back(j);
                    }
                }
                valid_idx = move(new_valid_idx);
            }
            if (f[i] + bit_width(max_num / nums[i]) - 1 > max_f) {
                valid_idx.push_back(i);
            }
        }

        vector<int> path;
        for (int i = max_i; i >= 0; i = from[i]) {
            path.push_back(nums[i]);
        }
        return path;
    }
};
```

```go [sol-Go]
func largestDivisibleSubset(nums []int) []int {
    slices.Sort(nums)
    n := len(nums)
    maxNum := nums[n-1]

    f := make([]int, n)
    from := make([]int, n)
    for i := range from {
        from[i] = -1
    }
    maxF, maxI := 0, 0
    validIdx := []int{}

    for i, x := range nums {
        for _, j := range validIdx { // 只需要遍历在 validIdx 中的下标
            if x%nums[j] == 0 && f[j] > f[i] {
                f[i] = f[j]
                from[i] = j
            }
        }
        f[i]++
        if f[i] > maxF {
            maxF = f[i]
            maxI = i
            // maxF 变大，去掉 validIdx 中的无用数据
            newValidIdx := validIdx[:0]
            for _, j := range validIdx {
                if f[j]+bits.Len(uint(maxNum/nums[j]))-1 > maxF {
                    newValidIdx = append(newValidIdx, j)
                }
            }
            validIdx = newValidIdx
        }
        if f[i]+bits.Len(uint(maxNum/x))-1 > maxF {
            validIdx = append(validIdx, i)
        }
    }

    path := make([]int, 0, maxF)
    for i := maxI; i >= 0; i = from[i] {
        path = append(path, nums[i])
    }
    return path
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

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
