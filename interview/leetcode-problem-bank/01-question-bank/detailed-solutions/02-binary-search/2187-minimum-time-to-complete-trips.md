# 2187. 完成旅途的最少时间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-time-to-complete-trips/
- 题目 slug：`minimum-time-to-complete-trips`
- 来源专题：二分算法
- 来源分类路径：二、二分答案 / §2.1 求最小 / 答疑
- 难度分：1641
- 外部题解来源：https://leetcode.cn/problems/minimum-time-to-complete-trips/solutions/1295955/er-fen-da-an-python-yi-xing-gao-ding-by-xwvs8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[二分答案，附题单（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/minimum-time-to-complete-trips/solutions/1295955/er-fen-da-an-python-yi-xing-gao-ding-by-xwvs8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`er-fen-da-an-python-yi-xing-gao-ding-by-xwvs8`
- topic id：`1295955`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 思路

时间越多，可以完成的旅途也就越多；时间越少，可以完成的旅途也就越少。有这样的性质，我们可以**二分猜答案**。关于二分的原理，请看视频[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

猜测答案为 $x$，问题变成：

- 每辆车都用时 $x$，总共能完成多少趟旅途？能否达到 $\textit{totalTrips}$？

根据题意，我们可以完成

$$
\sum\limits_{i=0}^{n-1} \left\lfloor\dfrac{x}{\textit{time}[i]}\right\rfloor
$$

趟旅途，将它与 $\textit{totalTrips}$ 比较，如果比 $\textit{totalTrips}$ 小，说明二分的答案小了，更新二分区间左端点 $\textit{left}$，否则更新二分区间右端点 $\textit{right}$。

## 细节

下面代码采用开区间二分，这仅仅是二分的一种写法，使用闭区间或者半闭半开区间都是可以的。

- 开区间左端点初始值：$\min(\textit{time})-1$。这个时间任何车都没法完成一趟旅途，一定不满足题目要求。
- 开区间右端点初始值：让**最快**的车完成 $\textit{totalTrips}$ 趟旅途，用时 $\min(\textit{time}) \cdot \textit{totalTrips}$，这个时间一定能满足题目要求。

**优化（可选）**：

- 开区间左端点初始值：假设所有 $\textit{time}[i]$ 都等于 $\min(\textit{time})$，那么每辆车可以完成的旅途为 $k=\dfrac{x}{\min(\textit{time})}$。需要满足 $k\cdot n \ge \textit{totalTrips}$，即 $k\ge \left\lceil\dfrac{\textit{totalTrips}}{n}\right\rceil$。取 $x= \min(\textit{time})\cdot\left\lceil\dfrac{\textit{totalTrips}}{n}\right\rceil - 1$，这个时间一定不满足题目要求。
- 开区间右端点初始值：假设所有 $\textit{time}[i]$ 都等于 $\max(\textit{time})$，那么每辆车可以完成的旅途为 $k=\dfrac{x}{\max(\textit{time})}$。需要满足 $k\cdot n \ge \textit{totalTrips}$，即 $k\ge \left\lceil\dfrac{\textit{totalTrips}}{n}\right\rceil$。取 $x= \max(\textit{time})\cdot\left\lceil\dfrac{\textit{totalTrips}}{n}\right\rceil$，这个时间一定能满足题目要求。进一步地，可以与上面的 $\min(\textit{time}) \cdot \textit{totalTrips}$ 取最小值，作为开区间右端点初始值。

```py [sol-Python3]
class Solution:
    def minimumTime(self, time: List[int], totalTrips: int) -> int:
        min_t = min(time)
        left = min_t - 1  # 循环不变量：sum >= totalTrips 恒为 False
        right = min_t * totalTrips  # 循环不变量：sum >= totalTrips 恒为 True
        while left + 1 < right:  # 开区间 (left, right) 不为空
            mid = (left + right) // 2
            if sum(mid // t for t in time) >= totalTrips:
                right = mid  # 缩小二分区间为 (left, mid)
            else:
                left = mid  # 缩小二分区间为 (mid, right)
        # 此时 left 等于 right-1
        # sum(left) < totalTrips 且 sum(right) >= totalTrips，所以答案是 right
        return right
```

```py [sol-Python3 库函数]
class Solution:
    def minimumTime(self, time: List[int], totalTrips: int) -> int:
        check = lambda x: sum(x // t for t in time) >= totalTrips
        min_t = min(time)
        # bisect_left 需要用左闭右开区间
        left = min_t
        right = min_t * totalTrips
        return bisect_left(range(right), True, left, key=check)
```

```java [sol-Java]
class Solution {
    public long minimumTime(int[] time, int totalTrips) {
        int minT = Integer.MAX_VALUE;
        for (int t : time) {
            minT = Math.min(minT, t);
        }
        long left = minT - 1; // 循环不变量：check(left) 恒为 false
        long right = (long) minT * totalTrips; // 循环不变量：check(right) 恒为 true
        while (left + 1 < right) { // 开区间 (left, right) 不为空
            long mid = (left + right) >>> 1;
            if (check(mid, time, totalTrips)) {
                right = mid; // 缩小二分区间为 (left, mid)
            } else {
                left = mid; // 缩小二分区间为 (mid, right)
            }
        }
        // 此时 left 等于 right-1
        // check(left) = false 且 check(right) = true，所以答案是 right
        return right; // 最小的 true
    }

    private boolean check(long x, int[] time, int totalTrips) {
        long sum = 0;
        for (int t : time) {
            sum += x / t;
            if (sum >= totalTrips) {
                return true;
            }
        }
        return false;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minimumTime(vector<int>& time, int totalTrips) {
        auto check = [&](long long x) -> bool {
            long long sum = 0;
            for (int t : time) {
                sum += x / t;
                if (sum >= totalTrips) {
                    return true;
                }
            }
            return false;
        };

        int min_t = ranges::min(time);
        long long left = min_t - 1; // 循环不变量：check(left) 恒为 false
        long long right = 1LL * min_t * totalTrips; // 循环不变量：check(right) 恒为 true
        while (left + 1 < right) { // 开区间 (left, right) 不为空
            long long mid = left + (right - left) / 2;
            (check(mid) ? right : left) = mid;
        }
        // 此时 left 等于 right-1
        // check(left) = false 且 check(right) = true，所以答案是 right
        return right; // 最小的 true
    }
};
```

```c [sol-C]
#define MIN(a, b) ((b) < (a) ? (b) : (a))

long long minimumTime(int* time, int timeSize, int totalTrips) {
    bool check(long long x) {
        long long sum = 0;
        for (int i = 0; i < timeSize; i++) {
            sum += x / time[i];
            if (sum >= totalTrips) {
                return true;
            }
        }
        return false;
    }

    int min_t = INT_MAX;
    for (int i = 0; i < timeSize; i++) {
        min_t = MIN(min_t, time[i]);
    }
    long long left = min_t - 1; // 循环不变量：check(left) 恒为 false
    long long right = (long long) min_t * totalTrips; // 循环不变量：check(right) 恒为 true
    while (left + 1 < right) { // 开区间 (left, right) 不为空
        long long mid = left + (right - left) / 2;
        if (check(mid)) {
            right = mid; // 缩小二分区间为 (left, mid)
        } else {
            left = mid; // 缩小二分区间为 (mid, right)
        }
    }
    // 此时 left 等于 right-1
    // check(left) = false 且 check(right) = true，所以答案是 right
    return right; // 最小的 true
}
```

```go [sol-Go]
func minimumTime(time []int, totalTrips int) int64 {
    minT := slices.Min(time)
    left := minT - 1           // 循环不变量：check(left) 恒为 false
    right := minT * totalTrips // 循环不变量：check(right) 恒为 true
    for left+1 < right { // 开区间 (left, right) 不为空
        mid := left + (right-left)/2
        sum := 0
        for _, t := range time {
            sum += mid / t
        }
        if sum >= totalTrips {
            right = mid // 缩小二分区间为 (left, mid)
        } else {
            left = mid // 缩小二分区间为 (mid, right)
        }
    }
    // 此时 left 等于 right-1
    // check(left) = false 且 check(right) = true，所以答案是 right
    return int64(right) // 最小的 true
}
```

```go [sol-Go 库函数]
func minimumTime(time []int, totalTrips int) int64 {
    minT := slices.Min(time)
    left := minT
    right := minT * totalTrips
    return int64(left + sort.Search(right-left, func(x int) bool {
        x += left
        sum := 0
        for _, t := range time {
            sum += x / t
            if sum >= totalTrips {
                return true
            }
        }
        return false
    }))
}
```

```js [sol-JS]
var minimumTime = function(time, totalTrips) {
    const minT = Math.min(...time);
    let left = minT - 1; // 循环不变量：check(left) 恒为 false
    let right = minT * totalTrips; // 循环不变量：check(right) 恒为 true
    while (left + 1 < right) { // 开区间 (left, right) 不为空
        const mid = Math.floor((left + right) / 2);
        let sum = 0;
        for (const t of time) {
            sum += Math.floor(mid / t);
        }
        if (sum >= totalTrips) {
            right = mid; // 缩小二分区间为 (left, mid)
        } else {
            left = mid; // 缩小二分区间为 (mid, right)
        }
    }
    // 此时 left 等于 right-1
    // check(left) = false 且 check(right) = true，所以答案是 right
    return right; // 最小的 true
};
```

```rust [sol-Rust]
impl Solution {
    pub fn minimum_time(time: Vec<i32>, total_trips: i32) -> i64 {
        let total_trips = total_trips as i64;
        let min_t = *time.iter().min().unwrap() as i64;
        let mut left = min_t - 1; // 循环不变量：check(left) 恒为 false
        let mut right = min_t * total_trips; // 循环不变量：check(right) 恒为 true
        while left + 1 < right { // 开区间 (left, right) 不为空
            let mid = left + (right - left) / 2;
            let mut sum = 0;
            for &t in &time {
                sum += mid / t as i64;
            }
            if sum >= total_trips {
                right = mid; // 缩小二分区间为 (left, mid)
            } else {
                left = mid; // 缩小二分区间为 (mid, right)
            }
        }
        // 此时 left 等于 right-1
        // check(left) = false 且 check(right) = true，所以答案是 right
        right // 最小的 true
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log U)$，其中 $n$ 为 $\textit{time}$ 的长度，$U$ 为二分上下界之差。在本题数据范围下，$U\le 10^{14}$。
- 空间复杂度：$\mathcal{O}(1)$。

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

本题来自 `二、二分答案 / §2.1 求最小 / 答疑`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、二分答案 / §2.1 求最小 / 答疑`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
