# 1000. 合并石头的最低成本

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-to-merge-stones/
- 题目 slug：`minimum-cost-to-merge-stones`
- 来源专题：动态规划
- 来源分类路径：八、区间 DP / §8.2 区间 DP
- 难度分：2423
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-to-merge-stones/solutions/2207235/tu-jie-qu-jian-dpzhuang-tai-she-ji-yu-yo-ppv0/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[【图解】区间 DP：状态设计与优化（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-cost-to-merge-stones/solutions/2207235/tu-jie-qu-jian-dpzhuang-tai-she-ji-yu-yo-ppv0/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tu-jie-qu-jian-dpzhuang-tai-she-ji-yu-yo-ppv0`
- topic id：`2207235`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识

1. [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)
2. [区间 DP【基础算法精讲 22】](https://www.bilibili.com/video/BV1Gs4y1E7EU/)
3. [前缀和原理](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)

## 思路（优化前）

![1000-3d-cut.png](https://pic.leetcode.cn/1680534488-qZHfMY-1000-3d-cut.png)

什么时候输出 $-1$ 呢？

从 $n$ 堆变成 $1$ 堆，需要减少 $n-1$ 堆。而每次合并都会减少 $k-1$ 堆，所以 $n-1$ 必须是 $k-1$ 的倍数。

代码实现时，由于整个递归中有大量重复递归调用（递归入参相同），且递归函数没有副作用（同样的入参无论计算多少次，算出来的结果都是一样的），因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组（或哈希表）中。
- 如果一个状态不是第一次遇到，那么直接返回 $\textit{memo}$ 中保存的结果。

### 答疑

**问**：为什么只考虑分出 $1$ 堆和 $p-1$ 堆，而不考虑分出 $x$ 堆和 $p-x$ 堆？

**答**：无需计算，因为 $p-1$ 堆继续递归又可以分出 $1$ 堆和 $p-2$ 堆，和之前分出的 $1$ 堆组合，就已经能表达出「分出 $2$ 堆和 $p-2$ 堆」的情况了。其他同理。所以只需要考虑分出 $1$ 堆和 $p-1$ 堆。

```py [sol1-Python3]
class Solution:
    def mergeStones(self, stones: List[int], k: int) -> int:
        n = len(stones)
        if (n - 1) % (k - 1):  # 无法合并成一堆
            return -1
        s = list(accumulate(stones, initial=0))  # 前缀和
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int, j: int, p: int) -> int:
            if p == 1:  # 合并成一堆
                return 0 if i == j else dfs(i, j, k) + s[j + 1] - s[i]
            return min(dfs(i, m, 1) + dfs(m + 1, j, p - 1) for m in range(i, j, k - 1))
        return dfs(0, n - 1, 1)
```

```java [sol1-Java]
class Solution {
    private int[][][] memo;
    private int[] s;
    private int k;

    public int mergeStones(int[] stones, int k) {
        int n = stones.length;
        if ((n - 1) % (k - 1) > 0) // 无法合并成一堆
            return -1;

        s = new int[n + 1];
        for (int i = 0; i < n; i++)
            s[i + 1] = s[i] + stones[i]; // 前缀和
        this.k = k;
        memo = new int[n][n][k + 1];
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                Arrays.fill(memo[i][j], -1); // -1 表示还没有计算过
        return dfs(0, n - 1, 1);
    }

