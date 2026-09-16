# 3693. 爬楼梯 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/climbing-stairs-ii/
- 题目 slug：`climbing-stairs-ii`
- 来源专题：动态规划
- 来源分类路径：一、入门 DP / §1.1 爬楼梯
- 难度分：1560
- 外部题解来源：https://leetcode.cn/problems/climbing-stairs-ii/solutions/3793871/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-lkrf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推到空间优化（Python/Java/C++/Go）](https://leetcode.cn/problems/climbing-stairs-ii/solutions/3793871/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-lkrf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-lkrf`
- topic id：`3793871`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、寻找子问题

我们要从 $0$ 爬到 $n$。

考虑最后一步爬了多少个台阶：

- 最后一步爬 $1$ 个台阶，问题变成从 $0$ 爬到 $n-1$ 的最小总成本。
- 最后一步爬 $2$ 个台阶，问题变成从 $0$ 爬到 $n-2$ 的最小总成本。
- 最后一步爬 $3$ 个台阶，问题变成从 $0$ 爬到 $n-3$ 的最小总成本。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

> 注：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。

## 二、状态定义与状态转移方程

根据上面的讨论，定义 $\textit{dfs}(i)$，表示从 $0$ 爬到 $i$ 的最小总成本。

枚举最后一步从 $j$ 爬到 $i$，问题变成从 $0$ 爬到 $j$ 的最小总成本，即 $\textit{dfs}(j)$。加上从 $j$ 到 $i$ 的成本 $(i-j)^2 + \textit{costs}[i]$，更新 $\textit{dfs}(i)$ 的最小值：

$$
\textit{dfs}(i) = \min_{j=\max(i-3,0)}^{i-1}\textit{dfs}(j) + (i-j)^2 + \textit{costs}[i]
$$

**递归边界**：$\textit{dfs}(0)=0$。从 $0$ 爬到 $0$ 无需移动，成本为 $0$。

**递归入口**：$\textit{dfs}(n)$，这是原问题（从 $0$ 爬到 $n$ 的最小总成本），也是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

⚠**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。但本题成本是正数，$i>0$ 时 $\textit{dfs}(i) > 0$，所以可以把 $\textit{memo}[i]$ 初始化成 $0$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

⚠**注意**：题目说 $\textit{costs}$ 的下标从 $1$ 开始，但传入的 $\textit{costs}$ 的下标是从 $0$ 开始的。访问数组的时候下标要减一。

```py [sol-Python3]
class Solution:
    def climbStairs(self, n: int, costs: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int) -> int:
            if i == 0:
                return 0
            # costs[i-1] 与 j 无关，可以提到循环外面
            return min(dfs(j) + (i - j) * (i - j) for j in range(max(i - 3, 0), i)) + costs[i - 1]
        return dfs(n)
```

```java [sol-Java]
class Solution {
    public int climbStairs(int n, int[] costs) {
        int[] memo = new int[n + 1];
        return dfs(n, costs, memo);
    }

    private int dfs(int i, int[] costs, int[] memo) {
        if (i == 0) {
            return 0;
        }
        if (memo[i] != 0) { // 之前计算过
            return memo[i];
        }
        int res = Integer.MAX_VALUE;
        for (int j = Math.max(i - 3, 0); j < i; j++) {
            res = Math.min(res, dfs(j, costs, memo) + (i - j) * (i - j));
        }
        res += costs[i - 1]; // costs[i-1] 与 j 无关，可以提到循环外面
        return memo[i] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int climbStairs(int n, vector<int>& costs) {
        vector<int> memo(n + 1);

        // lambda 递归函数
        auto dfs = [&](this auto&& dfs, int i) -> int {
            if (i == 0) {
                return 0;
            }
            int& res = memo[i]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }
            res = INT_MAX;
            for (int j = max(i - 3, 0); j < i; j++) {
                res = min(res, dfs(j) + (i - j) * (i - j));
            }
            res += costs[i - 1]; // costs[i-1] 与 j 无关，可以提到循环外面
            return res;
        };

        return dfs(n);
    }
};
```

