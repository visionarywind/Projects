# 1690. 石子游戏 VII

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/stone-game-vii/
- 题目 slug：`stone-game-vii`
- 来源专题：数学算法
- 来源分类路径：四、博弈论
- 难度分：1951
- 外部题解来源：https://leetcode.cn/problems/stone-game-vii/solutions/2629582/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-zktx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考动态规划：从记忆化搜索到递推（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/stone-game-vii/solutions/2629582/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-zktx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-zktx`
- topic id：`2629582`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:33:36 +0800

## 一、启发思考：寻找子问题

设爱丽丝的最终得分为 $A$，鲍勃的最终得分为 $B$，那么爱丽丝需要最大化 $A-B$，鲍勃需要最小化 $A-B$，或者说最大化 $B-A$。

也就是说，每个玩家都需要**最大化自己的得分减去对手的得分**。

看示例 1，$\textit{stones} = [5,3,1,4,2]$。枚举爱丽丝在第一回合移除的石子：

- 移除最左边的石子 $\textit{stones}[0]=5$，那么需要解决的问题是：剩余石子为 $\textit{stones}' = [3,1,4,2]$，鲍勃（他现在是先手）的得分减去爱丽丝（她现在是后手）的得分最大是多少。
- 移除最右边的石子 $\textit{stones}[4]=2$，那么需要解决的问题是：剩余石子为 $\textit{stones}' = [5,3,1,4]$，鲍勃（他现在是先手）的得分减去爱丽丝（她现在是后手）的得分最大是多少。

我们需要解决的子问题，都是**最大化「先手」的得分减去「后手」的得分**，都是**和原问题相似的、规模更小的子问题**，所以可以用**递归**解决。

虽然我们找到了子问题，但是，**原问题和子问题的关系**是什么？怎么利用子问题的结果，算出原问题的答案？

## 二、递归怎么写：状态定义与状态转移方程

因为要解决的问题都形如「对于 $\textit{stones}$ 中的一个连续子数组，计算先手得分减去后手得分的最大值」，所以定义 $\textit{dfs}(i,j)$ 表示剩余石子从 $\textit{stones}[i]$ 到 $\textit{stones}[j]$，先手得分减去后手得分的最大值。

例如 $\textit{stones} = [5,3,1,4,2]$。在第一回合，如果爱丽丝移除最右边的 $\textit{stones}[4]=2$，得到 $\textit{pt}_4 = 5 + 3 + 1 + 4 = 13$ 分，那么问题变成：对于 $\textit{stones}' = [5,3,1,4]$，鲍勃选哪颗石子，可以最大化「鲍勃的得分减去爱丽丝的得分」，注意这里的得分是指在 $\textit{stones}'$ 上的得分。

对于 $\textit{stones}'$，设鲍勃的最终得分为 $B'$，爱丽丝的最终得分为 $A'$，那么子问题

$$
\textit{dfs}(0,3) = B'-A'
$$

我们要计算的原问题

$$
\textit{dfs}(0,4)=A-B
$$

由于 $A = \textit{pt}_4 + A',\ B = B'$，所以有

