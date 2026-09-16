# 983. 最低票价

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-for-tickets/
- 题目 slug：`minimum-cost-for-tickets`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.1 一维 DP
- 难度分：1786
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-for-tickets/solutions/2936177/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-tkw4/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[教你一步步思考 DP：从记忆化搜索到递推到三指针优化！（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-cost-for-tickets/solutions/2936177/jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-tkw4/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`jiao-ni-yi-bu-bu-si-kao-dpcong-ji-yi-hua-tkw4`
- topic id：`2936177`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 一、寻找子问题

假设第 $100$ 天是旅行的最后一天，分类讨论：

- 在第 $100$ 天购买为期 $1$ 天的通行证，接下来需要解决的问题为：$1$ 到 $99$ 天的最小花费。
- 在第 $94$ 天购买为期 $7$ 天的通行证，接下来需要解决的问题为：$1$ 到 $93$ 天的最小花费。
- 在第 $71$ 天购买为期 $30$ 天的通行证，接下来需要解决的问题为：$1$ 到 $70$ 天的最小花费。

这些问题都是**和原问题相似的、规模更小的子问题**，可以用**递归**解决。

> 注 1：从右往左思考，主要是为了方便把递归翻译成递推。从左往右思考也是可以的。
> 
> 注 2：动态规划有「**选或不选**」和「**枚举选哪个**」两种基本思考方式。在做题时，可根据题目要求，选择适合题目的一种来思考。本题用到的是「**枚举选哪个**」。

## 二、状态定义与状态转移方程

根据上面的讨论，定义 $\textit{dfs}(i)$ 表示 $1$ 到 $i$ 天的最小花费。

如果第 $i$ 天不在 $\textit{days}$ 中，那么问题变成 $1$ 到 $i-1$ 天的最小花费，即 

$$
\textit{dfs}(i) = \textit{dfs}(i-1)
$$

如果第 $i$ 天在 $\textit{days}$ 中，分类讨论：

- 在第 $i$ 天购买为期 $1$ 天的通行证，接下来需要解决的问题为：$1$ 到 $i-1$ 天的最小花费，即 $\textit{dfs}(i) = \textit{dfs}(i-1)+\textit{costs}[0]$。
- 在第 $i-6$ 天购买为期 $7$ 天的通行证，接下来需要解决的问题为：$1$ 到 $i-7$ 天的最小花费，即 $\textit{dfs}(i) = \textit{dfs}(i-7)+\textit{costs}[1]$。
- 在第 $i-29$ 天购买为期 $30$ 天的通行证，接下来需要解决的问题为：$1$ 到 $i-30$ 天的最小花费，即 $\textit{dfs}(i) = \textit{dfs}(i-30)+\textit{costs}[2]$。

这三种情况取最小值，就得到了 $\textit{dfs}(i)$，即

$$
\textit{dfs}(i) = \min(\textit{dfs}(i-1)+\textit{costs}[0],\textit{dfs}(i-7)+\textit{costs}[1],\textit{dfs}(i-30)+\textit{costs}[2])
$$

**递归边界**：$\textit{dfs}(i)=0$，其中 $i\le 0$。此时没有要旅行的天数。

**递归入口**：$\textit{dfs}(D)$，其中 $D=\textit{days}[n-1]$ 是最后一天。为了方便翻译成递推，我们从最后一天开始思考。

## 三、递归搜索 + 保存递归返回值 = 记忆化搜索

考虑到整个递归过程中有大量重复递归调用（递归入参相同）。由于递归函数没有副作用，同样的入参无论计算多少次，算出来的结果都是一样的，因此可以用**记忆化搜索**来优化：

- 如果一个状态（递归入参）是第一次遇到，那么可以在返回前，把状态及其结果记到一个 $\textit{memo}$ 数组中。
- 如果一个状态不是第一次遇到（$\textit{memo}$ 中保存的结果不等于 $\textit{memo}$ 的初始值），那么可以直接返回 $\textit{memo}$ 中保存的结果。

**注意**：$\textit{memo}$ 数组的**初始值**一定不能等于要记忆化的值！例如初始值设置为 $0$，并且要记忆化的 $\textit{dfs}(i)$ 也等于 $0$，那就没法判断 $0$ 到底表示第一次遇到这个状态，还是表示之前遇到过了，从而导致记忆化失效。一般把初始值设置为 $-1$。

本题由于 $\textit{costs}[i]$ 均为正数，不会算出 $0$，把初始值设置为 $0$ 也可以。

> Python 用户可以无视上面这段，直接用 `@cache` 装饰器。

具体请看视频讲解 [动态规划入门：从记忆化搜索到递推](https://www.bilibili.com/video/BV1Xj411K7oF/)，其中包含把记忆化搜索 1:1 翻译成递推的技巧。

```py [sol-Python3]
class Solution:
    def mincostTickets(self, days: List[int], costs: List[int]) -> int:
        last_day = days[-1]
        days = set(days)
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int) -> int:
            if i <= 0:
                return 0
            if i not in days:
                return dfs(i - 1)
            return min(dfs(i - 1) + costs[0], dfs(i - 7) + costs[1], dfs(i - 30) + costs[2])
        return dfs(last_day)
```

```java [sol-Java]
class Solution {
    public int mincostTickets(int[] days, int[] costs) {
        int lastDay = days[days.length - 1];
        boolean[] isTravel = new boolean[lastDay + 1];
        for (int d : days) {
            isTravel[d] = true;
        }
        int[] memo = new int[lastDay + 1];
        return dfs(lastDay, isTravel, costs, memo);
    }

    private int dfs(int i, boolean[] isTravel, int[] costs, int[] memo) {
        if (i <= 0) {
            return 0;
        }
        if (memo[i] > 0) { // 之前计算过
            return memo[i];
        }
        if (!isTravel[i]) {
            return memo[i] = dfs(i - 1, isTravel, costs, memo);
        }
        return memo[i] = Math.min(dfs(i - 1, isTravel, costs, memo) + costs[0],
                         Math.min(dfs(i - 7, isTravel, costs, memo) + costs[1],
                                  dfs(i - 30, isTravel, costs, memo) + costs[2]));
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int mincostTickets(vector<int>& days, vector<int>& costs) {
        int last_day = days.back();
        unordered_set<int> day_set(days.begin(), days.end());
        vector<int> memo(last_day + 1);
        auto dfs = [&](auto&& dfs, int i) -> int {
            if (i <= 0) {
                return 0;
            }
            int& res = memo[i]; // 注意这里是引用
            if (res) { // 之前计算过
                return res;
            }
            if (!day_set.count(i)) {
                return res = dfs(dfs, i - 1);
            }
            return res = min({dfs(dfs, i - 1) + costs[0],
                              dfs(dfs, i - 7) + costs[1],
                              dfs(dfs, i - 30) + costs[2]});
        };
        return dfs(dfs, last_day);
    }
};
```

```go [sol-Go]
func mincostTickets(days, costs []int) int {
    lastDay := days[len(days)-1]
    isTravel := make([]bool, lastDay+1)
    for _, day := range days {
        isTravel[day] = true
    }
    memo := make([]int, lastDay+1)
    var dfs func(int) int
    dfs = func(i int) (res int) {
        if i <= 0 {
            return
        }
        p := &memo[i]
        if *p > 0 { // 之前计算过
            return *p
        }
        defer func() { *p = res }() // 记忆化
        if !isTravel[i] {
            return dfs(i - 1)
        }
        return min(dfs(i-1)+costs[0], dfs(i-7)+costs[1], dfs(i-30)+costs[2])
    }
    return dfs(lastDay)
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(D)$，其中 $D=\textit{days}[n-1]$，$n$ 为 $\textit{days}$ 的长度。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(D)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以总的时间复杂度为 $\mathcal{O}(D)$。
- 空间复杂度：$\mathcal{O}(D)$。保存多少状态，就需要多少空间。

## 四、1:1 翻译成递推

我们可以去掉递归中的「递」，只保留「归」的部分，即自底向上计算。

具体来说，$f[i]$ 的定义和 $\textit{dfs}(i)$ 的定义是一样的，都表示 $1$ 到 $i$ 天的最小花费。

相应的递推式（状态转移方程）也和 $\textit{dfs}$ 一样：

$$
f[i] = \min(f[i-1]+\textit{costs}[0],f[i-7]+\textit{costs}[1],f[i-30]+\textit{costs}[2])
$$

由于 $f[0]=0$ 且负数 $i$ 的状态值也为 $0$，我们可以把负数 $i$ 视作 $0$，上式等价于

$$
f[i] = \min(f[i-1]+\textit{costs}[0],f[\max(i-7,0)]+\textit{costs}[1],f[\max(i-30,0)]+\textit{costs}[2])
$$

初始值 $f[0]=0$，翻译自递归边界 $\textit{dfs}(0)=0$。

答案为 $f[D]$，翻译自递归入口 $\textit{dfs}(D)$。

```py [sol-Python3]
class Solution:
    def mincostTickets(self, days: List[int], costs: List[int]) -> int:
        last_day = days[-1]
        days = set(days)
        f = [0] * (last_day + 1)
        for i in range(1, last_day + 1):
            if i not in days:
                f[i] = f[i - 1]
            else:
                f[i] = min(f[i - 1] + costs[0], 
                           f[max(i - 7, 0)] + costs[1],
                           f[max(i - 30, 0)] + costs[2])
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int mincostTickets(int[] days, int[] costs) {
        int lastDay = days[days.length - 1];
        boolean[] isTravel = new boolean[lastDay + 1];
        for (int d : days) {
            isTravel[d] = true;
        }
        int[] f = new int[lastDay + 1];
        for (int i = 1; i <= lastDay; i++) {
            if (!isTravel[i]) {
                f[i] = f[i - 1];
            } else {
                f[i] = Math.min(f[i - 1] + costs[0],
                       Math.min(f[Math.max(i - 7, 0)] + costs[1],
                                f[Math.max(i - 30, 0)] + costs[2]));
            }
        }
        return f[lastDay];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int mincostTickets(vector<int>& days, vector<int>& costs) {
        int last_day = days.back();
        unordered_set<int> day_set(days.begin(), days.end());
        vector<int> f(last_day + 1);
        for (int i = 1; i <= last_day; i++) {
            if (!day_set.contains(i)) {
                f[i] = f[i - 1];
            } else {
                f[i] = min({f[i - 1] + costs[0],
                            f[max(i - 7, 0)] + costs[1],
                            f[max(i - 30, 0)] + costs[2]});
            }
        }
        return f[last_day];
    }
};
```

```go [sol-Go]
func mincostTickets(days, costs []int) int {
    lastDay := days[len(days)-1]
    isTravel := make([]bool, lastDay+1)
    for _, day := range days {
        isTravel[day] = true
    }
    f := make([]int, lastDay+1)
    for i := 1; i <= lastDay; i++ {
        if !isTravel[i] {
            f[i] = f[i-1]
        } else {
            f[i] = min(f[i-1]+costs[0], f[max(i-7, 0)]+costs[1], f[max(i-30, 0)]+costs[2])
        }
    }
    return f[lastDay]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(D)$，其中 $D=\textit{days}[n-1]$，$n$ 为 $\textit{days}$ 的长度。
- 空间复杂度：$\mathcal{O}(D)$。

## 五、三指针优化

如果把数据范围修改为 $\textit{days}[i]\le 10^9$，上面的做法就不行了。

能不能做到时间复杂度和 $D$ 无关呢？比如只和 $\textit{days}$ 的长度 $n$ 有关？

既然要做到只和 $n$ 有关，那么仿照上面的状态定义，我们定义 $f[i]$ 表示旅行了 $i$ 天的最小花费：

- $f[0]$ 表示旅行 $0$ 天的最小花费，根据定义，$f[0]=0$。
- $f[1]$ 表示旅行 $1$ 天的最小花费，也就是完成 $\textit{days}[0]$ 的最小花费。
- $f[2]$ 表示旅行 $2$ 天的最小花费，也就是完成 $\textit{days}[0]$ 和 $\textit{days}[1]$ 的最小花费。

一般地，$f[i+1]$ 表示完成 $\textit{days}[0]$ 到 $\textit{days}[i]$ 的最小花费。

分类讨论：

- 在 $\textit{days}[i]$ 购买为期 $1$ 天的通行证，接下来需要解决的问题为：完成 $\textit{days}[0]$ 到 $\textit{days}[i-1]$ 的最小花费，即 $f[i+1] = f[i]+\textit{costs}[0]$。
- 在第 $\textit{days}[j]$ 天购买为期 $7$ 天的通行证，满足 $\textit{days}[j]+7 > \textit{days}[i]$（注意不是 $\ge$），接下来需要解决的问题为：完成 $\textit{days}[0]$ 到 $\textit{days}[j-1]$ 的最小花费，即 $f[i+1] = f[j]+\textit{costs}[1]$。
- 在第 $\textit{days}[k]$ 天购买为期 $30$ 天的通行证，满足 $\textit{days}[k]+30 > \textit{days}[i]$，接下来需要解决的问题为：完成 $\textit{days}[0]$ 到 $\textit{days}[k-1]$ 的最小花费，即 $f[i+1] = f[k]+\textit{costs}[2]$。

这三种情况取最小值，就得到了 $f[i+1]$，即

$$
f[i+1] = \min(f[i]+\textit{costs}[0],f[j]+\textit{costs}[1],f[k]+\textit{costs}[2])
$$

其中：

- $j$ 是满足 $\textit{days}[j] > \textit{days}[i]-7$ 的最小的 $j$。
- $k$ 是满足 $\textit{days}[k] > \textit{days}[i]-30$ 的最小的 $k$。

由于 $\textit{days}$ 是有序数组，计算 $j$ 和 $k$ 可以用双指针（三指针）算法。

初始值 $f[0]=0,\ j=0,\ k=0$。

答案为 $f[n]$。

```py [sol-Python3]
class Solution:
    def mincostTickets(self, days: List[int], costs: List[int]) -> int:
        f = [0] * (len(days) + 1)
        j = k = 0
        for i, d in enumerate(days):
            while days[j] <= d - 7:
                j += 1
            while days[k] <= d - 30:
                k += 1
            f[i + 1] = min(f[i] + costs[0], f[j] + costs[1], f[k] + costs[2])
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int mincostTickets(int[] days, int[] costs) {
        int n = days.length;
        int[] f = new int[n + 1];
        int j = 0;
        int k = 0;
        for (int i = 0; i < n; i++) {
            int d = days[i];
            while (days[j] <= d - 7) {
                j++;
            }
            while (days[k] <= d - 30) {
                k++;
            }
            f[i + 1] = Math.min(f[i] + costs[0], Math.min(f[j] + costs[1], f[k] + costs[2]));
        }
        return f[n];
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int mincostTickets(vector<int>& days, vector<int>& costs) {
        int n = days.size();
        vector<int> f(n + 1);
        int j = 0, k = 0;
        for (int i = 0; i < n; i++) {
            int d = days[i];
            while (days[j] <= d - 7) {
                j++;
            }
            while (days[k] <= d - 30) {
                k++;
            }
            f[i + 1] = min({f[i] + costs[0], f[j] + costs[1], f[k] + costs[2]});
        }
        return f[n];
    }
};
```

```go [sol-Go]
func mincostTickets(days, costs []int) int {
    n := len(days)
    f := make([]int, n+1)
    j, k := 0, 0
    for i, d := range days {
        for days[j] <= d-7 {
            j++
        }
        for days[k] <= d-30 {
            k++
        }
        f[i+1] = min(f[i]+costs[0], f[j]+costs[1], f[k]+costs[2])
    }
    return f[n]
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{days}$ 的长度。注意二重循环中的下标 $j$ 和 $k$ 都只会增大，不会减小或者重置。由于下标只会增大 $\mathcal{O}(n)$ 次，所以二重循环的总循环次数是 $\mathcal{O}(n)$ 的。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

见 [动态规划题单](https://leetcode.cn/circle/discuss/tXLS3i/) 中的「**§7.1 一维线性 DP**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.1 一维 DP`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.1 一维 DP`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
