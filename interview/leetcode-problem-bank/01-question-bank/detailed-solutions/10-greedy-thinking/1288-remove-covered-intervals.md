# 1288. 删除被覆盖区间

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/remove-covered-intervals/
- 题目 slug：`remove-covered-intervals`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.6 其他区间贪心
- 难度分：Unknown
- 外部题解来源：https://leetcode.cn/problems/remove-covered-intervals/solutions/3987373/wei-shi-yao-yao-pai-xu-pythonjavaccgojsr-x0y7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[为什么要排序？（Python/Java/C++/C/Go/JS/Rust）](https://leetcode.cn/problems/remove-covered-intervals/solutions/3987373/wei-shi-yao-yao-pai-xu-pythonjavaccgojsr-x0y7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`wei-shi-yao-yao-pai-xu-pythonjavaccgojsr-x0y7`
- topic id：`3987373`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:01:04 +0800

如果区间 $[a,b)$ 被另一个区间 $[c,d)$ 覆盖，则必须满足：

- $c\le a$
- $b\le d$

这里有两个约束，不好处理。

如果按照区间**左端点从小到大**的顺序遍历 $\textit{intervals}$，那么遍历过的区间的左端点都 $\le$ 当前区间的左端点，这样就**只需比较区间右端点的大小**。

在遍历过的区间中，如果有一个区间的右端点 $\ge$ 当前区间的右端点，则当前区间被覆盖。我们可以维护遍历过的区间中的**最大右端点**。

⚠**细节**：当区间左端点相同时，为了保证算法能正确检测到小区间被大区间覆盖，我们要先遍历大区间，再遍历小区间，也就是按照区间右端点从大到小排序。

注意题目保证 $\textit{intervals}$ 的所有区间互不相同。

```py [sol-Python3]
class Solution:
    def removeCoveredIntervals(self, intervals: list[list[int]]) -> int:
        # 按区间左端点从小到大排序
        # 区间左端点相同时，按区间右端点从大到小排序，这样会先遍历大区间，再遍历被大区间覆盖的小区间
        intervals.sort(key=lambda a: (a[0], -a[1]))

        ans = 0
        max_right = 0  # 已遍历区间中的最大右端点
        for _, r in intervals:
            # 由于区间左端点是从小到大排序的，已遍历区间的左端点都 <= 当前区间的左端点
            # 如果当前区间右端点 <= max_right，说明当前区间被另一个区间覆盖，否则没被覆盖
            if r > max_right:
                max_right = r
                ans += 1  # 当前区间没被覆盖
        return ans
```

```java [sol-Java]
class Solution {
    public int removeCoveredIntervals(int[][] intervals) {
        // 按区间左端点从小到大排序
        // 区间左端点相同时，按区间右端点从大到小排序，这样会先遍历大区间，再遍历被大区间覆盖的小区间
        Arrays.sort(intervals, (a, b) -> a[0] != b[0] ? a[0] - b[0] : b[1] - a[1]);

        int ans = 0;
        int maxRight = 0; // 已遍历区间中的最大右端点
        for (int[] p : intervals) {
            // 由于区间左端点是从小到大排序的，已遍历区间的左端点都 <= 当前区间的左端点
            // 如果当前区间右端点 <= maxRight，说明当前区间被另一个区间覆盖，否则没被覆盖
            if (p[1] > maxRight) {
                maxRight = p[1];
                ans++; // 当前区间没被覆盖
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int removeCoveredIntervals(vector<vector<int>>& intervals) {
        // 按区间左端点从小到大排序
        // 区间左端点相同时，按区间右端点从大到小排序，这样会先遍历大区间，再遍历被大区间覆盖的小区间
        ranges::sort(intervals, {}, [](auto& a) { return pair(a[0], -a[1]); });

        int ans = 0;
        int max_right = 0; // 已遍历区间中的最大右端点
        for (auto& p : intervals) {
            // 由于区间左端点是从小到大排序的，已遍历区间的左端点都 <= 当前区间的左端点
            // 如果当前区间右端点 <= max_right，说明当前区间被另一个区间覆盖，否则没被覆盖
            if (p[1] > max_right) {
                max_right = p[1];
                ans++; // 当前区间没被覆盖
            }
        }
        return ans;
    }
};
```

