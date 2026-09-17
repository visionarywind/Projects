# 1330. 翻转子数组得到最大的数组值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/reverse-subarray-to-maximize-array-value/
- 题目 slug：`reverse-subarray-to-maximize-array-value`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：2482
- 外部题解来源：https://leetcode.cn/problems/reverse-subarray-to-maximize-array-value/solutions/2266500/bu-hui-hua-jian-qing-kan-zhe-pythonjavac-c2s6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[化简绝对值（Python/Java/C++/Go）](https://leetcode.cn/problems/reverse-subarray-to-maximize-array-value/solutions/2266500/bu-hui-hua-jian-qing-kan-zhe-pythonjavac-c2s6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`bu-hui-hua-jian-qing-kan-zhe-pythonjavac-c2s6`
- topic id：`2266500`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:22:48 +0800

## 初步分析

如果不翻转，或者翻转的是一个长为 $1$ 的子数组，那么 $\textit{nums}$ 不变，此时的「数组值」记作 $\textit{base}$。

> 示例 1 的 $\textit{base}=|2-3|+|3-1|+|1-5|+|5-4|=1+2+4+1=8$。

为了计算出最大的「数组值」，考虑翻转后与翻转前的差值 $d$，那么答案为 $\textit{base}+d$，所以 $d$ 越大，答案也就越大。

假设从 $\textit{nums}[i]$ 到 $\textit{nums}[j]$ 的这段子数组翻转了，且 $1\le i < j < n-1$（其中 $n$ 为 $\textit{nums}$ 的长度）。设 $a=\textit{nums}[i-1],\ b=\textit{nums}[i],\ x=\textit{nums}[j],\ y=\textit{nums}[j+1]$。

> 对于 $i=0$ 或 $j=n-1$ 的翻转，单独用 $\mathcal{O}(n)$ 的时间枚举。

翻转前，这 $4$ 个数对数组值的贡献为 

$$
|a-b| + |x-y|
$$

翻转后，顺序变为 $a,x,b,y$，贡献为

$$
|a-x|+|b-y|
$$

得到

$$
d = |a-x|+|b-y|-|a-b|-|x-y| \tag{1}
$$

问题转换成求 $d$ 的最大值。

> 示例 1 中翻转的子数组对应的 $a=2,b=3,x=5,y=4$，代入上式得 $d=2$，数组值为 $\textit{base}+d=8+2=10$。

暴力枚举 $i$ 和 $j$ 的时间复杂度是 $\mathcal{O}(n^2)$ 的，如何优化？化简 $(1)$ 式是本题的核心。

## 若干恒等式

对于 $|a-b|$：

- 如果 $a\ge b$，结果是 $a-b$；
- 如果 $a< b$，结果是 $b-a$；
- 总而言之，结果就是大的减去小的。

所以

$$
|a-b| = \max(a,b) - \min(a,b) \tag{2}
$$

此外还有如下恒等式

$$
a+b = \max(a,b) + \min(a,b) \tag{3}
$$

$(3)+(2)$ 得

$$
a+b+|a-b| = 2\cdot\max(a,b)  \tag{4}
$$

$(3)-(2)$ 得

$$
a+b-|a-b| = 2\cdot\min(a,b)  \tag{5}
$$

恒等式 $(4)$ 和 $(5)$ 是化简 $(1)$ 式的钥匙。

转动钥匙，让我们开始吧！

## 分类讨论

$a,b,x,y$ 这 $4$ 个数的大小关系一共有 $4!=24$ 种情况，例如 $a\le b\le x\le y,\ b\le x\le a \le y$ 等等。

按照这 $4$ 个数中的哪两个数是最小的两个，可以分为 $C(4,2)=6$ 类，每类 $4$ 种情况。

利用**对称性**，只需讨论其中 $3$ 类，便可以得到另外 $3$ 类的结果。

**第 1 类**：$\max(a,b)\le \min(x,y)$

把 $a,b,x,y$ 画在数轴上，相当于 $a$ 和 $b$ 都在 $x$ 和 $y$ 的左边（或重合）。

那么

$$
\begin{aligned}
d &= |a-x|+|b-y|-|a-b|-|x-y|\\
  &= (x-a) + (y-b) -|a-b|-|x-y|\\
  &= (x+y) - (a+b) -|a-b|-|x-y|\\
  &= (x+y-|x-y|)-(a+b +|a-b|)\\
  &= 2\cdot\min(x,y) - 2\cdot\max(a,b)
\end{aligned}
$$

注意 $\max(a,b)\le \min(x,y)$，上式是 $\ge 0$ 的。

利用对称性，对于 $\max(x,y)\le \min(a,b)$ 的 $4$ 种情况，可以得到类似的结果

$$
d = 2\cdot\min(a,b) - 2\cdot\max(x,y) \ge 0
$$

很好，已经讨论清楚 $8$ 种情况了！

**第 2 类**：$\max(a,x)\le \min(b,y)$

把 $a,b,x,y$ 画在数轴上，相当于 $a$ 和 $x$ 都在 $b$ 和 $y$ 的左边（或重合）。

那么