    private int dfs(int i, int j, int p) {
        if (memo[i][j][p] != -1) return memo[i][j][p];
        if (p == 1) // 合并成一堆
            return memo[i][j][p] = i == j ? 0 : dfs(i, j, k) + s[j + 1] - s[i];
        int res = Integer.MAX_VALUE;
        for (int m = i; m < j; m += k - 1) // 枚举哪些石头堆合并成第一堆
            res = Math.min(res, dfs(i, m, 1) + dfs(m + 1, j, p - 1));
        return memo[i][j][p] = res;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int mergeStones(vector<int> &stones, int k) {
        int n = stones.size();
        if ((n - 1) % (k - 1)) // 无法合并成一堆
            return -1;

        int s[n + 1];
        s[0] = 0;
        for (int i = 0; i < n; i++)
            s[i + 1] = s[i] + stones[i]; // 前缀和

        int memo[n][n][k + 1];
        memset(memo, -1, sizeof(memo)); // -1 表示还没有计算过
        function<int(int, int, int)> dfs = [&](int i, int j, int p) -> int {
            int &res = memo[i][j][p]; // 注意这里是引用，下面会直接修改 memo[i][j][p]
            if (res != -1) return res;
            if (p == 1) // 合并成一堆
                return res = i == j ? 0 : dfs(i, j, k) + s[j + 1] - s[i];
            res = INT_MAX;
            for (int m = i; m < j; m += k - 1) // 枚举哪些石头堆合并成第一堆
                res = min(res, dfs(i, m, 1) + dfs(m + 1, j, p - 1));
            return res;
        };
        return dfs(0, n - 1, 1);
    }
};
```

```go [sol1-Go]
func mergeStones(stones []int, k int) int {
    n := len(stones)
    if (n-1)%(k-1) != 0 { // 无法合并成一堆
        return -1
    }

    s := make([]int, n+1)
    for i, x := range stones {
        s[i+1] = s[i] + x // 前缀和
    }

    memo := make([][][]int, n)
    for i := range memo {
        memo[i] = make([][]int, n)
        for j := range memo[i] {
            memo[i][j] = make([]int, k+1)
            for p := range memo[i][j] {
                memo[i][j][p] = -1 // -1 表示还没有计算过
            }
        }
    }
    var dfs func(int, int, int) int
    dfs = func(i, j, p int) (res int) {
        ptr := &memo[i][j][p]
        if *ptr != -1 {
            return *ptr
        }
        defer func() { *ptr = res }()
        if p == 1 {
            if i == j { // 只有一堆石头，无需合并
                return
            }
            return dfs(i, j, k) + s[j+1] - s[i]
        }
        res = math.MaxInt
        for m := i; m < j; m += k - 1 { // 枚举哪些石头堆合并成第一堆
            res = min(res, dfs(i, m, 1)+dfs(m+1, j, p-1))
        }
        return
    }
    return dfs(0, n-1, 1)
}
```

#### 复杂度分析

- 时间复杂度：$O(n^3)$，其中 $n$ 为 $\textit{stones}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。这里状态个数为 $O(n^2k)$，单个状态的计算时间为 $O\left(\dfrac{n}{k}\right)$，因此时间复杂度为 $O(n^3)$。
- 空间复杂度：$O(n^2k)$。

![1000-2d.png](https://pic.leetcode.cn/1680534839-WSibYe-1000-2d.png)

```py [sol2-Python3]
class Solution:
    def mergeStones(self, stones: List[int], k: int) -> int:
        n = len(stones)
        if (n - 1) % (k - 1):  # 无法合并成一堆
            return -1
        s = list(accumulate(stones, initial=0))  # 前缀和
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int, j: int) -> int:
            if i == j:  # 只有一堆石头，无需合并
                return 0
            res = min(dfs(i, m) + dfs(m + 1, j) for m in range(i, j, k - 1))
            if (j - i) % (k - 1) == 0:  # 可以合并成一堆
                res += s[j + 1] - s[i]
            return res
        return dfs(0, n - 1)
```

```java [sol2-Java]
class Solution {
    private int[][] memo;
    private int[] s;
    private int k;

    public int mergeStones(int[] stones, int k) {
        int n = stones.length;
        if ((n - 1) % (k - 1) > 0) // 无法合并成一堆
            return -1;

        s = new int[n + 1];
        for (int i = 0; i < n; i++)
            s[i + 1] = s[i] + stones[i]; // 前缀和
        this.k = k;
        memo = new int[n][n];
        for (int i = 0; i < n; ++i)
            Arrays.fill(memo[i], -1); // -1 表示还没有计算过
        return dfs(0, n - 1);
    }

