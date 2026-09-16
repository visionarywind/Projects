# 2786. 访问数组中的位置使分数最大

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/visit-array-positions-to-maximize-score/
- 题目 slug：`visit-array-positions-to-maximize-score`
- 来源专题：动态规划
- 来源分类路径：六、状态机 DP / §6.2 基础
- 难度分：1733
- 外部题解来源：https://leetcode.cn/problems/visit-array-positions-to-maximize-score/solutions/2810386/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-jhvr/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推到空间优化！（Python/Java/C++/Go）](https://leetcode.cn/problems/visit-array-positions-to-maximize-score/solutions/2810386/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-jhvr/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-jhvr`
- topic id：`2810386`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意概括

选一个 $\textit{nums}$ 的子序列，得分为子序列的元素和，再减去 $k\cdot x$，其中 $k$ 是子序列中的奇偶性不同的相邻元素对的个数。

## 一、寻找子问题

看示例 1，$\textit{nums}=[2,3,6,1,9,2]$。

子序列的第一个数是 $\textit{nums}[0]=2$，这是一个偶数。然后要解决的问题有两种：

- 在下标 $[1, n-1]$ 中选一个子序列，其第一个数的奇偶性与 $\textit{nums}[0]$ 相同（偶数）。
- 在下标 $[1, n-1]$ 中选一个子序列，其第一个数的奇偶性与 $\textit{nums}[0]$ 不同（奇数）。

考虑 $\textit{nums}[1]=3$ 选或不选：

- 如果要解决的问题是第一种（选偶数），由于不符合要求，不选，问题变成：在下标 $[2, n-1]$ 中选一个子序列，其第一个数是偶数。
- 如果要解决的问题是第二种（选奇数），符合要求，那么一定要选。为什么？因为 $\textit{nums}$ 中的元素都是正数，假如不选更优，那么在最终子序列的任意奇数前插入 $\textit{nums}[1]=3$，不会减去 $x$，而又增加了得分，矛盾，所以一定要选。选之后，接下来要解决的问题，也有两种：
    - 在下标 $[2, n-1]$ 中选一个子序列，其第一个数的奇偶性与 $\textit{nums}[1]$ 相同（奇数）。
    - 在下标 $[2, n-1]$ 中选一个子序列，其第一个数的奇偶性与 $\textit{nums}[1]$ 不同（偶数）。
    
这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

## 二、状态定义与状态转移方程

因为要解决的问题都形如「在下标 $[i, n-1]$ 中选一个子序列，其第一个数的奇偶性为 $j$（也就是模 $2$ 的结果为 $j$）时的最大得分」，所以用它作为本题的状态定义 $\textit{dfs}(i,j)$。

考虑 $v=\textit{nums}[i]$ 选或不选：

- 如果 $v\bmod 2\ne j$，由于不符合要求，不选，问题变成：在下标 $[i+1, n-1]$ 中选一个子序列，其第一个数的奇偶性为 $j$ 时的最大得分，即 $\textit{dfs}(i,j)= \textit{dfs}(i+1,j)$。
- 如果 $v\bmod 2=j$，根据上文的讨论，一定要选，接下来要解决的问题有两种：
    - 在下标 $[i+1, n-1]$ 中选一个子序列，其第一个数的奇偶性为 $j$ 时的最大得分，即 $\textit{dfs}(i,j)=\textit{dfs}(i+1,j)+v$。
    - 在下标 $[i+1, n-1]$ 中选一个子序列，其第一个数的奇偶性为 $j\oplus 1$ 时的最大得分，即 $\textit{dfs}(i,j)=\textit{dfs}(i+1,j\oplus 1)-x+v$。其中 $j\oplus 1$ 表示奇偶性不等于 $j$，即 $j=0$ 时要选一个奇偶性为 $1$ 的数，$j=1$ 时要选一个奇偶性为 $0$ 的数。减去 $x$ 是因为奇偶性不同。
    - 这两种情况取最大值，就得到了 $\textit{dfs}(i,j)$，即

