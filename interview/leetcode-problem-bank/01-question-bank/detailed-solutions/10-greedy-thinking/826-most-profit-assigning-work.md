# 826. 安排工作以达到最大收益

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/most-profit-assigning-work/
- 题目 slug：`most-profit-assigning-work`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.3 双序列配对
- 难度分：1709
- 外部题解来源：https://leetcode.cn/problems/most-profit-assigning-work/solutions/2780326/pai-xu-shuang-zhi-zhen-pythonjavacgojsru-gthg/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序+双指针（Python/Java/C++/Go/JS/Rust）](https://leetcode.cn/problems/most-profit-assigning-work/solutions/2780326/pai-xu-shuang-zhi-zhen-pythonjavacgojsru-gthg/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-shuang-zhi-zhen-pythonjavacgojsru-gthg`
- topic id：`2780326`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:46:10 +0800

如果把 $\textit{worker}$ 按照从小到大的顺序排序，那么第 $i$ 个工人能做的工作，他右边的（$\textit{worker}$ 值更大的）工人也能做。这意味着，如果我们遍历了所有 $\textit{difficulty}[j]\le \textit{worker}[i]$ 的工作，那么从第 $i$ 个工人到第 $i+1$ 个工人，只需要**额外遍历** $\textit{worker}[i] < \textit{difficulty}[j]\le \textit{worker}[i+1]$ 的工作。

把 $\textit{difficulty}$ 和 $\textit{profit}$ 绑在一起，按照 $\textit{difficulty}$ 从小到大排序，我们可以在遍历 $\textit{worker}$ 的同时，用双指针遍历并维护 $\textit{difficulty}[j]\le \textit{worker}[i]$ 的最大的 $\textit{profit}[j]$，即为第 $i$ 个工人所能获得的最大利润。累加每名工人能获得的最大利润，即为答案。

```py [sol-Python3]
class Solution:
    def maxProfitAssignment(self, difficulty: List[int], profit: List[int], worker: List[int]) -> int:
        jobs = sorted(zip(difficulty, profit))
        worker.sort()
        ans = j = max_profit = 0
        for w in worker:
            while j < len(jobs) and jobs[j][0] <= w:
                max_profit = max(max_profit, jobs[j][1])
                j += 1
            ans += max_profit
        return ans
```

```java [sol-Java]
class Solution {
    public int maxProfitAssignment(int[] difficulty, int[] profit, int[] worker) {
        int n = difficulty.length;
        int[][] jobs = new int[n][2];
        for (int i = 0; i < n; i++) {
            jobs[i][0] = difficulty[i];
            jobs[i][1] = profit[i];
        }
        Arrays.sort(jobs, (a, b) -> a[0] - b[0]);
        Arrays.sort(worker);
        int ans = 0, j = 0, maxProfit = 0;
        for (int w : worker) {
            while (j < n && jobs[j][0] <= w) {
                maxProfit = Math.max(maxProfit, jobs[j++][1]);
            }
            ans += maxProfit;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxProfitAssignment(vector<int>& difficulty, vector<int>& profit, vector<int>& worker) {
        int n = difficulty.size();
        vector<pair<int, int>> jobs(n);
        for (int i = 0; i < n; i++) {
            jobs[i] = {difficulty[i], profit[i]};
        }
        ranges::sort(jobs);
        ranges::sort(worker);
        int ans = 0, j = 0, max_profit = 0;
        for (int w : worker) {
            while (j < n && jobs[j].first <= w) {
                max_profit = max(max_profit, jobs[j++].second);
            }
            ans += max_profit;
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxProfitAssignment(difficulty []int, profit []int, worker []int) (ans int) {
    n := len(difficulty)
    type job struct{ d, p int }
    jobs := make([]job, n)
    for i, d := range difficulty {
        jobs[i] = job{d, profit[i]}
    }
    slices.SortFunc(jobs, func(a, b job) int { return a.d - b.d })
    slices.Sort(worker)
    j, maxProfit := 0, 0
    for _, w := range worker {
        for j < n && jobs[j].d <= w {
            maxProfit = max(maxProfit, jobs[j].p)
            j++
        }
        ans += maxProfit
    }
    return ans
}
```

```js [sol-JavaScript]
var maxProfitAssignment = function(difficulty, profit, worker) {
    const jobs = _.zip(difficulty, profit).sort((a, b) => a[0] - b[0]);
    worker.sort((a, b) => a - b);
    let ans = 0, j = 0, maxProfit = 0;
    for (const w of worker) {
        while (j < jobs.length && jobs[j][0] <= w) {
            maxProfit = Math.max(maxProfit, jobs[j++][1]);
        }
        ans += maxProfit;
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn max_profit_assignment(difficulty: Vec<i32>, profit: Vec<i32>, mut worker: Vec<i32>) -> i32 {
        let mut jobs = difficulty.into_iter().zip(profit.into_iter()).collect::<Vec<_>>();
        jobs.sort_unstable_by(|a, b| a.0.cmp(&b.0));
        worker.sort_unstable();
        let mut ans = 0;
        let mut j = 0;
        let mut max_profit = 0;
        for w in worker {
            while j < jobs.len() && jobs[j].0 <= w {
                max_profit = max_profit.max(jobs[j].1);
                j += 1;
            }
            ans += max_profit;
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n + m\log m)$，其中 $n$ 是 $\textit{difficulty}$ 的长度，$m$ 是 $\textit{worker}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(n)$。

## 分类题单

1. [滑动窗口（定长/不定长/多指针）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（矩形系列/字典序最小/贡献法）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/贪心/脑筋急转弯）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. [动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.3 双序列配对`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.3 双序列配对`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
