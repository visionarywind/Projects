# 1997. 访问完所有房间的第一天

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/first-day-where-you-have-been-in-all-the-rooms/
- 题目 slug：`first-day-where-you-have-been-in-all-the-rooms`
- 来源专题：动态规划
- 来源分类路径：十一、优化 DP / §11.1 前缀和优化 DP
- 难度分：2260
- 外部题解来源：https://leetcode.cn/problems/first-day-where-you-have-been-in-all-the-rooms/solutions/979221/qian-zhui-he-you-hua-dp-by-endlesscheng-j10b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和优化 DP（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/first-day-where-you-have-been-in-all-the-rooms/solutions/979221/qian-zhui-he-you-hua-dp-by-endlesscheng-j10b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-you-hua-dp-by-endlesscheng-j10b`
- topic id：`979221`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 观察

根据题意，首次访问房间 $i$ 时，因为我们是第一次访问，$1$ 是奇数，所以下一天一定要访问 $j=\textit{nextVisit}[i]$ 房间，下文简称为「回访」。

由于访问偶数次才能访问右边的下一个房间，所以对于 $i$ 左边的房间，**我们一定都访问了偶数次**（不然不可能到达 $i$）。

这意味着，当我们从 $i$ 回到 $j$ 时，此时 $[j,i-1]$ 范围内的房间都处于访问偶数次的状态。那么当我们访问这个范围内的每个房间时，算上本次访问，访问次数一定是奇数，所以要想重新回到 $i$，对于 $[j,i-1]$ 范围内的每个房间，我们都需要执行一次「回访」。

## 状态设计

对于房间 $i$，其访问次数的奇偶性变化如下表：

| 描述  | 奇偶性  |
|---|---|
|  访问房间 $i$ 之前 | 偶数  |
|  访问到房间 $i$ |  奇数 |
|  回访完毕，**重新回到**房间 $i$ |  偶数 |

这说明从「访问到房间 $i$ 且次数为奇数」到「访问到房间 $i$ 且次数为偶数」是一个**完整的周期**，适合作为状态，即定义 $f[i]$ 表示从「访问到房间 $i$ 且次数为奇数」到「访问到房间 $i$ 且次数为偶数」所需要的天数。

请注意，这个定义包含了首次访问房间 $i$ 的一天，和重新回到房间 $i$ 的一天。

## 状态转移方程

由于 $[j,i-1]$ 范围内的每个房间都需要「回访」，所以需要把这个范围内的 $f$ 值都加起来，再算上房间 $i$ 需要访问 $2$ 次，于是得到如下状态转移方程：

$$
f[i] = 2 + \sum_{k=j}^{i-1} f[k] \tag{1}
$$