$$
\textit{dfs}(i,j) = \max(\textit{dfs}(i+1,j), \textit{dfs}(i+1,j\oplus 1)-x) + v
$$

递归边界：$\textit{dfs}(n,j)=0$。没有元素可以选了，得分为 $0$。

递归入口：$\textit{dfs}(0,\textit{nums}[0]\bmod 2)$，也就是答案。注意 $\textit{nums}[0]$ 一定要选。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含如何把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def maxScore(self, nums: List[int], x: int) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int, j: int) -> int:
            if i == len(nums):
                return 0
            if nums[i] % 2 != j:
                return dfs(i + 1, j)
            return max(dfs(i + 1, j), dfs(i + 1, j ^ 1) - x) + nums[i]
        return dfs(0, nums[0] % 2)
```

```java [sol-Java]
class Solution {
    public long maxScore(int[] nums, int x) {
        int n = nums.length;
        long[][] memo = new long[n][2];
        for (long[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return dfs(0, nums[0] % 2, nums, x, memo);
    }

    private long dfs(int i, int j, int[] nums, int x, long[][] memo) {
        if (i == nums.length) {
            return 0;
        }
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }
        if (nums[i] % 2 != j) {
            return memo[i][j] = dfs(i + 1, j, nums, x, memo);
        }
        long res1 = dfs(i + 1, j, nums, x, memo);
        long res2 = dfs(i + 1, j ^ 1, nums, x, memo);
        return memo[i][j] = Math.max(res1, res2 - x) + nums[i];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxScore(vector<int>& nums, int x) {
        int n = nums.size();
        vector<array<long long, 2>> memo(n, {-1, -1}); // -1 表示没有计算过
        auto dfs = [&](auto&& dfs, int i, int j) -> long long {
            if (i == n) {
                return 0;
            }
            long long& res = memo[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            if (nums[i] % 2 != j) {
                return res = dfs(dfs, i + 1, j);
            }
            return res = max(dfs(dfs, i + 1, j), dfs(dfs, i + 1, j ^ 1) - x) + nums[i];
        };
        return dfs(dfs, 0, nums[0] % 2);
    }
};
```

```go [sol-Go]
func maxScore(nums []int, x int) int64 {
    n := len(nums)
    memo := make([][2]int, n)
    for i := range memo {
        memo[i] = [2]int{-1, -1} // -1 表示没有计算过
    }
    var dfs func(int, int) int
    dfs = func(i, j int) (res int) {
        if i == n {
            return
        }
        p := &memo[i][j]
        if *p != -1 { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if nums[i]%2 != j {
            return dfs(i+1, j)
        }
        return max(dfs(i+1, j), dfs(i+1, j^1)-x) + nums[i]
    }
    return int64(dfs(0, nums[0]%2))
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示在下标 $[i, n-1]$ 中选一个子序列，其第一个数的奇偶性为 $j$（也就是模 $2$ 的结果为 $j$）时的最大得分。

设 $v=\textit{nums}[i]$，相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i][j] =
\begin{cases}
f[i+1][j],\ &v\bmod 2 \ne j\\
\max(f[i+1][j], f[i+1][j\oplus 1]-x) + v,\ &v\bmod 2 = j
\end{cases}
$$

初始值 $f[n][j]=0$，翻译自递归边界 $\textit{dfs}(n,j)=0$。

答案为 $f[0][\textit{nums}[0]\bmod 2]$，翻译自递归入口 $\textit{dfs}(0, \textit{nums}[0]\bmod 2)$。

```py [sol-Python3]
class Solution:
    def maxScore(self, nums: List[int], x: int) -> int:
        n = len(nums)
        f = [[0, 0] for _ in range(n + 1)]
        for i in range(n - 1, -1, -1):
            v = nums[i]
            r = v % 2
            f[i][r ^ 1] = f[i + 1][r ^ 1]  # v%2 != j 的情况
            f[i][r] = max(f[i + 1][r], f[i + 1][r ^ 1] - x) + v
        return f[0][nums[0] % 2]
```

```java [sol-Java]
class Solution {
    public long maxScore(int[] nums, int x) {
        int n = nums.length;
        long[][] f = new long[n + 1][2];
        for (int i = n - 1; i >= 0; i--) {
            int v = nums[i];
            int r = v % 2;
            f[i][r ^ 1] = f[i + 1][r ^ 1]; // v%2 != j 的情况
            f[i][r] = Math.max(f[i + 1][r], f[i + 1][r ^ 1] - x) + v;
        }
        return f[0][nums[0] % 2];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxScore(vector<int>& nums, int x) {
        int n = nums.size();
        vector<array<long long, 2>> f(n + 1);
        for (int i = n - 1; i >= 0; i--) {
            int v = nums[i];
            int r = v % 2;
            f[i][r ^ 1] = f[i + 1][r ^ 1]; // v%2 != j 的情况
            f[i][r] = max(f[i + 1][r], f[i + 1][r ^ 1] - x) + v;
        }
        return f[0][nums[0] % 2];
    }
};
```

```go [sol-Go]
func maxScore(nums []int, x int) int64 {
    n := len(nums)
    f := make([][2]int, n+1)
    for i := n - 1; i >= 0; i-- {
        v := nums[i]
        r := v % 2
        f[i][r^1] = f[i+1][r^1] // v%2 != j 的情况
        f[i][r] = max(f[i+1][r], f[i+1][r^1]-x) + v
    }
    return int64(f[0][nums[0]%2])
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 五、空间优化：一个数组

观察上面的状态转移方程，在计算 $f[i]$ 时，只会用到 $f[i+1]$，不会用到比 $i+1$ 更大的状态。

因此可以去掉第一个维度，反复利用同一个长为 $2$ 的小数组。

状态转移方程改为，当 $v\bmod 2 = j$ 时，有

$$
f[j] = \max(f[j], f[j\oplus 1]-x) + v
$$

注意 $v\bmod 2 \ne j$ 的情况无需计算，因为化简后就是 $f[j]=f[j]$。 

初始值 $f[j]=0$。

答案为 $f[\textit{nums}[0]\bmod 2]$。

```py [sol-Python3]
class Solution:
    def maxScore(self, nums: List[int], x: int) -> int:
        f = [0, 0]
        for v in reversed(nums):
            r = v % 2
            f[r] = max(f[r], f[r ^ 1] - x) + v
        return f[nums[0] % 2]
```

```py [sol-Python3]
class Solution:
    def maxScore(self, nums: List[int], x: int) -> int:
        f = [0, 0]
        for v in reversed(nums):
            r = v % 2
            # 手写 max，速度更快
            t = f[r ^ 1] - x
            if f[r] > t:
                t = f[r]
            f[r] = t + v
        return f[nums[0] % 2]
```

```java [sol-Java]
class Solution {
    public long maxScore(int[] nums, int x) {
        long[] f = new long[2];
        for (int i = nums.length - 1; i >= 0; i--) {
            int v = nums[i];
            int r = v & 1; // 比 % 2 快一点
            f[r] = Math.max(f[r], f[r ^ 1] - x) + v;
        }
        return f[nums[0] & 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxScore(vector<int>& nums, int x) {
        long long f[2]{};
        for (int i = nums.size() - 1; i >= 0; i--) {
            int v = nums[i];
            int r = v % 2;
            f[r] = max(f[r], f[r ^ 1] - x) + v;
        }
        return f[nums[0] % 2];
    }
};
```

```go [sol-Go]
func maxScore(nums []int, x int) int64 {
    f := [2]int{}
    for i := len(nums) - 1; i >= 0; i-- {
        v := nums[i]
        r := v % 2
        f[r] = max(f[r], f[r^1]-x) + v
    }
    return int64(f[nums[0]%2])
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 注

本题如果不要求 $\textit{nums}[0]$ 必选，那么正着递推和倒着递推没有区别。

在 $\textit{nums}[0]$ 必选的情况下，相比正着递推，倒着递推的初始值和返回值都更加简单。读者可以仔细对比这两种写法的区别，加深对 DP 初始值和返回值的理解。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `六、状态机 DP / §6.2 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、状态机 DP / §6.2 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
