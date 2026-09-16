# 1595. 连通两组点的最小成本

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-to-connect-two-groups-of-points/
- 题目 slug：`minimum-cost-to-connect-two-groups-of-points`
- 来源专题：动态规划
- 来源分类路径：九、状态压缩 DP（状压 DP） / §9.7 其他状压 DP
- 难度分：2538
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-to-connect-two-groups-of-points/solutions/2314687/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-djxq/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考动态规划！（附题单）](https://leetcode.cn/problems/minimum-cost-to-connect-two-groups-of-points/solutions/2314687/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-djxq/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-djxq`
- topic id：`2314687`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识：位运算

详见 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

## 前置知识：动态规划入门

详见 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)

> 制作不易，欢迎点赞！APP 用户如果无法打开，可以分享到微信。

## 一、寻找子问题

下文中 $n=\textit{size}_1$，$m=\textit{size}_2$。

假设 $n=5,m=3$。第一组的点编号为 $0,1,2,3,4$，第二组的点编号为 $0,1,2$。

用「枚举选哪个」来思考。考虑第一组的最后一个点（$4$），可以枚举它和第二组的 $0,1,2$ 相连。假设和第二组的 $1$ 相连，那么问题变成：「第一组的 $0,1,2,3$ 和第二组的 $0,1,2$ 相连，且第二组的 $0,2$ 未被连接时，最小成本是多少」。然后考虑第一组的点 $3$ 和第二组的哪个相连（可以连之前连过的点），接着考虑第一组的点 $2$，点 $1$，最后是点 $0$。

第一组的点全部连完后，第二组的某些点可能未被连接，这些点可以去第一组找个成本最小的点连上。

上述做法枚举了第一组的每个点连接第二组的所有情况，并在最后用贪心的思路处理第二组剩余没有连的点，所以一定可以算出（枚举出）最优解。

## 二、状态定义与状态转移方程

根据上面的讨论，定义 $\textit{dfs}(i,j)$ 表示第一组的 $0,1,\ldots,i$ 和第二组的 $0,1,\ldots,m-1$ 相连，且第二组的集合 $j$ 未被连接时，最小成本是多少。

枚举第一组的点 $i$ 和第二组的 $0,1,\ldots,m-1$ 其中一个点相连，取最小值，即

$$
\textit{dfs}(i,j) =\min_{k=0}^{m-1} \textit{dfs}(i-1,j\setminus\{k\}) + \textit{cost}[i][k]
$$

其中 $j\setminus\{k\}$ 表示从集合 $j$ 中去掉元素 $k$ 后的集合。注意，如果 $k$ 不在 $j$ 中，那么 $j$ 不变。

递归边界：设第二组的点 $x$ 与第一组的点连接时，最小成本是 $\textit{minCost}[x]$，那么有

$$
\textit{dfs}(-1,j) = \sum_{k\in j} \textit{minCost}[k]
$$

递归入口：$\textit{dfs}(n-1,\{0,1,\ldots,m-1\})$。

代码实现时，$\textit{minCost}$ 数组可以提前预处理出来。

### 记忆化搜索

下文用 $x-y$ 表示第一组的点 $x$ 连第二组的点 $y$。

假设 $n=5,m=3$。由于无论是先 $4-0$ 再 $3-1$，还是先 $4-1$ 再 $3-0$，都会递归到 $\textit{dfs}(2,\{2\})$ 这个状态上。一叶知秋，整个递归中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组（或哈希表）中。
- 如果一个状态不是第一次遇到，那么直接返回 $\textit{memo}$ 中保存的结果。

### 位运算

代码实现时，需要把上述集合语言用位运算实现，见 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

### 答疑

**问**：能不能枚举第二组的点，去连接第一组的点？

**答**：也可以，但这样做的时间复杂度是 $\mathcal{O}(nm2^n)$，相比 $\mathcal{O}(nm2^m)$ 更慢。注意本题 $n\ge m$。

```py [sol-Python3]
class Solution:
    def connectTwoGroups(self, cost: List[List[int]]) -> int:
        n, m = len(cost), len(cost[0])
        min_cost = [min(col) for col in zip(*cost)]  # 每一列的最小值

        @cache  # 记忆化搜索
        def dfs(i: int, j: int) -> int:
            if i < 0:
                return sum(c for k, c in enumerate(min_cost) if j >> k & 1)
            return min(dfs(i - 1, j & ~(1 << k)) + c
                       for k, c in enumerate(cost[i]))  # 第一组的点 i 与第二组的点 k
        return dfs(n - 1, (1 << m) - 1)
```

