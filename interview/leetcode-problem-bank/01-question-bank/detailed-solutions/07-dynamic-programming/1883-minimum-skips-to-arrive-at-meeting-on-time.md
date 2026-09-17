# 1883. 准时抵达会议现场的最小跳过休息次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-skips-to-arrive-at-meeting-on-time/
- 题目 slug：`minimum-skips-to-arrive-at-meeting-on-time`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：2588
- 外部题解来源：https://leetcode.cn/problems/minimum-skips-to-arrive-at-meeting-on-time/solutions/2746611/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-gxd2/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考动态规划：从记忆化搜索到递推（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-skips-to-arrive-at-meeting-on-time/solutions/2746611/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-gxd2/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-gxd2`
- topic id：`2746611`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

## 一、寻找子问题

考虑**枚举**最小跳过次数。由于 $\textit{dist}[n-1]$ 无需跳过，我们从 $\textit{dist}[n-2]$ 开始思考。

以示例 2 的 $\textit{dist}=[7,3,5,5]$ 为例，如果最多跳 $2$ 次，用「选或不选」分类讨论：

- 不跳过 $\textit{dist}[n-2]=5$：问题变成在最多跳过 $2$ 次的情况下，经过 $[7,3]$ 需要的最小时间。
- 跳过 $\textit{dist}[n-2]=5$：问题变成在最多跳过 $1$ 次的情况下，经过 $[7,3]$ 需要的最小时间。

由于是否跳过都会把原问题变成一个**和原问题相似的、规模更小的子问题**，所以可以用**递归**解决。

> 注：动态规划有「选或不选」和「枚举选哪个」两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「选或不选」。

## 二、状态定义与状态转移

因为要解决的问题都形如「在最多跳过 $i$ 次的情况下，从 $\textit{dist}[0]$ 到 $\textit{dist}[j]$ 需要的最小时间」，所以把它定义成 $\textit{dfs}(i,j)$。请注意，如果在 $j$ 处选择不跳过的话，$\textit{dfs}(i,j)$ 需包含在 $j$ 处等待的时间。

考虑 $\textit{dist}[j]$ 是否跳过：

- 不跳过：先算出在最多跳过 $i$ 次的情况下，从 $\textit{dist}[0]$ 到 $\textit{dist}[j-1]$ 需要的最小时间，即 $\textit{dfs}(i,j-1)$，然后加上 $\textit{dist}[j]$ 需要的时间，再休息并等待（上取整），得 $\textit{dfs}(i,j) = \left\lceil \textit{dfs}(i,j-1) + \dfrac{\textit{dist}[j]}{\textit{speed}}\right\rceil$。
- 跳过：先算出在最多跳过 $i-1$ 次的情况下，从 $\textit{dist}[0]$ 到 $\textit{dist}[j-1]$ 需要的最小时间，即 $\textit{dfs}(i-1,j-1)$，然后加上 $\textit{dist}[j]$ 需要的时间，得 $\textit{dfs}(i,j) =  \textit{dfs}(i-1,j-1) + \dfrac{\textit{dist}[j]}{\textit{speed}}$。

这两种情况取最小值，就得到了 $\textit{dfs}(i,j)$，即

$$
\textit{dfs}(i,j) = \min\left(\left\lceil \textit{dfs}(i,j-1) + \dfrac{\textit{dist}[j]}{\textit{speed}}\right\rceil, \textit{dfs}(i-1,j-1) + \dfrac{\textit{dist}[j]}{\textit{speed}}\right)
$$

递归边界：$\textit{dfs}(i,-1)=0$。

递归入口：$\textit{dfs}(i,n-2)$。注意 $\textit{dist}[n-1]$ 无需跳过，单独计算。

最小的满足 

$$
\textit{dfs}(i,n-2) + \dfrac{\textit{dist}[n-1]}{\textit{speed}} \le \textit{hoursBefore}
$$ 

的 $i$ 就是答案。

## 三、改进：避免浮点误差

注意上面的计算过程包含除法，所以 $\textit{dfs}(i,j)$ 不一定是个整数。但如果使用浮点数存储 $\textit{dfs}(i,j)$ 的话，会因为**舍入误差**导致计算结果可能与实际不符。为避免浮点运算带来的误差，可以把 $\textit{dfs}(i,j)$ 的定义改成在同等时间下用 $\textit{speed}$ 速度能走的**距离**。换句话说，在最多跳过 $i$ 次的情况下，从 $\textit{dist}[0]$ 到 $\textit{dist}[j]$ 需要的最小时间是 $\dfrac{\textit{dfs}(i,j)}{\textit{speed}}$。

考虑 $\textit{dist}[j]$ 是否跳过：