```go [sol-Go]
func climbStairs(n int, costs []int) int {
	memo := make([]int, n+1)
	var dfs func(int) int
	dfs = func(i int) int {
		if i == 0 {
			return 0
		}
		p := &memo[i]
		if *p != 0 { // 之前计算过
			return *p
		}
		res := math.MaxInt
		for j := max(i-3, 0); j < i; j++ {
			res = min(res, dfs(j)+(i-j)*(i-j))
		}
		res += costs[i-1] // costs[i-1] 与 j 无关，可以提到循环外面
		*p = res // 记忆化
		return res
	}
	return dfs(n)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nK)$，其中 $K=3$ 是最多可以跳的台阶数。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(K)$，所以总的时间复杂度为 $\mathcal{O}(nK)$。
- 空间复杂度：$\mathcal{O}(n)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i]$ 的定义和 $\textit{dfs}(i)$ 的定义是完全一样的，都表示从 $0$ 爬到 $i$ 的最小总成本。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i] = \min_{j=\max(i-3,0)}^{i-1}f[j] + (i-j)^2 + \textit{costs}[i]
$$

初始值 $f[0]=0$，翻译自递归边界 $\textit{dfs}(0)=0$。

答案为 $f[n]$，翻译自递归入口 $\textit{dfs}(n)$。

```py [sol-Python3]
class Solution:
    def climbStairs(self, n: int, costs: List[int]) -> int:
        f = [0] * (n + 1)
        for i in range(1, n + 1):
            f[i] = min(f[j] + (i - j) * (i - j) for j in range(max(i - 3, 0), i)) + costs[i - 1]
        return f[n]
