# 1262. 可被三整除的最大和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/greatest-sum-divisible-by-three/
- 题目 slug：`greatest-sum-divisible-by-three`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：1762
- 外部题解来源：https://leetcode.cn/problems/greatest-sum-divisible-by-three/solutions/2313700/liang-chong-suan-fa-tan-xin-dong-tai-gui-tsll/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种算法：贪心/动态规划（Python/Java/C++/Go）](https://leetcode.cn/problems/greatest-sum-divisible-by-three/solutions/2313700/liang-chong-suan-fa-tan-xin-dong-tai-gui-tsll/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-suan-fa-tan-xin-dong-tai-gui-tsll`
- topic id：`2313700`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：贪心

由于数组中没有负数，如果整个数组的元素和 $s$ 可以被 $3$ 整除，那么 $s$ 就是最大的元素和。

否则，如果 $s$ 不能被 $3$ 整除，那就看看能否让 $s$ 减去某些 $\textit{nums}[i]$，使得 $s$ 可以被 $3$ 整除。

找到所有 $\textit{nums}[i]\bmod 3 = 1$ 的 $\textit{nums}[i]$，放到数组 $a_1$ 中；找到所有 $\textit{nums}[i]\bmod 3 = 2$ 的 $\textit{nums}[i]$，放到数组 $a_2$ 中。

对 $a_1$ 和 $a_2$ 从小到大排序。分类讨论：

- 如果 $s\bmod 3 = 1$：
   - 如果 $a_1$ 不为空，那么答案可能是 $s-a_1[0]$；
   - 如果 $a_2$ 中至少有两个数，那么答案可能是 $s-a_2[0]-a_2[1]$；
   - 这两种情况取最大值。
   - 如果没有这样的数，返回 $0$。
- 如果 $s\bmod 3 = 2$：
    - 如果 $a_2$ 不为空，那么答案可能是 $s-a_2[0]$；
    - 如果 $a_1$ 中至少有两个数，那么答案可能是 $s-a_1[0]-a_1[1]$；
    - 这两种情况取最大值。
    - 如果没有这样的数，返回 $0$。

代码实现时，如果 $s\bmod 3 = 2$，那么可以**交换**数组 $a_1$ 和 $a_2$，从而复用同一套逻辑。

但是，贪心算法是有局限的。试想一下，如果把题目中的 $3$ 换成 $4$，要如何分类讨论？换成 $5$，又要如何分类讨论？随着数字的变大，要讨论的内容越来越复杂。那么，是否有更加通用的做法呢？请继续阅读。

```py [sol-Python3]
class Solution:
    def maxSumDivThree(self, nums: List[int]) -> int:
        s = sum(nums)
        if s % 3 == 0:
            return s
        a1 = sorted(x for x in nums if x % 3 == 1)
        a2 = sorted(x for x in nums if x % 3 == 2)
        if s % 3 == 2:
            a1, a2 = a2, a1
        ans = s - a1[0] if a1 else 0
        if len(a2) > 1:
            ans = max(ans, s - a2[0] - a2[1])
        return ans
```

```java [sol-Java]
class Solution {
    public int maxSumDivThree(int[] nums) {
        int s = 0;
        for (int x : nums)
            s += x;
        if (s % 3 == 0)
            return s;

        var a1 = new ArrayList<Integer>();
        var a2 = new ArrayList<Integer>();
        for (int x : nums) {
            if (x % 3 == 1) a1.add(x);
            else if (x % 3 == 2) a2.add(x);
        }
        Collections.sort(a1);
        Collections.sort(a2);

        if (s % 3 == 2) { // swap(a1,a2)
            var tmp = a1;
            a1 = a2;
            a2 = tmp;
        }
        int ans = a1.isEmpty() ? 0 : s - a1.get(0);
        if (a2.size() > 1)
            ans = Math.max(ans, s - a2.get(0) - a2.get(1));
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSumDivThree(vector<int> &nums) {
        int s = accumulate(nums.begin(), nums.end(), 0);
        if (s % 3 == 0)
            return s;

        vector<int> a[3];
        for (int x: nums)
            a[x % 3].push_back(x);
        sort(a[1].begin(), a[1].end());
        sort(a[2].begin(), a[2].end());

        if (s % 3 == 2)
            swap(a[1], a[2]);
        int ans = a[1].size() ? s - a[1][0] : 0;
        if (a[2].size() > 1)
            ans = max(ans, s - a[2][0] - a[2][1]);
        return ans;
    }
};
```

