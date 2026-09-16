# 1553. 吃掉 N 个橘子的最少天数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-number-of-days-to-eat-n-oranges/
- 题目 slug：`minimum-number-of-days-to-eat-n-oranges`
- 来源专题：动态规划
- 来源分类路径：其他
- 难度分：2048
- 外部题解来源：https://leetcode.cn/problems/minimum-number-of-days-to-eat-n-oranges/solutions/2773476/liang-chong-fang-fa-ji-yi-hua-sou-suo-zu-18jv/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：记忆化搜索/最短路，附证明（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/minimum-number-of-days-to-eat-n-oranges/solutions/2773476/liang-chong-fang-fa-ji-yi-hua-sou-suo-zu-18jv/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-chong-fang-fa-ji-yi-hua-sou-suo-zu-18jv`
- topic id：`2773476`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 题意解读

有三种操作，每次操作，选择其中一种：

- 把 $n$ 减少 $1$。
- 如果 $n$ 能被 $2$ 整除，把 $n$ 变成 $\dfrac{n}{2}$。
- 如果 $n$ 能被 $3$ 整除，把 $n$ 变成 $\dfrac{n}{3}$。

返回把 $n$ 变成 $0$ 的最少操作次数。

## 分析

一个初步的想法是，为了让 $n$ 快速地变小，多做除法比多做减法要更好。

**引理**：在最优操作序列中，不存在「减 $1$，减 $1$，再除以 $2$」这样的操作。

**证明**：反证法。设当前剩下 $m$ 个橘子，先执行两次减 $1$，再除以 $2$，我们得到 $\dfrac{m-1-1}{2} = \dfrac{m}{2}-1$（注意这意味着 $m$ 是偶数）。但如果先除以 $2$，再减 $1$，同样可以得到 $\dfrac{m}{2}-1$，少操作一次，矛盾。故原命题成立。

**定理**：在最优操作序列中，如果第一个不是「减 $1$」的操作是「除以 $2$」，那么当 $n$ 是偶数时，第一个操作一定是「除以 $2$」，当 $n$ 是奇数时，前两个操作一定是「减 $1$，除以 $2$」。

**证明**：反证法。在第一个不是「减 $1$」的操作是「除以 $2$」的前提下：

- 如果 $n$ 是偶数，且第一个操作不执行「除以 $2$」，那么要连续执行至少两次「减 $1$」操作才能「除以 $2$」，与引理不符。
- 如果 $n$ 是奇数，且第二个操作不执行「除以 $2$」，那么要连续执行至少三次「减 $1$」操作才能「除以 $2$」，与引理不符。

同理可证明，在最优操作序列中，不存在「减 $1$，减 $1$，减 $1$，再除以 $3$」这样的操作序列。

同理可证明，在最优操作序列中，如果第一个不是「减 $1$」的操作是「除以 $3$」，那么一定会先执行 $n\bmod 3$ 次「减 $1$」操作，把 $n$ 变成 $3$ 的倍数，然后再执行「除以 $3$」。

- 如果把 $n$ 通过减一除以 $2$ 变成 $\left\lfloor\dfrac{n}{2}\right\rfloor$，那么接下来要解决的问题是：把 $\left\lfloor\dfrac{n}{2}\right\rfloor$ 变成 $0$ 的最少操作次数。
- 如果把 $n$ 通过减一除以 $3$ 变成 $\left\lfloor\dfrac{n}{3}\right\rfloor$，那么接下来要解决的问题是：把 $\left\lfloor\dfrac{n}{3}\right\rfloor$ 变成 $0$ 的最少操作次数。
- 这些都是**和原问题相似的子问题**，分析和处理方法同上。

上述结论引出了以下两种方法：

- DFS 思路：记忆化搜索。 
- BFS 思路：Dijkstra 算法求最短路。

## 方法一：记忆化搜索

根据上面找到的子问题，定义 $\textit{dfs}(i)$ 表示把 $i$ 变成 $0$ 的最少操作次数。

分类讨论：

