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
