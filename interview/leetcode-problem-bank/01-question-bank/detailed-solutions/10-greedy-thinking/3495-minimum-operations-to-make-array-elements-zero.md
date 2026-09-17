# 3495. 使数组元素都变为零的最少操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-make-array-elements-zero/
- 题目 slug：`minimum-operations-to-make-array-elements-zero`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.8 相邻不同
- 难度分：2206
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-make-array-elements-zero/solutions/3624312/o1-gong-shi-pythonjavacgo-by-endlesschen-2gos/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[从 O(log U) 到 O(1)（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-operations-to-make-array-elements-zero/solutions/3624312/o1-gong-shi-pythonjavacgo-by-endlesschen-2gos/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`o1-gong-shi-pythonjavacgo-by-endlesschen-2gos`
- topic id：`3624312`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

## 分析

$a$ 除以 $4$ 下取整等价于 $a$ 右移 $2$ 位。所以把一个长为 $m$ 的二进制数变成 $0$，需要操作 $\left\lceil\dfrac{m}{2}\right\rceil$ 次。

例如 $l=1$，$r=5$，如果每次操作只把一个数右移 $2$ 位，那么 $1$ 到 $5$ 每个数的操作次数分别为

$$
\textit{ops}=[1,1,1,2,2]
$$

本题一次可以操作两个数，问题等价于：

- 每次操作把 $\textit{ops}$ 中的两个数都减去 $1$，问：要让 $\textit{ops}$ 中没有正数，至少要操作多少次？

**分析**：设 $\textit{tot}=\sum \textit{ops}[i]$，$\textit{mx}=\max(\textit{ops})$。假如每次可以把 $\textit{tot}$ 减少 $2$，那么把 $\textit{tot}$ 减少到 $\le 0$，至少要操作 $\left\lceil\dfrac{\textit{tot}}{2}\right\rceil$ 次。但如果 $\textit{mx}$ 很大，操作次数就等于 $\textit{mx}$（每次操作选 $\textit{mx}$ 和另一个数）。

**定理**：最少操作次数为

$$
\max\left(\left\lceil\dfrac{\textit{tot}}{2}\right\rceil, \textit{mx}\right)
$$

[证明+具体操作方案](https://zhuanlan.zhihu.com/p/1945782212176909162)。

本题由于 $\textit{nums}$ 中的数字是连续整数，相邻数字的操作次数至多相差 $1$。

例如两个数的情况 $\textit{ops}=[x-1,x]$，得到 $\textit{mx}=x$，$\textit{tot} = 2x-1$。由于 $\left\lceil\dfrac{2x-1}{2}\right\rceil = x$，所以 $\textit{mx} \le \left\lceil\dfrac{\textit{tot}}{2}\right\rceil$ 成立。推广到更多元素的情况，设最大值为 $\textit{mx}=x$，其余元素不超过 $x$，当元素增多时，$\textit{tot}$ 更大，$\textit{mx} \le \left\lceil\dfrac{\textit{tot}}{2}\right\rceil$ 更加成立。

所以本题

$$
\textit{mx} \le \left\lceil\dfrac{\textit{tot}}{2}\right\rceil
$$

恒成立。

所以

$$
\max\left(\left\lceil\dfrac{\textit{tot}}{2}\right\rceil, \textit{mx}\right) = \left\lceil\dfrac{\textit{tot}}{2}\right\rceil
$$

算出了 $\textit{tot}$，就算出了答案。

## 公式推导

定义 $f(n)$ 为 $[1,n]$ 中的单个数的操作次数之和。

设 $n$ 的二进制长度为 $m$。按照 $[1,n]$ 中的数字的二进制长度，分组计算：

- 对于长为 $i$ 的二进制数（其中 $1\le i\le m-1$），最小是 $2^{i-1}$，最大是 $2^i-1$，共有 $2^{i-1}$ 个，每个需要操作 $\left\lceil\dfrac{i}{2}\right\rceil$ 次。
- 对于长为 $m$ 的二进制数，最小是 $2^{m-1}$，最大是 $n$，共有 $n+1-2^{m-1}$ 个，每个需要操作 $\left\lceil\dfrac{m}{2}\right\rceil$ 次。

两部分累加，得

$$
f(n) = \sum_{i=1}^{m-1} \left\lceil\dfrac{i}{2}\right\rceil 2^{i-1} + \left\lceil\dfrac{m}{2}\right\rceil(n+1-2^{m-1})
$$

由于 $[l,r]$ 可以拆分成 $[1,r]$ 减去 $[1,l-1]$，所以 $[l,r]$ 中的单个数的操作次数之和为 

$$
\textit{tot} = f(r) - f(l-1)
$$

每次操作至多两个数，操作次数为

$$
\left\lceil\dfrac{f(r) - f(l-1)}{2}\right\rceil = \left\lfloor\dfrac{ f(r) - f(l-1) + 1}{2}\right\rfloor
$$

[本题视频讲解](https://www.bilibili.com/video/BV12eXYYVE5H/?t=22m46s)，欢迎点赞关注~

## 优化前

```py [sol-Python3]
# 返回 [1,n] 的单个元素的操作次数之和
def f(n: int) -> int:
    m = n.bit_length()
    res = sum((i + 1) // 2 << (i - 1) for i in range(1, m))
    return res + (m + 1) // 2 * (n + 1 - (1 << m >> 1))

class Solution:
    def minOperations(self, queries: List[List[int]]) -> int:
        return sum((f(r) - f(l - 1) + 1) // 2 for l, r in queries)
```

```java [sol-Java]
class Solution {
    public long minOperations(int[][] queries) {
        long ans = 0;
        for (int[] q : queries) {
            ans += (f(q[1]) - f(q[0] - 1) + 1) / 2;
        }
        return ans;
    }

    // 返回 [1,n] 的单个元素的操作次数之和
    private long f(int n) {
        int m = 32 - Integer.numberOfLeadingZeros(n);
        long res = 0;
        for (int i = 1; i < m; i++) {
            res += (long) (i + 1) / 2 << (i - 1);
        }
        return res + (long) (m + 1) / 2 * (n + 1 - (1 << m >> 1));
    }
}
```

```cpp [sol-C++]
class Solution {
    // 返回 [1,n] 的单个元素的操作次数之和
    long long f(int n) {
        int m = bit_width((uint32_t) n);
        long long res = 0;
        for (int i = 1; i < m; i++) {
            res += 1LL * (i + 1) / 2 << (i - 1);
        }
        return res + 1LL * (m + 1) / 2 * (n + 1 - (1 << m >> 1));
    }

public:
    long long minOperations(vector<vector<int>>& queries) {
        long long ans = 0;
        for (auto& q : queries) {
            ans += (f(q[1]) - f(q[0] - 1) + 1) / 2;
        }
        return ans;
    }
};
```

```go [sol-Go]
// 返回 [1,n] 的单个元素的操作次数之和
func f(n int) (res int) {
	m := bits.Len(uint(n))
	for i := 1; i < m; i++ {
		res += (i + 1) / 2 << (i - 1)
	}
	return res + (m+1)/2*(n+1-1<<m>>1)
}

func minOperations(queries [][]int) int64 {
	ans := 0
	for _, q := range queries {
		ans += (f(q[1]) - f(q[0]-1) + 1) / 2
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(q\log U)$，其中 $q$ 是 $\textit{queries}$ 的长度，$U$ 是 $r$ 的最大值。每个询问需要 $\mathcal{O}(\log U)$ 的时间。
- 空间复杂度：$\mathcal{O}(1)$。

## 化简公式

$\mathcal{O}(1)$ 计算 $f(n)$。

设 $n$ 的二进制长度为 $m$。设 $k$ 为小于 $m$ 的最大偶数。

上面代码的循环，把二进制长为 $1,2$ 的分成一组（每个数操作 $1$ 次），长为 $3,4$ 的分成一组（每个数操作 $2$ 次），长为 $5,6$ 的分成一组（每个数操作 $3$ 次）……依此类推，累加得

$$
(2^2-2^0)\cdot 1 + (2^4-2^2)\cdot 2 + \cdots + (2^k-2^{k-2})\cdot \dfrac{k}{2}
$$

利用错位相减法（详见 [视频讲解](https://www.bilibili.com/video/BV12eXYYVE5H/?t=22m46s)），上式可化简为

$$
k\cdot 2^{k-1} - \dfrac{2^k-1}{3}
$$

对于长为 $[k+1,m]$ 的二进制数，最小是 $2^k$，最大是 $n$，共有 $n+1-2^k$ 个，每个需要操作 $\left\lceil\dfrac{m}{2}\right\rceil$ 次。

相加得

$$
f(n) = k\cdot 2^{k-1} - \dfrac{2^k-1}{3} + \left\lceil\dfrac{m}{2}\right\rceil(n+1-2^k)
$$

代码实现时，如果 $k=0$，没法左移 $k-1=-1$ 位。可以改为先左移 $k$ 位，再右移一位，这样无需特判 $k=0$ 的情况。

```py [sol-Python3]
def f(n: int) -> int:
    if n == 0:
        return 0
    m = n.bit_length()
    k = (m - 1) // 2 * 2
    res = (k << k >> 1) - (1 << k) // 3  # -1 可以省略
    return res + (m + 1) // 2 * (n + 1 - (1 << k))

class Solution:
    def minOperations(self, queries: List[List[int]]) -> int:
        return sum((f(r) - f(l - 1) + 1) // 2 for l, r in queries)
```

```java [sol-Java]
class Solution {
    public long minOperations(int[][] queries) {
        long ans = 0;
        for (int[] q : queries) {
            ans += (f(q[1]) - f(q[0] - 1) + 1) / 2;
        }
        return ans;
    }

    private long f(int n) {
        int m = 32 - Integer.numberOfLeadingZeros(n);
        int k = (m - 1) / 2 * 2;
        long res = ((long) k << k >> 1) - (1 << k) / 3; // -1 可以省略
        return res + (long) (m + 1) / 2 * (n + 1 - (1 << k));
    }
}
```

```cpp [sol-C++]
class Solution {
    long long f(int n) {
        int m = bit_width((uint32_t) n);
        int k = (m - 1) / 2 * 2;
        long long res = (1LL * k << k >> 1) - (1 << k) / 3; // -1 可以省略
        return res + 1LL * (m + 1) / 2 * (n + 1 - (1 << k));
    }

public:
    long long minOperations(vector<vector<int>>& queries) {
        long long ans = 0;
        for (auto& q : queries) {
            ans += (f(q[1]) - f(q[0] - 1) + 1) / 2;
        }
        return ans;
    }
};
```

```go [sol-Go]
func f(n int) int {
	m := bits.Len(uint(n))
	k := (m - 1) / 2 * 2
	res := k<<k>>1 - 1<<k/3 // -1 可以省略
	return res + (m+1)/2*(n+1-1<<k)
}

func minOperations(queries [][]int) int64 {
	ans := 0
	for _, q := range queries {
		ans += (f(q[1]) - f(q[0]-1) + 1) / 2
	}
	return int64(ans)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(q)$，其中 $q$ 是 $\textit{queries}$ 的长度。每个询问只需 $\mathcal{O}(1)$ 时间。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面贪心题单的「**§1.8 相邻不同**」。

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

本题来自 `一、贪心策略 / §1.8 相邻不同`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.8 相邻不同`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
