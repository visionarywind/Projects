# 172. 阶乘后的零

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/factorial-trailing-zeroes/
- 题目 slug：`factorial-trailing-zeroes`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.4 阶乘分解
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/factorial-trailing-zeroes/solutions/2972637/yan-jin-shu-xue-zheng-ming-pythonjavaccg-fe5t/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[严谨数学证明（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/factorial-trailing-zeroes/solutions/2972637/yan-jin-shu-xue-zheng-ming-pythonjavaccg-fe5t/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yan-jin-shu-xue-zheng-ming-pythonjavaccg-fe5t`
- topic id：`2972637`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

例如 $10! = 3628800 = 36288\times 10^2$，尾零个数为 $2$。

一般地，$n!$ 的尾零个数，取决于 $n!$ 可以分解出多少个 $10$。由于 $10=2\times 5$，我们需要知道 $n!$ 中质因子 $2$ 的个数、质因子 $5$ 的个数，二者的较小值，即为 $n!$ 的尾零个数。

下面来计算 $n!$ 的质因数分解中，质数 $p$ 的个数。

- 在 $[1,n]$ 中，有 $\left\lfloor\dfrac{n}{p}\right\rfloor$ 个数可以被 $p$ 整除，其中又有 $\left\lfloor\dfrac{n}{p^2}\right\rfloor$ 个数可以被 $p^2$ 整除，所以**恰好**能被 $p$ 整除，但不能被 $p^2$ 整除的数有 $\left\lfloor\dfrac{n}{p}\right\rfloor - \left\lfloor\dfrac{n}{p^2}\right\rfloor$ 个。例如 $[1,10]$ 中，有 $3$ 个数 $2,6,10$ 恰好能被 $p=2$ 整除。
- 在 $[1,n]$ 中，有 $\left\lfloor\dfrac{n}{p^2}\right\rfloor$ 个数可以被 $p^2$ 整除，其中又有 $\left\lfloor\dfrac{n}{p^3}\right\rfloor$ 个数可以被 $p^3$ 整除，所以**恰好**能被 $p^2$ 整除，但不能被 $p^3$ 整除的数有 $\left\lfloor\dfrac{n}{p^2}\right\rfloor - \left\lfloor\dfrac{n}{p^3}\right\rfloor$ 个。
- 依此类推，$[1,n]$ 中**恰好**能被 $p^{k-1}$ 整除，但不能被 $p^{k}$ 整除的数有 $\left\lfloor\dfrac{n}{p^{k-1}}\right\rfloor - \left\lfloor\dfrac{n}{p^{k}}\right\rfloor$ 个。
- 设 $p^{k}\le n < p^{k+1}$，那么 $[1,n]$ 中**恰好**能被 $p^k$ 整除的数有 $\left\lfloor\dfrac{n}{p^{k}}\right\rfloor$ 个。

继续讨论：

- 恰好能被 $p$ 整除的数，有 $1$ 个质因子 $p$，所以这些数一共有 $1\cdot\left(\left\lfloor\dfrac{n}{p}\right\rfloor - \left\lfloor\dfrac{n}{p^2}\right\rfloor\right)$ 个 $p$。
- 恰好能被 $p^2$ 整除的数，有 $2$ 个质因子 $p$，所以这些数一共有 $2\cdot\left(\left\lfloor\dfrac{n}{p^2}\right\rfloor - \left\lfloor\dfrac{n}{p^3}\right\rfloor\right)$ 个 $p$。
- ……
- 恰好能被 $p^{k-1}$ 整除的数，有 $k-1$ 个质因子 $p$，所以这些数一共有 $(k-1)\cdot\left(\left\lfloor\dfrac{n}{p^{k-1}}\right\rfloor - \left\lfloor\dfrac{n}{p^k}\right\rfloor\right)$ 个 $p$。
- 恰好能被 $p^k$ 整除的数，有 $k$ 个质因子 $p$，所以这些数一共有 $k\cdot \left\lfloor\dfrac{n}{p^{k}}\right\rfloor$ 个 $p$。

累加，即为 $n!$ 的质因数分解中质数 $p$ 的个数：

$$
1\cdot\left(\left\lfloor\dfrac{n}{p}\right\rfloor - \left\lfloor\dfrac{n}{p^2}\right\rfloor\right) + 2\cdot\left(\left\lfloor\dfrac{n}{p^2}\right\rfloor - \left\lfloor\dfrac{n}{p^3}\right\rfloor\right) + \cdots + (k-1)\cdot\left(\left\lfloor\dfrac{n}{p^{k-1}}\right\rfloor - \left\lfloor\dfrac{n}{p^k}\right\rfloor\right) + k\cdot \left\lfloor\dfrac{n}{p^{k}}\right\rfloor
$$

化简得

$$
\left\lfloor\dfrac{n}{p}\right\rfloor + \left\lfloor\dfrac{n}{p^2}\right\rfloor + \cdots + \left\lfloor\dfrac{n}{p^k}\right\rfloor
$$

由于 $p$ 越大，上式越小，所以 $n!$ 中的质因子 $5$ 的个数比 $2$ 少，我们只需计算上式 $p=5$ 的结果。

此外，有如下恒等式

$$
\left\lfloor\dfrac{n}{p^k}\right\rfloor = \left\lfloor\dfrac{ n/{p^{k-1}} }{p}\right\rfloor = \left\lfloor\dfrac{\lfloor n/{p^{k-1}} \rfloor}{p}\right\rfloor
$$

证明见 [下取整恒等式及其应用](https://zhuanlan.zhihu.com/p/1893240318645732760)。

注意该恒等式是一个递推式，我们可以用 $\left\lfloor\dfrac{n}{p}\right\rfloor$ 算出 $\left\lfloor\dfrac{n}{p^2}\right\rfloor$，用 $\left\lfloor\dfrac{n}{p^2}\right\rfloor$ 算出 $\left\lfloor\dfrac{n}{p^3}\right\rfloor$，依此类推。

```py [sol-Python3]
class Solution:
    def trailingZeroes(self, n: int) -> int:
        ans = 0
        while n:
            # 循环 k 次后，n 变成了 floor(n/5^k)
            n //= 5
            ans += n
        return ans
```

```java [sol-Java]
class Solution {
    public int trailingZeroes(int n) {
        int ans = 0;
        while (n > 0) {
            // 循环 k 次后，n 变成了 floor(n/5^k)
            n /= 5;
            ans += n;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int trailingZeroes(int n) {
        int ans = 0;
        while (n) {
            // 循环 k 次后，n 变成了 floor(n/5^k)
            n /= 5;
            ans += n;
        }
        return ans;
    }
};
```

```c [sol-C]
int trailingZeroes(int n) {
    int ans = 0;
    while (n) {
        // 循环 k 次后，n 变成了 floor(n/5^k)
        n /= 5;
        ans += n;
    }
    return ans;
}
```

```go [sol-Go]
func trailingZeroes(n int) (ans int) {
    for n > 0 {
        // 循环 k 次后，n 变成了 floor(n/5^k)
        n /= 5
        ans += n
    }
    return
}
```

```js [sol-JavaScript]
var trailingZeroes = function(n) {
    let ans = 0;
    while (n > 0) {
        // 循环 k 次后，n 变成了 floor(n/5^k)
        n = Math.floor(n / 5);
        ans += n;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn trailing_zeroes(mut n: i32) -> i32 {
        let mut ans = 0;
        while n > 0 {
            // 循环 k 次后，n 变成了 floor(n/5^k)
            n /= 5;
            ans += n;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [793. 阶乘函数后 K 个零](https://leetcode.cn/problems/preimage-size-of-factorial-zeroes-function/)

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、数论 / §1.4 阶乘分解`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.4 阶乘分解`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
