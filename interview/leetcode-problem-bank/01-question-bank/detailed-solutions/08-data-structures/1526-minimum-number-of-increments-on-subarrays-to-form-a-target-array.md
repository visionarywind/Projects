# 1526. 形成目标数组的子数组最少增加次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-increments-on-subarrays-to-form-a-target-array/
- 题目 slug：`minimum-number-of-increments-on-subarrays-to-form-a-target-array`
- 来源专题：常用数据结构
- 来源分类路径：二、差分 / §2.1 一维差分 / §2.1.2 进阶
- 难度分：1872
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-increments-on-subarrays-to-form-a-target-array/solutions/3808716/yong-chai-fen-si-kao-jian-ji-xie-fa-pyth-0v7x/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[用差分思考，简洁写法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-number-of-increments-on-subarrays-to-form-a-target-array/solutions/3808716/yong-chai-fen-si-kao-jian-ji-xie-fa-pyth-0v7x/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yong-chai-fen-si-kao-jian-ji-xie-fa-pyth-0v7x`
- topic id：`3808716`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置知识**：[差分数组原理讲解](https://leetcode.cn/problems/car-pooling/solution/suan-fa-xiao-ke-tang-chai-fen-shu-zu-fu-9d4ra/)。

本题有「区间加一」操作，这非常适合用差分数组做。

定义 $\textit{target}$ 的差分数组 $d$ 为

$$
d[i] =
\begin{cases}
\textit{target}[i], & i=0     \\
\textit{target}[i]-\textit{target}[i-1], & i\ge 1     \\
\end{cases}
$$

示例 2 的 $\textit{target}=[3,1,1,2]$，差分数组为 $d=[3,-2,0,1]$。

由于全 $0$ 数组的差分数组也全为 $0$，所以示例 2 相当于把 $d_0 = [0,0,0,0]$ 变成 $d = [3,-2,0,1]$ 的最小操作次数。

根据前置知识，如果「区间加一」操作的不是 $\textit{initial}$ 的后缀，那么操作等价于修改 $d_0$ 两个位置上的数，**左边加一，右边减一**。如果操作的是 $\textit{initial}$ 的后缀，那么操作等价于把 $d_0$ 中的一个数**单独加一**。

示例 2 的一种操作方案如下：

$$
[0,0,0,0] \to [1,-1,0,0] \to [2,-2,0,0] \to [3,-2,0,0] \to [3,-2,0,1]
$$

为了最小化操作次数，优先修改两个位置上的数（加一减一），然后再考虑单独加一。

注意到，无论每次操作修改的是两个数还是一个数，一定会把一个数加一。所以最小操作次数等于加一的次数，即 $d$ 中**所有正数之和**。

> **注**：具体操作方案为，从左到右遍历 $d$，先让负数与其左边的正数匹配，然后剩余的未匹配正数单独算（对应后缀加）。由于 $d$ 的前缀和就是原数组 $\textit{target}$，且 $\textit{target}[i] > 0$，所以负数一定可以与其左边的正数匹配，不会出现 $d=[2,2,-5]$ 或 $d=[2,-3,4]$ 等无法匹配的情况。具体见下面答疑。

### 答疑

**问**：有没有一种可能，$d=[2,2,-5]$，负数绝对值之和比正数之和还大？

**答**：不可能。由于差分数组 $d$ 的前缀和就是原数组 $\textit{target}$，如果出现负数绝对值之和比正数之和还大，说明 $d$ 的总和是负数，即 $\textit{target}[n-1] < 0$，这与本题的数据范围 $\textit{target}[i] > 0$ 矛盾。

**问**：有没有一种可能，$d=[2,-3,4]$，无法使用「左边加一，右边减一」的操作从 $[0,0,0]$ 得到 $d$？

**答**：不可能。同样地，计算 $d$ 的前缀和，如果出现这种情况，说明前缀和中的某个数是负数，即 $\textit{target}[i]<0$，这与本题的数据范围 $\textit{target}[i] > 0$ 矛盾。

```py [sol-Python3]
class Solution:
    def minNumberOperations(self, target: List[int]) -> int:
        ans = target[0]
        for x, y in pairwise(target):
            ans += max(y - x, 0)
        return ans
```

```py [sol-Python3 一行]
class Solution:
    def minNumberOperations(self, target: List[int]) -> int:
        return target[0] + sum(max(y - x, 0) for x, y in pairwise(target))
```

```java [sol-Java]
class Solution {
    public int minNumberOperations(int[] target) {
        // 题目保证答案在 int 范围内
        int ans = target[0];
        for (int i = 1; i < target.length; i++) {
            ans += Math.max(target[i] - target[i - 1], 0);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minNumberOperations(vector<int>& target) {
        // 题目保证答案在 int 范围内
        int ans = target[0];
        for (int i = 1; i < target.size(); i++) {
            ans += max(target[i] - target[i - 1], 0);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int minNumberOperations(int* target, int targetSize) {
    // 题目保证答案在 int 范围内
    int ans = target[0];
    for (int i = 1; i < targetSize; i++) {
        ans += MAX(target[i] - target[i - 1], 0);
    }
    return ans;
}
```

```go [sol-Go]
func minNumberOperations(target []int) int {
	ans := target[0]
	for i := 1; i < len(target); i++ {
		ans += max(target[i]-target[i-1], 0)
	}
	return ans
}
```

```js [sol-JS]
var minNumberOperations = function(target) {
    let ans = target[0];
    for (let i = 1; i < target.length; i++) {
        ans += Math.max(target[i] - target[i - 1], 0);
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn min_number_operations(target: Vec<i32>) -> i32 {
        // 题目保证答案在 i32 范围内
        target[0] + target.windows(2)
            .map(|w| 0.max(w[1] - w[0]))
            .sum::<i32>()
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{target}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 相似题目

[3229. 使数组等于目标数组所需的最少操作次数](https://leetcode.cn/problems/minimum-operations-to-make-array-equal-to-target/)

## 专题训练

见下面数据结构题单的「**§2.1 一维差分**」。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、差分 / §2.1 一维差分 / §2.1.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、差分 / §2.1 一维差分 / §2.1.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
