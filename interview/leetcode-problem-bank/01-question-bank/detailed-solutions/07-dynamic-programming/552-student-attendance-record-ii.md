# 552. 学生出勤记录 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/student-attendance-record-ii/
- 题目 slug：`student-attendance-record-ii`
- 来源专题：动态规划
- 来源分类路径：十一、优化 DP / §11.6 矩阵快速幂优化 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/student-attendance-record-ii/solutions/2885136/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-a8kj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推，再到矩阵快速幂优化！（Python/Java/C++/Go）](https://leetcode.cn/problems/student-attendance-record-ii/solutions/2885136/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-a8kj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-a8kj`
- topic id：`2885136`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、寻找子问题

本题要构造长为 $n$ 的字符串 $s$，同时满足：

1. $s$ 至多包含 $1$ 个 $\texttt{A}$。
2. $s$ 不包含 $\texttt{LLL}$，也就是说，至多有 $2$ 个连续的 $\texttt{L}$。

考虑按顺序一个一个地填入字母。在这个过程中，为了判断是否填完了、可以填哪些字母，我们需要知道：

- 还剩下多少个字母需要填。
- 已经填了多少个 $\texttt{A}$。如果之前填过 $\texttt{A}$，那么后续不能填 $\texttt{A}$。
- 相邻位置上有多少个连续 $\texttt{L}$。如果之前连续填了 $2$ 个 $\texttt{L}$，那么当前位置不能填 $\texttt{L}$。

**从右往左**填入字母。一开始，右边填的字母不含 $\texttt{A}$，且上一个填的字母不是 $\texttt{L}$。

考虑 $s$ 的最后一个位置填什么字母：

- 填 $\texttt{P}$，接下来要解决的问题是：在右边填的字母不含 $\texttt{A}$，且上一个填的字母不是 $\texttt{L}$ 的情况下，继续向左填字母，能构造多少个长为 $n-1$ 的字符串。
- 填 $\texttt{A}$，接下来要解决的问题是：在右边填的字母包含 $\texttt{A}$，且上一个填的字母不是 $\texttt{L}$ 的情况下，继续向左填字母，能构造多少个长为 $n-1$ 的字符串。在这种情况下，后续位置不能填 $\texttt{A}$。
- 填 $\texttt{L}$，接下来要解决的问题是：在右边填的字母不含 $\texttt{A}$，且上一个填的字母是 $\texttt{L}$ 的情况下，继续向左填字母，能构造多少个长为 $n-1$ 的字符串。如果继续填 $\texttt{L}$，问题变成，在右边填的字母不含 $\texttt{A}$，且右边相邻位置有 $2$ 个连续 $\texttt{L}$ 的情况下，继续向左填字母，能构造多少个长为 $n-2$ 的字符串。在这种情况下，下一个位置不能填 $\texttt{L}$。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

> 注：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。

## 二、状态定义与状态转移方程

根据上面的讨论，我们需要在递归过程中跟踪以下信息：

- 还剩下 $i$ 个字母需要填。
- 右边填了 $j$ 个 $\texttt{A}$。
- 右边相邻位置有 $k$ 个连续 $\texttt{L}$。

因此，定义状态为 $\textit{dfs}(i,j,k)$，表示在在右边填了 $j$ 个 $\texttt{A}$，且右边相邻位置有 $k$ 个连续 $\texttt{L}$ 的情况下，继续向左填字母，能构造多少个长为 $i$ 的字符串。

接下来，思考如何从一个状态转移到另一个状态。

考虑长为 $i$ 的字符串的最后一个位置填什么字母：

- 填 $\texttt{P}$，接下来要解决的问题是：在右边填了 $j$ 个 $\texttt{A}$，且右边相邻位置有 $0$ 个连续 $\texttt{L}$ 的情况下，继续向左填字母，能构造多少个长为 $i-1$ 的字符串，即 $\textit{dfs}(i-1,j,0)$。
- 如果 $j=0$，那么可以填 $\texttt{A}$，接下来要解决的问题是：在右边填了 $1$ 个 $\texttt{A}$，且右边相邻位置有 $0$ 个连续 $\texttt{L}$ 的情况下，继续向左填字母，能构造多少个长为 $i-1$ 的字符串，即 $\textit{dfs}(i-1,1,0)$。
- 如果 $k<2$，那么可以填 $\texttt{L}$，接下来要解决的问题是：在右边填了 $j$ 个 $\texttt{A}$，且右边相邻位置有 $k+1$ 个连续 $\texttt{L}$ 的情况下，继续向左填字母，能构造多少个长为 $i-1$ 的字符串，即 $\textit{dfs}(i-1,j,k+1)$。

由于最后一个位置填的字母不同，所以三种情况互相独立，根据**加法原理**，将上述方案数相加，即为 $\textit{dfs}(i,j,k)$。

递归边界：$\textit{dfs}(0,j,k)=1$。如果能递归到 $i=0$ 的状态，说明我们找到了一个合法方案，返回 $1$。

递归入口：$\textit{dfs}(n,0,0)$，也就是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j,k)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

