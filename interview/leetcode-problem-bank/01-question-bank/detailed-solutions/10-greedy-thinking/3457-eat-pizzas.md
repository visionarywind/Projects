# 3457. 吃披萨

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/eat-pizzas/
- 题目 slug：`eat-pizzas`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.1 从最小/最大开始贪心
- 难度分：1704
- 外部题解来源：https://leetcode.cn/problems/eat-pizzas/solutions/3076629/tan-xin-pythonjavacgo-by-endlesscheng-fpjx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心（Python/Java/C++/Go）](https://leetcode.cn/problems/eat-pizzas/solutions/3076629/tan-xin-pythonjavacgo-by-endlesscheng-fpjx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-pythonjavacgo-by-endlesscheng-fpjx`
- topic id：`3076629`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

推荐先完成本题的简单版本：[1561. 你可以获得的最大硬币数目](https://leetcode.cn/problems/maximum-number-of-coins-you-can-get/)。

一共有 $\textit{days}=\dfrac{n}{4}$ 天，其中有 $\textit{odd} = \left\lceil\dfrac{\textit{days}}{2}\right\rceil = \left\lfloor\dfrac{\textit{days}+1}{2}\right\rfloor$ 个奇数天，$\textit{even}=\left\lfloor\dfrac{\textit{days}}{2}\right\rfloor$ 个偶数天。

根据题意，**奇数天选最大的，偶数天只能选次大的**。

例如 $\textit{odd}=3,\ \textit{even}=2$，一共需要考虑最大的 $3+2\cdot2=7$ 块披萨，从中选 $3+2=5$ 块披萨。由于**不选的披萨越靠后，元素和就越大**，所以可以先考虑奇数天怎么选（先选前 $3$ 个最大的），再考虑偶数天怎么选（再从剩余披萨中跳着选 $2$ 个最大的）。假设披萨从大到小排序，那么选择方案为

$$
选选选\underline{\phantom{跳}}选\underline{\phantom{跳}}选
$$

其中 $\underline{\phantom{跳}}$ 表示跳过不选。

用**交换论证法**可以证明这是最优的。比如交换第二个奇数天和最后一个偶数天，也就是按照「奇偶奇偶奇」的方法选，那么选择方案为

$$
选\underline{\phantom{跳}}选选\underline{\phantom{跳}}选选
$$

对比可以发现，本质上这个交换把后面的一个 $\underline{\phantom{跳}}$ 插入到了前面的某个位置中，于是这两个位置之间的所有「选」的下标都向后移动了一位，所以这个方案的元素和一定不会比交换前的优。

所以答案为

$$
\sum_{i=0}^{\textit{odd}-1} \textit{pizzas}[i] + \sum_{i=0}^{\textit{even}-1} \textit{pizzas}[\textit{odd}+2i+1]
$$

其中 $\textit{pizzas}$ 按照从大到小的顺序排序。

[本题视频讲解](https://www.bilibili.com/video/BV1pmAGegEcw/?t=4m)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxWeight(self, pizzas: List[int]) -> int:
        pizzas.sort(reverse=True)
        days = len(pizzas) // 4
        odd = (days + 1) // 2
        return sum(pizzas[:odd]) + sum(pizzas[odd + 1: odd + days // 2 * 2: 2])
```

```java [sol-Java]
class Solution {
    public long maxWeight(int[] pizzas) {
        Arrays.sort(pizzas);
        int n = pizzas.length;
        int days = n / 4;
        int odd = (days + 1) / 2;
        long ans = 0;
        for (int i = 0; i < odd; i++) {
            ans += pizzas[n - 1 - i];
        }
        for (int i = 0; i < days / 2; i++) {
            ans += pizzas[n - 2 - odd - i * 2];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maxWeight(vector<int>& pizzas) {
        ranges::sort(pizzas, greater());
        int days = pizzas.size() / 4;
        int odd = (days + 1) / 2;
        long long ans = 0;
        for (int i = 0; i < odd; i++) {
            ans += pizzas[i];
        }
        for (int i = 0; i < days / 2; i++) {
            ans += pizzas[odd + i * 2 + 1];
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxWeight(pizzas []int) (ans int64) {
	slices.SortFunc(pizzas, func(a, b int) int { return b - a })
	days := len(pizzas) / 4
	odd := (days + 1) / 2
	for _, x := range pizzas[:odd] {
		ans += int64(x)
	}
	for i := range days / 2 {
		ans += int64(pizzas[odd+i*2+1])
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{pizzas}$ 的长度，瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略切片和排序的栈开销。

更多相似题目，见下面贪心题单中的「**§1.1 从最小/最大开始贪心**」。

## 思考题

把最大改成最小，怎么做？

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

本题来自 `一、贪心策略 / §1.1 从最小/最大开始贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.1 从最小/最大开始贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