```

```java [sol-Java]
class Solution {
    public int climbStairs(int n, int[] costs) {
        int[] f = new int[n + 1];
        for (int i = 1; i <= n; i++) {
            int res = Integer.MAX_VALUE;
            for (int j = Math.max(i - 3, 0); j < i; j++) {
                res = Math.min(res, f[j] + (i - j) * (i - j));
            }
            f[i] = res + costs[i - 1];
        }
        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int climbStairs(int n, vector<int>& costs) {
        vector<int> f(n + 1);
        for (int i = 1; i <= n; i++) {
            int res = INT_MAX;
            for (int j = max(i - 3, 0); j < i; j++) {
                res = min(res, f[j] + (i - j) * (i - j));
            }
            f[i] = res + costs[i - 1];
        }
        return f[n];
    }
};
```

```go [sol-Go]
func climbStairs(n int, costs []int) int {
	f := make([]int, n+1)
	for i := 1; i <= n; i++ {
		res := math.MaxInt
		for j := max(i-3, 0); j < i; j++ {
			res = min(res, f[j]+(i-j)*(i-j))
		}
		f[i] = res + costs[i-1]
	}
	return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nK)$，其中 $K=3$ 是最多可以跳的台阶数。
- 空间复杂度：$\mathcal{O}(n)$。

## 五、空间优化

类似 [70. 爬楼梯](https://leetcode.cn/problems/climbing-stairs/)，用三个变量滚动计算。原理见 [我的题解](https://leetcode.cn/problems/climbing-stairs/solutions/2560716/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-7zm1/)。

```py [sol-Python3]
class Solution:
    def climbStairs(self, _, costs: List[int]) -> int:
        f0 = f1 = f2 = 0
        for c in costs:
            f0, f1, f2 = f1, f2, min(f0 + 9, f1 + 4, f2 + 1) + c
        return f2
```

```py [sol-Python3 手写 min]
class Solution:
    def climbStairs(self, _, costs: List[int]) -> int:
        f0 = f1 = f2 = 0
        for c in costs:
            mn = f0 + 9
            if (t := f1 + 4) < mn: mn = t
            if (t := f2 + 1) < mn: mn = t
            f0, f1, f2 = f1, f2, mn + c
        return f2
```

```java [sol-Java]
class Solution {
    public int climbStairs(int n, int[] costs) {
        int f0 = 0, f1 = 0, f2 = 0;
        for (int c : costs) {
            int newF = Math.min(Math.min(f0 + 9, f1 + 4), f2 + 1) + c;
            f0 = f1;
            f1 = f2;
            f2 = newF;
        }
        return f2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int climbStairs(int, vector<int>& costs) {
        int f0 = 0, f1 = 0, f2 = 0;
        for (int c : costs) {
            // 另一种写法是 min({f0 + 9, f1 + 4, f2 + 1})
            int new_f = min(min(f0 + 9, f1 + 4), f2 + 1) + c;
            f0 = f1;
            f1 = f2;
            f2 = new_f;
        }
        return f2;
    }
};
```

```go [sol-Go]
func climbStairs(_ int, costs []int) int {
	var f0, f1, f2 int
	for _, c := range costs {
		f0, f1, f2 = f1, f2, min(f0+9, f1+4, f2+1)+c
	}
	return f2
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nK)$，其中 $K=3$ 是最多可以跳的台阶数。
- 空间复杂度：$\mathcal{O}(K)$。

本题有两个变形：

1. 去掉 $K=3$ 的约束（可以爬任意多级台阶），这可以用斜率优化，时间复杂度 $\mathcal{O}(n)$。
2. 额外传入一个参数 $K$，这可以用 [李超线段树](https://oi-wiki.org/ds/li-chao-tree/)，时间复杂度 $\mathcal{O}(n\log^2 n)$。

## 专题训练

1. 动态规划题单的「**§1.1 爬楼梯**」。
2. 动态规划题单的「**§11.7 斜率优化 DP**」。

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

## 一、寻找子问题

我们要从 $0$ 爬到 $n$。

考虑最后一步爬了多少个台阶：

- 最后一步爬 $1$ 个台阶，问题变成从 $0$ 爬到 $n-1$ 的最小总成本。
- 最后一步爬 $2$ 个台阶，问题变成从 $0$ 爬到 $n-2$ 的最小总成本。
- 最后一步爬 $3$ 个台阶，问题变成从 $0$ 爬到 $n-3$ 的最小总成本。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

> 注：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。

## 二、状态定义与状态转移方程

根据上面的讨论，定义 $\textit{dfs}(i)$，表示从 $0$ 爬到 $i$ 的最小总成本。

枚举最后一步从 $j$ 爬到 $i$，问题变成从 $0$ 爬到 $j$ 的最小总成本，即 $\textit{dfs}(j)$。加上从 $j$ 到 $i$ 的成本 $(i-j)^2 + \textit{costs}[i]$，更新 $\textit{dfs}(i)$ 的最小值：

$$
\textit{dfs}(i) = \min_{j=\max(i-3,0)}^{i-1}\textit{dfs}(j) + (i-j)^2 + \textit{costs}[i]
$$

**递归边界**：$\textit{dfs}(0)=0$。从 $0$ 爬到 $0$ 无需移动，成本为 $0$。

**递归入口**：$\textit{dfs}(n)$，这是原问题（从 $0$ 爬到 $n$ 的最小总成本），也是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

⚠**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。但本题成本是正数，$i>0$ 时 $\textit{dfs}(i) > 0$，所以可以把 $\textit{memo}[i]$ 初始化成 $0$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

⚠**注意**：题目说 $\textit{costs}$ 的下标从 $1$ 开始，但传入的 $\textit{costs}$ 的下标是从 $0$ 开始的。访问数组的时候下标要减一。

```py [sol-Python3]
class Solution:
    def climbStairs(self, n: int, costs: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int) -> int:
            if i == 0:
                return 0
            # costs[i-1] 与 j 无关，可以提到循环外面
            return min(dfs(j) + (i - j) * (i - j) for j in range(max(i - 3, 0), i)) + costs[i - 1]
        return dfs(n)
```

```java [sol-Java]
class Solution {
    public int climbStairs(int n, int[] costs) {
        int[] memo = new int[n + 1];
        return dfs(n, costs, memo);
    }

    private int dfs(int i, int[] costs, int[] memo) {
        if (i == 0) {
            return 0;
        }
        if (memo[i] != 0) { // 之前计算过
            return memo[i];
        }
        int res = Integer.MAX_VALUE;
        for (int j = Math.max(i - 3, 0); j < i; j++) {
            res = Math.min(res, dfs(j, costs, memo) + (i - j) * (i - j));
        }
        res += costs[i - 1]; // costs[i-1] 与 j 无关，可以提到循环外面
        return memo[i] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int climbStairs(int n, vector<int>& costs) {
        vector<int> memo(n + 1);

        // lambda 递归函数
        auto dfs = [&](this auto&& dfs, int i) -> int {
            if (i == 0) {
                return 0;
            }
            int& res = memo[i]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }
            res = INT_MAX;
            for (int j = max(i - 3, 0); j < i; j++) {
                res = min(res, dfs(j) + (i - j) * (i - j));
            }
            res += costs[i - 1]; // costs[i-1] 与 j 无关，可以提到循环外面
            return res;
        };

        return dfs(n);
    }
};
```

```go [sol-Go]
func climbStairs(n int, costs []int) int {
	memo := make([]int, n+1)
	var dfs func(int) int
	dfs = func(i int) int {
		if i == 0 {
			return 0
		}
		p := &memo[i]
		if *p != 0 { // 之前计算过
			return *p
		}
		res := math.MaxInt
		for j := max(i-3, 0); j < i; j++ {
			res = min(res, dfs(j)+(i-j)*(i-j))
		}
		res += costs[i-1] // costs[i-1] 与 j 无关，可以提到循环外面
		*p = res // 记忆化
		return res
	}
	return dfs(n)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nK)$，其中 $K=3$ 是最多可以跳的台阶数。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(K)$，所以总的时间复杂度为 $\mathcal{O}(nK)$。
- 空间复杂度：$\mathcal{O}(n)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i]$ 的定义和 $\textit{dfs}(i)$ 的定义是完全一样的，都表示从 $0$ 爬到 $i$ 的最小总成本。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i] = \min_{j=\max(i-3,0)}^{i-1}f[j] + (i-j)^2 + \textit{costs}[i]
$$

