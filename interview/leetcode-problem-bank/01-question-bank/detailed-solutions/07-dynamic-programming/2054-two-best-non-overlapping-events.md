# 2054. 两个最好的不重叠活动

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/two-best-non-overlapping-events/
- 题目 slug：`two-best-non-overlapping-events`
- 来源专题：动态规划
- 来源分类路径：七、其他线性 DP / §7.2 不相交区间
- 难度分：1883
- 外部题解来源：https://leetcode.cn/problems/two-best-non-overlapping-events/solutions/1075386/yong-you-xian-dui-lie-wei-hu-ling-yi-ge-8ld3x/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[排序 + 单调栈二分（通用做法是 DP）Python/Java/C++/Go](https://leetcode.cn/problems/two-best-non-overlapping-events/solutions/1075386/yong-you-xian-dui-lie-wei-hu-ling-yi-ge-8ld3x/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`yong-you-xian-dui-lie-wei-hu-ling-yi-ge-8ld3x`
- topic id：`1075386`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

设参加的第二个活动的开始时间为 $\textit{startTime}$，那么第一个活动选哪个最好？

在结束时间小于 $\textit{startTime}$ 的活动中，选择价值最大的活动。

为了方便查找，先把 $\textit{events}$ 按照结束时间从小到大排序。

排序后，对比如下两个活动：

- 活动一：结束于 $3$ 时刻，价值 $999$。
- 活动二：结束于 $6$ 时刻，价值 $9$。

活动二的结束时间又晚，价值又小，**全方面不如活动一**，是垃圾数据，直接忽略。

