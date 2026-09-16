# 857. 雇佣 K 名工人的最低成本

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-to-hire-k-workers/
- 题目 slug：`minimum-cost-to-hire-k-workers`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.2 进阶
- 难度分：2260
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-to-hire-k-workers/solutions/1815856/yi-bu-bu-ti-shi-ru-he-si-kao-ci-ti-by-en-1p00/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一步步提示如何思考此题（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-cost-to-hire-k-workers/solutions/1815856/yi-bu-bu-ti-shi-ru-he-si-kao-ci-ti-by-en-1p00/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yi-bu-bu-ti-shi-ru-he-si-kao-ci-ti-by-en-1p00`
- topic id：`1815856`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

### 提示 1

**定理**：在最优发工资方案下，至少有一名工人，发给他的工资恰好等于他的最低期望工资。

**证明**：反证法。假设不存在这样的工人，那么发给每名工人的工资都高于其最低期望工资，我们可以把每人的工资都下调，从而得到更优的方案，矛盾。

### 提示 2

枚举发了最低期望工资的那名工人，在满足题干中规则 1 的前提下，哪些工人可以满足规则 2？

如何快速地求出这些工人？

### 提示 3

定义 $r_i = \dfrac{\textit{wage}_i}{\textit{quality}_i}$，表示「每单位工作质量的工资」。

若以某人的 $r_i$ 为基准发工资，那么对于 $r$ 值不超过 $r_i$ 的工人，发给他们的工资是不低于其最低期望工资的，因此这些工人是可以随意选择（雇佣）的。

**按照 $r_i$ 从小到大排序**，我们便可以快速地求出这些工人。

要选哪 $k$ 名工人呢？

### 提示 4

设这 $k$ 名工人的 $\textit{quality}$ 之和为 $\textit{sumQ}$，若以 $r_i$ 为基准发工资，那么发的工资总额为 $\textit{sumQ}\cdot r_i$，因此 $\textit{sumQ}$ 越小发的工资总额就越小。

因此我们需要在从小到大枚举 $r_i$ 时，维护当前最小的 $k$ 个 $\textit{quality}$ 值。

如何高效地维护呢？

### 提示 5

用一个**最大堆**来维护。

按照 $r_i$ 从小到大的顺序遍历工人，当堆中有 $k$ 个元素时，如果 $\textit{quality}_i$ 比堆顶小，则可以弹出堆顶，将 $\textit{quality}_i$ 入堆，从而得到一个更小的 $\textit{sumQ}$，此时有可能找到一个更优解 $\textit{sumQ}\cdot r_i$，更新答案。

```py [sol-Python3]
class Solution:
    def mincostToHireWorkers(self, quality: List[int], wage: List[int], k: int) -> float:
        pairs = sorted(zip(quality, wage), key=lambda p: p[1] / p[0])  # 按照 r 值排序

        h = [q for q, _ in pairs[:k]]  # 最大堆
        heapify_max(h)

        sum_q = sum(h)
        ans = sum_q * pairs[k - 1][1] / pairs[k - 1][0]  # 选 r 值最小的 k 名工人
        for q, w in pairs[k:]:  # 后面的工人 r 值更大
            if q < h[0]:  # 但是 sum_q 可以变小，从而可能得到更优的答案
                sum_q -= heapreplace_max(h, q) - q  # 更新堆顶和 sum_q
                ans = min(ans, sum_q * w / q)
        return ans
