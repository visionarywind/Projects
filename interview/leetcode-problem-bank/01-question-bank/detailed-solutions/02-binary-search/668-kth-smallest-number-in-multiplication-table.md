# 668. 乘法表中第 K 小的数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/kth-smallest-number-in-multiplication-table/
- 题目 slug：`kth-smallest-number-in-multiplication-table`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.6 第 K 小/大
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/kth-smallest-number-in-multiplication-table/solutions/2999698/di-k-xiao-da-wen-ti-de-tong-yong-zhuan-h-9y8i/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[第 k 小/大问题的通用转化方法 + 优化（Python/Java/C++/Go）](https://leetcode.cn/problems/kth-smallest-number-in-multiplication-table/solutions/2999698/di-k-xiao-da-wen-ti-de-tong-yong-zhuan-h-9y8i/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`di-k-xiao-da-wen-ti-de-tong-yong-zhuan-h-9y8i`
- topic id：`2999698`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

第 $k$ 小/大问题的**通用转化方法**：

- 第 $k$ 小等价于：求最小的 $x$，满足 $\le x$ 的数**至少**有 $k$ 个。（注意是至少不是恰好）
- 第 $k$ 大等价于：求最大的 $x$，满足 $\ge x$ 的数**至少**有 $k$ 个。

对于本题，$x$ 越大，越能找到 $k$ 个数；$x$ 越小，越不能找到 $k$ 个数。据此，可以**二分猜答案**。关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

现在本题转化成一个判定性问题：

- 给定整数 $x$，统计乘法表中 $\le x$ 的元素个数 $\textit{cnt}$，判断是否满足 $\textit{cnt}\ge k$。

从第 $1$ 行到第 $m$ 行，我们一行一行地遍历乘法表。

第 $i$ 行的数都是 $i$ 的倍数，其中有多少个 $\le x$ 的正整数？

在 $[1,x]$ 中，有 $\left\lfloor\dfrac{x}{i}\right\rfloor$ 个 $i$ 的倍数（但这不能超过 $n$），所以乘法表中一共有

$$
\sum_{i=1}^{m} \min\left(\left\lfloor\dfrac{x}{i}\right\rfloor, n\right)
$$

个 $\le x$ 的数。

上式还可以继续优化，见后文的「优化」。

## 细节

如果写闭区间二分，左右边界是 $[1,mn-1]$，注意 $mn$ 不需要在二分区间内，因为如果我们没有在 $[1,mn-1]$ 中找到答案，那么答案就一定是 $mn$。

下面代码用的是开区间，即 $(0,mn)$。

- 开区间左端点 $0$ 一定不满足要求：$\le 0$ 的数有 $0 < k$ 个。
- 开区间右端点 $mn$ 一定满足要求：$\le mn$ 的数有 $mn \ge k$ 个。

## 答疑

**问**：为什么二分结束后，答案 $\textit{ans}$ 一定在乘法表中？

**答**：反证法。假设 $\textit{ans}$ 不在乘法表中，这意味着乘法表中第 $k$ 小的数比 $\textit{ans}$ 小，或者说 $\le \textit{ans}-1$。换句话说，$\le \textit{ans}-1$ 的数有 $k$ 个，即 $\text{check}(\textit{ans}-1)=\texttt{true}$。但根据循环不变量，二分结束后 $\text{check}(\textit{ans}-1)=\texttt{false}$，矛盾。故原命题成立。

## 优化前

```py [sol-Python3]
class Solution:
    def findKthNumber(self, m: int, n: int, k: int) -> int:
        def check(x: int) -> bool:
            cnt = 0
            for i in range(1, m + 1):
                cnt += min(x // i, n)
            return cnt >= k

        left, right = 0, m * n
        while left + 1 < right:
            mid = (left + right) // 2
            if check(mid):
                right = mid
            else:
                left = mid
        return right
```

```py [sol-Python3 库函数]
class Solution:
    def findKthNumber(self, m: int, n: int, k: int) -> int:
        check = lambda x: sum(min(x // i, n) for i in range(1, m + 1)) >= k
        return bisect_left(range(m * n), True, 1, key=check)
```

```java [sol-Java]
class Solution {
    public int findKthNumber(int m, int n, int k) {
        int left = 0;
        int right = m * n;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            if (check(mid, m, n, k)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    private boolean check(int x, int m, int n, int k) {
        int cnt = 0;
        for (int i = 1; i <= m; i++) {
            cnt += Math.min(x / i, n);
        }
        return cnt >= k;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findKthNumber(int m, int n, int k) {
        auto check = [&](int x) -> bool {
            int cnt = 0;
            for (int i = 1; i <= m; i++) {
                cnt += min(x / i, n);
            }
            return cnt >= k;
        };

        int left = 0, right = m * n;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right;
    }
};
```

```go [sol-Go]
func findKthNumber(m, n, k int) int {
    return sort.Search(m*n, func(x int) bool {
        cnt := 0
        for i := 1; i <= m; i++ {
            cnt += min(x/i, n)
        }
        return cnt >= k
    })
}
```

## 优化

如下和式可以继续优化。

$$
\sum_{i=1}^{m} \min\left(\left\lfloor\dfrac{x}{i}\right\rfloor, n\right)
$$

如果 $\left\lfloor\dfrac{x}{i}\right\rfloor\ge n$，那么 $\dfrac{x}{i}\ge \left\lfloor\dfrac{x}{i}\right\rfloor \ge n$，即 $i\le \dfrac{x}{n}$。

所以当 $1\le i\le \left\lfloor\dfrac{x}{n}\right\rfloor$ 时，$\min\left(\left\lfloor\dfrac{x}{i}\right\rfloor, n\right) = n$。

而如果 $i > \left\lfloor\dfrac{x}{n}\right\rfloor$，该式等价于 $n > \left\lfloor\dfrac{x}{i}\right\rfloor$，此时 $\min\left(\left\lfloor\dfrac{x}{i}\right\rfloor, n\right) = \left\lfloor\dfrac{x}{i}\right\rfloor$。

令 $k = \left\lfloor\dfrac{x}{n}\right\rfloor$，和式优化为

$$
kn + \sum_{i=k+1}^{m} \left\lfloor\dfrac{x}{i}\right\rfloor
$$

```py [sol-Python3]
class Solution:
    def findKthNumber(self, m: int, n: int, k: int) -> int:
        def check(x: int) -> bool:
            cnt = x // n * n
            for i in range(x // n + 1, m + 1):
                cnt += x // i
            return cnt >= k

        left, right = 0, m * n
        while left + 1 < right:
            mid = (left + right) // 2
            if check(mid):
                right = mid
            else:
                left = mid
        return right
```

```py [sol-Python3 库函数]
class Solution:
    def findKthNumber(self, m: int, n: int, k: int) -> int:
        check = lambda x: x // n * n + sum(x // i for i in range(x // n + 1, m + 1)) >= k
        return bisect_left(range(m * n), True, 1, key=check)
```

```java [sol-Java]
class Solution {
    public int findKthNumber(int m, int n, int k) {
        int left = 0;
        int right = m * n;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            if (check(mid, m, n, k)) {
                right = mid;
            } else {
                left = mid;
            }
        }
        return right;
    }

    private boolean check(int x, int m, int n, int k) {
        int cnt = x / n * n;
        for (int i = x / n + 1; i <= m; i++) {
            cnt += x / i;
        }
        return cnt >= k;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int findKthNumber(int m, int n, int k) {
        auto check = [&](int x) -> bool {
            int cnt = x / n * n;
            for (int i = x / n + 1; i <= m; i++) {
                cnt += x / i;
            }
            return cnt >= k;
        };

        int left = 0, right = m * n;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        return right;
    }
};
```

```go [sol-Go]
func findKthNumber(m, n, k int) int {
    return sort.Search(m*n, func(x int) bool {
        cnt := x / n * n
        for i := x/n + 1; i <= m; i++ {
            cnt += x / i
        }
        return cnt >= k
    })
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log (mn))$。也可以在 $m>n$ 时交换 $m$ 和 $n$，做到 $\mathcal{O}(\min(m,n)\log (mn))$。
- 空间复杂度：$\mathcal{O}(1)$。

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

本题来自 `二、二分答案 / §2.6 第 K 小/大`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.6 第 K 小/大`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