```c [sol-C]
int cmp(const void* pa, const void* pb) {
    int* a = *(int**)pa;
    int* b = *(int**)pb;
    return a[0] != b[0] ? a[0] - b[0] : b[1] - a[1];
}

int removeCoveredIntervals(int** intervals, int intervalsSize, int* intervalsColSize) {
    // 按区间左端点从小到大排序
    // 区间左端点相同时，按区间右端点从大到小排序，这样会先遍历大区间，再遍历被大区间覆盖的小区间
    qsort(intervals, intervalsSize, sizeof(int*), cmp);

    int ans = 0;
    int max_right = 0; // 已遍历区间中的最大右端点
    for (int i = 0; i < intervalsSize; i++) {
        int r = intervals[i][1];
        // 由于区间左端点是从小到大排序的，已遍历区间的左端点都 <= 当前区间的左端点
        // 如果当前区间右端点 <= max_right，说明当前区间被另一个区间覆盖，否则没被覆盖
        if (r > max_right) {
            max_right = r;
            ans++; // 当前区间没被覆盖
        }
    }
    return ans;
}
```

```go [sol-Go]
func removeCoveredIntervals(intervals [][]int) (ans int) {
	// 按区间左端点从小到大排序
	// 区间左端点相同时，按区间右端点从大到小排序，这样会先遍历大区间，再遍历被大区间覆盖的小区间
	slices.SortFunc(intervals, func(a, b []int) int {
		return cmp.Or(a[0]-b[0], b[1]-a[1])
	})

	maxRight := 0 // 已遍历区间中的最大右端点
	for _, p := range intervals {
		// 由于区间左端点是从小到大排序的，已遍历区间的左端点都 <= 当前区间的左端点
		// 如果当前区间右端点 <= maxRight，说明当前区间被另一个区间覆盖，否则没被覆盖
		if p[1] > maxRight {
			maxRight = p[1]
			ans++ // 当前区间没被覆盖
		}
	}
	return
}
```

```js [sol-JavaScript]
var removeCoveredIntervals = function(intervals) {
    // 按区间左端点从小到大排序
    // 区间左端点相同时，按区间右端点从大到小排序，这样会先遍历大区间，再遍历被大区间覆盖的小区间
    intervals.sort((a, b) => a[0] - b[0] || b[1] - a[1]);

    let ans = 0;
    let maxRight = 0; // 已遍历区间中的最大右端点
    for (const [_, r] of intervals) {
        // 由于区间左端点是从小到大排序的，已遍历区间的左端点都 <= 当前区间的左端点
        // 如果当前区间右端点 <= maxRight，说明当前区间被另一个区间覆盖，否则没被覆盖
        if (r > maxRight) {
            maxRight = r;
            ans++; // 当前区间没被覆盖
        }
    }
    return ans;
};
```

```rust [sol-Rust]
impl Solution {
    pub fn remove_covered_intervals(mut intervals: Vec<Vec<i32>>) -> i32 {
        // 按区间左端点从小到大排序
        // 区间左端点相同时，按区间右端点从大到小排序，这样会先遍历大区间，再遍历被大区间覆盖的小区间
        intervals.sort_unstable_by_key(|a| (a[0], -a[1]));

        let mut ans = 0;
        let mut max_right = 0; // 已遍历区间中的最大右端点
        for p in intervals {
            // 由于区间左端点是从小到大排序的，已遍历区间的左端点都 <= 当前区间的左端点
            // 如果当前区间右端点 <= max_right，说明当前区间被另一个区间覆盖，否则没被覆盖
            if p[1] > max_right {
                max_right = p[1];
                ans += 1; // 当前区间没被覆盖
            }
        }
        ans
    }
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{intervals}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 专题训练

见下面贪心题单的「**二、区间贪心**」。

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

本题来自 `二、区间贪心 / §2.6 其他区间贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、区间贪心 / §2.6 其他区间贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
