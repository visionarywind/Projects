# 2462. 雇佣 K 位工人的总代价

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/total-cost-to-hire-k-workers/
- 题目 slug：`total-cost-to-hire-k-workers`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.1 基础
- 难度分：1764
- 外部题解来源：https://leetcode.cn/problems/total-cost-to-hire-k-workers/solutions/1951938/liang-ge-zui-xiao-dui-mo-ni-by-endlessch-nagm/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两个最小堆模拟（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/total-cost-to-hire-k-workers/solutions/1951938/liang-ge-zui-xiao-dui-mo-ni-by-endlessch-nagm/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-ge-zui-xiao-dui-mo-ni-by-endlessch-nagm`
- topic id：`1951938`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

雇佣过程可以用两个最小堆来模拟，一个负责维护 $\textit{costs}$ 剩余数字的最前面 $\textit{candidates}$ 个数的最小值，另一个负责维护 $\textit{costs}$ 剩余数字的最后面 $\textit{candidates}$ 个数的最小值。

具体算法如下：

1. 设 $\textit{cost}$ 的长度为 $n$。如果 $\textit{candidates}\cdot 2 + k > n$，我们一定可以选到 $\textit{cost}$ 中最小的 $k$ 个数，所以直接返回 $\textit{cost}$ 的前 $k$ 小之和。
2. 初始化答案 $\textit{ans}=0$。初始化最小堆 $\textit{pre}$ 为 $\textit{costs}$ 最前面的 $\textit{candidates}$ 个数，初始化最小堆 $\textit{suf}$ 为 $\textit{costs}$ 最后面的 $\textit{candidates}$ 个数。初始化下标 $i = \textit{candidates},\ j = n-1- \textit{candidates}$。
3. 循环 $k$ 次。每次循环，如果 $\textit{pre}$ 的堆顶小于等于 $\textit{suf}$ 的堆顶，则弹出 $\textit{pre}$ 的堆顶，加入答案，然后把 $\textit{costs}[i]$ 加入 $\textit{pre}$，$i$ 增加 $1$；如果 $\textit{suf}$ 的堆顶小于 $\textit{pre}$ 的堆顶，则弹出 $\textit{suf}$ 的堆顶，加入答案，然后把 $\textit{costs}[j]$ 加入 $\textit{suf}$，$j$ 减少 $1$。
4. 返回答案。

```py [sol-Python3]
class Solution:
    def totalCost(self, costs: List[int], k: int, candidates: int) -> int:
        n = len(costs)
        if candidates * 2 + k > n:
            # 也可以 sum(nsmallest(k, costs))，但效率不如直接排序
            costs.sort()
            return sum(costs[:k])

        pre = costs[:candidates]
        suf = costs[-candidates:]
        heapify(pre)
        heapify(suf)

        ans = 0
        i = candidates
        j = n - 1 - candidates
        for _ in range(k):
            if pre[0] <= suf[0]:
                ans += heapreplace(pre, costs[i])
                i += 1
            else:
                ans += heapreplace(suf, costs[j])
                j -= 1
        return ans
```

```java [sol-Java]
class Solution {
    public long totalCost(int[] costs, int k, int candidates) {
        int n = costs.length;
        long ans = 0;
        if (candidates * 2 + k > n) {
            Arrays.sort(costs);
            for (int i = 0; i < k; i++) {
                ans += costs[i];
            }
            return ans;
        }

        PriorityQueue<Integer> pre = new PriorityQueue<>();
        PriorityQueue<Integer> suf = new PriorityQueue<>();
        for (int i = 0; i < candidates; i++) {
            pre.offer(costs[i]);
            suf.offer(costs[n - 1 - i]);
        }

        int i = candidates;
        int j = n - 1 - candidates;
        while (k-- > 0) {
            if (pre.peek() <= suf.peek()) {
                ans += pre.poll();
                pre.offer(costs[i++]);
            } else {
                ans += suf.poll();
                suf.offer(costs[j--]);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long totalCost(vector<int>& costs, int k, int candidates) {
        int n = costs.size();
        if (candidates * 2 + k > n) {
            ranges::nth_element(costs, costs.begin() + k);
            return accumulate(costs.begin(), costs.begin() + k, 0LL);
        }

        priority_queue<int, vector<int>, greater<>> pre, suf;
        for (int i = 0; i < candidates; i++) {
            pre.push(costs[i]);
            suf.push(costs[n - 1 - i]);
        }

        long long ans = 0;
        int i = candidates, j = n - 1 - candidates;
        while (k--) {
            if (pre.top() <= suf.top()) {
                ans += pre.top();
                pre.pop();
                pre.push(costs[i++]);
            } else {
                ans += suf.top();
                suf.pop();
                suf.push(costs[j--]);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func totalCost(costs []int, k, candidates int) (ans int64) {
    n := len(costs)
    if candidates*2+k > n {
        slices.Sort(costs)
        for _, x := range costs[:k] {
            ans += int64(x)
        }
        return
    }

    pre := hp{costs[:candidates]}
    suf := hp{costs[len(costs)-candidates:]}
    heap.Init(&pre)
    heap.Init(&suf)
    for i, j := candidates, n-1-candidates; k > 0; k-- {
        if pre.IntSlice[0] <= suf.IntSlice[0] {
            ans += int64(pre.replace(costs[i]))
            i++
        } else {
            ans += int64(suf.replace(costs[j]))
            j--
        }
    }
    return
}

type hp struct{ sort.IntSlice }
func (hp) Push(any)             {}
func (hp) Pop() (_ any)         { return }
func (h *hp) replace(v int) int { top := h.IntSlice[0]; h.IntSlice[0] = v; heap.Fix(h, 0); return top }
```

```js [sol-JavaScript]
var totalCost = function(costs, k, candidates) {
    const n = costs.length;
    let ans = 0;
    if (candidates * 2 + k > n) {
        costs.sort((a, b) => a - b);
        for (let i = 0; i < k; i++) {
            ans += costs[i];
        }
        return ans;
    }

    const pre = new MinPriorityQueue();
    const suf = new MinPriorityQueue();
    for (let i = 0; i < candidates; i++) {
        pre.enqueue(costs[i]);
        suf.enqueue(costs[n - 1 - i]);
    }

    let i = candidates;
    let j = n - 1 - candidates;
    while (k--) {
        if (pre.front() <= suf.front()) {
            ans += pre.dequeue();
            pre.enqueue(costs[i++]);
        } else {
            ans += suf.dequeue();
            suf.enqueue(costs[j--]);
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn total_cost(mut costs: Vec<i32>, k: i32, candidates: i32) -> i64 {
        let n = costs.len();
        let k = k as usize;
        let c = candidates as usize;
        if c * 2 + k > n {
            costs.sort_unstable();
            return costs[..k].iter().map(|&x| x as i64).sum();
        }

        let mut pre = BinaryHeap::new();
        let mut suf = BinaryHeap::new();
        for i in 0..c {
            pre.push(-costs[i]); // 加负号，变成最小堆
            suf.push(-costs[n - 1 - i]);
        }

        let mut ans = 0;
        let mut i = c;
        let mut j = n - 1 - c;
        for _ in 0..k {
            if pre.peek().unwrap() >= suf.peek().unwrap() {
                ans -= pre.pop().unwrap() as i64;
                pre.push(-costs[i]);
                i += 1;
            } else {
                ans -= suf.pop().unwrap() as i64;
                suf.push(-costs[j]);
                j -= 1;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}((c+k)\log (c+k))$，其中 $c=\textit{candidate}$。
- 空间复杂度：$\mathcal{O}(c)$。

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

本题来自 `五、堆（优先队列） / §5.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