本题方案数均为正数，初始化成 $0$ 也可以。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

注意要在 $\textit{dfs}$ 的中途取模，原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

代码实现时，可以把 $\textit{dfs}$ 写在外面，这样多个测试用例之间可以**共享**记忆化搜索的结果，效率更高。

```py [sol-Python3]
MOD = 1_000_000_007

@cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
def dfs(i: int, j: int, k: int) -> int:
    if i == 0:
        return 1
    res = dfs(i - 1, j, 0)  # 填 P
    if j == 0:
        res += dfs(i - 1, 1, 0)  # 填 A
    if k < 2:
        res += dfs(i - 1, j, k + 1)  # 填 L
    return res % MOD

class Solution:
    def checkRecord(self, n: int) -> int:
        return dfs(n, 0, 0)
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int MX = 100_001;
    private static final int[][][] memo = new int[MX][2][3];

    public int checkRecord(int n) {
        return dfs(n, 0, 0);
    }

    private static int dfs(int i, int j, int k) {
        if (i == 0) {
            return 1;
        }
        if (memo[i][j][k] > 0) { // 之前计算过
            return memo[i][j][k];
        }
        long res = dfs(i - 1, j, 0); // 填 P
        if (j == 0) {
            res += dfs(i - 1, 1, 0); // 填 A
        }
        if (k < 2) {
            res += dfs(i - 1, j, k + 1); // 填 L
        }
        return memo[i][j][k] = (int) (res % MOD);
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
const int MX = 100'001;

int memo[MX][2][3];

int dfs(int i, int j, int k) {
    if (i == 0) {
        return 1;
    }
    int& res = memo[i][j][k]; // 注意这里是引用
    if (res) {
        return res;
    }
    res = dfs(i - 1, j, 0); // 填 P
    if (j == 0) {
        res = (res + dfs(i - 1, 1, 0)) % MOD; // 填 A
    }
    if (k < 2) {
        res = (res + dfs(i - 1, j, k + 1)) % MOD; // 填 L
    }
    return res;
};

class Solution {
public:
    int checkRecord(int n) {
        return dfs(n, 0, 0);
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007
const mx = 100_001

var memo [mx][2][3]int

func dfs(i, j, k int) int {
    if i == 0 {
        return 1
    }
    p := &memo[i][j][k]
    if *p > 0 { // 之前计算过
        return *p
    }
    res := dfs(i-1, j, 0) // 填 P
    if j == 0 {
        res += dfs(i-1, 1, 0) // 填 A
    }
    if k < 2 {
        res += dfs(i-1, j, k+1) // 填 L
    }
    *p = res % mod // 记忆化
    return *p
}

func checkRecord(n int) int {
    return dfs(n, 0, 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i][j][k]$ 的定义和 $\textit{dfs}(i,j,k)$ 的定义是一样的，都表示在右边填了 $j$ 个 $\texttt{A}$，且右边相邻位置有 $k$ 个连续 $\texttt{L}$ 的情况下，继续向左填字母，能构造多少个长为 $i$ 的字符串。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样。

初始值 $f[0][j][k]=1$，翻译自递归边界 $\textit{dfs}(0,j,k)=1$。

答案为 $f[n][0][0]$，翻译自递归入口 $\textit{dfs}(n,0,0)$。

代码实现时，可以把 $f$ 数组和递推的计算过程写在外面，方便在多个测试用例之间**共享**，效率更高。

```py [sol-Python3]
MOD = 1_000_000_007
MX = 100_001

