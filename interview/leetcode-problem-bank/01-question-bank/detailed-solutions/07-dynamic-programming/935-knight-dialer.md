# 935. 骑士拨号器

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/knight-dialer/
- 题目 slug：`knight-dialer`
- 来源专题：动态规划
- 来源分类路径：六、状态机 DP / §6.3 进阶
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/knight-dialer/solutions/3004116/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-x06l/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推到矩阵快速幂！（Python/Java/C++/Go）](https://leetcode.cn/problems/knight-dialer/solutions/3004116/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-x06l/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-x06l`
- topic id：`3004116`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

首先，由于无法从 $5$ 移动到其他单元格，所以当 $n\ge 2$ 时，马的初始位置不能等于 $5$。

特判 $n=1$ 的情况，直接返回 $10$。

在下文中，$n\ge 2$。

## 一、寻找子问题

比如 $n=3$，我们要解决的问题（原问题）是：

- 把马放在单元格 $0$ 到 $9$（除了 $5$）上，然后移动 $n-1=2$ 步，一共有多少种移动方案？

枚举移动到的位置，比如一开始在 $1$，移动到 $6$，那么问题变成：

- 把马放在单元格 $6$ 上，然后移动 $n-2=1$ 步，一共有多少种移动方案？

这是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

## 二、状态定义与状态转移方程（优化前）

根据上面的讨论，我们需要在递归过程中跟踪以下信息：

- $i$：还需要移动 $i$ 步。
- $j$：马在单元格 $j$ 上。

因此，定义状态为 $\textit{dfs}(i,j)$，表示把马放在单元格 $j$ 上，然后移动 $i$ 步，有多少种移动方案。

枚举马能移动到的单元格 $k$，问题变成：把马放在单元格 $k$ 上，然后移动 $i-1$ 步，有多少种移动方案，即 $\textit{dfs}(i-1,k)$。

累加得

$$
\textit{dfs}(i,j) = \sum_{k} \textit{dfs}(i-1,k)
$$

**递归边界**：$\textit{dfs}(0,j)=1$。无法移动，算作一种移动方案，对应着电话号码 $j$。

**递归入口**：$\sum\limits_{j=0}^{9}\textit{dfs}(n-1,j)$，这是原问题，也是答案。注意 $\textit{dfs}(n-1,5)=0$。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

#### 细节

1. 为避免写出大量的 $\texttt{if-else}$，可以用一个表（数组）保存每个数字可以移动到的数字。
2. 把 DFS 写在外面，这样记忆化的值可以在不同的测试数据间共享。

```py [sol-Python3]
MOD = 1_000_000_007
NEXT = (4, 6), (6, 8), (7, 9), (4, 8), (0, 3, 9), (), (0, 1, 7), (2, 6), (1, 3), (2, 4)

@cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
def dfs(i: int, j: int) -> int:
    if i == 0:
        return 1
    return sum(dfs(i - 1, k) for k in NEXT[j]) % MOD

class Solution:
    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        return (sum(dfs(n - 1, j) for j in range(10))) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int[][] NEXT = {
        {4, 6}, {6, 8}, {7, 9}, {4, 8}, {0, 3, 9}, {}, {0, 1, 7}, {2, 6}, {1, 3}, {2, 4}
    };
    private static final int[][] memo = new int[5000][10];

    public int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        int ans = 0;
        for (int j = 0; j < 10; j++) {
            ans = (ans + dfs(n - 1, j)) % MOD;
        }
        return ans;
    }

    private int dfs(int i, int j) {
        if (i == 0) {
            return 1;
        }
        if (memo[i][j] > 0) { // 之前计算过
            return memo[i][j];
        }
        int res = 0;
        for (int k : NEXT[j]) {
            res = (res + dfs(i - 1, k)) % MOD;
        }
        return memo[i][j] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
const vector<int> NEXT[10] = {{4, 6}, {6, 8}, {7, 9}, {4, 8}, {0, 3, 9}, {}, {0, 1, 7}, {2, 6}, {1, 3}, {2, 4}};
int memo[5000][10];

int dfs(int i, int j) {
    if (i == 0) {
        return 1;
    }
    int& res = memo[i][j]; // 注意这里是引用
    if (res) { // 之前计算过
        return res;
    }
    for (int k : NEXT[j]) {
        res = (res + dfs(i - 1, k)) % MOD;
    }
    return res;
}

class Solution {
public:
    int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        long long ans = 0;
        for (int j = 0; j < 10; j++) {
            ans += dfs(n - 1, j);
        }
        return ans % MOD;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

var next = [10][]int{{4, 6}, {6, 8}, {7, 9}, {4, 8}, {0, 3, 9}, {}, {0, 1, 7}, {2, 6}, {1, 3}, {2, 4}}
var memo [5000][10]int

func dfs(i, j int) int {
    if i == 0 {
        return 1
    }
    p := &memo[i][j]
    if *p > 0 { // 之前计算过
        return *p
    }
    res := 0
    for _, k := range next[j] {
        res += dfs(i-1, k)
    }
    res %= mod
    *p = res // 记忆化
    return res
}

func knightDialer(n int) int {
    if n == 1 {
        return 10
    }
    ans := 0
    for j := range 10 {
        ans += dfs(n-1, j)
    }
    return ans % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nD)$，其中 $D=9$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(nD)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(nD)$。
- 空间复杂度：$\mathcal{O}(nD)$。保存多少状态，就需要多少空间。

## 三、状态定义与状态转移方程（优化后）

如下图所示，其实只有 $4$ 种**本质不同**的数字。

![lc935-c.png](https://pic.leetcode.cn/1732785081-HHnGDF-lc935-c.png)

把 $A,B,C,D$ 类分别用 $0,1,2,3$ 表示，作为 $\textit{dfs}$ 的第二个参数，那么有状态转移方程

$$
\begin{aligned}
\textit{dfs}(i,0) &= \textit{dfs}(i - 1, 1) + \textit{dfs}(i - 1, 2)     \\
\textit{dfs}(i,1) &= 2\cdot \textit{dfs}(i - 1, 0)    \\
\textit{dfs}(i,2) &= 2\cdot \textit{dfs}(i - 1, 0) + \textit{dfs}(i - 1, 3)     \\
\textit{dfs}(i,3) &= 2\cdot \textit{dfs}(i - 1, 2)      \\
\end{aligned}
$$

答案为

$$
4\cdot\textit{dfs}(n - 1, 0)  + 2\cdot \textit{dfs}(n - 1, 1) + 2\cdot \textit{dfs}(n - 1, 2)  + \textit{dfs}(n - 1, 3)
$$

> 注：这相当于把方法二和式 $\sum\limits_{j=0}^{9}\textit{dfs}(n-1,j)$ 中的 $j=1,3,7,9$ 合并成 $4\cdot\textit{dfs}(n - 1, 0)$，其他同理。

```py [sol-Python3]
MOD = 1_000_000_007

@cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
def dfs(i: int, j: int) -> int:
    if i == 0:
        return 1
    if j == 0:
        return (dfs(i - 1, 1) + dfs(i - 1, 2)) % MOD
    if j == 1:
        return dfs(i - 1, 0) * 2 % MOD
    if j == 2:
        return (dfs(i - 1, 0) * 2 + dfs(i - 1, 3)) % MOD
    return dfs(i - 1, 2) * 2 % MOD

class Solution:
    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        return (dfs(n - 1, 0) * 4 + dfs(n - 1, 1) * 2 +
                dfs(n - 1, 2) * 2 + dfs(n - 1, 3)) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final long[][] memo = new long[5000][10];

    public int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        return (int) ((dfs(n - 1, 0) * 4 + dfs(n - 1, 1) * 2 +
                       dfs(n - 1, 2) * 2 + dfs(n - 1, 3)) % MOD);
    }

    private long dfs(int i, int j) {
        if (i == 0) {
            return 1;
        }
        if (memo[i][j] > 0) { // 之前计算过
            return memo[i][j];
        }
        if (j == 0) {
            memo[i][j] = (dfs(i - 1, 1) + dfs(i - 1, 2)) % MOD;
        } else if (j == 1) {
            memo[i][j] = dfs(i - 1, 0) * 2 % MOD;
        } else if (j == 2) {
            memo[i][j] = (dfs(i - 1, 0) * 2 + dfs(i - 1, 3)) % MOD;
        } else {
            memo[i][j] = dfs(i - 1, 2) * 2 % MOD;
        }
        return memo[i][j];
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
long long memo[5000][10];

long long dfs(int i, int j) {
    if (i == 0) {
        return 1;
    }
    long long& res = memo[i][j]; // 注意这里是引用
    if (res) { // 之前计算过
        return res;
    }
    if (j == 0) {
        res = (dfs(i - 1, 1) + dfs(i - 1, 2)) % MOD;
    } else if (j == 1) {
        res = dfs(i - 1, 0) * 2 % MOD;
    } else if (j == 2) {
        res = (dfs(i - 1, 0) * 2 + dfs(i - 1, 3)) % MOD;
    } else {
        res = dfs(i - 1, 2) * 2 % MOD;
    }
    return res;
}

class Solution {
public:
    int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        return (dfs(n - 1, 0) * 4 + dfs(n - 1, 1) * 2 +
                dfs(n - 1, 2) * 2 + dfs(n - 1, 3)) % MOD;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

var memo [5000][10]int

func dfs(i, j int) int {
    if i == 0 {
        return 1
    }
    p := &memo[i][j]
    if *p > 0 { // 之前计算过
        return *p
    }
    if j == 0 {
        *p = (dfs(i-1, 1) + dfs(i-1, 2)) % mod
    } else if j == 1 {
        *p = dfs(i-1, 0) * 2 % mod
    } else if j == 2 {
        *p = (dfs(i-1, 0)*2 + dfs(i-1, 3)) % mod
    } else {
        *p = dfs(i-1, 2) * 2 % mod
    }
    return *p
}

func knightDialer(n int) int {
    if n == 1 {
        return 10
    }
    return (dfs(n-1, 0)*4 + dfs(n-1, 1)*2 + dfs(n-1, 2)*2 + dfs(n-1, 3)) % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nD)$，其中 $D=4$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(nD)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(nD)$。
- 空间复杂度：$\mathcal{O}(nD)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示把马放在 $j$ 类单元格上，然后移动 $i$ 步，有多少种移动方案。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
\begin{aligned}
f[i][0] &= f[i-1][1] + f[i-1][2]     \\
f[i][1] &= 2\cdot f[i-1][0]    \\
f[i][2] &= 2\cdot f[i-1][0] + f[i-1][3]     \\
f[i][3] &= 2\cdot f[i-1][2]      \\
\end{aligned}
$$

初始值 $f[0][j]=1$，翻译自递归边界 $\textit{dfs}(0,j)=1$。

答案为 $4\cdot f[n-1][0] + 2\cdot f[n-1][1] + 2\cdot f[n-1][2]  + f[n-1][3]$，翻译自递归入口 $4\cdot\textit{dfs}(n - 1, 0)  + 2\cdot \textit{dfs}(n - 1, 1) + 2\cdot \textit{dfs}(n - 1, 2)  + \textit{dfs}(n - 1, 3)$。

```py [sol-Python3]
MOD = 1_000_000_007
MX = 5000

f = [[1] * 4 for _ in range(MX)]
for i in range(1, MX):
    f[i][0] = (f[i - 1][1] + f[i - 1][2]) % MOD
    f[i][1] = f[i - 1][0] * 2 % MOD
    f[i][2] = (f[i - 1][0] * 2 + f[i - 1][3]) % MOD
    f[i][3] = f[i - 1][2] * 2 % MOD

class Solution:
    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        return (f[n - 1][0] * 4 + f[n - 1][1] * 2 +
                f[n - 1][2] * 2 + f[n - 1][3]) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int MX = 5000;
    private static final long[][] f = new long[MX][4];

    static {
        f[0][0] = f[0][1] = f[0][2] = f[0][3] = 1;
        for (int i = 1; i < MX; i++) {
            f[i][0] = (f[i - 1][1] + f[i - 1][2]) % MOD;
            f[i][1] = f[i - 1][0] * 2 % MOD;
            f[i][2] = (f[i - 1][0] * 2 + f[i - 1][3]) % MOD;
            f[i][3] = f[i - 1][2] * 2 % MOD;
        }
    }

    public int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        return (int) ((f[n - 1][0] * 4 + f[n - 1][1] * 2 +
                       f[n - 1][2] * 2 + f[n - 1][3]) % MOD);
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
const int MX = 5000;
long long f[MX][4];

int init = []() {
    f[0][0] = f[0][1] = f[0][2] = f[0][3] = 1;
    for (int i = 1; i < MX; i++) {
        f[i][0] = (f[i - 1][1] + f[i - 1][2]) % MOD;
        f[i][1] = f[i - 1][0] * 2 % MOD;
        f[i][2] = (f[i - 1][0] * 2 + f[i - 1][3]) % MOD;
        f[i][3] = f[i - 1][2] * 2 % MOD;
    }
    return 0;
}();

class Solution {
public:
    int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        return (f[n - 1][0] * 4 + f[n - 1][1] * 2 +
                f[n - 1][2] * 2 + f[n - 1][3]) % MOD;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

var f = [5000][4]int{{1, 1, 1, 1}}

func init() {
    for i := 1; i < len(f); i++ {
        f[i][0] = (f[i-1][1] + f[i-1][2]) % mod
        f[i][1] = f[i-1][0] * 2 % mod
        f[i][2] = (f[i-1][0]*2 + f[i-1][3]) % mod
        f[i][3] = f[i-1][2] * 2 % mod
    }
}

func knightDialer(n int) int {
    if n == 1 {
        return 10
    }
    return (f[n-1][0]*4 + f[n-1][1]*2 + f[n-1][2]*2 + f[n-1][3]) % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。忽略预处理的时间和空间。
- 空间复杂度：$\mathcal{O}(1)$。

## 五、矩阵快速幂优化

把状态转移方程用矩阵乘法表示，即

$$
\begin{bmatrix}
f[i][0] \\
f[i][1] \\
f[i][2] \\
f[i][3] \\
\end{bmatrix}
= \begin{bmatrix}
0 & 1 & 1 & 0 \\
2 & 0 & 0 & 0 \\
2 & 0 & 0 & 1 \\
0 & 0 & 2 & 0 \\
\end{bmatrix}
\begin{bmatrix}
f[i-1][0] \\
f[i-1][1] \\
f[i-1][2] \\
f[i-1][3] \\
\end{bmatrix}
$$

把上式中的三个矩阵分别记作 $F[i],M,F[i-1]$，即

$$
F[i] = M\times F[i-1]
$$

那么有

$$
\begin{aligned}
F[n-1] &= M\times F[n-2]      \\
&= M\times M\times F[n-3]        \\
&= M\times M\times M\times  F[n-4]        \\
&\ \ \vdots  \\
&= M^{n-1}\times F[0] \\
\end{aligned}
$$

其中 $M^{n-1}$ 可以用**快速幂**计算，原理请看[【图解】一张图秒懂快速幂](https://leetcode.cn/problems/powx-n/solution/tu-jie-yi-zhang-tu-miao-dong-kuai-su-mi-ykp3i/)。

初始值

$$
F[0] =
\begin{bmatrix}
1 \\
1 \\
1 \\
1 \\
\end{bmatrix}
$$

> 注：由于力扣不计入预处理的时间，矩阵快速幂的实际运行时间可能比递推慢。

```py [sol-Python3]
MOD = 1_000_000_007

# a @ b，其中 @ 是矩阵乘法
def mul(a: List[List[int]], b: List[List[int]]) -> List[List[int]]:
    return [[sum(x * y for x, y in zip(row, col)) % MOD for col in zip(*b)]
            for row in a]

# a^n @ f0
def pow_mul(a: List[List[int]], n: int, f0: List[List[int]]) -> List[List[int]]:
    res = f0
    while n:
        if n & 1:
            res = mul(a, res)
        a = mul(a, a)
        n >>= 1
    return res

class Solution:
    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        m = [[0, 1, 1, 0], [2, 0, 0, 0], [2, 0, 0, 1], [0, 0, 2, 0]]
        f0 = [[1], [1], [1], [1]]
        fn = pow_mul(m, n - 1, f0)
        return (fn[0][0] * 4 + fn[1][0] * 2 + fn[2][0] * 2 + fn[3][0]) % MOD
```

```py [sol-NumPy]
import numpy as np

MOD = 1_000_000_007

# a^n @ f0
def pow_mul(a: np.ndarray, n: int, f0: np.ndarray) -> np.ndarray:
    res = f0
    while n:
        if n & 1:
            res = a @ res % MOD
        a = a @ a % MOD
        n >>= 1
    return res

class Solution:
    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        m = np.array([[0, 1, 1, 0], [2, 0, 0, 0], [2, 0, 0, 1], [0, 0, 2, 0]], dtype=object)
        f0 = np.ones((4,), dtype=object)
        fn = pow_mul(m, n - 1, f0)
        return (fn[0] * 4 + fn[1] * 2 + fn[2] * 2 + fn[3]) % MOD
```

```py [sol-NumPy 写法二]
import numpy as np

class Solution:
    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        m = np.array([[0, 1, 1, 0], [2, 0, 0, 0], [2, 0, 0, 1], [0, 0, 2, 0]], dtype=object)
        f0 = np.ones((4,), dtype=object)
        # 注：没法中途取模，参与运算的数字很大，效率低，所以我并不推荐这种写法
        fn = np.linalg.matrix_power(m, n - 1) @ f0
        return (fn[0] * 4 + fn[1] * 2 + fn[2] * 2 + fn[3]) % 1_000_000_007
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        long[][] m = {
            {0, 1, 1, 0},
            {2, 0, 0, 0},
            {2, 0, 0, 1},
            {0, 0, 2, 0},
        };
        long[][] f0 = {{1}, {1}, {1}, {1}};
        long[][] fn = powMul(m, n - 1, f0);
        return (int) ((fn[0][0] * 4 + fn[1][0] * 2 +
                       fn[2][0] * 2 + fn[3][0]) % MOD);
    }

    // a^n * f0
    private long[][] powMul(long[][] a, int n, long[][] f0) {
        long[][] res = f0;
        while (n > 0) {
            if ((n & 1) > 0) {
                res = mul(a, res);
            }
            a = mul(a, a);
            n >>= 1;
        }
        return res;
    }

    // 返回矩阵 a 和矩阵 b 相乘的结果
    private long[][] mul(long[][] a, long[][] b) {
        long[][] c = new long[a.length][b[0].length];
        for (int i = 0; i < a.length; i++) {
            for (int k = 0; k < a[i].length; k++) {
                if (a[i][k] == 0) {
                    continue;
                }
                for (int j = 0; j < b[k].length; j++) {
                    c[i][j] = (c[i][j] + a[i][k] * b[k][j]) % MOD;
                }
            }
        }
        return c;
    }
}
```

```cpp [sol-C++]
constexpr int MOD = 1'000'000'007;