- 不跳过：用时为 $\left\lceil \dfrac{\textit{dfs}(i,j-1)}{\textit{speed}} + \dfrac{\textit{dist}[j]}{\textit{speed}}\right\rceil$，所以有 $\textit{dfs}(i,j) = \left\lceil  \dfrac{\textit{dfs}(i,j-1) +\textit{dist}[j]}{\textit{speed}}\right\rceil\cdot \textit{speed}$
- 跳过：用时为 $ \dfrac{\textit{dfs}(i-1,j-1)}{\textit{speed}} + \dfrac{\textit{dist}[j]}{\textit{speed}}$，所以有 $\textit{dfs}(i,j) =  \dfrac{\textit{dfs}(i-1,j-1)+\textit{dist}[j]}{\textit{speed}} \cdot \textit{speed} = \textit{dfs}(i-1,j-1) + \textit{dist}[j]$。

二者取最小值，状态转移变成

$$
\textit{dfs}(i,j) = \min\left(\left\lceil  \dfrac{\textit{dfs}(i,j-1) +\textit{dist}[j]}{\textit{speed}}\right\rceil\cdot \textit{speed}, \textit{dfs}(i-1,j-1) + \textit{dist}[j]\right)
$$

最小的满足

$$
\dfrac{\textit{dfs}(i,n-2)}{\textit{speed}} + \dfrac{\textit{dist}[n-1]}{\textit{speed}} \le  \textit{hoursBefore}
$$

即

$$
\textit{dfs}(i,n-2) + \textit{dist}[n-1] \le \textit{speed}\cdot \textit{hoursBefore}
$$

的 $i$ 就是答案。

这样就完全不涉及浮点运算了！

在递归前，可以先判断是否满足 $\sum\limits_{i=0}^{n-1} \textit{dist}[i]\le \textit{speed}\cdot \textit{hoursBefore}$，如果不满足则返回 $-1$。

代码实现时，利用如下恒等式把上取整转化成计算机更容易计算的下取整。

$$
\left\lceil\dfrac{a}{b}\right\rceil = \left\lfloor\dfrac{a+b-1}{b}\right\rfloor
$$

证明见 [上取整下取整转换公式的证明](https://zhuanlan.zhihu.com/p/1890356682149838951)。

关于**记忆化搜索**，请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含如何把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def minSkips(self, dist: List[int], speed: int, hoursBefore: int) -> int:
        if sum(dist) > speed * hoursBefore:
            return -1

        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int, j: int) -> int:
            if j < 0:
                return 0
            res = (dfs(i, j - 1) + dist[j] + speed - 1) // speed * speed
            if i:
                res = min(res, dfs(i - 1, j - 1) + dist[j])
            return res

        for i in count(0):
            if dfs(i, len(dist) - 2) + dist[-1] <= speed * hoursBefore:
                return i
```

```java [sol-Java]
class Solution {
    public int minSkips(int[] dist, int speed, int hoursBefore) {
        int sumDist = 0;
        for (int d : dist) {
            sumDist += d;
        }
        if (sumDist > (long) speed * hoursBefore) {
            return -1;
        }

        int n = dist.length;
        int[][] memo = new int[n][n];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        for (int i = 0; ; i++) {
            if (dfs(i, n - 2, memo, dist, speed) + dist[n - 1] <= (long) speed * hoursBefore) {
                return i;
            }
        }
    }

    private int dfs(int i, int j, int[][] memo, int[] dist, int speed) {
        if (j < 0) { // 递归边界
            return 0;
        }
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }
        int res = (dfs(i, j - 1, memo, dist, speed) + dist[j] + speed - 1) / speed * speed;
        if (i > 0) {
            res = Math.min(res, dfs(i - 1, j - 1, memo, dist, speed) + dist[j]);
        }
        return memo[i][j] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSkips(vector<int>& dist, int speed, int hoursBefore) {
        if (accumulate(dist.begin(), dist.end(), 0) > (long long) speed * hoursBefore) {
            return -1;
        }
        int n = dist.size();
        vector<vector<int>> memo(n, vector<int>(n, -1)); // -1 表示没有计算过
        function<int(int, int)> dfs = [&](int i, int j) -> int {
            if (j < 0) { // 递归边界
                return 0;
            }
            auto& res = memo[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            res = (dfs(i, j - 1) + dist[j] + speed - 1) / speed * speed;
            if (i) res = min(res, dfs(i - 1, j - 1) + dist[j]);
            return res;
        };
        for (int i = 0; ; i++) {
            if (dfs(i, n - 2) + dist[n - 1] <= (long long) speed * hoursBefore) {
                return i;
            }
        }
    }
};
```

```go [sol-Go]
func minSkips(dist []int, speed, hoursBefore int) int {
    sumDist := 0
    for _, d := range dist {
        sumDist += d
    }
    if sumDist > speed*hoursBefore {
        return -1
    }

    n := len(dist)
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, n)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if j < 0 { // 递归边界
            return 0
        }
        p := &memo[i][j]
        if *p != -1 { // 之前计算过
            return *p
        }
        res := (dfs(i, j-1) + dist[j] + speed - 1) / speed * speed
        if i > 0 {
            res = min(res, dfs(i-1, j-1)+dist[j])
        }
        *p = res // 记忆化
        return res
    }
    for i := 0; ; i++ {
        if dfs(i, n-2)+dist[n-1] <= speed*hoursBefore {
            return i
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{dist}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n^2)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。有多少个状态，$\textit{memo}$ 数组的大小就是多少。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示在最多跳过 $i$ 次的情况下，从 $\textit{dist}[0]$ 到 $\textit{dist}[j]$ 需要的最小时间，再乘上 $\textit{speed}$。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i][j] = \min\left(\left\lceil  \dfrac{f[i][j-1] +\textit{dist}[j]}{\textit{speed}}\right\rceil\cdot \textit{speed}, f[i-1][j-1] + \textit{dist}[j]\right)
$$