    private int dfs(int i, int j) {
        if (i == j) return 0; // 只有一堆石头，无需合并
        if (memo[i][j] != -1) return memo[i][j];
        int res = Integer.MAX_VALUE;
        for (int m = i; m < j; m += k - 1)
            res = Math.min(res, dfs(i, m) + dfs(m + 1, j));
        if ((j - i) % (k - 1) == 0) // 可以合并成一堆
            res += s[j + 1] - s[i];
        return memo[i][j] = res;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int mergeStones(vector<int> &stones, int k) {
        int n = stones.size();
        if ((n - 1) % (k - 1)) // 无法合并成一堆
            return -1;

        int s[n + 1];
        s[0] = 0;
        for (int i = 0; i < n; i++)
            s[i + 1] = s[i] + stones[i]; // 前缀和

        int memo[n][n];
        memset(memo, -1, sizeof(memo)); // -1 表示还没有计算过
        function<int(int, int)> dfs = [&](int i, int j) -> int {
            if (i == j) return 0; // 只有一堆石头，无需合并
            int &res = memo[i][j]; // 注意这里是引用，下面会直接修改 memo[i][j]
            if (res != -1) return res;
            res = INT_MAX;
            for (int m = i; m < j; m += k - 1)
                res = min(res, dfs(i, m) + dfs(m + 1, j));
            if ((j - i) % (k - 1) == 0) // 可以合并成一堆
                res += s[j + 1] - s[i];
            return res;
        };
        return dfs(0, n - 1);
    }
};
```

```go [sol2-Go]
func mergeStones(stones []int, k int) int {
    n := len(stones)
    if (n-1)%(k-1) != 0 {
        return -1
    }

    s := make([]int, n+1)
    for i, x := range stones {
        s[i+1] = s[i] + x // 前缀和
    }

    memo := make([][]int, n)
    for i := range memo {
        memo[i] = make([]int, n)
        for j := range memo[i] {
            memo[i][j] = -1 // -1 表示还没有计算过
        }
    }
    var dfs func(int, int) int
    dfs = func(i, j int) (res int) {
        if i == j { // 只有一堆石头，无需合并
            return
        }
        ptr := &memo[i][j]
        if *ptr != -1 {
            return *ptr
        }
        defer func() { *ptr = res }()
        res = math.MaxInt
        for m := i; m < j; m += k - 1 {
            res = min(res, dfs(i, m)+dfs(m+1, j))
        }
        if (j-i)%(k-1) == 0 { // 可以合并成一堆
            res += s[j+1] - s[i]
        }
        return
    }
    return dfs(0, n-1)
}
```

## 1:1 翻译成递推

把 $\textit{dfs}$ 改成 $f$ 数组，把递归改成循环就好了。相当于原来是用递归计算每个状态 $(i,j)$，现在改用循环去计算每个状态 $(i,j)$。

需要注意循环的顺序：

- 由于 $i<m+1$，$f[i]$ 要能从 $f[m+1]$ 转移过来，必须先计算出 $f[m+1]$，所以 $i$ 要**倒序**枚举；
- 由于 $j>m$，$f[i][j]$ 要能从 $f[i][m]$ 转移过来，必须先计算出 $f[i][m]$，所以 $j$ 要**正序**枚举。

```py [sol3-Python3]
class Solution:
    def mergeStones(self, stones: List[int], k: int) -> int:
        n = len(stones)
        if (n - 1) % (k - 1):  # 无法合并成一堆
            return -1
        s = list(accumulate(stones, initial=0))  # 前缀和
        f = [[0] * n for _ in range(n)]
        for i in range(n - 1, -1, -1):
            for j in range(i + 1, n):
                f[i][j] = min(f[i][m] + f[m + 1][j] for m in range(i, j, k - 1))
                if (j - i) % (k - 1) == 0:  # 可以合并成一堆
                    f[i][j] += s[j + 1] - s[i]
        return f[0][-1]
```

```java [sol3-Java]
class Solution {
    public int mergeStones(int[] stones, int k) {
        int n = stones.length;
        if ((n - 1) % (k - 1) > 0) // 无法合并成一堆
            return -1;

        var s = new int[n + 1];
        for (int i = 0; i < n; i++)
            s[i + 1] = s[i] + stones[i]; // 前缀和

        var f = new int[n][n];
        for (int i = n - 1; i >= 0; --i)
            for (int j = i + 1; j < n; ++j) {
                f[i][j] = Integer.MAX_VALUE;
                for (int m = i; m < j; m += k - 1)
                    f[i][j] = Math.min(f[i][j], f[i][m] + f[m + 1][j]);
                if ((j - i) % (k - 1) == 0) // 可以合并成一堆
                    f[i][j] += s[j + 1] - s[i];
            }
        return f[0][n - 1];
    }
}
```

```cpp [sol3-C++]
class Solution {
public:
    int mergeStones(vector<int> &stones, int k) {
        int n = stones.size();
        if ((n - 1) % (k - 1)) // 无法合并成一堆
            return -1;

        int s[n + 1];
        s[0] = 0;
        for (int i = 0; i < n; i++)
            s[i + 1] = s[i] + stones[i]; // 前缀和

        int f[n][n];
        for (int i = n - 1; i >= 0; --i) {
            f[i][i] = 0;
            for (int j = i + 1; j < n; ++j) {
                f[i][j] = INT_MAX;
                for (int m = i; m < j; m += k - 1)
                    f[i][j] = min(f[i][j], f[i][m] + f[m + 1][j]);
                if ((j - i) % (k - 1) == 0) // 可以合并成一堆
                    f[i][j] += s[j + 1] - s[i];
            }
        }
        return f[0][n - 1];
    }
};
```

```go [sol3-Go]
func mergeStones(stones []int, k int) int {
    n := len(stones)
    if (n-1)%(k-1) > 0 { // 无法合并成一堆
        return -1
    }

    s := make([]int, n+1)
    for i, x := range stones {
        s[i+1] = s[i] + x // 前缀和
    }

    f := make([][]int, n)
    for i := n - 1; i >= 0; i-- {
        f[i] = make([]int, n)
        for j := i + 1; j < n; j++ {
            f[i][j] = math.MaxInt
            for m := i; m < j; m += k - 1 {
                f[i][j] = min(f[i][j], f[i][m]+f[m+1][j])
            }
            if (j-i)%(k-1) == 0 { // 可以合并成一堆
                f[i][j] += s[j+1] - s[i]
            }
        }
    }
    return f[0][n-1]
}
```

#### 复杂度分析

- 时间复杂度：$O\left(\dfrac{n^3}{k}\right)$，其中 $n$ 为 $\textit{stones}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。这里状态个数为 $O(n^2)$，单个状态的计算时间为 $O\left(\dfrac{n}{k}\right)$，因此时间复杂度为 $O\left(\dfrac{n^3}{k}\right)$。
- 空间复杂度：$O(n^2)$。

## 分类题单

以下题单没有特定的顺序，可以按照个人喜好刷题。

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心算法（基本贪心策略/反悔/区间/字典序/数学/思维/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)

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
