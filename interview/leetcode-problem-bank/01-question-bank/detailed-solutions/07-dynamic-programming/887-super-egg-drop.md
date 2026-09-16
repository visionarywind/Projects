# 887. 鸡蛋掉落

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/super-egg-drop/
- 题目 slug：`super-egg-drop`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.6 多维 DP
- 难度分：2377
- 外部题解来源：https://leetcode.cn/problems/super-egg-drop/solutions/2949779/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-xkbx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推到空间优化！（Python/Java/C++/Go）](https://leetcode.cn/problems/super-egg-drop/solutions/2949779/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-xkbx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-xkbx`
- topic id：`2949779`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前言

本文接着 [1884. 鸡蛋掉落-两枚鸡蛋题解](https://leetcode.cn/problems/egg-drop-with-2-eggs-and-n-floors/solutions/2945577/liang-chong-fang-fa-dong-tai-gui-hua-shu-hd4i/) 的方法二（数学做法）继续讲，请先看这篇题解，相当于本题 $k=2$ 的简单版本。

## 一、寻找子问题

反过来，如果已知答案（操作次数），$n$ 最大可以是多少？

### 第一个例子

假设我们有 $i=5$ 次操作机会，有 $j=2$ 枚鸡蛋。

第一次操作，你打算在几楼扔第一枚鸡蛋？

你可能会想：搏一搏，单车变摩托。楼层越高越好嘛，如果鸡蛋没碎，这对我们会更有利。

最高可以在几楼扔？可以在 $6$ 楼扔吗？

不能。万一~~赌错~~鸡蛋碎了，后面就只能依次在 $1,2,3,4,5$ 楼扔第二枚鸡蛋，最坏情况下，总共要操作 $6$ 次。

所以第一次操作的最优做法，是在 $5$ 楼扔第一枚鸡蛋：

- 即使鸡蛋碎了，后续也只需要依次在 $1,2,3,4$ 楼扔第二枚鸡蛋，最坏情况下，总共要操作 $1+4=5$ 次。注意这是一个 $i=4,\ j=1$ 的子问题。
- 如果鸡蛋没碎，现在还剩下 $4$ 次扔蛋机会。问题变成 $i=4,\ j=2$ 的子问题。这个子问题的答案（等于 $10$），加上 $5$，就是原问题的答案（等于 $15$）。关于这里 $10$ 和 $15$ 的计算，请看 1884 题的 [题解](https://leetcode.cn/problems/egg-drop-with-2-eggs-and-n-floors/solutions/2945577/liang-chong-fang-fa-dong-tai-gui-hua-shu-hd4i/) 方法二。

请记住，$i=5,\ j=2$ 对应的最大的 $n$ 是 $15$。

### 第二个例子

假设我们有 $i=6$ 次操作机会，有 $j=3$ 枚鸡蛋。

第一次操作，你打算在几楼扔第一枚鸡蛋？

可以在 $17$ 楼扔吗？

不能。如果鸡蛋碎了，问题就变成 $i=5,\ j=2$ 的子问题（答案等于 $15$），我们无法确定（题干中的）$f$ 的值是否等于 $16$。

所以第一次操作的最优做法，是在 $16$ 楼扔第一枚鸡蛋：

- 如果鸡蛋碎了，问题变成 $i=5,\ j=2$ 的子问题（答案等于 $15$），由于只需要在 $[1,15]$ 中扔鸡蛋，所以可以确定 $f$ 的值。
- 如果鸡蛋没碎，问题变成 $i=5,\ j=3$ 的子问题。这个子问题的答案，加上 $16$，就是原问题的答案。

由于这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

## 二、状态定义与状态转移方程

根据上面的讨论，定义状态为 $\textit{dfs}(i,j)$，表示在有 $i$ 次操作机会和 $j$ 枚鸡蛋的情况下，可以让我们能确定 $f$ 值的最大建筑层数。

在 $\textit{dfs}(i-1,j-1)+1$ 楼扔第一枚鸡蛋：

- 如果鸡蛋碎了，接下来只需要在 $[1,\textit{dfs}(i-1,j-1)]$ 中扔鸡蛋，就可以确定 $f$ 的值。
- 如果鸡蛋没碎，问题变成在有 $i-1$ 次操作机会和 $j$ 枚鸡蛋的情况下，可以让我们能确定 $f$ 值的最大建筑层数。这个子问题的答案 $\textit{dfs}(i-1,j)$，加上 $\textit{dfs}(i-1,j-1)+1$，就是原问题的答案 $\textit{dfs}(i,j)$。

所以有

$$
\textit{dfs}(i,j) = \textit{dfs}(i-1,j) + \textit{dfs}(i-1,j-1)+1 
$$

**递归边界**：$\textit{dfs}(0,j)=\textit{dfs}(i,0)=0$。无法扔鸡蛋。

**递归入口**：枚举 $i=1,2,3,\cdots$，首个满足 $\textit{dfs}(i,k)\ge n$ 的 $i$ 是答案。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i,j)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。由于本题不会算出 $0$，所以可以把初始值设置为 $0$。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
@cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
def dfs(i: int, j: int) -> int:
    if i == 0 or j == 0:
        return 0
    return dfs(i - 1, j) + dfs(i - 1, j - 1) + 1

class Solution:
    def superEggDrop(self, k: int, n: int) -> int:
        for i in count(1):  # 从 1 开始枚举 i
            if dfs(i, k) >= n:
                return i
```

```java [sol-Java]
class Solution {
    public int superEggDrop(int k, int n) {
        int[][] memo = new int[n + 1][];
        for (int i = 1; ; i++) {
            memo[i] = new int[k + 1]; // 动态创建 memo
            if (dfs(i, k, memo) >= n) {
                return i;
            }
        }
    }

    private int dfs(int i, int j, int[][] memo) {
        if (i == 0 || j == 0) {
            return 0;
        }
        if (memo[i][j] != 0) { // 之前计算过
            return memo[i][j];
        }
        return memo[i][j] = dfs(i - 1, j, memo) + dfs(i - 1, j - 1, memo) + 1;
    }
}
```

```cpp [sol-C++]
class Solution {
    vector<vector<int>> memo{{}};

    int dfs(int i, int j) {
        if (i == 0 || j == 0) {
            return 0;
        }
        int& res = memo[i][j]; // 注意这里是引用
        if (res != 0) { // 之前计算过
            return res;
        }
        return res = dfs(i - 1, j) + dfs(i - 1, j - 1) + 1;
    };

public:
    int superEggDrop(int k, int n) {
        for (int i = 1; ; i++) {
            memo.emplace_back(k + 1); // 插入一个长为 k+1 的 vector
            if (dfs(i, k) >= n) {
                return i;
            }
        }
    }
};
```

```go [sol-Go]
func superEggDrop(k, n int) int {
    memo := [][]int{{}}
    var dfs func(int, int) int
    dfs = func(i, j int) int {
        if i == 0 || j == 0 {
            return 0
        }
        p := &memo[i][j]
        if *p != 0 { // 之前计算过
            return *p
        }
        *p = dfs(i-1, j) + dfs(i-1, j-1) + 1
        return *p
    }
    for i := 1; ; i++ {
        memo = append(memo, make([]int, k+1)) // 动态创建 memo
        if dfs(i, k) >= n {
            return i
        }
    }
}
```

#### 复杂度分析

根据 [这篇题解](https://leetcode.cn/problems/super-egg-drop/solutions/2949084/zui-kuai-jie-fa-zhi-jie-suan-chu-tong-xi-8219/)，以及阶乘的斯特林公式，我们枚举的操作次数的上限为 $\mathcal{O}(k\cdot\sqrt[k]{\sqrt{k}n})$。对于相同的 $i$，有 $\mathcal{O}(k)$ 个不同的 $j$，所以状态个数为 $\mathcal{O}(k^2\cdot\sqrt[k]{\sqrt{k}n})$。

- 时间复杂度：$\mathcal{O}(k^2\cdot\sqrt[k]{\sqrt{k}n})$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(k^2\cdot\sqrt[k]{\sqrt{k}n})$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(k^2\cdot\sqrt[k]{\sqrt{k}n})$。
- 空间复杂度：$\mathcal{O}(k^2\cdot\sqrt[k]{\sqrt{k}n})$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i][j]$ 的定义和 $\textit{dfs}(i,j)$ 的定义是一样的，都表示在有 $i$ 次操作机会和 $j$ 枚鸡蛋的情况下，可以让我们能确定 $f$ 值的最大建筑层数。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i][j] = f[i-1][j] + f[i-1][j-1]+1
$$

初始值 $f[0][j]=f[i][0]=0$，翻译自递归边界 $\textit{dfs}(0,j)=\textit{dfs}(i,0)=0$。

枚举 $i=1,2,3,\cdots$，首个满足 $f[i][k]\ge n$ 的 $i$ 是答案。

```py [sol-Python3]
class Solution:
    def superEggDrop(self, k: int, n: int) -> int:
        f = [[0] * (k + 1) for _ in range(n + 1)]
        for i in count(1):  # 从 1 开始枚举 i
            for j in range(1, k + 1):
                f[i][j] = f[i - 1][j] + f[i - 1][j - 1] + 1
            if f[i][k] >= n:
                return i
```

```java [sol-Java]
class Solution {
    public int superEggDrop(int k, int n) {
        int[][] f = new int[n + 1][k + 1];
        for (int i = 1; ; i++) {
            for (int j = 1; j <= k; j++) {
                f[i][j] = f[i - 1][j] + f[i - 1][j - 1] + 1;
            }
            if (f[i][k] >= n) {
                return i;
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int superEggDrop(int k, int n) {
        vector<vector<int>> f(n + 1, vector<int>(k + 1));
        for (int i = 1; ; i++) {
            for (int j = 1; j <= k; j++) {
                f[i][j] = f[i - 1][j] + f[i - 1][j - 1] + 1;
            }
            if (f[i][k] >= n) {
                return i;
            }
        }
    }
};
```

```go [sol-Go]
func superEggDrop(k, n int) int {
    f := make([][]int, n+1)
    f[0] = make([]int, k+1)
    for i := 1; ; i++ {
        f[i] = make([]int, k+1)
        for j := 1; j <= k; j++ {
            f[i][j] = f[i-1][j] + f[i-1][j-1] + 1
        }
        if f[i][k] >= n {
            return i
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk)$。瓶颈主要在创建数组上。
- 空间复杂度：$\mathcal{O}(nk)$。

## 五、空间优化

观察上面的状态转移方程，在计算 $f[i]$ 时，只会用到 $f[i-1]$，不会用到比 $i-1$ 更早的状态。

因此可以像 [0-1 背包](https://www.bilibili.com/video/BV16Y411v7Y6/) 那样，去掉第一个维度，把 $f[i]$ 和 $f[i-1]$ 保存到**同一个数组**中。注意 $j$ 要**倒序**枚举，原理和 0-1 背包的空间优化是一样的。

状态转移方程改为

$$
f[j] = f[j] + f[j-1] + 1
$$

初始值 $f[j]=0$。

枚举 $i=1,2,3,\cdots$，首个满足 $f[k]\ge n$ 的 $i$ 是答案。

```py [sol-Python3]
class Solution:
    def superEggDrop(self, k: int, n: int) -> int:
        f = [0] * (k + 1)
        for i in count(1):  # 从 1 开始枚举 i
            for j in range(k, 0, -1):
                f[j] += f[j - 1] + 1
            if f[k] >= n:
                return i
```

```java [sol-Java]
class Solution {
    public int superEggDrop(int k, int n) {
        int[] f = new int[k + 1];
        for (int i = 1; ; i++) {
            for (int j = k; j > 0; j--) {
                f[j] += f[j - 1] + 1;
            }
            if (f[k] >= n) {
                return i;
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int superEggDrop(int k, int n) {
        vector<int> f(k + 1);
        for (int i = 1; ; i++) {
            for (int j = k; j > 0; j--) {
                f[j] += f[j - 1] + 1;
            }
            if (f[k] >= n) {
                return i;
            }
        }
    }
};
```

```go [sol-Go]
func superEggDrop(k, n int) int {
    f := make([]int, k+1)
    for i := 1; ; i++ {
        for j := k; j > 0; j-- {
            f[j] += f[j-1] + 1
        }
        if f[k] >= n {
            return i
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k^2\cdot\sqrt[k]{\sqrt{k}n})$。见记忆化搜索的复杂度分析。
- 空间复杂度：$\mathcal{O}(k)$。

## 相似题目

见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**§7.5 多维 DP**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.6 多维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.6 多维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