$$
\begin{aligned}
d &= |a-x|+|b-y|-|a-b|-|x-y|\\
  &= |a-x|+|b-y| -(b-a)-(y-x)\\
  &= |a-x|+|b-y| +(a+x) -(b+y)\\
  &= (a+x+|a-x|)-(b+y -|b-y|)\\
  &= 2\cdot\max(a,x) - 2\cdot\min(b,y)
\end{aligned}
$$

由于 $\max(a,x)\le \min(b,y)$，上式 $\le 0$。

利用对称性，对于 $\max(b,y)\le \min(a,x)$ 的 $4$ 种情况，可以得到类似的结果。

$$
d = 2\cdot\max(b,y) - 2\cdot\min(a,x) \le 0
$$

所以这 $8$ 种情况不会对 $d$ 的最大值产生影响。（注意可以只翻转长为 $1$ 的子数组，此时 $d=0$。）

很好，已经讨论清楚 $16$ 种情况了！

**第 3 类**：$\max(a,y)\le \min(b,x)$

把 $a,b,x,y$ 画在数轴上，相当于 $a$ 和 $y$ 都在 $b$ 和 $x$ 的左边（或重合）。

那么

$$
\begin{aligned}
d &= |a-x|+|b-y|-|a-b|-|x-y|\\
  &= (x-a)+(b-y)-(b-a)-(x-y)\\
  &= 0
\end{aligned}
$$

利用对称性，对于 $\max(b,x)\le \min(a,y)$ 的 $4$ 种情况，同样可以得到 $d=0$。

所以这 $8$ 种情况也不会对 $d$ 的最大值产生影响。

$24$ 种情况讨论完毕。

## 算法

由于只有第 1 类的情况会影响 $d$ 的最大值，为了最大化 $d$，在遍历 $\textit{nums}$ 的所有相邻元素 $a,b$ 的同时，维护 $\min(a,b)$ 的最大值 $\textit{mx}$，以及 $\max(a,b)$ 的最小值 $\textit{mn}$。

遍历结束后，如果 $\textit{mx}>\textit{mn}$，那么对应的 $a,b,x,y$ 存在，且大小关系必然属于第 1 类讨论的 $8$ 种情况之一。则有

$$
d=2\cdot (\textit{mx}-\textit{mn})
$$

如果 $\textit{mx}=\textit{mn}$，由于 $d$ 初始值为 $0$，不会产生影响。

特别地，对于翻转范围在数组边界的情况（$i=0$ 或 $j=n-1$），单独枚举，并更新 $d$ 的最大值。

```py [sol-Python3]
class Solution:
    def maxValueAfterReverse(self, nums: List[int]) -> int:
        base = d = 0
        mx, mn = -inf, inf
        for a, b in pairwise(nums):
            base += abs(a - b)
            mx = max(mx, min(a, b))
            mn = min(mn, max(a, b))
            d = max(d, abs(nums[0] - b) - abs(a - b),  # i=0
                       abs(nums[-1] - a) - abs(a - b))  # j=n-1
        return base + max(d, 2 * (mx - mn))
```

```java [sol-Java]
class Solution {
    public int maxValueAfterReverse(int[] nums) {
        int n = nums.length;
        int base = 0;
        int mx = Integer.MIN_VALUE;
        int mn = Integer.MAX_VALUE;
        int d = 0;
        for (int i = 1; i < n; i++) {
            int a = nums[i - 1];
            int b = nums[i];
            int dab = Math.abs(a - b);
            base += dab;
            mx = Math.max(mx, Math.min(a, b));
            mn = Math.min(mn, Math.max(a, b));
            d = Math.max(d, Math.max(Math.abs(nums[0] - b) - dab, // i=0
                                     Math.abs(nums[n - 1] - a) - dab)); // j=n-1
        }
        return base + Math.max(d, 2 * (mx - mn));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxValueAfterReverse(vector<int>& nums) {
        int base = 0, d = 0, mx = INT_MIN, mn = INT_MAX, n = nums.size();
        for (int i = 1; i < n; i++) {
            int a = nums[i - 1], b = nums[i];
            base += abs(a - b);
            mx = max(mx, min(a, b));
            mn = min(mn, max(a, b));
            d = max(d, max(abs(nums[0] - b) - abs(a - b), // i=0
                           abs(nums[n - 1] - a) - abs(a - b))); // j=n-1
        }
        return base + max(d, 2 * (mx - mn));
    }
};
```

```go [sol-Go]
func maxValueAfterReverse(nums []int) int {
    base, d, n := 0, 0, len(nums)
    mx, mn := math.MinInt, math.MaxInt
    for i := 1; i < n; i++ {
        a, b := nums[i-1], nums[i]
        base += abs(a - b)
        mx = max(mx, min(a, b))
        mn = min(mn, max(a, b))
        d = max(d, max(abs(nums[0]-b)-abs(a-b), // i=0
                       abs(nums[n-1]-a)-abs(a-b))) // j=n-1
    }
    return base + max(d, 2*(mx-mn))
}

func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

- [1131. 绝对值表达式的最大值](https://leetcode.cn/problems/maximum-of-absolute-value-expression/)
- [CF1898D](https://codeforces.com/problemset/problem/1898/D)

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
