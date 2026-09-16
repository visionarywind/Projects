# 3414. 不重叠区间的最大得分

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-score-of-non-overlapping-intervals/
- 题目 slug：`maximum-score-of-non-overlapping-intervals`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.2 不相交区间
- 难度分：2723
- 外部题解来源：https://leetcode.cn/problems/maximum-score-of-non-overlapping-intervals/solutions/3039058/dong-tai-gui-hua-er-fen-cha-zhao-you-hua-wmuy/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[动态规划 + 二分查找优化（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-score-of-non-overlapping-intervals/solutions/3039058/dong-tai-gui-hua-er-fen-cha-zhao-you-hua-wmuy/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`dong-tai-gui-hua-er-fen-cha-zhao-you-hua-wmuy`
- topic id：`3039058`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题是 [1235. 规划兼职工作](https://leetcode.cn/problems/maximum-profit-in-job-scheduling/) 的带约束版本，请先完成那题，并阅读 [我的题解](https://leetcode.cn/problems/maximum-profit-in-job-scheduling/solutions/1913089/dong-tai-gui-hua-er-fen-cha-zhao-you-hua-zkcg/)。

本题约束至多选 $4$ 个区间，那么在 1235 题的基础上，额外加个参数（维度）$j$，即定义 $f[i+1][j]$ 表示在下标 $[0,i]$ 中选**至多** $j$ 个不重叠区间的最大总和。（这些区间已按照右端点排序）

和 1235 题一样，状态转移方程为

$$
f[i+1][j] = \max(f[i][j], f[k+1][j-1]+\textit{weight}_i)
$$

其中 $k$ 是最大的满足右端点严格小于第 $i$ 个区间左端点的区间下标，不存在时为 $-1$。

初始值 $f[0][j]=0$，答案为 $f[n][4]$。

本题需要额外维护所选区间的字典序最小下标序列。我们可以在 $f$ 中额外维护下标数组，并修改计算 $\max$ 的逻辑，具体见代码。

具体请看 [视频讲解](https://www.bilibili.com/video/BV18srKYLEd8/?t=28m55s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maximumWeight(self, intervals: List[List[int]]) -> List[int]:
        a = [(r, l, weight, i) for i, (l, r, weight) in enumerate(intervals)]
        a.sort(key=lambda t: t[0])  # 按照右端点排序
        f = [[(0, []) for _ in range(5)] for _ in range(len(intervals) + 1)]
        for i, (r, l, weight, idx) in enumerate(a):
            k = bisect_left(a, (l,), hi=i)  # hi=i 表示二分上界为 i（默认为 n）
            for j in range(1, 5):
                # 为什么是 f[k] 不是 f[k+1]：上面算的是 >= l，-1 后得到 < l，但由于还要 +1，抵消了
                s2, id2 = f[k][j - 1]
                # 注意这里是减去 weight，这样取 min 后相当于计算的是最大和
                f[i + 1][j] = min(f[i][j], (s2 - weight, sorted(id2 + [idx])))
        return f[-1][4][1]
```

```java [sol-Java]
class Solution {
    private record Tuple(int l, int r, int weight, int i) {}

    private record Pair(long sum, List<Integer> id) {}

    public int[] maximumWeight(List<List<Integer>> intervals) {
        int n = intervals.size();
        Tuple[] a = new Tuple[n];
        for (int i = 0; i < n; i++) {
            List<Integer> interval = intervals.get(i);
            a[i] = new Tuple(interval.get(0), interval.get(1), interval.get(2), i);
        }
        Arrays.sort(a, (p, q) -> p.r - q.r);

        Pair[][] f = new Pair[n + 1][5];
        Arrays.setAll(f[0], i -> new Pair(0, new ArrayList<>()));
        for (int i = 0; i < n; i++) {
            Tuple t = a[i];
            int k = search(a, i, t.l);
            f[i + 1][0] = new Pair(0, new ArrayList<>());
            for (int j = 1; j < 5; j++) {
                long s1 = f[i][j].sum;
                long s2 = f[k + 1][j - 1].sum + t.weight;
                if (s1 > s2) {
                    f[i + 1][j] = f[i][j];
                    continue;
                }
                List<Integer> newId = new ArrayList<>(f[k + 1][j - 1].id);
                newId.add(t.i);
                Collections.sort(newId);
                if (s1 == s2 && compareLists(f[i][j].id, newId) < 0) {
                    newId = f[i][j].id;
                }
                f[i + 1][j] = new Pair(s2, newId);
            }
        }
        return f[n][4].id.stream().mapToInt(v -> v).toArray();
    }

    // 返回 r < upper 的最大下标
    private int search(Tuple[] a, int right, int upper) {
        int left = -1;
        while (left + 1 < right) {
            int mid = (left + right) >>> 1;
            if (a[mid].r < upper) {
                left = mid;
            } else {
                right = mid;
            }
        }
        return left;
    }

    // 比较两个 List 的字典序
    private int compareLists(List<Integer> a, List<Integer> b) {
        for (int i = 0; i < Math.min(a.size(), b.size()); i++) {
            if (!a.get(i).equals(b.get(i))) {
                return a.get(i) - b.get(i);
            }
        }
        return a.size() - b.size();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> maximumWeight(vector<vector<int>>& intervals) {
        int n = intervals.size();
        struct tuple { int l, r, weight, i; };
        vector<tuple> a(n);
        for (int i = 0; i < n; i++) {
            a[i] = {intervals[i][0], intervals[i][1], intervals[i][2], i};
        }
        ranges::sort(a, {}, &tuple::r);

        vector<array<pair<long long, vector<int>>, 5>> f(n + 1);
        for (int i = 0; i < n; i++) {
            auto [l, r, weight, idx] = a[i];
            int k = lower_bound(a.begin(), a.begin() + i, l, [](tuple& t, int val) { return t.r < val; }) - a.begin();
            for (int j = 1; j < 5; j++) {
                long long s1 = f[i][j].first;
			    // 为什么是 f[k] 不是 f[k+1]：上面算的是 >= l，-1 后得到 < l，但由于还要 +1，抵消了
                long long s2 = f[k][j - 1].first + weight;
                if (s1 > s2) {
                    f[i + 1][j] = f[i][j];
                    continue;
                }
                vector<int> new_id = f[k][j - 1].second;
                new_id.push_back(idx);
                ranges::sort(new_id);
                if (s1 == s2 && f[i][j].second < new_id) {
                    new_id = f[i][j].second;
                }
                f[i + 1][j] = {s2, new_id};
            }
        }
        return f[n][4].second;
    }
};
```

```cpp [sol-C++ 写法二]
class Solution {
public:
    vector<int> maximumWeight(vector<vector<int>>& intervals) {
        int n = intervals.size();
        struct tuple { int l, r, weight, i; };
        vector<tuple> a(n);
        for (int i = 0; i < n; i++) {
            a[i] = {intervals[i][0], intervals[i][1], intervals[i][2], i};
        }
        ranges::sort(a, {}, &tuple::r);

        vector<array<pair<long long, vector<int>>, 5>> f(n + 1);
        for (int i = 0; i < n; i++) {
            auto [l, r, weight, idx] = a[i];
            int k = lower_bound(a.begin(), a.begin() + i, l, [](tuple& t, int val) { return t.r < val; }) - a.begin();
            for (int j = 1; j < 5; j++) {
                auto p = f[k][j - 1]; // 注意这会拷贝一份 pair
                p.first -= weight; // 注意这里是减去 weight，这样取 min 后相当于计算的是最大和
                p.second.push_back(idx);
                ranges::sort(p.second);
                f[i + 1][j] = min(f[i][j], p);
            }
        }
        return f[n][4].second;
    }
};
```

```go [sol-Go]
func maximumWeight(intervals [][]int) []int {
	type tuple struct{ l, r, weight, i int }
	a := make([]tuple, len(intervals))
	for i, interval := range intervals {
		a[i] = tuple{interval[0], interval[1], interval[2], i}
	}
	slices.SortFunc(a, func(a, b tuple) int { return a.r - b.r })

	n := len(intervals)
	type pair struct {
		sum int
		id  []int
	}
	f := make([][5]pair, n+1)
	for i, t := range a {
		k := sort.Search(i, func(k int) bool { return a[k].r >= t.l })
		for j := 1; j < 5; j++ {
			s1 := f[i][j].sum
			// 为什么是 f[k] 不是 f[k+1]：上面算的是 >= t.l，-1 后得到 < t.l，但由于还要 +1，抵消了
			s2 := f[k][j-1].sum + t.weight
			if s1 > s2 {
				f[i+1][j] = f[i][j]
				continue
			}
			newId := slices.Clone(f[k][j-1].id)
			newId = append(newId, t.i)
			slices.Sort(newId)
			if s1 == s2 && slices.Compare(f[i][j].id, newId) < 0 {
				newId = f[i][j].id
			}
			f[i+1][j] = pair{s2, newId}
		}
	}
	return f[n][4].id
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n(\log n + k^2))$，其中 $n$ 是 $\textit{nums}$ 的长度，$k=4$。注意下标排序是 $\mathcal{O}(k)$ 的，因为最后一个数前面的数都是有序的，而标准库在处理小数组时，用到的排序是插入排序，在这种情况下插入排序是 $\mathcal{O}(k)$ 的。
- 空间复杂度：$\mathcal{O}(nk^2)$。有 $\mathcal{O}(nk)$ 个状态，每个状态需要 $\mathcal{O}(k)$ 的空间保存下标列表。

更多相似题目，见下面动态规划题单中的「**§6.4 不相交区间**」和「**§7.5 多维 DP**」。

## 分类题单

[如何科学刷题？](https://leetcode.cn/circle/discuss/RvFUtj/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/circle/discuss/0viNMK/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/circle/discuss/SqopEo/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/circle/discuss/9oZFK9/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/circle/discuss/YiXPXW/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/circle/discuss/dHn9Vk/)
6. [图论算法（DFS/BFS/拓扑排序/最短路/最小生成树/二分图/基环树/欧拉路径）](https://leetcode.cn/circle/discuss/01LUak/)
7. 【本题相关】[动态规划（入门/背包/状态机/划分/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/circle/discuss/tXLS3i/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/circle/discuss/mOr1u6/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/circle/discuss/IYT3ss/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

## 本地原创解析

### 1. 题意重述

本题来自 `七、其他线性 DP / §7.2 不相交区间`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、其他线性 DP / §7.2 不相交区间`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
