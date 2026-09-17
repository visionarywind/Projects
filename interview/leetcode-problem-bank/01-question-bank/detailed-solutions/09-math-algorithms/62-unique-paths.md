# 62. 不同路径

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/unique-paths/
- 题目 slug：`unique-paths`
- 来源专题：数学算法
- 来源分类路径：二、组合数学 / §2.2 组合计数
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/unique-paths/solutions/3062432/liang-chong-fang-fa-dong-tai-gui-hua-zu-o5k32/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：动态规划 / 组合数学（Python/Java/C++/Go）](https://leetcode.cn/problems/unique-paths/solutions/3062432/liang-chong-fang-fa-dong-tai-gui-hua-zu-o5k32/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-dong-tai-gui-hua-zu-o5k32`
- topic id：`3062432`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 一、寻找子问题

![lc62.png](https://pic.leetcode.cn/1738544636-iTMEwV-lc62.png)

看上图，假设左上角的坐标是 $(0,0)$，右下角的坐标是 $(2,6)$。

想一想，**最后一步**发生了什么？

- 如果从 $(1,6)$ 向下走到终点 $(2,6)$，那么要解决的问题是从起点 $(0,0)$ 走到 $(1,6)$ 的路径数。
- 如果从 $(2,5)$ 向右走到终点 $(2,6)$，那么要解决的问题是从起点 $(0,0)$ 走到 $(2,5)$ 的路径数。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

## 二、状态定义与状态转移方程

根据上面的讨论，定义状态为 $\textit{dfs}(i,j)$，表示从起点 $(0,0)$ 走到 $(i,j)$ 的路径数。

讨论我们是如何到达 $(i,j)$ 的：

- 如果是从 $(i-1,j)$ 过来，那么问题变成从起点 $(0,0)$ 走到 $(i-1,j)$ 的路径数，即 $\textit{dfs}(i-1,j)$。
- 如果是从 $(i,j-1)$ 过来，那么问题变成从起点 $(0,0)$ 走到 $(i,j-1)$ 的路径数，即 $\textit{dfs}(i,j-1)$。

这两种情况互斥，根据**加法原理**，有

$$
\textit{dfs}(i,j) = \textit{dfs}(i-1,j) + \textit{dfs}(i,j-1)
$$

**递归边界**：

- $\textit{dfs}(-1,j)=\textit{dfs}(i,-1)=0$。无法从 $(0,0)$ 到达这些位置。
- $\textit{dfs}(0,0)=1$。起点到它自己有一条路径，即原地不动。

**递归入口**：$\textit{dfs}(m-1,n-1)$，这是原问题，也是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。本题由于方案数不为 $0$，也可以初始化成 $0$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def uniquePaths(self, m: int, n: int) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（一行代码实现记忆化）
        def dfs(i: int, j: int) -> int:
            if i < 0 or j < 0:
                return 0
            if i == 0 and j == 0:
                return 1
            return dfs(i - 1, j) + dfs(i, j - 1)
        return dfs(m - 1, n - 1)
```

```py [sol-Python3 写法二]
class Solution:
    @cache
    def uniquePaths(self, m: int, n: int) -> int:
        if m == 0 or n == 0:
            return 0
        if m == 1 and n == 1:
            return 1
        return self.uniquePaths(m - 1, n) + self.uniquePaths(m, n - 1)
```

```java [sol-Java]
class Solution {
    public int uniquePaths(int m, int n) {
        int[][] memo = new int[m][n];
        return dfs(m - 1, n - 1, memo);
    }

    private int dfs(int i, int j, int[][] memo) {
        if (i < 0 || j < 0) {
            return 0;
        }
        if (i == 0 && j == 0) {
            return 1;
        }
        if (memo[i][j] != 0) { // 之前计算过
            return memo[i][j];
        }
        return memo[i][j] = dfs(i - 1, j, memo) + dfs(i, j - 1, memo);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int uniquePaths(int m, int n) {
        vector memo(m, vector<int>(n));
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i < 0 || j < 0) {
                return 0;
            }
            if (i == 0 && j == 0) {
                return 1;
            }
            int& res = memo[i][j]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }
            return res = dfs(i - 1, j) + dfs(i, j - 1);
        };
        return dfs(m - 1, n - 1);
    }
};
```

```go [sol-Go]
func uniquePaths(m, n int) int {
    memo := make([][]int, m)
    for i := range memo {
        memo[i] = make([]int, n)
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i < 0 || j < 0 {
            return 0
        }
        if i == 0 && j == 0 {
            return 1
        }
        p := &memo[i][j]
        if *p == 0 { // 没有计算过
            *p = dfs(i-1, j) + dfs(i, j-1)
        }
        return *p
    }
    return dfs(m-1, n-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(mn)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(mn)$。
- 空间复杂度：$\mathcal{O}(mn)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i+1][j+1]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示从起点 $(0,0)$ 走到 $(i,j)$ 的方案数。这里 $+1$ 是为了把 $\textit{dfs}(-1,j)$ 和 $\textit{dfs}(i,-1)$ 这些状态也翻译过来，这样我们可以把 $f[0][j]$ 和 $f[i][0]$ 作为初始值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i+1][j+1] = f[i][j+1] + f[i+1][j]
$$

