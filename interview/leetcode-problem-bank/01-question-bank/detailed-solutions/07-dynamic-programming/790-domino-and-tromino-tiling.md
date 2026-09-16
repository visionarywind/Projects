# 790. 多米诺和托米诺平铺

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/domino-and-tromino-tiling/
- 题目 slug：`domino-and-tromino-tiling`
- 来源专题：动态规划
- 来源分类路径：十一、优化 DP / §11.6 矩阵快速幂优化 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/domino-and-tromino-tiling/solutions/1968516/by-endlesscheng-umpp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】f[n]=2*f[n-1]+f[n-3]（Python/Java/C++/Go）](https://leetcode.cn/problems/domino-and-tromino-tiling/solutions/1968516/by-endlesscheng-umpp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-umpp`
- topic id：`1968516`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

定义 $f[i]$ 表示平铺 $2 \times i$ 面板的方案数。答案为 $f[n]$。

尝试计算 $f$ 的前几项，从中找到规律，得到 $f[i]$ 的递推式。（请点击图片放大查看）

![lc790-2.png](https://pic.leetcode.cn/1746413269-jCDnAs-lc790-2.png)

**文字版解释**

对于 $f[i]$，考虑最右边的可以独立切割出来的矩形，有三种情况：

1. 一个竖着的 $1\times 2$。去掉这个瓷砖，问题变成平铺 $2 \times (i-1)$ 面板的方案数，即 $f[i-1]$。
2. 两个横着的 $2\times 1$。去掉这两瓷砖，问题变成平铺 $2 \times (i-2)$ 面板的方案数，即 $f[i-2]$。
3. 考虑最右边那一对 L 型的铺设方法，这一对 L 型必须一左一右，中间的都是 $2\times 1$ 横着摆。中间的 $2\times 1$ 的个数为 $0,1,2,\ldots,i-3$，对应的子问题分别为 $f[i-3],f[i-4],f[i-5],\ldots,f[0]$，每一种又有上下对称的平铺方案。

累加得

$$
f[i] = f[i-1] + f[i-2] + 2\sum_{j=0}^{i-3} f[j]
$$

化简方式见图。

代码实现时，可以定义 $f[0]=1$，这样我们可以从 $f[3]$ 开始算。 

## 写法一

```py [sol-Python3]
MOD = 1_000_000_007

class Solution:
    def numTilings(self, n: int) -> int:
        if n == 1:
            return 1
        f = [0] * (n + 1)
        f[0] = f[1] = 1
        f[2] = 2
        for i in range(3, n + 1):
            f[i] = (f[i - 1] * 2 + f[i - 3]) % MOD
        return f[n]
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int numTilings(int n) {
        if (n == 1) {
            return 1;
        }
        long[] f = new long[n + 1];
        f[0] = f[1] = 1;
        f[2] = 2;
        for (int i = 3; i <= n; i++) {
            f[i] = (f[i - 1] * 2 + f[i - 3]) % MOD;
        }
        return (int) f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
    const int MOD = 1'000'000'007;
public:
    int numTilings(int n) {
        if (n == 1) {
            return 1;
        }
        vector<long long> f(n + 1);
        f[0] = f[1] = 1;
        f[2] = 2;
        for (int i = 3; i <= n; i++) {
            f[i] = (f[i - 1] * 2 + f[i - 3]) % MOD;
        }
        return f[n];
    }
};
```

```go [sol-Go]
func numTilings(n int) int {
    if n == 1 {
        return 1
    }
    f := make([]int, n+1)
    f[0], f[1], f[2] = 1, 1, 2
    for i := 3; i <= n; i++ {
        f[i] = (f[i-1]*2 + f[i-3]) % 1_000_000_007
    }
    return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 写法二

也可以用 $3$ 个变量滚动计算 $f$。原理见 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

```py [sol-Python3]
MOD = 1_000_000_007

class Solution:
    def numTilings(self, n: int) -> int:
        if n == 1:
            return 1
        a, b, c = 1, 1, 2
        for _ in range(3, n + 1):
            a, b, c = b, c, (c * 2 + a) % MOD
        return c
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int numTilings(int n) {
        if (n == 1) {
            return 1;
        }
        long a = 1, b = 1, c = 2;
        for (int i = 3; i <= n; i++) {
            long f = (c * 2 + a) % MOD;
            a = b;
            b = c;
            c = f;
        }
        return (int) c;
    }
}
```

```cpp [sol-C++]
class Solution {
    const int MOD = 1'000'000'007;
public:
    int numTilings(int n) {
        if (n == 1) {
            return 1;
        }
        long long a = 1, b = 1, c = 2;
        for (int i = 3; i <= n; i++) {
            long f = (c * 2 + a) % MOD;
            a = b;
            b = c;
            c = f;
        }
        return c;
    }
};
```

```go [sol-Go]
func numTilings(n int) int {
    if n == 1 {
        return 1
    }
    a, b, c := 1, 1, 2
    for i := 3; i <= n; i++ {
        a, b, c = b, c, (c*2+a)%1_000_000_007
    }
    return c
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 写法三

由于 $n\le 1000$，可以预处理 $1000$ 以内的 $f$ 值。

由于力扣不计入预处理的时间，所以这种写法的时间复杂度是 $\mathcal{O}(1)$。

```py [sol-Python3]
MOD = 1_000_000_007

f = [0] * 1001
f[0] = f[1] = 1
f[2] = 2
for i in range(3, len(f)):
    f[i] = (f[i - 1] * 2 + f[i - 3]) % MOD

class Solution:
    def numTilings(self, n: int) -> int:
        return f[n]
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final long[] f = new long[1001];

    static {
        f[0] = f[1] = 1;
        f[2] = 2;
        for (int i = 3; i < f.length; i++) {
            f[i] = (f[i - 1] * 2 + f[i - 3]) % MOD;
        }
    }

    public int numTilings(int n) {
        return (int) f[n];
    }
}
```

```cpp [sol-C++]
const int MOD = 1'000'000'007;
const int MX = 1001;
long long f[MX];

int init = []() {
    f[0] = f[1] = 1;
    f[2] = 2;
    for (int i = 3; i < MX; i++) {
        f[i] = (f[i - 1] * 2 + f[i - 3]) % MOD;
    }
    return 0;
}();

class Solution {
public:
    int numTilings(int n) {
        return f[n];
    }
};
```

```go [sol-Go]
var f = [1001]int{1, 1, 2}

func init() {
    for i := 3; i < len(f); i++ {
        f[i] = (f[i-1]*2 + f[i-3]) % 1_000_000_007
    }
}

func numTilings(n int) int {
    return f[n]
}
```

#### 复杂度分析

忽略预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 写法四：矩阵快速幂

下面的方法，即使 $n=10^{18}$ 也能轻松通过。

把状态转移方程用矩阵乘法表示，即

$$
\begin{bmatrix}
f[i] \\
f[i-1] \\
f[i-2] \\
\end{bmatrix}
= \begin{bmatrix}
2 & 0 & 1 \\
1 & 0 & 0 \\
0 & 1 & 0 \\
\end{bmatrix}
\begin{bmatrix}
f[i-1] \\
f[i-2] \\
f[i-3] \\
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
&= M^{n-2}\times F[2] \\
\end{aligned}
$$

其中

$$
F[2] =
\begin{bmatrix}
f[2] \\
f[1] \\
f[0] \\
\end{bmatrix}
=
\begin{bmatrix}
2 \\
1 \\
1 \\
\end{bmatrix}
$$

$M^n$ 可以用**快速幂**计算，原理请看[【图解】一张图秒懂快速幂](https://leetcode.cn/problems/powx-n/solution/tu-jie-yi-zhang-tu-miao-dong-kuai-su-mi-ykp3i/)。

最终答案为 $F[n]$ 的第一项，即 $f[n]$。

```py [sol-Python3]
MOD = 1_000_000_007

# a @ b，其中 @ 是矩阵乘法
def mul(a: List[List[int]], b: List[List[int]]) -> List[List[int]]:
    return [[sum(x * y for x, y in zip(row, col)) % MOD for col in zip(*b)]
            for row in a]

# a^n @ f
def pow_mul(a: List[List[int]], n: int, f: List[List[int]]) -> List[List[int]]:
    res = f
    while n:
        if n & 1:
            res = mul(a, res)
        a = mul(a, a)
        n >>= 1
    return res

class Solution:
    def numTilings(self, n: int) -> int:
        if n == 1:
            return 1
        f2 = [[2], [1], [1]]
        m = [[2, 0, 1], [1, 0, 0], [0, 1, 0]]
        fn = pow_mul(m, n - 2, f2)
        return fn[0][0]
```

```py [sol-NumPy]
import numpy as np

MOD = 1_000_000_007

# a^n @ f
def pow_mul(a: np.ndarray, n: int, f: np.ndarray) -> np.ndarray:
    res = f
    while n:
        if n & 1:
            res = a @ res % MOD
        a = a @ a % MOD
        n >>= 1
    return res

class Solution:
    def numTilings(self, n: int) -> int:
        if n == 1:
            return 1
        f2 = np.array([2, 1, 1], dtype=object)
        m = np.array([[2, 0, 1], [1, 0, 0], [0, 1, 0]], dtype=object)
        fn = pow_mul(m, n - 2, f2)
        return fn[0]
```

```py [sol-NumPy 写法二]
import numpy as np

class Solution:
    def numTilings(self, n: int) -> int:
        if n == 1:
            return 1
        f2 = np.array([2, 1, 1], dtype=object)
        m = np.array([[2, 0, 1], [1, 0, 0], [0, 1, 0]], dtype=object)
        # 注：没法中途取模，参与运算的数字很大，效率低，所以我并不推荐这种写法
        fn = np.linalg.matrix_power(m, n - 2) @ f2
        return fn[0] % 1_000_000_007
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int numTilings(int n) {
        if (n == 1) {
            return 1;
        }
        long[][] f2 = {{2}, {1}, {1}};
        long[][] m = {
            {2, 0, 1},
            {1, 0, 0},
            {0, 1, 0},
        };
        long[][] fn = powMul(m, n - 2, f2);
        return (int) fn[0][0];
    }

    // a^n * f
    private long[][] powMul(long[][] a, int n, long[][] f) {
        long[][] res = f;
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
const int MOD = 1'000'000'007;

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

// a^n * f
matrix pow_mul(matrix a, int n, matrix& f) {
    matrix res = f;
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
    int numTilings(int n) {
        if (n == 1) {
            return 1;
        }
        matrix f2 = {{2}, {1}, {1}};
        matrix m = {
            {2, 0, 1},
            {1, 0, 0},
            {0, 1, 0},
        };
        matrix fn = pow_mul(m, n - 2, f2);
        return fn[0][0];
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

// a^n * f
func (a matrix) powMul(n int, f matrix) matrix {
    res := f
    for ; n > 0; n /= 2 {
        if n%2 > 0 {
            res = a.mul(res)
        }
        a = a.mul(a)
    }
    return res
}

func numTilings(n int) int {
    if n == 1 {
        return 1
    }
    f2 := matrix{{2}, {1}, {1}}
    m := matrix{
        {2, 0, 1},
        {1, 0, 0},
        {0, 1, 0},
    };
    fn := m.powMul(n-2, f2)
    return fn[0][0]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(D^3\log n)$。其中 $D=3$。
- 空间复杂度：$\mathcal{O}(D^2)$。

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
