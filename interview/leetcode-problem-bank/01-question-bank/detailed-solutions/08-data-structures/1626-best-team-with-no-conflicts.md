# 1626. 无矛盾的最佳球队

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/best-team-with-no-conflicts/
- 题目 slug：`best-team-with-no-conflicts`
- 来源专题：常用数据结构
- 来源分类路径：八、树状数组和线段树 / §8.1 树状数组
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/best-team-with-no-conflicts/solutions/2183029/zui-chang-di-zeng-zi-xu-lie-cong-on2-dao-ojqu/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[最长递增子序列：从 O(n^2) 到 O(nlogn)（Python/Java/C++/Go）](https://leetcode.cn/problems/best-team-with-no-conflicts/solutions/2183029/zui-chang-di-zeng-zi-xu-lie-cong-on2-dao-ojqu/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zui-chang-di-zeng-zi-xu-lie-cong-on2-dao-ojqu`
- topic id：`2183029`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

# 方法一：排序 + 动态规划

### 前置知识：动态规划

入门讲解：[【基础算法精讲 17】](https://www.bilibili.com/video/BV1Xj411K7oF/)。

最长递增子序列：[【基础算法精讲 20】](https://www.bilibili.com/video/BV1ub411Q7sB/)，推荐先看完这期再阅读下面的题解。

> APP 用户需要分享到 wx 打开链接。

### 思路

由于球员在数组中的下标不重要，所以可以按照分数或者年龄排序。这里按照分数从小到大排序，分数相同的按照年龄从小到大排序。

假设第 $i$ 个人是球队中下标最大的，那么对于在球队中的下标比 $i$ 小的 $j$，必然有 $\textit{scores}[j]\le\textit{scores}[i]$：

- 如果 $\textit{scores}[j]=\textit{scores}[i]$，由于分数相同的，按照年龄从小到大排序，所以 $\textit{ages}[j]\le \textit{ages}[i]$；
- 如果 $\textit{scores}[j]<\textit{scores}[i]$，根据题目要求，必须满足 $\textit{ages}[j]\le \textit{ages}[i]$。

所以 $\textit{ages}[j]\le \textit{ages}[i]$。

也就是说，需要从 $\textit{ages}$ 中选择一个得分之和最大的递增子序列（允许有相同元素），这与 [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/) 是类似的，具体讲解可以看我在上面贴的链接。

排序后，仿照最长递增子序列，定义 $f[i]$ 表示以 $\textit{ages}[i]$ 结尾的递增子序列的最大得分，则有转移方程

$$
f[i] = \max(f[j]) + \textit{scores}[i]
$$

其中 $j<i$ 且 $\textit{ages}[j] \le \textit{ages}[i]$。

初始 $f[i]=0$。答案为 $\max(f[i])$。

### 答疑

**问**：能否先按照年龄，再按照分数排序？

**答**：可以的。分析过程同上。

```py [sol1-Python3]
class Solution:
    def bestTeamScore(self, scores: List[int], ages: List[int]) -> int:
        a = sorted(zip(scores, ages))
        f = [0] * len(a)
        for i, (score, age) in enumerate(a):
            for j in range(i):
                if a[j][1] <= age:
                    f[i] = max(f[i], f[j])
            f[i] += score
        return max(f)
```

```java [sol1-Java]
class Solution {
    public int bestTeamScore(int[] scores, int[] ages) {
        int n = scores.length, ans = 0;
        var ids = new Integer[n];
        for (int i = 0; i < n; ++i)
            ids[i] = i;
        Arrays.sort(ids, (i, j) -> scores[i] != scores[j] ? scores[i] - scores[j] : ages[i] - ages[j]);

        var f = new int[n + 1];
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < i; ++j)
                if (ages[ids[j]] <= ages[ids[i]])
                    f[i] = Math.max(f[i], f[j]);
            f[i] += scores[ids[i]];
            ans = Math.max(ans, f[i]);
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int bestTeamScore(vector<int> &scores, vector<int> &ages) {
        int n = scores.size();
        pair<int, int> a[n];
        for (int i = 0; i < n; ++i)
            a[i] = {scores[i], ages[i]};
        sort(a, a + n);

        int f[n]; memset(f, 0, sizeof(f));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < i; ++j)
                if (a[j].second <= a[i].second)
                    f[i] = max(f[i], f[j]);
            f[i] += a[i].first;
        }
        return *max_element(f, f + n);
    }
};
```

```go [sol1-Go]
func bestTeamScore(scores, ages []int) (ans int) {
    n := len(scores)
    type pair struct{ score, age int }
    a := make([]pair, n)
    for i, s := range scores {
        a[i] = pair{s, ages[i]}
    }
    sort.Slice(a, func(i, j int) bool {
        a, b := a[i], a[j]
        return a.score < b.score || a.score == b.score && a.age < b.age
    })

    f := make([]int, n)
    for i, p := range a {
        for j, q := range a[:i] {
            if q.age <= p.age {
                f[i] = max(f[i], f[j])
            }
        }
        f[i] += p.score
        ans = max(ans, f[i])
    }
    return
}

func max(a, b int) int { if a < b { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$O(n^2)$，其中 $n$ 为 $\textit{ages}$ 的长度。
- 空间复杂度：$O(n)$。

# 方法二：基于值域计算

同样按照方法一排序。

注意到 $\textit{ages}[i]$ 的范围比较小，考虑从值域来计算。

在遍历球员的同时，维护数组 $\textit{maxSum}$，其中 $\textit{maxSum}[x]$ 表示年龄最大值**恰好等于** $x$ 的球队最大分数和。

枚举年龄不超过 $\textit{ages}[i]$ 的 $y$，有

$$
\textit{maxSum}[\textit{ages}[i]] = \max\limits_{y=1}^{\textit{ages}[i]}\textit{maxSum}[y] + \textit{scores}[i]
$$

初始 $\textit{maxSum}[x]=0$，答案为 $\max(\textit{maxSum}[x])$。

### 答疑

**问**：怎么想到要从值域思考的，这是一个套路吗？

**答**：是的。LIS 问题除了有 $O(n\log n)$ 的二分做法外，还有基于值域的线段树做法，具体可以看 [2407. 最长递增子序列 II](https://leetcode.cn/problems/longest-increasing-subsequence-ii/)。

```py [sol2-Python3]
class Solution:
    def bestTeamScore(self, scores: List[int], ages: List[int]) -> int:
        max_sum = [0] * (max(ages) + 1)
        for score, age in sorted(zip(scores, ages)):
            max_sum[age] = max(max_sum[:age + 1]) + score
        return max(max_sum)
```

```java [sol2-Java]
class Solution {
    public int bestTeamScore(int[] scores, int[] ages) {
        int n = scores.length, u = 0, ans = 0;
        var ids = new Integer[n];
        for (int i = 0; i < n; ++i) {
            ids[i] = i;
            u = Math.max(u, ages[i]);
        }
        Arrays.sort(ids, (i, j) -> scores[i] != scores[j] ? scores[i] - scores[j] : ages[i] - ages[j]);

        var maxSum = new int[u + 1];
        for (int i : ids) {
            int age = ages[i];
            for (int j = 1; j <= age; ++j)
                maxSum[age] = Math.max(maxSum[age], maxSum[j]);
            maxSum[age] += scores[i];
            ans = Math.max(ans, maxSum[age]);
        }
        return ans;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    int bestTeamScore(vector<int> &scores, vector<int> &ages) {
        int n = scores.size();
        pair<int, int> a[n];
        for (int i = 0; i < n; ++i)
            a[i] = {scores[i], ages[i]};
        sort(a, a + n);

        int u = *max_element(ages.begin(), ages.end());
        int max_sum[u + 1]; memset(max_sum, 0, sizeof(max_sum));
        for (auto &[score, age] : a)
            max_sum[age] = *max_element(max_sum, max_sum + age + 1) + score;
        return *max_element(max_sum, max_sum + u + 1);
    }
};
```

```go [sol2-Go]
func bestTeamScore(scores, ages []int) int {
    n := len(scores)
    type pair struct{ score, age int }
    a := make([]pair, n)
    for i, age := range ages {
        a[i] = pair{scores[i], age}
    }
    sort.Slice(a, func(i, j int) bool {
        a, b := a[i], a[j]
        return a.score < b.score || a.score == b.score && a.age < b.age
    })

    maxSum := make([]int, max(ages)+1)
    for _, p := range a {
        maxSum[p.age] = max(maxSum[:p.age+1]) + p.score
    }
    return max(maxSum)
}

func max(a []int) int {
    mx := a[0]
    for _, x := range a {
        if x > mx {
            mx = x
        }
    }
    return mx
}
```

### 复杂度分析

- 时间复杂度：$O(n\log n + nU)$，其中 $n$ 为 $\textit{ages}$ 的长度，$U=\max(\textit{ages})$。
- 空间复杂度：$O(n+U)$。

> 注：如果对 $\textit{ages}$ **离散化**，可以做到 $O(n^2)$ 时间复杂度和 $O(n)$ 空间复杂度。

# 方法三：树状数组优化

### 前置知识：树状数组

见 [树状数组](https://oi-wiki.org/ds/fenwick/#%E6%A0%91%E7%8A%B6%E6%95%B0%E7%BB%84)。

### 思路

方法二循环中计算的是 $\textit{maxSum}$ 的**前缀最大值**，这可以用树状数组（或者线段树）优化至 $O(\log U)$（若先离散化，则为 $O(\log n)$）。

```py [sol3-Python3]
class Solution:
    def bestTeamScore(self, scores: List[int], ages: List[int]) -> int:
        u = max(ages)
        t = [0] * (u + 1)

        # 返回 max(max_sum[:i+1])
        def query(i: int) -> int:
            mx = 0
            while i:
                mx = max(mx, t[i])
                i &= i - 1
            return mx

        # 更新 max_sum[i] 为 mx
        def update(i: int, mx: int) -> None:
            while i < len(t):
                t[i] = max(t[i], mx)
                i += i & -i

        for score, age in sorted(zip(scores, ages)):
            update(age, query(age) + score)
        return query(u)
```

```java [sol3-Java]
class Solution {
    public int bestTeamScore(int[] scores, int[] ages) {
        int n = scores.length;
        var ids = new Integer[n];
        for (int i = 0; i < n; ++i)
            ids[i] = i;
        Arrays.sort(ids, (i, j) -> scores[i] != scores[j] ? scores[i] - scores[j] : ages[i] - ages[j]);

        for (int i : ids)
            update(ages[i], query(ages[i]) + scores[i]);
        return query(MX);
    }

    private static final int MX = 1000;
    private final int[] t = new int[MX + 1];

    // 返回 max(maxSum[:i+1])
    private int query(int i) {
        int mx = 0;
        for (; i > 0; i &= i - 1)
            mx = Math.max(mx, t[i]);
        return mx;
    }

    // 更新 maxSum[i] 为 mx
    private void update(int i, int mx) {
        for (; i <= MX; i += i & -i)
            t[i] = Math.max(t[i], mx);
    }
}
```

```cpp [sol3-C++]
class Solution {
    static constexpr int MX = 1000;
    int t[MX + 1];

    // 返回 max(max_sum[:i+1])
    int query(int i) {
        int mx = 0;
        for (; i; i &= i - 1)
            mx = max(mx, t[i]);
        return mx;
    }

    // 更新 max_sum[i] 为 mx
    void update(int i, int mx) {
        for (; i <= MX; i += i & -i)
            t[i] = max(t[i], mx);
    }

public:
    int bestTeamScore(vector<int> &scores, vector<int> &ages) {
        int n = scores.size();
        pair<int, int> a[n];
        for (int i = 0; i < n; ++i)
            a[i] = {scores[i], ages[i]};
        sort(a, a + n);

        for (auto &[score, age] : a)
            update(age, query(age) + score);
        return query(MX);
    }
};
```

```go [sol3-Go]
type BIT []int

// 返回 max(maxSum[:i+1])
func (t BIT) query(i int) (mx int) {
    for ; i > 0; i &= i - 1 {
        mx = max(mx, t[i])
    }
    return
}

// 更新 maxSum[i] 为 mx
func (t BIT) update(i, mx int) {
    for ; i < len(t); i += i & -i {
        t[i] = max(t[i], mx)
    }
}

func bestTeamScore(scores, ages []int) (ans int) {
    type pair struct{ score, age int }
    a, u := make([]pair, len(scores)), 0
    for i, age := range ages {
        a[i] = pair{scores[i], age}
        u = max(u, age)
    }
    sort.Slice(a, func(i, j int) bool {
        a, b := a[i], a[j]
        return a.score < b.score || a.score == b.score && a.age < b.age
    })

    t := make(BIT, u+1)
    for _, p := range a {
        t.update(p.age, t.query(p.age)+p.score)
    }
    return t.query(u)
}

func max(a, b int) int { if a < b { return b }; return a }
```

### 复杂度分析

- 时间复杂度：$O(n\log n + n\log U)$，其中 $n$ 为 $\textit{ages}$ 的长度，$U=\max(\textit{ages})$。排序的时间复杂度为 $O(n\log n)$，单次查询和修改树状数组的时间复杂度都是 $O(\log U)$，一共操作 $O(n)$ 次，所以总的时间复杂度为 $O(n\log n + n\log U)$。
- 空间复杂度：$O(n+U)$。

> 注：如果对 $\textit{ages}$ **离散化**，可以做到 $O(n\log n)$ 时间复杂度和 $O(n)$ 空间复杂度。本题由于 $U$ 比较小，这一优化并不明显。感兴趣的读者可以在评论区贴出你的代码实现。

### 相似题目

- [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/)
- [354. 俄罗斯套娃信封问题](https://leetcode.cn/problems/russian-doll-envelopes/)
- [1691. 堆叠长方体的最大高度](https://leetcode.cn/problems/maximum-height-by-stacking-cuboids/)
- [2407. 最长递增子序列 II](https://leetcode.cn/problems/longest-increasing-subsequence-ii/)

---

欢迎关注[【biIibiIi@灵茶山艾府】](https://space.bilibili.com/206214)，高质量算法教学，持续更新中~

附：[每日一题·高质量题解精选](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)。

## 本地原创解析

### 1. 题意重述

本题来自 `八、树状数组和线段树 / §8.1 树状数组`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `八、树状数组和线段树 / §8.1 树状数组`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
