# 514. 自由之路

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/freedom-trail/
- 题目 slug：`freedom-trail`
- 来源专题：图论算法
- 来源分类路径：一、图的遍历 / §1.3 图论建模 + BFS 最短路
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/freedom-trail/solutions/2623534/onm-zuo-fa-cong-ji-yi-hua-sou-suo-dao-di-dnec/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种 O(nm) 做法：DP / BFS（Python/Java/C++/Go）](https://leetcode.cn/problems/freedom-trail/solutions/2623534/onm-zuo-fa-cong-ji-yi-hua-sou-suo-dao-di-dnec/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`onm-zuo-fa-cong-ji-yi-hua-sou-suo-dao-di-dnec`
- topic id：`2623534`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识

请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含如何把记忆化搜索 1:1 翻译成递推的技巧。

## 一、启发思考：寻找子问题

为方便描述，下文把 $\textit{ring}$ 记作 $s$，把 $\textit{key}$ 记作 $t$。设 $s$ 的长度为 $n$，$t$ 的长度为 $m$。

我们可以只计算最小旋转次数，最后加上 $m$，即为答案。

举例说明，假设 $s=\texttt{goddid},\ t=\texttt{gdi}$。

一开始 12:00 方向为 $s[0]$，由于 $s[0]=t[0]$，无需旋转，问题变成 $s=\texttt{goddid},\ t=\texttt{di}$ 且 12:00 方向为 $s[0]$ 状态下的最小旋转次数。

现在要旋转到 $s[i]=\texttt{d}$ 上，分类讨论：

- 旋转到 $s[0]=\texttt{g}$ **右边最近**的 $s[2]=\texttt{d}$ 上，需要旋转 $2$ 次。现在问题变成 $s=\texttt{goddid},\ t=\texttt{i}$ 且 12:00 方向为 $s[2]$ 状态下的最小旋转次数。
- 旋转到 $s[0]=\texttt{g}$ **左边最近**的 $s[5]=\texttt{d}$ 上，需要旋转 $1$ 次。注意 $s$ 是环形的，$s[0]$ 左边是 $s[n-1]$。现在问题变成 $s=\texttt{goddid},\ t=\texttt{i}$ 且 12:00 方向为 $s[5]$ 状态下的最小旋转次数。

我们把要解决的问题，变成了**和原问题相似的、规模更小的子问题**，所以可以用**递归**解决。

> 注：动态规划有「选或不选」和「枚举选哪个」两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「枚举选哪个」。

## 二、递归怎么写：状态定义与状态转移方程

要解决的问题都形如「给定 $s$ 和 $t'$，此时 12:00 方向为 $s[i]$，计算拼写出 $t'$ 的最小旋转次数」，其中 $t'$ 是 $t$ 的一个后缀。据此定义 $\textit{dfs}(j,i)$ 表示拼写后缀 $t[j]$ 到 $t[m-1]$，此时 12:00 方向为 $s[i]$ 时的最小旋转次数。

把 $t[j]$ 记作 $c$。

如果 $s[i] = c$，那么无需旋转，即

$$
\textit{dfs}(j,i) = \textit{dfs}(j+1, i)
$$

如果 $s[i] \ne c$，分类讨论：

- 旋转到 $s[i]$ **左边最近**的 $c$ 上，设其下标为 $l$，如果 $l > i$ 那么需要旋转 $n-(l-i)$ 次，否则需要旋转 $i-l$ 次，设旋转次数为 $\textit{kl}$。
- 旋转到 $s[i]$ **右边最近**的 $c$ 上，设其下标为 $r$，如果 $r < i$ 那么需要旋转 $n-(i-r)$ 次，否则需要旋转 $r-i$ 次，设旋转次数为 $\textit{kr}$。

这两种情况取最小值，就得到了 $\textit{dfs}(j,i)$，即

$$
\textit{dfs}(j,i) = \min(\textit{dfs}(j+1,l) + \textit{kl}, \textit{dfs}(j+1,r) + \textit{kr})
$$

其中 $l$ 和 $r$ 可以在递归之前预处理出来，具体请看代码。

递归边界：$\textit{dfs}(m,i)=0$。拼写完毕。

递归入口：$\textit{dfs}(0,0)$，加上 $m$ 即为答案。

注意下面的递归代码尚未优化（记忆化），会超时。

```py [sol-Python3]
# 会超时的递归代码
class Solution:
    def findRotateSteps(self, s: str, t: str) -> int:
        s = [ord(c) - ord('a') for c in s]
        t = [ord(c) - ord('a') for c in t]
        n = len(s)

        # 先算出每个字母的最后一次出现的下标
        # 由于 s 是环形的，循环结束后的 pos 就刚好是 left[0]
        pos = [0] * 26  # 初始值不重要
        for i, c in enumerate(s):
            pos[c] = i
        # 计算每个 s[i] 左边 a-z 的最近下标（左边没有就从 n-1 往左找）
        left = [None] * n
        for i, c in enumerate(s):
            left[i] = pos[:]
            pos[c] = i  # 更新下标

        # 先算出每个字母的首次出现的下标
        # 由于 s 是环形的，循环结束后的 pos 就刚好是 right[n-1]
        for i in range(n - 1, -1, -1):
            pos[s[i]] = i
        # 计算每个 s[i] 右边 a-z 的最近下标（左边没有就从 0 往右找）
        right = [None] * n
        for i in range(n - 1, -1, -1):
            right[i] = pos[:]
            pos[s[i]] = i  # 更新下标

        def dfs(j: int, i: int) -> int:
            if j == len(t):
                return 0
            c = t[j]
            if s[i] == c:  # 无需旋转
                return dfs(j + 1, i)
            # 左边最近 or 右边最近，取最小值
            l, r = left[i][c], right[i][c]
            return min(dfs(j + 1, l) + (n - l + i if l > i else i - l),
                       dfs(j + 1, r) + (n - i + r if r < i else r - i))

        return dfs(0, 0) + len(t)
```

```java [sol-Java]
// 会超时的递归代码
class Solution {
    private char[] s, t;
    private int[][] left, right;

    public int findRotateSteps(String S, String T) {
        s = S.toCharArray();
        t = T.toCharArray();
        int n = s.length;
        int m = t.length;

        // 先算出每个字母的最后一次出现的下标
        // 由于 s 是环形的，循环结束后的 pos 就刚好是 left[0]
        int[] pos = new int[26]; // 初始值不重要
        for (int i = 0; i < n; i++) {
            s[i] -= 'a';
            pos[s[i]] = i;
        }
        // 计算每个 s[i] 左边 a-z 的最近下标（左边没有就从 n-1 往左找）
        left = new int[n][26];
        for (int i = 0; i < n; i++) {
            System.arraycopy(pos, 0, left[i], 0, 26);
            pos[s[i]] = i; // 更新下标
        }

        // 先算出每个字母的首次出现的下标
        // 由于 s 是环形的，循环结束后的 pos 就刚好是 right[n-1]
        for (int i = n - 1; i >= 0; i--) {
            pos[s[i]] = i;
        }
        // 计算每个 s[i] 右边 a-z 的最近下标（左边没有就从 0 往右找）
        right = new int[n][26];
        for (int i = n - 1; i >= 0; i--) {
            System.arraycopy(pos, 0, right[i], 0, 26);
            pos[s[i]] = i; // 更新下标
        }

        return dfs(0, 0) + m;
    }

    private int dfs(int j, int i) {
        if (j == t.length) {
            return 0;
        }
        int c = t[j] - 'a';
        if (s[i] == c) { // 无需旋转
            return dfs(j + 1, i);
        }
        // 左边最近 or 右边最近，取最小值
        int l = left[i][c], r = right[i][c];
        return Math.min(dfs(j + 1, l) + (l > i ? s.length - l + i : i - l),
                        dfs(j + 1, r) + (r < i ? s.length - i + r : r - i));
    }
}
```

```cpp [sol-C++]
// 会超时的递归代码
class Solution {
public:
    int findRotateSteps(string s, string t) {
        int n = s.length(), m = t.length();

        // 先算出每个字母的最后一次出现的下标
        // 由于 s 是环形的，循环结束后的 pos 就刚好是 left[0]
        array<int, 26> pos; // 初始值不重要
        for (int i = 0; i < n; i++) {
            s[i] -= 'a';
            pos[s[i]] = i;
        }
        // 计算每个 s[i] 左边 a-z 的最近下标（左边没有就从 n-1 往左找）
        vector<array<int, 26>> left(n);
        for (int i = 0; i < n; i++) {
            left[i] = pos;
            pos[s[i]] = i; // 更新下标
        }

        // 先算出每个字母的首次出现的下标
        // 由于 s 是环形的，循环结束后的 pos 就刚好是 right[n-1]
        for (int i = n - 1; i >= 0; i--) {
            pos[s[i]] = i;
        }
        // 计算每个 s[i] 右边 a-z 的最近下标（左边没有就从 0 往右找）
        vector<array<int, 26>> right(n);
        for (int i = n - 1; i >= 0; i--) {
            right[i] = pos;
            pos[s[i]] = i; // 更新下标
        }

        function<int(int, int)> dfs = [&](int j, int i) -> int {
            if (j == m) {
                return 0;
            }
            char c = t[j] - 'a';
            if (s[i] == c) { // 无需旋转
                return dfs(j + 1, i);
            }
            // 左边最近 or 右边最近，取最小值
            int l = left[i][c], r = right[i][c];
            return min(dfs(j + 1, l) + (l > i ? n - l + i : i - l),
                       dfs(j + 1, r) + (r < i ? n - i + r : r - i));
        };
        return dfs(0, 0) + m;
    }
};
```

```go [sol-Go]
// 会超时的递归代码
func findRotateSteps(s, t string) int {
    n, m := len(s), len(t)

    // 先算出每个字母的最后一次出现的下标
    // 由于 s 是环形的，循环结束后的 pos 就刚好是 left[0]
    pos := [26]int{} // 初始值不重要
    for i, b := range s {
        pos[b-'a'] = i
    }
    // 计算每个 s[i] 左边 a-z 的最近下标（左边没有就从 n-1 往左找）
    left := make([][26]int, n)
    for i, b := range s {
        left[i] = pos
        pos[b-'a'] = i // 更新下标
    }

    // 先算出每个字母的首次出现的下标
    // 由于 s 是环形的，循环结束后的 pos 就刚好是 right[n-1]
    for i := n - 1; i >= 0; i-- {
        pos[s[i]-'a'] = i
    }
    // 计算每个 s[i] 右边 a-z 的最近下标（左边没有就从 0 往右找）
    right := make([][26]int, n)
    for i := n - 1; i >= 0; i-- {
        right[i] = pos
        pos[s[i]-'a'] = i // 更新下标
    }

    var dfs func(int, int) int
    dfs = func(j, i int) int {
        if j == m {
            return 0
        }
        if s[i] == t[j] { // 无需旋转
            return dfs(j+1, i)
        }
        // 左边最近 or 右边最近，取最小值
        l := left[i][t[j]-'a']
        res1 := dfs(j+1, l)
        if l > i {
            res1 += n - l + i
        } else {
            res1 += i - l
        }
        r := right[i][t[j]-'a']
        res2 := dfs(j+1, r)
        if r < i {
            res2 += n - i + r
        } else {
            res2 += r - i
        }
        return min(res1, res2)
    }
    return dfs(0, 0) + m
}
```

## 三、递归 + 记录返回值 = 记忆化搜索

上面的做法太慢了，怎么优化呢？

在整个递归过程中，其实有大量重复的递归调用。例如先往左再往右，和先往右再往左，到达的 $i$ 可能是一样的，这会导致递归入参相同。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(j,i)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

```py [sol-Python3]
class Solution:
    def findRotateSteps(self, s: str, t: str) -> int:
        s = [ord(c) - ord('a') for c in s]
        t = [ord(c) - ord('a') for c in t]
        n = len(s)

        # 先算出每个字母的最后一次出现的下标
        # 由于 s 是环形的，循环结束后的 pos 就刚好是 left[0]
        pos = [0] * 26  # 初始值不重要
        for i, c in enumerate(s):
            pos[c] = i
        # 计算每个 s[i] 左边 a-z 的最近下标（左边没有就从 n-1 往左找）
        left = [None] * n
        for i, c in enumerate(s):
            left[i] = pos[:]
            pos[c] = i  # 更新下标

        # 先算出每个字母的首次出现的下标
        # 由于 s 是环形的，循环结束后的 pos 就刚好是 right[n-1]
        for i in range(n - 1, -1, -1):
            pos[s[i]] = i
        # 计算每个 s[i] 右边 a-z 的最近下标（左边没有就从 0 往右找）
        right = [None] * n
        for i in range(n - 1, -1, -1):
            right[i] = pos[:]
            pos[s[i]] = i  # 更新下标

        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(j: int, i: int) -> int:
            if j == len(t):
                return 0
            c = t[j]
            if s[i] == c:  # 无需旋转
                return dfs(j + 1, i)
            # 左边最近 or 右边最近，取最小值
            l, r = left[i][c], right[i][c]
            return min(dfs(j + 1, l) + (n - l + i if l > i else i - l),
                       dfs(j + 1, r) + (n - i + r if r < i else r - i))

        return dfs(0, 0) + len(t)
```

```java [sol-Java]
class Solution {
    private char[] s, t;
    private int[][] left, right, memo;

    public int findRotateSteps(String S, String T) {
        s = S.toCharArray();
        t = T.toCharArray();
        int n = s.length;
        int m = t.length;

        // 先算出每个字母的最后一次出现的下标
        // 由于 s 是环形的，循环结束后的 pos 就刚好是 left[0]
        int[] pos = new int[26]; // 初始值不重要
        for (int i = 0; i < n; i++) {
            s[i] -= 'a';
            pos[s[i]] = i;
        }
        // 计算每个 s[i] 左边 a-z 的最近下标（左边没有就从 n-1 往左找）
        left = new int[n][26];
        for (int i = 0; i < n; i++) {
            System.arraycopy(pos, 0, left[i], 0, 26);
            pos[s[i]] = i; // 更新下标
        }

        // 先算出每个字母的首次出现的下标
        // 由于 s 是环形的，循环结束后的 pos 就刚好是 right[n-1]
        for (int i = n - 1; i >= 0; i--) {
            pos[s[i]] = i;
        }
        // 计算每个 s[i] 右边 a-z 的最近下标（左边没有就从 0 往右找）
        right = new int[n][26];
        for (int i = n - 1; i >= 0; i--) {
            System.arraycopy(pos, 0, right[i], 0, 26);
            pos[s[i]] = i; // 更新下标
        }

        memo = new int[m][n];
        for (int[] row : memo) {
            Arrays.fill(row, -1); // -1 表示没有计算过
        }
        return dfs(0, 0) + m;
    }

    private int dfs(int j, int i) {
        if (j == t.length) {
            return 0;
        }
        if (memo[j][i] != -1) { // 之前计算过
            return memo[j][i];
        }
        int c = t[j] - 'a';
        if (s[i] == c) { // 无需旋转
            return memo[j][i] = dfs(j + 1, i);
        }
        // 左边最近 or 右边最近，取最小值
        int l = left[i][c], r = right[i][c];
        return memo[j][i] = Math.min(dfs(j + 1, l) + (l > i ? s.length - l + i : i - l),
                                     dfs(j + 1, r) + (r < i ? s.length - i + r : r - i));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findRotateSteps(string s, string t) {
        int n = s.length(), m = t.length();

        // 先算出每个字母的最后一次出现的下标
        // 由于 s 是环形的，循环结束后的 pos 就刚好是 left[0]
        array<int, 26> pos; // 初始值不重要
        for (int i = 0; i < n; i++) {
            s[i] -= 'a';
            pos[s[i]] = i;
        }
        // 计算每个 s[i] 左边 a-z 的最近下标（左边没有就从 n-1 往左找）
        vector<array<int, 26>> left(n);
        for (int i = 0; i < n; i++) {
            left[i] = pos;
            pos[s[i]] = i; // 更新下标
        }

        // 先算出每个字母的首次出现的下标
        // 由于 s 是环形的，循环结束后的 pos 就刚好是 right[n-1]
        for (int i = n - 1; i >= 0; i--) {
            pos[s[i]] = i;
        }
        // 计算每个 s[i] 右边 a-z 的最近下标（左边没有就从 0 往右找）
        vector<array<int, 26>> right(n);
        for (int i = n - 1; i >= 0; i--) {
            right[i] = pos;
            pos[s[i]] = i; // 更新下标
        }

        vector<vector<int>> memo(m, vector<int>(n, -1)); // -1 表示没有计算过
        function<int(int, int)> dfs = [&](int j, int i) -> int {
            if (j == m) {
                return 0;
            }
            int &res = memo[j][i]; // 注意这里是引用
            if (res != -1) { // 之前计算过
                return res;
            }
            char c = t[j] - 'a';
            if (s[i] == c) { // 无需旋转
                return res = dfs(j + 1, i);
            }
            // 左边最近 or 右边最近，取最小值
            int l = left[i][c], r = right[i][c];
            return res = min(dfs(j + 1, l) + (l > i ? n - l + i : i - l),
                             dfs(j + 1, r) + (r < i ? n - i + r : r - i));
        };
        return dfs(0, 0) + m;
    }
};
```

```go [sol-Go]
func findRotateSteps(s, t string) int {
    n, m := len(s), len(t)

    // 先算出每个字母的最后一次出现的下标
    // 由于 s 是环形的，循环结束后的 pos 就刚好是 left[0]
    pos := [26]int{} // 初始值不重要
    for i, b := range s {
        pos[b-'a'] = i
    }
    // 计算每个 s[i] 左边 a-z 的最近下标（左边没有就从 n-1 往左找）
    left := make([][26]int, n)
    for i, b := range s {
        left[i] = pos
        pos[b-'a'] = i // 更新下标
    }

    // 先算出每个字母的首次出现的下标
    // 由于 s 是环形的，循环结束后的 pos 就刚好是 right[n-1]
    for i := n - 1; i >= 0; i-- {
        pos[s[i]-'a'] = i
    }
    // 计算每个 s[i] 右边 a-z 的最近下标（左边没有就从 0 往右找）
    right := make([][26]int, n)
    for i := n - 1; i >= 0; i-- {
        right[i] = pos
        pos[s[i]-'a'] = i // 更新下标
    }

    memo := make([][]int, m)
    for i := range memo {
        memo[i] = make([]int, n)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(j, i int) (res int) {
        if j == m {
            return 0
        }
        p := &memo[j][i]
        if *p != -1 { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if s[i] == t[j] { // 无需旋转
            return dfs(j+1, i)
        }
        // 左边最近 or 右边最近，取最小值
        l := left[i][t[j]-'a']
        res1 := dfs(j+1, l)
        if l > i {
            res1 += n - l + i
        } else {
            res1 += i - l
        }
        r := right[i][t[j]-'a']
        res2 := dfs(j+1, r)
        if r < i {
            res2 += n - i + r
        } else {
            res2 += r - i
        }
        return min(res1, res2)
    }
    return dfs(0, 0) + m
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n(|\Sigma|+m))$，其中 $n$ 为 $s$ 的长度，$m$ 为 $t$ 的长度，$|\Sigma|$ 为字符集合的大小，本题中字符均为小写字母，所以 $|\Sigma|=26$。预处理 $\textit{left}$ 和 $\textit{right}$ 需要 $\mathcal{O}(n|\Sigma|)$ 的时间。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(nm)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(nm)$。
- 空间复杂度：$\mathcal{O}(n(|\Sigma|+m))$。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[j][i]$ 的定义和 $\textit{dfs}(j,i)$ 的定义是一样的，都表示拼写后缀 $t[j]$ 到 $t[m-1]$，此时 12:00 方向为 $s[i]$ 时的最小旋转次数。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[j][i] = \min(f[j+1][l] + \textit{kl}, f[j+1][r] + \textit{kr})
$$

> 相当于之前是用递归去计算每个状态，现在是**枚举**并计算每个状态。

初始值 $f[m][i]=0$，翻译自递归边界 $\textit{dfs}(m,i)=0$。

答案为 $f[0][0]+m$，翻译自递归入口 $\textit{dfs}(0,0)$ 再加上 $t$ 的长度。

#### 答疑

**问**：如何思考循环顺序？什么时候要正序枚举，什么时候要倒序枚举？

**答**：这里有一个通用的做法：盯着状态转移方程，想一想，要计算 $f[j][i]$，必须先把 $f[j+1][\cdot]$ 算出来，那么只有 $j$ 从大到小枚举才能做到。

**问**：为什么递推的效率不如记忆化搜索？

**答**：注意到我们其实计算了很多「无效状态」，比如在 $s$ 中但不在 $t$ 中的字母我们也枚举了。记忆化搜索不会递归到这样的状态，计算的状态数更少。

```py [sol-Python3]
class Solution:
    def findRotateSteps(self, s: str, t: str) -> int:
        s = [ord(c) - ord('a') for c in s]
        t = [ord(c) - ord('a') for c in t]
        n, m = len(s), len(t)

        # 先算出每个字母的最后一次出现的下标
        # 由于 s 是环形的，循环结束后的 pos 就刚好是 left[0]
        pos = [0] * 26  # 初始值不重要
        for i, c in enumerate(s):
            pos[c] = i
        # 计算每个 s[i] 左边 a-z 的最近下标（左边没有就从 n-1 往左找）
        left = [None] * n
        for i, c in enumerate(s):
            left[i] = pos[:]
            pos[c] = i  # 更新下标

        # 先算出每个字母的首次出现的下标
        # 由于 s 是环形的，循环结束后的 pos 就刚好是 right[n-1]
        for i in range(n - 1, -1, -1):
            pos[s[i]] = i
        # 计算每个 s[i] 右边 a-z 的最近下标（左边没有就从 0 往右找）
        right = [None] * n
        for i in range(n - 1, -1, -1):
            right[i] = pos[:]
            pos[s[i]] = i  # 更新下标

        f = [[0] * n for _ in range(m + 1)]
        for j in range(m - 1, -1, -1):
            c = t[j]
            for i, x in enumerate(s):
                if x == c:  # 无需旋转
                    f[j][i] = f[j + 1][i]
                else:  # 左边最近 or 右边最近，取最小值
                    l, r = left[i][c], right[i][c]
                    f[j][i] = min(f[j + 1][l] + (n - l + i if l > i else i - l),
                                  f[j + 1][r] + (n - i + r if r < i else r - i))
        return f[0][0] + m
```

```java [sol-Java]
class Solution {
    public int findRotateSteps(String S, String T) {
        char[] s = S.toCharArray();
        char[] t = T.toCharArray();
        int n = s.length;
        int m = t.length;

        // 先算出每个字母的最后一次出现的下标
        // 由于 s 是环形的，循环结束后的 pos 就刚好是 left[0]
        int[] pos = new int[26]; // 初始值不重要
        for (int i = 0; i < n; i++) {
            s[i] -= 'a';
            pos[s[i]] = i;
        }
        // 计算每个 s[i] 左边 a-z 的最近下标（左边没有就从 n-1 往左找）
        int[][] left = new int[n][26];
        for (int i = 0; i < n; i++) {
            System.arraycopy(pos, 0, left[i], 0, 26);
            pos[s[i]] = i; // 更新下标
        }

        // 先算出每个字母的首次出现的下标
        // 由于 s 是环形的，循环结束后的 pos 就刚好是 right[n-1]
        for (int i = n - 1; i >= 0; i--) {
            pos[s[i]] = i;
        }
        // 计算每个 s[i] 右边 a-z 的最近下标（左边没有就从 0 往右找）
        int[][] right = new int[n][26];
        for (int i = n - 1; i >= 0; i--) {
            System.arraycopy(pos, 0, right[i], 0, 26);
            pos[s[i]] = i; // 更新下标
        }

        int[][] f = new int[m + 1][n];
        for (int j = m - 1; j >= 0; j--) {
            int c = t[j] - 'a';
            for (int i = 0; i < n; i++) {
                if (s[i] == c) { // 无需旋转
                    f[j][i] = f[j + 1][i];
                } else { // 左边最近 or 右边最近，取最小值
                    int l = left[i][c], r = right[i][c];
                    f[j][i] = Math.min(f[j + 1][l] + (l > i ? n - l + i : i - l),
                                       f[j + 1][r] + (r < i ? n - i + r : r - i));
                }
            }
        }
        return f[0][0] + m;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findRotateSteps(string s, string t) {
        int n = s.length(), m = t.length();

        // 先算出每个字母的最后一次出现的下标
        // 由于 s 是环形的，循环结束后的 pos 就刚好是 left[0]
        array<int, 26> pos; // 初始值不重要
        for (int i = 0; i < n; i++) {
            s[i] -= 'a';
            pos[s[i]] = i;
        }
        // 计算每个 s[i] 左边 a-z 的最近下标（左边没有就从 n-1 往左找）
        vector<array<int, 26>> left(n);
        for (int i = 0; i < n; i++) {
            left[i] = pos;
            pos[s[i]] = i; // 更新下标
        }

        // 先算出每个字母的首次出现的下标
        // 由于 s 是环形的，循环结束后的 pos 就刚好是 right[n-1]
        for (int i = n - 1; i >= 0; i--) {
            pos[s[i]] = i;
        }
        // 计算每个 s[i] 右边 a-z 的最近下标（左边没有就从 0 往右找）
        vector<array<int, 26>> right(n);
        for (int i = n - 1; i >= 0; i--) {
            right[i] = pos;
            pos[s[i]] = i; // 更新下标
        }

        vector<vector<int>> f(m + 1, vector<int>(n));
        for (int j = m - 1; j >= 0; j--) {
            char c = t[j] - 'a';
            for (int i = 0; i < n; i++) {
                if (s[i] == c) { // 无需旋转
                    f[j][i] = f[j + 1][i];
                } else { // 左边最近 or 右边最近，取最小值
                    int l = left[i][c], r = right[i][c];
                    f[j][i] = min(f[j + 1][l] + (l > i ? n - l + i : i - l),
                                  f[j + 1][r] + (r < i ? n - i + r : r - i));
                }
            }
        }
        return f[0][0] + m;
    }
};
```

```go [sol-Go]
func findRotateSteps(s, t string) int {
    n, m := len(s), len(t)

    // 先算出每个字母的最后一次出现的下标
    // 由于 s 是环形的，循环结束后的 pos 就刚好是 left[0]
    pos := [26]int{} // 初始值不重要
    for i, b := range s {
        pos[b-'a'] = i
    }
    // 计算每个 s[i] 左边 a-z 的最近下标（左边没有就从 n-1 往左找）
    left := make([][26]int, n)
    for i, b := range s {
        left[i] = pos
        pos[b-'a'] = i // 更新下标
    }

    // 先算出每个字母的首次出现的下标
    // 由于 s 是环形的，循环结束后的 pos 就刚好是 right[n-1]
    for i := n - 1; i >= 0; i-- {
        pos[s[i]-'a'] = i
    }
    // 计算每个 s[i] 右边 a-z 的最近下标（左边没有就从 0 往右找）
    right := make([][26]int, n)
    for i := n - 1; i >= 0; i-- {
        right[i] = pos
        pos[s[i]-'a'] = i // 更新下标
    }

    f := make([][]int, m+1)
    for i := range f {
        f[i] = make([]int, n)
    }
    for j := m - 1; j >= 0; j-- {
        for i := 0; i < n; i++ {
            if s[i] == t[j] { // 无需旋转
                f[j][i] = f[j+1][i]
                continue
            }
            // 左边最近 or 右边最近，取最小值
            l := left[i][t[j]-'a']
            res1 := f[j+1][l]
            if l > i {
                res1 += n - l + i
            } else {
                res1 += i - l
            }
            r := right[i][t[j]-'a']
            res2 := f[j+1][r]
            if r < i {
                res2 += n - i + r
            } else {
                res2 += r - i
            }
            f[j][i] = min(res1, res2)
        }
    }
    return f[0][0] + m
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n(|\Sigma|+m))$，其中 $n$ 为 $s$ 的长度，$m$ 为 $t$ 的长度，$|\Sigma|$ 为字符集合的大小，本题中字符均为小写字母，所以 $|\Sigma|=26$。预处理 $\textit{left}$ 和 $\textit{right}$ 需要 $\mathcal{O}(n|\Sigma|)$ 的时间。动态规划需要 $\mathcal{O}(nm)$ 的时间。
- 空间复杂度：$\mathcal{O}(n(|\Sigma|+m))$。利用滚动数组可以优化至 $\mathcal{O}(n|\Sigma|)$，具体请看基础算法精讲中的背包那期视频。

## 另一种解法：BFS

定义状态 $(i,j)$ 表示 12:00 方向为 $s[i]$，当前需要拼写 $t[j]$。

- 如果 $s[i]=t[j]$，拼写 $t[j]$，移动到状态 $(i,j+1)$。
- 否则向左移动到状态 $(i-1+n)\bmod n, j)$，或者向右移动到状态 $(i+1)\bmod n, j)$。