$$
\textit{dfs}(0,4) = A-B = (\textit{pt}_4+A') - B' = \textit{pt}_4 - (B'-A') = \textit{pt}_4 - \textit{dfs}(0,3)
$$

即

$$
\textit{dfs}(0,4) = \textit{pt}_4 - \textit{dfs}(0,3)
$$

这样就找到了**原问题和子问题的关系**。

设 $\textit{stones}$ 的**前缀和**数组为 $s$。关于 $s$ 数组的定义，请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

一般地，如果剩余石子从 $\textit{stones}[i]$ 到 $\textit{stones}[j]$，我们枚举先手移除的石子：

- 如果移除的是最左边的石子 $\textit{stones}[i]$，我们得到了 $s[j+1]-s[i+1]$ 分，这种情况下 $\textit{dfs}(i,j) = s[j+1]-s[i+1]-\textit{dfs}(i+1,j)$。
- 如果移除的是最右边的石子 $\textit{stones}[j]$，我们得到了 $s[j]-s[i]$ 分，这种情况下 $\textit{dfs}(i,j) = s[j]-s[i]-\textit{dfs}(i,j-1)$。

这两种情况取最大值，即

$$
\textit{dfs}(i,j) = \max(s[j+1]-s[i+1]-\textit{dfs}(i+1,j),\ s[j]-s[i]-\textit{dfs}(i,j-1))
$$

递归边界：$\textit{dfs}(i,i)=0$。此时只有一颗石子 $\textit{stones}[i]$，移除后没有剩余石子，无法继续游戏，得分之差为 $0$。

递归入口：$\textit{dfs}(0,n-1)$，也就是答案。

> 注：动态规划有「选或不选」和「枚举选哪个」两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「枚举选（移除）哪个」。

```py [sol-Python3]
# 会超时的递归代码
class Solution:
    def stoneGameVII(self, stones: List[int]) -> int:
        s = list(accumulate(stones, initial=0))  # 前缀和
        def dfs(i: int, j: int) -> int:
            if i == j:  # 递归边界
                return 0
            return max(s[j + 1] - s[i + 1] - dfs(i + 1, j), s[j] - s[i] - dfs(i, j - 1))
        return dfs(0, len(stones) - 1)
```

```java [sol-Java]
// 会超时的递归代码
class Solution {
    public int stoneGameVII(int[] stones) {
        int n = stones.length;
        int[] s = new int[n + 1]; // 前缀和
        for (int i = 0; i < n; i++) {
            s[i + 1] += s[i] + stones[i];
        }
        return dfs(0, n - 1, s);
    }

    private int dfs(int i, int j, int[] s) {
        if (i == j) { // 递归边界
            return 0;
        }
        int res1 = s[j + 1] - s[i + 1] - dfs(i + 1, j, s);
        int res2 = s[j] - s[i] - dfs(i, j - 1, s);
        return Math.max(res1, res2);
    }
}
```

```cpp [sol-C++]
// 会超时的递归代码
class Solution {
public:
    int stoneGameVII(vector<int>& stones) {
        int n = stones.size();
        vector<int> s(n + 1); // 前缀和
        partial_sum(stones.begin(), stones.end(), s.begin() + 1);
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i == j) { // 递归边界
                return 0;
            }
            return max(s[j + 1] - s[i + 1] - dfs(i + 1, j), s[j] - s[i] - dfs(i, j - 1));
        };
        return dfs(0, n - 1);
    }
};
```

```go [sol-Go]
// 会超时的递归代码
func stoneGameVII(stones []int) int {
    n := len(stones)
    s := make([]int, n+1) // 前缀和
    for i, x := range stones {
        s[i+1] = s[i] + x
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i == j { // 递归边界
            return 0
        }
        return max(s[j+1]-s[i+1]-dfs(i+1, j), s[j]-s[i]-dfs(i, j-1))
    }
    return dfs(0, n-1)
}
```

```js [sol-JavaScript]
// 会超时的递归代码
var stoneGameVII = function(stones) {
    const n = stones.length;
    const s = Array(n + 1); // 前缀和
    s[0] = 0;
    for (let i = 0; i < n; i++) {
        s[i + 1] = s[i] + stones[i];
    }
    function dfs(i, j) {
        if (i === j) { // 递归边界
            return 0;
        }
        return Math.max(s[j + 1] - s[i + 1] - dfs(i + 1, j), s[j] - s[i] - dfs(i, j - 1));
    }
    return dfs(0, n - 1);
};
```

```rust [sol-Rust]
// 会超时的递归代码
impl Solution {
    pub fn stone_game_vii(stones: Vec<i32>) -> i32 {
        let n = stones.len();
        let mut s = vec![0; n + 1]; // 前缀和
        for (i, &x) in stones.iter().enumerate() {
            s[i + 1] = s[i] + x;
        }
        fn dfs(i: usize, j: usize, s: &Vec<i32>) -> i32 {
            if i == j { // 递归边界
                return 0;
            }
            let res1 = s[j + 1] - s[i + 1] - dfs(i + 1, j, s);
            let res2 = s[j] - s[i] - dfs(i, j - 1, s);
            res1.max(res2)
        }
        dfs(0, n - 1, &s)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(2^n)$，其中 $n$ 是 $\textit{stones}$ 的长度。搜索树可以近似为一棵二叉树，树高为 $\mathcal{O}(n)$，所以节点个数为 $\mathcal{O}(2^n)$，遍历搜索树需要 $\mathcal{O}(2^n)$ 的时间。
- 空间复杂度：$\mathcal{O}(n)$。

## 三、递归 + 记录返回值 = 记忆化搜索

上面的做法太慢了，怎么优化呢？

注意到，「先移除 $\textit{stones}[0]$ 再移除 $\textit{stones}[n-1]$」和「先移除 $\textit{stones}[n-1]$ 再移除 $\textit{stones}[0]$」，都会从 $\textit{dfs}(0,n-1)$ 递归到 $\textit{dfs}(1,n-2)$。

一叶知秋，整个递归中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

本题由于每次计算的得分是一个连续子数组的元素和，并且下一回合计算的子数组，一定在当前回合计算的子数组中，所以随着游戏的不断进行，每回合的得分是严格递减的（注意 $\textit{stones}$ 中的数都是正数），所以在 $i<j$ 时，$\textit{dfs}(i,j) > 0$，就算我们把 $\textit{memo}$ 数组初始化成 $0$ 也没问题。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

关于记忆化搜索的原理，请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def stoneGameVII(self, stones: List[int]) -> int:
        s = list(accumulate(stones, initial=0))  # 前缀和
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int, j: int) -> int:
            if i == j:  # 递归边界
                return 0
            return max(s[j + 1] - s[i + 1] - dfs(i + 1, j), s[j] - s[i] - dfs(i, j - 1))
        ans = dfs(0, len(stones) - 1)
        dfs.cache_clear()  # 防止爆内存
        return ans
```

```java [sol-Java]
class Solution {
    public int stoneGameVII(int[] stones) {
        int n = stones.length;
        int[] s = new int[n + 1]; // 前缀和
        for (int i = 0; i < n; i++) {
            s[i + 1] += s[i] + stones[i];
        }
        int[][] memo = new int[n][n];
        return dfs(0, n - 1, s, memo);
    }

    private int dfs(int i, int j, int[] s, int[][] memo) {
        if (i == j) { // 递归边界
            return 0;
        }
        if (memo[i][j] > 0) { // 之前计算过
            return memo[i][j];
        }
        int res1 = s[j + 1] - s[i + 1] - dfs(i + 1, j, s, memo);
        int res2 = s[j] - s[i] - dfs(i, j - 1, s, memo);
        return memo[i][j] = Math.max(res1, res2); // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int stoneGameVII(vector<int>& stones) {
        int n = stones.size();
        vector<int> s(n + 1); // 前缀和
        partial_sum(stones.begin(), stones.end(), s.begin() + 1);
        vector memo(stones.size(), vector<int>(stones.size()));
        auto dfs = [&](this auto&& dfs, int i, int j) -> int {
            if (i == j) { // 递归边界
                return 0;
            }
            int &res = memo[i][j]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }
            return res = max(s[j + 1] - s[i + 1] - dfs(i + 1, j), s[j] - s[i] - dfs(i, j - 1));
        };
        return dfs(0, n - 1);
    }
};
```

```go [sol-Go]
func stoneGameVII(stones []int) int {
    n := len(stones)
    s := make([]int, n+1) // 前缀和
    for i, x := range stones {
        s[i+1] = s[i] + x
    }
    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, n)
    }
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i == j { // 递归边界
            return 0
        }
        p := &memo[i][j]
        if *p == 0 {
            *p = max(s[j+1]-s[i+1]-dfs(i+1, j), s[j]-s[i]-dfs(i, j-1))
        }
        return *p
    }
    return dfs(0, n-1)
}
```

```js [sol-JavaScript]
var stoneGameVII = function(stones) {
    const n = stones.length;
    const s = Array(n + 1); // 前缀和
    s[0] = 0;
    for (let i = 0; i < n; i++) {
        s[i + 1] = s[i] + stones[i];
    }
    const memo = Array(n).fill(null).map(() => Array(n).fill(0)); // 0 表示没有计算过
    function dfs(i, j) {
        if (i === j) { // 递归边界
            return 0;
        }
        if (memo[i][j]) { // 之前计算过
            return memo[i][j];
        }
        return memo[i][j] = Math.max(s[j + 1] - s[i + 1] - dfs(i + 1, j), s[j] - s[i] - dfs(i, j - 1));
    }
    return dfs(0, n - 1);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn stone_game_vii(stones: Vec<i32>) -> i32 {
        let n = stones.len();
        let mut s = vec![0; n + 1]; // 前缀和
        for (i, &x) in stones.iter().enumerate() {
            s[i + 1] = s[i] + x;
        }
        let mut memo = vec![vec![0; n]; n]; // 0 表示没有计算过
        fn dfs(i: usize, j: usize, s: &Vec<i32>, memo: &mut Vec<Vec<i32>>) -> i32 {
            if i == j { // 递归边界
                return 0;
            }
            if memo[i][j] > 0 { // 之前计算过
                return memo[i][j];
            }
            let res1 = s[j + 1] - s[i + 1] - dfs(i + 1, j, s, memo);
            let res2 = s[j] - s[i] - dfs(i, j - 1, s, memo);
            memo[i][j] = res1.max(res2); // 记忆化
            memo[i][j]
        }
        dfs(0, n - 1, &s, &mut memo)
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{stones}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(n^2)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。有多少个状态，$\textit{memo}$ 数组的大小就是多少。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示剩余石子从 $\textit{stones}[i]$ 到 $\textit{stones}[j]$，先手得分减去后手得分的最大值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i][j] = \max(s[j+1]-s[i+1]-f[i+1][j],\ s[j]-s[i]-f[i][j-1])
$$

> 相当于之前是用递归去计算每个状态，现在是**枚举**并计算每个状态。

初始值 $f[i][i]=0$，翻译自递归边界 $\textit{dfs}(i,i)=0$。

答案为 $f[0][n-1]$，翻译自递归入口 $\textit{dfs}(0,n-1)$。

#### 答疑

**问**：如何思考循环顺序？什么时候要正序枚举，什么时候要倒序枚举？

**答**：这里有一个通用的做法：盯着状态转移方程，想一想，要计算 $f[i][j]$，其一，必须先把 $f[i+1][j]$ 算出来，那么只有 $i$ 从大到小枚举才能做到；其二，必须先把 $f[i][j-1]$ 算出来，那么对于内层循环，只有 $j$ 从小到大枚举才能做到。

```py [sol-Python3]
class Solution:
    def stoneGameVII(self, stones: List[int]) -> int:
        n = len(stones)
        s = list(accumulate(stones, initial=0))
        f = [[0] * n for _ in range(n)]
        for i in range(n - 2, -1, -1):
            for j in range(i + 1, n):
                f[i][j] = max(s[j + 1] - s[i + 1] - f[i + 1][j], s[j] - s[i] - f[i][j - 1])
        return f[0][-1]
```

```java [sol-Java]
class Solution {
    public int stoneGameVII(int[] stones) {
        int n = stones.length;
        int[] s = new int[n + 1];
        for (int i = 0; i < n; i++) {
            s[i + 1] += s[i] + stones[i];
        }
        int[][] f = new int[n][n];
        for (int i = n - 2; i >= 0; i--) {
            for (int j = i + 1; j < n; j++) {
                f[i][j] = Math.max(s[j + 1] - s[i + 1] - f[i + 1][j], s[j] - s[i] - f[i][j - 1]);
            }
        }
        return f[0][n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int stoneGameVII(vector<int>& stones) {
        int n = stones.size();
        vector<int> s(n + 1);
        partial_sum(stones.begin(), stones.end(), s.begin() + 1);
        vector f(n, vector<int>(n));
        for (int i = n - 2; i >= 0; i--) {
            for (int j = i + 1; j < n; j++) {
                f[i][j] = max(s[j + 1] - s[i + 1] - f[i + 1][j], s[j] - s[i] - f[i][j - 1]);
            }
        }
        return f[0][n - 1];
    }
};
```

```go [sol-Go]
func stoneGameVII(stones []int) int {
    n := len(stones)
    s := make([]int, n+1)
    for i, x := range stones {
        s[i+1] = s[i] + x
    }
    f := make([][]int, n)
    for i := n - 1; i >= 0; i-- {
        f[i] = make([]int, n)
        for j := i + 1; j < n; j++ {
            f[i][j] = max(s[j+1]-s[i+1]-f[i+1][j], s[j]-s[i]-f[i][j-1])
        }
    }
    return f[0][n-1]
}
```

```js [sol-JavaScript]
var stoneGameVII = function(stones) {
    const n = stones.length;
    const s = Array(n + 1);
    s[0] = 0;
    for (let i = 0; i < n; i++) {
        s[i + 1] = s[i] + stones[i];
    }
    const f = Array(n).fill(null).map(() => Array(n));
    for (let i = n - 1; i >= 0; i--) {
        f[i][i] = 0;
        for (let j = i + 1; j < n; j++) {
            f[i][j] = Math.max(s[j + 1] - s[i + 1] - f[i + 1][j], s[j] - s[i] - f[i][j - 1]);
        }
    }
    return f[0][n - 1];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn stone_game_vii(stones: Vec<i32>) -> i32 {
        let n = stones.len();
        let mut s = vec![0; n + 1];
        for (i, &x) in stones.iter().enumerate() {
            s[i + 1] = s[i] + x;
        }
        let mut f = vec![vec![0; n]; n];
        for i in (0..n).rev() {
            for j in i + 1..n {
                f[i][j] = (s[j + 1] - s[i + 1] - f[i + 1][j]).max(s[j] - s[i] - f[i][j - 1]);
            }
        }
        f[0][n - 1]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{stones}$ 的长度。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 五、空间优化：一个数组

由于转移方程形似完全背包，可按照 [视频](https://www.bilibili.com/video/BV16Y411v7Y6/) 中的做法，去掉第一个维度，反复利用同一个长为 $n$ 的一维数组。

```py [sol-Python3]
class Solution:
    def stoneGameVII(self, stones: List[int]) -> int:
        n = len(stones)
        s = list(accumulate(stones, initial=0))
        f = [0] * n
        for i in range(n - 2, -1, -1):
            for j in range(i + 1, n):
                f[j] = max(s[j + 1] - s[i + 1] - f[j], s[j] - s[i] - f[j - 1])
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int stoneGameVII(int[] stones) {
        int n = stones.length;
        int[] s = new int[n + 1];
        for (int i = 0; i < n; i++) {
            s[i + 1] += s[i] + stones[i];
        }
        int[] f = new int[n];
        for (int i = n - 2; i >= 0; i--) {
            for (int j = i + 1; j < n; j++) {
                f[j] = Math.max(s[j + 1] - s[i + 1] - f[j], s[j] - s[i] - f[j - 1]);
            }
        }
        return f[n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int stoneGameVII(vector<int>& stones) {
        int n = stones.size();
        vector<int> s(n + 1);
        partial_sum(stones.begin(), stones.end(), s.begin() + 1);
        vector<int> f(n);
        for (int i = n - 2; i >= 0; i--) {
            for (int j = i + 1; j < n; j++) {
                f[j] = max(s[j + 1] - s[i + 1] - f[j], s[j] - s[i] - f[j - 1]);
            }
        }
        return f[n - 1];
    }
};
```

```go [sol-Go]
func stoneGameVII(stones []int) int {
    n := len(stones)
    s := make([]int, n+1)
    for i, x := range stones {
        s[i+1] = s[i] + x
    }
    f := make([]int, n)
    for i := n - 2; i >= 0; i-- {
        for j := i + 1; j < n; j++ {
            f[j] = max(s[j+1]-s[i+1]-f[j], s[j]-s[i]-f[j-1])
        }
    }
    return f[n-1]
}
```

```js [sol-JavaScript]
var stoneGameVII = function(stones) {
    const n = stones.length;
    const s = Array(n + 1);
    s[0] = 0;
    for (let i = 0; i < n; i++) {
        s[i + 1] = s[i] + stones[i];
    }
    const f = Array(n).fill(0);
    for (let i = n - 2; i >= 0; i--) {
        for (let j = i + 1; j < n; j++) {
            f[j] = Math.max(s[j + 1] - s[i + 1] - f[j], s[j] - s[i] - f[j - 1]);
        }
    }
    return f[n - 1];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn stone_game_vii(stones: Vec<i32>) -> i32 {
        let n = stones.len();
        let mut s = vec![0; n + 1];
        for (i, &x) in stones.iter().enumerate() {
            s[i + 1] = s[i] + x;
        }
        let mut f = vec![0; n];
        for i in (0..n - 1).rev() {
            for j in i + 1..n {
                f[j] = (s[j + 1] - s[i + 1] - f[j]).max(s[j] - s[i] - f[j - 1]);
            }
        }
        f[n - 1]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 是 $\textit{stones}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、博弈论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、博弈论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