f = [[[0] * 3, [0] * 3] for _ in range(MX)]
f[0] = [[1] * 3, [1] * 3]
for i in range(1, MX):
    for j in range(2):
        for k in range(3):
            res = f[i - 1][j][0]
            if j == 0:
                res += f[i - 1][1][0]
            if k < 2:
                res += f[i - 1][j][k + 1]
            f[i][j][k] = res % MOD

class Solution:
    def checkRecord(self, n: int) -> int:
        return f[n][0][0]
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int MX = 100_001;
    private static final int[][][] f = new int[MX][2][3];

    static {
        f[0] = new int[][]{{1, 1, 1}, {1, 1, 1}};
        for (int i = 1; i < MX; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 3; k++) {
                    long res = f[i - 1][j][0];
                    if (j == 0) {
                        res += f[i - 1][1][0];
                    }
                    if (k < 2) {
                        res += f[i - 1][j][k + 1];
                    }
                    f[i][j][k] = (int) (res % MOD);
                }
            }
        }
    }

    public int checkRecord(int n) {
        return f[n][0][0];
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
const int MX = 100'001;

int f[MX][2][3];

auto init = [] {
    f[0][0][0] = f[0][0][1] = f[0][0][2] = f[0][1][0] = f[0][1][1] = f[0][1][2] = 1;
    for (int i = 1; i < MX; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 3; k++) {
                int& res = f[i][j][k];
                res = f[i - 1][j][0];
                if (j == 0) {
                    res = (res + f[i - 1][1][0]) % MOD;
                }
                if (k < 2) {
                    res = (res + f[i - 1][j][k + 1]) % MOD;
                }
            }
        }
    }
    return 0;
}();