```java [sol-Java]
class Solution {
    private List<List<Integer>> cost;
    private int[] minCost;
    private int[][] memo;

    public int connectTwoGroups(List<List<Integer>> cost) {
        this.cost = cost;
        int n = cost.size(), m = cost.get(0).size();
        minCost = new int[m];
        Arrays.fill(minCost, Integer.MAX_VALUE);
        for (int j = 0; j < m; j++)
            for (var c : cost)
                minCost[j] = Math.min(minCost[j], c.get(j));

        memo = new int[n][1 << m];
        for (int i = 0; i < n; i++)
            Arrays.fill(memo[i], -1); // -1 表示没有计算过
        return dfs(n - 1, (1 << m) - 1);
    }

    private int dfs(int i, int j) {
        if (i < 0) {
            int res = 0;
            for (int k = 0; k < minCost.length; k++)
                if ((j >> k & 1) == 1) // 第二组的点 k 未连接
                    res += minCost[k]; // 去第一组找个成本最小的点连接
            return res;
        }
        if (memo[i][j] != -1) return memo[i][j]; // 之前算过了
        int res = Integer.MAX_VALUE;
        for (int k = 0; k < minCost.length; k++) // 第一组的点 i 与第二组的点 k
            res = Math.min(res, dfs(i - 1, j & ~(1 << k)) + cost.get(i).get(k));
        return memo[i][j] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int connectTwoGroups(vector<vector<int>> &cost) {
        int n = cost.size(), m = cost[0].size();
        vector<int> min_cost(m, INT_MAX);
        for (int j = 0; j < m; j++)
            for (auto &c: cost)
                min_cost[j] = min(min_cost[j], c[j]);

        vector<vector<int>> memo(n, vector<int>(1 << m, INT_MAX));
        function<int(int, int)> dfs = [&](int i, int j) -> int {
            if (i < 0) {
                int res = 0;
                for (int k = 0; k < m; k++)
                    if (j >> k & 1) // 第二组的点 k 未连接
                        res += min_cost[k]; // 去第一组找个成本最小的点连接
                return res;
            }
            int &res = memo[i][j]; // 注意这里是引用
            if (res != INT_MAX) return res; // 之前算过了
            for (int k = 0; k < m; k++) // 第一组的点 i 与第二组的点 k
                res = min(res, dfs(i - 1, j & ~(1 << k)) + cost[i][k]);
            return res;
        };
        return dfs(n - 1, (1 << m) - 1);
    }
};
```

```go [sol-Go]
func connectTwoGroups(cost [][]int) int {
    n, m := len(cost), len(cost[0])
    minCost := make([]int, m)
    for j := 0; j < m; j++ {
        minCost[j] = math.MaxInt
        for _, c := range cost {
            minCost[j] = min(minCost[j], c[j])
        }
    }

    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, 1<<m)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) (res int) {
        if i < 0 {
            for k, c := range minCost {
                if j>>k&1 == 1 { // 第二组的点 k 未连接
                    res += c // 去第一组找个成本最小的点连接
                }
            }
            return
        }
        p := &memo[i][j]
        if *p != -1 { // 之前算过了
            return *p
        }
        res = math.MaxInt
        for k, c := range cost[i] { // 第一组的点 i 与第二组的点 k
            res = min(res, dfs(i-1, j&^(1<<k))+c)
        }
        *p = res // 记忆化
        return *p
    }
    return dfs(n-1, 1<<m-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm2^m)$，其中 $n$ 和 $m$ 分别为 $\textit{cost}$ 的行数和列数。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n2^m)$，单个状态的计算时间为 $\mathcal{O}(m)$，所以总的时间复杂度为 $\mathcal{O}(nm2^m)$。
- 空间复杂度：$\mathcal{O}(n2^m)$。

## 三、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

通用做法：

- $\textit{dfs}$ 改成 $f$ 数组；
- 递归改成循环；
- 递归边界改成 $f$ 的初始状态。

> 相当于原来是用递归去计算每个状态，现在是用循环去计算每个状态。

具体来说，$f[i][j]$ 的含义和状态转移方程和上面的记忆化搜索是一样的，即

$$
f[i][j] =\min_{k=0}^{m-1} f[i-1][j\setminus\{k\}] + \textit{cost}[i][k]
$$

但当 $i=0$ 时，等号右边会出现负数下标。或者说，**这种定义方式没有状态能表示递归边界**。

解决办法：在 $f$ 数组的上边加一排，把原来的 $f[i]$ 改成 $f[i+1]$，$f[i-1]$ 改成 $f[i]$。此时 $f[0][j]$ 就对应着 $\textit{dfs}(-1,j)$。

修改后的递推式为

$$
f[i+1][j] =\min_{k=0}^{m-1} f[i][j\setminus\{k\}] + \textit{cost}[i][k]
$$

注意只需要把 $f$ 中的 $i$ 加一，$\textit{cost}$ 中的 $i$ 不受影响。

初始值 $f[0][j]= \sum\limits_{k\in j} \textit{minCost}[k]$。

答案为 $f[n][\{0,1,\ldots,m-1\}]$。

```py [sol-Python3]
class Solution:
    def connectTwoGroups(self, cost: List[List[int]]) -> int:
        n, m = len(cost), len(cost[0])
        min_cost = [min(col) for col in zip(*cost)]  # 每一列的最小值

        f = [[0] * (1 << m) for _ in range(n + 1)]
        for j in range(1 << m):
            f[0][j] = sum(c for k, c in enumerate(min_cost) if j >> k & 1)

        for i, row in enumerate(cost):
            for j in range(1 << m):
                f[i + 1][j] = min(f[i][j & ~(1 << k)] + c
                                  for k, c in enumerate(row))  # 第一组的点 i 与第二组的点 k
        return f[n][-1]
