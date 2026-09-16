# 1278. 分割回文串 III

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/palindrome-partitioning-iii/
- 题目 slug：`palindrome-partitioning-iii`
- 来源专题：动态规划
- 来源分类路径：五、划分型 DP / §5.3 约束划分个数
- 难度分：1979
- 外部题解来源：https://leetcode.cn/problems/palindrome-partitioning-iii/solutions/3081309/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-z363/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推（Python/Java/C++/Go）](https://leetcode.cn/problems/palindrome-partitioning-iii/solutions/3081309/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-z363/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-z363`
- topic id：`3081309`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、寻找子问题

### 1)

在示例 2 中，我们要解决的问题（原问题）是：

- 把 $s=\texttt{aabbc}$ 划分成 $k=3$ 个非空子串，每个子串修改成回文串，总修改次数的最小值。

枚举划分出的最右边那段子串的长度（或者左端点）：

- 划分出子串 $\texttt{c}$，那么需要解决的子问题为：把 $\texttt{aabb}$ 划分成 $2$ 个非空子串，每个子串修改成回文串，总修改次数的最小值。
- 划分出子串 $\texttt{bc}$，那么需要解决的子问题为：把 $\texttt{aab}$ 划分成 $2$ 个非空子串，每个子串修改成回文串，总修改次数的最小值。
- 划分出子串 $\texttt{bbc}$，那么需要解决的子问题为：把 $\texttt{aa}$ 划分成 $2$ 个非空子串，每个子串修改成回文串，总修改次数的最小值。
- 注意不能继续划分了，因为剩余的 $2$ 个子串必须是非空的，所以划分后的剩余长度至少是 $2$。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

> 注：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。

### 2)

举例。把子串 $s[1]$ 到 $s[5]$ 改成回文串，至少要改多少个字母？

分类讨论：

- 如果 $s[1]=s[5]$，无需修改，问题变成把 $s[2]$ 到 $s[4]$ 改成回文串，至少要改多少个字母。
- 如果 $s[1]\ne s[5]$，必须修改，且只需修改其中一个字母，比如把 $s[1]$ 改成 $s[5]$，问题变成把 $s[2]$ 到 $s[4]$ 改成回文串，至少要改多少个字母。

这同样可以得到**和原问题相似的、规模更小的子问题**，所以也可以用**递归**解决。

## 二、状态定义与状态转移方程

### 1)

根据上面的讨论，我们需要在递归过程中跟踪以下信息：

- $i$：还需要切 $i$ 刀，分出 $i+1$ 个子串。这样定义的目的是把 $i=0$ 用上，后面 1:1 翻译成递推不浪费空间。
- $r$：剩余字符串的右端点为 $r$。

因此，定义状态为 $\textit{dfs}(i,r)$，表示把 $s[0]$ 到 $s[r]$ 划分成 $i+1$ 个非空子串，每个子串修改成回文串，总修改次数的最小值。

枚举最后一段子串的左端点 $l=i,i+1,\ldots,r$，那么接下来要解决的问题是，把 $s[0]$ 到 $s[l-1]$ 划分成 $i$ 个非空子串，每个子串修改成回文串，总修改次数的最小值，即 $\textit{dfs}(i-1,l-1)$。

注意 $l$ 最小是 $i$，因为剩余的 $i$ 个子串必须是非空的，所以划分后的剩余长度至少是 $i$。

所有情况取最小值，就得到了 $\textit{dfs}(i,r)$，即

$$
\textit{dfs}(i,r) = \min_{l=i}^{r} \textit{dfs}(i-1,l-1) + \textit{minChange}(l,r)
$$

其中 $\textit{minChange}$ 的定义见下文。

**递归边界**：$\textit{dfs}(0,r)=\textit{minChange}(0,r)$。此时只有一个子串。

**递归入口**：$\textit{dfs}(k-1,|s|-1)$，这是原问题，也是答案。（$|s|$ 表示 $s$ 的长度）

### 2)

根据前文的讨论，定义 $\textit{minChange}(i,j)$ 表示把子串 $s[i]$ 到 $s[j]$ 改成回文串，至少要改多少个字母。

分类讨论：

