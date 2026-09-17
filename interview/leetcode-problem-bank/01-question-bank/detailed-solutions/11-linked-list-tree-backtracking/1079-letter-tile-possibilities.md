# 1079. 活字印刷

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/letter-tile-possibilities/
- 题目 slug：`letter-tile-possibilities`
- 来源专题：链表、树与回溯
- 来源分类路径：四、回溯 / §4.6 有重复元素的回溯
- 难度分：1741
- 外部题解来源：https://leetcode.cn/problems/letter-tile-possibilities/solutions/2275356/on2-ji-shu-dppythonjavacgo-by-endlessche-hmez/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n^2) 计数 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/letter-tile-possibilities/solutions/2275356/on2-ji-shu-dppythonjavacgo-by-endlessche-hmez/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`on2-ji-shu-dppythonjavacgo-by-endlessche-hmez`
- topic id：`2275356`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:57:36 +0800

## 寻找子问题

以 $\textit{tiles}=\texttt{AABCC}$ 为例。先来思考，如何计算长为 $5$ 的序列的数目？由于相同字母不作区分，先考虑 $2$ 个 $\texttt{C}$ 如何放置。

这等价于在 $5$ 个位置中选 $2$ 个位置放 $\texttt{C}$，其余位置放 $\texttt{AAB}$。这 $2$ 个 $\texttt{C}$ 有 $\dbinom 5 2=10$ 种放法。剩余要解决的问题为，用 $\texttt{AAB}$ 构造长为 $3$ 的序列的数目。这是一个与原问题相似，且规模更小的子问题。

> 注：$\dbinom n k$ 表示从 $n$ 个数中选 $k$ 个数的方案数，即 $\dfrac{n!}{k!(n-k)!}$。

## 状态定义与转移

根据上面的讨论，定义 $f[i][j]$ 表示用前 $i$ 种字符构造长为 $j$ 的序列的方案数。 

设第 $i$ 种字符有 $\textit{cnt}$ 个：

- 如果一个也不选，那么 $f[i][j] = f[i-1][j]$。
- 如果从中选 $k$ 个，那么需要从 $j$ 个位置中选 $k$ 个放第 $i$ 种字符，其余位置就是用前 $i-1$ 种字符构造长为 $j-k$ 的序列的方案数，所以有 $f[i][j] =f[i-1][j-k]\cdot \dbinom j k$。这里 $k\le\min(j,\textit{cnt})$。特别地，一个也不选相当于 $k=0$ 的情况。

所以，枚举 $k=0,1,\ldots,\min(j,\textit{cnt})$，把所有方案数相加，就得到了 $f[i][j]$，对应的状态转移方程为

$$
f[i][j] = \sum_{k=0}^{\min(j,\textit{cnt})} f[i-1][j-k]\cdot \binom j k
$$

初始值：$f[0][0]=1$，构造空序列的方案数为 $1$。

答案：$\sum\limits_{j=1}^{n}f[m][j]$，其中 $m$ 为 $\textit{tiles}$ 中的字母种数。

代码实现时，组合数可以用如下恒等式预处理

$$
\binom n k = \binom {n-1} {k-1} + \binom {n-1} k
$$

这个式子本质是考虑第 $n$ 个数「选或不选」。如果选，那么问题变成从 $n-1$ 个数中选 $k-1$ 个数的方案数；如果不选，那么问题变成从 $n-1$ 个数中选 $k$ 个数的方案数。二者相加即为从 $n$ 个数中选 $k$ 个数的方案数。

```py [sol1-Python3]
class Solution:
    def numTilePossibilities(self, tiles: str) -> int:
        counts = Counter(tiles).values()  # 统计每个字母的出现次数
        n, m = len(tiles), len(counts)
        f = [[0] * (n + 1) for _ in range(m + 1)]
        f[0][0] = 1  # 构造空序列的方案数
        for i, cnt in enumerate(counts, 1):  # 枚举第 i 种字母
            for j in range(n + 1):  # 枚举序列长度 j
                for k in range(min(j, cnt) + 1):  # 枚举第 i 种字母选了 k 个
                    f[i][j] += f[i - 1][j - k] * comb(j, k)  # comb 也可以预处理，见其它语言
        return sum(f[m][1:])
```

