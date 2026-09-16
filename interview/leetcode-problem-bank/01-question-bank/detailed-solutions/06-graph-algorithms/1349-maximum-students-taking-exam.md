# 1349. 参加考试的最大学生数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-students-taking-exam/
- 题目 slug：`maximum-students-taking-exam`
- 来源专题：图论算法
- 来源分类路径：八、网络流
- 难度分：2386
- 外部题解来源：https://leetcode.cn/problems/maximum-students-taking-exam/solutions/2580043/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-9y5k/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考动态规划：从记忆化搜索到递推（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-students-taking-exam/solutions/2580043/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-9y5k/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-9y5k`
- topic id：`2580043`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识：动态规划入门

请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，制作不易，欢迎点赞！

## 前置知识：从集合论到位运算

本文会用到如下位运算技巧：

1. 集合求并
2. 集合求差
3. lowbit
4. 枚举子集

这些技巧全部收录在 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)如果你没看懂代码中的位运算，可以查阅这篇文章，和本文的图解。

## 一、启发思考：寻找子问题

![LC1349-1-c.png](https://pic.leetcode.cn/1703494396-tdKdhi-LC1349-1-c.png){:width=400}

对于示例 1，我们要解决的问题是：在第三排的座位状态为 $\texttt{#.##.#}$ 的情况下，前三排最多可以坐多少个学生。

分类讨论：

- 如果第三排没有学生，那么需要解决的问题为：在第二排的座位状态为 $\texttt{.####.}$ 的情况下，前两排最多可以坐多少个学生。
- 如果第三排的左边有学生，那么需要解决的问题为：在第二排的座位状态为 $\texttt{#####.}$ 的情况下，前两排最多可以坐多少个学生。
- 如果第三排的右边有学生，那么需要解决的问题为：在第二排的座位状态为 $\texttt{.#####}$ 的情况下，前两排最多可以坐多少个学生。
- 如果第三排的左右都有学生，那么需要解决的问题为：在第二排的座位状态为 $\texttt{######}$ 的情况下，前两排最多可以坐多少个学生。

由于这四种情况都会把原问题变成一个**和原问题相似的、规模更小的子问题**，所以可以用**递归**解决。

> 注 1：从下往上思考，主要是为了方便把递归翻译成递推。从上往下思考也是可以的。
> 
> 注 2：动态规划有「选或不选」和「枚举选哪个」两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「枚举选哪个」。

## 二、递归怎么写：状态定义与状态转移方程

因为要解决的问题都形如「在第 x 排的座位状态为 xxx 的情况下，前 x 排最多可以坐多少个学生」，所以定义 $\textit{dfs}(i,j)$ 表示在**第** $i$ 排的座位状态为 $j$ 的情况下，**前** $i$ 排的最大学生数。

$j$ 是什么类型？如果用字符串就太麻烦了。我们可以用二进制数来表示座位状态，即二进制从低到高的第 $k$ 位为 $1$，就表示第 $i$ 排从左到右的第 $k$ 把椅子是可用的，为 $0$ 就表示这把椅子不可用。

例如 $\texttt{.#..}$ 可以用二进制数 $1101$ 表示，注意二进制数是**从右往左**读的。

同样地，也可以用二进制数来表示这一排的哪些椅子坐了学生，即二进制从低到高的第 $k$ 位为 $1$，就表示从左到右的第 $k$ 把椅子坐了学生，为 $0$ 就表示这把椅子没有坐学生。

示例 1 的第三排对应二进制数 $010010$，分类讨论：

- 如果第三排没有学生（$000000$），那么需要解决的问题为：在第二排的座位状态为 $100001$ 的情况下，前两排最多可以坐多少个学生。
- 如果第三排的左边有学生（$000010$），那么需要解决的问题为：在第二排的座位状态为 $100000$ 的情况下，前两排最多可以坐多少个学生。再次强调，二进制数是**从右往左**读的。
- 如果第三排的右边有学生（$010000$），那么需要解决的问题为：在第二排的座位状态为 $000001$ 的情况下，前两排最多可以坐多少个学生。
- 如果第三排的左右都有学生（$010010$），那么需要解决的问题为：在第二排的座位状态为 $000000$ 的情况下，前两排最多可以坐多少个学生。

把前两排的最大学生个数加上第三排的学生个数，取最大值，就得到了 $\textit{dfs}(i,j)$，即

$$
\textit{dfs}(i,j) = \max_{s\subseteq j}\{\textit{dfs}(i-1,t) + |s|\}
$$

其中：

- $s$ 表示坐在第 $i$ 排的学生，是 $j$ 的子集，且 $s$ 的二进制数中不能有两个 $1$ 是相邻的。判断二进制数中没有两个 $1$ 相邻，可以用 `(s & (s >> 1)) == 0`，如果有两个 $1$ 相邻，那么左边的 $1$ 右移一位后，与右边的 $1$ 的按位与（AND）必然是 $1$。注：写成 `(s & (s << 1)) == 0` 也是可以的。
- $|s|$ 表示坐在第 $i$ 排的学生个数，即 $s$ 的二进制数中 $1$ 的个数，可以直接调用库函数解决。
- $t$ 表示第 $i-1$ 排的可用座位，如何计算请看下图。

![LC1349-2-c.png](https://pic.leetcode.cn/1703497127-DjAzVW-LC1349-2-c.png){:width=450}

当第一排的可用座位为 $j$ 时，最多可以坐多少个学生？请看下图。

![LC1349-3-c.png](https://pic.leetcode.cn/1703496907-NXhkUL-LC1349-3-c.png){:width=450}

递归入口：$\textit{dfs}(m-1, a[m-1])$，也就是答案。其中 $a[i]$ 表示第 $i$ 排能坐人的位置。

```py [sol-Python3]
# 会超时的递归代码
class Solution:
    def maxStudents(self, seats: List[List[str]]) -> int:
        a = [sum((c == '.') << j for j, c in enumerate(s)) for s in seats]
        def dfs(i: int, j: int) -> int:
            if i == 0:
                lb = j & -j
                return dfs(i, j & ~(lb * 3)) + 1 if j else 0
            res = dfs(i - 1, a[i - 1])  # 第 i 排空着
            s = j
            while s:  # 枚举 j 的子集 s
                if (s & (s >> 1)) == 0:  # s 没有连续的 1
                    t = a[i - 1] & ~(s << 1 | s >> 1)  # 去掉不能坐人的位置
                    res = max(res, dfs(i - 1, t) + s.bit_count())
                s = (s - 1) & j
            return res
        return dfs(len(seats) - 1, a[-1])
```

```java [sol-Java]
// 会超时的递归代码
class Solution {
    public int maxStudents(char[][] seats) {
        int m = seats.length;
        int n = seats[0].length;
        int[] a = new int[m]; // a[i] 是第 i 排可用椅子的下标集合
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (seats[i][j] == '.') {
                    a[i] |= 1 << j;
                }
            }
        }
        return dfs(m - 1, a[m - 1], a);
    }

    private int dfs(int i, int j, int[] a) {
        if (i == 0) {
            if (j == 0) { // 递归边界
                return 0;
            }
            int lb = j & -j;
            return dfs(i, j & ~(lb * 3), a) + 1;
        }
        int res = dfs(i - 1, a[i - 1], a); // 第 i 排空着
        for (int s = j; s > 0; s = (s - 1) & j) { // 枚举 j 的子集 s
            if ((s & (s >> 1)) == 0) { // s 没有连续的 1
                int t = a[i - 1] & ~(s << 1 | s >> 1); // 去掉不能坐人的位置
                res = Math.max(res, dfs(i - 1, t, a) + Integer.bitCount(s));
            }
        }
        return res; // 记忆化
    }
}
```

```cpp [sol-C++]
// 会超时的递归代码
class Solution {
public:
    int maxStudents(vector<vector<char>> &seats) {
        int m = seats.size(), n = seats[0].size();
        vector<int> a(m); // a[i] 是第 i 排可用椅子的下标集合
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (seats[i][j] == '.') {
                    a[i] |= 1 << j;
                }
            }
        }

        function<int(int, int)> dfs = [&](int i, int j) -> int {
            if (i == 0) {
                if (j == 0) {
                    return 0;
                }
                int lb = j & -j;
                return dfs(i, j & ~(lb * 3)) + 1;
            }
            int res = dfs(i - 1, a[i - 1]); // 第 i 排空着
            for (int s = j; s; s = (s - 1) & j) { // 枚举 j 的子集 s
                if ((s & (s >> 1)) == 0) { // s 没有连续的 1
                    int t = a[i - 1] & ~(s << 1 | s >> 1); // 去掉不能坐人的位置
                    res = max(res, dfs(i - 1, t) + __builtin_popcount(s));
                }
            }
            return res;
        };
        return dfs(m - 1, a[m - 1]);
    }
};
```

```go [sol-Go]
// 会超时的递归代码
func maxStudents(seats [][]byte) int {
    m := len(seats)
    a := make([]int, m) // a[i] 是第 i 排可用椅子的下标集合
    for i, s := range seats {
        for j, c := range s {
            if c == '.' {
                a[i] |= 1 << j
            }
        }
    }

    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i == 0 {
            if j == 0 {
                return 0
            }
            lb := j & -j
            return dfs(i, j&^(lb*3)) + 1
        }
        res := dfs(i-1, a[i-1]) // 第 i 排空着
        for s := j; s > 0; s = (s - 1) & j { // 枚举 j 的子集 s
            if s&(s>>1) == 0 { // s 没有连续的 1
                t := a[i-1] & ^(s<<1 | s>>1) // 去掉不能坐人的位置
                res = max(res, dfs(i-1, t)+bits.OnesCount(uint(s)))
            }
        }
        return res
    }
    return dfs(m-1, a[m-1])
}
```

```js [sol-JavaScript]
// 会超时的递归代码
var maxStudents = function (seats) {
    const m = seats.length, n = seats[0].length;
    const a = Array(m).fill(0); // a[i] 是第 i 排可用椅子的下标集合
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (seats[i][j] === '.') {
                a[i] |= 1 << j;
            }
        }
    }

    function dfs(i, j) {
        if (i === 0) {
            if (j === 0) {
                return 0;
            }
            const lb = j & -j;
            return dfs(i, j & ~(lb * 3)) + 1;
        }
        let res = dfs(i - 1, a[i - 1]); // 第 i 排空着
        for (let s = j; s > 0; s = (s - 1) & j) { // 枚举 j 的子集 s
            if ((s & (s >> 1)) === 0) { // s 没有连续的 1
                const t = a[i - 1] & ~(s << 1 | s >> 1); // 去掉不能坐人的位置
                res = Math.max(res, dfs(i - 1, t) + bitCount32(s));
            }
        }
        return res;
    }
    return dfs(m - 1, a[m - 1]);
};

function bitCount32(n) {
    n = n - ((n >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    return ((n + (n >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}
```

```rust [sol-Rust]
// 会超时的递归代码
impl Solution {
    pub fn max_students(seats: Vec<Vec<char>>) -> i32 {
        let m = seats.len();
        let n = seats[0].len();
        let mut a = vec![0; m]; // a[i] 是第 i 排可用椅子的下标集合
        for (i, row) in seats.iter().enumerate() {
            for (j, &c) in row.iter().enumerate() {
                if c == '.' {
                    a[i] |= 1 << j;
                }
            }
        }

        fn dfs(i: usize, j: usize, a: &Vec<usize>) -> i32 {
            if i == 0 {
                if j == 0 {
                    return 0;
                }
                let lb = (j as i32 & -(j as i32)) as usize;
                return dfs(i, j & !(lb * 3), a) + 1;
            }
            let mut res = dfs(i - 1, a[i - 1], a); // 第 i 排空着
            let mut s = j;
            while s > 0 { // 枚举 j 的子集 s
                if (s & (s >> 1)) == 0 { // s 没有连续的 1
                    let t = a[i - 1] & !(s << 1 | s >> 1); // 去掉不能坐人的位置
                    res = res.max(dfs(i - 1, t, a) + s.count_ones() as i32);
                }
                s = (s - 1) & j;
            }
            res
        }
        dfs(m - 1, a[m - 1], &a)
    }
}
```

## 三、递归 + 记录返回值 = 记忆化搜索

上面的做法太慢了，怎么优化呢？

由于整个递归中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

```py [sol-Python3]
class Solution:
    def maxStudents(self, seats: List[List[str]]) -> int:
        a = [sum((c == '.') << j for j, c in enumerate(s)) for s in seats]
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int, j: int) -> int:
            if i == 0:
                lb = j & -j
                return dfs(i, j & ~(lb * 3)) + 1 if j else 0
            res = dfs(i - 1, a[i - 1])  # 第 i 排空着
            s = j
            while s:  # 枚举 j 的子集 s
                if (s & (s >> 1)) == 0:  # s 没有连续的 1
                    t = a[i - 1] & ~(s << 1 | s >> 1)
                    res = max(res, dfs(i - 1, t) + s.bit_count())
                s = (s - 1) & j
            return res
        return dfs(len(seats) - 1, a[-1])
```

```java [sol-Java]
class Solution {
    public int maxStudents(char[][] seats) {
        int m = seats.length;
        int n = seats[0].length;
        int[] a = new int[m]; // a[i] 是第 i 排可用椅子的下标集合
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (seats[i][j] == '.') {
                    a[i] |= 1 << j;
                }
            }
        }

        int[][] memo = new int[m][1 << n];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return dfs(m - 1, a[m - 1], memo, a);
    }

    private int dfs(int i, int j, int[][] memo, int[] a) {
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }
        if (i == 0) {
            if (j == 0) { // 递归边界
                return 0;
            }
            int lb = j & -j;
            return memo[i][j] = dfs(i, j & ~(lb * 3), memo, a) + 1; // 记忆化
        }
        int res = dfs(i - 1, a[i - 1], memo, a); // 第 i 排空着
        for (int s = j; s > 0; s = (s - 1) & j) { // 枚举 j 的子集 s
            if ((s & (s >> 1)) == 0) { // s 没有连续的 1
                int t = a[i - 1] & ~(s << 1 | s >> 1); // 去掉不能坐人的位置
                res = Math.max(res, dfs(i - 1, t, memo, a) + Integer.bitCount(s));
            }
        }
        return memo[i][j] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxStudents(vector<vector<char>> &seats) {
        int m = seats.size(), n = seats[0].size();
        vector<int> a(m); // a[i] 是第 i 排可用椅子的下标集合
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (seats[i][j] == '.') {
                    a[i] |= 1 << j;
                }
            }
        }

        vector<vector<int>> memo(m, vector<int>(1 << n, -1)); // -1 表示没有计算过
        function<int(int, int)> dfs = [&](int i, int j) -> int {
            int &res = memo[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            if (i == 0) {
                if (j == 0) {
                    return res = 0;
                }
                int lb = j & -j;
                return res = dfs(i, j & ~(lb * 3)) + 1;
            }
            res = dfs(i - 1, a[i - 1]); // 第 i 排空着
            for (int s = j; s; s = (s - 1) & j) { // 枚举 j 的子集 s
                if ((s & (s >> 1)) == 0) { // s 没有连续的 1
                    int t = a[i - 1] & ~(s << 1 | s >> 1); // 去掉不能坐人的位置
                    res = max(res, dfs(i - 1, t) + __builtin_popcount(s));
                }
            }
            return res;
        };
        return dfs(m - 1, a[m - 1]);
    }
};
```

```go [sol-Go]
func maxStudents(seats [][]byte) int {
    m, n := len(seats), len(seats[0])
    a := make([]int, m) // a[i] 是第 i 排可用椅子的下标集合
    for i, s := range seats {
        for j, c := range s {
            if c == '.' {
                a[i] |= 1 << j
            }
        }
    }

    memo := make([][]int, m)
    for i := range memo {
        memo[i] = make([]int, 1<<n)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) (res int) {
        p := &memo[i][j]
        if *p != -1 { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if i == 0 {
            if j == 0 {
                return 0
            }
            lb := j & -j
            return dfs(i, j&^(lb*3)) + 1
        }
        res = dfs(i-1, a[i-1]) // 第 i 排空着
        for s := j; s > 0; s = (s - 1) & j { // 枚举 j 的子集 s
            if s&(s>>1) == 0 { // s 没有连续的 1
                t := a[i-1] & ^(s<<1 | s>>1) // 去掉不能坐人的位置
                res = max(res, dfs(i-1, t)+bits.OnesCount(uint(s)))
            }
        }
        return
    }
    return dfs(m-1, a[m-1])
}
```

```js [sol-JavaScript]
var maxStudents = function (seats) {
    const m = seats.length, n = seats[0].length;
    const a = Array(m).fill(0); // a[i] 是第 i 排可用椅子的下标集合
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (seats[i][j] === '.') {
                a[i] |= 1 << j;
            }
        }
    }

    const memo = Array(m).fill(null).map(() => Array(1 << n).fill(-1));
    function dfs(i, j) {
        if (memo[i][j] !== -1) { // 之前计算过
            return memo[i][j];
        }
        if (i === 0) {
            if (j === 0) {
                return 0;
            }
            const lb = j & -j;
            return memo[i][j] = dfs(i, j & ~(lb * 3)) + 1;
        }
        let res = dfs(i - 1, a[i - 1]); // 第 i 排空着
        for (let s = j; s > 0; s = (s - 1) & j) { // 枚举 j 的子集 s
            if ((s & (s >> 1)) === 0) { // s 没有连续的 1
                const t = a[i - 1] & ~(s << 1 | s >> 1); // 去掉不能坐人的位置
                res = Math.max(res, dfs(i - 1, t) + bitCount32(s));
            }
        }
        return memo[i][j] = res; // 记忆化
    }
    return dfs(m - 1, a[m - 1]);
};

function bitCount32(n) {
    n = n - ((n >> 1) & 0x55555555);
    n = (n & 0x33333333) + ((n >> 2) & 0x33333333);
    return ((n + (n >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}
```

```rust [sol-Rust]
impl Solution {
    pub fn max_students(seats: Vec<Vec<char>>) -> i32 {
        let m = seats.len();
        let n = seats[0].len();
        let mut a = vec![0; m]; // a[i] 是第 i 排可用椅子的下标集合
        for (i, row) in seats.iter().enumerate() {
            for (j, &c) in row.iter().enumerate() {
                if c == '.' {
                    a[i] |= 1 << j;
                }
            }
        }

        let mut memo = vec![vec![-1; 1 << n]; m]; // -1 表示没有计算过
        fn dfs(i: usize, j: usize, memo: &mut Vec<Vec<i32>>, a: &Vec<usize>) -> i32 {
            if memo[i][j] != -1 { // 之前计算过
                return memo[i][j];
            }
            if i == 0 {
                if j == 0 {
                    return 0;
                }
                let lb = (j as i32 & -(j as i32)) as usize;
                memo[i][j] = dfs(i, j & !(lb * 3), memo, a) + 1;
                return memo[i][j];
            }
            let mut res = dfs(i - 1, a[i - 1], memo, a); // 第 i 排空着
            let mut s = j;
            while s > 0 { // 枚举 j 的子集 s
                if (s & (s >> 1)) == 0 { // s 没有连续的 1
                    let t = a[i - 1] & !(s << 1 | s >> 1); // 去掉不能坐人的位置
                    res = res.max(dfs(i - 1, t, memo, a) + s.count_ones() as i32);
                }
                s = (s - 1) & j;
            }
            memo[i][j] = res; // 记忆化
            res
        }
        dfs(m - 1, a[m - 1], &mut memo, &a)
    }
}
```

#### 复杂度分析之一（粗略分析）

- 时间复杂度：$\mathcal{O}(m\cdot 3^n)$。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(m\cdot 2^n)$，单个状态的计算时间为 $\mathcal{O}(2^n)$。但是，对于每一排，我们至多枚举了集合 $\{0,1,2,\cdots,n-1\}$ 的每个子集的子集，由于元素个数为 $k$ 的集合有 $\dbinom n k$ 个，其子集有 $2^k$ 个，根据二项式定理，$\sum\limits_{k=0}^n \dbinom n k 2^k = (2+1)^n = 3^n$，所以动态规划的时间复杂度实际上是 $\mathcal{O}(m\cdot 3^n)$。
- 空间复杂度：$\mathcal{O}(m\cdot 2^n)$，即状态个数。

#### 复杂度分析之二（稍紧一点）

- 时间复杂度：$\mathcal{O}(m\cdot 2.732^n)$。我们计算的 $t$ 是与 $s$ 相关的，为方便计算 $t$ 的子集个数，用没有连续 $1$ 的二进制数的补集的子集来估计（这比实际的子集个数大）。这个问题可以用动态规划解决，即考虑 $s$ 的最低位填 $0$ 还是填 $1$。如果填 $0$，那么补集的最低位必然是 $1$，问题转换成 $n-1$ 位的子问题；如果填 $1$，那么次低位必然填 $0$，补集的次低位必然是 $1$，问题转换成 $n-2$ 位的子问题。由于多一个 $1$，子集的大小就会多一倍，所以得到递推式 $f(n) = 2(f(n-1) + f(n-2))$，初始值 $f(0)=1,f(1)=3$。用特征方程法解出其通项公式，得到 $f(n) = \mathcal{O}((1+\sqrt{3})^n)$，近似为 $\mathcal{O}(2.732^n)$。
- 空间复杂度：$\mathcal{O}(m\cdot 1.618^n)$。我们计算的 $t$ 是与 $s$ 相关的，也就是没有连续 $1$ 的二进制数的个数。计算方式和上面类似，其递推式为 $f(n) = f(n-1) + f(n-2)$，初始值 $f(0)=1,f(1)=2$。由斐波那契数列通项公式可知，$f(n) = \mathcal{O}\left(\left(\dfrac{1+\sqrt{5}}{2}\right)^n\right)$，近似为 $\mathcal{O}(1.618^n)$。对于 Python 来说，`@cache` 至多保存了 $\mathcal{O}(m\cdot 1.618^n)$ 个状态。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示在**第** $i$ 排的座位状态为 $j$ 的情况下，**前** $i$ 排的最大学生数。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i][j] = \max_{s\subseteq j}\{f[i-1][t] + |s|\}
$$

相当于之前是用递归去计算每个状态，现在是**枚举**并计算每个状态。

在枚举的时候，注意 $j$ 必须是 $a[i]$ 的子集。这在记忆化搜索中是不需要考虑的，但是在递推中是需要考虑的。

答案为 $f[m-1][a[m-1]]$，翻译自递归入口 $\textit{dfs}(m-1,a[m-1])$。

代码实现时的小优化：无需调用库函数计算 $s$ 二进制中 $1$ 的个数，这是因为 $s$ 中没有连续的 $1$，那么按照 $f$ 数组的定义，这就是 $f[0][s]$。

```py [sol-Python3]
class Solution:
    def maxStudents(self, seats: List[List[str]]) -> int:
        m, n = len(seats), len(seats[0])
        # a[i] 是第 i 排可用椅子的下标集合
        a = [sum((c == '.') << j for j, c in enumerate(s)) for s in seats]
        f = [[0] * (1 << n) for _ in range(m)]
        for j in range(1, 1 << n):
            lb = j & -j
            f[0][j] = f[0][j & ~(lb * 3)] + 1
        for i in range(1, m):
            j = a[i]
            while j:  # 枚举 a[i] 的子集 j
                f[i][j] = f[i - 1][a[i - 1]]  # 第 i 排空着
                s = j
                while s:  # 枚举 j 的子集 s
                    if (s & (s >> 1)) == 0:  # s 没有连续的 1
                        t = a[i - 1] & ~(s << 1 | s >> 1)  # 去掉不能坐人的位置
                        f[i][j] = max(f[i][j], f[i - 1][t] + f[0][s])
                    s = (s - 1) & j
                j = (j - 1) & a[i]
            f[i][0] = f[i - 1][a[i - 1]]
        return f[-1][a[-1]]
```

```java [sol-Java]
class Solution {
    public int maxStudents(char[][] seats) {
        int m = seats.length;
        int n = seats[0].length;
        int[] a = new int[m]; // a[i] 是第 i 排可用椅子的下标集合
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (seats[i][j] == '.') {
                    a[i] |= 1 << j;
                }
            }
        }

        int[][] f = new int[m][1 << n];
        for (int j = 1; j < (1 << n); j++) {
            int lb = j & -j;
            f[0][j] = f[0][j & ~(lb * 3)] + 1;
        }
        for (int i = 1; i < m; i++) {
            for (int j = a[i]; j > 0; j = (j - 1) & a[i]) { // 枚举 a[i] 的子集 j
                f[i][j] = f[i - 1][a[i - 1]]; // 第 i 排空着
                for (int s = j; s > 0; s = (s - 1) & j) { // 枚举 j 的子集 s
                    if ((s & (s >> 1)) == 0) { // s 没有连续的 1
                        int t = a[i - 1] & ~(s << 1 | s >> 1); // 去掉不能坐人的位置
                        f[i][j] = Math.max(f[i][j], f[i - 1][t] + f[0][s]);
                    }
                }
            }
            f[i][0] = f[i - 1][a[i - 1]];
        }
        return f[m - 1][a[m - 1]];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxStudents(vector<vector<char>> &seats) {
        int m = seats.size(), n = seats[0].size();
        vector<int> a(m); // a[i] 是第 i 排可用椅子的下标集合
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (seats[i][j] == '.') {
                    a[i] |= 1 << j;
                }
            }
        }

        vector<vector<int>> f(m, vector<int>(1 << n));
        for (int j = 1; j < (1 << n); j++) {
            int lb = j & -j;
            f[0][j] = f[0][j & ~(lb * 3)] + 1;
        }
        for (int i = 1; i < m; i++) {
            for (int j = a[i]; j; j = (j - 1) & a[i]) { // 枚举 a[i] 的子集 j
                f[i][j] = f[i - 1][a[i - 1]]; // 第 i 排空着
                for (int s = j; s; s = (s - 1) & j) { // 枚举 j 的子集 s
                    if ((s & (s >> 1)) == 0) { // s 没有连续的 1
                        int t = a[i - 1] & ~(s << 1 | s >> 1); // 去掉不能坐人的位置
                        f[i][j] = max(f[i][j], f[i - 1][t] + f[0][s]);
                    }
                }
            }
            f[i][0] = f[i - 1][a[i - 1]];
        }
        return f[m - 1][a[m - 1]];
    }
};
```

```go [sol-Go]
func maxStudents(seats [][]byte) int {
    m, n := len(seats), len(seats[0])
    a := make([]int, m) // a[i] 是第 i 排可用椅子的下标集合
    for i, s := range seats {
        for j, c := range s {
            if c == '.' {
                a[i] |= 1 << j
            }
        }
    }

    f := make([][]int, m)
    for i := range f {
        f[i] = make([]int, 1<<n)
    }
    for j := 1; j < 1<<n; j++ {
        lb := j & -j
        f[0][j] = f[0][j&^(lb*3)] + 1
    }
    for i := 1; i < m; i++ {
        for j := a[i]; j > 0; j = (j - 1) & a[i] { // 枚举 a[i] 的子集 j
            f[i][j] = f[i-1][a[i-1]] // 第 i 排空着
            for s := j; s > 0; s = (s - 1) & j { // 枚举 j 的子集 s
                if s&(s>>1) == 0 { // s 没有连续的 1
                    t := a[i-1] &^ (s<<1 | s>>1) // 去掉不能坐人的位置
                    f[i][j] = max(f[i][j], f[i-1][t]+f[0][s])
                }
            }
        }
        f[i][0] = f[i-1][a[i-1]]
    }
    return f[m-1][a[m-1]]
}
```

```js [sol-JavaScript]
var maxStudents = function (seats) {
    const m = seats.length, n = seats[0].length;
    const a = Array(m).fill(0); // a[i] 是第 i 排可用椅子的下标集合
    for (let i = 0; i < m; i++) {
        for (let j = 0; j < n; j++) {
            if (seats[i][j] === '.') {
                a[i] |= 1 << j;
            }
        }
    }

    const f = Array(m).fill(null).map(() => Array(1 << n).fill(0));
    for (let j = 1; j < (1 << n); j++) {
        const lb = j & -j;
        f[0][j] = f[0][j & ~(lb * 3)] + 1;
    }
    for (let i = 1; i < m; i++) {
        for (let j = a[i]; j > 0; j = (j - 1) & a[i]) { // 枚举 a[i] 的子集 j
            f[i][j] = f[i - 1][a[i - 1]]; // 第 i 排空着
            for (let s = j; s > 0; s = (s - 1) & j) { // 枚举 j 的子集 s
                if ((s & (s >> 1)) === 0) { // s 没有连续的 1
                    const t = a[i - 1] & ~(s << 1 | s >> 1); // 去掉不能坐人的位置
                    f[i][j] = Math.max(f[i][j], f[i - 1][t] + f[0][s]);
                }
            }
        }
        f[i][0] = f[i - 1][a[i - 1]];
    }
    return f[m - 1][a[m - 1]];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_students(seats: Vec<Vec<char>>) -> i32 {
        let m = seats.len();
        let n = seats[0].len();
        let mut a = vec![0; m]; // a[i] 是第 i 排可用椅子的下标集合
        for (i, row) in seats.iter().enumerate() {
            for (j, &c) in row.iter().enumerate() {
                if c == '.' {
                    a[i] |= 1 << j;
                }
            }
        }

        let mut f = vec![vec![0; 1 << n]; m];
        for j in 1..(1 << n) {
            let lb = (j as i32 & -(j as i32)) as usize;
            f[0][j] = f[0][j & !(lb * 3)] + 1;
        }
        for i in 1..m {
            let mut j = a[i];
            while j > 0 { // 枚举 a[i] 的子集 j
                f[i][j] = f[i - 1][a[i - 1]]; // 第 i 排空着
                let mut s = j;
                while s > 0 { // 枚举 j 的子集 s
                    if (s & (s >> 1)) == 0 { // s 没有连续的 1
                        let t = a[i - 1] & !(s << 1 | s >> 1); // 去掉不能坐人的位置
                        f[i][j] = f[i][j].max(f[i - 1][t] + f[0][s]);
                    }
                    s = (s - 1) & j;
                }
                j = (j - 1) & a[i];
            }
            f[i][0] = f[i - 1][a[i - 1]];
        }
        f[m - 1][a[m - 1]]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\cdot 3^n)$。必须枚举所有子集的子集，所以同记忆化搜索的复杂度分析之一。对比记忆化搜索的复杂度分析之二，可以解释为什么记忆化搜索会比递推快一点。
- 空间复杂度：$\mathcal{O}(m\cdot 2^n)$。

## 附：用「选或不选」来枚举子集

在[【基础算法精讲 20】](https://www.bilibili.com/video/BV1ub411Q7sB/)中，我讲了求最长递增子序列的四种思路，前两种思路是「选或不选」和「枚举选哪个」，这两种思路是可以互相转换的。

本文前面讲的做法基于「枚举选哪个」，即枚举子集。事实上，枚举子集也可以用「选或不选」来做，这要如何实现呢？

前面讲过，在 $i=0$ 的时候，可以用 `j & ~(lb * 3)` 来表示对这一排的**单个座位**的「选」，这一技巧同样可以应用到 $i>0$ 的情况。

把 $j$ 当成是「这一排的剩余座位」，但只有 $j$ 的话，就丢失了「前面选了哪些座位」这一信息，所以要多加一个参数 $k$ 表示这一排前面选了哪些座位。

定义 $\textit{dfs}(i,j,k)$ 表示在**第** $i$ 排的剩余待选座位状态为 $j$，并且已选座位状态为 $k$ 的情况下，**前** $i$ 排的最大学生数。

如果还剩下需要选的座位，即 $j>0$，我们分类讨论：

- 不选，那么去掉最低的 $1$，即 `dfs(i, j, k) = dfs(i, j ^ lb, k)`。
- 选，那么去掉最低的 $1$，其左边相邻比特置 $0$，同时把最低的 $1$ 加到 $k$ 中，即 `dfs(i, j, k) = dfs(i, j & ~(lb * 3), k | lb) + 1`。

这两种情况取最大值。

如果没有需要选的座位，即 $j=0$，那么继续考虑前一排怎么选，从 $a[i-1]$ 中移除不能坐的位置，递归到 `dfs(i - 1, a[i - 1] & ~(k << 1 | k >> 1), 0)`。

递归边界：$\textit{dfs}(0,0,k)=0$。

递归入口：$\textit{dfs}(m-1,a[m-1],0)$，即答案。

> 注：由于状态较为离散，推荐用哈希表记录状态，用 Python 自带的 `@cache` 写是最舒服的。其它语言的写法请参考 Go 的实现。

```py [sol-Python3]
class Solution:
    def maxStudents(self, seats: List[List[str]]) -> int:
        a = [sum((c == '.') << j for j, c in enumerate(s)) for s in seats]
        @cache
        def dfs(i: int, j: int, k: int) -> int:
            if j == 0:
                return dfs(i - 1, a[i - 1] & ~(k << 1 | k >> 1), 0) if i else 0
            lb = j & -j
            return max(dfs(i, j ^ lb, k), dfs(i, j & ~(lb * 3), k | lb) + 1)
        return dfs(len(seats) - 1, a[-1], 0)
```

```go [sol-Go]
func maxStudents(seats [][]byte) int {
    m := len(seats)
    a := make([]int, m)
    for i, s := range seats {
        for j, c := range s {
            if c == '.' {
                a[i] |= 1 << j
            }
        }
    }

    memo := map[[3]int]int{}
    var dfs func(int, int, int) int
    dfs = func(i, j, k int) (res int) {
        t := [3]int{i, j, k}
        if v, ok := memo[t]; ok {
            return v
        }
        defer func() { memo[t] = res }()
        if j == 0 {
            if i == 0 {
                return 0
            }
            return dfs(i-1, a[i-1]&^(k<<1|k>>1), 0)
        }
        lb := j & -j
        return max(dfs(i, j^lb, k), dfs(i, j&^(lb*3), k|lb)+1)
    }
    return dfs(m-1, a[m-1], 0)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(mn\cdot 1.618^n)$。由于这种写法不会枚举不符合要求的子集，所以比「枚举选哪个」的写法更快。我用 $k$ 的补集当作第 $i-1$ 排的 $j$ 来估计（这比实际的要大），从数据来看复杂度是 $\mathcal{O}(mn\cdot 1.618^n)$。感兴趣的读者可以查阅论文 Lattice animals on a staircase and Fibonacci numbers。
- 空间复杂度：$\mathcal{O}(mn\cdot 1.618^n)$。

## 总结

本文的两种写法，「枚举选哪个」相当于一排一排地考虑座位，「选或不选」相当于一个一个地考虑座位。大多数人都写的前者，而鲜有写后者的。事实上，如果涉及到了无效的子集，用后者往往能得到更优的时间复杂度。

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

本题来自 `八、网络流`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、网络流`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
