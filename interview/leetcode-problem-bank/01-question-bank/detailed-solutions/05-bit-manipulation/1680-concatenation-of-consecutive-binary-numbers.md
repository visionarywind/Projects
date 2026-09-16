# 1680. 连接连续二进制数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/concatenation-of-consecutive-binary-numbers/
- 题目 slug：`concatenation-of-consecutive-binary-numbers`
- 来源专题：位运算
- 来源分类路径：九、其他
- 难度分：1630
- 外部题解来源：https://leetcode.cn/problems/concatenation-of-consecutive-binary-numbers/solutions/511016/golang-jian-ji-xie-fa-by-endlesscheng-2cg5/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：从 O(n) 暴力到 O(log^2 n) 数学公式（Python/Java/C++/Go）](https://leetcode.cn/problems/concatenation-of-consecutive-binary-numbers/solutions/511016/golang-jian-ji-xie-fa-by-endlesscheng-2cg5/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`golang-jian-ji-xie-fa-by-endlesscheng-2cg5`
- topic id：`511016`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：暴力

我们需要把二进制数 $1,10,11,100,101,110,111,\ldots,n$ 依次连接，然后把得到的结果模 $10^9+7$。

如何连接？比如把 $11011$ 和 $100$ 连接起来，我们可以先把 $11011$ 左移 $3$ 位（$3$ 是 $100$ 的长度），得到 $11011000$，然后和 $100$ 计算或运算（也可以用加法），得到 $11011100$。

注意取模。代码实现时，可以在循环内取模。为什么可以在中途取模？原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

```py [sol-Python3]
class Solution:
    def concatenatedBinary(self, n: int) -> int:
        MOD = 1_000_000_007
        ans = 0
        for i in range(1, n + 1):
            w = i.bit_length()
            ans = (ans << w | i) % MOD
        return ans
```

```java [sol-Java]
class Solution {
    public int concatenatedBinary(int n) {
        final int MOD = 1_000_000_007;
        long ans = 0;
        for (int i = 1; i <= n; i++) {
            int w = 32 - Integer.numberOfLeadingZeros(i);
            ans = (ans << w | i) % MOD;
        }
        return (int) ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int concatenatedBinary(int n) {
        constexpr int MOD = 1'000'000'007;
        long long ans = 0;
        for (int i = 1; i <= n; i++) {
            int w = bit_width((uint32_t) i);
            ans = (ans << w | i) % MOD;
        }
        return ans;
    }
};
```