```

```java [sol-Java]
class Solution {
    public double mincostToHireWorkers(int[] quality, int[] wage, int k) {
        int n = quality.length;
        Integer[] id = new Integer[n];
        Arrays.setAll(id, i -> i);
        // 按照 r 值排序
        Arrays.sort(id, (i, j) -> wage[i] * quality[j] - wage[j] * quality[i]);

        PriorityQueue<Integer> pq = new PriorityQueue<>((a, b) -> b - a);
        int sumQ = 0;
        for (int i = 0; i < k; i++) {
            pq.offer(quality[id[i]]);
            sumQ += quality[id[i]];
        }

        // 选 r 值最小的 k 名工人
        double ans = sumQ * ((double) wage[id[k - 1]] / quality[id[k - 1]]);

        // 后面的工人 r 值更大
        // 但是 sumQ 可以变小，从而可能得到更优的答案
        for (int i = k; i < n; i++) {
            int q = quality[id[i]];
            if (q < pq.peek()) {
                sumQ -= pq.poll() - q;
                pq.offer(q);
                ans = Math.min(ans, sumQ * ((double) wage[id[i]] / q));
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    double mincostToHireWorkers(vector<int> &quality, vector<int> &wage, int k) {
        int n = quality.size(), sum_q = 0;
        vector<int> id(n);
        iota(id.begin(), id.end(), 0);
        ranges::sort(id, [&](int i, int j) { return wage[i] * quality[j] < wage[j] * quality[i]; }); // 按照 r 值排序
        priority_queue<int> pq;
        for (int i = 0; i < k; i++) {
            pq.push(quality[id[i]]);
            sum_q += quality[id[i]];
        }
        double ans = sum_q * ((double) wage[id[k - 1]] / quality[id[k - 1]]); // 选 r 值最小的 k 名工人
        for (int i = k; i < n; i++) { // 后面的工人 r 值更大
            int q = quality[id[i]];
            if (q < pq.top()) { // 但是 sum_q 可以变小，从而可能得到更优的答案
                sum_q -= pq.top() - q;
                pq.pop();
                pq.push(q);
                ans = min(ans, sum_q * ((double) wage[id[i]] / q));
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func mincostToHireWorkers(quality, wage []int, k int) float64 {
    type pair struct{ q, w int }
    pairs := make([]pair, len(quality))
    for i, q := range quality {
        pairs[i] = pair{q, wage[i]}
    }
    slices.SortFunc(pairs, func(a, b pair) int { return a.w*b.q - b.w*a.q }) // 按照 r 值排序

    h := hp{make([]int, k)}
    sumQ := 0
    for i, p := range pairs[:k] {
        h.IntSlice[i] = p.q
        sumQ += p.q
    }
    heap.Init(&h)

    ans := float64(sumQ*pairs[k-1].w) / float64(pairs[k-1].q) // 选 r 值最小的 k 名工人

    for _, p := range pairs[k:] { // 后面的工人 r 值更大
        if p.q < h.IntSlice[0] { // 但是 sumQ 可以变小，从而可能得到更优的答案
            sumQ -= h.IntSlice[0] - p.q
            h.IntSlice[0] = p.q
            heap.Fix(&h, 0) // 更新堆顶
            ans = min(ans, float64(sumQ*p.w)/float64(p.q))
        }
    }
    return ans
}

type hp struct{ sort.IntSlice }
func (h hp) Less(i, j int) bool { return h.IntSlice[i] > h.IntSlice[j] } // 最大堆
func (hp) Push(any)             {}
func (hp) Pop() (_ any)         { return }
```

```js [sol-JavaScript]
var mincostToHireWorkers = function (quality, wage, k) {
    const n = quality.length;
    const id = [...Array(n).keys()]
    // 按照 r 值排序
    id.sort((i, j) => wage[i] * quality[j] - wage[j] * quality[i]);

    const pq = new MaxPriorityQueue();
    let sumQ = 0;
    for (let i = 0; i < k; i++) {
        pq.enqueue(quality[id[i]]);
        sumQ += quality[id[i]];
    }

    // 选 r 值最小的 k 名工人
    let ans = sumQ * wage[id[k - 1]] / quality[id[k - 1]];

    // 后面的工人 r 值更大
    // 但是 sumQ 可以变小，从而可能得到更优的答案
    for (let i = k; i < n; i++) {
        const q = quality[id[i]];
        if (q < pq.front()) {
            sumQ -= pq.dequeue() - q;
            pq.enqueue(q);
            ans = Math.min(ans, sumQ * wage[id[i]] / q);
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn mincost_to_hire_workers(quality: Vec<i32>, wage: Vec<i32>, k: i32) -> f64 {
        let n = quality.len();
        let k = k as usize;
        let mut id = (0..n).collect::<Vec<_>>();
        // 按照 r 值排序
        id.sort_unstable_by(|&i, &j| (wage[i] * quality[j]).cmp(&(wage[j] * quality[i])));

        let mut h = BinaryHeap::new();
        let mut sum_q = 0;
        for i in 0..k {
            h.push(quality[id[i]]);
            sum_q += quality[id[i]];
        }

        // 选 r 值最小的 k 名工人
        let mut ans = sum_q as f64 * wage[id[k - 1]] as f64 / quality[id[k - 1]] as f64;

        // 后面的工人 r 值更大
        // 但是 sum_q 可以变小，从而可能得到更优的答案
        for i in k..n {
            let q = quality[id[i]];
            if q < *h.peek().unwrap() {
                sum_q -= h.pop().unwrap() - q;
                h.push(q);
                ans = ans.min(sum_q as f64 * wage[id[i]] as f64 / q as f64);
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{quality}$ 的长度。排序的时间复杂度为 $\mathcal{O}(n\log n)$，后面和堆有关的时间复杂度为 $\mathcal{O}(n\log k)$，由于 $k\le n$，总的时间复杂度为 $\mathcal{O}(n\log n)$。
- 空间复杂度：$\mathcal{O}(n)$。由于 $k\le n$，空间复杂度主要取决于 $n$。

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
