# 1665. 完成所有任务的最少初始能量

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-initial-energy-to-finish-tasks/
- 题目 slug：`minimum-initial-energy-to-finish-tasks`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.7 交换论证法
- 难度分：1901
- 外部题解来源：https://leetcode.cn/problems/minimum-initial-energy-to-finish-tasks/solutions/3965592/jiao-huan-lun-zheng-fa-pythonjavaccgojsr-ozft/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[交换论证法（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-initial-energy-to-finish-tasks/solutions/3965592/jiao-huan-lun-zheng-fa-pythonjavaccgojsr-ozft/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-huan-lun-zheng-fa-pythonjavaccgojsr-ozft`
- topic id：`3965592`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

## 按照什么规则排序？

本文把 $\textit{actual}$ 简称为 $a$，把 $\textit{minimum}$ 简称为 $m$。

如果 $\textit{tasks}$ 的某个排列 $T$ 是最优的（完成所有任务所需的初始能量最少），那么 $T$ 中的相邻任务满足什么性质？

设 $T$ 中的一对相邻任务为 $t_1 = (a_1,m_1)$ 和 $t_2 = (a_2,m_2)$。设初始能量为 $E_0$，完成这两个任务之前的能量为 $E$。由于从一开始到完成这两个任务之前，所消耗的能量之和是固定的 $S$，所以有 $E = E_0-S$。根据该式，$E$ 越小，初始能量 $E_0$ 也越小。

先完成哪个任务更好？

- 如果先完成 $t_1$ 再完成 $t_2$，那么完成 $t_1$ 之前，必须满足 $E\ge m_1$；完成 $t_2$ 之前，必须满足 $E-a_1\ge m_2$。联立得 $E\ge \max(m_1,m_2+a_1)$。
- 如果先完成 $t_2$ 再完成 $t_1$，那么完成 $t_2$ 之前，必须满足 $E\ge m_2$；完成 $t_1$ 之前，必须满足 $E-a_2\ge m_1$。联立得 $E\ge \max(m_2,m_1+a_2)$。

如果先完成 $t_1$ 再完成 $t_2$ 更优（或者相同），则有 

$$
\max(m_1,m_2+a_1)\le \max(m_2,m_1+a_2)
$$

两边同时减去 $a_1+a_2$，得

$$
\max(m_1-a_1-a_2,m_2-a_2)\le \max(m_2-a_1-a_2,m_1-a_1)
$$

为方便阅读，令 $X=m_1-a_1$，$Y = m_2-a_2$，上式化简为

$$
\max(X-a_2,Y)\le \max(Y-a_1,X)
$$

- 如果 $X<Y$，那么上式左侧等于 $Y$，右侧严格小于 $Y$（题目保证 $a_1 > 0$），此时 $\max(X-a_2,Y) > \max(Y-a_1,X)$。
- 如果 $X\ge Y$，那么上式左侧两个值都 $\le X$，右侧等于 $X$，此时 $\max(X-a_2,Y) \le \max(Y-a_1,X)$。

所以当且仅当 $X\ge Y$ 成立时，$\max(X-a_2,Y)\le \max(Y-a_1,X)$ 成立。

所以当且仅当 $m_1-a_1 \ge m_2-a_2$ 成立时，先完成 $t_1$ 再完成 $t_2$ 更优（或者相同）。

这意味着，对于 $\textit{tasks}$ 中的相邻任务，设左边的任务为 $(a_1,m_1)$，右边的任务为 $(a_2,m_2)$，如果 $m_1-a_1 < m_2-a_2$，那么交换这两个任务可以让初始能量变得更小（或者不变）。于是通过交换 $\textit{tasks}$ 的相邻任务（类似冒泡排序的过程），把 $\textit{tasks}$ 按照 $m_i-a_i$ 从大到小排序，可以得到 $\textit{tasks}$ 的最优排列。

## 计算初始能量（正序）

设初始能量为 $E_0$，设执行 $\textit{tasks}[i] = (a_i,m_i)$ 之前，累计耗费的能量为 $S$，那么当前能量为 $E_0 - S$，必须满足

$$
E_0 - S \ge m_i
$$

即

$$
E_0 \ge S + m_i
$$

由此可以得到 $n$ 个关于 $E_0$ 的下界，所有下界取最大值，即为答案。

