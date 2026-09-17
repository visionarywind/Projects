# 1705. 吃苹果的最大数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-eaten-apples/
- 题目 slug：`maximum-number-of-eaten-apples`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.6 其他区间贪心
- 难度分：1930
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-eaten-apples/solutions/3011181/chi-zui-zao-fu-lan-de-ping-guo-pythonjav-hx5d/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[吃最早腐烂的苹果（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/maximum-number-of-eaten-apples/solutions/3011181/chi-zui-zao-fu-lan-de-ping-guo-pythonjav-hx5d/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`chi-zui-zao-fu-lan-de-ping-guo-pythonjav-hx5d`
- topic id：`3011181`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

**核心思路**：对于两个苹果 $A$ 和 $B$，设 $A$ 更早腐烂，那么应该先吃 $A$。如果先吃 $B$，可能下一天 $A$ 就烂了。

用最小堆维护苹果的腐烂日期和个数，模拟一天吃一个苹果。

注意第 $n$ 天之后还可以继续吃苹果，所以要一直模拟到堆为空为止。

## 优化前

```py [sol-Python3]
class Solution:
    def eatenApples(self, apples: List[int], days: List[int]) -> int:
        ans = i = 0
        h = []
        while i < len(apples) or h:
            while h and h[0][0] == i:  # 已腐烂
                heappop(h)
            if i < len(apples) and apples[i]:
                heappush(h, [i + days[i], apples[i]])
            if h:
                ans += 1
                h[0][1] -= 1  # 吃一个最早腐烂的苹果
                if h[0][1] == 0:
                    heappop(h)
            i += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int eatenApples(int[] apples, int[] days) {
        int ans = 0;
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> a[0] - b[0]);
        for (int i = 0; i < apples.length || !pq.isEmpty(); i++) {
            while (!pq.isEmpty() && pq.peek()[0] == i) { // 已腐烂
                pq.poll();
            }
            if (i < apples.length && apples[i] > 0) {
                pq.offer(new int[]{i + days[i], apples[i]});
            }
            if (!pq.isEmpty()) {
                // 吃一个最早腐烂的苹果
                ans++;
                if (--pq.peek()[1] == 0) {
                    pq.poll();
                }
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int eatenApples(vector<int>& apples, vector<int>& days) {
        int ans = 0;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        for (int i = 0; i < apples.size() || !pq.empty(); i++) {
            while (!pq.empty() && pq.top().first == i) { // 已腐烂
                pq.pop();
            }
            if (i < apples.size() && apples[i]) {
                pq.emplace(i + days[i], apples[i]);
            }
            if (!pq.empty()) {
                // 吃一个最早腐烂的苹果
                ans++;
                auto [rotten_day, num] = pq.top();
                pq.pop();
                if (num > 1) {
                    pq.emplace(rotten_day, num - 1);
                }
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func eatenApples(apples, days []int) (ans int) {
    h := hp{}
    for i := 0; i < len(apples) || h.Len() > 0; i++ {
        for len(h) > 0 && h[0].rottenDay == i { // 已腐烂
            heap.Pop(&h)
        }
        if i < len(apples) && apples[i] > 0 {
            heap.Push(&h, pair{i + days[i], apples[i]})
        }
        if len(h) > 0 {
            // 吃一个最早腐烂的苹果
            ans++
            h[0].num--
            if h[0].num == 0 {
                heap.Pop(&h)
            }
        }
    }
    return
}

type pair struct{ rottenDay, num int }
type hp []pair

func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].rottenDay < h[j].rottenDay }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(pair)) }
func (h *hp) Pop() any          { a := *h; v := a[len(a)-1]; *h = a[:len(a)-1]; return v }
```

```js [sol-JavaScript]
var eatenApples = function(apples, days) {
    const pq = new MinPriorityQueue(p => p[0]);
    let ans = 0;
    for (let i = 0; i < apples.length || !pq.isEmpty(); i++) {
        while (!pq.isEmpty() && pq.front()[0] === i) { // 已腐烂
            pq.dequeue();
        }
        if (i < apples.length && apples[i] > 0) {
            pq.enqueue([i + days[i], apples[i]]);
        }
        if (!pq.isEmpty()) {
            ans++;
            if (--pq.front()[1] === 0) {
                pq.dequeue();
            }
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn eaten_apples(apples: Vec<i32>, days: Vec<i32>) -> i32 {
        let mut h: BinaryHeap<(i32, i32)> = BinaryHeap::new();
        let mut ans = 0;
        let mut i = 0;
        while i < apples.len() || !h.is_empty() {
            while let Some(&(rotten_day, _)) = h.peek() {
                if -rotten_day > i as i32 {
                    break;
                }
                h.pop(); // 已腐烂
            }
            if i < apples.len() && apples[i] > 0 {
                h.push((-(i as i32 + days[i]), apples[i])); // 取反变最小堆
            }
            if let Some(mut p) = h.peek_mut() {
                // 吃一个最早腐烂的苹果
                ans += 1;
                p.1 -= 1;
            }
            if let Some(&(_, num)) = h.peek() {
                if num == 0 {
                    h.pop();
                }
            }
            i += 1;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(U + n\log n)$ 或 $\mathcal{O}((U + n)\log n)$，其中 $n$ 是 $\textit{apples}$ 的长度，$U = \max(\textit{days})$。虽然代码有个二重循环，但每个元素至多入堆出堆各一次，总的循环次数是 $\mathcal{O}(n+U)$ 的。其中 $\mathcal{O}(n)$ 次循环会执行入堆出堆操作，这部分的时间复杂度是 $\mathcal{O}(n\log n)$。部分语言无法直接修改堆顶，时间复杂度为 $\mathcal{O}((U + n)\log n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 优化

当 $i\ge n$ 时，我们浪费太多时间在「一个一个地」吃苹果上了。事实上，可以直接把最早腐烂的苹果（在腐烂前）一次性解决掉，也就是直接把 $i$ 跳到吃完（或者腐烂）的那一天。

设当前是第 $i$ 天，最早腐烂的苹果有 $\textit{num}$ 个，在第 $\textit{rottenDay}$ 天腐烂。

那么我们可以在腐烂前吃掉

$$
k = \min(\textit{num}, \textit{rottenDay} - i)
$$

个苹果。

吃完后，把答案和 $i$ 都增加 $k$，继续循环，直到堆为空为止。

```py [sol-Python3]
class Solution:
    def eatenApples(self, apples: List[int], days: List[int]) -> int:
        ans = 0
        h = []
        for i, (num, day) in enumerate(zip(apples, days)):
            while h and h[0][0] == i:  # 已腐烂
                heappop(h)
            if num:
                heappush(h, [i + day, num])
            if h:
                ans += 1
                h[0][1] -= 1  # 吃一个最早腐烂的苹果
                if h[0][1] == 0:
                    heappop(h)

        i = len(apples)
        while True:
            while h and h[0][0] <= i:  # 已腐烂
                heappop(h)
            if not h:
                return ans
            rotten_day, num = heappop(h)
            k = min(num, rotten_day - i)
            ans += k
            i += k
```

```java [sol-Java]
class Solution {
    public int eatenApples(int[] apples, int[] days) {
        int ans = 0;
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> a[0] - b[0]);
        int i = 0;
        for (; i < apples.length; i++) {
            while (!pq.isEmpty() && pq.peek()[0] == i) { // 已腐烂
                pq.poll();
            }
            if (apples[i] > 0) {
                pq.offer(new int[]{i + days[i], apples[i]});
            }
            if (!pq.isEmpty()) {
                // 吃一个最早腐烂的苹果
                ans++;
                if (--pq.peek()[1] == 0) {
                    pq.poll();
                }
            }
        }

        while (true) {
            while (!pq.isEmpty() && pq.peek()[0] <= i) { // 已腐烂
                pq.poll();
            }
            if (pq.isEmpty()) {
                return ans;
            }
            int[] top = pq.poll();
            int k = Math.min(top[1], top[0] - i);
            ans += k;
            i += k;
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int eatenApples(vector<int>& apples, vector<int>& days) {
        int ans = 0, i = 0;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        for (; i < apples.size(); i++) {
            while (!pq.empty() && pq.top().first == i) { // 已腐烂
                pq.pop();
            }
            if (apples[i]) {
                pq.emplace(i + days[i], apples[i]);
            }
            if (!pq.empty()) {
                // 吃一个最早腐烂的苹果
                ans++;
                auto [rotten_day, num] = pq.top();
                pq.pop();
                if (num > 1) {
                    pq.emplace(rotten_day, num - 1);
                }
            }
        }

        while (true) {
            while (!pq.empty() && pq.top().first <= i) { // 已腐烂
                pq.pop();
            }
            if (pq.empty()) {
                return ans;
            }
            auto [rotten_day, num] = pq.top();
            pq.pop();
            int k = min(num, rotten_day - i);
            ans += k;
            i += k;
        }
    }
};
```

```go [sol-Go]
func eatenApples(apples, days []int) (ans int) {
    h := hp{}
    for i, num := range apples {
        for len(h) > 0 && h[0].rottenDay == i { // 已腐烂
            heap.Pop(&h)
        }
        if num > 0 {
            heap.Push(&h, pair{i + days[i], num})
        }
        if len(h) > 0 {
            // 吃一个最早腐烂的苹果
            ans++
            h[0].num--
            if h[0].num == 0 {
                heap.Pop(&h)
            }
        }
    }

    i := len(apples)
    for {
        for len(h) > 0 && h[0].rottenDay <= i { // 已腐烂
            heap.Pop(&h)
        }
        if len(h) == 0 {
            return
        }
        p := heap.Pop(&h).(pair)
        k := min(p.num, p.rottenDay-i)
        ans += k
        i += k
    }
}

type pair struct{ rottenDay, num int }
type hp []pair

func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].rottenDay < h[j].rottenDay }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(pair)) }
func (h *hp) Pop() any          { a := *h; v := a[len(a)-1]; *h = a[:len(a)-1]; return v }
```

```js [sol-JavaScript]
var eatenApples = function(apples, days) {
    const pq = new MinPriorityQueue(p => p[0]);
    let ans = 0, i = 0;
    for (; i < apples.length; i++) {
        while (!pq.isEmpty() && pq.front()[0] === i) { // 已腐烂
            pq.dequeue();
        }
        if (apples[i] > 0) {
            pq.enqueue([i + days[i], apples[i]]);
        }
        if (!pq.isEmpty()) {
            ans++;
            if (--pq.front()[1] === 0) {
                pq.dequeue();
            }
        }
    }

    while (true) {
        while (!pq.isEmpty() && pq.front()[0] <= i) { // 已腐烂
            pq.dequeue();
        }
        if (pq.isEmpty()) {
            return ans;
        }
        const [rottenDay, num] = pq.dequeue();
        const k = Math.min(num, rottenDay - i);
        ans += k;
        i += k;
    }
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn eaten_apples(apples: Vec<i32>, days: Vec<i32>) -> i32 {
        let n = apples.len();
        let mut h: BinaryHeap<(i32, i32)> = BinaryHeap::new();
        let mut ans = 0;
        for (i, (num, day)) in apples.into_iter().zip(days).enumerate() {
            while let Some(&(rotten_day, _)) = h.peek() {
                if -rotten_day > i as i32 {
                    break;
                }
                h.pop(); // 已腐烂
            }
            if num > 0 {
                h.push((-(i as i32 + day), num)); // 取反变最小堆
            }
            if let Some(mut p) = h.peek_mut() {
                // 吃一个最早腐烂的苹果
                ans += 1;
                p.1 -= 1;
            }
            if let Some(&(_, num)) = h.peek() {
                if num == 0 {
                    h.pop();
                }
            }
        }

        let mut i = n as i32;
        loop {
            while let Some(&(rotten_day, _)) = h.peek() {
                if -rotten_day > i {
                    break;
                }
                h.pop(); // 已腐烂
            }
            if let Some((rotten_day, num)) = h.pop() {
                let k = num.min(-rotten_day - i);
                ans += k;
                i += k;
            } else {
                return ans;
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{apples}$ 的长度。注意内层的 $\texttt{while}$ 循环每循环一次，就会出堆一个元素。我们入堆 $\mathcal{O}(n)$ 次，那么出堆也 $\mathcal{O}(n)$ 次，所以总的循环次数是 $\mathcal{O}(n)$ 的。算上每次操作堆需要 $\mathcal{O}(\log n)$ 的时间，所以总的时间复杂度为 $\mathcal{O}(n\log n)$。
- 空间复杂度：$\mathcal{O}(n)$。

更多相似题目，见下面数据结构题单中的「**五、堆（优先队列）**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. 【本题相关】[常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `二、区间贪心 / §2.6 其他区间贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、区间贪心 / §2.6 其他区间贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
