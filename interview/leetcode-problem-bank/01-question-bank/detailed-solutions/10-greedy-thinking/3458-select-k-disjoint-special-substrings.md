# 3458. 选择 K 个互不重叠的特殊子字符串

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/select-k-disjoint-special-substrings/
- 题目 slug：`select-k-disjoint-special-substrings`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.1 不相交区间
- 难度分：1520
- 外部题解来源：https://leetcode.cn/problems/select-k-disjoint-special-substrings/solutions/3077261/you-xiang-tu-jian-mo-he-bing-qu-jian-bu-sw1lb/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[有向图建模+合并区间+无重叠区间（Python/Java/C++/Go）](https://leetcode.cn/problems/select-k-disjoint-special-substrings/solutions/3077261/you-xiang-tu-jian-mo-he-bing-qu-jian-bu-sw1lb/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`you-xiang-tu-jian-mo-he-bing-qu-jian-bu-sw1lb`
- topic id：`3077261`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

题目要求「子字符串中的任何字符都不应该出现在字符串其余部分中」，所以如果子串包含字母 $\texttt{a}$，那么最左边的 $\texttt{a}$ 和最右边的 $\texttt{a}$ 一定要在子串中。把子串的下标区间记作 $A$。

如果子串中还有字母 $\texttt{b}$，那么同理，最左边的 $\texttt{b}$ 和最右边的 $\texttt{b}$ 也一定要在子串中。如果这些 $b$ 的下标在 $A$ 外面，我们就需要扩大 $A$ 的范围。

如果扩大后，又需要包含其他字母呢？

为了方便分析，把上述问题用有向图建模：

- 设最左边的 $\texttt{a}$ 和最右边的 $\texttt{a}$ 对应的下标区间为 $A$。
- 如果区间 $A$ 包含字母 $\texttt{b}$，那么连一条从 $\texttt{a}$ 到 $\texttt{b}$ 的**有向边**。预处理字母的下标列表，在列表中二分查找，可以判断区间是否包含某个字母。
- 为什么不是无向边？例如 $s=\texttt{aba}$，那么 $\texttt{b}$ 对应的区间并没有包含字母 $\texttt{a}$。
- 图中每个节点（字母）额外保存该字母在 $s$ 中的最左边的下标和最右边的下标。

建模后，如果子串要包含第 $i$ 个小写字母，那么最终该子串的下标区间为：

- 从第 $i$ 个小写字母开始，DFS 这个有向图，所有能访问到的点的对应区间的**并集**，即为最终子串的下标区间。

上述过程会得到**至多** $26$ 个区间，去掉其中等于 $[0,n-1]$ 的区间（因为题目要求子字符串不能是整个字符串 $s$），问题变成：

- 从这些区间中，能否选 $k$ 个互不重叠的区间？