```

```java [sol-Java]
class Solution {
    public int connectTwoGroups(List<List<Integer>> cost) {
        int n = cost.size(), m = cost.get(0).size();
        var minCost = new int[m];
        Arrays.fill(minCost, Integer.MAX_VALUE);
        for (int j = 0; j < m; j++)
            for (var c : cost)
                minCost[j] = Math.min(minCost[j], c.get(j));

        var f = new int[n + 1][1 << m];
        for (int j = 0; j < 1 << m; j++)
            for (int k = 0; k < m; k++)
                if ((j >> k & 1) == 1) // 第二组的点 k 未连接
                    f[0][j] += minCost[k]; // 去第一组找个成本最小的点连接

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < 1 << m; j++) {
                int res = Integer.MAX_VALUE;
                for (int k = 0; k < m; k++) // 第一组的点 i 与第二组的点 k
                    res = Math.min(res, f[i][j & ~(1 << k)] + cost.get(i).get(k));
                f[i + 1][j] = res;
            }
        }
        return f[n][(1 << m) - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int connectTwoGroups(vector<vector<int>> &cost) {
        int n = cost.size(), m = cost[0].size();
        vector<int> min_cost(m, INT_MAX);
        for (int j = 0; j < m; j++)
            for (auto &c: cost)
                min_cost[j] = min(min_cost[j], c[j]);

        vector<vector<int>> f(n + 1, vector<int>(1 << m));
        for (int j = 0; j < 1 << m; j++)
            for (int k = 0; k < m; k++)
                if (j >> k & 1) // 第二组的点 k 未连接
                    f[0][j] += min_cost[k]; // 去第一组找个成本最小的点连接

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < 1 << m; j++) {
                int res = INT_MAX;
                for (int k = 0; k < m; k++) // 第一组的点 i 与第二组的点 k
                    res = min(res, f[i][j & ~(1 << k)] + cost[i][k]);
                f[i + 1][j] = res;
            }
        }
        return f[n][(1 << m) - 1];
    }
};
```

```go [sol-Go]
func connectTwoGroups(cost [][]int) int {
    n, m := len(cost), len(cost[0])
    minCost := make([]int, m)
    for j := 0; j < m; j++ {
        minCost[j] = math.MaxInt
        for _, c := range cost {
            minCost[j] = min(minCost[j], c[j])
        }
    }

    f := make([][]int, n+1)
    for i := range f {
        f[i] = make([]int, 1<<m)
    }
    for j := 0; j < 1<<m; j++ {
        for k, c := range minCost {
            if j>>k&1 == 1 { // 第二组的点 k 未连接
                f[0][j] += c // 去第一组找个成本最小的点连接
            }
        }
    }

    for i, row := range cost {
        for j := 0; j < 1<<m; j++ {
            res := math.MaxInt
            for k, c := range row { // 第一组的点 i 与第二组的点 k
                res = min(res, f[i][j&^(1<<k)]+c)
            }
            f[i+1][j] = res
        }
    }
    return f[n][1<<m-1]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm2^m)$，其中 $n$ 和 $m$ 分别为 $\textit{cost}$ 的行数和列数。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n2^m)$，单个状态的计算时间为 $\mathcal{O}(m)$，所以总的时间复杂度为 $\mathcal{O}(nm2^m)$。
- 空间复杂度：$\mathcal{O}(n2^m)$。

## 四、两个优化

