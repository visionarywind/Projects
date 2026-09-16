# 630. 课程表 III

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/course-schedule-iii/
- 题目 slug：`course-schedule-iii`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.5 反悔堆
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/course-schedule-iii/solutions/2436667/tan-xin-huan-neng-fan-hui-pythonjavacgoj-lcwp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心还能反悔？（Python/Java/C++/Go/JS）](https://leetcode.cn/problems/course-schedule-iii/solutions/2436667/tan-xin-huan-neng-fan-hui-pythonjavacgoj-lcwp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-huan-neng-fan-hui-pythonjavacgoj-lcwp`
- topic id：`2436667`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

先说为什么不考虑动态规划。比如前 $3$ 天上 $\textit{duration}=3$ 的课程，那么问题就变成从第 $4$ 天开始上课，至多可以上多少课程。这个思路是没问题，但有亿点点慢。比如在 $\textit{duration}$ 都比较小，而 $\textit{lastDay}$ 都比较大的情况下，上完一门课后，考虑「枚举选哪个」作为接下来要上的课程，我们每次都有 $\mathcal{O}(n)$ 个课程可以选，那么总体就需要 $\mathcal{O}(n^2)$ 的时间。在本题的数据范围下，这有可能会超时。

有没有更快的做法呢？来试试贪心行不行。

经验告诉我们，在准备期末考试的时候，先考的课程先准备。同理，$\textit{lastDay}$ 越早的课程，应当越早上完。但是，有的课程 $\textit{duration}$ 比较长，上完需要花很多时间，可能把这些时间花在其它课程，早就上完好几门课了。

看上去，找不到一个合适的贪心策略。别放弃！顺着这个思路，如果我们可以「反悔」呢？

按照 $\textit{lastDay}$ 从小到大排序，然后遍历 $\textit{courses}$。比如先上完 $\textit{duration}=7$ 的课和 $\textit{duration}=10$ 的课，后面遍历到了 $\textit{duration}=4$ 的课，但受到 $\textit{lastDay}$ 的限制，无法上 $\textit{duration}=4$ 的课。此时，我们可以「撤销」前面 $\textit{duration}$ 最长的课，也就是 $\textit{duration}=10$ 的课，这样就可以上 $\textit{duration}=4$ 的课了！虽然能上完的课程数目没有变化，但是由于我们多出了 $10-4=6$ 天时间，**在后续的遍历中，更有机会上完更多的课程**。

在上面的讨论中，我们需要维护一个数据结构，来帮助我们快速找到 $\textit{duration}$ 最长的课程。这可以用**最大堆**解决。

```py [sol-Python3]
class Solution:
    def scheduleCourse(self, courses: List[List[int]]) -> int:
        courses.sort(key=lambda c: c[1])  # 按照 last_day 从小到大排序
        h = []  # 最大堆
        day = 0  # 已消耗时间
        for duration, last_day in courses:
            if day + duration <= last_day:  # 没有超过 last_day，直接学习
                day += duration
                heappush_max(h, duration)
            elif h and duration < h[0]:  # 该课程的时间比之前的最长时间要短
                # 反悔，撤销之前 duration 最长的课程，改为学习该课程
                # 节省出来的时间，能在后面上完更多的课程
                day -= heapreplace_max(h, duration) - duration
        return len(h)
```

```java [sol-Java]
class Solution {
    public int scheduleCourse(int[][] courses) {
        Arrays.sort(courses, (a, b) -> a[1] - b[1]); // 按照 lastDay 从小到大排序
        PriorityQueue<Integer> pq = new PriorityQueue<>((a, b) -> b - a); // 最大堆
        int day = 0; // 已消耗时间
        for (int[] c : courses) {
            int duration = c[0];
            int lastDay = c[1];
            if (day + duration <= lastDay) { // 没有超过 lastDay，直接学习
                day += duration;
                pq.offer(duration);
            } else if (!pq.isEmpty() && duration < pq.peek()) { // 该课程的时间比之前的最长时间要短
                // 反悔，撤销之前 duration 最长的课程，改为学习该课程
                // 节省出来的时间，能在后面上完更多的课程
                day -= pq.poll() - duration;
                pq.offer(duration);
            }
        }
        return pq.size();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int scheduleCourse(vector<vector<int>>& courses) {
        ranges::sort(courses, [](const auto& a, const auto& b) {
            return a[1] < b[1]; // 按照 last_day 从小到大排序
        });
        priority_queue<int> pq; // 最大堆
        int day = 0; // 已消耗时间
        for (auto& c: courses) {
            int duration = c[0], last_day = c[1];
            if (day + duration <= last_day) { // 没有超过 last_day，直接学习
                day += duration;
                pq.push(duration);
            } else if (!pq.empty() && duration < pq.top()) { // 该课程的时间比之前的最长时间要短
                // 反悔，撤销之前 duration 最长的课程，改为学习该课程
                // 节省出来的时间，能在后面上完更多的课程
                day -= pq.top() - duration;
                pq.pop();
                pq.push(duration);
            }
        }
        return pq.size();
    }
};
```

```go [sol-Go]
func scheduleCourse(courses [][]int) int {
    slices.SortFunc(courses, func(a, b []int) int { return a[1] - b[1] })
    h := hp{}
    day := 0 // 已消耗时间
    for _, c := range courses {
        d := c[0]
        if day+d <= c[1] { // 没有超过 lastDay，直接学习
            day += d
            heap.Push(&h, d)
        } else if h.Len() > 0 && d < h.IntSlice[0] { // 该课程的时间比之前的最长时间要短
            day -= h.replace(d) - d // 反悔，撤销之前 duration 最长的课程，改为学习该课程
            // 节省出来的时间，能在后面上完更多的课程
        }
    }
    return h.Len()
}

type hp struct{ sort.IntSlice }
func (h hp) Less(i, j int) bool { return h.IntSlice[i] > h.IntSlice[j] } // 最大堆
func (h *hp) Push(v any)        { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() any          { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
func (h *hp) replace(v int) int { top := h.IntSlice[0]; h.IntSlice[0] = v; heap.Fix(h, 0); return top }
```

```js [sol-JavaScript]
var scheduleCourse = function(courses) {
    courses.sort((a, b) => a[1] - b[1]); // 按照 lastDay 从小到大排序
    const pq = new MaxPriorityQueue();
    let day = 0; // 已消耗时间
    for (const [duration, lastDay] of courses) {
        if (day + duration <= lastDay) { // 没有超过 lastDay，直接学习
            day += duration;
            pq.enqueue(duration);
        } else if (!pq.isEmpty() && duration < pq.front()) { // 该课程的时间比之前的最长时间要短
            // 反悔，撤销之前 duration 最长的课程，改为学习该课程
            // 节省出来的时间，能在后面上完更多的课程
            day -= pq.dequeue() - duration;
            pq.enqueue(duration);
        }
    }
    return pq.size();
};
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{courses}$ 的长度。排序需要 $\mathcal{O}(n\log n)$ 的时间；遍历 $\textit{courses}$ 时，每次操作堆都需要 $\mathcal{O}(\log n)$ 的时间。总的时间复杂度为 $\mathcal{O}(n\log n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面贪心题单的「**§1.9 反悔贪心**」。

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

本题来自 `五、堆（优先队列） / §5.5 反悔堆`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.5 反悔堆`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