初始值 $f[0]=0$，翻译自递归边界 $\textit{dfs}(0)=0$。

答案为 $f[n]$，翻译自递归入口 $\textit{dfs}(n)$。

```py [sol-Python3]
class Solution:
    def climbStairs(self, n: int, costs: List[int]) -> int:
        f = [0] * (n + 1)
        for i in range(1, n + 1):
            f[i] = min(f[j] + (i - j) * (i - j) for j in range(max(i - 3, 0), i)) + costs[i - 1]
        return f[n]
```

```java [sol-Java]
class Solution {
    public int climbStairs(int n, int[] costs) {
        int[] f = new int[n + 1];
        for (int i = 1; i <= n; i++) {
            int res = Integer.MAX_VALUE;
            for (int j = Math.max(i - 3, 0); j < i; j++) {
                res = Math.min(res, f[j] + (i - j) * (i - j));
            }
            f[i] = res + costs[i - 1];
        }
        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int climbStairs(int n, vector<int>& costs) {
        vector<int> f(n + 1);
        for (int i = 1; i <= n; i++) {
            int res = INT_MAX;
            for (int j = max(i - 3, 0); j < i; j++) {
                res = min(res, f[j] + (i - j) * (i - j));
            }
            f[i] = res + costs[i - 1];
        }
        return f[n];
    }
};
```

```go [sol-Go]
func climbStairs(n int, costs []int) int {
	f := make([]int, n+1)
	for i := 1; i <= n; i++ {
		res := math.MaxInt
		for j := max(i-3, 0); j < i; j++ {
			res = min(res, f[j]+(i-j)*(i-j))
		}
		f[i] = res + costs[i-1]
	}
	return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nK)$，其中 $K=3$ 是最多可以跳的台阶数。
- 空间复杂度：$\mathcal{O}(n)$。

## 五、空间优化

类似 [70. 爬楼梯](https://leetcode.cn/problems/climbing-stairs/)，用三个变量滚动计算。原理见 [我的题解](https://leetcode.cn/problems/climbing-stairs/solutions/2560716/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-7zm1/)。

```py [sol-Python3]
class Solution:
    def climbStairs(self, _, costs: List[int]) -> int:
        f0 = f1 = f2 = 0
        for c in costs:
            f0, f1, f2 = f1, f2, min(f0 + 9, f1 + 4, f2 + 1) + c
        return f2
```

```py [sol-Python3 手写 min]
class Solution:
    def climbStairs(self, _, costs: List[int]) -> int:
        f0 = f1 = f2 = 0
        for c in costs:
            mn = f0 + 9
            if (t := f1 + 4) < mn: mn = t
            if (t := f2 + 1) < mn: mn = t
            f0, f1, f2 = f1, f2, mn + c
        return f2
```

```java [sol-Java]
class Solution {
    public int climbStairs(int n, int[] costs) {
        int f0 = 0, f1 = 0, f2 = 0;
        for (int c : costs) {
            int newF = Math.min(Math.min(f0 + 9, f1 + 4), f2 + 1) + c;
            f0 = f1;
            f1 = f2;
            f2 = newF;
        }
        return f2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int climbStairs(int, vector<int>& costs) {
        int f0 = 0, f1 = 0, f2 = 0;
        for (int c : costs) {
            // 另一种写法是 min({f0 + 9, f1 + 4, f2 + 1})
            int new_f = min(min(f0 + 9, f1 + 4), f2 + 1) + c;
            f0 = f1;
            f1 = f2;
            f2 = new_f;
        }
        return f2;
    }
};
```

```go [sol-Go]
func climbStairs(_ int, costs []int) int {
	var f0, f1, f2 int
	for _, c := range costs {
		f0, f1, f2 = f1, f2, min(f0+9, f1+4, f2+1)+c
	}
	return f2
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nK)$，其中 $K=3$ 是最多可以跳的台阶数。
- 空间复杂度：$\mathcal{O}(K)$。

本题有两个变形：

1. 去掉 $K=3$ 的约束（可以爬任意多级台阶），这可以用斜率优化，时间复杂度 $\mathcal{O}(n)$。
2. 额外传入一个参数 $K$，这可以用 [李超线段树](https://oi-wiki.org/ds/li-chao-tree/)，时间复杂度 $\mathcal{O}(n\log^2 n)$。

## 专题训练

1. 动态规划题单的「**§1.1 爬楼梯**」。
2. 动态规划题单的「**§11.7 斜率优化 DP**」。

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

本题来自 `一、入门 DP / §1.1 爬楼梯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、入门 DP / §1.1 爬楼梯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
