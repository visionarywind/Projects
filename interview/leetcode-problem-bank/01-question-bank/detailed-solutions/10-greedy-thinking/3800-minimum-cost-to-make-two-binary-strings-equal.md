# 3800. 使二进制字符串相等的最小成本

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-to-make-two-binary-strings-equal/
- 题目 slug：`minimum-cost-to-make-two-binary-strings-equal`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：1841
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-to-make-two-binary-strings-equal/solutions/3872286/fen-lei-tao-lun-pythonjavacgo-by-endless-u3jx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[分类讨论（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-cost-to-make-two-binary-strings-equal/solutions/3872286/fen-lei-tao-lun-pythonjavacgo-by-endless-u3jx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`fen-lei-tao-lun-pythonjavacgo-by-endless-u3jx`
- topic id：`3872286`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

统计满足 $s[i] = \texttt{0}$ 且 $t[i]=\texttt{1}$ 的 $i$，记作集合 $A$，其大小记作 $a$。

统计满足 $s[i] = \texttt{1}$ 且 $t[i]=\texttt{0}$ 的 $i$，记作集合 $B$，其大小记作 $b$。

不妨设 $a\le b$（不满足则交换 $a$ 和 $b$）。

## 情况一：只翻转

一共要翻转 $a+b$ 个下标，代价为

$$
(a + b) \cdot \textit{flipCost}
$$

## 情况二：先串内交换，再翻转

交换集合 $A$ 与集合 $B$ 中的一对下标，可以同时消除一对 $\texttt{01}$ 和一对 $\texttt{10}$，等价于两次翻转操作。

这意味着，如果 $\textit{swapCost} < 2\cdot \textit{flipCost}$，那么应当先尽量多地交换，再翻转。否则只做翻转即可。

所以**要么不交换，要么全交换，没有中间选项**。

先交换 $a$ 次（注意我们保证了 $a\le b$）。

剩余 $b-a$ 个下标，各自翻转一次。

代价一共为

$$
a\cdot \textit{swapCost} + (b-a)\cdot \textit{flipCost}
$$

## 情况三：先串间交换，再串内交换，最后翻转

上面两种情况只用到了翻转（操作一）和串内交换（操作二）。如果引入串间交换（操作三），会发生什么？串间交换有什么用？

串间交换不会改变 $a + b$，所以不会减少翻转次数，只会影响串内交换的次数。

减少串内交换的次数？那样只会让翻转次数尽量多，这在情况一中考虑了。

所以只需考虑增大串内交换的次数，即让 $a$ 和 $b$ **尽量均匀**，从而可以**尽量多地执行串内交换**。

设 $\textit{avg} = \left\lfloor\dfrac{a+b}{2}\right\rfloor$。

先执行 $\textit{avg} - a$ 次操作三。操作后 $|a-b|\le 1$。

然后执行 $\textit{avg}$ 次操作二。

最后还剩下 $(a+b)\bmod 2$ 个下标需要操作，执行翻转。

代价一共为

$$
(\textit{avg}-a)\cdot \textit{crossCost} + \textit{avg}\cdot \textit{swapCost} + ((a+b)\bmod 2)\cdot \textit{flipCost}
$$

三种情况取最小值。

> 注：由于减少其中一种操作，转而使用其他操作代替，对总代价的影响是线性的，所以最小值只会在端点或交点处取到。所以只需考虑上述三种情况。

[本题视频讲解](https://www.bilibili.com/video/BV1TgijB7Eer/?t=7m7s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minimumCost(self, s: str, t: str, flipCost: int, swapCost: int, crossCost: int) -> int:
        # 更快的写法见【Python3 写法二】
        cnt = Counter(x + y for x, y in zip(s, t))

        a = cnt["01"]
        b = cnt["10"]
        if a > b:
            a, b = b, a

        res1 = (a + b) * flipCost
        res2 = a * swapCost + (b - a) * flipCost
        avg, rem = divmod(a + b, 2)
        res3 = (avg - a) * crossCost + avg * swapCost + rem * flipCost
        return min(res1, res2, res3)
```

```py [sol-Python3 写法二]
class Solution:
    def minimumCost(self, s: str, t: str, flipCost: int, swapCost: int, crossCost: int) -> int:
        x = int(s, 2)
        y = int(t, 2)

        a = (~x & y).bit_count()  # 01 个数
        b = (x & ~y).bit_count()  # 10 个数
        if a > b:
            a, b = b, a

        res1 = (a + b) * flipCost
        res2 = a * swapCost + (b - a) * flipCost
        avg, rem = divmod(a + b, 2)
        res3 = (avg - a) * crossCost + avg * swapCost + rem * flipCost
        return min(res1, res2, res3)
```

```java [sol-Java]
class Solution {
    public long minimumCost(String s, String t, int flipCost, int swapCost, int crossCost) {
        int[][] cnt = new int[2][2];
        for (int i = 0; i < s.length(); i++) {
            cnt[s.charAt(i) & 1][t.charAt(i) & 1]++;
        }

        int a = cnt[0][1];
        int b = cnt[1][0];
        if (a > b) {
            int tmp = a;
            a = b;
            b = tmp;
        }

        long res1 = (long) (a + b) * flipCost;
        long res2 = (long) a * swapCost + (long) (b - a) * flipCost;
        int avg = (a + b) / 2;
        long res3 = (long) (avg - a) * crossCost + (long) avg * swapCost + (long) (a + b) % 2 * flipCost;
        return Math.min(Math.min(res1, res2), res3);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minimumCost(string s, string t, int flipCost, int swapCost, int crossCost) {
        int cnt[2][2]{};
        for (int i = 0; i < s.size(); i++) {
            cnt[s[i] & 1][t[i] & 1]++;
        }

        int a = cnt[0][1];
        int b = cnt[1][0];
        if (a > b) {
            swap(a, b);
        }

        long long res1 = 1LL * (a + b) * flipCost;
        long long res2 = 1LL * a * swapCost + 1LL * (b - a) * flipCost;
        int avg = (a + b) / 2;
        long long res3 = 1LL * (avg - a) * crossCost + 1LL * avg * swapCost + 1LL * (a + b) % 2 * flipCost;
        return min({res1, res2, res3});
    }
};
```

```go [sol-Go]
func minimumCost(s, t string, flipCost, swapCost, crossCost int) int64 {
	cnt := [2][2]int{}
	for i, ch := range s {
		cnt[ch&1][t[i]&1]++
	}

	a := cnt[0][1]
	b := cnt[1][0]
	if a > b {
		a, b = b, a
	}

	res1 := (a + b) * flipCost
	res2 := a*swapCost + (b-a)*flipCost
	avg := (a + b) / 2
	res3 := (avg-a)*crossCost + avg*swapCost + (a+b)%2*flipCost
	return int64(min(res1, res2, res3))
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $s$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

[3789. 采购的最小花费](https://leetcode.cn/problems/minimum-cost-to-acquire-required-items/)

更多分类讨论题目，见下面贪心题单的「**§5.6 分类讨论**」。

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

本题来自 `五、思维题 / §5.8 分类讨论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.8 分类讨论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
