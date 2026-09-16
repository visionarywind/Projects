# 2513. 最小化两个数组中的最大值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimize-the-maximum-of-two-arrays/
- 题目 slug：`minimize-the-maximum-of-two-arrays`
- 来源专题：数学算法
- 来源分类路径：二、组合数学 / §2.3 容斥原理
- 难度分：2302
- 外部题解来源：https://leetcode.cn/problems/minimize-the-maximum-of-two-arrays/solutions/2031827/er-fen-da-an-by-endlesscheng-y8fp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：二分答案 / 数学公式](https://leetcode.cn/problems/minimize-the-maximum-of-two-arrays/solutions/2031827/er-fen-da-an-by-endlesscheng-y8fp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-by-endlesscheng-y8fp`
- topic id：`2031827`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：二分答案

看到「最小化最大值」就要先尝试二分答案，这是因为：

- 最大元素越大，约束条件越宽松，越能够满足要求。
- 最大元素越小，约束条件越严苛，越不能满足要求。

下文把 $\textit{divisor}_1$ 和 $\textit{divisor}_2$ 简写成 $d_1$ 和 $d_2$，记 $\textit{LCM}$ 为 $d_1$ 和 $d_2$ 的最小公倍数。

由于：

- 能被 $d_2$ 整除但不能被 $d_1$ 整除的数，能在 $\textit{arr}_1$ 中且不能在 $\textit{arr}_2$ 中；
- 能被 $d_1$ 整除但不能被 $d_2$ 整除的数，能在 $\textit{arr}_2$ 中且不能在 $\textit{arr}_1$ 中；
- 既不能被 $d_1$ 整除也不能被 $d_2$ 整除的数，可以在 $\textit{arr}_1$ 和 $\textit{arr}_2$ 中。

因此二分答案 $x$，则：

- 有 $\left\lfloor\dfrac{x}{d_2}\right\rfloor - \left\lfloor\dfrac{x}{\textit{LCM}}\right\rfloor$ 个数是 $\textit{arr}_1$ 独享的；
- 有 $\left\lfloor\dfrac{x}{d_1}\right\rfloor - \left\lfloor\dfrac{x}{\textit{LCM}}\right\rfloor$ 个数是 $\textit{arr}_2$ 独享的；
- 有 $x - \left\lfloor\dfrac{x}{d_1}\right\rfloor - \left\lfloor\dfrac{x}{d_2}\right\rfloor + \left\lfloor\dfrac{x}{\textit{LCM}}\right\rfloor$ 个数（根据容斥原理）是 $\textit{arr}_1$ 和 $\textit{arr}_2$ 共享的。

去掉独享的，剩余的数字只能在共享中选择，因此二分判定条件为

$$
x - \left\lfloor\dfrac{x}{d_1}\right\rfloor - \left\lfloor\dfrac{x}{d_2}\right\rfloor + \left\lfloor\dfrac{x}{\textit{LCM}}\right\rfloor \ge \max\left(\textit{uniqueCnt}_1 - \left\lfloor\dfrac{x}{d_2}\right\rfloor + \left\lfloor\dfrac{x}{\textit{LCM}}\right\rfloor, 0\right) + \max\left(\textit{uniqueCnt}_2 - \left\lfloor\dfrac{x}{d_1}\right\rfloor + \left\lfloor\dfrac{x}{\textit{LCM}}\right\rfloor, 0\right)
$$

代码实现时，由于 $d_i$ 越小，不能在数组中的数越多，数组的最大元素越大，所以最坏情况下 $d_1=d_2=2$，只能取奇数。此时二分上界为 $(\textit{uniqueCnt}_1 + \textit{uniqueCnt}_2)\cdot 2-1$。

关于二分算法的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