换句话说，在遍历 $\textit{events}$ 的过程中（注意 $\textit{events}$ 已按照结束时间排序），**只在遇到更大价值的活动时，才记录该活动**。把这些活动记录到一个栈（列表）中，那么从栈底到栈顶，结束时间是递增的，价值也是递增的，非常适合**二分查找**。关于二分查找的原理，请看 [二分查找 红蓝染色法【基础算法精讲 04】](https://www.bilibili.com/video/BV1AP41137w7/)。

枚举第二个活动，在单调栈中二分查找结束时间严格小于 $\textit{startTime}$ 的最后一个活动，即为价值最大的第一个活动。如果没找到，那么只能选一个活动。

为了简化判断逻辑，可以在栈底加一个结束时间为 $0$，价值也为 $0$ 的哨兵。

## 写法一

```py [sol-Python3]
class Solution:
    def maxTwoEvents(self, events: List[List[int]]) -> int:
        # 按照结束时间排序
        events.sort(key=lambda e: e[1])  

        # 从栈底到栈顶，结束时间递增，价值递增
        st = [(0, 0)]  # 栈底哨兵 
        ans = 0
        for start_time, end_time, value in events:
            # 二分查找最后一个结束时间 < start_time 的活动
            i = bisect_left(st, (start_time,)) - 1
            ans = max(ans, st[i][1] + value)
            # 遇到比栈顶更大的价值，入栈
            if value > st[-1][1]:
                st.append((end_time, value))
        return ans
```

```java [sol-Java]
class Solution {
    public int maxTwoEvents(int[][] events) {
        // 按照结束时间排序
        Arrays.sort(events, (a, b) -> a[1] - b[1]);

        // 从栈底到栈顶，结束时间递增，价值递增
        ArrayList<int[]> st = new ArrayList<>(); // (结束时间, 价值)
        st.add(new int[]{0, 0}); // 栈底哨兵

        int ans = 0;
        for (int[] e : events) {
            int i = search(st, e[0]);
            int value = e[2];
            ans = Math.max(ans, st.get(i)[1] + value);
            // 遇到比栈顶更大的价值，入栈
            if (value > st.getLast()[1]) {
                st.add(new int[]{e[1], value});
            }
        }
        return ans;
    }

    // 返回最后一个满足 st[i][0] < target 的 i
    private int search(List<int[]> st, int target) {
        int left = -1, right = st.size();
        while (left + 1 < right) { // 开区间二分
            int mid = left + (right - left) / 2;
            if (st.get(mid)[0] < target) {
                left = mid;
            } else {
                right = mid;
            }
        }
        return left;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxTwoEvents(vector<vector<int>>& events) {
        // 按照结束时间排序
        ranges::sort(events, {}, [](auto& e) { return e[1]; });

        // 从栈底到栈顶，结束时间递增，价值递增
        vector<pair<int, int>> st = {{0, 0}}; // 栈底哨兵
        int ans = 0;
        for (auto& e : events) {
            int start_time = e[0], value = e[2];
            // 二分查找最后一个结束时间 < start_time 的活动
            auto it = --ranges::lower_bound(st, start_time, {}, &pair<int, int>::first);
            ans = max(ans, it->second + value);
            // 遇到比栈顶更大的价值，入栈
            if (value > st.back().second) {
                st.emplace_back(e[1], value);
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxTwoEvents(events [][]int) (ans int) {
	// 按照结束时间排序
	slices.SortFunc(events, func(a, b []int) int { return a[1] - b[1] })

	// 从栈底到栈顶，结束时间递增，价值递增
	type pair struct{ endTime, value int }
	st := []pair{{}} // 栈底哨兵
	for _, e := range events {
		startTime, value := e[0], e[2]
		// 二分查找最后一个结束时间 < startTime 的活动
		i := sort.Search(len(st), func(i int) bool { return st[i].endTime >= startTime }) - 1
		ans = max(ans, st[i].value+value)
		// 遇到比栈顶更大的价值，入栈
		if value > st[len(st)-1].value {
			st = append(st, pair{e[1], value})
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{events}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 写法二：原地

也可以把 $\textit{events}$ 当作栈。

缺点是没法用哨兵。

```py [sol-Python3]
class Solution:
    def maxTwoEvents(self, events: List[List[int]]) -> int:
        events.sort(key=lambda e: e[1])

        ans = size = 0  # 把 events 当作栈
        for start_time, end_time, value in events:
            i = bisect_left(events, (start_time,), hi=size) - 1
            if i >= 0:
                ans = max(ans, value + events[i][1])
            else:
                ans = max(ans, value)

            if size == 0 or value > events[size - 1][1]:
                events[size] = (end_time, value)
                size += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int maxTwoEvents(int[][] events) {
        Arrays.sort(events, (a, b) -> a[1] - b[1]);

        int ans = 0;
        int size = 0; // 把 events 当作栈
        for (int[] e : events) {
            int i = search(events, size, e[0]);
            int value = e[2];
            if (i >= 0) {
                ans = Math.max(ans, value + events[i][2]);
            } else {
                ans = Math.max(ans, value);
            }

            if (size == 0 || value > events[size - 1][2]) {
                events[size++] = e;
            }
        }
        return ans;
    }

    // 返回最后一个满足 st[i][1] < target 的 i
    private int search(int[][] st, int right, int target) {
        int left = -1;
        while (left + 1 < right) { // 开区间二分
            int mid = left + (right - left) / 2;
            if (st[mid][1] < target) {
                left = mid;
            } else {
                right = mid;
            }
        }
        return left;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxTwoEvents(vector<vector<int>>& events) {
        ranges::sort(events, {}, [](auto& e) { return e[1]; });

        int ans = 0, size = 0; // 把 events 当作栈
        for (auto& e : events) {
            int start_time = e[0], value = e[2];
            auto it = ranges::lower_bound(events.begin(), events.begin() + size, start_time, {}, [](auto& e) { return e[1]; });
            if (it != events.begin()) {
                ans = max(ans, value + (*--it)[2]);
            } else {
                ans = max(ans, value);
            }

            if (size == 0 || value > events[size - 1][2]) {
                events[size++] = e;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maxTwoEvents(events [][]int) (ans int) {
	slices.SortFunc(events, func(a, b []int) int { return a[1] - b[1] })

	st := events[:0] // 把 events 当作栈
	for _, e := range events {
		startTime, value := e[0], e[2]
		i := sort.Search(len(st), func(i int) bool { return st[i][1] >= startTime }) - 1
		if i >= 0 {
			ans = max(ans, value+events[i][2])
		} else {
			ans = max(ans, value)
		}
		if len(st) == 0 || value > st[len(st)-1][2] {
			st = append(st, e)
		}
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{events}$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。忽略排序的栈开销。

## 进阶问题：选至多 k 个不重叠区间

这题是 [1751. 最多可以参加的会议数目 II](https://leetcode.cn/problems/maximum-number-of-events-that-can-be-attended-ii/)，[我的题解](https://leetcode.cn/problems/maximum-number-of-events-that-can-be-attended-ii/solutions/1913087/dong-tai-gui-hua-er-fen-cha-zhao-you-hua-fuip/)。

## 专题训练

见下面动态规划题单的「**§7.2 不相交区间**」。

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
