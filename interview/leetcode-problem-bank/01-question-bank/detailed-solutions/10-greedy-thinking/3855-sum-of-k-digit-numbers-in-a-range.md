# 3855. 给定范围内 K 位数字之和

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-of-k-digit-numbers-in-a-range/
- 题目 slug：`sum-of-k-digit-numbers-in-a-range`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.5 贡献法
- 难度分：2085
- 外部题解来源：https://leetcode.cn/problems/sum-of-k-digit-numbers-in-a-range/solutions/3910668/gong-xian-fa-shu-xue-gong-shi-pythonjava-qek9/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贡献法，数学公式（Python/Java/C++/Go）](https://leetcode.cn/problems/sum-of-k-digit-numbers-in-a-range/solutions/3910668/gong-xian-fa-shu-xue-gong-shi-pythonjava-qek9/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`gong-xian-fa-shu-xue-gong-shi-pythonjava-qek9`
- topic id：`3910668`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

比如 $k=3$（三位数），我们在十位数上填了一个 $5$，它对答案的贡献是多少？有多少个三位数，其十位数上是 $5$？

如果 $\ell = 2$，$r = 5$，那么当十位数填 $5$ 的时候，百位数有 $r - \ell+1 = 4$ 种填法，个位数也有 $4$ 种填法，所以有 $4^2=16$ 个不同的三位数，其十位数上是 $5$。本题需要求出这些数的和，例如 $456 = 400 + 50 + 6$，我们可以把十位数上的 $5$ 看成是 $50$，这个 $50$ 出现在 $16$ 个不同的三位数中，所以十位数填 $5$ 对答案的贡献是 $50\times 16 = 800$。

一般地，在从低到高第 $i$ 位（$i$ 从 $0$ 开始）上填 $x\ (\ell \le x \le r)$，相当于填了一个 $x\cdot 10^i$。其余 $k-1$ 位，每一位都有 $r-\ell+1$ 种填法，一共有 $(r-\ell+1)^{k-1}$ 种填法，所以 $x$ 对答案的贡献为

$$
x\cdot 10^i\cdot(r-\ell+1)^{k-1}
$$

设 $m = r-\ell+1$。枚举 $x$ 和 $i$，答案为

$$
\begin{aligned}
    & \sum_{x=\ell}^{r}\sum_{i=0}^{k-1} x\cdot 10^i\cdot m^{k-1}      \\
={} & \left(\sum_{x=\ell}^{r} x\right)\left(\sum_{i=0}^{k-1} 10^i\right) m^{k-1}     \\
={} & \dfrac{(\ell + r)m}{2}\cdot \dfrac{10^k-1}{9}\cdot m^{k-1}     \\
\end{aligned}
$$

> 前两个和式分别是等差数列求和、等比数列求和。

其中计算 $10^k$ 和 $m^{k-1}$ 需要用**快速幂**，原理见[【图解】一张图秒懂快速幂](https://leetcode.cn/problems/powx-n/solution/tu-jie-yi-zhang-tu-miao-dong-kuai-su-mi-ykp3i/)。

在模运算下，除法运算需要计算**逆元**，见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

## 答疑

**问**：什么是贡献法？

**答**：答案本质是一堆数字相加，这里面有很多重复的数字。比如十位数填 $5$，就对应数字 $50$。统计出 $50$ 在「一堆数字相加」这个式子中出现了多少次，就是 $50$ 对答案的贡献，或者说对「一堆数字相加」这个式子的贡献。

[本题视频讲解](https://www.bilibili.com/video/BV1VvABz9EGz/?t=30m43s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def sumOfNumbers(self, l: int, r: int, k: int) -> int:
        MOD = 1_000_000_007
        m = r - l + 1
        return (l + r) * m * (pow(10, k, MOD) - 1) * pow(18, -1, MOD) * pow(m, k - 1, MOD) % MOD
```

```java [sol-Java]
class Solution {
    private static final int MOD = 1_000_000_007;

    public int sumOfNumbers(int l, int r, int k) {
        int m = r - l + 1;
        return (int) ((l + r) * m * (pow(10, k) - 1 + MOD) % MOD * pow(18, MOD - 2) % MOD * pow(m, k - 1) % MOD);
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
    int sumOfNumbers(int l, int r, int k) {
        int m = r - l + 1;
        return (l + r) * m * (pow(10, k) - 1 + MOD) % MOD * pow(18, MOD - 2) % MOD * pow(m, k - 1) % MOD;
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

func sumOfNumbers(l, r, k int) int {
	m := r - l + 1
	return (l + r) * m * (pow(10, k) - 1 + mod) % mod * pow(18, mod-2) % mod * pow(m, k-1) % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log k)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 专题训练

见下面思维题单的「**§5.5 贡献法**」。

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