```go [sol-Go]
func maxSumDivThree(nums []int) (ans int) {
    s := 0
    for _, x := range nums {
        s += x
    }
    if s%3 == 0 {
        return s
    }

    a := [3][]int{}
    for _, x := range nums {
        a[x%3] = append(a[x%3], x)
    }
    sort.Ints(a[1])
    sort.Ints(a[2])

    if s%3 == 2 {
        a[1], a[2] = a[2], a[1]
    }
    if len(a[1]) > 0 {
        ans = s - a[1][0]
    }
    if len(a[2]) > 1 {
        ans = max(ans, s-a[2][0]-a[2][1])
    }
    return
}

func max(a, b int) int { if a < b { return b }; return a }
```

#### 复杂度分析

注：由于只要最小的两个数，可以做到 $\mathcal{O}(n)$ 时间和 $\mathcal{O}(1)$ 额外空间，但写起来较为复杂。考虑到动态规划的做法同样可以做到 $\mathcal{O}(n)$ 时间和 $\mathcal{O}(1)$ 额外空间，所以这里就不进一步优化了。

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：动态规划

### 前置知识：动态规划入门

详见 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

### 一、寻找子问题

用「选或不选」的思路，考虑最后一个数 $x = \textit{nums}[n-1]$：

- 如果 $x\bmod 3= 0$，选 $x$ 不影响结果，一定要选（不选白不选），问题变成从 $\textit{nums}[0]$ 到 $\textit{nums}[n-2]$ 中寻找能被 $3$ 整除的元素最大和。你也可以从结果的角度考虑，结果是 $3$ 的倍数，如果 $\textit{nums}$ 中有一个 $3$ 的倍数没有选，岂不是白白浪费？
- 如果 $x\bmod 3= 1$：
   - 如果不选 $x$，和上面一样，问题变成从 $\textit{nums}[0]$ 到 $\textit{nums}[n-2]$ 中寻找能被 $3$ 整除的元素最大和 $s_0$。
   - 如果选 $x$，问题变成从 $\textit{nums}[0]$ 到 $\textit{nums}[n-2]$ 中寻找最大元素和 $s_2$，满足 $s_2\bmod 3 = 2$。
   - 答案为 $\max(s_0, s_2 + x)$。
- 如果 $x\bmod 3= 2$：
    - 如果不选 $x$，和上面一样，问题变成从 $\textit{nums}[0]$ 到 $\textit{nums}[n-2]$ 中寻找能被 $3$ 整除的元素最大和 $s_0$。
    - 如果选 $x$，问题变成从 $\textit{nums}[0]$ 到 $\textit{nums}[n-2]$ 中寻找最大元素和 $s_1$，满足 $s_1\bmod 3 = 1$。
    - 答案为 $\max(s_0, s_1 + x)$。

上述讨论，刻画了这道题的两个重要参数：

- $i$：表示从 $\textit{nums}[0]$ 到 $\textit{nums}[i]$ 中选数。
- $j$：表示所选数字之和 $s$ 需要满足 $s\bmod 3 = j$。

那么原问题就是 $(i=n-1, j=0)$，上述讨论得到的子问题有 $(i=n-2,j=0),\ (i=n-2,j=1),\ (i=n-2,j=2)$。

> 注：为什么要从最后一个数开始讨论？主要是为了方便后面把记忆化搜索改成递推。当然，从第一个数开始讨论也是可以的。

### 二、状态定义与状态转移方程

根据上面的讨论，定义 $\textit{dfs}(i,j)$ 表示从 $\textit{nums}[0]$ 到 $\textit{nums}[i]$ 中选数，**后续还需要选**的数字之和 $s$ 满足 $s\bmod 3 = j$ 的前提下，$s$ 的最大值。

设 $x=\textit{nums}[i]$，分类讨论：

- 如果不选 $x$，问题变成从 $\textit{nums}[0]$ 到 $\textit{nums}[i-1]$ 中选数，所选数字之和 $s$ 满足 $s\bmod 3 = j$ 的前提下，$s$ 的最大值。即 $\textit{dfs}(i,j) = \textit{dfs}(i-1,j)$。
- 如果选 $x$，问题变成从 $\textit{nums}[0]$ 到 $\textit{nums}[i-1]$ 中选数，所选数字之和 $s$ 满足 $(s+x)\bmod 3 = j$，即 $s\bmod 3 = (j-x)\bmod 3$ 的前提下，$s$ 的最大值。即 $\textit{dfs}(i,j) = \textit{dfs}(i-1,(j-x)\bmod 3) + x$。

这两种情况取最大值，有 

$$
\textit{dfs}(i,j) = \max(\textit{dfs}(i-1,j),\textit{dfs}(i-1,(j-x)\bmod 3) + x)
$$

注意，如果 $(j-x)\bmod 3 < 0$，需要再 $+3$ 调整到 $[0,2]$ 内。考虑到这样写有些麻烦，不妨把 $j$ 的定义改为**已选**数字之和 $\bmod\ 3 = j$。（注意这里的定义是已经选的，上面定义的是还需要选的。）

