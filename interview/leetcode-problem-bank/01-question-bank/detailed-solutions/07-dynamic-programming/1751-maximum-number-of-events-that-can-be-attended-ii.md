# 1751. 最多可以参加的会议数目 II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-events-that-can-be-attended-ii/
- 题目 slug：`maximum-number-of-events-that-can-be-attended-ii`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.2 不相交区间
- 难度分：2041
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-events-that-can-be-attended-ii/solutions/1913087/dong-tai-gui-hua-er-fen-cha-zhao-you-hua-fuip/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划 + 二分查找优化（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-number-of-events-that-can-be-attended-ii/solutions/1913087/dong-tai-gui-hua-er-fen-cha-zhao-you-hua-fuip/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-er-fen-cha-zhao-you-hua-fuip`
- topic id：`1913087`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

请先完成本题的简单版本：[1235. 规划兼职工作](https://leetcode.cn/problems/maximum-profit-in-job-scheduling/)，[我的题解](https://leetcode.cn/problems/maximum-profit-in-job-scheduling/solutions/1913089/dong-tai-gui-hua-er-fen-cha-zhao-you-hua-zkcg/)。

> 注意 1235 题两个相邻区间的端点可以相同，本题不能相同。

本题在 1235 的基础上，约束参加的会议个数至多为 $k$。相应地，DP 要增加一个参数 $j$，表示至多参加 $j$ 个会议。

和 1235 一样，按照结束时间排序。定义 $f[i+1][j]$ 表示参加 $\textit{events}[0]$ 到 $\textit{events}[i]$ 中的**至多** $j$ 个会议，能得到的最大价值和。其中 $+1$ 是为了方便用 $f[0]$ 表示初始值，即没有会议的情况。

分类讨论：

- 不参加 $\textit{events}[i]$，问题变成参加 $\textit{events}[0]$ 到 $\textit{events}[i-1]$ 中的至多 $j$ 个会议，能得到的最大价值和，即 $f[i][j]$；
- 参加 $\textit{events}[i]$，设 $p$ 是最大的满足 $\textit{endDay}_p<\textit{startDay}_i$ 的 $p$（若不存在则 $p=-1$），问题变成参加 $\textit{events}[0]$ 到 $\textit{events}[p]$ 中的至多 $j-1$ 个会议，能得到的会议价值的最大和，即 $f[p+1][j-1] + \textit{value}_i$。 

两种情况取最大值，得到**状态转移方程**：

$$
f[i+1][j] = \max(f[i][j], f[p+1][j-1] + \textit{value}_i)
$$

由于结束时间是有序的（我们排过序了），$p$ 可以用**二分查找**快速计算，原理见 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

**初始值**：$f[0][j] = 0$。没有会议，价值和为 $0$。

**答案**：$f[n][k]$。

**优化**：如果 $k=1$，只能参加一个会议，直接返回最大的价值。

```py [sol-Python3]
# 手写 max 更快
fmax = lambda a, b: b if b > a else a

class Solution:
    def maxValue(self, events: List[List[int]], k: int) -> int:
        # 特判 k=1 的情况可以更快
        if k == 1:
            return max(e[2] for e in events)

        events.sort(key=lambda e: e[1])  # 按照结束时间排序
        n = len(events)
        f = [[0] * (k + 1) for _ in range(n + 1)]
        for i, (start_day, _, value) in enumerate(events):
            p = bisect_left(events, start_day, hi=i, key=lambda e: e[1])  # hi=i 表示二分上界为 i（默认为 n）
            for j in range(1, k + 1):
                # 为什么是 p 不是 p+1：上面算的是 >= start_day，-1 后得到 < start_day，但由于还要 +1，抵消了
                f[i + 1][j] = fmax(f[i][j], f[p][j - 1] + value)
        return f[n][k]
```

```java [sol-Java]
class Solution {
    public int maxValue(int[][] events, int k) {
        // 特判 k=1 的情况可以更快
        if (k == 1) {
            int mx = 0;
            for (int[] e : events) {
                mx = Math.max(mx, e[2]);
            }
            return mx;
        }

        Arrays.sort(events, (a, b) -> a[1] - b[1]); // 按照结束时间排序
        int n = events.length;
        int[][] f = new int[n + 1][k + 1];
        for (int i = 0; i < n; i++) {
            int p = search(events, i, events[i][0]);
            for (int j = 1; j <= k; j++) {
                f[i + 1][j] = Math.max(f[i][j], f[p + 1][j - 1] + events[i][2]);
            }
        }
        return f[n][k];
    }