- 如果 $s[i]=s[j]$，无需修改，问题变成把 $s[i+1]$ 到 $s[j-1]$ 改成回文串，至少要改多少个字母，即 $\textit{minChange}(i,j) = \textit{minChange}(i+1,j-1)$。
- 如果 $s[i]\ne s[j]$，必须修改其中一个字母，问题变成把 $s[i+1]$ 到 $s[j-1]$ 改成回文串，至少要改多少个字母，即 $\textit{minChange}(i,j) = \textit{minChange}(i+1,j-1) + 1$。

所以有

$$
\textit{minChange}(i,j) = \textit{minChange}(i+1,j-1) + [s[i]\ne s[j]]
$$

其中 $[s[i]\ne s[j]]$ 在 $s[i]\ne s[j]$ 成立时为 $1$，否则为 $0$。

**递归边界**：$\textit{minChange}(i,i)= \textit{minChange}(i+1,i) = 0$。如果子串只有一个字母，或者子串是空串，那么无需修改。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,r)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def palindromePartition(self, s: str, k: int) -> int:
        # 把 s[i:j+1] 改成回文串的最小修改次数
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（一行代码实现记忆化）
        def min_change(i: int, j: int) -> int:
            if i >= j:  # 子串只有一个字母，或者子串是空串
                return 0  # 无需修改
            return min_change(i + 1, j - 1) + (1 if s[i] != s[j] else 0)

        # 把 s[:r+1] 切 i 刀，分成 i+1 个子串，每个子串改成回文串的最小总修改次数
        @cache
        def dfs(i: int, r: int) -> int:
            if i == 0:  # 只有一个子串
                return min_change(0, r)
            # 枚举子串左端点 l
            return min(dfs(i - 1, l - 1) + min_change(l, r)
                       for l in range(i, r + 1))

        return dfs(k - 1, len(s) - 1)
```

```java [sol-Java]
class Solution {
    public int palindromePartition(String s, int k) {
        int n = s.length();
        int[][] memoChange = new int[n][n];
        for (int[] row : memoChange) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        int[][] memoDfs = new int[k][n];
        for (int[] row : memoDfs) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return dfs(k - 1, n - 1, s.toCharArray(), memoDfs, memoChange);
    }

    // 把 s[:r+1] 切 i 刀，分成 i+1 个子串，每个子串改成回文串的最小总修改次数
    private int dfs(int i, int r, char[] s, int[][] memoDfs, int[][] memoChange) {
        if (i == 0) { // 只有一个子串
            return minChange(0, r, s, memoChange);
        }
        if (memoDfs[i][r] != -1) { // 之前计算过
            return memoDfs[i][r];
        }
        int res = Integer.MAX_VALUE;
        // 枚举子串左端点 l
        for (int l = i; l <= r; l++) {
            res = Math.min(res, dfs(i - 1, l - 1, s, memoDfs, memoChange) + minChange(l, r, s, memoChange));
        }
        return memoDfs[i][r] = res; // 记忆化
    }

