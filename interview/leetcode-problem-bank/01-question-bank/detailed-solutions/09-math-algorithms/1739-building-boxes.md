# 1739. 放置盒子

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/building-boxes/
- 题目 slug：`building-boxes`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：2198
- 外部题解来源：https://leetcode.cn/problems/building-boxes/solutions/2031813/mei-xiang-ming-bai-yi-ge-dong-hua-miao-d-8vbe/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[没想明白？一个动画秒懂！附 O(1) 做法（Python/Java/C++/Go）](https://leetcode.cn/problems/building-boxes/solutions/2031813/mei-xiang-ming-bai-yi-ge-dong-hua-miao-d-8vbe/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-xiang-ming-bai-yi-ge-dong-hua-miao-d-8vbe`
- topic id：`2031813`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

<![1739-1.png](https://pic.leetcode.cn/1671765455-jhGgLp-1739-1.png),![1739-2.png](https://pic.leetcode.cn/1671765455-iRHnBC-1739-2.png),![1739-3.png](https://pic.leetcode.cn/1671765455-iGSebO-1739-3.png),![1739-4.png](https://pic.leetcode.cn/1671765455-vULAkC-1739-4.png),![1739-5.png](https://pic.leetcode.cn/1671765455-KtZtTE-1739-5.png),![1739-6.png](https://pic.leetcode.cn/1671765455-KjofVz-1739-6.png),![1739-7.png](https://pic.leetcode.cn/1671765455-kfHwdk-1739-7.png),![1739-8.png](https://pic.leetcode.cn/1671765455-hTrTGG-1739-8.png),![1739-9.png](https://pic.leetcode.cn/1671765455-Gjcdcx-1739-9.png),![1739-10.png](https://pic.leetcode.cn/1671765455-PPGHPj-1739-10.png)>

根据上图，当接触地面的盒子 $\textit{ans}$ 等于

$$
1+2+3+\cdots+i=\dfrac{i(i+1)}{2}
$$

时，对应的盒子上限 $\textit{maxN}$ 为 

$$
1+(1+2)+(1+2+3)+\cdots+\dfrac{i(i+1)}{2}=\dfrac{i(i+1)(i+2)}{6}
$$

在 $\dfrac{i(i+1)(i+2)}{6}$ 的基础上，接触地面的盒子再增加 $j$ 个 $(j\le i+1)$，盒子上限就会再增加 $1+2+\cdots+j=\dfrac{j(j+1)}{2}$ 个。

因此，设 $x$ 为最大的满足 $\dfrac{i(i+1)(i+2)}{6} \le n$ 的 $i$，$y$ 为最小的满足 $\dfrac{x(x+1)(x+2)}{6} + \dfrac{j(j+1)}{2} \ge n$ 的 $j$，则答案为 $\dfrac{x(x+1)}{2}+y$。

实现时可以把 $x$ 和 $y$ 直接加到 $\textit{ans}$ 中。

```py [sol-Python3]
class Solution:
    def minimumBoxes(self, n: int) -> int:
        ans = max_n = 0
        i = j = 1
        while max_n + ans + i <= n:
            ans += i
            max_n += ans
            i += 1
        while max_n < n:
            ans += 1
            max_n += j
            j += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int minimumBoxes(int n) {
        int ans = 0;
        int maxN = 0;
        for (int i = 1; maxN + ans + i <= n; i++) {
            ans += i;
            maxN += ans;
        }
        for (int j = 1; maxN < n; j++) {
            ans++;
            maxN += j;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumBoxes(int n) {
        int ans = 0, max_n = 0;
        for (int i = 1; max_n + ans + i <= n; i++) {
            ans += i;
            max_n += ans;
        }
        for (int j = 1; max_n < n; j++) {
            ans++;
            max_n += j;
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumBoxes(n int) (ans int) {
	maxN := 0
	for i := 1; maxN+ans+i <= n; i++ {
		ans += i
		maxN += ans
	}
	for j := 1; maxN < n; j++ {
		ans++
		maxN += j
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\sqrt[3]n)$。根据上面推导的公式，$i$ 和 $j$ 至多为 $\mathcal{O}(\sqrt[3]n)$。
- 空间复杂度：$\mathcal{O}(1)$，仅用到若干变量。

#### 优化

直接计算 $x$ 和 $y$。

对于 $x$，哪个整数是满足 $\dfrac{x(x+1)(x+2)}{6} \le n$ 的最大整数呢？

如果 $x=\lfloor\sqrt[3]{6n}\rfloor$：

- 由于 $(x+1)(x+2)(x+3)>(x+1)^3>6n$，所以 $x+1$ 必不满足要求。
- 由于 $(x-1)x(x+1)=x(x^2-1)=x^3-x<x^3\le 6n$，所以 $x-1$ 必满足要求。
- $x$ 是否满足要求？计算一下就知道了。

因此，取 $x=\lfloor\sqrt[3]{6n}\rfloor$，如果 $\dfrac{x(x+1)(x+2)}{6} > n$ 则将 $x$ 减一。

对于 $y$，设 $N=n-\dfrac{x(x+1)(x+2)}{6}$，则

$$
\dfrac{y(y+1)}{2}\ge N
$$

解得

$$
y\ge \dfrac{-1+\sqrt{1+8N}}{2}
$$

由于 $y$ 是整数，所以

$$
y=\left\lceil\dfrac{-1+\sqrt{1+8N}}{2}\right\rceil = \left\lceil\dfrac{-1+\lceil\sqrt{1+8N}\rceil}{2}\right\rceil = \left\lfloor\dfrac{\lceil\sqrt{1+8N}\rceil}{2}\right\rfloor
$$

```py [sol-Python3]
class Solution:
    def minimumBoxes(self, n: int) -> int:
        x = floor(cbrt(6 * n))
        ans = x * (x + 1) // 2
        max_n = x * (x + 1) * (x + 2) // 6
        if max_n > n:
            max_n -= ans
            ans -= x
        return ans + ceil(sqrt(1 + 8 * (n - max_n))) // 2
```

```java [sol-Java]
class Solution {
    public int minimumBoxes(int n) {
        int x = (int) Math.cbrt(6L * n);
        int ans = x * (x + 1) / 2;
        int maxN = (int) ((long) x * (x + 1) * (x + 2) / 6);
        if (maxN > n) {
            maxN -= ans;
            ans -= x;
        }
        return ans + (int) Math.ceil(Math.sqrt(1 + 8 * (n - maxN))) / 2;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumBoxes(int n) {
        int x = cbrt(6L * n);
        int ans = x * (x + 1) / 2;
        int max_n = (long) x * (x + 1) * (x + 2) / 6;
        if (max_n > n) {
            max_n -= ans;
            ans -= x;
        }
        return ans + (int) ceil(sqrt(1 + 8 * (n - max_n))) / 2;
    }
};
```

```go [sol-Go]
func minimumBoxes(n int) int {
    x := int(math.Cbrt(float64(6 * n)))
    ans := x * (x + 1) / 2
    maxN := x * (x + 1) * (x + 2) / 6
    if maxN > n {
        maxN -= ans
        ans -= x
    }
    return ans + int(math.Ceil(math.Sqrt(float64(1+8*(n-maxN)))))/2
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。计算开立方和开平方均视作 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$，仅用到若干变量。

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

本题来自 `七、杂项 / §7.10 其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、杂项 / §7.10 其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