    // 返回 endDay[i] < upper 的最大 i
    private int search(int[][] events, int right, int upper) {
        int left = -1;
        while (left + 1 < right) {
            int mid = left + (right - left) / 2;
            if (events[mid][1] < upper) {
                left = mid;
            } else {
                right = mid;
            }
        }
        return left;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxValue(vector<vector<int>>& events, int k) {
        // 特判 k=1 的情况可以更快
        if (k == 1) {
            return ranges::max(events, {}, [](auto& e) { return e[2]; })[2];
        }

        ranges::sort(events, {}, [](auto& e) { return e[1]; }); // 按照结束时间排序
        int n = events.size();
        vector f(n + 1, vector<int>(k + 1));
        for (int i = 0; i < n; i++) {
            int p = lower_bound(events.begin(), events.begin() + i, events[i][0],
                                [](auto& e, int lower) { return e[1] < lower; }) - events.begin();
            for (int j = 1; j <= k; j++) {
                // 为什么是 p 不是 p+1：上面算的是 >= startDay，-1 后得到 < startDay，但由于还要 +1，抵消了
                f[i + 1][j] = max(f[i][j], f[p][j - 1] + events[i][2]);
            }
        }
        return f[n][k];
    }
};
```

```go [sol-Go]
func maxValue(events [][]int, k int) int {
    // 特判 k=1 的情况可以更快
    if k == 1 {
        mx := 0
        for _, e := range events {
            mx = max(mx, e[2])
        }
        return mx
    }

    slices.SortFunc(events, func(a, b []int) int { return a[1] - b[1] })
    n := len(events)
    f := make([][]int, n+1)
    for i := range f {
        f[i] = make([]int, k+1)
    }
    for i, e := range events {
        startDay, value := e[0], e[2]
        p := sort.Search(i, func(j int) bool { return events[j][1] >= startDay })
        for j := 1; j <= k; j++ {
            // 为什么是 p 不是 p+1：上面算的是 >= startDay，-1 后得到 < startDay，但由于还要 +1，抵消了
            f[i+1][j] = max(f[i][j], f[p][j-1]+value)
        }
    }
    return f[n][k]
}
```

```js [sol-JavaScript]
var maxValue = function(events, k) {
    // 特判 k=1 的情况可以更快
    if (k === 1) {
        return _.maxBy(events, e => e[2])[2];
    }

    events.sort((a, b) => a[1] - b[1]);
    const n = events.length;
    const f = Array.from({ length: n + 1 }, () => Array(k + 1).fill(0));
    for (let i = 0; i < n; i++) {
        const p = search(events, i, events[i][0]);
        for (let j = 1; j <= k; j++) {
            f[i + 1][j] = Math.max(f[i][j], f[p + 1][j - 1] + events[i][2]);
        }
    }
    return f[n][k];
};

// 返回 endDay[i] < upper 的最大 i
var search = function(events, right, upper) {
    let left = -1;
    while (left + 1 < right) {
        const mid = (left + right) >>> 1;
        if (events[mid][1] < upper) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return left;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_value(mut events: Vec<Vec<i32>>, k: i32) -> i32 {
        // 特判 k=1 的情况可以更快
        if k == 1 {
            return events.into_iter().map(|e| e[2]).max().unwrap();
        }

        events.sort_unstable_by_key(|a| a[1]);
        let n = events.len();
        let k = k as usize;
        let mut f = vec![vec![0; k + 1]; n + 1];
        for (i, e) in events.iter().enumerate() {
            let p = events[..i].partition_point(|a| a[1] < e[0]);
            for j in 1..=k {
                // 为什么是 p 不是 p+1：上面算的是 >= startDay，-1 后得到 < startDay，但由于还要 +1，抵消了
                f[i + 1][j] = f[i][j].max(f[p][j - 1] + e[2]);
            }
        }
        f[n][k]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(nk+n\log n)$，其中 $n$ 为 $\textit{events}$ 的长度。
- 空间复杂度：$\mathcal{O}(nk)$。

**注**：也可以交换 DP 的两个维度，外层循环枚举 $j$，内层循环枚举 $i$。预处理二分结果后，可以用滚动数组优化，做到 $\mathcal{O}(n)$ 的空间复杂度（时间复杂度不变）。留给感兴趣的读者实现。

## 专题训练

见下面动态规划题单的「**§7.2 不相交区间**」。

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

本题来自 `七、其他线性 DP / §7.2 不相交区间`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.2 不相交区间`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
