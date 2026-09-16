# 509. 斐波那契数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/fibonacci-number/
- 题目 slug：`fibonacci-number`
- 来源专题：动态规划
- 来源分类路径：十一、优化 DP / §11.6 矩阵快速幂优化 DP
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/fibonacci-number/solutions/3811875/san-chong-xie-fa-shu-zu-di-tui-kong-jian-3lwj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种写法：数组递推/空间优化/矩阵快速幂（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/fibonacci-number/solutions/3811875/san-chong-xie-fa-shu-zu-di-tui-kong-jian-3lwj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`san-chong-xie-fa-shu-zu-di-tui-kong-jian-3lwj`
- topic id：`3811875`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、递推

题目已经把递推式告诉我们了：

$$
f[i] = f[i-1] + f[i-2]
$$

初始值 $f[0]=0,\ f[1]=1$。

答案为 $f[n]$。

为避免 $n=0$ 时代码初始化 $f[1]$ 下标越界，可以创建大小为 $n+2$ 的数组。

```py [sol-Python3]
class Solution:
    def fib(self, n: int) -> int:
        f = [0] * (n + 2)
        f[1] = 1
        for i in range(2, n + 1):
            f[i] = f[i - 1] + f[i - 2]
        return f[n]
```

```java [sol-Java]
class Solution {
    public int fib(int n) {
        int[] f = new int[n + 2];
        f[1] = 1;
        for (int i = 2; i <= n; i++) {
            f[i] = f[i - 1] + f[i - 2];
        }
        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int fib(int n) {
        vector<int> f(n + 2);
        f[1] = 1;
        for (int i = 2; i <= n; i++) {
            f[i] = f[i - 1] + f[i - 2];
        }
        return f[n];
    }
};
```

```go [sol-Go]
func fib(n int) int {
    f := make([]int, n+2)
    f[1] = 1
    for i := 2; i <= n; i++ {
        f[i] = f[i-1] + f[i-2]
    }
    return f[n]
}
```

```js [sol-JavaScript]
var fib = function(n) {
    const f = Array(n + 2).fill(0);
    f[1] = 1;
    for (let i = 2; i <= n; i++) {
        f[i] = f[i - 1] + f[i - 2];
    }
    return f[n];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn fib(n: i32) -> i32 {
        let n = n as usize;
        let mut f = vec![0; n + 2];
        f[1] = 1;
        for i in 2..=n {
            f[i] = f[i - 1] + f[i - 2];
        }
        f[n]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 二、空间优化

观察状态转移方程，发现一旦算出 $f[i]$，那么 $f[i-2]$ 及其左边的状态就永远不会用到了。

这意味着每次循环，只需要知道「上一个状态」和「上上一个状态」的 $f$ 值是多少，分别记作 $f_1$ 和 $f_0$。它俩的初始值均为 $1$，对应着 $f[1]$ 和 $f[0]$。

每次循环，计算出新的状态 $\textit{newF} = f_1 + f_0$，那么对于下一轮循环来说：

- 「上上一个状态」就是 $f_1$，更新 $f_0 = f_1$。
- 「上一个状态」就是 $\textit{newF}$，更新 $f_1 = \textit{newF}$。

最后答案为 $f_1$，因为最后一轮循环算出的 $\textit{newF}$ 赋给了 $f_1$。

```py [sol-Python3]
class Solution:
    def fib(self, n: int) -> int:
        if n == 0:
            return 0
        f0, f1 = 0, 1
        for _ in range(2, n + 1):
            f0, f1 = f1, f1 + f0
        return f1
