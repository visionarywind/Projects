# 1105. 填充书架

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/filling-bookcase-shelves/
- 题目 slug：`filling-bookcase-shelves`
- 来源专题：动态规划
- 来源分类路径：五、划分型 DP / §5.2 最优划分
- 难度分：2014
- 外部题解来源：https://leetcode.cn/problems/filling-bookcase-shelves/solutions/2240688/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-0vg6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考动态规划！（Python/Java/C++/Go）](https://leetcode.cn/problems/filling-bookcase-shelves/solutions/2240688/jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-0vg6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dong-tai-gui-hua-0vg6`
- topic id：`2240688`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识：动态规划入门

详见 [动态规划入门：从记忆化搜索到递推【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

> APP 用户需要分享到微信打开视频链接。

## 一、寻找子问题

![1105.png](https://pic.leetcode.cn/1682046159-SpxUwb-1105.png){:width=300}

对于示例 1（上图），考虑书架的最底层，可以放 $\textit{books}$ 的最后 $1$ 到 $4$ 本书（题目要求**按顺序摆放**，且书的厚度之和不能超过 $\textit{shelfWidth}$）。假设放了 $4$ 本书，那么剩余的要解决的问题是 $\textit{books}$ 的前 $3$ 本书按顺序摆放后的最小高度。由于这是一个和原问题相似的子问题，所以可以用递归解决。

由于每个子问题处理的都是 $\textit{books}$ 的前缀，所以递归参数只需要一个 $i$。定义 $\textit{dfs}(i)$ 表示把 $\textit{books}[0]$ 到 $\textit{books}[i]$ 按顺序摆放后的最小高度。

枚举最后一层的第一本书的下标 $j$，那么从 $\textit{books}[j]$ 到 $\textit{books}[i]$ 的厚度之和不能超过 $\textit{shelfWidth}$。

把这些书的最大高度，加上 $\textit{dfs}(j-1)$ 这个子问题的结果，再取最小值，就是 $\textit{dfs}(i)$。写成式子就是

$$
\textit{dfs}(i) = \min_{j}\left\{\textit{dfs}(j-1) +  \max_{k=j}^{i}\textit{books}[k][1]\right\}
$$

其中 $j\le i$ 且 $\sum\limits_{k=j}^{i}\textit{books}[k][0]\le \textit{shelfWidth}$。

递归边界：$\textit{dfs}(-1) = 0$。没有书，最小高度为 $0$。

递归入口：$\textit{dfs}(n-1)$，表示把所有书按顺序摆放后的最小高度，也就是答案。

代码实现时，可以从 $i$ 开始倒着枚举 $j$，一边枚举一边计算厚度之和（或者剩余空间），同时更新最大高度。

```py [sol1-Python3]
# 暴力递归，会超时
class Solution:
    def minHeightShelves(self, books: List[List[int]], shelf_width: int) -> int:
        def dfs(i: int) -> int:
            if i < 0: return 0  # 没有书了，高度是 0
            res, max_h, left_w = inf, 0, shelf_width
            for j in range(i, -1, -1):
                left_w -= books[j][0]
                if left_w < 0: break  # 空间不足，无法放书
                max_h = max(max_h, books[j][1])  # 从 j 到 i 的最大高度
                res = min(res, dfs(j - 1) + max_h)
            return res
        return dfs(len(books) - 1)
```

```java [sol1-Java]
// 暴力递归，会超时
class Solution {
    private int[][] books;
    private int shelfWidth;

    public int minHeightShelves(int[][] books, int shelfWidth) {
        this.books = books;
        this.shelfWidth = shelfWidth;
        return dfs(books.length - 1);
    }

    private int dfs(int i) {
        if (i < 0) return 0; // 没有书了，高度是 0
        int res = Integer.MAX_VALUE, maxH = 0, leftW = shelfWidth;
        for (int j = i; j >= 0; --j) {
            leftW -= books[j][0];
            if (leftW < 0) break; // 空间不足，无法放书
            maxH = Math.max(maxH, books[j][1]); // 从 j 到 i 的最大高度
            res = Math.min(res, dfs(j - 1) + maxH);
        }
        return res;
    }
}
```

```cpp [sol1-C++]
// 暴力递归，会超时
class Solution {
public:
    int minHeightShelves(vector<vector<int>> &books, int shelf_width) {
        function<int(int)> dfs = [&](int i) -> int {
            if (i < 0) return 0; // 没有书了，高度是 0
            int res = INT_MAX, max_h = 0, left_w = shelf_width;
            for (int j = i; j >= 0; --j) {
                left_w -= books[j][0];
                if (left_w < 0) break; // 空间不足，无法放书
                max_h = max(max_h, books[j][1]); // 从 j 到 i 的最大高度
                res = min(res, dfs(j - 1) + max_h);
            }
            return res;
        };
        return dfs(books.size() - 1);
    }
};
```

```go [sol1-Go]
// 暴力递归，会超时
func minHeightShelves(books [][]int, shelfWidth int) int {
    var dfs func(int) int
    dfs = func(i int) int {
        if i < 0 { // 没有书了，高度是 0
            return 0
        }
        res := math.MaxInt
        maxH, leftW := 0, shelfWidth
        for j := i; j >= 0; j-- {
            leftW -= books[j][0]
            if leftW < 0 { // 空间不足，无法放书
                break
            }
            maxH = max(maxH, books[j][1]) // 从 j 到 i 的最大高度
            res = min(res, dfs(j-1)+maxH)
        }
        return res
    }
    return dfs(len(books) - 1)
}

func min(a, b int) int { if b < a { return b }; return a }
func max(a, b int) int { if b > a { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$\mathcal{O}(2^n)$，其中 $n$ 为 $\textit{books}$ 的长度。等价于枚举 $n-1$ 条分割线选或不选，对于选择的分割线，把数组划分成若干段，每一段都对应一层书架上的书。所以至多有 $2^{n-1}$ 种划分方式，所以时间复杂度为 $\mathcal{O}(2^n)$。
- 空间复杂度：$\mathcal{O}(n)$。递归需要 $\mathcal{O}(n)$ 的栈空间

## 二、递归 + 记录返回值 = 记忆化搜索

上面的做法太慢了，怎么优化呢？

举个例子，「把 $\textit{books}[n-1]$ 单独放一层，把 $\textit{books}[n-2]$ 单独放一层」和「把 $\textit{books}[n-2]$ 和 $\textit{books}[n-1]$放同一层」，都会递归到 $\textit{dfs}(n-3)$。

一叶知秋，整个递归中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组（或哈希表）中。
- 如果一个状态不是第一次遇到，那么直接返回 $\textit{memo}$ 中保存的结果。

```py [sol2-Python3]
class Solution:
    def minHeightShelves(self, books: List[List[int]], shelf_width: int) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果
        def dfs(i: int) -> int:
            if i < 0: return 0  # 没有书了，高度是 0
            res, max_h, left_w = inf, 0, shelf_width
            for j in range(i, -1, -1):
                left_w -= books[j][0]
                if left_w < 0: break  # 空间不足，无法放书
                max_h = max(max_h, books[j][1])  # 从 j 到 i 的最大高度
                res = min(res, dfs(j - 1) + max_h)
            return res
        return dfs(len(books) - 1)
```

```java [sol2-Java]
class Solution {
    private int[][] books;
    private int shelfWidth;
    private int[] memo;

    public int minHeightShelves(int[][] books, int shelfWidth) {
        this.books = books;
        this.shelfWidth = shelfWidth;
        int n = books.length;
        memo = new int[n];
        Arrays.fill(memo, -1); // -1 表示还没有计算过
        return dfs(n - 1);
    }

    private int dfs(int i) {
        if (i < 0) return 0; // 没有书了，高度是 0
        if (memo[i] != -1) return memo[i]; // 之前计算过了
        int res = Integer.MAX_VALUE, maxH = 0, leftW = shelfWidth;
        for (int j = i; j >= 0; --j) {
            leftW -= books[j][0];
            if (leftW < 0) break; // 空间不足，无法放书
            maxH = Math.max(maxH, books[j][1]); // 从 j 到 i 的最大高度
            res = Math.min(res, dfs(j - 1) + maxH);
        }
        return memo[i] = res; // 记忆化
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int minHeightShelves(vector<vector<int>> &books, int shelf_width) {
        int n = books.size(), memo[n];
        memset(memo, -1, sizeof(memo)); // -1 表示还没有计算过
        function<int(int)> dfs = [&](int i) -> int {
            if (i < 0) return 0; // 没有书了，高度是 0
            int &res = memo[i]; // 注意这里是引用，下面会直接修改 memo[i]
            if (res != -1) return res; // 之前计算过了
            res = INT_MAX;
            int max_h = 0, left_w = shelf_width;
            for (int j = i; j >= 0; --j) {
                left_w -= books[j][0];
                if (left_w < 0) break; // 空间不足，无法放书
                max_h = max(max_h, books[j][1]); // 从 j 到 i 的最大高度
                res = min(res, dfs(j - 1) + max_h);
            }
            return res;
        };
        return dfs(n - 1);
    }
};
```

```go [sol2-Go]
func minHeightShelves(books [][]int, shelfWidth int) int {
    n := len(books)
    memo := make([]int, n)
    for i := range memo {
        memo[i] = -1 // -1 表示还没有计算过
    }
    var dfs func(int) int
    dfs = func(i int) int {
        if i < 0 { // 没有书了，高度是 0
            return 0
        }
        if memo[i] != -1 { // 之前计算过了
            return memo[i]
        }
        res := math.MaxInt
        maxH, leftW := 0, shelfWidth
        for j := i; j >= 0; j-- {
            leftW -= books[j][0]
            if leftW < 0 { // 空间不足，无法放书
                break
            }
            maxH = max(maxH, books[j][1]) // 从 j 到 i 的最大高度
            res = min(res, dfs(j-1)+maxH)
        }
        memo[i] = res // 记忆化
        return res
    }
    return dfs(n - 1)
}

func min(a, b int) int { if b < a { return b }; return a }
func max(a, b int) int { if b > a { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{books}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。这里状态个数为 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(n)$，因此时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n)$。即状态个数。

## 三、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

通用做法：

- $\textit{dfs}$ 改成 $f$ 数组；
- 递归改成循环（每个参数都对应一层循环）；
- 递归边界改成 $f$ 数组的初始值。

> 相当于原来是用递归去计算每个状态，现在是按照某个顺序去枚举并计算每个状态。

具体来说，$f[i]$ 的含义和 $\textit{dfs}(i)$ 的含义是一致的，都表示把 $\textit{books}[0]$ 到 $\textit{books}[i]$ 按顺序摆放后的最小高度。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 的一致：

$$
f[i] = \min_{j}\left\{f[j-1] +  \max_{k=j}^{i}\textit{books}[k][1]\right\}
$$

其中 $j\le i$ 且 $\sum\limits_{k=j}^{i}\textit{books}[k][0]\le \textit{shelfWidth}$。

但是这种定义方式**没有状态能表示递归边界**，即 $i=-1$ 的情况。

解决办法：在 $f$ 数组的最左边插入一个状态，那么其余状态全部向右偏移一位，也就是 $f[i]$ 改为 $f[i+1]$，$f[j-1]$ 改为 $f[j]$。

修改后 $f[i+1]$ 表示把 $\textit{books}[0]$ 到 $\textit{books}[i]$ 按顺序摆放后的最小高度。这样 $f[0]=0$ 就对应递归边界了。

修改后的递推式为

$$
f[i+1] = \min_{j}\left\{f[j] +  \max_{k=j}^{i}\textit{books}[k][1]\right\}
$$

其中 $j\le i$ 且 $\sum\limits_{k=j}^{i}\textit{books}[k][0]\le \textit{shelfWidth}$。

> 问：为什么 $j$ 的起止下标不用变？为什么 $\textit{books}[k]$ 的下标不用变？
>
> 答：只是在 $f$ 的最左边插入一个状态，那就只需要修改和 $f$ 有关的下标，其余任何逻辑都无需修改。

初始值 $f[0]=0$。（翻译自 $\textit{dfs}(-1)=0$。）

答案为 $f[n]$。（翻译自 $\textit{dfs}(n-1)$。）

```py [sol3-Python3]
class Solution:
    def minHeightShelves(self, books: List[List[int]], shelf_width: int) -> int:
        n = len(books)
        f = [0] + [inf] * n  # 在前面插入一个状态表示 dfs(-1)=0
        for i in range(n):
            max_h, left_w = 0, shelf_width
            for j in range(i, -1, -1):
                left_w -= books[j][0]
                if left_w < 0: break  # 空间不足，无法放书
                max_h = max(max_h, books[j][1])  # 从 j 到 i 的最大高度
                f[i + 1] = min(f[i + 1], f[j] + max_h)
        return f[n]  # 翻译自 dfs(n-1)
```

```java [sol3-Java]
class Solution {
    public int minHeightShelves(int[][] books, int shelfWidth) {
        int n = books.length;
        var f = new int[n + 1]; // f[0]=0，翻译自 dfs(-1)=0
        for (int i = 0; i < n; ++i) {
            f[i + 1] = Integer.MAX_VALUE;
            int maxH = 0, leftW = shelfWidth;
            for (int j = i; j >= 0; --j) {
                leftW -= books[j][0];
                if (leftW < 0) break; // 空间不足，无法放书
                maxH = Math.max(maxH, books[j][1]); // 从 j 到 i 的最大高度
                f[i + 1] = Math.min(f[i + 1], f[j] + maxH);
            }
        }
        return f[n]; // 翻译自 dfs(n-1)
    }
}
```

```cpp [sol3-C++]
class Solution {
public:
    int minHeightShelves(vector<vector<int>> &books, int shelf_width) {
        int n = books.size(), f[n + 1];
        f[0] = 0; // 翻译自 dfs(-1)=0
        for (int i = 0; i < n; ++i) {
            f[i + 1] = INT_MAX;
            int max_h = 0, left_w = shelf_width;
            for (int j = i; j >= 0; --j) {
                left_w -= books[j][0];
                if (left_w < 0) break; // 空间不足，无法放书
                max_h = max(max_h, books[j][1]); // 从 j 到 i 的最大高度
                f[i + 1] = min(f[i + 1], f[j] + max_h);
            }
        };
        return f[n]; // 翻译自 dfs(n-1)
    }
};
```

```go [sol3-Go]
func minHeightShelves(books [][]int, shelfWidth int) int {
    n := len(books)
    f := make([]int, n+1) // f[0]=0，翻译自 dfs(-1)=0
    for i := range books {
        f[i+1] = math.MaxInt
        maxH, leftW := 0, shelfWidth
        for j := i; j >= 0; j-- {
            leftW -= books[j][0]
            if leftW < 0 { // 空间不足，无法放书
                break
            }
            maxH = max(maxH, books[j][1]) // 从 j 到 i 的最大高度
            f[i+1] = min(f[i+1], f[j]+maxH)
        }
    }
    return f[n] // 翻译自 dfs(n-1)
}

func min(a, b int) int { if b < a { return b }; return a }
func max(a, b int) int { if b > a { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$\mathcal{O}(n^2)$，其中 $n$ 为 $\textit{books}$ 的长度。动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。这里状态个数为 $\mathcal{O}(n)$，单个状态的计算时间为 $\mathcal{O}(n)$，因此时间复杂度为 $\mathcal{O}(n^2)$。
- 空间复杂度：$\mathcal{O}(n)$。即状态个数。

## 练习

- [2369. 检查数组是否存在有效划分](https://leetcode.cn/problems/check-if-there-is-a-valid-partition-for-the-array/)
- [1416. 恢复数组](https://leetcode.cn/problems/restore-the-array/)
- [1043. 分隔数组以得到最大和](https://leetcode.cn/problems/partition-array-for-maximum-sum/)

[每日一题·高质量题解精选](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

---

欢迎关注[ biIibiIi@灵茶山艾府](https://space.bilibili.com/206214)，高质量算法教学，持续输出中~

## 本地原创解析

### 1. 题意重述

本题来自 `五、划分型 DP / §5.2 最优划分`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、划分型 DP / §5.2 最优划分`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