```py [sol-Python3]
class Solution:
    def minimumEffort(self, tasks: List[List[int]]) -> int:
        tasks.sort(key=lambda t: t[0] - t[1])  # 按照 minimum - actual 从大到小排序

        ans = 0
        s = 0  # 累计耗费的能量
        for actual, minimum in tasks:
            # 题目要求 E0 - s >= minimum，即 E0 >= s + minimum
            # 由此可以得到 n 个关于 E0 的下界，所有下界的最大值即为答案
            ans = max(ans, s + minimum)
            s += actual
        return ans
```

```java [sol-Java]
class Solution {
    public int minimumEffort(int[][] tasks) {
        // 按照 minimum - actual 从大到小排序
        Arrays.sort(tasks, (a, b) -> (b[1] - b[0]) - (a[1] - a[0]));

        int ans = 0;
        int s = 0; // 累计耗费的能量
        for (int[] t : tasks) {
            int actual = t[0];
            int minimum = t[1];
            // 题目要求 E0 - s >= minimum，即 E0 >= s + minimum
            // 由此可以得到 n 个关于 E0 的下界，所有下界的最大值即为答案
            ans = Math.max(ans, s + minimum);
            s += actual;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumEffort(vector<vector<int>>& tasks) {
        ranges::sort(tasks, {}, [](auto& t) { return t[0] - t[1]; }); // 按照 minimum - actual 从大到小排序

        int ans = 0;
        int s = 0; // 累计耗费的能量
        for (auto& t : tasks) {
            int actual = t[0], minimum = t[1];
            // 题目要求 E0 - s >= minimum，即 E0 >= s + minimum
            // 由此可以得到 n 个关于 E0 的下界，所有下界的最大值即为答案
            ans = max(ans, s + minimum);
            s += actual;
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* p, const void* q) {
    int* a = *(int**)p;
    int* b = *(int**)q;
    return (b[1] - b[0]) - (a[1] - a[0]); // 按照 minimum - actual 从大到小排序
}

int minimumEffort(int** tasks, int tasksSize, int* tasksColSize) {
    qsort(tasks, tasksSize, sizeof(int*), cmp);

    int ans = 0;
    int s = 0; // 累计耗费的能量
    for (int i = 0; i < tasksSize; i++) {
        int actual = tasks[i][0];
        int minimum = tasks[i][1];
        // 题目要求 E0 - s >= minimum，即 E0 >= s + minimum
        // 由此可以得到 n 个关于 E0 的下界，所有下界的最大值即为答案
        ans = MAX(ans, s + minimum);
        s += actual;
    }
    return ans;
}
```

```go [sol-Go]
func minimumEffort(tasks [][]int) (ans int) {
	slices.SortFunc(tasks, func(a, b []int) int {
		return (b[1] - b[0]) - (a[1] - a[0]) // 按照 minimum - actual 从大到小排序
	})

	s := 0 // 累计耗费的能量
	for _, t := range tasks {
		actual, minimum := t[0], t[1]
		// 题目要求 E0 - s >= minimum，即 E0 >= s + minimum
		// 由此可以得到 n 个关于 E0 的下界，所有下界的最大值即为答案
		ans = max(ans, s+minimum)
		s += actual
	}
	return
}
```

```js [sol-JavaScript]
var minimumEffort = function(tasks) {
    tasks.sort((a, b) => (b[1] - b[0]) - (a[1] - a[0])); // 按照 minimum - actual 从大到小排序

    let ans = 0;
    let s = 0; // 累计耗费的能量
    for (const [actual, minimum] of tasks) {
        // 题目要求 E0 - s >= minimum，即 E0 >= s + minimum
        // 由此可以得到 n 个关于 E0 的下界，所有下界的最大值即为答案
        ans = Math.max(ans, s + minimum);
        s += actual;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_effort(mut tasks: Vec<Vec<i32>>) -> i32 {
        tasks.sort_unstable_by_key(|t| t[0] - t[1]); // 按照 minimum - actual 从大到小排序

        let mut ans = 0;
        let mut s = 0; // 累计耗费的能量
        for t in tasks {
            let actual = t[0];
            let minimum = t[1];
            // 题目要求 E0 - s >= minimum，即 E0 >= s + minimum
            // 由此可以得到 n 个关于 E0 的下界，所有下界的最大值即为答案
            ans = ans.max(s + minimum);
            s += actual;
        }
        ans
    }
}
```

## 计算初始能量（倒序）

也可以从右到左计算。