    // 把 s[i:j+1] 改成回文串的最小修改次数
    private int minChange(int i, int j, char[] s, int[][] memoChange) {
        if (i >= j) { // 子串只有一个字母，或者子串是空串
            return 0; // 无需修改
        }
        if (memoChange[i][j] != -1) { // 之前计算过
            return memoChange[i][j];
        }
        int res = minChange(i + 1, j - 1, s, memoChange);
        if (s[i] != s[j]) {
            res++;
        }
        return memoChange[i][j] = res; // 记忆化
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int palindromePartition(string s, int k) {
        int n = s.size();
        vector memo_change(n, vector<int>(n, -1)); // -1 表示没有计算过
        // 把 s[i:j+1] 改成回文串的最小修改次数
        auto minChange = [&](this auto&& minChange, int i, int j) -> int {
            if (i >= j) { // 子串只有一个字母，或者子串是空串
                return 0; // 无需修改
            }
            int& res = memo_change[i][j]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            return res = minChange(i + 1, j - 1) + (s[i] != s[j]);
        };

        vector memo_dfs(k, vector<int>(n, -1)); // -1 表示没有计算过
        // 把 s[:r+1] 切 i 刀，分成 i+1 个子串，每个子串改成回文串的最小总修改次数
        auto dfs = [&](this auto&& dfs, int i, int r) -> int {
            if (i == 0) { // 只有一个子串
                return minChange(0, r);
            }
            int& res = memo_dfs[i][r]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            res = INT_MAX;
            // 枚举子串左端点 l
            for (int l = i; l <= r; l++) {
                res = min(res, dfs(i - 1, l - 1) + minChange(l, r));
            }
            return res;
        };

        return dfs(k - 1, n - 1);
    }
};
```

```go [sol-Go]
func palindromePartition(s string, k int) int {
    n := len(s)
    memoChange := make([][]int, n)
    for i := range memoChange {
        memoChange[i] = make([]int, n)
        for j := range memoChange[i] {
            memoChange[i][j] = -1 // -1 表示没有计算过
        }
    }
    // 把 s[i:j+1] 改成回文串的最小修改次数
    var minChange func(int, int) int
    minChange = func(i, j int) int {
        if i >= j { // 子串只有一个字母，或者子串是空串
            return 0 // 无需修改
        }
        p := &memoChange[i][j]
        if *p != -1 { // 之前计算过
            return *p
        }
        res := minChange(i+1, j-1)
        if s[i] != s[j] {
            res++
        }
        *p = res // 记忆化
        return res
    }

    memoDfs := make([][]int, k)
    for i := range memoDfs {
        memoDfs[i] = make([]int, n)
        for j := range memoDfs[i] {
            memoDfs[i][j] = -1 // -1 表示没有计算过
        }
    }
    // 把 s[:r+1] 切 i 刀，分成 i+1 个子串，每个子串改成回文串的最小总修改次数
    var dfs func(int, int) int
    dfs = func(i, r int) int {
        if i == 0 { // 只有一个子串
            return minChange(0, r)
        }
        p := &memoDfs[i][r]
        if *p != -1 { // 之前计算过
            return *p
        }
        res := math.MaxInt
        // 枚举子串左端点 l
        for l := i; l <= r; l++ {
            res = min(res, dfs(i-1, l-1)+minChange(l, r))
        }
        *p = res // 记忆化
        return res
    }

    return dfs(k-1, n-1)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2k)$，其中 $n$ 是 $s$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(nk)$，单个状态的计算时间为 $\mathcal{O}(n)$，所以总的时间复杂度为 $\mathcal{O}(n^2k)$。注意 $\textit{minChange}$ 只需要 $\mathcal{O}(n^2)$ 的时间，相比 $\mathcal{O}(n^2k)$ 可以忽略。
- 空间复杂度：$\mathcal{O}(n^2)$。保存多少状态，就需要多少空间。这里 $\textit{dfs}$ 的状态个数是 $\mathcal{O}(nk)$，$\textit{minChange}$ 的状态个数是 $\mathcal{O}(n^2)$，由于 $k\le n$，所以空间复杂度为 $\mathcal{O}(n^2)$。

**注**：实际的时间复杂度比 $\mathcal{O}(n^2k)$ 低，见下面递推的复杂度分析。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

### 1)

$f[i][r]$ 的定义和 $\textit{dfs}(i,r)$ 的定义是完全一样的，都表示把 $s[0]$ 到 $s[r]$ 划分成 $i+1$ 个非空子串，每个子串修改成回文串，总修改次数的最小值。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i][r] = \min_{l=i}^{r} f[i-1][l-1] + \textit{minChange}[l][r]
$$

初始值 $f[0][r]=\textit{minChange}[0][r]$，翻译自递归边界 $\textit{dfs}(0,r)=\textit{minChange}(0,r)$。

答案为 $f[k-1][|s|-1]$，翻译自递归入口 $\textit{dfs}(k-1,|s|-1)$。

### 2)

定义 $\textit{minChange}[i][j]$ 表示把子串 $s[i]$ 到 $s[j]$ 改成回文串，至少要改多少个字母。

递推式（状态转移方程）为

$$
\textit{minChange}[i][j] = \textit{minChange}[i+1][j-1] + [s[i]\ne s[j]]
$$

初始值 $\textit{minChange}[i][i]=\textit{minChange}[i+1][i] = 0$，翻译自递归边界 $\textit{minChange}(i,i)= \textit{minChange}(i+1,i) = 0$。