- 先执行 $i\bmod 2$ 次减 $1$ 操作，把 $i$ 变成 $2$ 的倍数，然后再执行一次除以 $2$，问题变成把 $\left\lfloor i/2 \right\rfloor$ 变成 $0$ 的最小操作次数，即 $\textit{dfs}(i) = \textit{dfs}(\left\lfloor i/2 \right\rfloor)+i\bmod 2 + 1$。
- 先执行 $i\bmod 3$ 次减 $1$ 操作，把 $i$ 变成 $3$ 的倍数，然后再执行一次除以 $3$，问题变成把 $\left\lfloor i/3 \right\rfloor$ 变成 $0$ 的最小操作次数，即 $\textit{dfs}(i) = \textit{dfs}(\left\lfloor i/3 \right\rfloor)+i\bmod 3 + 1$。

二者取最小值，得

$$
\textit{dfs}(i) = \min(\textit{dfs}(\left\lfloor i/2 \right\rfloor)+i\bmod 2,\ \textit{dfs}(\left\lfloor i/3 \right\rfloor)+i\bmod 3) + 1
$$

递归边界：$\textit{dfs}(0)=0,\ \textit{dfs}(1)=1$。

递归入口：$\textit{dfs}(n)$，即答案。

代码实现时，可以用**记忆化搜索**：用一个哈希表 $\textit{memo}$ 记录 $i$ 及其 $\textit{dfs}(i)$，再次递归调用 $\textit{dfs}(i)$ 时，就可以直接返回 $\textit{memo}$ 中保存的数据了。Python 用户可以直接使用 `@cache`。

```py [sol-Python3]
class Solution:
    def minDays(self, n: int) -> int:
        @cache  # 缓存装饰器，避免重复计算 dfs 的结果（记忆化）
        def dfs(i: int) -> int:
            if i <= 1:
                return i
            return min(dfs(i // 2) + i % 2, dfs(i // 3) + i % 3) + 1
        return dfs(n)
```

```py [sol-Python3 写法二]
class Solution:
    @cache
    def minDays(self, n: int) -> int:
        if n <= 1:
            return n
        return min(self.minDays(n // 2) + n % 2,
                   self.minDays(n // 3) + n % 3) + 1
```

```java [sol-Java]
class Solution {
    private final Map<Integer, Integer> memo = new HashMap<>();

    public int minDays(int n) {
        if (n <= 1) {
            return n;
        }
        Integer res = memo.get(n);
        if (res != null) { // 之前计算过
            return res;
        }
        res = Math.min(minDays(n / 2) + n % 2, minDays(n / 3) + n % 3) + 1;
        memo.put(n, res); // 记忆化
        return res;
    }
}
```

```cpp [sol-C++]
class Solution {
    unordered_map<int, int> memo;
public:
    int minDays(int n) {
        if (n <= 1) {
            return n;
        }
        if (memo.contains(n)) { // 之前计算过
            return memo[n];
        }
        return memo[n] = min(minDays(n / 2) + n % 2,
                             minDays(n / 3) + n % 3) + 1;
    }
};
```

```go [sol-Go]
func minDays(n int) int {
    memo := map[int]int{}
    var dfs func(int) int
    dfs = func(i int) int {
        if i <= 1 {
            return i
        }
        if v, ok := memo[i]; ok { // 之前计算过
            return v
        }
        res := min(dfs(i/2)+i%2, dfs(i/3)+i%3) + 1
        memo[i] = res // 记忆化
        return res
    }
    return dfs(n)
}
```

```js [sol-JavaScript]
var minDays = function(n) {
    const memo = new Map();
    function dfs(i) {
        if (i <= 1) {
            return i;
        }
        if (memo.has(i)) { // 之前计算过
            return memo.get(i);
        }
        const res = Math.min(dfs(Math.floor(i / 2)) + i % 2, dfs(Math.floor(i / 3)) + i % 3) + 1;
        memo.set(i, res); // 记忆化
        return res;
    }
    return dfs(n);
};
```