```

```java [sol-Java]
class Solution {
    public int fib(int n) {
        if (n == 0) {
            return 0;
        }
        int f0 = 0;
        int f1 = 1;
        for (int i = 2; i <= n; i++) {
            int newF = f1 + f0;
            f0 = f1;
            f1 = newF;
        }
        return f1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int fib(int n) {
        if (n == 0) {
            return 0;
        }
        int f0 = 0, f1 = 1;
        for (int i = 2; i <= n; i++) {
            int new_f = f1 + f0;
            f0 = f1;
            f1 = new_f;
        }
        return f1;
    }
};
```

```go [sol-Go]
func fib(n int) int {
    if n == 0 {
        return 0
    }
    f0, f1 := 0, 1
    for i := 2; i <= n; i++ {
        f0, f1 = f1, f1+f0
    }
    return f1
}
```

```js [sol-JavaScript]
var fib = function(n) {
    if (n === 0) {
        return 0;
    }
    let f0 = 0, f1 = 1;
    for (let i = 2; i <= n; i++) {
        let newF = f1 + f0;
        f0 = f1;
        f1 = newF;
    }
    return f1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn fib(n: i32) -> i32 {
        if n == 0 {
            return 0;
        }
        let mut f0 = 0;
        let mut f1 = 1;
        for i in 2..=n {
            (f0, f1) = (f1, f1 + f0);
        }
        f1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 三、矩阵快速幂优化

把状态转移方程用矩阵乘法表示，即

$$
\begin{bmatrix}
f[i] \\
f[i-1] \\
\end{bmatrix}
= \begin{bmatrix}
1 & 1 \\
1 & 0 \\
\end{bmatrix}
\begin{bmatrix}
f[i-1] \\
f[i-2] \\
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
&= M^{n-1}\times F[1] \\
\end{aligned}
$$

其中 $M^{n-1}$ 可以用**快速幂**计算，原理请看[【图解】一张图秒懂快速幂](https://leetcode.cn/problems/powx-n/solution/tu-jie-yi-zhang-tu-miao-dong-kuai-su-mi-ykp3i/)。

初始值

$$
F[1] = \begin{bmatrix}
f[1] \\
f[0] \\
\end{bmatrix}
= \begin{bmatrix}
1 \\
0 \\
\end{bmatrix}
$$

答案为 $f[n]$，即 $F[n]$ 的第一项。

```py [sol-Python3]
# a @ b，其中 @ 是矩阵乘法
def mul(a: List[List[int]], b: List[List[int]]) -> List[List[int]]:
    return [[sum(x * y for x, y in zip(row, col)) for col in zip(*b)]
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
    def fib(self, n: int) -> int:
        if n == 0:
            return 0
        m = [[1, 1], [1, 0]]
        f1 = [[1], [0]]
        fn = pow_mul(m, n - 1, f1)
        return fn[0][0]
```

```py [sol-NumPy]
import numpy as np

class Solution:
    def fib(self, n: int) -> int:
        if n == 0:
            return 0
        m = np.array([[1, 1], [1, 0]], dtype=object)
        f1 = np.array([1, 0], dtype=object)
        fn = np.linalg.matrix_power(m, n - 1) @ f1
        return fn[0]
```

```java [sol-Java]
class Solution {
    public int fib(int n) {
        if (n == 0) {
            return 0;
        }
        int[][] m = {
            {1, 1},
            {1, 0},
        };
        int[][] f1 = {{1}, {0}};
        int[][] fn = powMul(m, n - 1, f1);
        return fn[0][0];
    }

    // a^n * f0
    private int[][] powMul(int[][] a, int n, int[][] f0) {
        int[][] res = f0;
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
    private int[][] mul(int[][] a, int[][] b) {
        int[][] c = new int[a.length][b[0].length];
        for (int i = 0; i < a.length; i++) {
            for (int k = 0; k < a[i].length; k++) {
                if (a[i][k] == 0) {
                    continue;
                }
                for (int j = 0; j < b[k].length; j++) {
                    c[i][j] += a[i][k] * b[k][j];
                }
            }
        }
        return c;
    }
}
```

```cpp [sol-C++]
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
                c[i][j] += a[i][k] * b[k][j];
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
    int fib(int n) {
        if (n == 0) {
            return 0;
        }
        matrix m = {
            {1, 1},
            {1, 0},
        };
        matrix f1 = {{1}, {0}};
        matrix fn = pow_mul(m, n - 1, f1);
        return fn[0][0];
    }
};
```

```go [sol-Go]
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
                c[i][j] += x * y
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

func fib(n int) int {
    if n == 0 {
        return 0
    }
    m := matrix{
        {1, 1},
        {1, 0},
    }
    f1 := matrix{{1}, {0}}
    fn := m.powMul(n-1, f1)
    return fn[0][0]
}
```

```js [sol-JavaScript]
// 返回矩阵 a 和矩阵 b 相乘的结果
function mul(a, b) {
    const n = a.length, m = b[0].length;
    const c = Array.from({length: n}, () => Array(m).fill(0));
    for (let i = 0; i < n; i++) {
        for (let k = 0; k < a[i].length; k++) {
            if (a[i][k] === 0) {
                continue;
            }
            for (let j = 0; j < m; j++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return c;
}

// a^n * f0
function powMul(a, n, f0) {
    let res = f0;
    while (n > 0) {
        if (n % 2 !== 0) {
            res = mul(a, res);
        }
        a = mul(a, a);
        n = Math.floor(n / 2);
    }
    return res;
}

var fib = function(n) {
    if (n === 0) {
        return 0;
    }
    const m = [
        [1, 1],
        [1, 0],
    ];
    const f1 = [[1], [0]];
    const fn = powMul(m, n - 1, f1);
    return fn[0][0];
};
```

```rust [sol-Rust]
// 返回矩阵 a 和矩阵 b 相乘的结果
fn mul(a: &Vec<Vec<i64>>, b: &Vec<Vec<i64>>) -> Vec<Vec<i64>> {
    let n = a.len();
    let m = b[0].len();
    let mut c = vec![vec![0; m]; n];
    for i in 0..n {
        for k in 0..a[i].len() {
            if a[i][k] == 0 {
                continue;
            }
            for j in 0..m {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    c
}

// a^n * f0
fn pow_mul(mut a: Vec<Vec<i64>>, mut n: i32, f0: Vec<Vec<i64>>) -> Vec<Vec<i64>> {
    let mut res = f0;
    while n > 0 {
        if n & 1 > 0 {
            res = mul(&a, &res);
        }
        a = mul(&a, &a);
        n >>= 1;
    }
    res
}

impl Solution {
    pub fn fib(n: i32) -> i32 {
        if n == 0 {
            return 0;
        }
        let m = vec![
            vec![1, 1],
            vec![1, 0],
        ];
        let f1 = vec![vec![1], vec![0]];
        let fn_ = pow_mul(m, n - 1, f1);
        fn_[0][0] as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$。
- 空间复杂度：$\mathcal{O}(1)$。

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