设完成任务 $t_i = (a_i,m_i)$ 之后的能量为 $E$，那么完成 $t_i$ 之前的能量为 $E+a_i$，但这个能量又必须 $\ge m_i$，所以完成 $t_i$ 之前的能量至少为

$$
\max(E+a_i, m_i)
$$

为了最小化初始能量，完成最后一个任务后的能量应当为 $0$，作为 $E$ 的初始值。

代码实现时，可以改成按照 $m_i-a_i$ 从小到大排序，这样可以正序遍历数组，更方便。

```py [sol-Python3]
class Solution:
    def minimumEffort(self, tasks: list[list[int]]) -> int:
        tasks.sort(key=lambda t: t[1] - t[0])  # 按照 minimum - actual 从小到大排序

        e = 0
        for actual, minimum in tasks:
            # 完成该任务之后的能量为 e，那么完成该任务之前的能量为 e+actual，同时该能量必须至少为 minimum
            e = max(e + actual, minimum)
        return e
```

```java [sol-Java]
class Solution {
    public int minimumEffort(int[][] tasks) {
        // 按照 minimum - actual 从小到大排序
        Arrays.sort(tasks, (a, b) -> (a[1] - a[0]) - (b[1] - b[0]));

        int e = 0;
        for (int[] t : tasks) {
            int actual = t[0];
            int minimum = t[1];
            // 完成 t 之后的能量为 e，那么完成 t 之前的能量为 e+actual，同时该能量必须至少为 minimum
            e = Math.max(e + actual, minimum);
        }
        return e;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minimumEffort(vector<vector<int>>& tasks) {
        ranges::sort(tasks, {}, [](auto& t) { return t[1] - t[0]; }); // 按照 minimum - actual 从小到大排序

        int e = 0;
        for (auto& t : tasks) {
            int actual = t[0], minimum = t[1];
            // 完成 t 之后的能量为 e，那么完成 t 之前的能量为 e+actual，同时该能量必须至少为 minimum
            e = max(e + actual, minimum);
        }
        return e;
    }
};
```

```c [sol-C]
int cmp(const void* p, const void* q) {
    int* a = *(int**)p;
    int* b = *(int**)q;
    return (a[1] - a[0]) - (b[1] - b[0]); // 按照 minimum - actual 从小到大排序
}

int minimumEffort(int** tasks, int tasksSize, int* tasksColSize) {
    qsort(tasks, tasksSize, sizeof(int*), cmp);

    int e = 0;
    for (int i = 0; i < tasksSize; i++) {
        int actual = tasks[i][0];
        int minimum = tasks[i][1];
        // 完成 tasks[i] 之后的能量为 e，那么完成 tasks[i] 之前的能量为 e+actual，同时该能量必须至少为 minimum
        e = MAX(e + actual, minimum);
    }
    return e;
}
```

```go [sol-Go]
func minimumEffort(tasks [][]int) (e int) {
	slices.SortFunc(tasks, func(a, b []int) int {
		return (a[1] - a[0]) - (b[1] - b[0]) // 按照 minimum - actual 从小到大排序
	})

	for _, t := range tasks {
		actual, minimum := t[0], t[1]
		// 完成 t 之后的能量为 e，那么完成 t 之前的能量为 e+actual，同时该能量必须至少为 minimum
		e = max(e+actual, minimum)
	}
	return
}
```

```js [sol-JavaScript]
var minimumEffort = function(tasks) {
    tasks.sort((a, b) => (a[1] - a[0]) - (b[1] - b[0])); // 按照 minimum - actual 从小到大排序

    let e = 0;
    for (const [actual, minimum] of tasks) {
        // 完成该任务之后的能量为 e，那么完成该任务之前的能量为 e+actual，同时该能量必须至少为 minimum
        e = Math.max(e + actual, minimum);
    }
    return e;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_effort(mut tasks: Vec<Vec<i32>>) -> i32 {
        tasks.sort_unstable_by_key(|t| t[1] - t[0]); // 按照 minimum - actual 从小到大排序

        let mut e = 0;
        for t in tasks {
            let actual = t[0];
            let minimum = t[1];
            // 完成 t 之后的能量为 e，那么完成 t 之前的能量为 e+actual，同时该能量必须至少为 minimum
            e = minimum.max(e + actual);
        }
        e
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{tasks}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。不计入排序的栈开销。

## 专题训练

见下面贪心题单的「**§1.7 交换论证法**」。

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

本题来自 `一、贪心策略 / §1.7 交换论证法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.7 交换论证法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
