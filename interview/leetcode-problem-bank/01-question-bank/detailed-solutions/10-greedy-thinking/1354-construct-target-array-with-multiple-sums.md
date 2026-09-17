# 1354. 多次求和构造目标数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/construct-target-array-with-multiple-sums/
- 题目 slug：`construct-target-array-with-multiple-sums`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.4 逆向思维
- 难度分：2015
- 外部题解来源：https://leetcode.cn/problems/construct-target-array-with-multiple-sums/solutions/3832109/ni-xiang-si-wei-pythonjavacgojsrust-by-e-0701/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[逆向思维（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/construct-target-array-with-multiple-sums/solutions/3832109/ni-xiang-si-wei-pythonjavacgojsrust-by-e-0701/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ni-xiang-si-wei-pythonjavacgojsrust-by-e-0701`
- topic id：`3832109`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

## 思路

倒着想，最后一步发生了什么？

由于每次操作得到的元素和会越来越大，所以 $\textit{target}$ 的最大值就是最后一步操作的元素和。

示例 1 的 $\textit{target}=[9,3,5]$。

- 最后一步，最大值为 $9$，说明操作之前所有元素和等于 $9$。也就是操作之前 $\textit{target}[0]+3+5 = 9$，解得操作之前 $\textit{target}[0] = 1$。
- 现在 $\textit{target}=[1,3,5]$，继续倒推，操作之前 $\textit{target}[2] +1+3 = 5$，解得操作之前 $\textit{target}[2] = 1$。
- 现在 $\textit{target}=[1,3,1]$，继续倒推，操作之前 $\textit{target}[1] +1+1 = 3$，解得操作之前 $\textit{target}[1] = 1$。
- 现在 $\textit{target}=[1,1,1]$，即初始数组。这说明我们可以通过操作初始数组得到目标数组。

在上面的过程中，我们需要：

1. 快速找到当前 $\textit{target}$ 的最大值，这可以用**最大堆**维护。
2. 快速计算除了 $\max(\textit{target})$ 以外的其余元素和，这可以用一个变量 $\textit{sum}$ 维护。

## 细节

如果最大值 $x = 100$，但其余元素之和只有 $\textit{sum} = 3$，那么 $100-3=97$，由于 $97>3$，新得到的数字 $97$ 比其余元素之和还要大，必然还是 $\textit{target}$ 的最大值。我们会把这个最大值反复地减去 $3$，直到 $\le 3$ 为止。

$$
100\to 97\to 94\to 91\to\cdots \to 4\to 1
$$

这太慢了。注意到上式与取模是类似的，即 $100\bmod 3 = 1$。

一般地，由于目标是把 $x$ 减少到 $\le \textit{sum}$ 为止（不能减成 $0$ 或者负数），也就是计算

$$
\begin{cases} 
x\bmod \textit{sum}, & x\bmod \textit{sum} > 0     \\
\textit{sum}, & x\bmod \textit{sum} = 0     \\
\end{cases}
$$

这等价于

$$
(x - 1) \bmod \textit{sum} + 1
$$

解释：把 $x$ 看成 $(x-1) + 1$。如果 $x$ 不是 $\textit{sum}$ 的倍数，那么 $x \bmod \textit{sum}  = (x-1) \bmod \textit{sum} + 1$，比如 $8\bmod 5 = 7\bmod 5 + 1 = 3$；如果 $x$ 是 $\textit{sum}$ 的倍数，那么 $(x - 1) \bmod \textit{sum} = \textit{sum}-1$，加一得到 $\textit{sum}$，这正好是我们要的。

## 答疑

**问**：我们把 $x$ 更新成了一个 $\le \textit{sum}$ 的数，但 $x$ 仍可能是 $\textit{target}$ 的最大值，这种情况怎么办？

**答**：有两种情况。第一种情况是更新后的数组全为 $1$，比如 $[1,1,1]$，$x$ 虽然是 $\textit{target}$ 的最大值，但我们可以跳出循环了。第二种情况是更新后的数组不全为 $1$，比如 $[2,1,1,1]$，那么下一轮循环我们一定会返回 $\texttt{false}$。

```py [sol-Python3]
class Solution:
    def isPossible(self, target: List[int]) -> bool:
        s = sum(target)
        h = target
        heapify_max(h)

        # 如果最大值等于 1，说明所有数都等于 1
        while h[0] > 1:
            x = h[0]
            s -= x  # 减去 x 后，s 为其余元素之和

            # s 不能是 0，这意味着 target 只有一个数且这个数大于 1
            # x 减去 s 后必须是正数
            if s == 0 or x <= s:
                return False

            # 把 x 多次减去 s，直到 x <= s 为止
            # 也就是计算 x%s，但如果 x%s == 0 则调整为 s
            x = (x - 1) % s + 1
            s += x
            heapreplace_max(h, x)

        return True
```