由于 $f[i+1]$ 只和 $f[i]$ 有关，因此可以仿照 [0-1 背包](https://www.bilibili.com/video/BV16Y411v7Y6/)，优化掉第一个维度，只用一个长为 $2^m$ 的一维数组。

去掉第一个维度后，考虑 $f$ 数组初始值的计算，还可以进一步优化。

假设现在算出了 $f[00],f[01],f[10],f[11]$（这里用二进制数表示集合），那么结合 $\textit{minCost}[2]$，可以算出

$$
f[100] = f[00] + \textit{minCost}[2]\\
f[101] = f[01] + \textit{minCost}[2]\\
f[110] = f[10] + \textit{minCost}[2]\\
f[111] = f[11] + \textit{minCost}[2]
$$

这样就得到了 $f[0]$ 到 $f[111]$。按照同样的方式，结合 $\textit{minCost}[3]$，可以算出 $f[0]$ 到 $f[1111]$。依此类推。

这样每个初始值只需要 $\mathcal{O}(1)$ 的时间就能递推算出来。

```py [sol-Python3]
class Solution:
    def connectTwoGroups(self, cost: List[List[int]]) -> int:
        f = [0] * (1 << len(cost[0]))
        for j, mn in enumerate(map(min, zip(*cost))):
            bit = 1 << j
            for mask in range(bit):
                f[bit | mask] = f[mask] + mn
        for row in cost:
            for j in range(len(f) - 1, -1, -1):
                f[j] = min(f[j & ~(1 << k)] + c for k, c in enumerate(row))
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int connectTwoGroups(List<List<Integer>> cost) {
        int m = cost.get(0).size();
        var f = new int[1 << m];
        for (int j = 0; j < m; j++) {
            int mn = Integer.MAX_VALUE;
            for (var c : cost)
                mn = Math.min(mn, c.get(j));
            int bit = 1 << j;
            for (int mask = 0; mask < bit; mask++)
                f[bit | mask] = f[mask] + mn;
        }
        for (var row : cost) {
            var r = row.toArray(Integer[]::new); // 转成数组效率更高
            for (int j = (1 << m) - 1; j >= 0; j--) {
                int res = Integer.MAX_VALUE;
                for (int k = 0; k < m; k++) // 第一组的点 i 与第二组的点 k
                    res = Math.min(res, f[j & ~(1 << k)] + r[k]);
                f[j] = res;
            }
        }
        return f[(1 << m) - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int connectTwoGroups(vector<vector<int>> &cost) {
        int m = cost[0].size(), f[1 << m];
        f[0] = 0;
        for (int j = 0; j < m; j++) {
            int mn = INT_MAX;
            for (auto &c: cost)
                mn = min(mn, c[j]);
            int bit = 1 << j;
            for (int mask = 0; mask < bit; mask++)
                f[bit | mask] = f[mask] + mn;
        }
        for (auto &row: cost) {
            for (int j = (1 << m) - 1; j >= 0; j--) {
                int res = INT_MAX;
                for (int k = 0; k < m; k++) // 第一组的点 i 与第二组的点 k
                    res = min(res, f[j & ~(1 << k)] + row[k]);
                f[j] = res;
            }
        }
        return f[(1 << m) - 1];
    }
};
```

```go [sol-Go]
func connectTwoGroups(cost [][]int) int {
    m := len(cost[0])
    f := make([]int, 1<<m)
    for j := 0; j < m; j++ {
        mn := math.MaxInt
        for _, c := range cost {
            mn = min(mn, c[j])
        }
        bit := 1 << j
        for mask := 0; mask < bit; mask++ {
            f[bit|mask] = f[mask] + mn
        }
    }
    for _, row := range cost {
        for j := 1<<m - 1; j >= 0; j-- {
            res := math.MaxInt
            for k, c := range row { // 第一组的点 i 与第二组的点 k
                res = min(res, f[j&^(1<<k)]+c)
            }
            f[j] = res
        }
    }
    return f[1<<m-1]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm2^m)$，其中 $n$ 和 $m$ 分别为 $\textit{cost}$ 的行数和列数。计算 $f$ 数组的初始值，循环次数为 $2^0+2^1+2^2+\cdots+2^{m-1} = 2^m-1$，这比优化前的 $m2^m$ 要快。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n2^m)$，单个状态的计算时间为 $\mathcal{O}(m)$，所以总的时间复杂度为 $\mathcal{O}(nm2^m)$。
- 空间复杂度：$\mathcal{O}(2^m)$。

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

本题来自 `九、状态压缩 DP（状压 DP） / §9.7 其他状压 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、状态压缩 DP（状压 DP） / §9.7 其他状压 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
