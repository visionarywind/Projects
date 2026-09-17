# 3091. 执行操作使数据元素之和大于等于 K

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/apply-operations-to-make-sum-of-array-greater-than-or-equal-to-k/
- 题目 slug：`apply-operations-to-make-sum-of-array-greater-than-or-equal-to-k`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：1522
- 外部题解来源：https://leetcode.cn/problems/apply-operations-to-make-sum-of-array-greater-than-or-equal-to-k/solutions/2704767/liang-chong-fang-fa-mei-ju-o1-shu-xue-go-ug47/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：枚举 / O(1) 数学公式（Python/Java/C++/Go）](https://leetcode.cn/problems/apply-operations-to-make-sum-of-array-greater-than-or-equal-to-k/solutions/2704767/liang-chong-fang-fa-mei-ju-o1-shu-xue-go-ug47/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-mei-ju-o1-shu-xue-go-ug47`
- topic id：`2704767`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

请看 [视频讲解](https://www.bilibili.com/video/BV1wr421h7xY/) 第二题。

## 方法一：贪心 + 枚举

设当前数组最大值为 $m$，对它做加一操作更好（因为复制最大值最优）。

- 如果先复制 $m$，再加一，那么元素和增加了 $m+1$。
- 如果先加一，再复制 $m+1$，那么元素和增加了 $m+2$。

所以，先加一再复制更优。

所以，加一操作都应当在复制操作之前。

我们可以枚举加一操作执行了 $\textit{add}= 0,1,2,\cdots, k-1$ 次。

设 $m=1+\textit{add}$，我们还需要复制

$$
\left\lceil\dfrac{k}{m}\right\rceil-1 = \left\lfloor\dfrac{k-1}{m}\right\rfloor
$$

次，才能让元素和至少为 $k$。上式的证明见 [上取整下取整转换公式的证明](https://zhuanlan.zhihu.com/p/1890356682149838951)。

所以答案为

$$
\min\limits_{m=1}^{k} m-1 + \left\lfloor\dfrac{k-1}{m}\right\rfloor
$$

```py [sol-Python3]
class Solution:
    def minOperations(self, k: int) -> int:
        return min(m - 1 + (k - 1) // m for m in range(1, k + 1))
```

```java [sol-Java]
class Solution {
    public int minOperations(int k) {
        int ans = Integer.MAX_VALUE;
        for (int m = 1; m <= k; m++) {
            ans = Math.min(ans, m - 1 + (k - 1) / m);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOperations(int k) {
        int ans = INT_MAX;
        for (int m = 1; m <= k; m++) {
            ans = min(ans, m - 1 + (k - 1) / m);
        }
        return ans;
    }
};
```

```go [sol-Go]
func minOperations(k int) int {
	ans := math.MaxInt
	for m := 1; m <= k; m++ {
		ans = min(ans, m-1+(k-1)/m)
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k)$。
- 空间复杂度：$\mathcal{O}(1)$。

## 方法二：数学

由基本不等式，或者对勾函数性质可知，设 $\textit{rt} = \left\lfloor\sqrt{k-1}\right\rfloor$，那么当 $m$ 取 $\textit{rt}$ 或者 $\textit{rt}+1$ 时我们可以得到最小值。

为防止 $m=0$，可以和 $1$ 取最大值（或者特判）。

> 注：在本题数据范围下，开平方结果的整数部分是正确的，无需调整。

```py [sol-Python3]
class Solution:
    def minOperations(self, k: int) -> int:
        rt = max(isqrt(k - 1), 1)
        return min(rt - 1 + (k - 1) // rt, rt + (k - 1) // (rt + 1))
```

```java [sol-Java]
class Solution {
    public int minOperations(int k) {
        int rt = Math.max((int) Math.sqrt(k - 1), 1);
        return Math.min(rt - 1 + (k - 1) / rt, rt + (k - 1) / (rt + 1));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minOperations(int k) {
        int rt = max((int) sqrt(k - 1), 1);
        return min(rt - 1 + (k - 1) / rt, rt + (k - 1) / (rt + 1));
    }
};
```

```go [sol-Go]
func minOperations(k int) int {
	rt := max(int(math.Sqrt(float64(k-1))), 1)
	return min(rt-1+(k-1)/rt, rt+(k-1)/(rt+1))
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(1)$。开平方有专门的 CPU 指令，可以视作 $\mathcal{O}(1)$。
- 空间复杂度：$\mathcal{O}(1)$。


## 分类题单

- [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
- [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
- [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
- [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
- [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
- [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
- [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

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