### 优化

对于 $f[i][r]$ 中的 $r$，因为右边还有 $k-1-i$ 个子串，所以 $r$ 至多枚举到 $n-1-(k-1-i)=n-k+i$。（继续枚举没有意义，这些状态不会参与状态转移。）

### 答疑

**问**：如何思考循环顺序？什么时候要正序枚举，什么时候要倒序枚举？

**答**：这里有一个通用的做法：盯着状态转移方程，想一想，要计算 $\textit{minChange}[i][j]$，必须先把 $\textit{minChange}[i+1][j-1]$ 算出来，那么只有 $i$ 从大到小枚举才能做到。

对于 $j$ 来说，由于在计算 $\textit{minChange}[i][j]$ 时，$\textit{minChange}[i+1]$ 已经全部计算完毕，所以 $j$ 无论是正序还是倒序枚举都可以。

```py [sol-Python3]
class Solution:
    def palindromePartition(self, s: str, k: int) -> int:
        n = len(s)
        min_change = [[0] * n for _ in range(n)]
        for i in range(n - 2, -1, -1):
            for j in range(i + 1, n):
                min_change[i][j] = min_change[i + 1][j - 1] + (1 if s[i] != s[j] else 0)

        f = [[0] * n for _ in range(k)]
        f[0] = min_change[0]  # 无需 copy
        for i in range(1, k):
            for r in range(i, n - (k - 1 - i)):  # 右边还有 k−1−i 个子串
                f[i][r] = min(f[i - 1][l - 1] + min_change[l][r] for l in range(i, r + 1))
        return f[-1][-1]
```

```java [sol-Java]
class Solution {
    public int palindromePartition(String S, int k) {
        char[] s = S.toCharArray();
        int n = s.length;
        int[][] minChange = new int[n][n];
        for (int i = n - 2; i >= 0; i--) {
            for (int j = i + 1; j < n; j++) {
                minChange[i][j] = minChange[i + 1][j - 1] + (s[i] != s[j] ? 1 : 0);
            }
        }

        int[][] f = new int[k][n];
        f[0] = minChange[0]; // 无需 copy
        for (int i = 1; i < k; i++) {
            for (int r = i; r <= n - k + i; r++) {
                f[i][r] = Integer.MAX_VALUE;
                for (int l = i; l <= r; l++) {
                    f[i][r] = Math.min(f[i][r], f[i - 1][l - 1] + minChange[l][r]);
                }
            }
        }
        return f[k - 1][n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int palindromePartition(string s, int k) {
        int n = s.size();
        vector min_change(n, vector<int>(n));
        for (int i = n - 2; i >= 0; i--) {
            for (int j = i + 1; j < n; j++) {
                min_change[i][j] = min_change[i + 1][j - 1] + (s[i] != s[j] ? 1 : 0);
            }
        }

        vector f(k, vector<int>(n, INT_MAX));
        f[0] = move(min_change[0]);
        for (int i = 1; i < k; i++) {
            for (int r = i; r <= n - k + i; r++) {
                for (int l = i; l <= r; l++) {
                    f[i][r] = min(f[i][r], f[i - 1][l - 1] + min_change[l][r]);
                }
            }
        }
        return f[k - 1][n - 1];
    }
};
```