初始值：

- $f[0][j]=f[i][0] = 0$，翻译自递归边界 $\textit{dfs}(-1,j)=\textit{dfs}(i,-1)=0$。
- $f[1][1]=1$，翻译自递归边界 $\textit{dfs}(0,0)=1$。

答案为 $f[m][n]$，翻译自递归入口 $\textit{dfs}(m-1,n-1)$。

```py [sol-Python3]
class Solution:
    def uniquePaths(self, m: int, n: int) -> int:
        f = [[0] * (n + 1) for _ in range(m + 1)]
        for i in range(m):
            for j in range(n):
                if i == j == 0:
                    f[1][1] = 1
                else:
                    f[i + 1][j + 1] = f[i][j + 1] + f[i + 1][j]
        return f[m][n]
```

```java [sol-Java]
class Solution {
    public int uniquePaths(int m, int n) {
        int[][] f = new int[m + 1][n + 1];
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (i == 0 && j == 0) {
                    f[1][1] = 1;
                } else {
                    f[i + 1][j + 1] = f[i][j + 1] + f[i + 1][j];
                }
            }
        }
        return f[m][n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int uniquePaths(int m, int n) {
        vector f(m + 1, vector<int>(n + 1));
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (i == 0 && j == 0) {
                    f[1][1] = 1;
                } else {
                    f[i + 1][j + 1] = f[i][j + 1] + f[i + 1][j];
                }
            }
        }
        return f[m][n];
    }
};
```

```go [sol-Go]
func uniquePaths(m, n int) int {
	f := make([][]int, m+1)
	for i := range f {
		f[i] = make([]int, n+1)
	}
	for i := range m {
		for j := range n {
			if i == 0 && j == 0 {
				f[1][1] = 1
			} else {
				f[i+1][j+1] = f[i][j+1] + f[i+1][j]
			}
		}
	}
	return f[m][n]
}
```

也可以把 $f[0][1]$ 初始化成 $1$，这样我们无需单独计算 $f[1][1]$。从 $\textit{dfs}$ 的角度理解，就是把 $(-1,0)$ 当作起点，且第一步只能往下走。

```py [sol-Python3]
class Solution:
    def uniquePaths(self, m: int, n: int) -> int:
        f = [[0] * (n + 1) for _ in range(m + 1)]
        f[0][1] = 1
        for i in range(m):
            for j in range(n):
                f[i + 1][j + 1] = f[i][j + 1] + f[i + 1][j]
        return f[m][n]
```

```java [sol-Java]
class Solution {
    public int uniquePaths(int m, int n) {
        int[][] f = new int[m + 1][n + 1];
        f[0][1] = 1;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                f[i + 1][j + 1] = f[i][j + 1] + f[i + 1][j];
            }
        }
        return f[m][n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int uniquePaths(int m, int n) {
        vector f(m + 1, vector<int>(n + 1));
        f[0][1] = 1;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                f[i + 1][j + 1] = f[i][j + 1] + f[i + 1][j];
            }
        }
        return f[m][n];
    }
};
```

```go [sol-Go]
func uniquePaths(m, n int) int {
    f := make([][]int, m+1)
    for i := range f {
        f[i] = make([]int, n+1)
    }
    f[0][1] = 1
    for i := range m {
        for j := range n {
            f[i+1][j+1] = f[i][j+1] + f[i+1][j]
        }
    }
    return f[m][n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$。
- 空间复杂度：$\mathcal{O}(mn)$。

## 五、空间优化

回顾上面的代码，我们在一行一行地计算 $f[i][j]$：算完 $f[i]$，然后算 $f[i+1]$，再算 $f[i+2]$。

算到 $f[i+2]$ 的时候，$f[i]$ 就再也用不到了。看上去，这里浪费了很多空间。

能不能只用一个长为 $n+1$ 的数组呢？

好比在一面墙上画画，原来这面墙画的是 $f[i]$，现在要画一副新的画，把原来的画**覆盖**掉，新的画叫做 $f[i+1]$。

在这个「覆盖」的过程中，对于这面墙的其中一个点 $f[i+1][j+1]$，我们要用 $f[i+1][j+1]$ 覆盖掉 $f[i][j+1]$。

怎么覆盖？看转移方程 $f[i+1][j+1] = f[i][j+1] + f[i+1][j]$。其中 $f[i][j+1]$ 就地取材，$f[i+1][j]$ 是新画的内容，从这面墙的下标 $j$ 处取到。所以空间优化后是 $f[j+1] = f[j+1] + f[j]$，也就是把 $f[j+1]$ 增加 $f[j]$。

```py [sol-Python3]
class Solution:
    def uniquePaths(self, m: int, n: int) -> int:
        f = [0] * (n + 1)
        f[1] = 1
        for _ in range(m):
            for j in range(n):
                f[j + 1] += f[j]
        return f[n]