但是，这种定义方式**没有状态能表示递归边界**，即 $j=-1$ 的情况。

解决办法：在二维数组 $f$ 的左侧**插入一列状态**，那么其余状态全部向右移动一位，$f$ 的下标 $j$ 需要加一，也就是把 $f[\cdot][j]$ 改为 $f[\cdot][j+1]$，把 $f[\cdot][j-1]$ 改为 $f[\cdot][j]$。

修改后 $f[i][j+1]$ 表示在最多跳过 $i$ 次的情况下，从 $\textit{dist}[0]$ 到 $\textit{dist}[j]$ 需要的最小时间，再乘上 $\textit{speed}$。此时 $f[i][0]$ 就对应递归边界了。

修改后的递推式为

$$
f[i][j+1] = \min\left(\left\lceil  \dfrac{f[i][j] +\textit{dist}[j]}{\textit{speed}}\right\rceil\cdot \textit{speed}, f[i-1][j] + \textit{dist}[j]\right)
$$

初始值 $f[i][0]=0$，翻译自递归边界 $\textit{dfs}(i,-1)=0$。

最小的满足

$$
f[i][n-1] + \textit{dist}[n-1] \le \textit{speed}\cdot \textit{hoursBefore}
$$

的 $i$ 就是答案。

### 答疑

**问**：为什么 $\textit{dist}[j]$ 的下标不用改？

**答**：既然是在 $f$ 的左侧插入一列状态，那么就只需要修改 $f$ 中的下标 $j$，其余任何逻辑都无需修改。或者说，如果把 $\textit{dist}[j]$ 也改成 $\textit{dist}[j+1]$，那么 $\textit{dist}[0]$ 就被我们给忽略掉了。

```py [sol-Python3]
class Solution:
    def minSkips(self, dist: List[int], speed: int, hoursBefore: int) -> int:
        if sum(dist) > speed * hoursBefore:
            return -1
        n = len(dist)
        f = [[0] * n for _ in range(n)]
        for i in count(0):
            for j, d in enumerate(dist[:-1]):
                f[i][j + 1] = (f[i][j] + d + speed - 1) // speed * speed
                if i:
                    f[i][j + 1] = min(f[i][j + 1], f[i - 1][j] + d)
            if f[i][-1] + dist[-1] <= speed * hoursBefore:
                return i
```

```java [sol-Java]
class Solution {
    public int minSkips(int[] dist, int speed, int hoursBefore) {
        int sumDist = 0;
        for (int d : dist) {
            sumDist += d;
        }
        if (sumDist > (long) speed * hoursBefore) {
            return -1;
        }

        int n = dist.length;
        int[][] f = new int[n][n];
        for (int i = 0; ; i++) {
            for (int j = 0; j < n - 1; j++) {
                f[i][j + 1] = (f[i][j] + dist[j] + speed - 1) / speed * speed;
                if (i > 0) {
                    f[i][j + 1] = Math.min(f[i][j + 1], f[i - 1][j] + dist[j]);
                }
            }
            if (f[i][n - 1] + dist[n - 1] <= (long) speed * hoursBefore) {
                return i;
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSkips(vector<int>& dist, int speed, int hoursBefore) {
        if (accumulate(dist.begin(), dist.end(), 0) > (long long) speed * hoursBefore) {
            return -1;
        }
        int n = dist.size();
        vector<vector<int>> f(n, vector<int>(n));
        for (int i = 0; ; i++) {
            for (int j = 0; j < n - 1; j++) {
                f[i][j + 1] = (f[i][j] + dist[j] + speed - 1) / speed * speed;
                if (i) {
                    f[i][j + 1] = min(f[i][j + 1], f[i - 1][j] + dist[j]);
                }
            }
            if (f[i][n - 1] + dist[n - 1] <= (long long) speed * hoursBefore) {
                return i;
            }
        }
    }
};
```