这和 [435. 无重叠区间](https://leetcode.cn/problems/non-overlapping-intervals/) 是一样的，见 [我的题解](https://leetcode.cn/problems/non-overlapping-intervals/solutions/3077218/tan-xin-zheng-ming-pythonjavaccgojsrust-3jx4f/)。求最多可以选多少个互不重叠的区间，返回个数是否 $\ge k$。

[本题视频讲解](https://www.bilibili.com/video/BV1pmAGegEcw/?t=22m45s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxSubstringLength(self, s: str, k: int) -> bool:
        if k == 0:  # 提前返回
            return True

        # 记录每种字母的出现位置
        pos = defaultdict(list)
        for i, b in enumerate(s):
            pos[b].append(i)

        # 构建有向图
        g = defaultdict(list)
        for i, p in pos.items():
            l, r = p[0], p[-1]
            for j, q in pos.items():
                if j == i:
                    continue
                qi = bisect_left(q, l)
                # [l, r] 包含第 j 个小写字母
                if qi < len(q) and q[qi] <= r:
                    g[i].append(j)

        # 遍历有向图
        def dfs(x: str) -> None:
            nonlocal l, r
            vis.add(x)
            p = pos[x]
            l = min(l, p[0])  # 合并区间
            r = max(r, p[-1])
            for y in g[x]:
                if y not in vis:
                    dfs(y)

        intervals = []
        for i, p in pos.items():
            # 如果要包含第 i 个小写字母，最终得到的区间是什么？
            vis = set()
            l, r = inf, 0
            dfs(i)
            # 不能选整个 s，即区间 [0, n-1]
            if l > 0 or r < len(s) - 1:
                intervals.append((l, r))

        return self.maxNonoverlapIntervals(intervals) >= k

    # 435. 无重叠区间
    # 直接计算最多能选多少个区间
    def maxNonoverlapIntervals(self, intervals: List[Tuple[int, int]]) -> int:
        intervals.sort(key=lambda x: x[1])
        ans = 0
        pre_r = -1
        for l, r in intervals:
            if l > pre_r:
                ans += 1
                pre_r = r
        return ans
```

```java [sol-Java]
class Solution {
    public boolean maxSubstringLength(String s, int k) {
        if (k == 0) { // 提前返回
            return true;
        }

        int n = s.length();
        // 记录每种字母的出现位置
        List<Integer>[] pos = new ArrayList[26];
        Arrays.setAll(pos, i -> new ArrayList<>());
        for (int i = 0; i < n; i++) {
            pos[s.charAt(i) - 'a'].add(i);
        }

        // 构建有向图
        List<Integer>[] g = new ArrayList[26];
        Arrays.setAll(g, i -> new ArrayList<>());
        for (int i = 0; i < 26; i++) {
            if (pos[i].isEmpty()) {
                continue;
            }
            List<Integer> p = pos[i];
            int l = p.get(0);
            int r = p.get(p.size() - 1);
            for (int j = 0; j < 26; j++) {
                if (j == i) {
                    continue;
                }
                List<Integer> q = pos[j];
                int qi = lowerBound(q, l);
                // [l, r] 包含第 j 个小写字母
                if (qi < q.size() && q.get(qi) <= r) {
                    g[i].add(j);
                }
            }
        }

        // 遍历有向图
        List<int[]> intervals = new ArrayList<>();
        boolean[] vis = new boolean[26];
        for (int i = 0; i < 26; i++) {
            if (pos[i].isEmpty()) {
                continue;
            }
            // 如果要包含第 i 个小写字母，最终得到的区间是什么？
            Arrays.fill(vis, false);
            l = n;
            r = 0;
            dfs(i, pos, g, vis);
            // 不能选整个 s，即区间 [0, n-1]
            if (l > 0 || r < n - 1) {
                intervals.add(new int[]{l, r});
            }
        }

        return maxNonoverlapIntervals(intervals) >= k;
    }

    private int l, r;

    private void dfs(int x, List<Integer>[] pos, List<Integer>[] g, boolean[] vis) {
        vis[x] = true;
        List<Integer> p = pos[x];
        l = Math.min(l, p.get(0)); // 合并区间
        r = Math.max(r, p.get(p.size() - 1));
        for (int y : g[x]) {
            if (!vis[y]) {
                dfs(y, pos, g, vis);
            }
        }
    }

    // 435. 无重叠区间
    // 直接计算最多能选多少个区间
    private int maxNonoverlapIntervals(List<int[]> intervals) {
        intervals.sort((a, b) -> (a[1] - b[1]));
        int ans = 0;
        int preR = -1;
        for (int[] p : intervals) {
            if (p[0] > preR) {
                ans++;
                preR = p[1];
            }
        }
        return ans;
    }

    // 开区间写法
    // 请看 https://www.bilibili.com/video/BV1AP41137w7/
    private int lowerBound(List<Integer> a, int target) {
        // 开区间 (left, right)
        int left = -1;
        int right = a.size();
        while (left + 1 < right) { // 区间不为空
            // 循环不变量：
            // a[left] < target
            // a[right] >= target
            int mid = (left + right) >>> 1;
            if (a.get(mid) >= target) {
                right = mid; // 范围缩小到 (left, mid)
            } else {
                left = mid; // 范围缩小到 (mid, right)
            }
        }
        return right; // right 是最小的满足 a[right] >= target 的下标
    }
}
```

```cpp [sol-C++]
class Solution {
    // 435. 无重叠区间
    // 直接计算最多能选多少个区间
    int maxNonoverlapIntervals(vector<pair<int, int>>& intervals) {
        ranges::sort(intervals, {}, &pair<int, int>::second);
        int ans = 0, pre_r = -1;
        for (auto& [l, r] : intervals) {
            if (l > pre_r) {
                ans++;
                pre_r = r;
            }
        }
        return ans;
    }

public:
    bool maxSubstringLength(string s, int k) {
        if (k == 0) { // 提前返回
            return true;
        }

        // 记录每种字母的出现位置
        vector<int> pos[26];
        for (int i = 0; i < s.size(); i++) {
            pos[s[i] - 'a'].push_back(i);
        }

        // 构建有向图
        vector<int> g[26];
        for (int i = 0; i < 26; i++) {
            if (pos[i].empty()) {
                continue;
            }
            int l = pos[i][0], r = pos[i].back();
            for (int j = 0; j < 26; j++) {
                if (j == i) {
                    continue;
                }
                auto& q = pos[j];
                int k = ranges::lower_bound(q, l) - q.begin();
                // [l, r] 包含第 j 个小写字母
                if (k < q.size() && q[k] <= r) {
                    g[i].push_back(j);
                }
            }
        }

        // 遍历有向图
        bool vis[26];
        int l, r;
        auto dfs = [&](this auto&& dfs, int x) -> void {
            vis[x] = true;
            l = min(l, pos[x][0]); // 合并区间
            r = max(r, pos[x].back());
            for (int y : g[x]) {
                if (!vis[y]) {
                    dfs(y);
                }
            }
        };

        vector<pair<int, int>> intervals;
        for (int i = 0; i < 26; i++) {
            if (pos[i].empty()) {
                continue;
            }
            // 如果要包含第 i 个小写字母，最终得到的区间是什么？
            ranges::fill(vis, false);
            l = INT_MAX;
            r = 0;
            dfs(i);
            // 不能选整个 s，即区间 [0, n-1]
            if (l > 0 || r < s.size() - 1) {
                intervals.emplace_back(l, r);
            }
        }

        return maxNonoverlapIntervals(intervals) >= k;
    }
};
```

```go [sol-Go]
func maxSubstringLength(s string, k int) bool {
    if k == 0 { // 提前返回
        return true
    }

    // 记录每种字母的出现位置
    pos := [26][]int{}
    for i, b := range s {
        b -= 'a'
        pos[b] = append(pos[b], i)
    }

    // 构建有向图
    g := [26][]int{}
    for i, p := range pos {
        if p == nil {
            continue
        }
        l, r := p[0], p[len(p)-1]
        for j, q := range pos {
            if j == i {
                continue
            }
            k := sort.SearchInts(q, l)
            // [l,r] 包含第 j 个小写字母
            if k < len(q) && q[k] <= r {
                g[i] = append(g[i], j)
            }
        }
    }

    // 遍历有向图
    vis := [26]bool{}
    var l, r int
    var dfs func(int)
    dfs = func(x int) {
        vis[x] = true
        p := pos[x]
        l = min(l, p[0]) // 合并区间
        r = max(r, p[len(p)-1])
        for _, y := range g[x] {
            if !vis[y] {
                dfs(y)
            }
        }
    }

    intervals := [][2]int{}
    for i, p := range pos {
        if p == nil {
            continue
        }
        // 如果要包含第 i 个小写字母，最终得到的区间是什么？
        vis = [26]bool{}
        l, r = len(s), 0
        dfs(i)
        // 不能选整个 s，即区间 [0,n-1]
        if l > 0 || r < len(s)-1 {
            intervals = append(intervals, [2]int{l, r})
        }
    }

    return maxNonoverlapIntervals(intervals) >= k
}

// 435. 无重叠区间
// 直接计算最多能选多少个区间
func maxNonoverlapIntervals(intervals [][2]int) (ans int) {
    slices.SortFunc(intervals, func(a, b [2]int) int { return a[1] - b[1] })
    preR := -1
    for _, p := range intervals {
        if p[0] > preR {
            ans++
            preR = p[1]
        }
    }
    return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n+|\Sigma|^2\log n)$，其中 $n$ 是 $s$ 的长度，$|\Sigma|=26$ 是字符集合的大小。
- 空间复杂度：$\mathcal{O}(n + |\Sigma|^2)$。

更多相似题目，见下面贪心题单中的「**§2.1 不相交区间**」。

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
10. 【本题相关】[贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `二、区间贪心 / §2.1 不相交区间`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、区间贪心 / §2.1 不相交区间`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