[视频讲解](https://www.bilibili.com/video/BV1Dd4y1h72z/) 第三题。

```py [sol-Python3]
class Solution:
    def minimizeSet(self, d1: int, d2: int, uniqueCnt1: int, uniqueCnt2: int) -> int:
        l = lcm(d1, d2)
        def check(x: int) -> bool:
            left1 = max(uniqueCnt1 - x // d2 + x // l, 0)
            left2 = max(uniqueCnt2 - x // d1 + x // l, 0)
            common = x - x // d1 - x // d2 + x // l
            return common >= left1 + left2
        return bisect_left(range((uniqueCnt1 + uniqueCnt2) * 2 - 1), True, key=check)
```

```go [sol-Go]
func minimizeSet(d1, d2, uniqueCnt1, uniqueCnt2 int) int {
	l := lcm(d1, d2)
	return sort.Search((uniqueCnt1+uniqueCnt2)*2-1, func(x int) bool {
		left1 := max(uniqueCnt1-x/d2+x/l, 0)
		left2 := max(uniqueCnt2-x/d1+x/l, 0)
		common := x - x/d1 - x/d2 + x/l
		return common >= left1+left2
	})
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}

func lcm(a, b int) int {
	return a / gcd(a, b) * b
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log \max(\textit{divisor}_1,\textit{divisor}_2) + \log (\textit{uniqueCnt}_1+\textit{uniqueCnt}_2))$。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：数学公式

见 [我的题解](https://leetcode.cn/problems/minimum-time-to-complete-all-deliveries/solution/liang-chong-fang-fa-er-fen-da-an-shu-xue-vyqv/)。

```py [sol-Python3]
class Solution:
    def minimizeSet(self, d1: int, d2: int, uniqueCnt1: int, uniqueCnt2: int) -> int:
        def f(cnt: int, d: int) -> int:
            return cnt + (cnt - 1) // (d - 1)

        l = lcm(d1, d2)
        return max(f(uniqueCnt1, d1), f(uniqueCnt2, d2), f(uniqueCnt1 + uniqueCnt2, l))
```

```java [sol-Java]
class Solution {
    public int minimizeSet(int d1, int d2, int uniqueCnt1, int uniqueCnt2) {
        long l = lcm(d1, d2);
        return Math.max(Math.max(f(uniqueCnt1, d1), f(uniqueCnt2, d2)), f(uniqueCnt1 + uniqueCnt2, l));
    }

    private int f(int cnt, long d) {
        return cnt + (int) ((cnt - 1) / (d - 1));
    }

    private int gcd(int a, int b) {
        while (a != 0) {
            int tmp = a;
            a = b % a;
            b = tmp;
        }
        return b;
    }

    private long lcm(int a, int b) {
        return (long) a / gcd(a, b) * b;
    }
}
```

```cpp [sol-C++]
class Solution {
    int f(int cnt, long long d) {
        return cnt + (cnt - 1) / (d - 1);
    }

public:
    int minimizeSet(int d1, int d2, int uniqueCnt1, int uniqueCnt2) {
        long long l = lcm(1LL * d1, 1LL * d2);
        return max({f(uniqueCnt1, d1), f(uniqueCnt2, d2), f(uniqueCnt1 + uniqueCnt2, l)});
    }
};
```

```go [sol-Go]
func f(cnt, d int) int {
	return cnt + (cnt-1)/(d-1)
}

func minimizeSet(d1, d2, uniqueCnt1, uniqueCnt2 int) int {
	l := lcm(d1, d2)
	return max(f(uniqueCnt1, d1), f(uniqueCnt2, d2), f(uniqueCnt1+uniqueCnt2, l))
}

func gcd(a, b int) int {
	for a != 0 {
		a, b = b%a, a
	}
	return b
}

func lcm(a, b int) int {
	return a / gcd(a, b) * b
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log \max(\textit{divisor}_1,\textit{divisor}_2))$。
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

本题来自 `二、组合数学 / §2.3 容斥原理`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、组合数学 / §2.3 容斥原理`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