```go [sol-Go]
func minSkips(dist []int, speed, hoursBefore int) int {
    sumDist := 0
    for _, d := range dist {
        sumDist += d
    }
    if sumDist > speed*hoursBefore {
        return -1
    }

    n := len(dist)
    f := make([][]int, n)
    for i := 0; ; i++ {
        f[i] = make([]int, n)
        for j, d := range dist[:n-1] {
            f[i][j+1] = (f[i][j] + d + speed - 1) / speed * speed
            if i > 0 {
                f[i][j+1] = min(f[i][j+1], f[i-1][j]+d)
            }
        }
        if f[i][n-1]+dist[n-1] <= speed*hoursBefore {
            return i
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{dist}$ 的长度。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 五、空间优化

观察上面的状态转移方程，在计算 $f[i]$ 时，不会用到下标小于 $i-1$ 的状态。

所以可以去掉第一个维度，反复利用同一个长为 $n$ 的一维数组。

为避免状态被覆盖，可以用一个变量 $\textit{pre}$ 记录 $f[i-1][j]$。

```py [sol-Python3]
class Solution:
    def minSkips(self, dist: List[int], speed: int, hoursBefore: int) -> int:
        if sum(dist) > speed * hoursBefore:
            return -1
        f = [0] * len(dist)
        for i in count(0):
            pre = 0
            for j, d in enumerate(dist[:-1]):
                tmp = f[j + 1]
                f[j + 1] = (f[j] + d + speed - 1) // speed * speed
                if i:
                    f[j + 1] = min(f[j + 1], pre + d)
                pre = tmp
            if f[-1] + dist[-1] <= speed * hoursBefore:
                return i
```

```py [sol-Python3 写法二]
class Solution:
    def minSkips(self, dist: List[int], speed: int, hoursBefore: int) -> int:
        if sum(dist) > speed * hoursBefore:
            return -1
        f = [0] * len(dist)
        for i in count(0):
            pre = 0
            for j, d in enumerate(dist[:-1]):
                tmp = f[j + 1]
                f[j + 1] = (f[j] + d + speed - 1) // speed * speed
                if i and pre + d < f[j + 1]:
                    f[j + 1] = pre + d
                pre = tmp
            if f[-1] + dist[-1] <= speed * hoursBefore:
                return i
```

```java [sol-Java]
class Solution {
    public int minSkips(int[] dist, int speed, int hoursBefore) {
        int sumDist = 0;
        for (int d : dist) {
            sumDist += d;
        }
        if (sumDist > (long) speed * hoursBefore) {
            return -1;
        }

        int n = dist.length;
        int[] f = new int[n];
        for (int i = 0; ; i++) {
            int pre = 0;
            for (int j = 0; j < n - 1; j++) {
                int tmp = f[j + 1];
                f[j + 1] = (f[j] + dist[j] + speed - 1) / speed * speed;
                if (i > 0) {
                    f[j + 1] = Math.min(f[j + 1], pre + dist[j]);
                }
                pre = tmp;
            }
            if (f[n - 1] + dist[n - 1] <= (long) speed * hoursBefore) {
                return i;
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minSkips(vector<int>& dist, int speed, int hoursBefore) {
        if (accumulate(dist.begin(), dist.end(), 0) > (long long) speed * hoursBefore) {
            return -1;
        }
        int n = dist.size();
        vector<int> f(n);
        for (int i = 0; ; i++) {
            int pre = 0;
            for (int j = 0; j < n - 1; j++) {
                int tmp = f[j + 1];
                f[j + 1] = (f[j] + dist[j] + speed - 1) / speed * speed;
                if (i) {
                    f[j + 1] = min(f[j + 1], pre + dist[j]);
                }
                pre = tmp;
            }
            if (f[n - 1] + dist[n - 1] <= (long long) speed * hoursBefore) {
                return i;
            }
        }
    }
};
```

```go [sol-Go]
func minSkips(dist []int, speed, hoursBefore int) int {
    sumDist := 0
    for _, d := range dist {
        sumDist += d
    }
    if sumDist > speed*hoursBefore {
        return -1
    }

    n := len(dist)
    f := make([]int, n)
    for i := 0; ; i++ {
        pre := 0
        for j, d := range dist[:n-1] {
            tmp := f[j+1]
            f[j+1] = (f[j] + d + speed - 1) / speed * speed
            if i > 0 {
                f[j+1] = min(f[j+1], pre+d)
            }
            pre = tmp
        }
        if f[n-1]+dist[n-1] <= speed*hoursBefore {
            return i
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{dist}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

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