```java [sol-Java]
class Solution {
    public boolean isPossible(int[] target) {
        PriorityQueue<Integer> pq = new PriorityQueue<>((a, b) -> b - a);
        long sum = 0;
        for (int x : target) {
            pq.offer(x);
            sum += x;
        }

        // 如果最大值等于 1，说明所有数都等于 1
        while (pq.peek() > 1) {
            int x = pq.poll();
            sum -= x; // 减去 x 后，sum 为其余元素之和

            // sum 不能是 0，这意味着 target 只有一个数且这个数大于 1
            // x 减去 sum 后必须是正数
            if (sum == 0 || x <= sum) {
                return false;
            }

            // 把 x 多次减去 sum，直到 x <= sum 为止
            // 也就是计算 x%sum，但如果 x%sum == 0 则调整为 sum
            x = (x - 1) % (int) sum + 1;
            sum += x;
            pq.offer(x);
        }

        return true;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    bool isPossible(vector<int>& target) {
        long long sum = reduce(target.begin(), target.end(), 0LL);
        priority_queue<int> pq(target.begin(), target.end());

        // 如果最大值等于 1，说明所有数都等于 1
        while (pq.top() > 1) {
            int x = pq.top();
            pq.pop();
            sum -= x; // 减去 x 后，sum 为其余元素之和

            // sum 不能是 0，这意味着 target 只有一个数且这个数大于 1
            // x 减去 sum 后必须是正数
            if (sum == 0 || x <= sum) {
                return false;
            }

            // 把 x 多次减去 sum，直到 x <= sum 为止
            // 也就是计算 x%sum，但如果 x%sum == 0 则调整为 sum
            x = (x - 1) % sum + 1;
            sum += x;
            pq.push(x);
        }

        return true;
    }
};
```

```go [sol-Go]
func isPossible(target []int) bool {
	sum := 0
	for _, x := range target {
		sum += x
	}

	h := hp{target}
	heap.Init(&h)

	// 如果最大值等于 1，说明所有数都等于 1
	for h.IntSlice[0] > 1 {
		x := h.IntSlice[0]
		sum -= x // 减去 x 后，sum 为其余元素之和

		// sum 不能是 0，这意味着 target 只有一个数且这个数大于 1
		// x 减去 sum 后必须是正数
		if sum == 0 || x <= sum {
			return false
		}

		// 把 x 多次减去 sum，直到 x <= sum 为止
		// 也就是计算 x%sum，但如果 x%sum == 0 则调整为 sum
		x = (x-1)%sum + 1
		sum += x
		h.IntSlice[0] = x
		heap.Fix(&h, 0)
	}

	return true
}

type hp struct{ sort.IntSlice }
func (h hp) Less(i, j int) bool { return h.IntSlice[i] > h.IntSlice[j] }
func (hp) Push(any)             {} // 没用到，无需实现
func (hp) Pop() (_ any)         { return }
```

```js [sol-JavaScript]
var isPossible = function(target) {
    let sum = _.sum(target);
    const pq = new MaxPriorityQueue();
    for (const x of target) {
        pq.enqueue(x);
    }

    // 如果最大值等于 1，说明所有数都等于 1
    while (pq.front() > 1) {
        let x = pq.dequeue();
        sum -= x; // 减去 x 后，sum 为其余元素之和

        // sum 不能是 0，这意味着 target 只有一个数且这个数大于 1
        // x 减去 sum 后必须是正数
        if (sum === 0 || x <= sum) {
            return false;
        }

        // 把 x 多次减去 sum，直到 x <= sum 为止
        // 也就是计算 x%sum，但如果 x%sum == 0 则调整为 sum
        x = (x - 1) % sum + 1;
        sum += x;
        pq.enqueue(x);
    }

    return true;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;

impl Solution {
    pub fn is_possible(target: Vec<i32>) -> bool {
        let mut sum = target.iter().map(|&x| x as i64).sum();
        let mut pq = BinaryHeap::from(target); // 原地 O(n) 堆化

        // 如果最大值等于 1，说明所有数都等于 1
        while *pq.peek().unwrap() > 1 {
            let x = pq.pop().unwrap() as i64;
            sum -= x; // 减去 x 后，sum 为其余元素之和

            // sum 不能是 0，这意味着 target 只有一个数且这个数大于 1
            // x 减去 sum 后必须是正数
            if sum == 0 || x <= sum {
                return false;
            }

            // 把 x 多次减去 sum，直到 x <= sum 为止
            // 也就是计算 x%sum，但如果 x%sum == 0 则调整为 sum
            let x = (x - 1) % sum + 1;
            sum += x;
            pq.push(x as i32);
        }

        true
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + \log S\cdot \log n)$ 或者 $\mathcal{O}(n\log n + \log S\cdot \log n)$，其中 $n$ 是 $\textit{target}$ 的长度，$S$ 是 $\textit{target}$ 的元素和。瓶颈在建堆上，可以做到 $\mathcal{O}(n)$ 堆化。至于后面的循环，设最大值为 $x$，其余元素和为 $r$。如果循环能继续跑下去，那么 $x>r$，所以 $S=x+r > 2r$，即 $r< \dfrac{S}{2}$。此外，下一轮循环的最大值 $x'\le r < \dfrac{S}{2}$，同时 $x'$ 又是下下一轮循环的元素和，所以两轮循环之后 $S'' < \dfrac{S}{2}$。所以只会循环 $\mathcal{O}(\log S)$ 次，每次 $\mathcal{O}(\log n)$ 操作堆。所以时间复杂度瓶颈其实在建堆上，后面的循环只有 $\mathcal{O}(\log S\cdot \log n)$。
- 空间复杂度：$\mathcal{O}(1)$ 或 $\mathcal{O}(n)$。取决于是否原地堆化。

## 专题训练

1. 数据结构题单的「**五、堆（优先队列）**」。
2. 贪心与思维题单的「**§5.3 逆向思维**」。

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

本题来自 `五、思维题 / §5.4 逆向思维`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.4 逆向思维`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
