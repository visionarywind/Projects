# 2406. 将区间分为最少组数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/divide-intervals-into-minimum-number-of-groups/
- 题目 slug：`divide-intervals-into-minimum-number-of-groups`
- 来源专题：贪心与思维
- 来源分类路径：二、区间贪心 / §2.2 区间分组
- 难度分：1713
- 外部题解来源：https://leetcode.cn/problems/divide-intervals-into-minimum-number-of-groups/solutions/1816294/by-endlesscheng-ze3t/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[一题双解：贪心+堆 / 差分](https://leetcode.cn/problems/divide-intervals-into-minimum-number-of-groups/solutions/1816294/by-endlesscheng-ze3t/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-ze3t`
- topic id：`1816294`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1it4y1L7kL) 已出炉，欢迎点赞三连，在评论区分享你对这场周赛的看法~

---
 
按照 $\textit{left}$ 排序后，用最小堆模拟，堆顶存储每个组最后一个区间的 $\textit{right}$。

遍历区间：

- 如果当前的 $\textit{left}$ 大于堆顶，则可以接在这个组的末尾，更新堆顶为 $\textit{right}$；
- 否则需要创建一个新的组。

```py [sol1-Python3]
class Solution:
    def minGroups(self, intervals: List[List[int]]) -> int:
        intervals.sort(key=lambda p: p[0])
        h = []
        for left, right in intervals:
            if h and left > h[0]: heapreplace(h, right)
            else: heappush(h, right)
        return len(h)
```

```java [sol1-Java]
class Solution {
    public int minGroups(int[][] intervals) {
        Arrays.sort(intervals, (a, b) -> a[0] - b[0]);
        var pq = new PriorityQueue<Integer>();
        for (var p : intervals) {
            if (!pq.isEmpty() && pq.peek() < p[0]) pq.poll();
            pq.offer(p[1]);
        }
        return pq.size();
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int minGroups(vector<vector<int>> &intervals) {
        sort(intervals.begin(), intervals.end(), [](auto &a, auto &b) { return a[0] < b[0]; });
        priority_queue<int, vector<int>, greater<>> pq;
        for (auto &p : intervals) {
            if (!pq.empty() && pq.top() < p[0]) pq.pop();
            pq.push(p[1]);
        }
        return pq.size();
    }
};
```

```go [sol1-Go]
func minGroups(intervals [][]int) int {
	h := hp{}
	sort.Slice(intervals, func(i, j int) bool { return intervals[i][0] < intervals[j][0] })
	for _, p := range intervals {
		if h.Len() == 0 || p[0] <= h.IntSlice[0] {
			heap.Push(&h, p[1])
		} else {
			h.IntSlice[0] = p[1]
			heap.Fix(&h, 0)
		}
	}
	return h.Len()
}

type hp struct{ sort.IntSlice }
func (h *hp) Push(v interface{}) { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() interface{}   { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
```

另外一种思路是转换成上下车模型，每个区间看成一个人，他在 $\textit{left}$ 时刻上车，$\textit{right}+1$ 时刻下车，最后答案为同时在车上的人数的最大值。

这可以用差分数组实现，下面代码用的平衡树，方便从小到大计算。

```cpp [sol2-C++]
class Solution {
public:
    int minGroups(vector<vector<int>> &intervals) {
        map<int, int> diff;
        for (auto &p : intervals)
            ++diff[p[0]], --diff[p[1] + 1];
        int ans = 0, sum = 0;
        for (auto &[_, d] : diff)
            ans = max(ans, sum += d);
        return ans;
    }
};
```

#### 复杂度分析

- 时间复杂度：$O(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$O(n)$。

#### 思考题

从所有满足 $1\le\textit{left}\le\textit{right}\le m$ 的一共 $\dfrac{m(m+1)}{2}$ 个区间中，随机选择 $n$ 个区间作为本题的输入，得到的答案的期望是多少？

## 本地原创解析

### 1. 题意重述

本题来自 `二、区间贪心 / §2.2 区间分组`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `二、区间贪心 / §2.2 区间分组`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
