# 1186. 删除一次得到子数组最大和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-subarray-sum-with-one-deletion/
- 题目 slug：`maximum-subarray-sum-with-one-deletion`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.3 最大子数组和（最大子段和）
- 难度分：1799
- 外部题解来源：https://leetcode.cn/problems/maximum-subarray-sum-with-one-deletion/solutions/2321829/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-hzz6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考动态规划：从记忆化搜索到递推（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-subarray-sum-with-one-deletion/solutions/2321829/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-hzz6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-hzz6`
- topic id：`2321829`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识：动态规划入门

详见 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)

## 一、启发思考：寻找子问题

最暴力的做法是枚举子数组的左右端点，以及要删除的元素。这种做法显然会超时。

保留「枚举子数组的右端点」这一想法，看看有没有可以优化的地方。

对于示例 1 中的数组 $[1,-2,0,3]$，假设元素和最大的连续子数组的右端点下标是 $3$。那么原问题为：「子数组的右端点下标是 $3$，且**至多**删除一个数，子数组元素和的最大值是多少？」

这个原问题可以拆分成两个问题：

1. 子数组的右端点下标是 $3$，且**不能**删除数字，子数组元素和的最大值是多少？
   - 如果子数组长度为 $1$，那么子数组就是 $[3]$。
   - 如果子数组长度大于 $1$，那么需要解决的问题为：「子数组的右端点下标是 $2$，且不能删除数字，子数组元素和的最大值是多少？」
2. 子数组的右端点下标是 $3$，且**必须**删除一个数字，子数组元素和的最大值是多少？
   - 如果子数组只有 $3$，那么必须删除 $3$，但这违背了题目要求：「（删除后）子数组中至少应当有一个元素」，所以**不考虑这种情况**。
   - 如果不删除 $3$，那么需要解决的问题为：「子数组的右端点下标是 $2$，且必须删除一个数字，子数组元素和的最大值是多少？」
   - 如果删除 $3$，那么需要解决的问题为：「子数组的右端点下标是 $2$，且不能删除数字，子数组元素和的最大值是多少？」

上面说的三个需要解决的问题，都是**和原问题相似的、规模更小的子问题**，所以可以用递归解决。

> 注 1：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。
> 
> 注 2：动态规划有「选或不选」和「枚举选哪个」这两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。上面用到的是「选或不选」。

## 二、递归怎么写：状态定义与状态转移方程

根据上面的讨论，递归参数需要一个 $i$，表示子数组的右端点下标是 $i$。此外，需要知道是否可以删除数字，所以递归参数还需要一个 $j$。其中 $j=0$ 表示不能删除数字，$j=1$ 表示必须删除一个数。

因此，定义 $\textit{dfs}(i,j)$ 表示子数组的右端点下标是 $i$，不能/必须删除数字的情况下，子数组元素和的最大值。

根据上面讨论出的子问题，可以得到：

- 如果 $j=0$（不能删除）：
  - 如果不考虑 $\textit{arr}[i]$ 左边的数，那么 $\textit{dfs}(i,0)=\textit{arr}[i]$。
  - 如果考虑 $\textit{arr}[i]$ 左边的数，那么 $\textit{dfs}(i,0)=\textit{dfs}(i-1,0) + \textit{arr}[i]$。
- 如果 $j=1$（必须删除）：
  - 如果不删除 $\textit{arr}[i]$，那么 $\textit{dfs}(i,1)=\textit{dfs}(i-1,1)+\textit{arr}[i]$。
  - 如果删除 $\textit{arr}[i]$，那么 $\textit{dfs}(i,1)=\textit{dfs}(i-1,0)$。

取最大值，就得到了 $\textit{dfs}(i,j)$。写成式子就是

$$
\begin{aligned}
&\textit{dfs}(i,0) = \max(\textit{dfs}(i-1,0), 0) + \textit{arr}[i] \\
&\textit{dfs}(i,1) = \max(\textit{dfs}(i-1,1) + \textit{arr}[i], \textit{dfs}(i-1,0))
\end{aligned}
$$

递归边界：$\textit{dfs}(-1,j)=-\infty$。这里 $-1$ 表示子数组中「没有数字」，但题目要求子数组不能为空，所以这种情况不合法，用 $-\infty$ 表示，这样取 $\max$ 的时候就自然会取到合法的情况。

递归入口：$\textit{dfs}(i,j)$。枚举子数组右端点 $i$，以及是否需要删除数字 $j=0,1$，取所有结果的最大值，作为答案。

> **注**：根据状态定义，$\textit{dfs}(i,1)$ 会在删除 $\textit{arr}[k]$ 的那一刻，从 $\textit{dfs}(k-1,0)$ 转移过来。由于 $\textit{dfs}(k-1,0)$ 对应一个非空的子数组，所以我们删除的 $\textit{arr}[k]$ 一定不是子数组的第一个数。**推论**：$\textit{dfs}(1,1)$ 不可能等于 $\textit{arr}[1]$（如果等于，则意味着我们删除了 $\textit{arr}[0]$，矛盾）。

## 三、递归 + 记录返回值 = 记忆化搜索

以 $[1,-2,0,3]$ 为例。在计算 $\textit{dfs}(3,1)$ 时，「删除 $3$，保留 $0$」和「保留 $3$，删除 $0$」，都会递归到 $\textit{dfs}(1,0)$。

一叶知秋，整个递归中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组（或哈希表）中。
- 如果一个状态不是第一次遇到，那么直接返回 $\textit{memo}$ 中保存的结果。

