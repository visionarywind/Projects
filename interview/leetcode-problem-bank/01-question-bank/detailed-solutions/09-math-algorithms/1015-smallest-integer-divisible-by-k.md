# 1015. 可被 K 整除的最小整数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/smallest-integer-divisible-by-k/
- 题目 slug：`smallest-integer-divisible-by-k`
- 来源专题：数学算法
- 来源分类路径：一、数论 / §1.12 其他
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/smallest-integer-divisible-by-k/solutions/2263780/san-chong-suan-fa-you-hua-pythonjavacgo-tk4cj/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[三种算法+优化（Python/Java/C++/Go）](https://leetcode.cn/problems/smallest-integer-divisible-by-k/solutions/2263780/san-chong-suan-fa-you-hua-pythonjavacgo-tk4cj/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`san-chong-suan-fa-you-hua-pythonjavacgo-tk4cj`
- topic id：`2263780`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 前置知识：模运算

如果让你计算 $1234\cdot 6789$ 的**个位数**，你会如何计算？

由于只有个位数会影响到乘积的个位数，那么 $4\cdot 9=36$ 的个位数 $6$ 就是答案。

对于 $1234+6789$ 的个位数，同理，$4+9=13$ 的个位数 $3$ 就是答案。

你能把这个结论抽象成数学等式吗？

一般地，涉及到取模的题目，通常会用到如下等式（上面计算的是 $m=10$）：

$$
(a+b)\bmod m = ((a\bmod m) + (b\bmod m)) \bmod m
$$

$$
(a\cdot b) \bmod m=((a\bmod m)\cdot  (b\bmod m)) \bmod m
$$

证明：根据**带余除法**，任意整数 $a$ 都可以表示为 $a=km+r$，这里 $r$ 相当于 $a\bmod m$。那么设 $a=k_1m+r_1,\ b=k_2m+r_2$。

第一个等式：

$$
\begin{aligned}
&(a+b) \bmod m\\
=&((k_1+k_2) m+r_1+r_2)\bmod m\\
=&(r_1+r_2)\bmod m\\
=&((a\bmod m) + (b\bmod m)) \bmod m
\end{aligned}
$$

第二个等式：

$$
\begin{aligned}
&(a\cdot b) \bmod m\\
=&(k_1k_2m^2+(k_1r_2+k_2r_1)m+r_1r_2)\bmod m\\
=&(r_1r_2)\bmod m\\
=&((a\bmod m)\cdot  (b\bmod m)) \bmod m
\end{aligned}
$$

更多有关模运算的知识，请看 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

## 思路

![1015-3.png](https://pic.leetcode.cn/1683680206-OGDZGf-1015-3.png)

### 算法一（无优化）

```py [sol-Python3]
class Solution:
    def smallestRepunitDivByK(self, k: int) -> int:
        seen = set()
        x = 1 % k
        while x and x not in seen:
            seen.add(x)
            x = (x * 10 + 1) % k
        return -1 if x else len(seen) + 1
```

```java [sol-Java]
class Solution {
    public int smallestRepunitDivByK(int k) {
        var seen = new HashSet<Integer>();
        int x = 1 % k;
        while (x > 0 && seen.add(x)) {
            x = (x * 10 + 1) % k;
        }
        return x > 0 ? -1 : seen.size() + 1;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int smallestRepunitDivByK(int k) {
        unordered_set<int> seen;
        int x = 1 % k;
        while (x && seen.insert(x).second) {
            x = (x * 10 + 1) % k;
        }
        return x ? -1 : seen.size() + 1;
    }
};
```

```go [sol-Go]
func smallestRepunitDivByK(k int) int {
    seen := map[int]bool{}
    x := 1 % k
    for x > 0 && !seen[x] {
        seen[x] = true
        x = (x*10 + 1) % k
    }
    if x > 0 {
        return -1
    }
    return len(seen) + 1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k)$。
- 空间复杂度：$\mathcal{O}(k)$。

### 算法二+优化

```py [sol-Python3]
class Solution:
    def smallestRepunitDivByK(self, k: int) -> int:
        if k % 2 == 0 or k % 5 == 0:
            return -1
        x = 1 % k
        for i in count(1):  # 一定有解
            if x == 0:
                return i
            x = (x * 10 + 1) % k
```

```java [sol-Java]
class Solution {
    public int smallestRepunitDivByK(int k) {
        if (k % 2 == 0 || k % 5 == 0) {
            return -1;
        }
        int x = 1 % k;
        for (int i = 1; ; i++) { // 一定有解
            if (x == 0) {
                return i;
            }
            x = (x * 10 + 1) % k;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int smallestRepunitDivByK(int k) {
        if (k % 2 == 0 || k % 5 == 0) {
            return -1;
        }
        int x = 1 % k;
        for (int i = 1; ; i++) { // 一定有解
            if (x == 0) {
                return i;
            }
            x = (x * 10 + 1) % k;
        }
    }
};
```

```go [sol-Go]
func smallestRepunitDivByK(k int) int {
    if k%2 == 0 || k%5 == 0 {
        return -1
    }
    x := 1 % k
    for i := 1; ; i++ { // 一定有解
        if x == 0 {
            return i
        }
        x = (x*10 + 1) % k
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k)$。
- 空间复杂度：$\mathcal{O}(1)$。

### 算法三

```py [sol-Python3]
# 计算欧拉函数（n 以内的与 n 互质的数的个数）
def phi(n: int) -> int:
    res = n
    i = 2
    while i * i <= n:
        if n % i == 0:
            res = res // i * (i - 1)
            while n % i == 0:
                n //= i
        i += 1
    if n > 1:
        res = res // n * (n - 1)
    return res

class Solution:
    def smallestRepunitDivByK(self, k: int) -> int:
        if k % 2 == 0 or k % 5 == 0:
            return -1

        m = phi(k * 9)

        # 从小到大枚举不超过 sqrt(m) 的因子
        i = 1
        while i * i <= m:
            if m % i == 0 and pow(10, i, k * 9) == 1:
                return i
            i += 1

        # 从小到大枚举不低于 sqrt(m) 的因子
        i -= 1
        while True:
            if m % i == 0 and pow(10, m // i, k * 9) == 1:
                return m // i
            i -= 1
```

```java [sol-Java]
class Solution {
    public int smallestRepunitDivByK(int k) {
        if (k % 2 == 0 || k % 5 == 0) {
            return -1;
        }

        int m = phi(k * 9);

        // 从小到大枚举不超过 sqrt(m) 的因子
        int i = 1;
        for (; i * i <= m; i++) {
            if (m % i == 0 && pow(10, i, k * 9) == 1) {
                return i;
            }
        }
      
        // 从小到大枚举不低于 sqrt(m) 的因子
        for (i--; ; i--) {
            if (m % i == 0 && pow(10, m / i, k * 9) == 1) {
                return m / i;
            }
        }
    }

    // 计算欧拉函数（n 以内的与 n 互质的数的个数）
    private int phi(int n) {
        int res = n;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                res = res / i * (i - 1);
                while (n % i == 0) {
                    n /= i;
                }
            }
        }
        if (n > 1) {
            res = res / n * (n - 1);
        }
        return res;
    }

    // 快速幂，返回 pow(x, n) % mod
    private long pow(long x, int n, long mod) {
        long res = 1;
        for (; n > 0; n /= 2) {
            if (n % 2 > 0) {
                res = res * x % mod;
            }
            x = x * x % mod;
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
    // 计算欧拉函数（n 以内的与 n 互质的数的个数）
    int phi(int n) {
        int res = n;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                res = res / i * (i - 1);
                while (n % i == 0) {
                    n /= i;
                }
            }
        }
        if (n > 1) {
            res = res / n * (n - 1);
        }
        return res;
    }

    // 快速幂，返回 pow(x, n) % mod
    long long pow(long long x, int n, long long mod) {
        long long res = 1;
        for (; n; n /= 2) {
            if (n % 2) {
                res = res * x % mod;
            }
            x = x * x % mod;
        }
        return res;
    }

public:
    int smallestRepunitDivByK(int k) {
        if (k % 2 == 0 || k % 5 == 0) {
            return -1;
        }

        int m = phi(k * 9);

        // 从小到大枚举不超过 sqrt(m) 的因子
        int i = 1;
        for (; i * i <= m; i++) {
            if (m % i == 0 && pow(10, i, k * 9) == 1) {
                return i;
            }
        }

        // 从小到大枚举不低于 sqrt(m) 的因子
        for (i--; ; i--) {
            if (m % i == 0 && pow(10, m / i, k * 9) == 1) {
                return m / i;
            }
        }
    }
};
```

```go [sol-Go]
// 计算欧拉函数（n 以内的与 n 互质的数的个数）
func phi(n int) int {
    res := n
    for i := 2; i*i <= n; i++ {
        if n%i == 0 {
            res = res / i * (i - 1)
            for n /= i; n%i == 0; n /= i {
            }
        }
    }
    if n > 1 {
        res = res / n * (n - 1)
    }
    return res
}

// 快速幂，返回 pow(x, n) % mod
func pow(x, n, mod int) int {
    x %= mod
    res := 1
    for ; n > 0; n /= 2 {
        if n%2 > 0 {
            res = res * x % mod
        }
        x = x * x % mod
    }
    return res
}

func smallestRepunitDivByK(k int) int {
    if k%2 == 0 || k%5 == 0 {
        return -1
    }

    m := phi(k * 9)

    // 从小到大枚举不超过 sqrt(m) 的因子
    i := 1
    for ; i*i <= m; i++ {
        if m%i == 0 && pow(10, i, k*9) == 1 {
            return i
        }
    }

    // 从小到大枚举不低于 sqrt(m) 的因子
    for i--; ; i-- {
        if m%i == 0 && pow(10, m/i, k*9) == 1 {
            return m / i
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\sqrt{k}\log k)$。计算 $\varphi(9k)$ 和枚举 $\varphi(9k)$ 的因子都需要 $\mathcal{O}(\sqrt{k})$ 的时间，对每个因子计算快速幂需要 $\mathcal{O}(\log k)$ 的时间，所以时间复杂度为 $\mathcal{O}(\sqrt{k}\log k)$。
- 空间复杂度：$\mathcal{O}(1)$。

### 思考题

对于算法三，把题目中的 $1$ 改成 $2$ 要怎么做？改成其它数字要怎么做？

欢迎在评论区分享你的思路/代码。

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

本题来自 `一、数论 / §1.12 其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、数论 / §1.12 其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