这样修改后，不选 $x$ 仍然是 $\textit{dfs}(i,j) = \textit{dfs}(i-1,j)$；选 $x$ 就是 $\textit{dfs}(i,j) = \textit{dfs}(i-1,(j+x)\bmod 3)$ 了。

这两种情况取最大值，有

$$
\textit{dfs}(i,j) = \max(\textit{dfs}(i-1,j),\textit{dfs}(i-1,(j+x)\bmod 3) + x)
$$

递归边界：$\textit{dfs}(-1,0)=0,\textit{dfs}(-1,1)=-\infty,\textit{dfs}(-1,2)=-\infty$。我们需要保证所选数字之和是 $3$ 的倍数，否则不符合题目要求。注意，如果没有选任何数字，那么会递归到 $\textit{dfs}(-1,0)$，得到 $0$，这是符合题目要求的。

> 注：$-\infty$ 表示非法方案，这样后面取 $\max$ 的时候会自动排除非法方案。

递归入口：$\textit{dfs}(n-1,0)$。

```py [sol-Python3]
class Solution:
    def maxSumDivThree(self, nums: List[int]) -> int:
        @cache  # 记忆化搜索
        def dfs(i: int, j: int) -> int:
            if i < 0: return -inf if j else 0
            return max(dfs(i - 1, j), dfs(i - 1, (j + nums[i]) % 3) + nums[i])
        return dfs(len(nums) - 1, 0)
```

```java [sol-Java]
class Solution {
    public int maxSumDivThree(int[] nums) {
        int n = nums.length;
        int[][] memo = new int[n][3];
        for (int i = 0; i < n; i++)
            Arrays.fill(memo[i], -1); // -1 表示没有计算过
        return dfs(memo, nums, n - 1, 0);
    }

    private int dfs(int[][] memo, int[] nums, int i, int j) {
        if (i < 0) return j == 0 ? 0 : Integer.MIN_VALUE;
        if (memo[i][j] != -1) return memo[i][j]; // 之前计算过
        return memo[i][j] = Math.max(dfs(memo, nums, i - 1, j),
                dfs(memo, nums, i - 1, (j + nums[i]) % 3) + nums[i]);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSumDivThree(vector<int> &nums) {
        int n = nums.size(), memo[n][3];
        memset(memo, -1, sizeof(memo)); // -1 表示没有计算过
        auto dfs = [&](auto&& dfs, int i, int j) -> int {
            if (i < 0) return j ? INT_MIN : 0;
            int &res = memo[i][j]; // 注意这里是引用，下面会直接修改 memo[i][j]
            if (res != -1) return res; // 之前计算过
            return res = max(dfs(dfs, i - 1, j), dfs(dfs, i - 1, (j + nums[i]) % 3) + nums[i]);
        };
        return dfs(dfs, n - 1, 0);
    }
};
```

```go [sol-Go]
func maxSumDivThree(nums []int) int {
    n := len(nums)
    memo := make([][3]int, n)
    for i := range memo {
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i < 0 {
            if j == 0 {
                return 0
            }
            return math.MinInt
        }
        p := &memo[i][j]
        if *p != -1 { // 之前计算过
            return *p
        }
        *p = max(dfs(i-1, j), dfs(i-1, (j+nums[i])%3)+nums[i])
        return *p
    }
    return dfs(n-1, 0)
}

func max(a, b int) int { if a < b { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk)$，其中 $n$ 为 $\textit{nums}$ 的长度，$k=3$。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(nk)$，单个状态的计算时间为 $\mathcal{O}(1)$，因此时间复杂度为 $\mathcal{O}(nk)$。
- 空间复杂度：$\mathcal{O}(nk)$。

### 三、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

做法：

- $\textit{dfs}$ 改成 $f$ 数组；
- 递归改成循环（每个参数都对应一层循环）；
- 递归边界改成 $f$ 数组的初始值。

具体来说，$f[i][j]$ 的含义与状态转移方程和 $\textit{dfs}(i,j)$ 的是一致的，即

$$
f[i][j] = \max(f[i-1][j],f[i-1][(j+x)\bmod 3] + x)
$$

但当 $i=0$ 时，等号右边会出现负数下标。或者说，**这种定义方式没有状态能表示递归边界**。

解决办法：在 $f$ 数组的上边加一排，把原来的 $f[i]$ 改成 $f[i+1]$，$f[i-1]$ 改成 $f[i]$。此时 $f[0][j]$ 就对应着 $\textit{dfs}(-1,j)$。

修改后的递推式为

$$
f[i+1][j] = \max(f[i][j],f[i][(j+x)\bmod 3] + x)
$$

初始值 $f[0]=[0,-\infty,-\infty]$，翻译自 $\textit{dfs}(-1,0)=0,\textit{dfs}(-1,1)=-\infty,\textit{dfs}(-1,2)=-\infty$。

答案为 $f[n][0]$，翻译自 $\textit{dfs}(n-1,0)$。

```py [sol-Python3]
class Solution:
    def maxSumDivThree(self, nums: List[int]) -> int:
        f = [[-inf] * 3 for _ in range(len(nums) + 1)]
        f[0][0] = 0
        for i, x in enumerate(nums):
            for j in range(3):
                f[i + 1][j] = max(f[i][j], f[i][(j + x) % 3] + x)
        return f[-1][0]
