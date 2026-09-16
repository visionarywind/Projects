# 1235. 规划兼职工作

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-profit-in-job-scheduling/
- 题目 slug：`maximum-profit-in-job-scheduling`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.2 进阶
- 难度分：2023
- 外部题解来源：https://leetcode.cn/problems/maximum-profit-in-job-scheduling/solutions/1913089/dong-tai-gui-hua-er-fen-cha-zhao-you-hua-zkcg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划 + 二分查找优化（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-profit-in-job-scheduling/solutions/1913089/dong-tai-gui-hua-er-fen-cha-zhao-you-hua-zkcg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-er-fen-cha-zhao-you-hua-zkcg`
- topic id：`1913089`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

### 提示 1

将工作按照结束时间排序，以示例 2 为例，得到下图：

![](https://pic.leetcode.cn/1666245025-aURzDO-20221020.drawio%20\(4\).png)

手动计算一下，按照结束时间排序后：

- 前 $1$ 个工作的最大报酬为 $20$；
- 前 $2$ 个工作的最大报酬为 $20$；
- 前 $3$ 个工作的最大报酬为前 $1$ 个工作的最大报酬 $+70=20+70=90$；
- 前 $4$ 个工作的最大报酬为前 $3$ 个工作的最大报酬 $+60=90+60=150$；
- 前 $5$ 个工作的最大报酬，如果选了第 $5$ 个工作，那么报酬为前 $1$ 个工作的最大报酬 $+100=20+100=120$；但也可以不选第 $5$ 个工作，报酬为前 $4$ 个工作的最大报酬，即 $150$。由于 $150>120$，不选第 $5$ 个工作更好，因此前 $5$ 个工作的最大报酬为 $150$。

示例 2 等价于计算前 $5$ 个工作的最大报酬，即 $150$。

### 提示 2

总结一下，我们可以分类讨论，求出按照结束时间排序后的**前** $i$ 个工作的最大报酬：

- 不选**第** $i$ 个工作，那么最大报酬等于**前** $i-1$ 个工作的最大报酬（转换成了一个规模更小的子问题）；
- 选**第** $i$ 个工作，由于工作时间不能重叠，设 $j$ 是最大的满足 $\textit{endTime}[j]\le\textit{startTime}[i]$ 的 $j$，那么最大报酬等于**前** $j$ 个工作的最大报酬加上 $\textit{profit}[i]$（同样转换成了一个规模更小的子问题）；
- 这两种决策取最大值。

注意，由于按照结束时间排序，**前** $j$ 个工作中的任意一个都不会与**第** $i$ 个工作的时间重叠。

### 提示 3

怎么实现？

上述思路是一个标准的关于**递推**的描述，定义 $f[i]$ 表示按照结束时间排序后的**前** $i$ 个工作的最大报酬，用「选或不选」分类讨论：

- 不选**第** $i$ 个工作：$f[i] = f[i-1]$；
- 选**第** $i$ 个工作：$f[i] = f[j]+\textit{profit}[i]$，其中 $j$ 是最大的满足 $\textit{endTime}[j]\le\textit{startTime}[i]$ 的 $j$，不存在时为 $-1$。

两者取最大值，即

$$
f[i] = \max(f[i-1], f[j]+\textit{profit}[i])
$$

由于 $i=0$ 时会产生 $-1$，可以在 $f$ 数组前面插入一个 $0$，与 $f$ 有关的下标都 $+1$，即

$$
f[i+1] = \max(f[i], f[j+1]+\textit{profit}[i])
$$

初始项 $f[0]=0$，答案为 $f[n]$。

代码实现时，由于结束时间是有序的，$j$ 可以用**二分查找**计算出来，原理请看[【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)

```py [sol-Python3]
class Solution:
    def jobScheduling(self, startTime: List[int], endTime: List[int], profit: List[int]) -> int:
        jobs = sorted(zip(endTime, startTime, profit))  # 按照结束时间排序
        f = [0] * (len(jobs) + 1)
        for i, (_, st, p) in enumerate(jobs):
            j = bisect_left(jobs, (st + 1,), hi=i)  # hi=i 表示二分上界为 i（默认为 n）
            # 状态转移中，为什么是 j 不是 j+1：上面算的是 > st，-1 后得到 <= st，但由于还要 +1，抵消了
            f[i + 1] = max(f[i], f[j] + p)
        return f[-1]
```

```java [sol-Java]
class Solution {
    public int jobScheduling(int[] startTime, int[] endTime, int[] profit) {
        int n = startTime.length;
        int[][] jobs = new int[n][];
        for (int i = 0; i < n; i++) {
            jobs[i] = new int[]{startTime[i], endTime[i], profit[i]};
        }
        Arrays.sort(jobs, (a, b) -> a[1] - b[1]); // 按照结束时间排序

        int[] f = new int[n + 1];
        for (int i = 0; i < n; i++) {
            int j = search(jobs, i, jobs[i][0]);
            f[i + 1] = Math.max(f[i], f[j + 1] + jobs[i][2]);
        }
        return f[n];
    }

    // 返回 endTime <= upper 的最大下标
    private int search(int[][] jobs, int right, int upper) {
        int left = -1;
        while (left + 1 < right) {
            int mid = (left + right) >>> 1;
            if (jobs[mid][1] <= upper) {
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
    int jobScheduling(vector<int> &startTime, vector<int> &endTime, vector<int> &profit) {
        int n = startTime.size();
        vector<array<int, 3>> jobs(n);
        for (int i = 0; i < n; i++) {
            jobs[i] = {endTime[i], startTime[i], profit[i]};
        }
        ranges::sort(jobs, [](auto &a, auto &b) { return a[0] < b[0]; }); // 按照结束时间排序

        vector<int> f(n + 1);
        for (int i = 0; i < n; i++) {
            int j = upper_bound(jobs.begin(), jobs.begin() + i, array<int, 3>{jobs[i][1], INT_MAX}) - jobs.begin();
            // 状态转移中，为什么是 j 不是 j+1：上面算的是 > 开始时间，-1 后得到 <= 开始时间，但由于还要 +1，抵消了
            f[i + 1] = max(f[i], f[j] + jobs[i][2]);
        }
        return f[n];
    }
};
```

```go [sol-Go]
func jobScheduling(startTime, endTime, profit []int) int {
    n := len(startTime)
    type job struct{ start, end, profit int }
    jobs := make([]job, n)
    for i, start := range startTime {
        jobs[i] = job{start, endTime[i], profit[i]}
    }
    slices.SortFunc(jobs, func(a, b job) int { return a.end - b.end }) // 按照结束时间排序

    f := make([]int, n+1)
    for i, job := range jobs {
        j := sort.Search(i, func(j int) bool { return jobs[j].end > job.start })
        // 状态转移中，为什么是 j 不是 j+1：上面算的是 > start，-1 后得到 <= start，但由于还要 +1，抵消了
        f[i+1] = max(f[i], f[j]+job.profit)
    }
    return f[n]
}
```

```js [sol-JavaScript]
var jobScheduling = function(startTime, endTime, profit) {
    const jobs = _.zip(startTime, endTime, profit);
    jobs.sort((a, b) => a[1] - b[1]); // 按照结束时间排序

    const n = jobs.length;
    const f = Array(n + 1);
    f[0] = 0;
    for (let i = 0; i < n; i++) {
        const j = search(jobs, i, jobs[i][0]);
        f[i + 1] = Math.max(f[i], f[j + 1] + jobs[i][2]);
    }
    return f[n];
}

// 返回 endTime <= upper 的最大下标
var search = function(jobs, right, upper) {
    let left = -1;
    while (left + 1 < right) {
        const mid = (left + right) >>> 1;
        if (jobs[mid][1] <= upper) {
            left = mid;
        } else {
            right = mid;
        }
    }
    return left;
}
```

```rust [sol-Rust]
impl Solution {
    pub fn job_scheduling(start_time: Vec<i32>, end_time: Vec<i32>, profit: Vec<i32>) -> i32 {
        let mut jobs = start_time.iter().zip(end_time.iter()).zip(profit.iter()).map(|((&s, &e), &p)| (s, e, p)).collect::<Vec<_>>();
        jobs.sort_unstable_by(|a, b| a.1.cmp(&b.1)); // 按照结束时间排序

        let n = jobs.len();
        let mut f = vec![0; n + 1];
        for (i, &(st, _, p)) in jobs.iter().enumerate() {
            let j = jobs[..i].partition_point(|job| job.1 <= st);
            // 状态转移中，为什么是 j 不是 j+1：上面算的是 > st，-1 后得到 <= st，但由于还要 +1，抵消了
            f[i + 1] = f[i].max(f[j] + p);
        }
        f[n]
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{startTime}$ 的长度。排序的时间复杂度为 $\mathcal{O}(n\log n)$，动态规划部分一共计算了 $n$ 次二分，时间复杂度为 $\mathcal{O}(n\log n)$，因此总的时间复杂度为 $\mathcal{O}(n\log n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 算法小课堂：标准库二分的灵活运用

在写二分题目时，经常会遇到形如「在有序数组中查询大于某个数的最小数」这类问题。具体来说有四类：

- $\ge$：在有序数组中查询大于或等于某个数的最小数；
- $>$：在有序数组中查询大于某个数的最小数；
- $\le$：在有序数组中查询小于或等于某个数的最大数；
- $<$：在有序数组中查询小于某个数的最大数。

上面的一些编程语言用到了标准库中的二分，但这些二分在设计的时候，只提供了查询 $\ge$ 和 $>$ 的功能，并没有提供查询 $\le$ 和 $<$ 的功能。

没有关系，稍微转换下就能解决。比如查询 $>$ 得到了下标 $i$，那么 $i-1$ 就是 $\le$ 的结果了（假设数组为升序），同理 $<$ 可以用 $\ge$ 算出来。

> 注：$>$ 和 $\ge$ 也可以转换，对于整数来说，$> x$ 等价于 $\ge x+1$。

## 相似题目

- [1751. 最多可以参加的会议数目 II](https://leetcode.cn/problems/maximum-number-of-events-that-can-be-attended-ii/)
- [2008. 出租车的最大盈利](https://leetcode.cn/problems/maximum-earnings-from-taxi/)

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `五、堆（优先队列） / §5.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
