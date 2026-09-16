# 3275. 第 K 近障碍物查询

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/k-th-nearest-obstacle-queries/
- 题目 slug：`k-th-nearest-obstacle-queries`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.1 基础
- 难度分：1420
- 外部题解来源：https://leetcode.cn/problems/k-th-nearest-obstacle-queries/solutions/2900061/zui-da-dui-wei-hu-qian-k-xiao-pythonjava-h15x/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[最大堆维护前 k 小（Python/Java/C++/Go）](https://leetcode.cn/problems/k-th-nearest-obstacle-queries/solutions/2900061/zui-da-dui-wei-hu-qian-k-xiao-pythonjava-h15x/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zui-da-dui-wei-hu-qian-k-xiao-pythonjava-h15x`
- topic id：`2900061`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

思路类似 [347. 前 K 个高频元素](https://leetcode.cn/problems/top-k-frequent-elements/)。

维护前 $k$ 小元素，可以用**最大堆**。

遍历 $\textit{queries}$，计算点 $(x,y)$ 到原点的曼哈顿距离 $d=|x|+|y|$。

把 $d$ 入堆，如果堆大小超过 $k$，就弹出堆顶（最大的元素）。

当堆的大小等于 $k$ 时，堆顶就是第 $k$ 小的距离。

[视频讲解](https://www.bilibili.com/video/BV142Hae7E5y/) 第二题，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def resultsArray(self, queries: List[List[int]], k: int) -> List[int]:
        ans = [-1] * len(queries)
        h = []
        for i, (x, y) in enumerate(queries):
            heappush_max(h, abs(x) + abs(y))
            if len(h) > k:
                heappop_max(h)
            if len(h) == k:
                ans[i] = h[0]
        return ans
```

```java [sol-Java]
class Solution {
    public int[] resultsArray(int[][] queries, int k) {
        PriorityQueue<Integer> pq = new PriorityQueue<>((a, b) -> b - a);
        int[] ans = new int[queries.length];
        for (int i = 0; i < queries.length; i++) {
            pq.offer(Math.abs(queries[i][0]) + Math.abs(queries[i][1]));
            if (pq.size() > k) {
                pq.poll();
            }
            ans[i] = pq.size() == k ? pq.peek() : -1;
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> resultsArray(vector<vector<int>>& queries, int k) {
        vector<int> ans(queries.size(), -1);
        priority_queue<int> pq;
        for (int i = 0; i < queries.size(); i++) {
            pq.push(abs(queries[i][0]) + abs(queries[i][1]));
            if (pq.size() > k) {
                pq.pop();
            }
            if (pq.size() == k) {
                ans[i] = pq.top();
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func resultsArray(queries [][]int, k int) []int {
	ans := make([]int, len(queries))
	h := hp{}
	for i, q := range queries {
		heap.Push(&h, abs(q[0])+abs(q[1]))
		if h.Len() > k {
			heap.Pop(&h)
		}
		if h.Len() < k {
			ans[i] = -1
		} else {
			ans[i] = h.IntSlice[0]
		}
	}
	return ans
}

type hp struct{ sort.IntSlice }
func (h hp) Less(i, j int) bool { return h.IntSlice[i] > h.IntSlice[j] }
func (h *hp) Push(v any)        { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() any          { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(m\log k)$，其中 $m$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(k)$。返回值不计入。

## 优化

如果 $\textit{queries}$ 的长度 $m$ 小于 $k$，那么返回一个全为 $-1$ 的数组。

否则，答案的前 $k-1$ 项都是 $-1$。

先把前 $k$ 项入堆。

对于后面的数，如果其大于等于堆顶，则不操作堆，否则替换堆顶。

下面代码仅提供 Python 和 Go，这两个语言可以直接修改堆顶。

```py [sol-Python3]
class Solution:
    def resultsArray(self, queries: List[List[int]], k: int) -> List[int]:
        m = len(queries)
        ans = [-1] * m
        if m < k:
            return ans

        h = [abs(x) + abs(y) for x, y in queries[:k]]
        heapify_max(h)
        ans[k - 1] = h[0]

        for i in range(k, m):
            x, y = queries[i]
            heappushpop_max(h, abs(x) + abs(y))
            ans[i] = h[0]
        return ans
```

```go [sol-Go]
func resultsArray(queries [][]int, k int) []int {
	m := len(queries)
	ans := make([]int, m)
	if m < k {
		for i := range ans {
			ans[i] = -1
		}
		return ans
	}

	h := hp{make([]int, k)}
	for i, q := range queries[:k] {
		h.IntSlice[i] = abs(q[0]) + abs(q[1])
		ans[i] = -1
	}
	heap.Init(&h)
	ans[k-1] = h.IntSlice[0]

	for i := k; i < m; i++ {
		q := queries[i]
		d := abs(q[0]) + abs(q[1])
		if d < h.IntSlice[0] {
			h.IntSlice[0] = d
			heap.Fix(&h, 0)
		}
		ans[i] = h.IntSlice[0]
	}
	return ans
}

type hp struct{ sort.IntSlice }
func (h hp) Less(i, j int) bool { return h.IntSlice[i] > h.IntSlice[j] }
func (hp) Push(any)             {}
func (hp) Pop() (_ any)         { return }
func abs(x int) int { if x < 0 { return -x }; return x }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(k + (m-k)\log k)$，其中 $m$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(k)$。返回值不计入。

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

本题来自 `五、堆（优先队列） / §5.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
