# 2398. 预算内的最多机器人数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-number-of-robots-within-budget/
- 题目 slug：`maximum-number-of-robots-within-budget`
- 来源专题：常用数据结构
- 来源分类路径：四、队列 / §4.4 单调队列
- 难度分：1917
- 外部题解来源：https://leetcode.cn/problems/maximum-number-of-robots-within-budget/solutions/1798725/by-endlesscheng-7ukp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[O(n) 做法：单调队列（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/maximum-number-of-robots-within-budget/solutions/1798725/by-endlesscheng-7ukp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-7ukp`
- topic id：`1798725`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置题目**：[239. 滑动窗口最大值](https://leetcode.cn/problems/sliding-window-maximum/)，视频讲解请看 [单调队列【基础算法精讲 27】](https://www.bilibili.com/video/BV1bM411X72E/)。

题目要求机器人连续运行，看成一个连续子数组，题目要求计算最长子数组长度。

枚举子数组右端点 $\textit{right}$，我们需要知道此时左端点 $\textit{left}$ 的最小值，这样子数组尽量长。

由于有 $\textit{budget}$ 的限制，所以 $\textit{right}$ 越大，$\textit{left}$ 也越大，有单调性，可以用**滑动窗口**解决。

本题的一种做法是二分答案，这样就转换成了固定长度的 239 题。

但实际上不用二分，在 239 题的基础上，把定长滑窗改为不定长滑窗，套路如下：

1. **入**：$\textit{chargeTimes}[\textit{right}]$ 进入窗口时，弹出队尾的 $\le \textit{chargeTimes}[\textit{right}]$ 的元素。
2. **出**：如果总开销超过 $\textit{budget}$，则不断移出左端点，直到总开销不超过 $\textit{budget}$。特别地，如果左端点恰好等于队首，则弹出队首。
3. **更新答案**：用窗口长度 $\textit{right}-\textit{left}+1$ 更新答案的最大值。

⚠**注意**：为了方便判断队首是否要出队，单调队列中保存的是下标。

```py [sol-Python3]
class Solution:
    def maximumRobots(self, chargeTimes: List[int], runningCosts: List[int], budget: int) -> int:
        ans = s = left = 0
        q = deque()
        for right, (t, c) in enumerate(zip(chargeTimes, runningCosts)):
            # 1. 入
            while q and t >= chargeTimes[q[-1]]:
                q.pop()
            q.append(right)
            s += c  # 维护 sum(runningCosts)

            # 2. 出
            while q and chargeTimes[q[0]] + (right - left + 1) * s > budget:
                if q[0] == left:
                    q.popleft()
                s -= runningCosts[left]  # 维护 sum(runningCosts)
                left += 1

            # 3. 更新答案
            ans = max(ans, right - left + 1)
        return ans
```

```java [sol-Java]
class Solution {
    public int maximumRobots(int[] chargeTimes, int[] runningCosts, long budget) {
        int ans = 0;
        int left = 0;
        long sum = 0;
        Deque<Integer> q = new ArrayDeque<>();
        for (int right = 0; right < chargeTimes.length; right++) {
            // 1. 入
            while (!q.isEmpty() && chargeTimes[right] >= chargeTimes[q.peekLast()]) {
                q.pollLast();
            }
            q.addLast(right);
            sum += runningCosts[right];

            // 2. 出
            while (!q.isEmpty() && chargeTimes[q.peekFirst()] + (right - left + 1) * sum > budget) {
                if (q.peekFirst() == left) {
                    q.pollFirst();
                }
                sum -= runningCosts[left++];
            }

            // 3. 更新答案
            ans = Math.max(ans, right - left + 1);
        }
        return ans;
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int maximumRobots(int[] chargeTimes, int[] runningCosts, long budget) {
        int ans = 0;
        int left = 0;
        long sum = 0;
        int[] q = new int[chargeTimes.length];
        int head = 0; // 队头
        int tail = 0; // 队尾+1
        for (int right = 0; right < chargeTimes.length; right++) {
            // 1. 入
            while (head < tail && chargeTimes[right] >= chargeTimes[q[tail - 1]]) {
                tail--;
            }
            q[tail++] = right;
            sum += runningCosts[right];

            // 2. 出
            while (head < tail && chargeTimes[q[head]] + (right - left + 1) * sum > budget) {
                if (q[head] == left) {
                    head++;
                }
                sum -= runningCosts[left++];
            }

            // 3. 更新答案
            ans = Math.max(ans, right - left + 1);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maximumRobots(vector<int>& chargeTimes, vector<int>& runningCosts, long long budget) {
        int ans = 0, left = 0;
        long long sum = 0;
        deque<int> q;
        for (int right = 0; right < chargeTimes.size(); right++) {
            // 1. 入
            while (!q.empty() && chargeTimes[right] >= chargeTimes[q.back()]) {
                q.pop_back();
            }
            q.push_back(right);
            sum += runningCosts[right];

            // 2. 出
            while (!q.empty() && chargeTimes[q.front()] + (right - left + 1) * sum > budget) {
                if (q.front() == left) {
                    q.pop_front();
                }
                sum -= runningCosts[left++];
            }

            // 3. 更新答案
            ans = max(ans, right - left + 1);
        }
        return ans;
    }
};
```

```c [sol-C]
#define MAX(a, b) ((b) > (a) ? (b) : (a))

int maximumRobots(int* chargeTimes, int chargeTimesSize, int* runningCosts, int runningCostsSize, long long budget) {
    int ans = 0, left = 0;
    long long sum = 0;
    int* q = malloc(chargeTimesSize * sizeof(int));
    int head = 0, tail = 0; // 队头和队尾
    for (int right = 0; right < chargeTimesSize; right++) {
        // 1. 入
        while (head < tail && chargeTimes[right] >= chargeTimes[q[tail - 1]]) {
            tail--;
        }
        q[tail++] = right;
        sum += runningCosts[right];

        // 2. 出
        while (head < tail && chargeTimes[q[head]] + (right - left + 1) * sum > budget) {
            if (q[head] == left) {
                head++;
            }
            sum -= runningCosts[left++];
        }

        // 3. 更新答案
        ans = MAX(ans, right - left + 1);
    }
    free(q);
    return ans;
}
```

```go [sol-Go]
func maximumRobots(chargeTimes, runningCosts []int, budget int64) (ans int) {
    q := []int{}
    sum := int64(0)
    left := 0
    for right, t := range chargeTimes {
        // 1. 入
        for len(q) > 0 && t >= chargeTimes[q[len(q)-1]] {
            q = q[:len(q)-1]
        }
        q = append(q, right)
        sum += int64(runningCosts[right])

        // 2. 出
        for len(q) > 0 && int64(chargeTimes[q[0]])+int64(right-left+1)*sum > budget {
            if q[0] == left {
                q = q[1:]
            }
            sum -= int64(runningCosts[left])
            left++
        }

        // 3. 更新答案
        ans = max(ans, right-left+1)
    }
    return
}
```

```js [sol-JavaScript]
var maximumRobots = function(chargeTimes, runningCosts, budget) {
    let ans = 0, left = 0, sum = 0;
    const q = Array(chargeTimes.length);
    let head = 0, tail = 0; // 队头和队尾
    for (let right = 0; right < chargeTimes.length; right++) {
        // 1. 入
        while (head < tail && chargeTimes[right] >= chargeTimes[q[tail - 1]]) {
            tail--;
        }
        q[tail++] = right;
        sum += runningCosts[right];

        // 2. 出
        while (head < tail && chargeTimes[q[head]] + (right - left + 1) * sum > budget) {
            if (q[head] === left) {
                head++;
            }
            sum -= runningCosts[left++];
        }

        // 3. 更新答案
        ans = Math.max(ans, right - left + 1);
    }
    return ans;
};
```

```rust [sol-Rust]
use std::collections::VecDeque;

impl Solution {
    pub fn maximum_robots(charge_times: Vec<i32>, running_costs: Vec<i32>, budget: i64) -> i32 {
        let mut ans = 0;
        let mut left = 0;
        let mut sum = 0i64;
        let mut q = VecDeque::new();
        for right in 0..charge_times.len() {
            // 1. 入
            while !q.is_empty() && charge_times[right] >= charge_times[*q.back().unwrap()] {
                q.pop_back();
            }
            q.push_back(right);
            sum += running_costs[right] as i64;

            // 2. 出
            while !q.is_empty() && charge_times[*q.front().unwrap()] as i64 + (right - left + 1) as i64 * sum > budget {
                if *q.front().unwrap() == left {
                    q.pop_front();
                }
                sum -= running_costs[left] as i64;
                left += 1;
            }

            // 3. 更新答案
            ans = ans.max(right - left + 1);
        }
        ans as _
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 为 $\textit{chargeTimes}$ 的长度。虽然有二重循环，但是每个元素至多出队一次，以及 $\textit{left}$ 最多增加 $n$ 次。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

把「子数组」改成「子序列」要怎么做？

欢迎在评论区发表的你的思路/代码。

更多相似题目，见下面数据结构题单中的「**§4.3 单调队列**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与一般树（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/circle/discuss/K0n2gO/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `四、队列 / §4.4 单调队列`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `四、队列 / §4.4 单调队列`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
