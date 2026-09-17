# 2145. 统计隐藏数组数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-the-hidden-sequences/
- 题目 slug：`count-the-hidden-sequences`
- 来源专题：数学算法
- 来源分类路径：七、杂项 / §7.10 其他
- 难度分：1614
- 外部题解来源：https://leetcode.cn/problems/count-the-hidden-sequences/solutions/1226681/huan-yuan-chai-fen-shu-zu-by-endlesschen-v38b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[详细数学推导（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/count-the-hidden-sequences/solutions/1226681/huan-yuan-chai-fen-shu-zu-by-endlesschen-v38b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`huan-yuan-chai-fen-shu-zu-by-endlesschen-v38b`
- topic id：`1226681`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:37:43 +0800

下文把 $\textit{hidden}$ 简记为 $a$，把 $\textit{differences}$ 简记为 $d$。

题目给出等式

$$
d_i = a_{i+1} - a_i
$$

移项得

$$
a_{i+1} = a_i + d_i
$$

所以有

$$
\begin{aligned}
a_1 &= a_0 + d_0     \\
a_2 &= a_1 + d_1 = a_0 + d_0 + d_1    \\
a_3 &= a_2 + d_2 = a_0 + d_0 + d_1 + d_2   \\
&\ \ \vdots \\
a_i &= a_0 + \sum_{i=0}^{i-1} d_i
\end{aligned}
$$

计算 $d$ 的 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/) 数组 $s$，那么有

$$
a_i = a_0 + s_i
$$

这意味着，确定了 $a_0$，就确定了整个数组 $a$。所以 $a_0$ 的取值范围的大小就是答案。

题目要求

$$
\textit{lower}\le a_i\le \textit{upper}
$$

等价于

$$
\textit{lower}\le a_0 + s_i \le \textit{upper}
$$

移项得

$$
\textit{lower} - s_i \le a_0 \le \textit{upper} - s_i
$$

这可以得到 $n+1$ 个关于 $a_0$ 不等式，或者说区间 $[\textit{lower} - s_i,\textit{upper} - s_i]$。这 $n+1$ 个区间的**交集**大小，就是答案。（注意数组 $a$ 的长度是 $n+1$）

区间交集的左端点为 $\max\limits_i \{\textit{lower} - s_i\} = \textit{lower} - \min\limits_i s_i$。

区间交集的右端点为 $\min\limits_i\{ \textit{upper} - s_i \}= \textit{upper} - \max\limits_i s_i$。

交集大小为

$$
\begin{aligned}
    & (\textit{upper} - \max_i s_i) - (\textit{lower} - \min_i s_i) + 1      \\
={} & \textit{upper} - \textit{lower} -  \max_i s_i + \min_i s_i + 1     \\
\end{aligned}
$$

注意交集可能是空的，上式是负数，所以要和 $0$ 取最大值。最终答案为

$$
\max(\textit{upper} - \textit{lower} -  \max_i s_i + \min_i s_i + 1, 0)
$$

代码实现时，$\textit{minS}$ 和 $\textit{maxS}$ 可以初始化成 $0$，这是因为前缀和数组中的 $s_0=0$。

```py [sol-Python3]
class Solution:
    def numberOfArrays(self, differences: List[int], lower: int, upper: int) -> int:
        min_s = min(accumulate(differences, initial=0))  # 前缀和的最小值
        max_s = max(accumulate(differences, initial=0))  # 前缀和的最大值
        return max(upper - lower - max_s + min_s + 1, 0)
```

```java [sol-Java]
class Solution {
    public int numberOfArrays(int[] differences, int lower, int upper) {
        long s = 0, minS = 0, maxS = 0; // s[0] = 0
        for (int d : differences) {
            s += d;
            minS = Math.min(minS, s);
            maxS = Math.max(maxS, s);
        }
        return (int) Math.max(upper - lower - maxS + minS + 1, 0);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int numberOfArrays(vector<int>& differences, int lower, int upper) {
        long long s = 0, min_s = 0, max_s = 0; // s[0] = 0
        for (int d : differences) {
            s += d;
            min_s = min(min_s, s);
            max_s = max(max_s, s);
        }
        return max(upper - lower - max_s + min_s + 1, 0LL);
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int numberOfArrays(int* differences, int differencesSize, int lower, int upper) {
    long long s = 0, min_s = 0, max_s = 0; // s[0] = 0
    for (int i = 0; i < differencesSize; i++) {
        s += differences[i];
        min_s = MIN(min_s, s);
        max_s = MAX(max_s, s);
    }
    return MAX(upper - lower - max_s + min_s + 1, 0);
}
```

```go [sol-Go]
func numberOfArrays(differences []int, lower, upper int) int {
    var s, minS, maxS int // s[0] = 0
    for _, d := range differences {
        s += d
        minS = min(minS, s)
        maxS = max(maxS, s)
    }
    return max(upper-lower-maxS+minS+1, 0)
}
```

```js [sol-JavaScript]
var numberOfArrays = function(differences, lower, upper) {
    let s = 0, minS = 0, maxS = 0; // s[0] = 0
    for (const d of differences) {
        s += d;
        minS = Math.min(minS, s);
        maxS = Math.max(maxS, s);
    }
    return Math.max(upper - lower - maxS + minS + 1, 0);
};
```

```rust [sol-Rust]
impl Solution {
    pub fn number_of_arrays(differences: Vec<i32>, lower: i32, upper: i32) -> i32 {
        let mut s = 0; // s[0] = 0
        let mut min_s = 0;
        let mut max_s = 0;
        for d in differences {
            s += d as i64;
            min_s = min_s.min(s);
            max_s = max_s.max(s);
        }
        0.max((upper - lower + 1) as i64 - max_s + min_s) as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{differences}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

#### 相似题目

- [3468. 可行数组的数目](https://leetcode.cn/problems/find-the-number-of-copy-arrays/) 1545

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
