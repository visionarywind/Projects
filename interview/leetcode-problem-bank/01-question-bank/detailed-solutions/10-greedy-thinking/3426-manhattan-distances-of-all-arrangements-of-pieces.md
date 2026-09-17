# 3426. 所有安放棋子方案的曼哈顿距离

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/manhattan-distances-of-all-arrangements-of-pieces/
- 题目 slug：`manhattan-distances-of-all-arrangements-of-pieces`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.5 贡献法
- 难度分：2443
- 外部题解来源：https://leetcode.cn/problems/manhattan-distances-of-all-arrangements-of-pieces/solutions/3051398/gong-xian-fa-yu-chu-li-hou-o1pythonjavac-2hgt/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贡献法，预处理后 O(1)（Python/Java/C++/Go）](https://leetcode.cn/problems/manhattan-distances-of-all-arrangements-of-pieces/solutions/3051398/gong-xian-fa-yu-chu-li-hou-o1pythonjavac-2hgt/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`gong-xian-fa-yu-chu-li-hou-o1pythonjavac-2hgt`
- topic id：`3051398`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

本质上，答案是一堆 $|x_i-x_j|+|y_i-y_j|$ 之和。拆分成 $|x_i-x_j|$ 之和、$|y_i-y_j|$ 之和。

看看示例 2 是怎么算的。答案包含 $|1-0|$，$|3-1|$ 等式子。想一想，$|1-0|$ 在答案中出现了多少次？

![3426.png](https://pic.leetcode.cn/1737249404-HvZLla-3426.png)

出现了 $2$ 次（图一和图二）。为什么？因为当我们在前两个位置放置棋子后，剩余的两个位置还可以放剩余的一个棋子，方案数为组合数 $\binom {4-2} {3-2} = \binom 2 1 = 2$。

继续，有这样几类式子：

- $|1-0|,|2-1|,|3-2|$，结果为 $1$。
- $|2-0|,|3-1|$，结果为 $2$。
- $|3-0|$，结果为 $3$。

每个式子在答案中的出现次数都是 $2$，所以示例 2 的答案为

$$
(1\cdot 3 + 2\cdot 2 + 3\cdot 1)\cdot \binom 2 1 = 20
$$

一般地，如果 $m=1$，那么绝对差为 $d$ 的式子 $|x_i-x_j|$ 有 $n-d$ 种，每个式子的出现次数为 $\binom {mn-2} {k-2}$，表示在其余 $mn-2$ 个位置中选择 $k-2$ 个位置放置剩余的 $k-2$ 个棋子。

所有 $|x_i-x_j|$ 之和为

$$
\binom {mn-2} {k-2} \sum_{d=1}^{n-1} d\cdot(n-d)
$$

推广到 $m$ 为任意数的情况。由于两个棋子处于同一列的情况下 $|x_i-x_j|=0$，所以只考虑两个棋子不同列的情况，那么每个棋子都可以在 $m$ 行中任选一行放置，所以上式要额外乘以 $m^2$，即

$$
\binom {mn-2} {k-2} m^2 \sum_{d=1}^{n-1} d\cdot(n-d)
$$

这就是所有 $|x_i-x_j|$ 之和。

同理，所有 $|y_i-y_j|$ 之和为

$$
\binom {mn-2} {k-2} n^2 \sum_{d=1}^{m-1} d\cdot(m-d)
$$

进一步地，

$$
\begin{aligned}
    & \sum_{d=1}^{n-1} d\cdot(n-d)      \\
={} & \sum_{d=1}^{n-1} (nd-d^2)        \\
={} & n\sum_{d=1}^{n-1} d- \sum_{d=1}^{n-1} d^2        \\
={} & n\cdot \dfrac{n(n-1)}{2}- \dfrac{n(n-1)(2n-1)}{6}        \\
={} & \dfrac{(n+1)n(n-1)}{6}        \\
={} & \binom {n+1} 3        \\
\end{aligned}
$$

所以最终答案为

$$
\binom {mn-2} {k-2} \left(m^2 \binom {n+1} 3 + n^2\binom {m+1} 3\right)
$$

## 代码实现

1. 关于组合数，我们需要预处理阶乘及其逆元，然后利用公式 $C(n,m) = \dfrac{n!}{m!(n-m)!}$ 计算。
2. 关于逆元的知识点，见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)，包含费马小定理的数学证明。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1xBwBeEEie/?t=18m01s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def distanceSum(self, m: int, n: int, k: int) -> int:
        MOD = 1_000_000_007
        return comb(m * n - 2, k - 2) % MOD * (m * m * comb(n + 1, 3) + n * n * comb(m + 1, 3)) % MOD