class Solution {
public:
    int checkRecord(int n) {
        return f[n][0][0];
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007
const mx = 100_001

var f [mx][2][3]int

func init() {
    f[0] = [2][3]int{{1, 1, 1}, {1, 1, 1}}
    for i := 1; i < mx; i++ {
        for j := 0; j < 2; j++ {
            for k := 0; k < 3; k++ {
                res := f[i-1][j][0]
                if j == 0 {
                    res += f[i-1][1][0]
                }
                if k < 2 {
                    res += f[i-1][j][k+1]
                }
                f[i][j][k] = res % mod
            }
        }
    }
}

func checkRecord(n int) int {
    return f[n][0][0]
}
```

#### 复杂度分析

- 时间复杂度：预处理 $\mathcal{O}(N)$，每次调用 $\texttt{checkRecord}$ 时 $\mathcal{O}(1)$。其中 $N=10^5$。
- 空间复杂度：预处理 $\mathcal{O}(N)$，每次调用 $\texttt{checkRecord}$ 时 $\mathcal{O}(1)$。

## 五、矩阵快速幂优化

为方便表示成矩阵乘法，首先把 $f$ 数组改成二维的，即用 $f[i][3j+k]$ 代替原来的 $f[i][j][k]$。

完整写出 $(j,k)=(0,0),(0,1),(0,2),(1,0),(1,1),(1,2)$ 分别对应的 $6$ 个状态转移方程：

$$
\begin{aligned}
f[i][0] &= f[i-1][0] + f[i-1][1] + f[i-1][3]     \\
f[i][1] &= f[i-1][0] + f[i-1][2] + f[i-1][3]     \\
f[i][2] &= f[i-1][0] + f[i-1][3]     \\
f[i][3] &= f[i-1][3] + f[i-1][4]     \\
f[i][4] &= f[i-1][3] + f[i-1][5]     \\
f[i][5] &= f[i-1][3]      \\
\end{aligned}
$$

用矩阵乘法表示，即

$$
\begin{bmatrix}
f[i][0] \\
f[i][1] \\
f[i][2] \\
f[i][3] \\
f[i][4] \\
f[i][5] \\
\end{bmatrix}
= \begin{bmatrix}
1 & 1 & 0 & 1 & 0 & 0 \\
1 & 0 & 1 & 1 & 0 & 0 \\
1 & 0 & 0 & 1 & 0 & 0 \\
0 & 0 & 0 & 1 & 1 & 0 \\
0 & 0 & 0 & 1 & 0 & 1 \\
0 & 0 & 0 & 1 & 0 & 0 \\
\end{bmatrix}
\begin{bmatrix}
f[i-1][0] \\
f[i-1][1] \\
f[i-1][2] \\
f[i-1][3] \\
f[i-1][4] \\
f[i-1][5] \\
\end{bmatrix}
$$

把上式中的三个矩阵分别记作 $F[i],M,F[i-1]$，即

$$
F[i] = M\times F[i-1]
$$

那么有

$$
\begin{aligned}
F[n] &= M\times F[n-1]      \\
&= M\times M\times F[n-2]        \\
&= M\times M\times M\times  F[n-3]        \\
&\ \ \vdots  \\
&= M^n\times F[0]
\end{aligned}
$$

其中

$$
F[0] = 
\begin{bmatrix}
1 \\
1 \\
1 \\
1 \\
1 \\
1 \\
\end{bmatrix}
$$

$M^n$ 可以用**快速幂**计算，原理请看[【图解】一张图秒懂快速幂](https://leetcode.cn/problems/powx-n/solution/tu-jie-yi-zhang-tu-miao-dong-kuai-su-mi-ykp3i/)。

由于答案 $f[n][0]$ 是 $F[n]$ 的第一项，根据矩阵乘法，其等于 $M^n$ 的第一行乘以 $F[0]$。由于 $F[0]$ 全为 $1$，答案也等于 $M^n$ 第一行的元素和。

```py [sol-Python3]
MOD = 1_000_000_007

# 返回矩阵 a 和矩阵 b 相乘的结果
def mul(a: List[List[int]], b: List[List[int]]) -> List[List[int]]:
    return [[sum(x * y for x, y in zip(row, col)) % MOD for col in zip(*b)]
            for row in a]

# 返回 n 个矩阵 a 相乘的结果
def pow(a: List[List[int]], n: int) -> List[List[int]]:
    size = len(a)
    res = [[0] * size for _ in range(size)]
    for i in range(size):
        res[i][i] = 1  # 单位矩阵
    while n:
        if n & 1:
            res = mul(res, a)
        a = mul(a, a)
        n >>= 1
    return res

class Solution:
    def checkRecord(self, n: int) -> int:
        m = [[1, 1, 0, 1, 0, 0],
             [1, 0, 1, 1, 0, 0],
             [1, 0, 0, 1, 0, 0],
             [0, 0, 0, 1, 1, 0],
             [0, 0, 0, 1, 0, 1],
             [0, 0, 0, 1, 0, 0]]
        return sum(pow(m, n)[0]) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int SIZE = 6;

    public int checkRecord(int n) {
        int[][] m = {
            {1, 1, 0, 1, 0, 0},
            {1, 0, 1, 1, 0, 0},
            {1, 0, 0, 1, 0, 0},
            {0, 0, 0, 1, 1, 0},
            {0, 0, 0, 1, 0, 1},
            {0, 0, 0, 1, 0, 0},
        };
        int ans = 0;
        for (int x : pow(m, n)[0]) {
            ans = (ans + x) % MOD;
        }
        return ans;
    }

    // 返回 n 个矩阵 a 相乘的结果
    private int[][] pow(int[][] a, int n) {
        int[][] res = new int[SIZE][SIZE];
        for (int i = 0; i < SIZE; i++) {
            res[i][i] = 1; // 单位矩阵
        }
        while (n > 0) {
            if ((n & 1) > 0) {
                res = mul(res, a);
            }
            a = mul(a, a);
            n >>= 1;
        }
        return res;
    }

    // 返回矩阵 a 和矩阵 b 相乘的结果
    private int[][] mul(int[][] a, int[][] b) {
        int[][] c = new int[SIZE][SIZE];
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                for (int k = 0; k < SIZE; k++) {
                    c[i][j] = (int) ((c[i][j] + (long) a[i][k] * b[k][j]) % MOD);
                }
            }
        }
        return c;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int MOD = 1'000'000'007;
    static constexpr int SIZE = 6;

    using matrix = array<array<int, SIZE>, SIZE>;