```

```java [sol-Java]
class Solution {
    public int uniquePaths(int m, int n) {
        int[] f = new int[n + 1];
        f[1] = 1;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                f[j + 1] += f[j];
            }
        }
        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int uniquePaths(int m, int n) {
        vector<int> f(n + 1);
        f[1] = 1;
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                f[j + 1] += f[j];
            }
        }
        return f[n];
    }
};
```

```go [sol-Go]
func uniquePaths(m, n int) int {
    f := make([]int, n+1)
    f[1] = 1
    for range m {
        for j := range n {
            f[j+1] += f[j]
        }
    }
    return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 六、另一种方法：组合数学

![lc62.png](https://pic.leetcode.cn/1738544636-iTMEwV-lc62.png)

再来看示例 1，我们会往下走 $2$ 步，往右走 $6$ 步，一共走 $8$ 步。

比如其中一种走法为

$$
右右下右右右下右
$$

这相当于有 $8$ 个位置，从中选 $2$ 个位置填「下」

$$
\underline{\phantom{右右}}下\underline{\phantom{右右右}}下\underline{\phantom{右}}
$$

确定哪里填「下」，其余位置必然填「右」。

所以只需要计算填「下」的方案数，也就是从 $8$ 个位置选 $2$ 个位置的组合数

$$
\binom 8 2 = \dfrac{8\times 7}{2\times 1} = 28
$$

一般地，我们会往下走 $m-1$ 步，往右走 $n-1$ 步，一共走 $m+n-2$ 步。

从 $m+n-2$ 个位置选 $m-1$ 个位置填「下」，组合数为

$$
\binom {m+n-2} {m-1} = \dfrac{(m+n-2)!}{(m-1)!(n-1)!}
$$

### 组合数的计算方法

本题保证答案小于等于 $2\times 10^9$，可以用简单的循环计算组合数。

组合数的计算公式为

$$
\binom n i = \dfrac{n!}{i!(n-i)!}
$$

把 $i$ 替换成 $i-1$，可得

$$
\binom n {i-1} = \dfrac{n!}{(i-1)!(n-i+1)!}
$$

对比上面两个等式，可以得到如下递推式

$$
\binom n i = \dfrac{\binom n {i-1} (n-i+1)}{i}
$$

由于等式左边 $\dbinom n i$ 是整数，所以等式右边的除法一定能整除。

例如

$$
\binom 9 4 = \dfrac{9\times 8\times 7\times 6}{1\times 2\times 3\times 4}
$$

可以先计算 $\dfrac{9}{1} = 9$，然后计算 $\dfrac{9\times 8}{2} = 36$，然后计算 $\dfrac{36\times 7}{3} = 84$，最后计算 $\dfrac{84\times 6}{4} = 126$。

> 注：也可以这样理解，由于任意连续 $i$ 个数中必然有 $i$ 的倍数，所以上述计算过程均为整除，不会产生小数。

所以可以写一个简单的循环，计算

$$
\binom n k = \dfrac{n\times (n-1)\times \cdots \times (n+1-k)}{1\times 2\times \cdots\times k}
$$

如果 $k$ 比较大，可以用组合数恒等式

$$
\binom n k = \binom n {n-k}
$$

减少循环次数。

```py [sol-Python3]
class Solution:
    def uniquePaths(self, m: int, n: int) -> int:
        return comb(m + n - 2, m - 1)
```

```py [sol-Python3 循环]
def comb(n: int, k: int) -> int:
    k = min(k, n - k)
    res = 1
    for i in range(1, k + 1):
        res = res * (n + 1 - i) // i
    return res

class Solution:
    def uniquePaths(self, m: int, n: int) -> int:
        return comb(m + n - 2, m - 1)
```

```java [sol-Java]
class Solution {
    public int uniquePaths(int m, int n) {
        return (int) comb(m + n - 2, m - 1);
    }

    private long comb(int n, int k) {
        k = Math.min(k, n - k);
        long res = 1;
        for (int i = 1; i <= k; i++) {
            res = res * (n + 1 - i) / i;
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
    long long comb(int n, int k) {
        k = min(k, n - k);
        long long res = 1;
        for (int i = 1; i <= k; i++) {
            res = res * (n + 1 - i) / i;
        }
        return res;
    }

public:
    int uniquePaths(int m, int n) {
        return comb(m + n - 2, m - 1);
    }
};
```

```go [sol-Go]
func comb(n, k int) int {
    k = min(k, n-k)
    res := 1
    for i := 1; i <= k; i++ {
        res = res * (n + 1 - i) / i
    }
    return res
}

func uniquePaths(m, n int) int {
    return comb(m+n-2, m-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\min(m,n))$。
- 空间复杂度：$\mathcal{O}(1)$。

## 进阶问题

如果网格图中有障碍物，要怎么做？

这题是 [63. 不同路径 II](https://leetcode.cn/problems/unique-paths-ii/)。

## 专题训练

1. 动态规划题单的「**二、网格图 DP**」。
2. 数学题单的「**§2.2 组合计数**」。

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

本题来自 `二、组合数学 / §2.2 组合计数`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、组合数学 / §2.2 组合计数`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