```rust [sol-Rust]
use std::collections::HashMap;

impl Solution {
    pub fn min_days(n: i32) -> i32 {
        fn dfs(i: i32, memo: &mut HashMap<i32, i32>) -> i32 {
            if i <= 1 {
                return i;
            }
            if let Some(&v) = memo.get(&i) { // 之前计算过
                return v;
            }
            let res = (dfs(i / 2, memo) + i % 2).min(dfs(i / 3, memo) + i % 3) + 1;
            memo.insert(i, res); // 记忆化
            res
        }
        let mut memo = HashMap::new();
        dfs(n, &mut memo)
    }
}
```

#### 复杂度分析

有多少个状态？怎么计算？

我们要用到如下恒等式，其中 $n,p,q$ 均为正整数。

$$
\left\lfloor\dfrac{\left\lfloor n/p \right\rfloor}{q}\right\rfloor = \left\lfloor\dfrac{n}{pq}\right\rfloor
$$

例如 $\left\lfloor\dfrac{\left\lfloor 15/2 \right\rfloor}{3}\right\rfloor = \left\lfloor\dfrac{15}{2\cdot 3}\right\rfloor = 2$。

证明见 [下取整恒等式及其应用](https://zhuanlan.zhihu.com/p/1893240318645732760)。

由该恒等式可得，$i$ 和 $n$ 有如下关系

$$
i = \left\lfloor\dfrac{n}{2^x 3^y}\right\rfloor
$$

其中 $x$ 和 $y$ 均为整数且 $0\le x\le \left\lfloor \log_2 n\right\rfloor + 1,\ 0\le y\le \left\lfloor \log_3 n\right\rfloor+1$。

所以有 $\mathcal{O}(\log n)$ 个不同的 $x$ 和 $\mathcal{O}(\log n)$ 个不同的 $y$。两两组合，一共有 $\mathcal{O}(\log^2 n)$ 个不同的 $i$，即状态个数。

- 时间复杂度：$\mathcal{O}(\log^2 n)$。由于每个状态只会计算一次，动态规划的时间复杂度 $=$ 状态个数 $\times$ 单个状态的计算时间。本题状态个数等于 $\mathcal{O}(\log^2 n)$，单个状态的计算时间为 $\mathcal{O}(1)$，所以动态规划的时间复杂度为 $\mathcal{O}(\log^2 n)$。
- 空间复杂度：$\mathcal{O}(\log^2 n)$。有多少个状态，$\textit{memo}$ 哈希表的大小就是多少。

## 方法二：Dijkstra 算法求最短路

也可以这样建图：

- $x$ 到 $\left\lfloor x/2 \right\rfloor$ 连一条边权为 $x\bmod 2 + 1$ 的边。
- $x$ 到 $\left\lfloor x/3 \right\rfloor$ 连一条边权为 $x\bmod 3 + 1$ 的边。
- $1$ 到 $0$ 连一条边权为 $1$ 的边。

答案为 $n$ 到 $0$ 的最短路，用 **Dijkstra 算法**计算。

原理见 [Dijkstra 算法介绍](https://leetcode.cn/problems/network-delay-time/solution/liang-chong-dijkstra-xie-fa-fu-ti-dan-py-ooe8/)。

代码实现时，无需建图，根据出堆的数字 $x$ 计算出对应的邻居和边权。

此外，如果 $x\le 1$，可以直接返回答案。

```py [sol-Python3]
class Solution:
    def minDays(self, n: int) -> int:
        dis = defaultdict(lambda: inf)
        h = [(0, n)]
        while True:
            dx, x = heappop(h)
            if x <= 1:
                return dx + x
            if dx > dis[x]:
                continue
            for d in 2, 3:
                y = x // d
                dy = dx + x % d + 1
                if dy < dis[y]:
                    dis[y] = dy
                    heappush(h, (dy, y))
```

```java [sol-Java]
class Solution {
    public int minDays(int n) {
        Map<Integer, Integer> dis = new HashMap<>();
        dis.put(n, 0);
        PriorityQueue<int[]> pq = new PriorityQueue<>((a, b) -> a[0] - b[0]);
        pq.offer(new int[]{0, n});
        while (true) {
            int[] p = pq.poll();
            int dx = p[0];
            int x = p[1];
            if (x <= 1) {
                return dx + x;
            }
            if (dx > dis.get(x)) {
                continue;
            }
            for (int d = 2; d <= 3; d++) {
                int y = x / d;
                int dy = dx + x % d + 1;
                if (dy < dis.getOrDefault(y, Integer.MAX_VALUE)) {
                    dis.put(y, dy);
                    pq.offer(new int[]{dy, y});
                }
            }
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int minDays(int n) {
        unordered_map<int, int> dis;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        pq.emplace(0, n);
        while (true) {
            auto [dx, x] = pq.top();
            pq.pop();
            if (x <= 1) {
                return dx + x;
            }
            if (dx > dis[x]) {
                continue;
            }
            for (int d = 2; d <= 3; d++) {
                int y = x / d;
                int dy = dx + x % d + 1;
                if (!dis.contains(y) || dy < dis[y]) {
                    dis[y] = dy;
                    pq.emplace(dy, y);
                }
            }
        }
    }
};
```

```go [sol-Go]
func minDays(n int) int {
    dis := map[int]int{}
    h := &hp{{0, n}}
    for {
        p := heap.Pop(h).(pair)
        dx, x := p.d, p.x
        if x <= 1 {
            return dx + x
        }
        if dx > dis[x] {
            continue
        }
        for d := 2; d <= 3; d++ {
            y := x / d
            dy := dx + x%d + 1
            if dis[y] == 0 || dy < dis[y] {
                dis[y] = dy
                heap.Push(h, pair{dy, y})
            }
        }
    }
}

type pair struct{ d, x int }
type hp []pair
func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].d < h[j].d }
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(pair)) }
func (h *hp) Pop() any          { a := *h; v := a[len(a)-1]; *h = a[:len(a)-1]; return v }
```

```js [sol-JavaScript]
var minDays = function(n) {
    const dis = new Map();
    const pq = new MinPriorityQueue(p => p[0]);
    dis.set(n, 0);
    pq.enqueue([0, n]);
    while (true) {
        const [dx, x] = pq.dequeue();
        if (x <= 1) {
            return dx + x;
        }
        if (dx > dis.get(x)) {
            continue;
        }
        for (let d = 2; d <= 3; d++) {
            const y = Math.floor(x / d);
            const dy = dx + x % d + 1;
            if (dy < (dis.get(y) ?? Infinity)) {
                dis.set(y, dy);
                pq.enqueue([dy, y]);
            }
        }
    }
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;
use std::collections::HashMap;

impl Solution {
    pub fn min_days(n: i32) -> i32 {
        let mut dis = HashMap::new();
        dis.insert(n, 0);
        let mut h = BinaryHeap::new();
        h.push((0, n));
        loop {
            let (dx, x) = h.pop().unwrap();
            let dx = -dx;
            if x <= 1 {
                return dx + x;
            }
            if dx > *dis.get(&x).unwrap() {
                continue;
            }
            for d in [2, 3] {
                let y = x / d;
                let dy = dx + x % d + 1;
                if dy < *dis.get(&y).unwrap_or(&i32::MAX) {
                    dis.insert(y, dy);
                    h.push((-dy, y)); // 取负号变最小堆
                }
            }
        }
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(\log^2 n\cdot \log\log n)$。有 $\mathcal{O}(\log^2 n)$ 个节点（理由同方法一的复杂度分析），以及 $\mathcal{O}(\log^2 n)$ 条边（每个节点连出去两条边），每次操作堆的时间复杂度为 $\mathcal{O}(\log\log n)$，所以 Dijkstra 算法的时间复杂度为 $\mathcal{O}(\log^2 n\cdot \log\log n)$。
- 空间复杂度：$\mathcal{O}(\log^2 n)$。

## 专题训练

见动态规划题单的「**专题：把 X 变成 Y**」。

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

本题来自 `其他`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `其他`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
