# 494. 目标和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/target-sum/
- 题目 slug：`target-sum`
- 来源专题：动态规划
- 来源分类路径：三、背包 / §3.1 0-1 背包
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/target-sum/solutions/2119041/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-s1cx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：0-1 背包 / 折半枚举（Python/Java/C++/Go）](https://leetcode.cn/problems/target-sum/solutions/2119041/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-s1cx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-s1cx`
- topic id：`2119041`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 本题视频讲解

请看[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)。如果这个视频对你有帮助，欢迎点赞关注~

## 一、递归搜索 + 保存计算结果 = 记忆化搜索

### 视频内容补充

设 $\textit{nums}$ 的元素和为 $S$，其中添加正号的元素之和为 $p$，其余添加负号的元素（绝对值）之和为 $q$，那么有

$$
p + q = S
$$

又因为表达式运算结果等于 $\textit{target}$，所以有

$$
p - q = \textit{target}
$$

解得

$$
\begin{cases}
p = \dfrac{S + \textit{target}}{2}     \\[2ex]
q = \dfrac{S - \textit{target}}{2}     \\[1ex]
\end{cases}
$$

这意味着，只要我们选出的取正号的元素和恰好等于 $p$，或者取负号的元素和恰好等于 $q$，就**等价于**表达式的结果恰好等于 $\textit{target}$。所以问题变成从 $\textit{nums}$ 中选一些数，使得这些数的元素和恰好等于 $p$（或者 $q$）的方案数。

- 如果 $\textit{target}\ge 0$，那么 $q\le p$，取 $q = \dfrac{S - \textit{target}}{2}$ 作为背包容量更好。
- 如果 $\textit{target}< 0$，那么 $p < q$，取 $p = \dfrac{S + \textit{target}}{2} = \dfrac{S - |\textit{target}|}{2}$ 作为背包容量更好。

综上所述，取 

$$
\dfrac{S - |\textit{target}|}{2}
$$

作为 0-1 背包的背包容量是最优的。（注意 $\textit{target}$ 可以是负数）

设 $s = S - |\textit{target}|$，有两种情况需要特判，可以直接返回 $0$：

- 如果 $s < 0$，由于 $\textit{nums}[i]\ge 0$，我们无法得到负数，方案数是 $0$。
- 如果 $s$ 是奇数，由于 $\textit{nums}[i]$ 都是整数，我们无法得到非整数 $\dfrac{s}{2}$，方案数是 $0$。

### 答疑

**问**：`dfs(i - 1, c) + dfs(i - 1, c - nums[i])` 中的加法是什么意思？

**答**：这叫**加法原理**，如果事件 A 和事件 B 是互斥的（即不能同时发生，不选 $\textit{nums}[i]$ 的同时，又选了 $\textit{nums}[i]$），那么发生事件 A **或**事件 B 的总数等于事件 A 的数量加上事件 B 的数量。

**问**：为什么不能在 $c=0$ 时就**提前返回** $1$？

**答**：注意本题 $\textit{nums}$ 中有 $0$，这些 $0$ 选或不选都可以。提前返回相当于剩下的 $0$ 全都不选，只找到了一个方案，这会导致算出的结果比正确答案小。反过来说，如果 $\textit{nums}$ 中的元素都是正数，那么可以在 $c=0$ 时返回 $1$。

```py [sol-Python3]
class Solution:
    def findTargetSumWays(self, nums: List[int], target: int) -> int:
        s = sum(nums) - abs(target)
        if s < 0 or s % 2:
            return 0

        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int, c: int) -> int:
            if i < 0:
                return 1 if c == 0 else 0
            if c < nums[i]:
                return dfs(i - 1, c)  # 只能不选
            return dfs(i - 1, c) + dfs(i - 1, c - nums[i])  # 不选 + 选

        m = s // 2  # 背包容量
        return dfs(len(nums) - 1, m)
```

```java [sol-Java]
class Solution {
    public int findTargetSumWays(int[] nums, int target) {
        int s = 0;
        for (int x : nums) {
            s += x;
        }

        s -= Math.abs(target);
        if (s < 0 || s % 2 == 1) {
            return 0;
        }

        int m = s / 2; // 背包容量
        int n = nums.length;
        int[][] memo = new int[n][m + 1];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }

        return dfs(n - 1, m, nums, memo);
    }

    private int dfs(int i, int c, int[] nums, int[][] memo) {
        if (i < 0) {
            return c == 0 ? 1 : 0;
        }
        if (memo[i][c] != -1) { // 之前计算过
            return memo[i][c];
        }
        int res = dfs(i - 1, c, nums, memo); // 不选 nums[i]
        if (c >= nums[i]) {
            res += dfs(i - 1, c - nums[i], nums, memo); // 选 nums[i]
        }
        return memo[i][c] = res;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findTargetSumWays(vector<int>& nums, int target) {
        int s = reduce(nums.begin(), nums.end()) - abs(target);
        if (s < 0 || s % 2) {
            return 0;
        }

        int m = s / 2; // 背包容量
        int n = nums.size();
        vector memo(n, vector<int>(m + 1, -1)); // -1 表示没有计算过

        // lambda 递归函数
        auto dfs = [&](this auto&& dfs, int i, int c) -> int {
            if (i < 0) {
                return c == 0;
            }
            int& res = memo[i][c]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            if (c < nums[i]) {
                return res = dfs(i - 1, c); // 只能不选
            }
            return res = dfs(i - 1, c) + dfs(i - 1, c - nums[i]); // 不选 + 选
        };

        return dfs(n - 1, m);
    }
};
```

```go [sol-Go]
func findTargetSumWays(nums []int, target int) int {
    s := 0
    for _, x := range nums {
        s += x
    }

    s -= abs(target)
    if s < 0 || s%2 == 1 {
        return 0
    }

    m := s / 2 // 背包容量
    n := len(nums)
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, m+1)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }

    var dfs func(int, int) int
    dfs = func(i, c int) (res int) {
        if i < 0 {
            if c == 0 {
                return 1
            }
            return 0
        }
        p := &memo[i][c]
        if *p != -1 { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if c < nums[i] {
            return dfs(i-1, c) // 只能不选
        }
        return dfs(i-1, c) + dfs(i-1, c-nums[i]) // 不选 + 选
    }

    return dfs(n-1, m)
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 为 $\textit{nums}$ 的长度，$m$ 为 $\textit{nums}$ 的元素和减去 $|\textit{target}|$（$1/2$ 的常系数可以忽略）。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(nm)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(nm)$。
- 空间复杂度：$\mathcal{O}(nm)$。保存多少状态，就需要多少空间。

## 二、1:1 翻译成递推

原理见本文开头的视频。

```py [sol-Python3]
class Solution:
    def findTargetSumWays(self, nums: List[int], target: int) -> int:
        s = sum(nums) - abs(target)
        if s < 0 or s % 2:
            return 0

        m = s // 2  # 背包容量
        n = len(nums)
        f = [[0] * (m + 1) for _ in range(n + 1)]
        f[0][0] = 1

        for i, x in enumerate(nums):
            for c in range(m + 1):
                if c < x:
                    f[i + 1][c] = f[i][c]  # 只能不选
                else:
                    f[i + 1][c] = f[i][c] + f[i][c - x]  # 不选 + 选

        return f[n][m]
```

```java [sol-Java]
class Solution {
    public int findTargetSumWays(int[] nums, int target) {
        int s = 0;
        for (int x : nums) {
            s += x;
        }

        s -= Math.abs(target);
        if (s < 0 || s % 2 == 1) {
            return 0;
        }

        int m = s / 2; // 背包容量
        int n = nums.length;
        int[][] f = new int[n + 1][m + 1];
        f[0][0] = 1;

        for (int i = 0; i < n; i++) {
            for (int c = 0; c <= m; c++) {
                if (c < nums[i]) {
                    f[i + 1][c] = f[i][c]; // 只能不选
                } else {
                    f[i + 1][c] = f[i][c] + f[i][c - nums[i]]; // 不选 + 选
                }
            }
        }

        return f[n][m];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findTargetSumWays(vector<int>& nums, int target) {
        int s = reduce(nums.begin(), nums.end()) - abs(target);
        if (s < 0 || s % 2) {
            return 0;
        }

        int m = s / 2; // 背包容量
        int n = nums.size();
        vector f(n + 1, vector<int>(m + 1));
        f[0][0] = 1;

        for (int i = 0; i < n; i++) {
            for (int c = 0; c <= m; c++) {
                if (c < nums[i]) {
                    f[i + 1][c] = f[i][c]; // 只能不选
                } else {
                    f[i + 1][c] = f[i][c] + f[i][c - nums[i]]; // 不选 + 选
                }
            }
        }

        return f[n][m];
    }
};
```

```go [sol-Go]
func findTargetSumWays(nums []int, target int) int {
    s := 0
    for _, x := range nums {
        s += x
    }

    s -= abs(target)
    if s < 0 || s%2 == 1 {
        return 0
    }

    m := s / 2 // 背包容量
    n := len(nums)
    f := make([][]int, n+1)
    for i := range f {
        f[i] = make([]int, m+1)
    }
    f[0][0] = 1

    for i, x := range nums {
        for c := 0; c <= m; c++ {
            if c < x {
                f[i+1][c] = f[i][c] // 只能不选
            } else {
                f[i+1][c] = f[i][c] + f[i][c-x] // 不选 + 选
            }
        }
    }

    return f[n][m]
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 为 $\textit{nums}$ 的长度，$m$ 为 $\textit{nums}$ 的元素和减去 $|\textit{target}|$。
- 空间复杂度：$\mathcal{O}(nm)$。

## 三、空间优化：两个数组（滚动数组）

```py [sol-Python3]
class Solution:
    def findTargetSumWays(self, nums: List[int], target: int) -> int:
        s = sum(nums) - abs(target)
        if s < 0 or s % 2:
            return 0

        m = s // 2  # 背包容量
        n = len(nums)
        f = [[0] * (m + 1) for _ in range(2)]
        f[0][0] = 1

        for i, x in enumerate(nums):
            for c in range(m + 1):
                if c < x:
                    f[(i + 1) % 2][c] = f[i % 2][c]  # 只能不选
                else:
                    f[(i + 1) % 2][c] = f[i % 2][c] + f[i % 2][c - x]  # 不选 + 选

        return f[n % 2][m]
```

```java [sol-Java]
class Solution {
    public int findTargetSumWays(int[] nums, int target) {
        int s = 0;
        for (int x : nums) {
            s += x;
        }

        s -= Math.abs(target);
        if (s < 0 || s % 2 == 1) {
            return 0;
        }

        int m = s / 2; // 背包容量
        int n = nums.length;
        int[][] f = new int[2][m + 1];
        f[0][0] = 1;

        for (int i = 0; i < n; i++) {
            for (int c = 0; c <= m; c++) {
                if (c < nums[i]) {
                    f[(i + 1) % 2][c] = f[i % 2][c]; // 只能不选
                } else {
                    f[(i + 1) % 2][c] = f[i % 2][c] + f[i % 2][c - nums[i]]; // 不选 + 选
                }
            }
        }

        return f[n % 2][m];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findTargetSumWays(vector<int>& nums, int target) {
        int s = reduce(nums.begin(), nums.end()) - abs(target);
        if (s < 0 || s % 2) {
            return 0;
        }

        int m = s / 2; // 背包容量
        int n = nums.size();
        vector f(2, vector<int>(m + 1));
        f[0][0] = 1;

        for (int i = 0; i < n; i++) {
            for (int c = 0; c <= m; c++) {
                if (c < nums[i]) {
                    f[(i + 1) % 2][c] = f[i % 2][c]; // 只能不选
                } else {
                    f[(i + 1) % 2][c] = f[i % 2][c] + f[i % 2][c - nums[i]]; // 不选 + 选
                }
            }
        }

        return f[n % 2][m];
    }
};
```

```go [sol-Go]
func findTargetSumWays(nums []int, target int) int {
    s := 0
    for _, x := range nums {
        s += x
    }

    s -= abs(target)
    if s < 0 || s%2 == 1 {
        return 0
    }

    m := s / 2 // 背包容量
    n := len(nums)
    f := make([][]int, 2)
    for i := range f {
        f[i] = make([]int, m+1)
    }
    f[0][0] = 1

    for i, x := range nums {
        for c := 0; c <= m; c++ {
            if c < x {
                f[(i+1)%2][c] = f[i%2][c] // 只能不选
            } else {
                f[(i+1)%2][c] = f[i%2][c] + f[i%2][c-x] // 不选 + 选
            }
        }
    }

    return f[n%2][m]
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 为 $\textit{nums}$ 的长度，$m$ 为 $\textit{nums}$ 的元素和减去 $|\textit{target}|$。
- 空间复杂度：$\mathcal{O}(m)$。

## 四、空间优化：一个数组

想象你在一面墙上画画，原来这面墙画的是 $f[i]$，现在要画一副新的画，把原来的画**覆盖**掉，新的画叫做 $f[i+1]$。

在循环的过程中：

- 对于 $c<x$ 的状态，转移方程是 $f[i+1][c] = f[i][c]$，这说明原来画的内容保持不变，空间优化后是 $f[c] = f[c]$，这个赋值是多余的。所以可以从 $c=x$ 开始循环。
- 对于 $c\ge x$ 的状态，转移方程是 $f[i+1][c] = f[i][c] + f[i][c-x]$，其中 $f[i][c]$ 可以就地取材，我们重点关注 $f[i][c-x]$，这是旧画的内容，需要从旧画的下标 $c-x$ 处取到，这意味着我们**必须按照下标从大到小的顺序画画**，才能保证画到 $f[i+1][c]$ 的时候，旧画 $f[i][c-x]$ 还在墙上。空间优化后是 $f[c] = f[c] + f[c - x]$。

```py [sol-Python3]
class Solution:
    def findTargetSumWays(self, nums: List[int], target: int) -> int:
        s = sum(nums) - abs(target)
        if s < 0 or s % 2:
            return 0

        m = s // 2  # 背包容量
        f = [1] + [0] * m
        for x in nums:
            for c in range(m, x - 1, -1):
                f[c] += f[c - x]
        return f[m]
```

```java [sol-Java]
class Solution {
    public int findTargetSumWays(int[] nums, int target) {
        int s = 0;
        for (int x : nums) {
            s += x;
        }

        s -= Math.abs(target);
        if (s < 0 || s % 2 == 1) {
            return 0;
        }

        int m = s / 2; // 背包容量
        int[] f = new int[m + 1];
        f[0] = 1;
        for (int x : nums) {
            for (int c = m; c >= x; c--) {
                f[c] += f[c - x];
            }
        }
        return f[m];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findTargetSumWays(vector<int>& nums, int target) {
        int s = reduce(nums.begin(), nums.end()) - abs(target);
        if (s < 0 || s % 2) {
            return 0;
        }

        int m = s / 2; // 背包容量
        vector<int> f(m + 1);
        f[0] = 1;
        for (int x : nums) {
            for (int c = m; c >= x; c--) {
                f[c] += f[c - x];
            }
        }
        return f[m];
    }
};
```

```go [sol-Go]
func findTargetSumWays(nums []int, target int) int {
    s := 0
    for _, x := range nums {
        s += x
    }

    s -= abs(target)
    if s < 0 || s%2 == 1 {
        return 0
    }

    m := s / 2
    f := make([]int, m+1)
    f[0] = 1
    for _, x := range nums {
        for c := m; c >= x; c-- {
            f[c] += f[c-x]
        }
    }
    return f[m]
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 为 $\textit{nums}$ 的长度，$m$ 为 $\textit{nums}$ 的元素和减去 $|\textit{target}|$。
- 空间复杂度：$\mathcal{O}(m)$。

## 五、另一种方法：折半枚举

如果 $\textit{nums}[i]$ 的范围大到 $10^9$，上面的方法就超时了。怎么办？

最暴力的方法是用 [78. 子集](https://leetcode.cn/problems/subsets/) 的方法求出 $\textit{nums}$ 的所有子集和，但这需要 $\mathcal{O}(2^n)$ 时间，有些慢。

设 $k = \left\lfloor\dfrac{n}{2}\right\rfloor$，把 $\textit{nums}$ 均分，得到长为 $k$ 和 $n-k$ 的两部分 $A$ 和 $B$。

求出 $A$ 的所有子集和，把子集和及其出现次数记录在一个哈希表 $\textit{cnt}_1$ 中。同理，求出 $B$ 的所有子集和，把子集和及其出现次数记录在一个哈希表 $\textit{cnt}_2$ 中。

现在问题变成：

- 从 $\textit{cnt}_1$ 中选一个子集和 $x$，从 $\textit{cnt}_2$ 中选一个子集和 $y$，满足 $x+y=m$ 的方案数。

做法类似 [1. 两数之和](https://leetcode.cn/problems/two-sum/)，枚举 $x$，问题变成在 $\textit{cnt}_2$ 中找 $m-x$。

有 $\textit{cnt}_1[x]$ 个 $x$ 和 $\textit{cnt}_2[m-x]$ 个 $y$，根据乘法原理，有

$$
\textit{cnt}_1[x]\cdot \textit{cnt}_2[m-x]
$$

个方案，满足 $x+y=m$。加到答案中。

下面代码用**二进制枚举**求子集和，原理见 [我的题解](https://leetcode.cn/problems/subsets/solutions/2059409/hui-su-bu-hui-xie-tao-lu-zai-ci-pythonja-8tkl/) 的方法三。

```py [sol-Python3]
class Solution:
    # 78. 子集（二进制枚举写法）
    def subsets(self, nums: List[int]) -> Dict[int, int]:
        cnt = defaultdict(int)
        for i in range(1 << len(nums)):
            s = sum(x for j, x in enumerate(nums) if i >> j & 1)
            cnt[s] += 1
        return cnt

    def findTargetSumWays(self, nums: List[int], target: int) -> int:
        s = sum(nums) - abs(target)
        if s < 0 or s % 2:
            return 0

        m = s // 2
        k = len(nums) // 2
        cnt1 = self.subsets(nums[:k])
        cnt2 = self.subsets(nums[k:])

        return sum(c1 * cnt2[m - x] for x, c1 in cnt1.items())
```

```java [sol-Java]
class Solution {
    public int findTargetSumWays(int[] nums, int target) {
        int s = 0;
        for (int x : nums) {
            s += x;
        }

        s -= Math.abs(target);
        if (s < 0 || s % 2 == 1) {
            return 0;
        }

        int m = s / 2;
        int k = nums.length / 2;
        Map<Integer, Integer> cnt1 = subsets(Arrays.copyOfRange(nums, 0, k));
        Map<Integer, Integer> cnt2 = subsets(Arrays.copyOfRange(nums, k, nums.length));

        int ans = 0;
        for (Map.Entry<Integer, Integer> e : cnt1.entrySet()) {
            int x = e.getKey();
            int c1 = e.getValue();
            ans += c1 * cnt2.getOrDefault(m - x, 0);
        }
        return ans;
    }

    // 78. 子集（二进制枚举写法）
    private Map<Integer, Integer> subsets(int[] nums) {
        Map<Integer, Integer> cnt = new HashMap<>();
        int n = nums.length;
        for (int i = 0; i < (1 << n); i++) {
            int s = 0;
            for (int j = 0; j < n; j++) {
                if ((i >> j & 1) == 1) {
                    s += nums[j];
                }
            }
            cnt.merge(s, 1, Integer::sum); // cnt[s]++
        }
        return cnt;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 78. 子集（二进制枚举写法）
    unordered_map<int, int> subsets(const vector<int>& nums) {
        unordered_map<int, int> cnt;
        int n = nums.size();
        for (int i = 0; i < (1 << n); i++) {
            int s = 0;
            for (int j = 0; j < n; j++) {
                if (i >> j & 1) {
                    s += nums[j];
                }
            }
            cnt[s]++;
        }
        return cnt;
    }

public:
    int findTargetSumWays(vector<int>& nums, int target) {
        int s = reduce(nums.begin(), nums.end()) - abs(target);
        if (s < 0 || s % 2) {
            return 0;
        }

        int m = s / 2;
        int k = nums.size() / 2;
        auto cnt1 = subsets({nums.begin(), nums.begin() + k});
        auto cnt2 = subsets({nums.begin() + k, nums.end()});

        int ans = 0;
        for (auto& [x, c1] : cnt1) {
            ans += c1 * cnt2[m - x];
        }
        return ans;
    }
};
```

```go [sol-Go]
// 78. 子集（二进制枚举写法）
func subsets(nums []int) map[int]int {
    cnt := map[int]int{}
    for i := range 1 << len(nums) {
        s := 0
        for j, x := range nums {
            if i>>j&1 == 1 {
                s += x
            }
        }
        cnt[s]++
    }
    return cnt
}

func findTargetSumWays(nums []int, target int) (ans int) {
    s := 0
    for _, x := range nums {
        s += x
    }

    s -= abs(target)
    if s < 0 || s%2 == 1 {
        return 0
    }

    m := s / 2
    k := len(nums) / 2
    cnt1 := subsets(nums[:k])
    cnt2 := subsets(nums[k:])

    for x, c1 := range cnt1 {
        ans += c1 * cnt2[m-x]
    }
    return ans
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n2^{n/2})$，其中 $n$ 为 $\textit{nums}$ 的长度。这里为了方便，求所有子集和用的二进制枚举。可以用回溯或者状压 DP 优化至 $\mathcal{O}(2^{n/2})$。
- 空间复杂度：$\mathcal{O}(2^{n/2})$。

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

本题来自 `三、背包 / §3.1 0-1 背包`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `三、背包 / §3.1 0-1 背包`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