```

```py [sol-Python3 预处理]
MOD = 1_000_000_007
MX = 100_000

fac = [0] * MX  # f[i] = i!
fac[0] = 1
for i in range(1, MX):
    fac[i] = fac[i - 1] * i % MOD

inv_f = [0] * MX  # inv_f[i] = i!^-1
inv_f[-1] = pow(fac[-1], -1, MOD)
for i in range(MX - 1, 0, -1):
    inv_f[i - 1] = inv_f[i] * i % MOD

def comb(n: int, m: int) -> int:
    return fac[n] * inv_f[m] * inv_f[n - m] % MOD

class Solution:
    def distanceSum(self, m: int, n: int, k: int) -> int:
        return comb(m * n - 2, k - 2) * (m * n * (m * (n * n - 1) + n * (m * m - 1))) // 6 % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;
    private static final int MX = 100_000;

    private static final long[] F = new long[MX]; // f[i] = i!
    private static final long[] INV_F = new long[MX]; // inv_f[i] = i!^-1

    static {
        F[0] = 1;
        for (int i = 1; i < MX; i++) {
            F[i] = F[i - 1] * i % MOD;
        }

        INV_F[MX - 1] = pow(F[MX - 1], MOD - 2);
        for (int i = MX - 1; i > 0; i--) {
            INV_F[i - 1] = INV_F[i] * i % MOD;
        }
    }

    public int distanceSum(int m, int n, int k) {
        return (int) ((m * n * (m * ((long) n * n - 1) + n * ((long) m * m - 1))) / 6 % MOD * comb(m * n - 2, k - 2) % MOD);
    }

    private long comb(int n, int m) {
        return F[n] * INV_F[m] % MOD * INV_F[n - m] % MOD;
    }

    private static long pow(long x, int n) {
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
const int MOD = 1'000'000'007;
const int MX = 100'000;

long long F[MX]; // F[i] = i!
long long INV_F[MX]; // INV_F[i] = i!^-1

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

auto init = [] {
    F[0] = 1;
    for (int i = 1; i < MX; i++) {
        F[i] = F[i - 1] * i % MOD;
    }

    INV_F[MX - 1] = pow(F[MX - 1], MOD - 2);
    for (int i = MX - 1; i; i--) {
        INV_F[i - 1] = INV_F[i] * i % MOD;
    }
    return 0;
}();

long long comb(int n, int m) {
    return F[n] * INV_F[m] % MOD * INV_F[n - m] % MOD;
}

class Solution {
public:
    int distanceSum(int m, int n, int k) {
        return (m * n * (m * (1LL * n * n - 1) + n * (1LL * m * m - 1))) / 6 % MOD * comb(m * n - 2, k - 2) % MOD;
    }
};
```

```go [sol-Go]
const mod = 1_000_000_007
const mx = 100_000

var f [mx]int    // f[i] = i!
var invF [mx]int // invF[i] = i!^-1

func init() {
	f[0] = 1
	for i := 1; i < mx; i++ {
		f[i] = f[i-1] * i % mod
	}

	invF[mx-1] = pow(f[mx-1], mod-2)
	for i := mx - 1; i > 0; i-- {
		invF[i-1] = invF[i] * i % mod
	}
}

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

func comb(n, m int) int {
	return f[n] * invF[m] % mod * invF[n-m] % mod
}

func distanceSum(m, n, k int) int {
	return (m * n * (m*(n*n-1) + n*(m*m-1))) / 6 % mod * comb(m*n-2, k-2) % mod
}
```

#### 复杂度分析

忽略预处理的时间和空间。

- 时间复杂度：$\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。

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
9. 【本题相关】[数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `五、思维题 / §5.5 贡献法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.5 贡献法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
