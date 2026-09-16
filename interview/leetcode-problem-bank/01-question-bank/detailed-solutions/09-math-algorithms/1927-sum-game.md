# 1927. 求和游戏

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/sum-game/
- 题目 slug：`sum-game`
- 来源专题：数学算法
- 来源分类路径：四、博弈论
- 难度分：2005
- 外部题解来源：https://leetcode.cn/problems/sum-game/solutions/4009715/liang-ren-de-ju-ti-ce-lue-yan-jin-zheng-iuo67/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两人的具体策略，严谨证明，附 Bob 获胜概率（Python/Java/C++/Go）](https://leetcode.cn/problems/sum-game/solutions/4009715/liang-ren-de-ju-ti-ce-lue-yan-jin-zheng-iuo67/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-ren-de-ju-ti-ce-lue-yan-jin-zheng-iuo67`
- topic id：`4009715`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

设 $\textit{num}$ 中有 $q$ 个问号。设 $\textit{num}$ 的左半数字和为 $S_L$，右半数字和为 $S_R$。

> 注：题目保证 $\textit{num}$ 的长度是偶数。

从最简单的情况开始思考。如果 $q=1$，Alice 会如何操作？

- 如果 $S_L = S_R$，那么 Alice 把问号改成任意非零数字，即可让 $S_L \ne S_R$。
- 如果 $S_L \ne S_R$，那么 Alice 把问号改成 $0$，即可让 $S_L \ne S_R$。

所以 $q=1$ 时，Alice 先手必胜。

该结论可以推广到 $q$ 是**奇数**的情况。无论前面两人如何操作，最后轮到 Alice，按照 $q=1$ 的操作方法，即可先手必胜。

如果 $q$ 是偶数呢？两人的必胜策略是什么样的？

依然从最简单的情况开始思考。如果 $q=2$，两人会如何操作？

- 如果左半右半各有一个问号。
    - 如果 $S_L = S_R$，那么 Alice 把问号改成 $x$，Bob **模仿**，把另一个问号也改成 $x$。最终 $S_L = S_R$，Bob 获胜。
    - 如果 $S_L > S_R$，那么 Alice 把左半中的问号改成 $9$，无论 Bob 怎么改，最终 $S_L > S_R$，Alice 获胜。
    - 如果 $S_L < S_R$，那么 Alice 把右半中的问号改成 $9$，无论 Bob 怎么改，最终 $S_L < S_R$，Alice 获胜。
- 如果两个问号都在左半。
    - 如果 $S_R - S_L = 9$，那么 Alice 把一个问号改成 $x$，Bob 只需把另一个问号改成 $9-x$，就可以使左半和恰好增加 $9$。最终 $S_L = S_R$，Bob 获胜。
    - 如果 $S_R - S_L > 9$，那么 Alice 把一个问号改成 $0$，无论 Bob 怎么改，最终 $S_R - S_L > 0$，Alice 获胜。
    - 如果 $S_R - S_L < 9$，那么 Alice 把一个问号改成 $9$，无论 Bob 怎么改，最终 $S_R - S_L < 0$，Alice 获胜。
- 两个问号都在右半的情况和上面类似，当且仅当 $S_L - S_R = 9$ 时 Bob 获胜。

又例如，如果 $q=4$ 且问号都在左半，那么 $S_R - S_L = 18$ 时 Bob 可以获胜：按照上面的方法，两人各操作一次，可以使左半和恰好增加 $9$。再各操作一次，就可以一共使左半和恰好增加 $18$。最终 Bob 获胜。

设左半的问号个数为 $q_L$，右半的问号个数为 $q_R$。根据上面的讨论，提出如下猜想：

**猜想**：当且仅当 $S_R - S_L = \dfrac{q_L - q_R}{2}\cdot 9$ 成立，Bob 获胜。

> 注：这里无需强调 $q$ 是偶数。如果 $q$ 是奇数，那么 $q - 2q_R = (q_L + q_R) - 2q_R = q_L - q_R$ 也是奇数（奇数减偶数还是奇数），上式一定不成立（上式等号左边是整数，右边是小数）。

**证明**：

设 $D = S_L - S_R + \dfrac{q_L - q_R}{2}\cdot 9$。

如果 $D = 0$，那么 Bob 按照如下策略，可以始终保持 Bob 操作后 $D = 0$，最终 $S_L - S_R = 0$，从而获胜。