答案就是从 $(0,0)$ 到 $(i,m)$ 的最短路长度，由于每次移动都是 $1$ 步，可以用 BFS 解决。

```py [sol-Python3]
class Solution:
    def findRotateSteps(self, s: str, t: str) -> int:
        n, m = len(s), len(t)
        vis = [[False] * (m + 1) for _ in range(n)]
        vis[0][0] = True
        q = [(0, 0)]
        for step in count(0):  # 枚举 step=0,1,2,...
            tmp = q
            q = []
            for i, j in tmp:
                if j == m:
                    return step
                if s[i] == t[j]:
                    if not vis[i][j + 1]:
                        vis[i][j + 1] = True
                        q.append((i, j + 1))
                    continue
                for i2 in (i - 1) % n, (i + 1) % n:
                    if not vis[i2][j]:
                        vis[i2][j] = True
                        q.append((i2, j))
```

```java [sol-Java]
class Solution {
    public int findRotateSteps(String S, String T) {
        char[] s = S.toCharArray();
        char[] t = T.toCharArray();
        int n = s.length;
        int m = t.length;
        boolean[][] vis = new boolean[n][m + 1];
        vis[0][0] = true;
        List<int[]> q = List.of(new int[]{0, 0});
        for (int step = 0; ; step++) {
            List<int[]> tmp = q;
            q = new ArrayList<>();
            for (int[] p : tmp) {
                int i = p[0], j = p[1];
                if (j == m) {
                    return step;
                }
                if (s[i] == t[j]) {
                    if (!vis[i][j + 1]) {
                        vis[i][j + 1] = true;
                        q.add(new int[]{i, j + 1});
                    }
                    continue;
                }
                for (int i2 : new int[]{(i - 1 + n) % n, (i + 1) % n}) {
                    if (!vis[i2][j]) {
                        vis[i2][j] = true;
                        q.add(new int[]{i2, j});
                    }
                }
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findRotateSteps(string s, string t) {
        int n = s.length(), m = t.length();
        vector<vector<int>> vis(n, vector<int>(m + 1));
        vis[0][0] = true;
        vector<pair<int, int>> q = {{0, 0}};
        for (int step = 0; ; step++) {
            vector<pair<int, int>> nxt;
            for (auto [i, j] : q) {
                if (j == m) {
                    return step;
                }
                if (s[i] == t[j]) {
                    if (!vis[i][j + 1]) {
                        vis[i][j + 1] = true;
                        nxt.emplace_back(i, j + 1);
                    }
                    continue;
                }
                for (int i2 : {(i - 1 + n) % n, (i + 1) % n}) {
                    if (!vis[i2][j]) {
                        vis[i2][j] = true;
                        nxt.emplace_back(i2, j);
                    }
                }
            }
            q = move(nxt);
        }
    }
};
```

```go [sol-Go]
func findRotateSteps(s, t string) int {
    n, m := len(s), len(t)
    vis := make([][]bool, n)
    for i := range vis {
        vis[i] = make([]bool, m+1)
    }
    vis[0][0] = true
    type pair struct{ i, j int }
    q := []pair{{0, 0}}
    for step := 0; ; step++ {
        tmp := q
        q = nil
        for _, p := range tmp {
            i, j := p.i, p.j
            if j == m {
                return step
            }
            if s[i] == t[j] {
                if !vis[i][j+1] {
                    vis[i][j+1] = true
                    q = append(q, pair{i, j + 1})
                }
                continue
            }
            for _, i2 := range []int{(i - 1 + n) % n, (i + 1) % n} {
                if !vis[i2][j] {
                    vis[i2][j] = true
                    q = append(q, pair{i2, j})
                }
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nm)$，其中 $n$ 为 $s$ 的长度，$m$ 为 $t$ 的长度。有 $\mathcal{O}(nm)$ 个状态，每个状态至多访问一次。
- 空间复杂度：$\mathcal{O}(nm)$。

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

本题来自 `一、图的遍历 / §1.3 图论建模 + BFS 最短路`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、图的遍历 / §1.3 图论建模 + BFS 最短路`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
