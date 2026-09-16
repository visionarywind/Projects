# 1039. 多边形三角剖分的最低得分

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-score-triangulation-of-polygon/
- 题目 slug：`minimum-score-triangulation-of-polygon`
- 来源专题：动态规划
- 来源分类路径：八、区间 DP / §8.2 区间 DP
- 难度分：2130
- 外部题解来源：https://leetcode.cn/problems/minimum-score-triangulation-of-polygon/solutions/2203005/shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-aty6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【视频】教你一步步思考动态规划，从记忆化搜索到递推（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-score-triangulation-of-polygon/solutions/2203005/shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-aty6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shi-pin-jiao-ni-yi-bu-bu-si-kao-dong-tai-aty6`
- topic id：`2203005`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 本题视频讲解

我单独制作了一期视频来讲**区间 DP**，其中就包括这道题目。

[区间 DP【基础算法精讲 22】](https://www.bilibili.com/video/BV1Gs4y1E7EU/)，制作不易，欢迎点赞关注~

## 一、记忆化搜索

![1039-cut.png](https://pic.leetcode.cn/1680388698-XNaKai-1039-cut.png){:width=600px}

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

⚠**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。本题由于 $\textit{values}[i]>0$，所以 $\textit{memo}[i][j]$ 可以初始化成 $0$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

#### 答疑

**问**：为什么凸 $n$ 边形剖分的结果一定是 $n-2$ 个三角形？

**答**：这可以归纳证明，从 $n=3$ 开始，$n$ 每增加 $1$，剖分就多 $1$ 个三角形。

**问**：区间 DP 有一个「复制一倍，断环成链」的技巧，本题为什么不用这样计算？

**答**：无论如何旋转多边形，无论从哪条边开始计算，得到的结果都是一样的，那么不妨就从 $0$ - $(n-1)$ 这条边开始计算。

```py [sol-Python3]
class Solution:
    def minScoreTriangulation(self, v: List[int]) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs（一行代码实现记忆化）
        def dfs(i: int, j: int) -> int:
            if i + 1 == j:
                return 0  # 只有两个点，无法组成三角形
            return min(dfs(i, k) + dfs(k, j) + v[i] * v[j] * v[k]
                       for k in range(i + 1, j))  # 枚举顶点 k

        return dfs(0, len(v) - 1)
```

```java [sol-Java]
class Solution {
    public int minScoreTriangulation(int[] values) {
        int n = values.length;
        int[][] memo = new int[n][n];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return dfs(0, n - 1, values, memo);
    }

    private int dfs(int i, int j, int[] v, int[][] memo) {
        if (i + 1 == j) {
            return 0; // 只有两个点，无法组成三角形
        }

        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }

        int res = Integer.MAX_VALUE;
        for (int k = i + 1; k < j; k++) { // 枚举顶点 k
            int subRes = dfs(i, k, v, memo) + dfs(k, j, v, memo) + v[i] * v[j] * v[k];
            res = Math.min(res, subRes);
        }

        return memo[i][j] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minScoreTriangulation(vector<int>& v) {
        int n = v.size();
        vector memo(n, vector<int>(n, -1)); // -1 表示没有计算过

        // lambda 递归函数
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i + 1 == j) {
                return 0; // 只有两个点，无法组成三角形
            }
            int& res = memo[i][j]; // 注意这里是引用，修改 res 相当于修改 memo[i][j]
            if (res != -1) { // 之前计算过
                return res;
            }
            res = INT_MAX;
            for (int k = i + 1; k < j; k++) { // 枚举顶点 k
                res = min(res, dfs(i, k) + dfs(k, j) + v[i] * v[j] * v[k]);
            }
            return res;
        };

        return dfs(0, n - 1);
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int minScoreTriangulation(int* v, int n) {
    int** memo = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        memo[i] = malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            memo[i][j] = -1; // -1 表示没有计算过
        }
    }

    int dfs(int i, int j) {
        if (i + 1 == j) {
            return 0; // 只有两个点，无法组成三角形
        }
        if (memo[i][j] != -1) { // 之前计算过
            return memo[i][j];
        }
        int res = INT_MAX;
        for (int k = i + 1; k < j; k++) { // 枚举顶点 k
            int sub_res = dfs(i, k) + dfs(k, j) + v[i] * v[j] * v[k];
            res = MIN(res, sub_res);
        }
        return memo[i][j] = res; // 记忆化
    }
    int ans = dfs(0, n - 1);

    for (int i = 0; i < n; i++) {
        free(memo[i]);
    }
    free(memo);
    return ans;
}
```

```go [sol-Go]
func minScoreTriangulation(v []int) int {
    n := len(v)
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, n)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }

    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i+1 == j { // 只有两个点，无法组成三角形
            return 0
        }
        p := &memo[i][j]
        if *p != -1 { // 之前计算过
            return *p
        }
        res := math.MaxInt
        for k := i + 1; k < j; k++ { // 枚举顶点 k
            res = min(res, dfs(i, k)+dfs(k, j)+v[i]*v[j]*v[k])
        }
        *p = res // 记忆化
        return res
    }

    return dfs(0, n-1)
}
```

```js [sol-JavaScript]
var minScoreTriangulation = function(v) {
    const n = v.length;
    const memo = Array.from({ length: n }, () => Array(n).fill(-1)); // -1 表示没有计算过

    function dfs(i, j) {
        if (i + 1 === j) {
            return 0; // 只有两个点，无法组成三角形
        }
        if (memo[i][j] !== -1) { // 之前计算过
            return memo[i][j];
        }
        let res = Infinity;
        for (let k = i + 1; k < j; k++) { // 枚举顶点 k
            res = Math.min(res, dfs(i, k) + dfs(k, j) + v[i] * v[j] * v[k]);
        }
        return memo[i][j] = res; // 记忆化
    }

    return dfs(0, n - 1);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_score_triangulation(v: Vec<i32>) -> i32 {
        fn dfs(i: usize, j: usize, v: &[i32], memo: &mut [Vec<i32>]) -> i32 {
            if i + 1 == j {
                return 0; // 只有两个点，无法组成三角形
            }
            if memo[i][j] != -1 { // 之前计算过
                return memo[i][j];
            }
            let mut res = i32::MAX;
            for k in i + 1..j { // 枚举顶点 k
                let val = dfs(i, k, v, memo) + dfs(k, j, v, memo) + v[i] * v[j] * v[k];
                res = res.min(val);
            }
            memo[i][j] = res; // 记忆化
            res
        }

        let n = v.len();
        let mut memo = vec![vec![-1; n]; n]; // -1 表示没有计算过
        dfs(0, n - 1, &v, &mut memo)
    }
}
```

### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^3)$，其中 $n$ 为 $\textit{values}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题中状态个数等于 $\mathcal{O}(n^2)$，单个状态的计算时间为 $\mathcal{O}(n)$，因此时间复杂度为 $\mathcal{O}(n^3)$。
- 空间复杂度：$\mathcal{O}(n^2)$。保存多少状态，就需要多少空间。

## 二、1:1 翻译成递推

根据视频中讲的，把 $\textit{dfs}$ 改成 $f$ 数组，把递归改成循环就好了。相当于原来是用递归计算每个状态 $(i,j)$，现在改用循环去计算每个状态 $(i,j)$。

状态转移方程和递归**完全一致**：

$$
f[i][j]=\min_{k=i+1}^{j-1}\{f[i][k]+f[k][j]+v[i]\cdot v[j]\cdot v[k]\}
$$

需要注意循环的顺序：

- 由于 $i<k$，$f[i]$ 要能从 $f[k]$ 转移过来，必须先计算出 $f[k]$，所以 $i$ 要**倒序**枚举；
- 由于 $j>k$，$f[i][j]$ 要能从 $f[i][k]$ 转移过来，必须先计算出 $f[i][k]$，所以 $j$ 要**正序**枚举。

此外，递推式中的 $j\ge i+2$（$j=i+1$ 的情况是初始值，无需计算），由于 $j\le n-1$，所以 $i+2\le n-1$，即 $i\le n-3$，所以 $i$ 从 $n-3$ 开始枚举。

初始值 $f[i][i+1]=0$，翻译自递归边界 $\textit{dfs}(i,i+1) = 0$。

答案为 $f[0][n-1]$，翻译自递归入口 $\textit{dfs}(0,n-1)$。

```py [sol-Python3]
class Solution:
    def minScoreTriangulation(self, v: List[int]) -> int:
        n = len(v)
        f = [[0] * n for _ in range(n)]
        for i in range(n - 3, -1, -1):
            for j in range(i + 2, n):
                f[i][j] = min(f[i][k] + f[k][j] + v[i] * v[j] * v[k]
                              for k in range(i + 1, j))
        return f[0][-1]
```

```java [sol-Java]
class Solution {
    public int minScoreTriangulation(int[] v) {
        int n = v.length;
        int[][] f = new int[n][n];
        for (int i = n - 3; i >= 0; i--) {
            for (int j = i + 2; j < n; j++) {
                f[i][j] = Integer.MAX_VALUE;
                for (int k = i + 1; k < j; k++) {
                    f[i][j] = Math.min(f[i][j], f[i][k] + f[k][j] + v[i] * v[j] * v[k]);
                }
            }
        }
        return f[0][n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minScoreTriangulation(vector<int>& v) {
        int n = v.size();
        vector f(n, vector<int>(n));
        for (int i = n - 3; i >= 0; i--) {
            for (int j = i + 2; j < n; j++) {
                f[i][j] = INT_MAX;
                for (int k = i + 1; k < j; k++) {
                    f[i][j] = min(f[i][j], f[i][k] + f[k][j] + v[i] * v[j] * v[k]);
                }
            }
        }
        return f[0][n - 1];
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

int minScoreTriangulation(int* v, int n) {
    int** f = malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        f[i] = calloc(n, sizeof(int));
    }
    for (int i = n - 3; i >= 0; i--) {
        for (int j = i + 2; j < n; j++) {
            f[i][j] = INT_MAX;
            for (int k = i + 1; k < j; k++) {
                f[i][j] = MIN(f[i][j], f[i][k] + f[k][j] + v[i] * v[j] * v[k]);
            }
        }
    }
    int ans = f[0][n - 1];

    for (int i = 0; i < n; i++) {
        free(f[i]);
    }
    free(f);
    return ans;
}
```

```go [sol-Go]
func minScoreTriangulation(v []int) int {
    n := len(v)
    f := make([][]int, n)
    for i := range f {
        f[i] = make([]int, n)
    }
    for i := n - 3; i >= 0; i-- {
        for j := i + 2; j < n; j++ {
            f[i][j] = math.MaxInt
            for k := i + 1; k < j; k++ {
                f[i][j] = min(f[i][j], f[i][k]+f[k][j]+v[i]*v[j]*v[k])
            }
        }
    }
    return f[0][n-1]
}
```

```js [sol-JavaScript]
var minScoreTriangulation = function(v) {
    const n = v.length;
    const f = Array.from({ length: n }, () => Array(n).fill(0));
    for (let i = n - 3; i >= 0; i--) {
        for (let j = i + 2; j < n; j++) {
            f[i][j] = Infinity;
            for (let k = i + 1; k < j; k++) {
                f[i][j] = Math.min(f[i][j], f[i][k] + f[k][j] + v[i] * v[j] * v[k]);
            }
        }
    }
    return f[0][n - 1];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_score_triangulation(v: Vec<i32>) -> i32 {
        let n = v.len();
        let mut f = vec![vec![0; n]; n];
        for i in (0..n - 2).rev() {
            for j in i + 2..n {
                f[i][j] = i32::MAX;
                for k in i + 1..j {
                    f[i][j] = f[i][j].min(f[i][k] + f[k][j] + v[i] * v[j] * v[k]);
                }
            }
        }
        f[0][n - 1]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^3)$，其中 $n$ 为 $\textit{values}$ 的长度。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 思考题

计算把 $n$ 边形三角剖分的**方案数**。

欢迎在评论区分享你的思路/代码。

## 专题训练

见下面动态规划题单的「**八、区间 DP**」

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

本题来自 `八、区间 DP / §8.2 区间 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、区间 DP / §8.2 区间 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