- 如果 Alice 把左半中的一个问号改成 $x$，那么 Bob 可以：
    - 把右半中的一个问号改成 $x$，那么 $D' = D + x - x + 0 = D = 0$。注：由于左半右半各少了一个问号，所以 $\dfrac{q_L - q_R}{2}\cdot 9$ 的变化量为 $0$。
    - 或者，把左半中的一个问号改成 $9-x$，那么 $D' = D + x + (9-x) - 9 = D = 0$。注：由于左半少了两个问号，所以 $\dfrac{q_L - q_R}{2}\cdot 9$ 的变化量为 $-9$。
- 如果 Alice 修改了右半中的问号，Bob 的做法类似。

如果 $D > 0$，那么 Alice 按照如下策略，可以保持 $D > 0$，从而获胜。

- 如果 $q_L > 0$，那么 Alice 把左半的一个问号改成 $9$。
    - 如果 Bob 把右半的一个问号改成 $y$，那么 $D' = D + 9 - y + 0 \ge D > 0$。
    - 如果 Bob 把左半的一个问号改成 $y$，那么 $D' = D + 9 + y - 9 \ge D > 0$。
- 如果 $q_L = 0$，那么 Alice 把右半的一个问号改成 $0$，Bob 把右半的一个问号改成 $y$，那么 $D' = D - 0 - y + 9 \ge D > 0$。

如果 $D < 0$，那么 Alice 按照如下策略，可以保持 $D < 0$，从而获胜。

- 如果 $q_L > 0$，那么 Alice 把左半的一个问号改成 $0$。
  - 如果 Bob 把右半的一个问号改成 $y$，那么 $D' = D + 0 - y + 0 \le D < 0$。
  - 如果 Bob 把左半的一个问号改成 $y$，那么 $D' = D + 0 + y - 9 \le D < 0$。
- 如果 $q_L = 0$，那么 Alice 把右半的一个问号改成 $9$，Bob 把右半的一个问号改成 $y$，那么 $D' = D - 9 - y + 9 \le D < 0$。

## 写法一

```py [sol-Python3]
class Solution:
    def calc(self, s: str) -> tuple[int, int]:
        q = sum_ = 0
        for ch in s:
            if ch == '?':
                q += 1
            else:
                sum_ += int(ch)
        return q, sum_

    def sumGame(self, num: str) -> bool:
        n = len(num)
        ql, sum_l = self.calc(num[:n // 2])
        qr, sum_r = self.calc(num[n // 2:])
        return (ql + qr) % 2 > 0 or (ql - qr) // 2 * 9 != sum_r - sum_l
```

```java [sol-Java]
class Solution {
    public boolean sumGame(String num) {
        int n = num.length();
        int[] left = calc(num.substring(0, n / 2));
        int[] right = calc(num.substring(n / 2));
        int ql = left[0], sumL = left[1];
        int qr = right[0], sumR = right[1];
        return (ql + qr) % 2 > 0 || (ql - qr) / 2 * 9 != sumR - sumL;
    }

    private int[] calc(String s) {
        int q = 0;
        int sum = 0;
        for (char ch : s.toCharArray()) {
            if (ch == '?') {
                q++;
            } else {
                sum += ch - '0';
            }
        }
        return new int[]{q, sum};
    }
}
```

```cpp [sol-C++]
class Solution {
    pair<int, int> calc(string_view s) {
        int q = 0, sum = 0;
        for (char ch : s) {
            if (ch == '?') {
                q++;
            } else {
                sum += ch - '0';
            }
        }
        return {q, sum};
    }

public:
    bool sumGame(string num) {
        int n = num.size();
        string_view s(num); // string_view 的 substr 是 O(1) 切片
        auto [ql, sum_l] = calc(s.substr(0, n / 2));
        auto [qr, sum_r] = calc(s.substr(n / 2));
        return (ql + qr) % 2 || (ql - qr) / 2 * 9 != sum_r - sum_l;
    }
};
```

```go [sol-Go]
func calc(s string) (q, sum int) {
	for _, ch := range s {
		if ch == '?' {
			q++
		} else {
			sum += int(ch - '0')
		}
	}
	return
}

func sumGame(num string) bool {
	n := len(num)
	ql, sumL := calc(num[:n/2])
	qr, sumR := calc(num[n/2:])
	return (ql+qr)%2 > 0 || (ql-qr)/2*9 != sumR-sumL
}
```

## 写法二

把 Bob 获胜的充要条件 $S_R - S_L = \dfrac{q_L - q_R}{2}\cdot 9$ 两边同乘以 $2$，移项得

$$
9q_L + 2S_L = 9q_R + 2S_R
$$

由上式可知，我们可以把问号视作 $9$，作为问号字符的权值。数字字符的权值等于该数字的值乘以 $2$。

