# 3468. 可行数组的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-the-number-of-copy-arrays/
- 题目 slug：`find-the-number-of-copy-arrays`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：1545
- 外部题解来源：https://leetcode.cn/problems/find-the-number-of-copy-arrays/solutions/3591446/xiang-xi-shu-xue-tui-dao-pythonjavacgo-b-ggb3/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[详细数学推导（Python/Java/C++/Go）](https://leetcode.cn/problems/find-the-number-of-copy-arrays/solutions/3591446/xiang-xi-shu-xue-tui-dao-pythonjavacgo-b-ggb3/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`xiang-xi-shu-xue-tui-dao-pythonjavacgo-b-ggb3`
- topic id：`3591446`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

题目要求 $\textit{copy}[i] - \textit{copy}[i - 1] = \textit{original}[i] - \textit{original}[i - 1]$，那么有

$$
\begin{aligned}
\textit{copy}[1] - \textit{copy}[0] &= \textit{original}[1] - \textit{original}[0]     \\
\textit{copy}[2] - \textit{copy}[1] &= \textit{original}[2] - \textit{original}[1]     \\
\textit{copy}[3] - \textit{copy}[2] &= \textit{original}[3] - \textit{original}[2]     \\
&\ \ \vdots \\
\textit{copy}[i] - \textit{copy}[i - 1] &= \textit{original}[i] - \textit{original}[i - 1] \\
\end{aligned}
$$

累加等号左边的所有项，累加等号右边的所有项，得

$$
\textit{copy}[i] - \textit{copy}[0] = \textit{original}[i] - \textit{original}[0]
$$

移项得

$$
\textit{copy}[i] = \textit{copy}[0] + \textit{original}[i] - \textit{original}[0]
$$

换句话说，确定了 $\textit{copy}[0]$，那么整个数组也就确定了。所以 $\textit{copy}[0]$ 的取值范围（整数集合）的大小就是答案。

题目要求

$$
u_i\le \textit{copy}[i] \le v_i
$$

设 $d_i = \textit{original}[i] - \textit{original}[0]$，用 $\textit{copy}[i] = \textit{copy}[0] + d_i$ 替换上式中的 $\textit{copy}[i]$，得

$$
u_i\le \textit{copy}[0] + d_i \le v_i
$$

移项得

$$
u_i - d_i \le \textit{copy}[0] \le v_i - d_i
$$

所以我们可以得到 $n$ 个关于 $\textit{copy}[0]$ 的不等式，或者说区间：

$$
\begin{aligned}
& [u_0,v_0] \\
& [u_1 - d_1 ,v_1-d_1] \\
& [u_2 - d_2 ,v_2-d_2] \\
& \ \ \vdots \\
& [u_{n-1} - d_{n-1} ,v_{n-1}-d_{n-1}] \\
& \end{aligned}
$$

这些区间的**交集**，即为 $\textit{copy}[0]$ 能取到的值。

区间交集的大小即为答案。如果交集为空，返回 $0$。

怎么算交集的范围？所有区间左端点取最大值，右端点取最小值。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1m39bYiEVV/?t=1m2s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def countArrays(self, original: List[int], bounds: List[List[int]]) -> int:
        mn, mx = -inf, inf
        for x, (u, v) in zip(original, bounds):
            d = x - original[0]
            mn = max(mn, u - d)  # 计算区间交集
            mx = min(mx, v - d)
        return max(mx - mn + 1, 0)  # 注意交集可能是空的
```

```java [sol-Java]
class Solution {
    public int countArrays(int[] original, int[][] bounds) {
        int mn = bounds[0][0], mx = bounds[0][1];
        for (int i = 1; i < bounds.length; i++) {
            int d = original[i] - original[0];
            mn = Math.max(mn, bounds[i][0] - d); // 计算区间交集
            mx = Math.min(mx, bounds[i][1] - d);
        }
        return Math.max(mx - mn + 1, 0); // 注意交集可能是空的
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countArrays(vector<int>& original, vector<vector<int>>& bounds) {
        int mn = bounds[0][0], mx = bounds[0][1];
        for (int i = 1; i < bounds.size(); i++) {
            int d = original[i] - original[0];
            mn = max(mn, bounds[i][0] - d); // 计算区间交集
            mx = min(mx, bounds[i][1] - d);
        }
        return max(mx - mn + 1, 0); // 注意交集可能是空的
    }
};
```

```go [sol-Go]
func countArrays(original []int, bounds [][]int) int {
	mn, mx := math.MinInt, math.MaxInt
	for i, b := range bounds {
		d := original[i] - original[0]
		mn = max(mn, b[0]-d) // 计算区间交集
		mx = min(mx, b[1]-d)
	}
	return max(mx-mn+1, 0) // 注意交集可能是空的
}
```

## 优化

也可以只减去 $\textit{original}[i]$ 而不是 $d_i$。这相当于每个区间都偏移了 $\textit{original}[0]$，所以区间交集的大小是不变的。

这样每次循环可以少算一次减法。

```py [sol-Python3]
class Solution:
    def countArrays(self, original: List[int], bounds: List[List[int]]) -> int:
        mn, mx = -inf, inf
        for x, (u, v) in zip(original, bounds):
            mn = max(mn, u - x)  # 计算区间交集
            mx = min(mx, v - x)
        return max(mx - mn + 1, 0)  # 注意交集可能是空的
```

```py [sol-Python3 写法二]
class Solution:
    def countArrays(self, original: List[int], bounds: List[List[int]]) -> int:
        mn = max(b[0] - x for x, b in zip(original, bounds))
        mx = min(b[1] - x for x, b in zip(original, bounds))
        return max(mx - mn + 1, 0)
```

```java [sol-Java]
class Solution {
    public int countArrays(int[] original, int[][] bounds) {
        int mn = Integer.MIN_VALUE, mx = Integer.MAX_VALUE;
        for (int i = 0; i < bounds.length; i++) {
            mn = Math.max(mn, bounds[i][0] - original[i]); // 计算区间交集
            mx = Math.min(mx, bounds[i][1] - original[i]);
        }
        return Math.max(mx - mn + 1, 0); // 注意交集可能是空的
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int countArrays(vector<int>& original, vector<vector<int>>& bounds) {
        int mn = INT_MIN, mx = INT_MAX;
        for (int i = 0; i < bounds.size(); i++) {
            mn = max(mn, bounds[i][0] - original[i]); // 计算区间交集
            mx = min(mx, bounds[i][1] - original[i]);
        }
        return max(mx - mn + 1, 0); // 注意交集可能是空的
    }
};
```

```go [sol-Go]
func countArrays(original []int, bounds [][]int) int {
	mn, mx := math.MinInt, math.MaxInt
	for i, b := range bounds {
		mn = max(mn, b[0]-original[i]) // 计算区间交集
		mx = min(mx, b[1]-original[i])
	}
	return max(mx-mn+1, 0) // 注意交集可能是空的
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{original}$ 的长度。
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
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

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
