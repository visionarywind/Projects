# 342. 4 的幂

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/power-of-four/
- 题目 slug：`power-of-four`
- 来源专题：位运算
- 来源分类路径：一、基础题
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/power-of-four/solutions/3747594/liang-chong-fang-fa-wei-yun-suan-shu-xue-cvk5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：位运算 / 数学（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/power-of-four/solutions/3747594/liang-chong-fang-fa-wei-yun-suan-shu-xue-cvk5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-wei-yun-suan-shu-xue-cvk5`
- topic id：`3747594`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前言

$n$ 是 $4$ 的幂的必要条件是，$n$ 是 [2 的幂](https://leetcode.cn/problems/power-of-two/)。做法见 [我的题解](https://leetcode.cn/problems/power-of-two/solutions/2973442/yan-ge-zheng-ming-yi-xing-xie-fa-pythonj-h04o/)。

在 $n$ 是 $2$ 的幂的前提下，如何判断 $n$ 是 $4$ 的幂？

## 方法一：位运算

分类讨论：

- $n = 2^0,2^2,2^4,2^6,\ldots = 4^0,4^1,4^2,4^3,\dots$ 这些数都是 $4$ 的幂。
- $n = 2^1,2^3,2^5,2^7,\dots$ 这些数都不是 $4$ 的幂。

观察这些数的二进制形式。

$n = 2^0,2^2,2^4,2^6,\dots$ 的二进制表示为

$$
\begin{aligned}
1\\
100\\
10000\\
1000000\\
\vdots \\
\end{aligned}
$$

$n = 2^1,2^3,2^5,2^7,\dots$ 的二进制表示为

$$
\begin{aligned}
10\\
1000\\
100000\\
10000000\\
\vdots \\
\end{aligned}
$$

取出 $n$ 的二进制的中的从低到高第 $0,2,4,6,\dots$ 位，如果其中有 $1$，那么 $n$ 就是 $4$ 的幂；否则不是。

怎么取出这些比特位？由于 $n$ 是 $32$ 位整数，我们可以把 $n$ 与二进制数 $01010101010101010101010101010101$ 计算按位与（即编程语言中的 $\texttt{&}$），就取出了这些比特位。如果按位与的结果大于 $0$，那么 $n$ 是 $4$ 的幂；否则不是。

代码实现时，二进制数 $01010101010101010101010101010101$ 可以用十六进制数 $55555555$ 表示。代码需要加上前缀 $\texttt{0x}$ 表示十六进制数。

更多位运算技巧，请看 [从集合论到位运算，常见位运算技巧分类总结！](https://leetcode.cn/circle/discuss/CaOJ45/)

```py [sol-Python3]
class Solution:
    def isPowerOfFour(self, n: int) -> bool:
        return n > 0 and n & (n - 1) == 0 and n & 0x55555555 > 0
```

```java [sol-Java]
class Solution {
    public boolean isPowerOfFour(int n) {
        return n > 0 && (n & (n - 1)) == 0 && (n & 0x55555555) > 0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isPowerOfFour(int n) {
        return n > 0 && (n & (n - 1)) == 0 && (n & 0x55555555) > 0;
    }
};
```

```c [sol-C]
int isPowerOfFour(int n) {
    return n > 0 && (n & (n - 1)) == 0 && (n & 0x55555555) > 0;
}
```

```go [sol-Go]
func isPowerOfFour(n int) bool {
    return n > 0 && n&(n-1) == 0 && n&0x55555555 > 0
}
```

```js [sol-JavaScript]
var isPowerOfFour = function(n) {
    return n > 0 && (n & (n - 1)) === 0 && (n & 0x55555555) > 0;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn is_power_of_four(n: i32) -> bool {
        n > 0 && n & (n - 1) == 0 && n & 0x55555555 > 0
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：数学

根据二项式定理，当 $k$ 是非负整数时，我们有

$$
4^k = (3+1)^k = \binom k 0 3^k1^0 + \binom k 1 3^{k-1}1^1 + \binom k 2 3^{k-2}1^2 + \cdots + \binom k {k-1} 3^11^{k-1} + \binom k k 3^01^k
$$

上式模 $3$，除了最后一项为 $1$，其余项全为 $0$，所以有

$$
4^k\bmod 3 = 1
$$

两边同时乘以 $2$（或者从二项式定理出发推导），得

$$
2\cdot 4^k\bmod 3 = 2
$$

据此，（在 $n$ 是 $2$ 的幂的**前提**下）我们可以计算 $n\bmod 3$：

- 如果结果等于 $1$，那么 $n$ 是 $4$ 的幂。（$n=2^{2k} = 4^k$）
- 否则结果等于 $2$，那么 $n$ 不是 $4$ 的幂。（$n=2^{2k+1} = 2\cdot 4^k$）

```py [sol-Python3]
class Solution:
    def isPowerOfFour(self, n: int) -> bool:
        return n > 0 and n & (n - 1) == 0 and n % 3 == 1
```

```java [sol-Java]
class Solution {
    public boolean isPowerOfFour(int n) {
        return n > 0 && (n & (n - 1)) == 0 && n % 3 == 1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isPowerOfFour(int n) {
        return n > 0 && (n & (n - 1)) == 0 && n % 3 == 1;
    }
};
```

```c [sol-C]
int isPowerOfFour(int n) {
    return n > 0 && (n & (n - 1)) == 0 && n % 3 == 1;
}
```

```go [sol-Go]
func isPowerOfFour(n int) bool {
    return n > 0 && n&(n-1) == 0 && n%3 == 1
}
```

```js [sol-JavaScript]
var isPowerOfFour = function(n) {
    return n > 0 && (n & (n - 1)) === 0 && n % 3 === 1;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn is_power_of_four(n: i32) -> bool {
        n > 0 && n & (n - 1) == 0 && n % 3 == 1
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

如何推广到一般情况？比如 $8$ 的幂，$6$ 的幂等等。

欢迎在评论区说说你的思路。

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

本题来自 `一、基础题`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、基础题`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
