# LCP 24. 数字游戏

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/5TxKeK/
- 题目 slug：`5TxKeK`
- 来源专题：贪心与思维
- 来源分类路径：四、数学贪心 / §4.5 中位数贪心
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/5TxKeK/solutions/2627350/zhuan-huan-zhong-wei-shu-tan-xin-dui-din-7r9b/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[转换+中位数贪心+对顶堆（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/5TxKeK/solutions/2627350/zhuan-huan-zhong-wei-shu-tan-xin-dui-din-7r9b/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zhuan-huan-zhong-wei-shu-tan-xin-dui-din-7r9b`
- topic id：`2627350`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:07:12 +0800

## 转换

设 $\textit{nums}$ 的前 $5$ 个数分别是 $a_0,a_1,a_2,a_3,a_4$。假设 $a_0$ 最终变成整数 $x$，那么这 $5$ 个数最终要变成 $x,x+1,x+2,x+3,x+4$，操作次数为

$$
\begin{aligned}
&|a_0-x| + |a_1 - (x+1)| + |a_2 - (x+2)| + |a_3 - (x+3)| + |a_4 - (x+4)|\\
=\ &|a_0-x| + |(a_1-1) - x| + |(a_2-2) - x| + |(a_3-3) - x| + |(a_4-4) - x|
\end{aligned}
$$

我们需要最小化上式的值。

定义 $b_i = a_i - i$，问题变成求

$$
|b_0-x| + |b_1 - x| + |b_2 - x| + |b_3 - x| + |b_4 - x|
$$

的最小值。

这相当于在问：**把** $b_i$ **都变成同一个整数** $x$，**最少要操作多少次**？

在示例 3 中，把 $[1,1,1,2,3]$ 变成连续递增序列的最小操作次数，等价与把 $[1,0,-1,-1,-1]$ 中的数都变成同一个整数的最小操作次数。最佳方案是都变成 $x=-1$，需要操作 $3$ 次。对应的，$[1,1,1,2,3]$ 就变成了 $[-1,0,1,2,3]$。

根据 [中位数贪心及其证明](https://zhuanlan.zhihu.com/p/1922938031687595039)，把所有数变成 $b$ 的中位数是最优的。

由证明方法一可知，如果 $n$ 是偶数，那么最小距离和等于 $b$ 的右半之和减去左半之和，即 

$$
\begin{aligned}
&(b_{n-1}-b_{0}) + (b_{n-2}-b_{1}) + \cdots + (b_{n/2} - b_{n/2-1})\\
=\ &(b_{n/2} + b_{n/2+1} + \cdots + b_{n-1}) - (b_0 + b_1 + \cdots + b_{n/2-1})
\end{aligned}
$$

如果 $n$ 是奇数，最小距离和等于去掉中位数 $b_{\lfloor n/2\rfloor}$ 之后，$b$ 的右半之和减去左半之和。

## 对顶堆维护动态中位数

回到题目，我们需要对 $\textit{nums}$ 的每个前缀，按照提示 2 中的公式，计算最小距离和。

做法类似 [295. 数据流的中位数](https://leetcode.cn/problems/find-median-from-data-stream/)，用两个堆来维护前 $k$ 个数中，$\textit{nums}[i]-i$ 较小的一半和较大的一半，从而动态维护提示 2 中的「左半之和」以及「右半之和」。

具体来说，用一个大根堆 $\textit{left}$ 维护较小的一半，其元素和为 $\textit{leftSum}$；用一个小根堆 $\textit{right}$ 维护较大的一半，其元素和为 $\textit{rightSum}$。遍历 $\textit{nums}[i]$，设 $b = \textit{nums}[i] - i$，分类讨论：

- 如果 $i$ 是偶数，在插入 $b$ 之前，$\textit{left}$ 和 $\textit{right}$ 大小相等，我们先把 $b$ 插入 $\textit{left}$，然后弹出 $\textit{left}$ 的堆顶，加到 $\textit{right}$ 中。这一操作可以保证，无论 $b$ 是大是小，此时 $\textit{right}$ 的堆顶就是中位数 $x$。最小距离和为 $\textit{rightSum}-x-\textit{leftSum}$。
- 如果 $i$ 是奇数，在插入 $b$ 之前，$\textit{left}$ 比 $\textit{right}$ 少一个元素，我们先把 $b$ 插入 $\textit{right}$，然后弹出 $\textit{right}$ 的堆顶，加到 $\textit{left}$ 中。最小距离和为 $\textit{rightSum}-\textit{leftSum}$。

最后，别忘了对 $10^9+7$ 取模。

```py [sol-Python3]
class Solution:
    def numsGame(self, nums: List[int]) -> List[int]:
        MOD = 1_000_000_007
        ans = [0] * len(nums)
        left = []  # 维护较小的一半，大根堆（小根堆取负号）
        right = []  # 维护较大的一半，小根堆
        left_sum = right_sum = 0
        for i, b in enumerate(nums):
            b -= i
            if i % 2 == 0:  # 前缀长度是奇数
                left_sum -= max(-left[0] - b, 0) if left else 0
                t = -heappushpop(left, -b)
                right_sum += t
                heappush(right, t)
                ans[i] = (right_sum - right[0] - left_sum) % MOD
            else:  # 前缀长度是偶数
                right_sum += max(b - right[0], 0)
                t = heappushpop(right, b)
                left_sum += t
                heappush(left, -t)
                ans[i] = (right_sum - left_sum) % MOD
        return ans
```

```java [sol-Java]
class Solution {
    public int[] numsGame(int[] nums) {
        final int MOD = 1_000_000_007;
        int[] ans = new int[nums.length];
        PriorityQueue<Integer> left = new PriorityQueue<>((a, b) -> b - a); // 维护较小的一半，大根堆
        PriorityQueue<Integer> right = new PriorityQueue<>(); // 维护较大的一半，小根堆
        long leftSum = 0, rightSum = 0;
        for (int i = 0; i < nums.length; i++) {
            int b = nums[i] - i;
            if (i % 2 == 0) { // 前缀长度是奇数
                if (!left.isEmpty() && b < left.peek()) {
                    leftSum -= left.peek() - b;
                    left.offer(b);
                    b = left.poll();
                }
                rightSum += b;
                right.offer(b);
                ans[i] = (int) ((rightSum - right.peek() - leftSum) % MOD);
            } else { // 前缀长度是偶数
                if (b > right.peek()) {
                    rightSum += b - right.peek();
                    right.offer(b);
                    b = right.poll();
                }
                leftSum += b;
                left.offer(b);
                ans[i] = (int) ((rightSum - leftSum) % MOD);
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> numsGame(vector<int>& nums) {
        const int MOD = 1'000'000'007;
        vector<int> ans(nums.size());
        priority_queue<int> left; // 维护较小的一半，大根堆
        priority_queue<int, vector<int>, greater<>> right; // 维护较大的一半，小根堆
        long long left_sum = 0, right_sum = 0;
        for (int i = 0; i < nums.size(); i++) {
            int b = nums[i] - i;
            if (i % 2 == 0) { // 前缀长度是奇数
                if (!left.empty() && b < left.top()) {
                    left_sum -= left.top() - b;
                    left.push(b);
                    b = left.top();
                    left.pop();
                }
                right_sum += b;
                right.push(b);
                ans[i] = (right_sum - right.top() - left_sum) % MOD;
            } else { // 前缀长度是偶数
                if (b > right.top()) {
                    right_sum += b - right.top();
                    right.push(b);
                    b = right.top();
                    right.pop();
                }
                left_sum += b;
                left.push(b);
                ans[i] = (right_sum - left_sum) % MOD;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func numsGame(nums []int) []int {
    const mod = 1_000_000_007
    ans := make([]int, len(nums))
    left := hp{}  // 维护较小的一半，大根堆（小根堆取负号）
    right := hp{} // 维护较大的一半，小根堆
    for i, b := range nums {
        b -= i
        if i%2 == 0 {
            heap.Push(&right, -left.pushPop(-b))
            x := right.IntSlice[0] // 中位数
            // 原本要减去 left.sum，但由于 left 所有元素都取负号，所以负负得正改为加法
            ans[i] = (right.sum - x + left.sum) % mod
        } else {
            heap.Push(&left, -right.pushPop(b))
            ans[i] = (right.sum + left.sum) % mod
        }
    }
    return ans
}

type hp struct {
    sort.IntSlice // 继承 Len, Less, Swap
    sum int // 堆中元素之和
}

func (h *hp) Push(v any) { h.IntSlice = append(h.IntSlice, v.(int)); h.sum += v.(int) }
func (hp) Pop() (_ any)  { return } // 没用到，无需实现

// pushPop 先把 v 入堆，然后弹出并返回堆顶
// 如果 v <= 堆顶，则直接返回 v
func (h *hp) pushPop(v int) int {
    if h.Len() > 0 && v > h.IntSlice[0] {
        h.sum += v - h.IntSlice[0]
        v, h.IntSlice[0] = h.IntSlice[0], v
        heap.Fix(h, 0)
    }
    return v
}
```

```js [sol-JavaScript]
var numsGame = function(nums) {
    const MOD = 1_000_000_007;
    const ans = Array(nums.length);
    const left = new MaxPriorityQueue(); // 维护较小的一半，大根堆
    const right = new MinPriorityQueue(); // 维护较大的一半，小根堆
    let leftSum = 0, rightSum = 0;
    for (let i = 0; i < nums.length; i++) {
        let b = nums[i] - i;
        if (i % 2 === 0) { // 前缀长度是奇数
            if (!left.isEmpty() && b < left.front()) {
                leftSum -= left.front() - b;
                left.enqueue(b);
                b = left.dequeue();
            }
            rightSum += b;
            right.enqueue(b);
            ans[i] = (rightSum - right.front() - leftSum) % MOD;
        } else { // 前缀长度是偶数
            if (b > right.front()) {
                rightSum += b - right.front();
                right.enqueue(b);
                b = right.dequeue();
            }
            leftSum += b;
            left.enqueue(b);
            ans[i] = (rightSum - leftSum) % MOD;
        }
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::BinaryHeap;
use std::cmp::Reverse;

impl Solution {
    pub fn nums_game(nums: Vec<i32>) -> Vec<i32> {
        const MOD: i64 = 1_000_000_007;
        let mut ans = vec![0; nums.len()];
        let mut left = BinaryHeap::new(); // 维护较小的一半，大根堆
        let mut right = BinaryHeap::new(); // 维护较大的一半，小根堆
        let mut left_sum = 0i64;
        let mut right_sum = 0i64;
        for (i, &num) in nums.iter().enumerate() {
            let mut b = num - i as i32;
            if i % 2 == 0 { // 前缀长度是奇数
                if let Some(&top) = left.peek() {
                    if b < top {
                        left_sum -= (top - b) as i64;
                        left.push(b);
                        b = left.pop().unwrap();
                    }
                }
                right_sum += b as i64;
                right.push(Reverse(b));
                ans[i] = ((right_sum - right.peek().unwrap().0 as i64 - left_sum) % MOD) as i32;
            } else { // 前缀长度是偶数
                let top = right.peek().unwrap().0;
                if b > top {
                    right_sum += (b - top) as i64;
                    right.push(Reverse(b));
                    b = right.pop().unwrap().0;
                }
                left_sum += b as i64;
                left.push(b);
                ans[i] = ((right_sum - left_sum) % MOD) as i32;
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。每次操作堆的时间复杂度是 $\mathcal{O}(\log n)$。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

1. 贪心题单的「**§4.5 中位数贪心**」。
2. 数据结构题单的「**§5.7 对顶堆**」。

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

本题来自 `四、数学贪心 / §4.5 中位数贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、数学贪心 / §4.5 中位数贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