    // 返回矩阵 a 和矩阵 b 相乘的结果
    matrix mul(matrix& a, matrix& b) {
        matrix c{};
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                for (int k = 0; k < SIZE; k++) {
                    c[i][j] = (c[i][j] + (long long) a[i][k] * b[k][j]) % MOD;
                }
            }
        }
        return c;
    }

    // 返回 n 个矩阵 a 相乘的结果
    matrix pow(matrix a, int n) {
        matrix res = {};
        for (int i = 0; i < SIZE; i++) {
            res[i][i] = 1; // 单位矩阵
        }
        while (n) {
            if (n & 1) {
                res = mul(res, a);
            }
            a = mul(a, a);
            n >>= 1;
        }
        return res;
    }

public:
    int checkRecord(int n) {
        matrix m = {{
            {{1, 1, 0, 1, 0, 0}},
            {{1, 0, 1, 1, 0, 0}},
            {{1, 0, 0, 1, 0, 0}},
            {{0, 0, 0, 1, 1, 0}},
            {{0, 0, 0, 1, 0, 1}},
            {{0, 0, 0, 1, 0, 0}},
        }};
        matrix res = pow(m, n);
        int ans = 0;
        for (int x : res[0]) {
            ans = (ans + x) % MOD;
        }
        return ans;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

type matrix [6][6]int

// 返回矩阵 a 和矩阵 b 相乘的结果
func (a matrix) mul(b matrix) matrix {
    c := matrix{}
    for i, row := range a {
        for j := range b[0] {
            for k, x := range row {
                c[i][j] = (c[i][j] + x*b[k][j]) % mod
            }
        }
    }
    return c
}

// 返回 n 个矩阵 a 相乘的结果
func (a matrix) pow(n int) matrix {
    res := matrix{}
    for i := range res {
        res[i][i] = 1 // 单位矩阵
    }
    for n > 0 {
        if n&1 > 0 {
            res = res.mul(a)
        }
        a = a.mul(a)
        n >>= 1
    }
    return res
}

var m = matrix{
    {1, 1, 0, 1, 0, 0},
    {1, 0, 1, 1, 0, 0},
    {1, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 0, 1},
    {0, 0, 0, 1, 0, 0},
}

func checkRecord(n int) (ans int) {
    for _, x := range m.pow(n)[0] {
        ans += x
    }
    return ans % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(K^3\log n)$，其中 $K=6$ 是系数矩阵的阶。
- 空间复杂度：$\mathcal{O}(K^2)$。系数矩阵需要的空间。

## 六、Berlekamp-Massey 算法 + Kitamasa 算法

使用 [Berlekamp-Massey 算法](https://zhuanlan.zhihu.com/p/1966417899825665440) 找规律，可以直接得到线性递推式

$$
f_n = 2f_{n-1}+f_{n-2}-3f_{n-4}-2f_{n-5}-f_{n-6} \ \ (n \ge 6)
$$

初始值 $f = [1,3,8,19,43,94]$。

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

    def checkRecord(self, n: int) -> int:
        coef = [2, 1, 0, -3, -2, -1]
        a = [1, 3, 8, 19, 43, 94]
        return self.kitamasa(coef[::-1], a, n)  #  注意 kitamasa 入参的顺序
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    private static final int[] coef = {-1, -2, -3, 0, 1, 2}; // 注意 kitamasa 入参的顺序
    private static final int[] a = {1, 3, 8, 19, 43, 94}; // 初始值

    public int checkRecord(int n) {
        return kitamasa(coef, a, n);
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
    int checkRecord(int n) {
        vector<int> coef = {2, 1, 0, -3, -2, -1};
        vector<int> a = {1, 3, 8, 19, 43, 94};
        ranges::reverse(coef); // 注意 kitamasa 入参的顺序
        return kitamasa(coef, a, n);
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

func checkRecord(n int) int {
    coef := []int{2, 1, 0, -3, -2, -1}
    a := []int{1, 3, 8, 19, 43, 94}
    slices.Reverse(coef) // 注意 kitamasa 入参的顺序
    return kitamasa(coef, a, n)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(K^2\log n)$，其中 $K=6$ 是递推式的阶。
- 空间复杂度：$\mathcal{O}(K)$。

## 相似题目

见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**六、状态机 DP**」和「**§11.6 矩阵快速幂优化 DP**」。

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

本题来自 `十一、优化 DP / §11.6 矩阵快速幂优化 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十一、优化 DP / §11.6 矩阵快速幂优化 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