如果左半权值和等于右半权值和，那么 Bob 获胜，否则 Alice 获胜。

代码实现时，可以用 $d$ 表示左半权值和减右半权值和，最后判断 $d\ne 0$ 是否成立。 

```py [sol-Python3]
class Solution:
    def sumGame(self, num: str) -> bool:
        m = len(num) // 2
        d = 0
        for i, ch in enumerate(num):
            x = int(ch) * 2 if ch != '?' else 9
            d += x if i < m else -x
        return d != 0
```

```java [sol-Java]
class Solution {
    public boolean sumGame(String num) {
        char[] s = num.toCharArray();
        int n = s.length;
        int d = 0;
        for (int i = 0; i < n; i++) {
            int x = s[i] == '?' ? 9 : (s[i] - '0') * 2;
            d += i < n / 2 ? x : -x;
        }
        return d != 0;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool sumGame(string num) {
        int n = num.size();
        int d = 0;
        for (int i = 0; i < n; i++) {
            int x = num[i] == '?' ? 9 : (num[i] - '0') * 2;
            d += i < n / 2 ? x : -x;
        }
        return d != 0;
    }
};
```

```go [sol-Go]
func sumGame(num string) bool {
	m := len(num) / 2
	d := 0
	for i, ch := range num {
		x := 9
		if ch != '?' {
			x = int(ch-'0') * 2
		}
		if i < m {
			d += x
		} else {
			d -= x
		}
	}
	return d != 0
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{num}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$ 或 $\mathcal{O}(n)$，取决于实现。

## 思考题：Bob 获胜的概率

在随机数据下，Bob 获胜的概率与 $n$ 的近似关系是什么？其中 $n$ 是 $\textit{num}$ 的长度。

定义随机变量 $X_i$ 为左半第 $i$ 个字符的权值，$Y_i$ 为右半第 $i$ 个字符的权值。字符权值的定义见写法二。

计算期望与方差

$$
\text{E}[X_i] = \dfrac{1}{11} \left(9 + \sum_{k=0}^9 2k \right) = 9
$$

$$
\text{E}[X_i^2] = \dfrac{1}{11} \left(9^2 + \sum_{k=0}^9 (2k)^2 \right) = 111
$$

$$
\operatorname{Var}(X_i) = \text{E}[X_i^2] - (\text{E}[X_i])^2 = 111 - 9^2 = 30
$$

设 $Z_i = X_i - Y_i$。由于 $X_i$ 与 $Y_i$ 独立同分布，所以有

$$
\text{E}[Z_i] = \text{E}[X_i] - \text{E}[Y_i] = 0
$$

$$
\operatorname{Var}(Z_i) = \operatorname{Var}(X_i) + \operatorname{Var}(Y_i) = 60
$$

设 $m = n/2$，$S = \sum\limits_{i=1}^m Z_i$。Bob 获胜的概率即 $P(S=0)$。

对于 $S$，我们有

$$
\text{E}[S] = 0
$$

$$
\operatorname{Var}(S) = 60m
$$

由于 $S$ 是 $m$ 个独立同分布随机变量的和，根据中心极限定理，$S$ 近似服从正态分布 $\mathcal{N}(0, 60m)$，误差 $\mathcal{O}(1/m)$。

其概率密度函数为

$$
f(x) = \dfrac{1}{\sqrt{120\pi m}} e^{-\frac{x^2}{120m}}
$$

如何用 $f(x)$ 计算 $P(S=0)$？前者是连续的，后者是个离散的点概率。

把单点 $S=0$ 扩展为连续区间 $-0.5\le S\le 0.5$（连续性修正），从而得到

$$
P(S=0)\approx \int_{-0.5}^{0.5} f(x)\,\mathrm{d}x
$$

由于 $e$ 的指数 $-\frac{x^2}{120m}$ 很小，根据泰勒展开有 $e^{-\frac{x^2}{120m}} \approx 1-\frac{x^2}{120m}$（这意味着 $f(x)$ 在 $[-0.5,0.5]$ 中几乎是条水平直线），所以有

$$
\int_{-0.5}^{0.5} f(x)\,\mathrm{d}x = \dfrac{1}{\sqrt{120\pi m}} + \mathcal{O}(m^{-3/2})
$$

所以，当 $n$ 较大时：

$$
\boxed{P(\text{Bob 获胜})\approx \frac{1}{\sqrt{120\pi m}} = \frac{1}{\sqrt{60\pi n}}}
$$

## 专题训练

见下面数学题单的「**四、博弈论**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、博弈论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、博弈论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
