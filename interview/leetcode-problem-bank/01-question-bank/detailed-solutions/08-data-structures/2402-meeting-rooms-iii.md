# 2402. 会议室 III

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/meeting-rooms-iii/
- 题目 slug：`meeting-rooms-iii`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.2 进阶
- 难度分：2093
- 外部题解来源：https://leetcode.cn/problems/meeting-rooms-iii/solutions/1799420/shuang-dui-mo-ni-pythonjavacgo-by-endles-ctwc/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[双堆模拟（Python/Java/C++/Go）](https://leetcode.cn/problems/meeting-rooms-iii/solutions/1799420/shuang-dui-mo-ni-pythonjavacgo-by-endles-ctwc/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`shuang-dui-mo-ni-pythonjavacgo-by-endles-ctwc`
- topic id：`1799420`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

按照时间顺序模拟开会过程。

对于会议 $[\textit{start},\textit{end})$，我们需要知道：

- 在 $\textit{start}$ 时刻空闲的会议室中，编号最小的会议室。可以用一个最小堆 $\textit{idle}$ 维护空闲会议室的编号。
- 如果没有空闲的会议室呢？我们需要找最早结束的会议室。可以用一个最小堆 $\textit{using}$ 维护使用中的会议室的结束时间和编号。

这两类会议室是**互补关系**，伴随着会议的开始和结束，会议室在这两类中来回倒：

- 首先，从 $\textit{using}$ 中去掉结束时间小于等于 $\textit{start}$ 的所有会议室，将其编号添加到 $\textit{idle}$ 中。
- 然后分类讨论：
  - 如果此时有空闲的会议室，那么从 $\textit{idle}$ 中弹出编号最小的会议室，和 $\textit{end}$ 一起，添加到 $\textit{using}$ 中。
  - 否则，从 $\textit{using}$ 中弹出一个最早结束的会议室（若有多个同时结束，弹出编号最小的会议室），设其结束时间为 $e$，则我们等待了 $e-\textit{start}$ 时间，所以当前会议的结束时间为 $\textit{end} + e-\textit{start}$。

在上述模拟的过程中，每使用一个编号为 $i$ 的会议室，就把 $i$ 的出现次数加一。最后返回出现次数最大的编号（如果有多个编号的出现次数相同，返回其中最小的编号）。

注意题目保证所有会议的开始时间互不相同。

```py [sol-Python3]
class Solution:
    def mostBooked(self, n: int, meetings: List[List[int]]) -> int:
        meetings.sort(key=lambda m: m[0])

        idle = list(range(n))  # 会议室编号
        using = []  # (结束时间，会议室编号)
        cnt = [0] * n  # 会议室的开会次数

        for start, end in meetings:
            # 在 start 时刻空出来的会议室
            while using and using[0][0] <= start:
                heappush(idle, heappop(using)[1])

            if idle:  # 有空闲的会议室
                i = heappop(idle)
            else:  # 没有空闲的会议室
                e, i = heappop(using)  # 弹出一个最早结束的会议室（若有多个同时结束，弹出编号最小的会议室）
                end += e - start  # 更新当前会议的结束时间

            heappush(using, (end, i))  # 使用一个会议室
            cnt[i] += 1

        return cnt.index(max(cnt))
```