using matrix = vector<vector<long long>>;

// 返回矩阵 a 和矩阵 b 相乘的结果
matrix mul(matrix& a, matrix& b) {
    int n = a.size(), m = b[0].size();
    matrix c = matrix(n, vector<long long>(m));
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < a[i].size(); k++) {
            if (a[i][k] == 0) {
                continue;
            }
            for (int j = 0; j < m; j++) {
                c[i][j] = (c[i][j] + a[i][k] * b[k][j]) % MOD;
            }
        }
    }
    return c;
}

// a^n * f0
matrix pow_mul(matrix a, int n, matrix& f0) {
    matrix res = f0;
    while (n) {
        if (n & 1) {
            res = mul(a, res);
        }
        a = mul(a, a);
        n >>= 1;
    }
    return res;
}

class Solution {
public:
    int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        matrix m = {
            {0, 1, 1, 0},
            {2, 0, 0, 0},
            {2, 0, 0, 1},
            {0, 0, 2, 0},
        };
        matrix f0 = {{1}, {1}, {1}, {1}};
        matrix fn = pow_mul(m, n - 1, f0);
        return (fn[0][0] * 4 + fn[1][0] * 2 +
                fn[2][0] * 2 + fn[3][0]) % MOD;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

type matrix [][]int

func newMatrix(n, m int) matrix {
    a := make(matrix, n)
    for i := range a {
        a[i] = make([]int, m)
    }
    return a
}

// 返回矩阵 a 和矩阵 b 相乘的结果
func (a matrix) mul(b matrix) matrix {
    c := newMatrix(len(a), len(b[0]))
    for i, row := range a {
        for k, x := range row {
            if x == 0 {
                continue
            }
            for j, y := range b[k] {
                c[i][j] = (c[i][j] + x*y) % mod
            }
        }
    }
    return c
}

// a^n * f0
func (a matrix) powMul(n int, f0 matrix) matrix {
    res := f0
    for ; n > 0; n /= 2 {
        if n%2 > 0 {
            res = a.mul(res)
        }
        a = a.mul(a)
    }
    return res
}

func knightDialer(n int) int {
    if n == 1 {
        return 10
    }
    m := matrix{
        {0, 1, 1, 0},
        {2, 0, 0, 0},
        {2, 0, 0, 1},
        {0, 0, 2, 0},
    }
    f0 := matrix{{1}, {1}, {1}, {1}}
    fn := m.powMul(n-1, f0)
    return (fn[0][0]*4 + fn[1][0]*2 + fn[2][0]*2 + fn[3][0]) % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(D^3\log n)$，其中 $D=4$ 为矩阵的阶数。
- 空间复杂度：$\mathcal{O}(D^2)$。

## 六、Berlekamp-Massey 算法 + Kitamasa 算法

使用 [Berlekamp-Massey 算法](https://zhuanlan.zhihu.com/p/1966417899825665440) 找规律，可以直接得到线性递推式

$$
f_n = 6f_{n-2}-4f_{n-4} \ \ (n \ge 5)
$$

初始值 $f = [10,20,46,104,240]$。

然后使用 [Kitamasa 算法](https://zhuanlan.zhihu.com/p/1964051212304364939) 解决。

```py [sol-Python3]
MOD = 1_000_000_007

class Solution:
    # 给定常系数齐次线性递推式 f(n) = coef[k-1] * f(n-1) + coef[k-2] * f(n-2) + ... + coef[0] * f(n-k)
    # 以及初始值 f(i) = a[i] (0 <= i < k)
    # 返回 f(n) % MOD，其中参数 n 从 0 开始
    # 注意 coef 的顺序
    # 时间复杂度 O(k^2 log n)，其中 k 是 coef 的长度
    def kitamasa(self, coef: List[int], a: List[int], n: int) -> int:
        if n < len(a):
            return a[n] % MOD

        k = len(coef)

        # 已知 f(n) 的各项系数为 a，f(m) 的各项系数为 b
        # 计算并返回 f(n+m) 的各项系数 c
        def compose(a: List[int], b: List[int]) -> List[int]:
            c = [0] * k
            for v in a:
                for j, w in enumerate(b):
                    c[j] = (c[j] + v * w) % MOD
                # 原地计算下一组系数，比如已知 f(4) 的各项系数，现在要计算 f(5) 的各项系数
                # 倒序遍历，避免提前覆盖旧值
                bk1 = b[-1]
                for i in range(k - 1, 0, -1):
                    b[i] = (b[i - 1] + bk1 * coef[i]) % MOD
                b[0] = bk1 * coef[0] % MOD
            return c

        # 计算 res_c，以表出 f(n) = res_c[k-1] * a[k-1] + res_c[k-2] * a[k-2] + ... + res_c[0] * a[0]
        res_c = [0] * k
        c = [0] * k
        res_c[0] = c[1] = 1
        while n > 0:
            if n % 2:
                res_c = compose(c, res_c)
            # 由于会修改 compose 的第二个参数，这里把 c 复制一份再传入
            c = compose(c, c[:])
            n //= 2

        return sum(c * v for c, v in zip(res_c, a)) % MOD

    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        coef = [0, 6, 0, -4]
        a = [20, 46, 104, 240]
        return self.kitamasa(coef[::-1], a, n - 2)  #  注意 kitamasa 入参的顺序
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    private static final int[] coef = {-4, 0, 6, 0}; // 注意 kitamasa 入参的顺序
    private static final int[] a = {20, 46, 104, 240}; // 初始值

    public int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        return kitamasa(coef, a, n - 2);
    }

    // 给定常系数齐次线性递推式 f(n) = coef[k-1] * f(n-1) + ... + coef[0] * f(n-k)
    // 以及初始值 f(i) = a[i] (0 <= i < k)
    // 返回 f(n) % MOD，其中参数 n 从 0 开始
    // 注意 coef 的顺序
    // 时间复杂度 O(k^2 log n)，其中 k 是 coef 的长度
    private int kitamasa(int[] coef, int[] a, long n) {
        if (n < a.length) {
            return a[(int) n] % MOD;
        }

        int k = coef.length;

        // 计算 resC，以表出 f(n) = resC[k-1] * a[k-1] + ... + resC[0] * a[0]
        int[] resC = new int[k];
        int[] c = new int[k];
        resC[0] = c[1] = 1;
        for (; n > 0; n /= 2) {
            if (n % 2 > 0) {
                resC = compose(coef, c, resC);
            }
            // 由于会修改 compose 的第三个参数，这里把 c 复制一份再传入
            c = compose(coef, c, c.clone());
        }

        long ans = 0;
        for (int i = 0; i < k; i++) {
            ans = (ans + (long) resC[i] * a[i]) % MOD;
        }

        return (int) ((ans + MOD) % MOD); // 保证返回值非负
    }

    // 已知 f(n) 的各项系数为 a，f(m) 的各项系数为 b
    // 计算并返回 f(n+m) 的各项系数 c
    private int[] compose(int[] coef, int[] a, int[] b) {
        int k = a.length;
        int[] c = new int[k];
        for (int v : a) {
            for (int j = 0; j < k; j++) {
                c[j] = (int) ((c[j] + (long) v * b[j]) % MOD);
            }
            // 原地计算下一组系数，比如已知 f(4) 的各项系数，现在要计算 f(5) 的各项系数
            // 倒序遍历，避免提前覆盖旧值
            long bk1 = b[k - 1];
            for (int i = k - 1; i > 0; i--) {
                b[i] = (int) ((b[i - 1] + bk1 * coef[i]) % MOD);
            }
            b[0] = (int) (bk1 * coef[0] % MOD);
        }
        return c;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int MOD = 1'000'000'007;

    // 给定常系数齐次线性递推式 f(n) = coef[k-1] * f(n-1) + coef[k-2] * f(n-2) + ... + coef[0] * f(n-k)
    // 以及初始值 f(i) = a[i] (0 <= i < k)
    // 返回 f(n) % MOD，其中参数 n 从 0 开始
    // 注意 coef 的顺序
    // 时间复杂度 O(k^2 log n)，其中 k 是 coef 的长度
    int kitamasa(const vector<int>& coef, const vector<int>& a, long long n) {
        if (n < a.size()) {
            return a[n] % MOD;
        }

        int k = coef.size();

        // 已知 f(n) 的各项系数为 A，f(m) 的各项系数为 B
        // 计算并返回 f(n+m) 的各项系数 C
        auto compose = [&](const vector<int>& A, vector<int> B) -> vector<int> {
            vector<int> C(k);
            for (int v : A) {
                for (int j = 0; j < k; j++) {
                    C[j] = (C[j] + 1LL * v * B[j]) % MOD;
                }
                // 原地计算下一组系数，比如已知 f(4) 的各项系数，现在要计算 f(5) 的各项系数
                // 倒序遍历，避免提前覆盖旧值
                int bk1 = B.back();
                for (int i = k - 1; i > 0; i--) {
                    B[i] = (B[i - 1] + 1LL * bk1 * coef[i]) % MOD;
                }
                B[0] = 1LL * bk1 * coef[0] % MOD;
            }
            return C;
        };

        // 计算 res_c，以表出 f(n) = res_c[k-1] * a[k-1] + res_c[k-2] * a[k-2] + ... + res_c[0] * a[0]
        vector<int> res_c(k), c(k);
        res_c[0] = c[1] = 1;
        for (; n > 0; n /= 2) {
            if (n % 2) {
                res_c = compose(c, move(res_c));
            }
            c = compose(c, c);
        }

        long long ans = 0;
        for (int i = 0; i < k; i++) {
            ans = (ans + 1LL * res_c[i] * a[i]) % MOD;
        }

        return (ans + MOD) % MOD; // 保证返回值非负
    }

public:
    int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        vector<int> coef = {0, 6, 0, -4};
        vector<int> a = {20, 46, 104, 240};
        ranges::reverse(coef); // 注意 kitamasa 入参的顺序
        return kitamasa(coef, a, n - 2);
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

// 给定常系数齐次线性递推式 f(n) = coef[k-1] * f(n-1) + coef[k-2] * f(n-2) + ... + coef[0] * f(n-k)
// 以及初始值 f(i) = a[i] (0 <= i < k)
// 返回 f(n) % mod，其中参数 n 从 0 开始
// 注意 coef 的顺序
// 时间复杂度 O(k^2 log n)，其中 k 是 coef 的长度
func kitamasa(coef, a []int, n int) (ans int) {
    if n < len(a) {
        return a[n] % mod
    }

    k := len(coef)

    // 已知 f(n) 的各项系数为 a，f(m) 的各项系数为 b
    // 计算并返回 f(n+m) 的各项系数 c
    compose := func(a, b []int) []int {
        c := make([]int, k)
        for _, v := range a {
            for j, w := range b {
                c[j] = (c[j] + v*w) % mod
            }
            // 原地计算下一组系数，比如已知 f(4) 的各项系数，现在要计算 f(5) 的各项系数
            // 倒序遍历，避免提前覆盖旧值
            bk1 := b[k-1]
            for i := k - 1; i > 0; i-- {
                b[i] = (b[i-1] + bk1*coef[i]) % mod
            }
            b[0] = bk1 * coef[0] % mod
        }
        return c
    }

    // 计算 resC，以表出 f(n) = resC[k-1] * a[k-1] + resC[k-2] * a[k-2] + ... + resC[0] * a[0]
    resC := make([]int, k)
    resC[0] = 1
    c := make([]int, k)
    c[1] = 1
    for ; n > 0; n /= 2 {
        if n%2 > 0 {
            resC = compose(c, resC)
        }
        // 由于会修改 compose 的第二个参数，这里把 c 复制一份再传入
        c = compose(c, slices.Clone(c))
    }

    for i, c := range resC {
        ans = (ans + c*a[i]) % mod
    }

    return (ans + mod) % mod // 保证结果非负
}

func knightDialer(n int) int {
    if n == 1 {
        return 10
    }
    coef := []int{0, 6, 0, -4}
    a := []int{20, 46, 104, 240}
    slices.Reverse(coef) // 注意 kitamasa 入参的顺序
    return kitamasa(coef, a, n-2)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(D^2\log n)$，其中 $D=4$ 为递推式的阶数。
- 空间复杂度：$\mathcal{O}(D)$。

## 专题训练

见下面动态规划题单的「**§11.6 矩阵快速幂优化 DP**」。

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

比如 $n=3$，我们要解决的问题（原问题）是：

- 把马放在单元格 $0$ 到 $9$（除了 $5$）上，然后移动 $n-1=2$ 步，一共有多少种移动方案？

枚举移动到的位置，比如一开始在 $1$，移动到 $6$，那么问题变成：

- 把马放在单元格 $6$ 上，然后移动 $n-2=1$ 步，一共有多少种移动方案？

这是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

## 二、状态定义与状态转移方程（优化前）

根据上面的讨论，我们需要在递归过程中跟踪以下信息：

- $i$：还需要移动 $i$ 步。
- $j$：马在单元格 $j$ 上。

因此，定义状态为 $\textit{dfs}(i,j)$，表示把马放在单元格 $j$ 上，然后移动 $i$ 步，有多少种移动方案。

枚举马能移动到的单元格 $k$，问题变成：把马放在单元格 $k$ 上，然后移动 $i-1$ 步，有多少种移动方案，即 $\textit{dfs}(i-1,k)$。

累加得

$$
\textit{dfs}(i,j) = \sum_{k} \textit{dfs}(i-1,k)
$$

**递归边界**：$\textit{dfs}(0,j)=1$。无法移动，算作一种移动方案，对应着电话号码 $j$。

**递归入口**：$\sum\limits_{j=0}^{9}\textit{dfs}(n-1,j)$，这是原问题，也是答案。注意 $\textit{dfs}(n-1,5)=0$。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

#### 细节

1. 为避免写出大量的 $\texttt{if-else}$，可以用一个表（数组）保存每个数字可以移动到的数字。
2. 把 DFS 写在外面，这样记忆化的值可以在不同的测试数据间共享。

```py [sol-Python3]
MOD = 1_000_000_007
NEXT = (4, 6), (6, 8), (7, 9), (4, 8), (0, 3, 9), (), (0, 1, 7), (2, 6), (1, 3), (2, 4)

@cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
def dfs(i: int, j: int) -> int:
    if i == 0:
        return 1
    return sum(dfs(i - 1, k) for k in NEXT[j]) % MOD

class Solution:
    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        return (sum(dfs(n - 1, j) for j in range(10))) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int[][] NEXT = {
        {4, 6}, {6, 8}, {7, 9}, {4, 8}, {0, 3, 9}, {}, {0, 1, 7}, {2, 6}, {1, 3}, {2, 4}
    };
    private static final int[][] memo = new int[5000][10];

    public int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        int ans = 0;
        for (int j = 0; j < 10; j++) {
            ans = (ans + dfs(n - 1, j)) % MOD;
        }
        return ans;
    }

    private int dfs(int i, int j) {
        if (i == 0) {
            return 1;
        }
        if (memo[i][j] > 0) { // 之前计算过
            return memo[i][j];
        }
        int res = 0;
        for (int k : NEXT[j]) {
            res = (res + dfs(i - 1, k)) % MOD;
        }
        return memo[i][j] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
const vector<int> NEXT[10] = {{4, 6}, {6, 8}, {7, 9}, {4, 8}, {0, 3, 9}, {}, {0, 1, 7}, {2, 6}, {1, 3}, {2, 4}};
int memo[5000][10];

int dfs(int i, int j) {
    if (i == 0) {
        return 1;
    }
    int& res = memo[i][j]; // 注意这里是引用
    if (res) { // 之前计算过
        return res;
    }
    for (int k : NEXT[j]) {
        res = (res + dfs(i - 1, k)) % MOD;
    }
    return res;
}

class Solution {
public:
    int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        long long ans = 0;
        for (int j = 0; j < 10; j++) {
            ans += dfs(n - 1, j);
        }
        return ans % MOD;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

var next = [10][]int{{4, 6}, {6, 8}, {7, 9}, {4, 8}, {0, 3, 9}, {}, {0, 1, 7}, {2, 6}, {1, 3}, {2, 4}}
var memo [5000][10]int

func dfs(i, j int) int {
    if i == 0 {
        return 1
    }
    p := &memo[i][j]
    if *p > 0 { // 之前计算过
        return *p
    }
    res := 0
    for _, k := range next[j] {
        res += dfs(i-1, k)
    }
    res %= mod
    *p = res // 记忆化
    return res
}

func knightDialer(n int) int {
    if n == 1 {
        return 10
    }
    ans := 0
    for j := range 10 {
        ans += dfs(n-1, j)
    }
    return ans % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nD)$，其中 $D=9$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(nD)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(nD)$。
- 空间复杂度：$\mathcal{O}(nD)$。保存多少状态，就需要多少空间。

## 三、状态定义与状态转移方程（优化后）

如下图所示，其实只有 $4$ 种**本质不同**的数字。

![lc935-c.png](https://pic.leetcode.cn/1732785081-HHnGDF-lc935-c.png)

把 $A,B,C,D$ 类分别用 $0,1,2,3$ 表示，作为 $\textit{dfs}$ 的第二个参数，那么有状态转移方程

$$
\begin{aligned}
\textit{dfs}(i,0) &= \textit{dfs}(i - 1, 1) + \textit{dfs}(i - 1, 2)     \\
\textit{dfs}(i,1) &= 2\cdot \textit{dfs}(i - 1, 0)    \\
\textit{dfs}(i,2) &= 2\cdot \textit{dfs}(i - 1, 0) + \textit{dfs}(i - 1, 3)     \\
\textit{dfs}(i,3) &= 2\cdot \textit{dfs}(i - 1, 2)      \\
\end{aligned}
$$

答案为

$$
4\cdot\textit{dfs}(n - 1, 0)  + 2\cdot \textit{dfs}(n - 1, 1) + 2\cdot \textit{dfs}(n - 1, 2)  + \textit{dfs}(n - 1, 3)
$$

> 注：这相当于把方法二和式 $\sum\limits_{j=0}^{9}\textit{dfs}(n-1,j)$ 中的 $j=1,3,7,9$ 合并成 $4\cdot\textit{dfs}(n - 1, 0)$，其他同理。

```py [sol-Python3]
MOD = 1_000_000_007

@cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
def dfs(i: int, j: int) -> int:
    if i == 0:
        return 1
    if j == 0:
        return (dfs(i - 1, 1) + dfs(i - 1, 2)) % MOD
    if j == 1:
        return dfs(i - 1, 0) * 2 % MOD
    if j == 2:
        return (dfs(i - 1, 0) * 2 + dfs(i - 1, 3)) % MOD
    return dfs(i - 1, 2) * 2 % MOD

class Solution:
    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        return (dfs(n - 1, 0) * 4 + dfs(n - 1, 1) * 2 +
                dfs(n - 1, 2) * 2 + dfs(n - 1, 3)) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final long[][] memo = new long[5000][10];

    public int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        return (int) ((dfs(n - 1, 0) * 4 + dfs(n - 1, 1) * 2 +
                       dfs(n - 1, 2) * 2 + dfs(n - 1, 3)) % MOD);
    }

    private long dfs(int i, int j) {
        if (i == 0) {
            return 1;
        }
        if (memo[i][j] > 0) { // 之前计算过
            return memo[i][j];
        }
        if (j == 0) {
            memo[i][j] = (dfs(i - 1, 1) + dfs(i - 1, 2)) % MOD;
        } else if (j == 1) {
            memo[i][j] = dfs(i - 1, 0) * 2 % MOD;
        } else if (j == 2) {
            memo[i][j] = (dfs(i - 1, 0) * 2 + dfs(i - 1, 3)) % MOD;
        } else {
            memo[i][j] = dfs(i - 1, 2) * 2 % MOD;
        }
        return memo[i][j];
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
long long memo[5000][10];

long long dfs(int i, int j) {
    if (i == 0) {
        return 1;
    }
    long long& res = memo[i][j]; // 注意这里是引用
    if (res) { // 之前计算过
        return res;
    }
    if (j == 0) {
        res = (dfs(i - 1, 1) + dfs(i - 1, 2)) % MOD;
    } else if (j == 1) {
        res = dfs(i - 1, 0) * 2 % MOD;
    } else if (j == 2) {
        res = (dfs(i - 1, 0) * 2 + dfs(i - 1, 3)) % MOD;
    } else {
        res = dfs(i - 1, 2) * 2 % MOD;
    }
    return res;
}

class Solution {
public:
    int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        return (dfs(n - 1, 0) * 4 + dfs(n - 1, 1) * 2 +
                dfs(n - 1, 2) * 2 + dfs(n - 1, 3)) % MOD;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

var memo [5000][10]int

func dfs(i, j int) int {
    if i == 0 {
        return 1
    }
    p := &memo[i][j]
    if *p > 0 { // 之前计算过
        return *p
    }
    if j == 0 {
        *p = (dfs(i-1, 1) + dfs(i-1, 2)) % mod
    } else if j == 1 {
        *p = dfs(i-1, 0) * 2 % mod
    } else if j == 2 {
        *p = (dfs(i-1, 0)*2 + dfs(i-1, 3)) % mod
    } else {
        *p = dfs(i-1, 2) * 2 % mod
    }
    return *p
}

func knightDialer(n int) int {
    if n == 1 {
        return 10
    }
    return (dfs(n-1, 0)*4 + dfs(n-1, 1)*2 + dfs(n-1, 2)*2 + dfs(n-1, 3)) % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nD)$，其中 $D=4$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(nD)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(nD)$。
- 空间复杂度：$\mathcal{O}(nD)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示把马放在 $j$ 类单元格上，然后移动 $i$ 步，有多少种移动方案。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
\begin{aligned}
f[i][0] &= f[i-1][1] + f[i-1][2]     \\
f[i][1] &= 2\cdot f[i-1][0]    \\
f[i][2] &= 2\cdot f[i-1][0] + f[i-1][3]     \\
f[i][3] &= 2\cdot f[i-1][2]      \\
\end{aligned}
$$

初始值 $f[0][j]=1$，翻译自递归边界 $\textit{dfs}(0,j)=1$。

答案为 $4\cdot f[n-1][0] + 2\cdot f[n-1][1] + 2\cdot f[n-1][2]  + f[n-1][3]$，翻译自递归入口 $4\cdot\textit{dfs}(n - 1, 0)  + 2\cdot \textit{dfs}(n - 1, 1) + 2\cdot \textit{dfs}(n - 1, 2)  + \textit{dfs}(n - 1, 3)$。

```py [sol-Python3]
MOD = 1_000_000_007
MX = 5000

f = [[1] * 4 for _ in range(MX)]
for i in range(1, MX):
    f[i][0] = (f[i - 1][1] + f[i - 1][2]) % MOD
    f[i][1] = f[i - 1][0] * 2 % MOD
    f[i][2] = (f[i - 1][0] * 2 + f[i - 1][3]) % MOD
    f[i][3] = f[i - 1][2] * 2 % MOD

class Solution:
    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        return (f[n - 1][0] * 4 + f[n - 1][1] * 2 +
                f[n - 1][2] * 2 + f[n - 1][3]) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int MX = 5000;
    private static final long[][] f = new long[MX][4];

    static {
        f[0][0] = f[0][1] = f[0][2] = f[0][3] = 1;
        for (int i = 1; i < MX; i++) {
            f[i][0] = (f[i - 1][1] + f[i - 1][2]) % MOD;
            f[i][1] = f[i - 1][0] * 2 % MOD;
            f[i][2] = (f[i - 1][0] * 2 + f[i - 1][3]) % MOD;
            f[i][3] = f[i - 1][2] * 2 % MOD;
        }
    }

    public int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        return (int) ((f[n - 1][0] * 4 + f[n - 1][1] * 2 +
                       f[n - 1][2] * 2 + f[n - 1][3]) % MOD);
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
const int MX = 5000;
long long f[MX][4];

int init = []() {
    f[0][0] = f[0][1] = f[0][2] = f[0][3] = 1;
    for (int i = 1; i < MX; i++) {
        f[i][0] = (f[i - 1][1] + f[i - 1][2]) % MOD;
        f[i][1] = f[i - 1][0] * 2 % MOD;
        f[i][2] = (f[i - 1][0] * 2 + f[i - 1][3]) % MOD;
        f[i][3] = f[i - 1][2] * 2 % MOD;
    }
    return 0;
}();

class Solution {
public:
    int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        return (f[n - 1][0] * 4 + f[n - 1][1] * 2 +
                f[n - 1][2] * 2 + f[n - 1][3]) % MOD;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

var f = [5000][4]int{{1, 1, 1, 1}}

func init() {
    for i := 1; i < len(f); i++ {
        f[i][0] = (f[i-1][1] + f[i-1][2]) % mod
        f[i][1] = f[i-1][0] * 2 % mod
        f[i][2] = (f[i-1][0]*2 + f[i-1][3]) % mod
        f[i][3] = f[i-1][2] * 2 % mod
    }
}

func knightDialer(n int) int {
    if n == 1 {
        return 10
    }
    return (f[n-1][0]*4 + f[n-1][1]*2 + f[n-1][2]*2 + f[n-1][3]) % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。忽略预处理的时间和空间。
- 空间复杂度：$\mathcal{O}(1)$。

## 五、矩阵快速幂优化

把状态转移方程用矩阵乘法表示，即

$$
\begin{bmatrix}
f[i][0] \\
f[i][1] \\
f[i][2] \\
f[i][3] \\
\end{bmatrix}
= \begin{bmatrix}
0 & 1 & 1 & 0 \\
2 & 0 & 0 & 0 \\
2 & 0 & 0 & 1 \\
0 & 0 & 2 & 0 \\
\end{bmatrix}
\begin{bmatrix}
f[i-1][0] \\
f[i-1][1] \\
f[i-1][2] \\
f[i-1][3] \\
\end{bmatrix}
$$

把上式中的三个矩阵分别记作 $F[i],M,F[i-1]$，即

$$
F[i] = M\times F[i-1]
$$

那么有

$$
\begin{aligned}
F[n-1] &= M\times F[n-2]      \\
&= M\times M\times F[n-3]        \\
&= M\times M\times M\times  F[n-4]        \\
&\ \ \vdots  \\
&= M^{n-1}\times F[0] \\
\end{aligned}
$$

其中 $M^{n-1}$ 可以用**快速幂**计算，原理请看[【图解】一张图秒懂快速幂](https://leetcode.cn/problems/powx-n/solution/tu-jie-yi-zhang-tu-miao-dong-kuai-su-mi-ykp3i/)。

初始值

$$
F[0] =
\begin{bmatrix}
1 \\
1 \\
1 \\
1 \\
\end{bmatrix}
$$

> 注：由于力扣不计入预处理的时间，矩阵快速幂的实际运行时间可能比递推慢。

```py [sol-Python3]
MOD = 1_000_000_007

# a @ b，其中 @ 是矩阵乘法
def mul(a: List[List[int]], b: List[List[int]]) -> List[List[int]]:
    return [[sum(x * y for x, y in zip(row, col)) % MOD for col in zip(*b)]
            for row in a]

# a^n @ f0
def pow_mul(a: List[List[int]], n: int, f0: List[List[int]]) -> List[List[int]]:
    res = f0
    while n:
        if n & 1:
            res = mul(a, res)
        a = mul(a, a)
        n >>= 1
    return res

class Solution:
    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        m = [[0, 1, 1, 0], [2, 0, 0, 0], [2, 0, 0, 1], [0, 0, 2, 0]]
        f0 = [[1], [1], [1], [1]]
        fn = pow_mul(m, n - 1, f0)
        return (fn[0][0] * 4 + fn[1][0] * 2 + fn[2][0] * 2 + fn[3][0]) % MOD
```

```py [sol-NumPy]
import numpy as np

MOD = 1_000_000_007

# a^n @ f0
def pow_mul(a: np.ndarray, n: int, f0: np.ndarray) -> np.ndarray:
    res = f0
    while n:
        if n & 1:
            res = a @ res % MOD
        a = a @ a % MOD
        n >>= 1
    return res

class Solution:
    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        m = np.array([[0, 1, 1, 0], [2, 0, 0, 0], [2, 0, 0, 1], [0, 0, 2, 0]], dtype=object)
        f0 = np.ones((4,), dtype=object)
        fn = pow_mul(m, n - 1, f0)
        return (fn[0] * 4 + fn[1] * 2 + fn[2] * 2 + fn[3]) % MOD
```

```py [sol-NumPy 写法二]
import numpy as np

class Solution:
    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        m = np.array([[0, 1, 1, 0], [2, 0, 0, 0], [2, 0, 0, 1], [0, 0, 2, 0]], dtype=object)
        f0 = np.ones((4,), dtype=object)
        # 注：没法中途取模，参与运算的数字很大，效率低，所以我并不推荐这种写法
        fn = np.linalg.matrix_power(m, n - 1) @ f0
        return (fn[0] * 4 + fn[1] * 2 + fn[2] * 2 + fn[3]) % 1_000_000_007
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        long[][] m = {
            {0, 1, 1, 0},
            {2, 0, 0, 0},
            {2, 0, 0, 1},
            {0, 0, 2, 0},
        };
        long[][] f0 = {{1}, {1}, {1}, {1}};
        long[][] fn = powMul(m, n - 1, f0);
        return (int) ((fn[0][0] * 4 + fn[1][0] * 2 +
                       fn[2][0] * 2 + fn[3][0]) % MOD);
    }

    // a^n * f0
    private long[][] powMul(long[][] a, int n, long[][] f0) {
        long[][] res = f0;
        while (n > 0) {
            if ((n & 1) > 0) {
                res = mul(a, res);
            }
            a = mul(a, a);
            n >>= 1;
        }
        return res;
    }

    // 返回矩阵 a 和矩阵 b 相乘的结果
    private long[][] mul(long[][] a, long[][] b) {
        long[][] c = new long[a.length][b[0].length];
        for (int i = 0; i < a.length; i++) {
            for (int k = 0; k < a[i].length; k++) {
                if (a[i][k] == 0) {
                    continue;
                }
                for (int j = 0; j < b[k].length; j++) {
                    c[i][j] = (c[i][j] + a[i][k] * b[k][j]) % MOD;
                }
            }
        }
        return c;
    }
}
```

```cpp [sol-C++]
constexpr int MOD = 1'000'000'007;

using matrix = vector<vector<long long>>;

// 返回矩阵 a 和矩阵 b 相乘的结果
matrix mul(matrix& a, matrix& b) {
    int n = a.size(), m = b[0].size();
    matrix c = matrix(n, vector<long long>(m));
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < a[i].size(); k++) {
            if (a[i][k] == 0) {
                continue;
            }
            for (int j = 0; j < m; j++) {
                c[i][j] = (c[i][j] + a[i][k] * b[k][j]) % MOD;
            }
        }
    }
    return c;
}

// a^n * f0
matrix pow_mul(matrix a, int n, matrix& f0) {
    matrix res = f0;
    while (n) {
        if (n & 1) {
            res = mul(a, res);
        }
        a = mul(a, a);
        n >>= 1;
    }
    return res;
}

class Solution {
public:
    int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        matrix m = {
            {0, 1, 1, 0},
            {2, 0, 0, 0},
            {2, 0, 0, 1},
            {0, 0, 2, 0},
        };
        matrix f0 = {{1}, {1}, {1}, {1}};
        matrix fn = pow_mul(m, n - 1, f0);
        return (fn[0][0] * 4 + fn[1][0] * 2 +
                fn[2][0] * 2 + fn[3][0]) % MOD;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

type matrix [][]int

func newMatrix(n, m int) matrix {
    a := make(matrix, n)
    for i := range a {
        a[i] = make([]int, m)
    }
    return a
}

// 返回矩阵 a 和矩阵 b 相乘的结果
func (a matrix) mul(b matrix) matrix {
    c := newMatrix(len(a), len(b[0]))
    for i, row := range a {
        for k, x := range row {
            if x == 0 {
                continue
            }
            for j, y := range b[k] {
                c[i][j] = (c[i][j] + x*y) % mod
            }
        }
    }
    return c
}

// a^n * f0
func (a matrix) powMul(n int, f0 matrix) matrix {
    res := f0
    for ; n > 0; n /= 2 {
        if n%2 > 0 {
            res = a.mul(res)
        }
        a = a.mul(a)
    }
    return res
}

func knightDialer(n int) int {
    if n == 1 {
        return 10
    }
    m := matrix{
        {0, 1, 1, 0},
        {2, 0, 0, 0},
        {2, 0, 0, 1},
        {0, 0, 2, 0},
    }
    f0 := matrix{{1}, {1}, {1}, {1}}
    fn := m.powMul(n-1, f0)
    return (fn[0][0]*4 + fn[1][0]*2 + fn[2][0]*2 + fn[3][0]) % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(D^3\log n)$，其中 $D=4$ 为矩阵的阶数。
- 空间复杂度：$\mathcal{O}(D^2)$。

## 六、Berlekamp-Massey 算法 + Kitamasa 算法

使用 [Berlekamp-Massey 算法](https://zhuanlan.zhihu.com/p/1966417899825665440) 找规律，可以直接得到线性递推式

$$
f_n = 6f_{n-2}-4f_{n-4} \ \ (n \ge 5)
$$

初始值 $f = [10,20,46,104,240]$。

然后使用 [Kitamasa 算法](https://zhuanlan.zhihu.com/p/1964051212304364939) 解决。

```py [sol-Python3]
MOD = 1_000_000_007

class Solution:
    # 给定常系数齐次线性递推式 f(n) = coef[k-1] * f(n-1) + coef[k-2] * f(n-2) + ... + coef[0] * f(n-k)
    # 以及初始值 f(i) = a[i] (0 <= i < k)
    # 返回 f(n) % MOD，其中参数 n 从 0 开始
    # 注意 coef 的顺序
    # 时间复杂度 O(k^2 log n)，其中 k 是 coef 的长度
    def kitamasa(self, coef: List[int], a: List[int], n: int) -> int:
        if n < len(a):
            return a[n] % MOD

        k = len(coef)

        # 已知 f(n) 的各项系数为 a，f(m) 的各项系数为 b
        # 计算并返回 f(n+m) 的各项系数 c
        def compose(a: List[int], b: List[int]) -> List[int]:
            c = [0] * k
            for v in a:
                for j, w in enumerate(b):
                    c[j] = (c[j] + v * w) % MOD
                # 原地计算下一组系数，比如已知 f(4) 的各项系数，现在要计算 f(5) 的各项系数
                # 倒序遍历，避免提前覆盖旧值
                bk1 = b[-1]
                for i in range(k - 1, 0, -1):
                    b[i] = (b[i - 1] + bk1 * coef[i]) % MOD
                b[0] = bk1 * coef[0] % MOD
            return c

        # 计算 res_c，以表出 f(n) = res_c[k-1] * a[k-1] + res_c[k-2] * a[k-2] + ... + res_c[0] * a[0]
        res_c = [0] * k
        c = [0] * k
        res_c[0] = c[1] = 1
        while n > 0:
            if n % 2:
                res_c = compose(c, res_c)
            # 由于会修改 compose 的第二个参数，这里把 c 复制一份再传入
            c = compose(c, c[:])
            n //= 2

        return sum(c * v for c, v in zip(res_c, a)) % MOD

    def knightDialer(self, n: int) -> int:
        if n == 1:
            return 10
        coef = [0, 6, 0, -4]
        a = [20, 46, 104, 240]
        return self.kitamasa(coef[::-1], a, n - 2)  #  注意 kitamasa 入参的顺序
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    private static final int[] coef = {-4, 0, 6, 0}; // 注意 kitamasa 入参的顺序
    private static final int[] a = {20, 46, 104, 240}; // 初始值

    public int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        return kitamasa(coef, a, n - 2);
    }

    // 给定常系数齐次线性递推式 f(n) = coef[k-1] * f(n-1) + ... + coef[0] * f(n-k)
    // 以及初始值 f(i) = a[i] (0 <= i < k)
    // 返回 f(n) % MOD，其中参数 n 从 0 开始
    // 注意 coef 的顺序
    // 时间复杂度 O(k^2 log n)，其中 k 是 coef 的长度
    private int kitamasa(int[] coef, int[] a, long n) {
        if (n < a.length) {
            return a[(int) n] % MOD;
        }

        int k = coef.length;

        // 计算 resC，以表出 f(n) = resC[k-1] * a[k-1] + ... + resC[0] * a[0]
        int[] resC = new int[k];
        int[] c = new int[k];
        resC[0] = c[1] = 1;
        for (; n > 0; n /= 2) {
            if (n % 2 > 0) {
                resC = compose(coef, c, resC);
            }
            // 由于会修改 compose 的第三个参数，这里把 c 复制一份再传入
            c = compose(coef, c, c.clone());
        }

        long ans = 0;
        for (int i = 0; i < k; i++) {
            ans = (ans + (long) resC[i] * a[i]) % MOD;
        }

        return (int) ((ans + MOD) % MOD); // 保证返回值非负
    }

    // 已知 f(n) 的各项系数为 a，f(m) 的各项系数为 b
    // 计算并返回 f(n+m) 的各项系数 c
    private int[] compose(int[] coef, int[] a, int[] b) {
        int k = a.length;
        int[] c = new int[k];
        for (int v : a) {
            for (int j = 0; j < k; j++) {
                c[j] = (int) ((c[j] + (long) v * b[j]) % MOD);
            }
            // 原地计算下一组系数，比如已知 f(4) 的各项系数，现在要计算 f(5) 的各项系数
            // 倒序遍历，避免提前覆盖旧值
            long bk1 = b[k - 1];
            for (int i = k - 1; i > 0; i--) {
                b[i] = (int) ((b[i - 1] + bk1 * coef[i]) % MOD);
            }
            b[0] = (int) (bk1 * coef[0] % MOD);
        }
        return c;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int MOD = 1'000'000'007;

    // 给定常系数齐次线性递推式 f(n) = coef[k-1] * f(n-1) + coef[k-2] * f(n-2) + ... + coef[0] * f(n-k)
    // 以及初始值 f(i) = a[i] (0 <= i < k)
    // 返回 f(n) % MOD，其中参数 n 从 0 开始
    // 注意 coef 的顺序
    // 时间复杂度 O(k^2 log n)，其中 k 是 coef 的长度
    int kitamasa(const vector<int>& coef, const vector<int>& a, long long n) {
        if (n < a.size()) {
            return a[n] % MOD;
        }

        int k = coef.size();

        // 已知 f(n) 的各项系数为 A，f(m) 的各项系数为 B
        // 计算并返回 f(n+m) 的各项系数 C
        auto compose = [&](const vector<int>& A, vector<int> B) -> vector<int> {
            vector<int> C(k);
            for (int v : A) {
                for (int j = 0; j < k; j++) {
                    C[j] = (C[j] + 1LL * v * B[j]) % MOD;
                }
                // 原地计算下一组系数，比如已知 f(4) 的各项系数，现在要计算 f(5) 的各项系数
                // 倒序遍历，避免提前覆盖旧值
                int bk1 = B.back();
                for (int i = k - 1; i > 0; i--) {
                    B[i] = (B[i - 1] + 1LL * bk1 * coef[i]) % MOD;
                }
                B[0] = 1LL * bk1 * coef[0] % MOD;
            }
            return C;
        };

        // 计算 res_c，以表出 f(n) = res_c[k-1] * a[k-1] + res_c[k-2] * a[k-2] + ... + res_c[0] * a[0]
        vector<int> res_c(k), c(k);
        res_c[0] = c[1] = 1;
        for (; n > 0; n /= 2) {
            if (n % 2) {
                res_c = compose(c, move(res_c));
            }
            c = compose(c, c);
        }

        long long ans = 0;
        for (int i = 0; i < k; i++) {
            ans = (ans + 1LL * res_c[i] * a[i]) % MOD;
        }

        return (ans + MOD) % MOD; // 保证返回值非负
    }

public:
    int knightDialer(int n) {
        if (n == 1) {
            return 10;
        }
        vector<int> coef = {0, 6, 0, -4};
        vector<int> a = {20, 46, 104, 240};
        ranges::reverse(coef); // 注意 kitamasa 入参的顺序
        return kitamasa(coef, a, n - 2);
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

// 给定常系数齐次线性递推式 f(n) = coef[k-1] * f(n-1) + coef[k-2] * f(n-2) + ... + coef[0] * f(n-k)
// 以及初始值 f(i) = a[i] (0 <= i < k)
// 返回 f(n) % mod，其中参数 n 从 0 开始
// 注意 coef 的顺序
// 时间复杂度 O(k^2 log n)，其中 k 是 coef 的长度
func kitamasa(coef, a []int, n int) (ans int) {
    if n < len(a) {
        return a[n] % mod
    }

    k := len(coef)

    // 已知 f(n) 的各项系数为 a，f(m) 的各项系数为 b
    // 计算并返回 f(n+m) 的各项系数 c
    compose := func(a, b []int) []int {
        c := make([]int, k)
        for _, v := range a {
            for j, w := range b {
                c[j] = (c[j] + v*w) % mod
            }
            // 原地计算下一组系数，比如已知 f(4) 的各项系数，现在要计算 f(5) 的各项系数
            // 倒序遍历，避免提前覆盖旧值
            bk1 := b[k-1]
            for i := k - 1; i > 0; i-- {
                b[i] = (b[i-1] + bk1*coef[i]) % mod
            }
            b[0] = bk1 * coef[0] % mod
        }
        return c
    }

    // 计算 resC，以表出 f(n) = resC[k-1] * a[k-1] + resC[k-2] * a[k-2] + ... + resC[0] * a[0]
    resC := make([]int, k)
    resC[0] = 1
    c := make([]int, k)
    c[1] = 1
    for ; n > 0; n /= 2 {
        if n%2 > 0 {
            resC = compose(c, resC)
        }
        // 由于会修改 compose 的第二个参数，这里把 c 复制一份再传入
        c = compose(c, slices.Clone(c))
    }

    for i, c := range resC {
        ans = (ans + c*a[i]) % mod
    }

    return (ans + mod) % mod // 保证结果非负
}

func knightDialer(n int) int {
    if n == 1 {
        return 10
    }
    coef := []int{0, 6, 0, -4}
    a := []int{20, 46, 104, 240}
    slices.Reverse(coef) // 注意 kitamasa 入参的顺序
    return kitamasa(coef, a, n-2)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(D^2\log n)$，其中 $D=4$ 为递推式的阶数。
- 空间复杂度：$\mathcal{O}(D)$。

## 专题训练

见下面动态规划题单的「**§11.6 矩阵快速幂优化 DP**」。

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

本题来自 `六、状态机 DP / §6.3 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、状态机 DP / §6.3 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