```go [sol-Go]
func palindromePartition(s string, k int) int {
    n := len(s)
    minChange := make([][]int, n)
    for i := n - 1; i >= 0; i-- {
        minChange[i] = make([]int, n)
        for j := i + 1; j < n; j++ {
            minChange[i][j] = minChange[i+1][j-1]
            if s[i] != s[j] {
                minChange[i][j]++
            }
        }
    }

    f := make([][]int, k)
    for i := range f {
        f[i] = make([]int, n)
    }
    f[0] = minChange[0] // 无需 copy
    for i := 1; i < k; i++ {
        for r := i; r <= n-k+i; r++ {
            f[i][r] = math.MaxInt
            for l := i; l <= r; l++ {
                f[i][r] = min(f[i][r], f[i-1][l-1]+minChange[l][r])
            }
        }
    }
    return f[k-1][n-1]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2 + (n-k)^2k)$，其中 $n$ 是 $s$ 的长度。在三重循环中，$r$ 和 $l$ 的循环次数都是 $\mathcal{O}(n-k)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

## 五、进一步优化

观察上面的状态转移方程，在计算 $f[i]$ 时，只会用到 $f[i-1]$，不会用到比 $i-1$ 更早的状态。

因此可以像 [0-1 背包](https://www.bilibili.com/video/BV16Y411v7Y6/) 那样，去掉第一个维度，把 $f[i]$ 和 $f[i-1]$ 保存到**同一个数组**中。（注意 $r$ 要倒序枚举）

状态转移方程改为

$$
f[r] = \min_{l=i}^{r} f[l-1] + \textit{minChange}[l][r]
$$

初始值 $f[r]=\textit{minChange}[0][r]$。

答案为 $f[|s|-1]$。

```py [sol-Python3]
class Solution:
    def palindromePartition(self, s: str, k: int) -> int:
        n = len(s)
        min_change = [[0] * n for _ in range(n)]
        for i in range(n - 2, -1, -1):
            for j in range(i + 1, n):
                min_change[i][j] = min_change[i + 1][j - 1] + (1 if s[i] != s[j] else 0)

        f = min_change[0]  # 无需 copy
        for i in range(1, k):
            for r in range(n - k + i, i - 1, -1):
                f[r] = min(f[l - 1] + min_change[l][r] for l in range(i, r + 1))
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int palindromePartition(String S, int k) {
        char[] s = S.toCharArray();
        int n = s.length;
        int[][] minChange = new int[n][n];
        for (int i = n - 2; i >= 0; i--) {
            for (int j = i + 1; j < n; j++) {
                minChange[i][j] = minChange[i + 1][j - 1] + (s[i] != s[j] ? 1 : 0);
            }
        }

        int[] f = minChange[0];
        for (int i = 1; i < k; i++) {
            for (int r = n - k + i; r >= i; r--) {
                f[r] = Integer.MAX_VALUE;
                for (int l = i; l <= r; l++) {
                    f[r] = Math.min(f[r], f[l - 1] + minChange[l][r]);
                }
            }
        }
        return f[n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int palindromePartition(string s, int k) {
        int n = s.size();
        vector min_change(n, vector<int>(n));
        for (int i = n - 2; i >= 0; i--) {
            for (int j = i + 1; j < n; j++) {
                min_change[i][j] = min_change[i + 1][j - 1] + (s[i] != s[j] ? 1 : 0);
            }
        }

        auto f = move(min_change[0]);
        for (int i = 1; i < k; i++) {
            for (int r = n - k + i; r >= i; r--) {
                f[r] = INT_MAX;
                for (int l = i; l <= r; l++) {
                    f[r] = min(f[r], f[l - 1] + min_change[l][r]);
                }
            }
        }
        return f[n - 1];
    }
};
```

```go [sol-Go]
func palindromePartition(s string, k int) int {
    n := len(s)
    minChange := make([][]int, n)
    for i := n - 1; i >= 0; i-- {
        minChange[i] = make([]int, n)
        for j := i + 1; j < n; j++ {
            minChange[i][j] = minChange[i+1][j-1]
            if s[i] != s[j] {
                minChange[i][j]++
            }
        }
    }

    f := minChange[0]
    for i := 1; i < k; i++ {
        for r := n - k + i; r >= i; r-- {
            f[r] = math.MaxInt
            for l := i; l <= r; l++ {
                f[r] = min(f[r], f[l-1]+minChange[l][r])
            }
        }
    }
    return f[n-1]
}
```

> 注：从缓存友好的角度来说，代码中的 $\textit{minChange}[i][j]$ 改成 $\textit{minChange}[j][i]$ 也许更快。也可以把 $f[i][r]$ 改成 $f[i][l]$。留给感兴趣的读者实现。

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2 + (n-k)^2k)$，其中 $n$ 是 $s$ 的长度。在三重循环中，$r$ 和 $l$ 的循环次数都是 $\mathcal{O}(n-k)$。
- 空间复杂度：$\mathcal{O}(n^2)$。

更多相似题目，见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**§5.3 约束划分个数**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. 【本题相关】[动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `五、划分型 DP / §5.3 约束划分个数`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、划分型 DP / §5.3 约束划分个数`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