```

```java [sol-Java]
class Solution {
    public int maxSumDivThree(int[] nums) {
        int n = nums.length;
        var f = new int[n + 1][3];
        f[0][1] = f[0][2] = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < 3; j++)
                f[i + 1][j] = Math.max(f[i][j], f[i][(j + nums[i]) % 3] + nums[i]);
        return f[n][0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSumDivThree(vector<int> &nums) {
        int n = nums.size(), f[n + 1][3];
        f[0][0] = 0, f[0][1] = INT_MIN, f[0][2] = INT_MIN;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < 3; j++)
                f[i + 1][j] = max(f[i][j], f[i][(j + nums[i]) % 3] + nums[i]);
        return f[n][0];
    }
};
```

```go [sol-Go]
func maxSumDivThree(nums []int) int {
    n := len(nums)
    f := make([][3]int, n+1)
    f[0][1] = math.MinInt
    f[0][2] = math.MinInt
    for i, x := range nums {
        for j := 0; j < 3; j++ {
            f[i+1][j] = max(f[i][j], f[i][(j+x)%3]+x)
        }
    }
    return f[n][0]
}

func max(a, b int) int { if a < b { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk)$，其中 $n$ 为 $\textit{nums}$ 的长度，$k=3$。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(nk)$，单个状态的计算时间为 $\mathcal{O}(1)$，因此时间复杂度为 $\mathcal{O}(nk)$。
- 空间复杂度：$\mathcal{O}(nk)$。

### 四、用滚动数组优化空间

由于 $f[i+1]$ 只依赖 $f[i]$，那么 $f[i-1]$ 及其之前的数据就没用了。

例如计算 $f[2]$ 的时候，数组 $f[0]$ 不再使用了。

那么干脆把 $f[2]$ 填到 $f[0]$ 中。同理，把 $f[3]$ 填到 $f[1]$ 中，$f[4]$ 填到 $f[0]$ 中，……

因此可以只用两个长为 $n+1$ 的数组滚动计算。

具体可以看[【基础算法精讲 18】](https://www.bilibili.com/video/BV16Y411v7Y6/)中的画图讲解。

```py [sol-Python3]
class Solution:
    def maxSumDivThree(self, nums: List[int]) -> int:
        f = [[-inf] * 3 for _ in range(2)]
        f[0][0] = 0
        for i, x in enumerate(nums):
            for j in range(3):
                f[(i + 1) % 2][j] = max(f[i % 2][j], f[i % 2][(j + x) % 3] + x)
        return f[len(nums) % 2][0]
```

```java [sol-Java]
class Solution {
    public int maxSumDivThree(int[] nums) {
        int n = nums.length;
        var f = new int[2][3];
        f[0][1] = f[0][2] = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < 3; j++)
                f[(i + 1) % 2][j] = Math.max(f[i % 2][j], f[i % 2][(j + nums[i]) % 3] + nums[i]);
        return f[n % 2][0];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxSumDivThree(vector<int> &nums) {
        int n = nums.size(), f[2][3];
        f[0][0] = 0, f[0][1] = INT_MIN, f[0][2] = INT_MIN;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < 3; j++)
                f[(i + 1) % 2][j] = max(f[i % 2][j], f[i % 2][(j + nums[i]) % 3] + nums[i]);
        return f[n % 2][0];
    }
};
```

```go [sol-Go]
func maxSumDivThree(nums []int) int {
    f := [2][3]int{{0, math.MinInt, math.MinInt}}
    for i, x := range nums {
        for j := 0; j < 3; j++ {
            f[(i+1)%2][j] = max(f[i%2][j], f[i%2][(j+x)%3]+x)
        }
    }
    return f[len(nums)%2][0]
}

func max(a, b int) int { if a < b { return b }; return a }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk)$，其中 $n$ 为 $\textit{nums}$ 的长度，$k=3$。
- 空间复杂度：$\mathcal{O}(k)$。

## 总结

相比贪心算法，动态规划的适用性更广。请你思考，如果数组中有负数，动态规划是否也能得到正确的结果？如果把 $3$ 换成其它数字呢？欢迎在评论区发表你的看法。

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

本题来自 `七、其他线性 DP / §7.6 多维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.6 多维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