```java [sol1-Java]
class Solution {
    private static final int MX = 8;
    private static final int[][] c = new int[MX][MX];

    static {
        for (int i = 0; i < MX; i++) {
            c[i][0] = c[i][i] = 1;
            for (int j = 1; j < i; j++)
                c[i][j] = c[i - 1][j - 1] + c[i - 1][j]; // 预处理组合数
        }
    }

    public int numTilePossibilities(String tiles) {
        var counts = new HashMap<Character, Integer>(); // 统计每个字母的出现次数
        for (var c : tiles.toCharArray())
            counts.merge(c, 1, Integer::sum); // counts[c]++
        int m = counts.size(), n = tiles.length();
        var f = new int[m + 1][n + 1];
        f[0][0] = 1; // 构造空序列的方案数
        int i = 1;
        for (var cnt : counts.values()) { // 枚举第 i 种字母
            for (int j = 0; j <= n; j++) // 枚举序列长度 j
                for (int k = 0; k <= j && k <= cnt; k++) // 枚举第 i 种字母选了 k 个
                    f[i][j] += f[i - 1][j - k] * c[j][k];
            i++;
        }
        int ans = 0;
        for (int j = 1; j <= n; j++)
            ans += f[m][j];
        return ans;
    }
}
```

```cpp [sol1-C++]
const int MX = 8;
int c[MX][MX];

int init = []() {
    for (int i = 0; i < MX; i++) {
        c[i][0] = c[i][i] = 1;
        for (int j = 1; j < i; j++)
            c[i][j] = c[i - 1][j - 1] + c[i - 1][j]; // 预处理组合数
    }
    return 0;
}();

class Solution {
public:
    int numTilePossibilities(string tiles) {
        unordered_map<char, int> counts;
        for (char c: tiles) counts[c]++; // 统计每个字母的出现次数
        int n = tiles.length(), m = counts.size(), f[m + 1][n + 1];
        memset(f, 0, sizeof(f));
        f[0][0] = 1; // 构造空序列的方案数
        int i = 1;
        for (auto &[_, cnt]: counts) { // 枚举第 i 种字母
            for (int j = 0; j <= n; j++) // 枚举序列长度 j
                for (int k = 0; k <= j && k <= cnt; k++) // 枚举第 i 种字母选了 k 个
                    f[i][j] += f[i - 1][j - k] * c[j][k];
            i++;
        }
        return accumulate(f[m] + 1, f[m] + n + 1, 0);
    }
};
```

```go [sol1-Go]
const mx = 8
var c [mx][mx]int

func init() {
    for i := 0; i < mx; i++ {
        c[i][0], c[i][i] = 1, 1
        for j := 1; j < i; j++ {
            c[i][j] = c[i-1][j-1] + c[i-1][j] // 预处理组合数
        }
    }
}

func numTilePossibilities(tiles string) (ans int) {
    counts := map[rune]int{}
    for _, ch := range tiles {
        counts[ch]++ // 统计每个字母的出现次数
    }
    n, m := len(tiles), len(counts)
    f := make([][]int, m+1)
    f[0] = make([]int, n+1)
    f[0][0] = 1 // 构造空序列的方案数
    i := 1
    for _, cnt := range counts { // 枚举第 i 种字母
        f[i] = make([]int, n+1)
        for j := 0; j <= n; j++ { // 枚举序列长度 j
            for k := 0; k <= j && k <= cnt; k++ { // 枚举第 i 种字母选了 k 个
                f[i][j] += f[i-1][j-k] * c[j][k]
            }
        }
        i++
    }
    for _, x := range f[m][1:] {
        ans += x
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{tiles}$ 的长度。虽然写了个三重循环，但换个角度，对于一个固定的 $j$，最内层的循环次数之和，约为所有字母的出现次数之和，即 $\mathcal{O}(n)$。相当于最外层和最内层合起来是一个 $\mathcal{O}(n)$ 的循环。所以三重循环的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n^2)$。忽略预处理组合数的时间和空间。

> 注：也可以只预处理阶乘，用公式计算组合数。

## 空间优化

视频讲解见 [背包问题](https://www.bilibili.com/video/BV16Y411v7Y6/) 中的的空间优化。

由于 $f[i]$ 只从 $f[i-1]$ 转移过来，我们可以去掉第一个维度，只用一个一维数组。

和 0-1 背包问题一样，如果 $j$ 从小到大遍历，那么 $f[i-1][j]$ 保存的数据会被 $f[i][j]$ 覆盖，但是计算右边的 $f[i][j']$ 时，又需要 $f[i-1][j]$。倒序遍历 $j$ 即可解决此问题。

此外，可以累加 $\textit{cnt}$，记作 $n$，作为第二层循环的初始值，因为就算全部都选，前 $i$ 种字母的长度之和也不会超过 $n$，计算比 $n$ 更大的状态是没有意义的。

```py [sol-Python3]
class Solution:
    def numTilePossibilities(self, tiles: str) -> int:
        f = [1] + [0] * len(tiles)
        n = 0
        for cnt in Counter(tiles).values():  # 枚举第 i 种字母
            n += cnt  # 常数优化：相比从 len(tiles) 开始要更快
            for j in range(n, 0, -1):  # 枚举序列长度 j
                # 枚举第 i 种字母选了 k 个，注意 k=0 时的方案数已经在 f[j] 中了
                for k in range(1, min(j, cnt) + 1):
                    f[j] += f[j - k] * comb(j, k)  # comb 也可以预处理，见其它语言的实现
        return sum(f[1:])