```py [sol-Python3]
class Solution:
    def maximumSum(self, arr: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int, j: int) -> int:
            if i < 0:
                return -inf  # 子数组至少要有一个数，不合法
            if j == 0:
                return max(dfs(i - 1, 0), 0) + arr[i]
            return max(dfs(i - 1, 1) + arr[i], dfs(i - 1, 0))

        return max(max(dfs(i, 0), dfs(i, 1)) for i in range(len(arr)))
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] arr) {
        int n = arr.length;
        int[][] memo = new int[n][2];
        for (int[] row : memo) {
            Arrays.fill(row, Integer.MIN_VALUE);
        }

        int ans = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++) {
            ans = Math.max(ans, Math.max(dfs(i, 0, arr, memo), dfs(i, 1, arr, memo)));
        }
        return ans;
    }

    private int dfs(int i, int j, int[] arr, int[][] memo) {
        if (i < 0) {
            return Integer.MIN_VALUE / 2; // 除 2 防止负数相加溢出
        }
        if (memo[i][j] != Integer.MIN_VALUE) {
            return memo[i][j]; // 之前计算过
        }
        if (j == 0) {
            return memo[i][j] = Math.max(dfs(i - 1, 0, arr, memo), 0) + arr[i];
        }
        return memo[i][j] = Math.max(dfs(i - 1, 1, arr, memo) + arr[i], dfs(i - 1, 0, arr, memo));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& arr) {
        int n = arr.size();
        vector<array<int, 2>> memo(n + 1, {INT_MIN, INT_MIN});

        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i < 0) {
                return INT_MIN / 2; // 除 2 防止负数相加溢出
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != INT_MIN) {
                return res; // 之前计算过
            }
            if (j == 0) {
                return res = max(dfs(i - 1, 0), 0) + arr[i];
            }
            return res = max(dfs(i - 1, 1) + arr[i], dfs(i - 1, 0));
        };

        int ans = INT_MIN;
        for (int i = 0; i < n; i++) {
            ans = max(ans, max(dfs(i, 0), dfs(i, 1)));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(arr []int) int {
    memo := make([][2]int, len(arr))
    for i := range memo {
        memo[i] = [2]int{math.MinInt, math.MinInt}
    }

    var dfs func(int, int) int
    dfs = func(i, j int) (res int) {
        if i < 0 {
            return math.MinInt / 2 // 除 2 防止负数相加溢出
        }
        p := &memo[i][j]
        if *p != math.MinInt { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if j == 0 {
            return max(dfs(i-1, 0), 0) + arr[i]
        }
        return max(dfs(i-1, 1)+arr[i], dfs(i-1, 0))
    }

    ans := math.MinInt
    for i := range arr {
        ans = max(ans, dfs(i, 0), dfs(i, 1))
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

做法：

- $\textit{dfs}$ 改成 $f$ 数组。
- 递归改成循环（每个参数都对应一层循环）。这里 $j$ 只有 $0$ 和 $1$，可以直接计算，无需循环 $j$。
- 递归边界改成 $f$ 数组的初始值。

> 相当于之前是用递归去计算每个状态，现在是（按照某种顺序）枚举并计算每个状态。

具体来说，$f[i][j]$ 的含义和 $\textit{dfs}(i,j)$ 的含义是一致的，都表示子数组的右端点下标是 $i$，不能/必须删除数字的情况下，子数组元素和的最大值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 的一致：

$$
\begin{aligned}
&f[i][0] = \max(f[i-1][0], 0) + \textit{arr}[i] \\
&f[i][1] = \max(f[i-1][1] + \textit{arr}[i], f[i-1][0])
\end{aligned}
$$

但是，这种定义方式**没有状态能表示递归边界**，即 $i=-1$ 的情况。

解决办法：把 $f$ 数组的长度加一，用 $f[0][j]$ 表示 $\textit{dfs}(-1,j)$。由于 $f[0]$ 被占用，原来的下标 $i$ 需要全部向右偏移一位，也就是 $f[i]$ 改为 $f[i+1]$，$f[i-1]$ 改为 $f[i]$。

修改后 $f[i+1]$ 表示子数组的右端点下标是 $i$，不能/必须删除数字的情况下，子数组元素和的最大值。

修改后的递推式为

$$
\begin{aligned}
&f[i+1][0] = \max(f[i][0], 0) + \textit{arr}[i] \\
&f[i+1][1] = \max(f[i][1] + \textit{arr}[i], f[i][0])
\end{aligned}
$$

> 问：为什么 $\textit{arr}$ 的下标不用变？
>
> 答：既然是把 $f$ 数组的长度加一，那么就只需要修改和 $f$ 有关的下标，其余任何逻辑都无需修改。

初始值 $f[0][j]=-\infty$，翻译自 $\textit{dfs}(-1,j)=-\infty$。

答案为所有 $f[i][j]$ 的最大值。

```py [sol-Python3]
class Solution:
    def maximumSum(self, arr: List[int]) -> int:
        f = [[-inf] * 2] + [[0, 0] for _ in arr]
        for i, x in enumerate(arr):
            f[i + 1][0] = max(f[i][0], 0) + x
            f[i + 1][1] = max(f[i][1] + x, f[i][0])
        return max(max(r) for r in f)
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] arr) {
        int n = arr.length;
        int[][] f = new int[n + 1][2];
        Arrays.fill(f[0], Integer.MIN_VALUE / 2); // 除 2 防止负数相加溢出
        int ans = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = Math.max(f[i][0], 0) + arr[i];
            f[i + 1][1] = Math.max(f[i][1] + arr[i], f[i][0]);
            ans = Math.max(ans, Math.max(f[i + 1][0], f[i + 1][1]));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& arr) {
        int n = arr.size();
        vector<array<int, 2>> f(n + 1, {INT_MIN / 2, INT_MIN / 2}); // 除 2 防止负数相加溢出
        int ans = INT_MIN;
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = max(f[i][0], 0) + arr[i];
            f[i + 1][1] = max(f[i][1] + arr[i], f[i][0]);
            ans = max(ans, max(f[i + 1][0], f[i + 1][1]));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(arr []int) int {
    ans := math.MinInt
    f := make([][2]int, len(arr)+1)
    f[0] = [2]int{math.MinInt / 2, math.MinInt / 2} // 除 2 防止负数相加溢出
    for i, x := range arr {
        f[i+1][0] = max(f[i][0], 0) + x
        f[i+1][1] = max(f[i][1]+x, f[i][0])
        ans = max(ans, f[i+1][0], f[i+1][1])
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 五、空间优化

观察上面的状态转移方程，在计算 $f[i+1]$ 时，只会用到 $f[i]$，不会用到下标 $< i$ 的状态。

因此只需要两个状态表示 $j=0,1$。

状态转移方程改为

$$
\begin{aligned}
&f[1] = \max(f[1] + \textit{arr}[i], f[0])\\
&f[0] = \max(f[0], 0) + \textit{arr}[i] 
\end{aligned}
$$

**请注意计算顺序**！必须先算 $f[1]$ 再算 $f[0]$。如果先算 $f[0]$ 再算 $f[1]$，那么在计算 $f[1]$ 时，相当于用到的不是原来的 $f[i][0]$，而是新算出来的 $f[i+1][0]$。

初始值 $f[j]=-\infty$。

一边计算，一边维护 $f[j]$ 的最大值，作为答案。

```py [sol-Python3]
class Solution:
    def maximumSum(self, arr: List[int]) -> int:
        ans = f0 = f1 = -inf
        for x in arr:
            f1 = max(f1 + x, f0)  # 注：手动 if 比大小会更快 
            f0 = max(f0, 0) + x
            ans = max(ans, f0, f1)
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] arr) {
        int ans = Integer.MIN_VALUE / 2;
        int f0 = ans;
        int f1 = ans;
        for (int x : arr) {
            f1 = Math.max(f1 + x, f0);
            f0 = Math.max(f0, 0) + x;
            ans = Math.max(ans, Math.max(f0, f1));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& arr) {
        int ans = INT_MIN / 2, f0 = ans, f1 = ans;
        for (int x : arr) {
            f1 = max(f1 + x, f0);
            f0 = max(f0, 0) + x;
            ans = max(ans, max(f0, f1));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(arr []int) int {
    ans := math.MinInt / 2
    f0, f1 := ans, ans
    for _, x := range arr {
        f1 = max(f1+x, f0)
        f0 = max(f0, 0) + x
        ans = max(ans, f0, f1)
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。只用到常数级的额外空间。

## 思考题

如果改成至多删除 $k$ 次，要怎么做？

改成恰好删除 $k$ 次呢？

改成至少删除 $k$ 次呢？

欢迎在评论区发表你的思路/代码。

## 专题训练

见下面动态规划题单的「**六、状态机 DP**」。

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

## 前置知识：动态规划入门

详见 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)

## 一、启发思考：寻找子问题

最暴力的做法是枚举子数组的左右端点，以及要删除的元素。这种做法显然会超时。

保留「枚举子数组的右端点」这一想法，看看有没有可以优化的地方。

对于示例 1 中的数组 $[1,-2,0,3]$，假设元素和最大的连续子数组的右端点下标是 $3$。那么原问题为：「子数组的右端点下标是 $3$，且**至多**删除一个数，子数组元素和的最大值是多少？」

这个原问题可以拆分成两个问题：

1. 子数组的右端点下标是 $3$，且**不能**删除数字，子数组元素和的最大值是多少？
   - 如果子数组长度为 $1$，那么子数组就是 $[3]$。
   - 如果子数组长度大于 $1$，那么需要解决的问题为：「子数组的右端点下标是 $2$，且不能删除数字，子数组元素和的最大值是多少？」
2. 子数组的右端点下标是 $3$，且**必须**删除一个数字，子数组元素和的最大值是多少？
   - 如果子数组只有 $3$，那么必须删除 $3$，但这违背了题目要求：「（删除后）子数组中至少应当有一个元素」，所以**不考虑这种情况**。
   - 如果不删除 $3$，那么需要解决的问题为：「子数组的右端点下标是 $2$，且必须删除一个数字，子数组元素和的最大值是多少？」
   - 如果删除 $3$，那么需要解决的问题为：「子数组的右端点下标是 $2$，且不能删除数字，子数组元素和的最大值是多少？」

上面说的三个需要解决的问题，都是**和原问题相似的、规模更小的子问题**，所以可以用递归解决。

> 注 1：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。
> 
> 注 2：动态规划有「选或不选」和「枚举选哪个」这两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。上面用到的是「选或不选」。

## 二、递归怎么写：状态定义与状态转移方程

根据上面的讨论，递归参数需要一个 $i$，表示子数组的右端点下标是 $i$。此外，需要知道是否可以删除数字，所以递归参数还需要一个 $j$。其中 $j=0$ 表示不能删除数字，$j=1$ 表示必须删除一个数。

因此，定义 $\textit{dfs}(i,j)$ 表示子数组的右端点下标是 $i$，不能/必须删除数字的情况下，子数组元素和的最大值。

根据上面讨论出的子问题，可以得到：

- 如果 $j=0$（不能删除）：
  - 如果不考虑 $\textit{arr}[i]$ 左边的数，那么 $\textit{dfs}(i,0)=\textit{arr}[i]$。
  - 如果考虑 $\textit{arr}[i]$ 左边的数，那么 $\textit{dfs}(i,0)=\textit{dfs}(i-1,0) + \textit{arr}[i]$。
- 如果 $j=1$（必须删除）：
  - 如果不删除 $\textit{arr}[i]$，那么 $\textit{dfs}(i,1)=\textit{dfs}(i-1,1)+\textit{arr}[i]$。
  - 如果删除 $\textit{arr}[i]$，那么 $\textit{dfs}(i,1)=\textit{dfs}(i-1,0)$。

取最大值，就得到了 $\textit{dfs}(i,j)$。写成式子就是

$$
\begin{aligned}
&\textit{dfs}(i,0) = \max(\textit{dfs}(i-1,0), 0) + \textit{arr}[i] \\
&\textit{dfs}(i,1) = \max(\textit{dfs}(i-1,1) + \textit{arr}[i], \textit{dfs}(i-1,0))
\end{aligned}
$$

递归边界：$\textit{dfs}(-1,j)=-\infty$。这里 $-1$ 表示子数组中「没有数字」，但题目要求子数组不能为空，所以这种情况不合法，用 $-\infty$ 表示，这样取 $\max$ 的时候就自然会取到合法的情况。

递归入口：$\textit{dfs}(i,j)$。枚举子数组右端点 $i$，以及是否需要删除数字 $j=0,1$，取所有结果的最大值，作为答案。

> **注**：根据状态定义，$\textit{dfs}(i,1)$ 会在删除 $\textit{arr}[k]$ 的那一刻，从 $\textit{dfs}(k-1,0)$ 转移过来。由于 $\textit{dfs}(k-1,0)$ 对应一个非空的子数组，所以我们删除的 $\textit{arr}[k]$ 一定不是子数组的第一个数。**推论**：$\textit{dfs}(1,1)$ 不可能等于 $\textit{arr}[1]$（如果等于，则意味着我们删除了 $\textit{arr}[0]$，矛盾）。

## 三、递归 + 记录返回值 = 记忆化搜索

以 $[1,-2,0,3]$ 为例。在计算 $\textit{dfs}(3,1)$ 时，「删除 $3$，保留 $0$」和「保留 $3$，删除 $0$」，都会递归到 $\textit{dfs}(1,0)$。

一叶知秋，整个递归中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组（或哈希表）中。
- 如果一个状态不是第一次遇到，那么直接返回 $\textit{memo}$ 中保存的结果。

```py [sol-Python3]
class Solution:
    def maximumSum(self, arr: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int, j: int) -> int:
            if i < 0:
                return -inf  # 子数组至少要有一个数，不合法
            if j == 0:
                return max(dfs(i - 1, 0), 0) + arr[i]
            return max(dfs(i - 1, 1) + arr[i], dfs(i - 1, 0))

        return max(max(dfs(i, 0), dfs(i, 1)) for i in range(len(arr)))
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] arr) {
        int n = arr.length;
        int[][] memo = new int[n][2];
        for (int[] row : memo) {
            Arrays.fill(row, Integer.MIN_VALUE);
        }

        int ans = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++) {
            ans = Math.max(ans, Math.max(dfs(i, 0, arr, memo), dfs(i, 1, arr, memo)));
        }
        return ans;
    }

    private int dfs(int i, int j, int[] arr, int[][] memo) {
        if (i < 0) {
            return Integer.MIN_VALUE / 2; // 除 2 防止负数相加溢出
        }
        if (memo[i][j] != Integer.MIN_VALUE) {
            return memo[i][j]; // 之前计算过
        }
        if (j == 0) {
            return memo[i][j] = Math.max(dfs(i - 1, 0, arr, memo), 0) + arr[i];
        }
        return memo[i][j] = Math.max(dfs(i - 1, 1, arr, memo) + arr[i], dfs(i - 1, 0, arr, memo));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& arr) {
        int n = arr.size();
        vector<array<int, 2>> memo(n + 1, {INT_MIN, INT_MIN});

        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i < 0) {
                return INT_MIN / 2; // 除 2 防止负数相加溢出
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != INT_MIN) {
                return res; // 之前计算过
            }
            if (j == 0) {
                return res = max(dfs(i - 1, 0), 0) + arr[i];
            }
            return res = max(dfs(i - 1, 1) + arr[i], dfs(i - 1, 0));
        };

        int ans = INT_MIN;
        for (int i = 0; i < n; i++) {
            ans = max(ans, max(dfs(i, 0), dfs(i, 1)));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(arr []int) int {
    memo := make([][2]int, len(arr))
    for i := range memo {
        memo[i] = [2]int{math.MinInt, math.MinInt}
    }

    var dfs func(int, int) int
    dfs = func(i, j int) (res int) {
        if i < 0 {
            return math.MinInt / 2 // 除 2 防止负数相加溢出
        }
        p := &memo[i][j]
        if *p != math.MinInt { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if j == 0 {
            return max(dfs(i-1, 0), 0) + arr[i]
        }
        return max(dfs(i-1, 1)+arr[i], dfs(i-1, 0))
    }

    ans := math.MinInt
    for i := range arr {
        ans = max(ans, dfs(i, 0), dfs(i, 1))
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

做法：

- $\textit{dfs}$ 改成 $f$ 数组。
- 递归改成循环（每个参数都对应一层循环）。这里 $j$ 只有 $0$ 和 $1$，可以直接计算，无需循环 $j$。
- 递归边界改成 $f$ 数组的初始值。

> 相当于之前是用递归去计算每个状态，现在是（按照某种顺序）枚举并计算每个状态。

具体来说，$f[i][j]$ 的含义和 $\textit{dfs}(i,j)$ 的含义是一致的，都表示子数组的右端点下标是 $i$，不能/必须删除数字的情况下，子数组元素和的最大值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 的一致：

$$
\begin{aligned}
&f[i][0] = \max(f[i-1][0], 0) + \textit{arr}[i] \\
&f[i][1] = \max(f[i-1][1] + \textit{arr}[i], f[i-1][0])
\end{aligned}
$$

但是，这种定义方式**没有状态能表示递归边界**，即 $i=-1$ 的情况。

解决办法：把 $f$ 数组的长度加一，用 $f[0][j]$ 表示 $\textit{dfs}(-1,j)$。由于 $f[0]$ 被占用，原来的下标 $i$ 需要全部向右偏移一位，也就是 $f[i]$ 改为 $f[i+1]$，$f[i-1]$ 改为 $f[i]$。

修改后 $f[i+1]$ 表示子数组的右端点下标是 $i$，不能/必须删除数字的情况下，子数组元素和的最大值。

修改后的递推式为

$$
\begin{aligned}
&f[i+1][0] = \max(f[i][0], 0) + \textit{arr}[i] \\
&f[i+1][1] = \max(f[i][1] + \textit{arr}[i], f[i][0])
\end{aligned}
$$

> 问：为什么 $\textit{arr}$ 的下标不用变？
>
> 答：既然是把 $f$ 数组的长度加一，那么就只需要修改和 $f$ 有关的下标，其余任何逻辑都无需修改。

初始值 $f[0][j]=-\infty$，翻译自 $\textit{dfs}(-1,j)=-\infty$。

答案为所有 $f[i][j]$ 的最大值。

```py [sol-Python3]
class Solution:
    def maximumSum(self, arr: List[int]) -> int:
        f = [[-inf] * 2] + [[0, 0] for _ in arr]
        for i, x in enumerate(arr):
            f[i + 1][0] = max(f[i][0], 0) + x
            f[i + 1][1] = max(f[i][1] + x, f[i][0])
        return max(max(r) for r in f)
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] arr) {
        int n = arr.length;
        int[][] f = new int[n + 1][2];
        Arrays.fill(f[0], Integer.MIN_VALUE / 2); // 除 2 防止负数相加溢出
        int ans = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = Math.max(f[i][0], 0) + arr[i];
            f[i + 1][1] = Math.max(f[i][1] + arr[i], f[i][0]);
            ans = Math.max(ans, Math.max(f[i + 1][0], f[i + 1][1]));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& arr) {
        int n = arr.size();
        vector<array<int, 2>> f(n + 1, {INT_MIN / 2, INT_MIN / 2}); // 除 2 防止负数相加溢出
        int ans = INT_MIN;
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = max(f[i][0], 0) + arr[i];
            f[i + 1][1] = max(f[i][1] + arr[i], f[i][0]);
            ans = max(ans, max(f[i + 1][0], f[i + 1][1]));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(arr []int) int {
    ans := math.MinInt
    f := make([][2]int, len(arr)+1)
    f[0] = [2]int{math.MinInt / 2, math.MinInt / 2} // 除 2 防止负数相加溢出
    for i, x := range arr {
        f[i+1][0] = max(f[i][0], 0) + x
        f[i+1][1] = max(f[i][1]+x, f[i][0])
        ans = max(ans, f[i+1][0], f[i+1][1])
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 五、空间优化

观察上面的状态转移方程，在计算 $f[i+1]$ 时，只会用到 $f[i]$，不会用到下标 $< i$ 的状态。

因此只需要两个状态表示 $j=0,1$。

状态转移方程改为

$$
\begin{aligned}
&f[1] = \max(f[1] + \textit{arr}[i], f[0])\\
&f[0] = \max(f[0], 0) + \textit{arr}[i] 
\end{aligned}
$$

**请注意计算顺序**！必须先算 $f[1]$ 再算 $f[0]$。如果先算 $f[0]$ 再算 $f[1]$，那么在计算 $f[1]$ 时，相当于用到的不是原来的 $f[i][0]$，而是新算出来的 $f[i+1][0]$。

初始值 $f[j]=-\infty$。

一边计算，一边维护 $f[j]$ 的最大值，作为答案。

```py [sol-Python3]
class Solution:
    def maximumSum(self, arr: List[int]) -> int:
        ans = f0 = f1 = -inf
        for x in arr:
            f1 = max(f1 + x, f0)  # 注：手动 if 比大小会更快 
            f0 = max(f0, 0) + x
            ans = max(ans, f0, f1)
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] arr) {
        int ans = Integer.MIN_VALUE / 2;
        int f0 = ans;
        int f1 = ans;
        for (int x : arr) {
            f1 = Math.max(f1 + x, f0);
            f0 = Math.max(f0, 0) + x;
            ans = Math.max(ans, Math.max(f0, f1));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& arr) {
        int ans = INT_MIN / 2, f0 = ans, f1 = ans;
        for (int x : arr) {
            f1 = max(f1 + x, f0);
            f0 = max(f0, 0) + x;
            ans = max(ans, max(f0, f1));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(arr []int) int {
    ans := math.MinInt / 2
    f0, f1 := ans, ans
    for _, x := range arr {
        f1 = max(f1+x, f0)
        f0 = max(f0, 0) + x
        ans = max(ans, f0, f1)
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。只用到常数级的额外空间。

## 思考题

如果改成至多删除 $k$ 次，要怎么做？

改成恰好删除 $k$ 次呢？

改成至少删除 $k$ 次呢？

欢迎在评论区发表你的思路/代码。

## 专题训练

见下面动态规划题单的「**六、状态机 DP**」。

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

## 前置知识：动态规划入门

详见 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)

## 一、启发思考：寻找子问题

最暴力的做法是枚举子数组的左右端点，以及要删除的元素。这种做法显然会超时。

保留「枚举子数组的右端点」这一想法，看看有没有可以优化的地方。

对于示例 1 中的数组 $[1,-2,0,3]$，假设元素和最大的连续子数组的右端点下标是 $3$。那么原问题为：「子数组的右端点下标是 $3$，且**至多**删除一个数，子数组元素和的最大值是多少？」

这个原问题可以拆分成两个问题：

1. 子数组的右端点下标是 $3$，且**不能**删除数字，子数组元素和的最大值是多少？
   - 如果子数组长度为 $1$，那么子数组就是 $[3]$。
   - 如果子数组长度大于 $1$，那么需要解决的问题为：「子数组的右端点下标是 $2$，且不能删除数字，子数组元素和的最大值是多少？」
2. 子数组的右端点下标是 $3$，且**必须**删除一个数字，子数组元素和的最大值是多少？
   - 如果子数组只有 $3$，那么必须删除 $3$，但这违背了题目要求：「（删除后）子数组中至少应当有一个元素」，所以**不考虑这种情况**。
   - 如果不删除 $3$，那么需要解决的问题为：「子数组的右端点下标是 $2$，且必须删除一个数字，子数组元素和的最大值是多少？」
   - 如果删除 $3$，那么需要解决的问题为：「子数组的右端点下标是 $2$，且不能删除数字，子数组元素和的最大值是多少？」

上面说的三个需要解决的问题，都是**和原问题相似的、规模更小的子问题**，所以可以用递归解决。

> 注 1：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。
> 
> 注 2：动态规划有「选或不选」和「枚举选哪个」这两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。上面用到的是「选或不选」。

## 二、递归怎么写：状态定义与状态转移方程

根据上面的讨论，递归参数需要一个 $i$，表示子数组的右端点下标是 $i$。此外，需要知道是否可以删除数字，所以递归参数还需要一个 $j$。其中 $j=0$ 表示不能删除数字，$j=1$ 表示必须删除一个数。

因此，定义 $\textit{dfs}(i,j)$ 表示子数组的右端点下标是 $i$，不能/必须删除数字的情况下，子数组元素和的最大值。

根据上面讨论出的子问题，可以得到：

- 如果 $j=0$（不能删除）：
  - 如果不考虑 $\textit{arr}[i]$ 左边的数，那么 $\textit{dfs}(i,0)=\textit{arr}[i]$。
  - 如果考虑 $\textit{arr}[i]$ 左边的数，那么 $\textit{dfs}(i,0)=\textit{dfs}(i-1,0) + \textit{arr}[i]$。
- 如果 $j=1$（必须删除）：
  - 如果不删除 $\textit{arr}[i]$，那么 $\textit{dfs}(i,1)=\textit{dfs}(i-1,1)+\textit{arr}[i]$。
  - 如果删除 $\textit{arr}[i]$，那么 $\textit{dfs}(i,1)=\textit{dfs}(i-1,0)$。

取最大值，就得到了 $\textit{dfs}(i,j)$。写成式子就是

$$
\begin{aligned}
&\textit{dfs}(i,0) = \max(\textit{dfs}(i-1,0), 0) + \textit{arr}[i] \\
&\textit{dfs}(i,1) = \max(\textit{dfs}(i-1,1) + \textit{arr}[i], \textit{dfs}(i-1,0))
\end{aligned}
$$

递归边界：$\textit{dfs}(-1,j)=-\infty$。这里 $-1$ 表示子数组中「没有数字」，但题目要求子数组不能为空，所以这种情况不合法，用 $-\infty$ 表示，这样取 $\max$ 的时候就自然会取到合法的情况。

递归入口：$\textit{dfs}(i,j)$。枚举子数组右端点 $i$，以及是否需要删除数字 $j=0,1$，取所有结果的最大值，作为答案。

> **注**：根据状态定义，$\textit{dfs}(i,1)$ 会在删除 $\textit{arr}[k]$ 的那一刻，从 $\textit{dfs}(k-1,0)$ 转移过来。由于 $\textit{dfs}(k-1,0)$ 对应一个非空的子数组，所以我们删除的 $\textit{arr}[k]$ 一定不是子数组的第一个数。**推论**：$\textit{dfs}(1,1)$ 不可能等于 $\textit{arr}[1]$（如果等于，则意味着我们删除了 $\textit{arr}[0]$，矛盾）。

## 三、递归 + 记录返回值 = 记忆化搜索

以 $[1,-2,0,3]$ 为例。在计算 $\textit{dfs}(3,1)$ 时，「删除 $3$，保留 $0$」和「保留 $3$，删除 $0$」，都会递归到 $\textit{dfs}(1,0)$。

一叶知秋，整个递归中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组（或哈希表）中。
- 如果一个状态不是第一次遇到，那么直接返回 $\textit{memo}$ 中保存的结果。

```py [sol-Python3]
class Solution:
    def maximumSum(self, arr: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int, j: int) -> int:
            if i < 0:
                return -inf  # 子数组至少要有一个数，不合法
            if j == 0:
                return max(dfs(i - 1, 0), 0) + arr[i]
            return max(dfs(i - 1, 1) + arr[i], dfs(i - 1, 0))

        return max(max(dfs(i, 0), dfs(i, 1)) for i in range(len(arr)))
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] arr) {
        int n = arr.length;
        int[][] memo = new int[n][2];
        for (int[] row : memo) {
            Arrays.fill(row, Integer.MIN_VALUE);
        }

        int ans = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++) {
            ans = Math.max(ans, Math.max(dfs(i, 0, arr, memo), dfs(i, 1, arr, memo)));
        }
        return ans;
    }

    private int dfs(int i, int j, int[] arr, int[][] memo) {
        if (i < 0) {
            return Integer.MIN_VALUE / 2; // 除 2 防止负数相加溢出
        }
        if (memo[i][j] != Integer.MIN_VALUE) {
            return memo[i][j]; // 之前计算过
        }
        if (j == 0) {
            return memo[i][j] = Math.max(dfs(i - 1, 0, arr, memo), 0) + arr[i];
        }
        return memo[i][j] = Math.max(dfs(i - 1, 1, arr, memo) + arr[i], dfs(i - 1, 0, arr, memo));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& arr) {
        int n = arr.size();
        vector<array<int, 2>> memo(n + 1, {INT_MIN, INT_MIN});

        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i < 0) {
                return INT_MIN / 2; // 除 2 防止负数相加溢出
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != INT_MIN) {
                return res; // 之前计算过
            }
            if (j == 0) {
                return res = max(dfs(i - 1, 0), 0) + arr[i];
            }
            return res = max(dfs(i - 1, 1) + arr[i], dfs(i - 1, 0));
        };

        int ans = INT_MIN;
        for (int i = 0; i < n; i++) {
            ans = max(ans, max(dfs(i, 0), dfs(i, 1)));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(arr []int) int {
    memo := make([][2]int, len(arr))
    for i := range memo {
        memo[i] = [2]int{math.MinInt, math.MinInt}
    }

    var dfs func(int, int) int
    dfs = func(i, j int) (res int) {
        if i < 0 {
            return math.MinInt / 2 // 除 2 防止负数相加溢出
        }
        p := &memo[i][j]
        if *p != math.MinInt { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if j == 0 {
            return max(dfs(i-1, 0), 0) + arr[i]
        }
        return max(dfs(i-1, 1)+arr[i], dfs(i-1, 0))
    }

    ans := math.MinInt
    for i := range arr {
        ans = max(ans, dfs(i, 0), dfs(i, 1))
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

做法：

- $\textit{dfs}$ 改成 $f$ 数组。
- 递归改成循环（每个参数都对应一层循环）。这里 $j$ 只有 $0$ 和 $1$，可以直接计算，无需循环 $j$。
- 递归边界改成 $f$ 数组的初始值。

> 相当于之前是用递归去计算每个状态，现在是（按照某种顺序）枚举并计算每个状态。

具体来说，$f[i][j]$ 的含义和 $\textit{dfs}(i,j)$ 的含义是一致的，都表示子数组的右端点下标是 $i$，不能/必须删除数字的情况下，子数组元素和的最大值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 的一致：

$$
\begin{aligned}
&f[i][0] = \max(f[i-1][0], 0) + \textit{arr}[i] \\
&f[i][1] = \max(f[i-1][1] + \textit{arr}[i], f[i-1][0])
\end{aligned}
$$

但是，这种定义方式**没有状态能表示递归边界**，即 $i=-1$ 的情况。

解决办法：把 $f$ 数组的长度加一，用 $f[0][j]$ 表示 $\textit{dfs}(-1,j)$。由于 $f[0]$ 被占用，原来的下标 $i$ 需要全部向右偏移一位，也就是 $f[i]$ 改为 $f[i+1]$，$f[i-1]$ 改为 $f[i]$。

修改后 $f[i+1]$ 表示子数组的右端点下标是 $i$，不能/必须删除数字的情况下，子数组元素和的最大值。

修改后的递推式为

$$
\begin{aligned}
&f[i+1][0] = \max(f[i][0], 0) + \textit{arr}[i] \\
&f[i+1][1] = \max(f[i][1] + \textit{arr}[i], f[i][0])
\end{aligned}
$$

> 问：为什么 $\textit{arr}$ 的下标不用变？
>
> 答：既然是把 $f$ 数组的长度加一，那么就只需要修改和 $f$ 有关的下标，其余任何逻辑都无需修改。

初始值 $f[0][j]=-\infty$，翻译自 $\textit{dfs}(-1,j)=-\infty$。

答案为所有 $f[i][j]$ 的最大值。

```py [sol-Python3]
class Solution:
    def maximumSum(self, arr: List[int]) -> int:
        f = [[-inf] * 2] + [[0, 0] for _ in arr]
        for i, x in enumerate(arr):
            f[i + 1][0] = max(f[i][0], 0) + x
            f[i + 1][1] = max(f[i][1] + x, f[i][0])
        return max(max(r) for r in f)
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] arr) {
        int n = arr.length;
        int[][] f = new int[n + 1][2];
        Arrays.fill(f[0], Integer.MIN_VALUE / 2); // 除 2 防止负数相加溢出
        int ans = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = Math.max(f[i][0], 0) + arr[i];
            f[i + 1][1] = Math.max(f[i][1] + arr[i], f[i][0]);
            ans = Math.max(ans, Math.max(f[i + 1][0], f[i + 1][1]));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& arr) {
        int n = arr.size();
        vector<array<int, 2>> f(n + 1, {INT_MIN / 2, INT_MIN / 2}); // 除 2 防止负数相加溢出
        int ans = INT_MIN;
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = max(f[i][0], 0) + arr[i];
            f[i + 1][1] = max(f[i][1] + arr[i], f[i][0]);
            ans = max(ans, max(f[i + 1][0], f[i + 1][1]));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(arr []int) int {
    ans := math.MinInt
    f := make([][2]int, len(arr)+1)
    f[0] = [2]int{math.MinInt / 2, math.MinInt / 2} // 除 2 防止负数相加溢出
    for i, x := range arr {
        f[i+1][0] = max(f[i][0], 0) + x
        f[i+1][1] = max(f[i][1]+x, f[i][0])
        ans = max(ans, f[i+1][0], f[i+1][1])
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 五、空间优化

观察上面的状态转移方程，在计算 $f[i+1]$ 时，只会用到 $f[i]$，不会用到下标 $< i$ 的状态。

因此只需要两个状态表示 $j=0,1$。

状态转移方程改为

$$
\begin{aligned}
&f[1] = \max(f[1] + \textit{arr}[i], f[0])\\
&f[0] = \max(f[0], 0) + \textit{arr}[i] 
\end{aligned}
$$

**请注意计算顺序**！必须先算 $f[1]$ 再算 $f[0]$。如果先算 $f[0]$ 再算 $f[1]$，那么在计算 $f[1]$ 时，相当于用到的不是原来的 $f[i][0]$，而是新算出来的 $f[i+1][0]$。

初始值 $f[j]=-\infty$。

一边计算，一边维护 $f[j]$ 的最大值，作为答案。

```py [sol-Python3]
class Solution:
    def maximumSum(self, arr: List[int]) -> int:
        ans = f0 = f1 = -inf
        for x in arr:
            f1 = max(f1 + x, f0)  # 注：手动 if 比大小会更快 
            f0 = max(f0, 0) + x
            ans = max(ans, f0, f1)
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] arr) {
        int ans = Integer.MIN_VALUE / 2;
        int f0 = ans;
        int f1 = ans;
        for (int x : arr) {
            f1 = Math.max(f1 + x, f0);
            f0 = Math.max(f0, 0) + x;
            ans = Math.max(ans, Math.max(f0, f1));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& arr) {
        int ans = INT_MIN / 2, f0 = ans, f1 = ans;
        for (int x : arr) {
            f1 = max(f1 + x, f0);
            f0 = max(f0, 0) + x;
            ans = max(ans, max(f0, f1));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(arr []int) int {
    ans := math.MinInt / 2
    f0, f1 := ans, ans
    for _, x := range arr {
        f1 = max(f1+x, f0)
        f0 = max(f0, 0) + x
        ans = max(ans, f0, f1)
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。只用到常数级的额外空间。

## 思考题

如果改成至多删除 $k$ 次，要怎么做？

改成恰好删除 $k$ 次呢？

改成至少删除 $k$ 次呢？

欢迎在评论区发表你的思路/代码。

## 专题训练

见下面动态规划题单的「**六、状态机 DP**」。

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

## 前置知识：动态规划入门

详见 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)

## 一、启发思考：寻找子问题

最暴力的做法是枚举子数组的左右端点，以及要删除的元素。这种做法显然会超时。

保留「枚举子数组的右端点」这一想法，看看有没有可以优化的地方。

对于示例 1 中的数组 $[1,-2,0,3]$，假设元素和最大的连续子数组的右端点下标是 $3$。那么原问题为：「子数组的右端点下标是 $3$，且**至多**删除一个数，子数组元素和的最大值是多少？」

这个原问题可以拆分成两个问题：

1. 子数组的右端点下标是 $3$，且**不能**删除数字，子数组元素和的最大值是多少？
   - 如果子数组长度为 $1$，那么子数组就是 $[3]$。
   - 如果子数组长度大于 $1$，那么需要解决的问题为：「子数组的右端点下标是 $2$，且不能删除数字，子数组元素和的最大值是多少？」
2. 子数组的右端点下标是 $3$，且**必须**删除一个数字，子数组元素和的最大值是多少？
   - 如果子数组只有 $3$，那么必须删除 $3$，但这违背了题目要求：「（删除后）子数组中至少应当有一个元素」，所以**不考虑这种情况**。
   - 如果不删除 $3$，那么需要解决的问题为：「子数组的右端点下标是 $2$，且必须删除一个数字，子数组元素和的最大值是多少？」
   - 如果删除 $3$，那么需要解决的问题为：「子数组的右端点下标是 $2$，且不能删除数字，子数组元素和的最大值是多少？」

上面说的三个需要解决的问题，都是**和原问题相似的、规模更小的子问题**，所以可以用递归解决。

> 注 1：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。
> 
> 注 2：动态规划有「选或不选」和「枚举选哪个」这两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。上面用到的是「选或不选」。

## 二、递归怎么写：状态定义与状态转移方程

根据上面的讨论，递归参数需要一个 $i$，表示子数组的右端点下标是 $i$。此外，需要知道是否可以删除数字，所以递归参数还需要一个 $j$。其中 $j=0$ 表示不能删除数字，$j=1$ 表示必须删除一个数。

因此，定义 $\textit{dfs}(i,j)$ 表示子数组的右端点下标是 $i$，不能/必须删除数字的情况下，子数组元素和的最大值。

根据上面讨论出的子问题，可以得到：

- 如果 $j=0$（不能删除）：
  - 如果不考虑 $\textit{arr}[i]$ 左边的数，那么 $\textit{dfs}(i,0)=\textit{arr}[i]$。
  - 如果考虑 $\textit{arr}[i]$ 左边的数，那么 $\textit{dfs}(i,0)=\textit{dfs}(i-1,0) + \textit{arr}[i]$。
- 如果 $j=1$（必须删除）：
  - 如果不删除 $\textit{arr}[i]$，那么 $\textit{dfs}(i,1)=\textit{dfs}(i-1,1)+\textit{arr}[i]$。
  - 如果删除 $\textit{arr}[i]$，那么 $\textit{dfs}(i,1)=\textit{dfs}(i-1,0)$。

取最大值，就得到了 $\textit{dfs}(i,j)$。写成式子就是

$$
\begin{aligned}
&\textit{dfs}(i,0) = \max(\textit{dfs}(i-1,0), 0) + \textit{arr}[i] \\
&\textit{dfs}(i,1) = \max(\textit{dfs}(i-1,1) + \textit{arr}[i], \textit{dfs}(i-1,0))
\end{aligned}
$$

递归边界：$\textit{dfs}(-1,j)=-\infty$。这里 $-1$ 表示子数组中「没有数字」，但题目要求子数组不能为空，所以这种情况不合法，用 $-\infty$ 表示，这样取 $\max$ 的时候就自然会取到合法的情况。

递归入口：$\textit{dfs}(i,j)$。枚举子数组右端点 $i$，以及是否需要删除数字 $j=0,1$，取所有结果的最大值，作为答案。

> **注**：根据状态定义，$\textit{dfs}(i,1)$ 会在删除 $\textit{arr}[k]$ 的那一刻，从 $\textit{dfs}(k-1,0)$ 转移过来。由于 $\textit{dfs}(k-1,0)$ 对应一个非空的子数组，所以我们删除的 $\textit{arr}[k]$ 一定不是子数组的第一个数。**推论**：$\textit{dfs}(1,1)$ 不可能等于 $\textit{arr}[1]$（如果等于，则意味着我们删除了 $\textit{arr}[0]$，矛盾）。

## 三、递归 + 记录返回值 = 记忆化搜索

以 $[1,-2,0,3]$ 为例。在计算 $\textit{dfs}(3,1)$ 时，「删除 $3$，保留 $0$」和「保留 $3$，删除 $0$」，都会递归到 $\textit{dfs}(1,0)$。

一叶知秋，整个递归中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组（或哈希表）中。
- 如果一个状态不是第一次遇到，那么直接返回 $\textit{memo}$ 中保存的结果。

```py [sol-Python3]
class Solution:
    def maximumSum(self, arr: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int, j: int) -> int:
            if i < 0:
                return -inf  # 子数组至少要有一个数，不合法
            if j == 0:
                return max(dfs(i - 1, 0), 0) + arr[i]
            return max(dfs(i - 1, 1) + arr[i], dfs(i - 1, 0))

        return max(max(dfs(i, 0), dfs(i, 1)) for i in range(len(arr)))
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] arr) {
        int n = arr.length;
        int[][] memo = new int[n][2];
        for (int[] row : memo) {
            Arrays.fill(row, Integer.MIN_VALUE);
        }

        int ans = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++) {
            ans = Math.max(ans, Math.max(dfs(i, 0, arr, memo), dfs(i, 1, arr, memo)));
        }
        return ans;
    }

    private int dfs(int i, int j, int[] arr, int[][] memo) {
        if (i < 0) {
            return Integer.MIN_VALUE / 2; // 除 2 防止负数相加溢出
        }
        if (memo[i][j] != Integer.MIN_VALUE) {
            return memo[i][j]; // 之前计算过
        }
        if (j == 0) {
            return memo[i][j] = Math.max(dfs(i - 1, 0, arr, memo), 0) + arr[i];
        }
        return memo[i][j] = Math.max(dfs(i - 1, 1, arr, memo) + arr[i], dfs(i - 1, 0, arr, memo));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& arr) {
        int n = arr.size();
        vector<array<int, 2>> memo(n + 1, {INT_MIN, INT_MIN});

        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i < 0) {
                return INT_MIN / 2; // 除 2 防止负数相加溢出
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res != INT_MIN) {
                return res; // 之前计算过
            }
            if (j == 0) {
                return res = max(dfs(i - 1, 0), 0) + arr[i];
            }
            return res = max(dfs(i - 1, 1) + arr[i], dfs(i - 1, 0));
        };

        int ans = INT_MIN;
        for (int i = 0; i < n; i++) {
            ans = max(ans, max(dfs(i, 0), dfs(i, 1)));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(arr []int) int {
    memo := make([][2]int, len(arr))
    for i := range memo {
        memo[i] = [2]int{math.MinInt, math.MinInt}
    }

    var dfs func(int, int) int
    dfs = func(i, j int) (res int) {
        if i < 0 {
            return math.MinInt / 2 // 除 2 防止负数相加溢出
        }
        p := &memo[i][j]
        if *p != math.MinInt { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if j == 0 {
            return max(dfs(i-1, 0), 0) + arr[i]
        }
        return max(dfs(i-1, 1)+arr[i], dfs(i-1, 0))
    }

    ans := math.MinInt
    for i := range arr {
        ans = max(ans, dfs(i, 0), dfs(i, 1))
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

做法：

- $\textit{dfs}$ 改成 $f$ 数组。
- 递归改成循环（每个参数都对应一层循环）。这里 $j$ 只有 $0$ 和 $1$，可以直接计算，无需循环 $j$。
- 递归边界改成 $f$ 数组的初始值。

> 相当于之前是用递归去计算每个状态，现在是（按照某种顺序）枚举并计算每个状态。

具体来说，$f[i][j]$ 的含义和 $\textit{dfs}(i,j)$ 的含义是一致的，都表示子数组的右端点下标是 $i$，不能/必须删除数字的情况下，子数组元素和的最大值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 的一致：

$$
\begin{aligned}
&f[i][0] = \max(f[i-1][0], 0) + \textit{arr}[i] \\
&f[i][1] = \max(f[i-1][1] + \textit{arr}[i], f[i-1][0])
\end{aligned}
$$

但是，这种定义方式**没有状态能表示递归边界**，即 $i=-1$ 的情况。

解决办法：把 $f$ 数组的长度加一，用 $f[0][j]$ 表示 $\textit{dfs}(-1,j)$。由于 $f[0]$ 被占用，原来的下标 $i$ 需要全部向右偏移一位，也就是 $f[i]$ 改为 $f[i+1]$，$f[i-1]$ 改为 $f[i]$。

修改后 $f[i+1]$ 表示子数组的右端点下标是 $i$，不能/必须删除数字的情况下，子数组元素和的最大值。

修改后的递推式为

$$
\begin{aligned}
&f[i+1][0] = \max(f[i][0], 0) + \textit{arr}[i] \\
&f[i+1][1] = \max(f[i][1] + \textit{arr}[i], f[i][0])
\end{aligned}
$$

> 问：为什么 $\textit{arr}$ 的下标不用变？
>
> 答：既然是把 $f$ 数组的长度加一，那么就只需要修改和 $f$ 有关的下标，其余任何逻辑都无需修改。

初始值 $f[0][j]=-\infty$，翻译自 $\textit{dfs}(-1,j)=-\infty$。

答案为所有 $f[i][j]$ 的最大值。

```py [sol-Python3]
class Solution:
    def maximumSum(self, arr: List[int]) -> int:
        f = [[-inf] * 2] + [[0, 0] for _ in arr]
        for i, x in enumerate(arr):
            f[i + 1][0] = max(f[i][0], 0) + x
            f[i + 1][1] = max(f[i][1] + x, f[i][0])
        return max(max(r) for r in f)
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] arr) {
        int n = arr.length;
        int[][] f = new int[n + 1][2];
        Arrays.fill(f[0], Integer.MIN_VALUE / 2); // 除 2 防止负数相加溢出
        int ans = Integer.MIN_VALUE;
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = Math.max(f[i][0], 0) + arr[i];
            f[i + 1][1] = Math.max(f[i][1] + arr[i], f[i][0]);
            ans = Math.max(ans, Math.max(f[i + 1][0], f[i + 1][1]));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& arr) {
        int n = arr.size();
        vector<array<int, 2>> f(n + 1, {INT_MIN / 2, INT_MIN / 2}); // 除 2 防止负数相加溢出
        int ans = INT_MIN;
        for (int i = 0; i < n; i++) {
            f[i + 1][0] = max(f[i][0], 0) + arr[i];
            f[i + 1][1] = max(f[i][1] + arr[i], f[i][0]);
            ans = max(ans, max(f[i + 1][0], f[i + 1][1]));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(arr []int) int {
    ans := math.MinInt
    f := make([][2]int, len(arr)+1)
    f[0] = [2]int{math.MinInt / 2, math.MinInt / 2} // 除 2 防止负数相加溢出
    for i, x := range arr {
        f[i+1][0] = max(f[i][0], 0) + x
        f[i+1][1] = max(f[i][1]+x, f[i][0])
        ans = max(ans, f[i+1][0], f[i+1][1])
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 五、空间优化

观察上面的状态转移方程，在计算 $f[i+1]$ 时，只会用到 $f[i]$，不会用到下标 $< i$ 的状态。

因此只需要两个状态表示 $j=0,1$。

状态转移方程改为

$$
\begin{aligned}
&f[1] = \max(f[1] + \textit{arr}[i], f[0])\\
&f[0] = \max(f[0], 0) + \textit{arr}[i] 
\end{aligned}
$$

**请注意计算顺序**！必须先算 $f[1]$ 再算 $f[0]$。如果先算 $f[0]$ 再算 $f[1]$，那么在计算 $f[1]$ 时，相当于用到的不是原来的 $f[i][0]$，而是新算出来的 $f[i+1][0]$。

初始值 $f[j]=-\infty$。

一边计算，一边维护 $f[j]$ 的最大值，作为答案。

```py [sol-Python3]
class Solution:
    def maximumSum(self, arr: List[int]) -> int:
        ans = f0 = f1 = -inf
        for x in arr:
            f1 = max(f1 + x, f0)  # 注：手动 if 比大小会更快 
            f0 = max(f0, 0) + x
            ans = max(ans, f0, f1)
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumSum(int[] arr) {
        int ans = Integer.MIN_VALUE / 2;
        int f0 = ans;
        int f1 = ans;
        for (int x : arr) {
            f1 = Math.max(f1 + x, f0);
            f0 = Math.max(f0, 0) + x;
            ans = Math.max(ans, Math.max(f0, f1));
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumSum(vector<int>& arr) {
        int ans = INT_MIN / 2, f0 = ans, f1 = ans;
        for (int x : arr) {
            f1 = max(f1 + x, f0);
            f0 = max(f0, 0) + x;
            ans = max(ans, max(f0, f1));
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumSum(arr []int) int {
    ans := math.MinInt / 2
    f0, f1 := ans, ans
    for _, x := range arr {
        f1 = max(f1+x, f0)
        f0 = max(f0, 0) + x
        ans = max(ans, f0, f1)
    }
    return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{arr}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。只用到常数级的额外空间。

## 思考题

如果改成至多删除 $k$ 次，要怎么做？

改成恰好删除 $k$ 次呢？

改成至少删除 $k$ 次呢？

欢迎在评论区发表你的思路/代码。

## 专题训练

见下面动态规划题单的「**六、状态机 DP**」。

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