```java [sol-Java]
class Solution {
    public int mostBooked(int n, int[][] meetings) {
        Arrays.sort(meetings, (a, b) -> a[0] - b[0]);

        PriorityQueue<Integer> idle = new PriorityQueue<>(); // 会议室编号
        for (int i = 0; i < n; i++) {
            idle.offer(i);
        }
        PriorityQueue<long[]> using = new PriorityQueue<>(
            (a, b) -> a[0] != b[0] ? Long.compare(a[0], b[0]) : Long.compare(a[1], b[1])
        ); // (结束时间，会议室编号)
        int[] cnt = new int[n]; // 会议室的开会次数

        for (int[] m : meetings) {
            long start = m[0];
            long end = m[1];

            // 在 start 时刻空出来的会议室
            while (!using.isEmpty() && using.peek()[0] <= start) {
                idle.offer((int) using.poll()[1]);
            }

            int i;
            if (!idle.isEmpty()) { // 有空闲的会议室
                i = idle.poll();
            } else { // 没有空闲的会议室
                long[] p = using.poll(); // 弹出一个最早结束的会议室（若有多个同时结束，弹出编号最小的会议室）
                end += p[0] - start; // 更新当前会议的结束时间
                i = (int) p[1];
            }

            using.offer(new long[]{end, i}); // 使用一个会议室
            cnt[i]++;
        }

        int ans = 0;
        for (int i = 1; i < n; i++) {
            if (cnt[i] > cnt[ans]) {
                ans = i;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int mostBooked(int n, vector<vector<int>>& meetings) {
        ranges::sort(meetings, {}, [](auto& m) { return m[0]; });

        priority_queue<int, vector<int>, greater<>> idle; // 会议室编号
        for (int i = 0; i < n; i++) {
            idle.push(i);
        }
        priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> using_; // (结束时间，会议室编号)
        vector<int> cnt(n); // 会议室的开会次数

        for (auto& m : meetings) {
            long long start = m[0], end = m[1];

            // 在 start 时刻空出来的会议室
            while (!using_.empty() && using_.top().first <= start) {
                idle.push(using_.top().second);
                using_.pop();
            }

            int i;
            if (!idle.empty()) { // 有空闲的会议室
                i = idle.top();
                idle.pop();
            } else { // 没有空闲的会议室
                // 弹出一个最早结束的会议室（若有多个同时结束，弹出编号最小的会议室）
                auto [e, room] = using_.top();
                i = room;
                using_.pop(); 
                end += e - start; // 更新当前会议的结束时间
            }

            using_.emplace(end, i); // 使用一个会议室
            cnt[i]++;
        }

        return ranges::max_element(cnt) - cnt.begin();
    }
};
```

```go [sol-Go]
func mostBooked(n int, meetings [][]int) (ans int) {
	slices.SortFunc(meetings, func(a, b []int) int { return a[0] - b[0] })

	idle := hp{make([]int, n)} // 会议室编号
	for i := range n {
		idle.IntSlice[i] = i
	}
	using := hp2{} // (结束时间，会议室编号)
	cnt := make([]int, n) // 会议室的开会次数

	for _, m := range meetings {
		start, end := m[0], m[1]

		// 在 start 时刻空出来的会议室
		for len(using) > 0 && using[0].end <= start {
			heap.Push(&idle, heap.Pop(&using).(pair).i)
		}

		var i int
		if idle.Len() > 0 { // 有空闲的会议室
			i = heap.Pop(&idle).(int)
		} else {
			// 弹出一个最早结束的会议室（若有多个同时结束，弹出编号最小的会议室）
			p := heap.Pop(&using).(pair)
			end += p.end - start // 更新当前会议的结束时间  
			i = p.i
		}

		heap.Push(&using, pair{end, i}) // 使用一个会议室
		cnt[i]++
	}

	for i, c := range cnt {
		if c > cnt[ans] {
			ans = i
		}
	}
	return
}

type hp struct{ sort.IntSlice }
func (h *hp) Push(v any) { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() any   { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }

type pair struct{ end, i int }
type hp2 []pair
func (h hp2) Len() int { return len(h) }
func (h hp2) Less(i, j int) bool {
	a, b := h[i], h[j]
	return a.end < b.end || a.end == b.end && a.i < b.i
}
func (h hp2) Swap(i, j int) { h[i], h[j] = h[j], h[i] }
func (h *hp2) Push(v any)   { *h = append(*h, v.(pair)) }
func (h *hp2) Pop() any     { a := *h; v := a[len(a)-1]; *h = a[:len(a)-1]; return v }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log m + n + m\log n)$，其中 $m$ 是 $\textit{meetings}$ 的长度。注意每个会议至多入堆出堆各一次。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

- [1882. 使用服务器处理任务](https://leetcode.cn/problems/process-tasks-using-servers/) 1979
- [1606. 找到处理最多请求的服务器](https://leetcode.cn/problems/find-servers-that-handled-most-number-of-requests/) 2276

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

本题来自 `五、堆（优先队列） / §5.2 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.2 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