```

```java [sol-Java]
class Solution {
    private static final int MX = 8;
    private static final int[][] c = new int[MX][MX];

    static {
        for (int i = 0; i < MX; i++) {
            c[i][0] = c[i][i] = 1;
            for (int j = 1; j < i; j++)
                c[i][j] = c[i - 1][j - 1] + c[i - 1][j]; // 预处理组合数
        }
    }

    public int numTilePossibilities(String tiles) {
        // 注：改成 int[26] 统计可能会快一点点，感兴趣可以试试（下面 DP 跳过 cnt=0 的情况）
        var counts = new HashMap<Character, Integer>(); // 统计每个字母的出现次数
        for (var c : tiles.toCharArray())
            counts.merge(c, 1, Integer::sum); // counts[c]++
        var f = new int[tiles.length() + 1];
        f[0] = 1; // 构造空序列的方案数
        int n = 0;
        for (var cnt : counts.values()) { // 枚举第 i 种字母
            n += cnt; // 常数优化：相比从 tiles.length() 开始要更快
            for (int j = n; j > 0; j--) // 枚举序列长度 j
                // 枚举第 i 种字母选了 k 个，注意 k=0 时的方案数已经在 f[j] 中了
                for (int k = 1; k <= j && k <= cnt; k++)
                    f[j] += f[j - k] * c[j][k];
        }
        int ans = 0;
        for (int j = 1; j <= n; j++)
            ans += f[j];
        return ans;
    }
}
```

```cpp [sol-C++]
const int MX = 8;
int c[MX][MX];

int init = []() {
    for (int i = 0; i < MX; i++) {
        c[i][0] = c[i][i] = 1;
        for (int j = 1; j < i; j++)
            c[i][j] = c[i - 1][j - 1] + c[i - 1][j]; // 预处理组合数
    }
    return 0;
}();

class Solution {
public:
    int numTilePossibilities(string tiles) {
        unordered_map<char, int> counts;
        for (char c: tiles) counts[c]++; // 统计每个字母的出现次数
        int f[tiles.size() + 1];
        memset(f, 0, sizeof(f));
        f[0] = 1; // 构造空序列的方案数
        int n = 0;
        for (auto &[_, cnt]: counts) { // 枚举第 i 种字母
            n += cnt; // 常数优化：相比从 tiles.length() 开始要更快
            for (int j = n; j; j--) // 枚举序列长度 j
                // 枚举第 i 种字母选了 k 个，注意 k=0 时的方案数已经在 f[j] 中了
                for (int k = 1; k <= j && k <= cnt; k++)
                    f[j] += f[j - k] * c[j][k];
        }
        return accumulate(f + 1, f + n + 1, 0);
    }
};
```

```go [sol-Go]
const mx = 8
var c [mx][mx]int

func init() {
    for i := 0; i < mx; i++ {
        c[i][0], c[i][i] = 1, 1
        for j := 1; j < i; j++ {
            c[i][j] = c[i-1][j-1] + c[i-1][j] // 预处理组合数
        }
    }
}

func numTilePossibilities(tiles string) (ans int) {
    counts := map[rune]int{}
    for _, ch := range tiles {
        counts[ch]++ // 统计每个字母的出现次数
    }
    f := make([]int, len(tiles)+1)
    f[0] = 1 // 构造空序列的方案数
    n := 0
    for _, cnt := range counts { // 枚举第 i 种字母
        n += cnt // 常数优化：相比从 tiles.length() 开始要更快
        for j := n; j > 0; j-- { // 枚举序列长度 j
            // 枚举第 i 种字母选了 k 个，注意 k=0 时的方案数已经在 f[j] 中了
            for k := 1; k <= j && k <= cnt; k++ {
                f[j] += f[j-k] * c[j][k]
            }
        }
    }
    for _, x := range f[1:] {
        ans += x
    }
    return
}
```

#### 复杂度分析

不计入预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{tiles}$ 的长度。虽然写了个三重循环，但换个角度，对于一个固定的 $j$，最内层的循环次数之和，约为所有字母的出现次数之和，即 $\mathcal{O}(n)$。相当于最外层和最内层合起来是一个 $\mathcal{O}(n)$ 的循环。所以三重循环的时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n)$。

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

本题来自 `四、回溯 / §4.6 有重复元素的回溯`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、回溯 / §4.6 有重复元素的回溯`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