```go [sol-Go]
func concatenatedBinary(n int) (ans int) {
	const mod = 1_000_000_007
	for i := 1; i <= n; i++ {
		w := bits.Len(uint(i))
		ans = (ans<<w | i) % mod
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：数学公式

比如 $n=9$，我们可以把 $[1,n]$ 中的数按照二进制长度，分为如下几组：

- $[1,1]$，二进制长度均为 $1$。
- $[2,3]$，二进制长度均为 $2$。
- $[4,7]$，二进制长度均为 $3$。
- $[8,9]$，二进制长度均为 $4$。

我们可以**分别计算每一组**连接后的值，最后再把这些值连接起来。

比如 $[4,7]$ 这一组，对应的二进制数分别为 $100,101,110,111$，连接后的值为

$$
100101110111
$$

最左边的 $100$ 是 $4$ 左移 $3\cdot 3 = 9$ 位后的结果，随后的 $101$ 是 $5$ 左移 $3\cdot 2 = 6$ 位后的结果，依此类推。

所以连接后的值，写成和式，即

$$
\begin{aligned}
    & 4\cdot 2^{3\cdot 3} + 5\cdot 2^{3\cdot 2} + 6\cdot 2^{3\cdot 1} + 7\cdot 2^{3\cdot 0}      \\
={} & 4\cdot 8^3 + 5\cdot 8^2 + 6\cdot 8^1 + 7\cdot 8^0        \\
\end{aligned}
$$

一般地，设 $L,L+1,L+2,\ldots, R$ 的二进制长度均为 $w$，设 $m = R-L+1$，设 $q = 2^w$，那么连接 $[L,R]$ 中的二进制整数，结果为

$$
\sum_{j=0}^{m-1} (L+j)\cdot q^{m-1-j}
$$

做变量替换 $i = m-1-j$，上式变形为

$$
\begin{aligned}
    & \sum_{i=0}^{m-1} (L+m-1-i)\cdot q^i      \\
={} & (L+m-1)\sum_{i=0}^{m-1}  q^i -  \sum_{i=0}^{m-1} i q^i      \\
={} & R\sum_{i=0}^{m-1}  q^i -  \sum_{i=0}^{m-1} i q^i      \\
\end{aligned}
$$

第一个和式，用**等比数列求和公式**，得

$$
\sum_{i=0}^{m-1}  q^i = \dfrac{q^m-1}{q-1}
$$

第二个和式，用**错位相减法**，得

$$
\sum_{i=0}^{m-1}  iq^i = \dfrac{q - mq^m + (m-1)q^{m+1}}{(q-1)^2}
$$

综上，连接 $[L,R]$ 中的二进制整数，结果为

$$
S = \dfrac{R(q^m-1)}{q-1} - \dfrac{q - mq^m + (m-1)q^{m+1}}{(q-1)^2}
$$

枚举 $w=1,2,3,\ldots$ 计算上式。在模运算下，除法运算需要求逆元，见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

把 $S$ 拼接在 $\textit{ans}$ 右边，需要把 $\textit{ans}$ 左移 $wm$ 位（有 $m$ 个长为 $w$ 的二进制数），也就是把 $\textit{ans}$ 乘以 $2^{wm} = q^m$。

```py [sol-Python3]
class Solution:
    def concatenatedBinary(self, n: int) -> int:
        MOD = 1_000_000_007
        ans = 0
        w = 1
        while (1 << (w - 1)) <= n:
            l = 1 << (w - 1)
            r = min((1 << w) - 1, n)
            m = r - l + 1
            q = 1 << w
            pow_q = pow(q, m, MOD)
            inv_q1 = pow(q - 1, -1, MOD)
            s = r * (pow_q - 1) * inv_q1 - (q - m * pow_q + (m - 1) * pow_q * q) * inv_q1 * inv_q1
            ans = ans * pow_q + s
            w += 1
        return ans % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int concatenatedBinary(int n) {
        long ans = 0;
        for (int w = 1; 1 << (w - 1) <= n; w++) {
            int l = 1 << (w - 1);
            int r = Math.min((1 << w) - 1, n);
            int m = r - l + 1;
            int q = 1 << w;
            long powQ = pow(q, m);
            long invQ1 = pow(q - 1, MOD - 2);
            long s = r * (powQ - 1) % MOD * invQ1 - (q - m * powQ + (m - 1) * powQ % MOD * q) % MOD * invQ1 % MOD * invQ1;
            ans = (ans * powQ + s) % MOD;
        }
        return (int) ((ans + MOD) % MOD); // 保证结果非负
    }

    private long pow(long x, int n) {
        long res = 1;
        for (; n > 0; n /= 2) {
            if (n % 2 > 0) {
                res = res * x % MOD;
            }
            x = x * x % MOD;
        }
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
    static constexpr int MOD = 1'000'000'007;

    long long pow(long long x, int n) {
        long long res = 1;
        for (; n; n /= 2) {
            if (n % 2) {
                res = res * x % MOD;
            }
            x = x * x % MOD;
        }
        return res;
    }

public:
    int concatenatedBinary(int n) {
        long long ans = 0;
        for (int w = 1; 1 << (w - 1) <= n; w++) {
            int l = 1 << (w - 1);
            int r = min((1 << w) - 1, n);
            int m = r - l + 1;
            int q = 1 << w;
            long long pow_q = pow(q, m);
            long long inv_q1 = pow(q - 1, MOD - 2);
            long long s = r * (pow_q - 1) % MOD * inv_q1 - (q - m * pow_q + (m - 1) * pow_q % MOD * q) % MOD * inv_q1 % MOD * inv_q1;
            ans = (ans * pow_q + s) % MOD;
        }
        return (ans + MOD) % MOD; // 保证结果非负
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007

func pow(x, n int) int {
	res := 1
	for ; n > 0; n /= 2 {
		if n%2 > 0 {
			res = res * x % mod
		}
		x = x * x % mod
	}
	return res
}

func concatenatedBinary(n int) (ans int) {
	for w := 1; 1<<(w-1) <= n; w++ {
		l := 1 << (w - 1)
		r := min(1<<w-1, n)
		m := r - l + 1
		q := 1 << w
		powQ := pow(q, m)
		invQ1 := pow(q-1, mod-2)
		s := r*(powQ-1)%mod*invQ1 - (q-m*powQ+(m-1)*powQ%mod*q)%mod*invQ1%mod*invQ1
		ans = (ans*powQ + s) % mod
	}
	return (ans + mod) % mod // 保证结果非负
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log^2 n)$。更准确地说是 $\mathcal{O}((\log n)(\log n + \log M))$，其中 $M=10^9+7$。
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
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `九、其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `九、其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