其中和式可以用前缀和优化，请看 [前缀和原理](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

定义前缀和 $s[0]=0,\ s[i+1]=\sum\limits_{j=0}^{i}f[i]$。

于是 $(1)$ 式化简为

$$
f[i] = 2 + s[i] - s[j] \tag{2}
$$

对于前缀和 $s$，有如下递推式

$$
s[i+1] = s[i] + f[i]  \tag{3}
$$

$(2)(3)$ 联立得

$$
s[i+1] = s[i]\cdot 2 - s[j] + 2  \tag{4}
$$

通过一番计算，我们省去了 $f$ 数组，$(4)$ 式就是最终的状态转移方程。

## 答案

访问完所有房间，相当于访问完 $0$ 号房间的天数，加上访问完 $1$ 号房间的天数，加上访问完 $2$ 号房间的天数，……，加上访问完 $n-2$ 号房间的天数，再加上访问第 $n-1$ 号房间的 $1$ 天。

所以总共需要

$$
f[0] + f[1] + \cdots + f[n-2] + 1 = s[n-1] + 1
$$

天。

由于我们从第 $0$ 天开始计数，上式需要减一，所以访问完所有房间时的**日期编号**为

$$
s[n-1]+1-1 = s[n-1]
$$

**注**：题目要求 $(i+1)\bmod n$，这个取模其实是多余的，因为当我们首次访问到 $n-1$ 号房间时，整个流程就结束了，在此之前 $i < n-1$，所以 $(i+1)\bmod n$ 其实就是 $i+1$。

#### 答疑

**问**：代码中的注释 `+ MOD 避免算出负数` 是什么意思？

**答**：$s$ 是前缀和，看上去它是个递增的序列，但是取模后，谁大谁小就不一定了，取模后 $s$ 就没有有序的性质了，那么在计算减法时，就可能会产生负数。为了保证计算结果非负，可以加上 `MOD`，这一方面不影响取模结果，另一方面保证了 `MOD - s[j]` 非负，从而保证计算结果非负。

注：Python 用户无需关心此问题，因为 Python 中的取模保证结果非负，例如 `-2 % 3` 在 Python 中的计算结果是 `1` 而不是 `-2`。

```py [sol-Python3]
class Solution:
    def firstDayBeenInAllRooms(self, nextVisit: List[int]) -> int:
        s = [0] * len(nextVisit)
        for i, j in enumerate(nextVisit[:-1]):
            s[i + 1] = (s[i] * 2 - s[j] + 2) % 1_000_000_007
        return s[-1]
```

```java [sol-Java]
class Solution {
    public int firstDayBeenInAllRooms(int[] nextVisit) {
        final long MOD = 1_000_000_007;
        int n = nextVisit.length;
        long[] s = new long[n];
        for (int i = 0; i < n - 1; i++) {
            int j = nextVisit[i];
            s[i + 1] = (s[i] * 2 - s[j] + 2 + MOD) % MOD; // + MOD 避免算出负数
        }
        return (int) s[n - 1];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int firstDayBeenInAllRooms(vector<int>& nextVisit) {
        const int MOD = 1'000'000'007;
        int n = nextVisit.size();
        vector<long> s(n);
        for (int i = 0; i < n - 1; i++) {
            int j = nextVisit[i];
            s[i + 1] = (s[i] * 2 - s[j] + 2 + MOD) % MOD; // + MOD 避免算出负数
        }
        return s[n - 1];
    }
};
```

```go [sol-Go]
func firstDayBeenInAllRooms(nextVisit []int) int {
    const mod = 1_000_000_007
    n := len(nextVisit)
    s := make([]int, n)
    for i, j := range nextVisit[:n-1] {
        s[i+1] = (s[i]*2 - s[j] + 2 + mod) % mod // + mod 避免算出负数
    }
    return s[n-1]
}
```

```js [sol-JavaScript]
var firstDayBeenInAllRooms = function(nextVisit) {
    const MOD = 1_000_000_007;
    const n = nextVisit.length;
    const s = Array(n);
    s[0] = 0;
    for (let i = 0; i < n - 1; i++) {
        const j = nextVisit[i];
        s[i + 1] = (s[i] * 2 - s[j] + 2 + MOD) % MOD; // + MOD 避免算出负数
    }
    return s[n - 1];
};
```

```rust [sol-Rust]
impl Solution {
    pub fn first_day_been_in_all_rooms(next_visit: Vec<i32>) -> i32 {
        const MOD: i64 = 1_000_000_007;
        let n = next_visit.len();
        let mut s = vec![0; n];
        for i in 0..n - 1 {
            let j = next_visit[i] as usize;
            s[i + 1] = (s[i] * 2 - s[j] + 2 + MOD) % MOD; // + MOD 避免算出负数
        }
        s[n - 1] as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{nextVisit}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。如果把前缀和保存在 $\textit{nextVisit}$ 数组中，可以做到 $\mathcal{O}(1)$ 额外空间，留给读者作为练习。

**注**：本题是 [CF1552F. Telepanting](https://codeforces.com/problemset/problem/1552/F) 的简化版。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)

更多题单，点我个人主页 - 讨论发布。

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[往期题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `十一、优化 DP / §11.1 前缀和优化 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `十一、优化 DP / §11.1 前缀和优化 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
