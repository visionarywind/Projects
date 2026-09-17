# 3362. 零数组变换 III

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/zero-array-transformation-iii/
- 题目 slug：`zero-array-transformation-iii`
- 来源专题：贪心与思维
- 来源分类路径：一、贪心策略 / §1.9 反悔贪心
- 难度分：2424
- 外部题解来源：https://leetcode.cn/problems/zero-array-transformation-iii/solutions/2998650/tan-xin-zui-da-dui-chai-fen-shu-zu-pytho-35o6/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心+最大堆+差分数组（Python/Java/C++/Go）](https://leetcode.cn/problems/zero-array-transformation-iii/solutions/2998650/tan-xin-zui-da-dui-chai-fen-shu-zu-pytho-35o6/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-zui-da-dui-chai-fen-shu-zu-pytho-35o6`
- topic id：`2998650`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 10:54:06 +0800

**前置题目**：[3355. 零数组变换 I](https://leetcode.cn/problems/zero-array-transformation-i/)

以 $\textit{nums}=[2,0,2,0,2]$ 为例。

从左到右遍历数组。对于 $\textit{nums}[0]=2$ 来说，我们必须从 $\textit{queries}$ 中选两个左端点为 $0$ 的区间。选哪两个呢？

贪心地想，**区间的右端点越大越好**，后面的元素越小，后续操作就越少。所以选两个左端点为 $0$，且右端点最大的区间。

继续遍历，由于 $\textit{nums}[1]=0$，可以直接跳过。

继续遍历，如果 $\textit{nums}[2]$ 仍然大于 $0$，我们需要从左端点 $\le 2$ 的未选区间中，选择右端点最大的区间。

这启发我们用**最大堆**维护左端点 $\le i$ 的未选区间的右端点。

剩下的就是用**差分数组**去维护区间减一了。你需要先完成 [3355. 零数组变换 I](https://leetcode.cn/problems/zero-array-transformation-i/) 这题。

最终堆的大小（剩余没有使用的区间个数）就是答案。

代码实现时，还需要把 $\textit{queries}$ 按照左端点排序，这样我们可以用双指针遍历 $\textit{nums}$ 和左端点 $\le i$ 的区间。

具体请看 [视频讲解](https://www.bilibili.com/video/BV1uzBxYoEJC/?t=4m40s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maxRemoval(self, nums: List[int], queries: List[List[int]]) -> int:
        queries.sort(key=lambda q: q[0])  # 按照左端点从小到大排序
        h = []
        diff = [0] * (len(nums) + 1)
        sum_d = j = 0
        for i, x in enumerate(nums):
            sum_d += diff[i]
            # 维护左端点 <= i 的区间
            while j < len(queries) and queries[j][0] <= i:
                heappush(h, -queries[j][1])  # 取相反数表示最大堆
                j += 1
            # 选择右端点最大的区间
            while sum_d < x and h and -h[0] >= i:
                sum_d += 1
                diff[-heappop(h) + 1] -= 1
            if sum_d < x:
                return -1
        return len(h)
```

```java [sol-Java]
class Solution {
    public int maxRemoval(int[] nums, int[][] queries) {
        Arrays.sort(queries, (a, b) -> a[0] - b[0]);
        PriorityQueue<Integer> pq = new PriorityQueue<>((a, b) -> b - a);
        int n = nums.length;
        int[] diff = new int[n + 1];
        int sumD = 0;
        int j = 0;
        for (int i = 0; i < n; i++) {
            sumD += diff[i];
            // 维护左端点 <= i 的区间
            while (j < queries.length && queries[j][0] <= i) {
                pq.add(queries[j][1]);
                j++;
            }
            // 选择右端点最大的区间
            while (sumD < nums[i] && !pq.isEmpty() && pq.peek() >= i) {
                sumD++;
                diff[pq.poll() + 1]--;
            }
            if (sumD < nums[i]) {
                return -1;
            }
        }
        return pq.size();
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int maxRemoval(vector<int>& nums, vector<vector<int>>& queries) {
        ranges::sort(queries, {}, [](auto& q) { return q[0]; }); // 按照左端点从小到大排序
        int n = nums.size(), j = 0, sum_d = 0;
        vector<int> diff(n + 1, 0);
        priority_queue<int> pq;
        for (int i = 0; i < n; i++) {
            sum_d += diff[i];
            // 维护左端点 <= i 的区间
            while (j < queries.size() && queries[j][0] <= i) {
                pq.push(queries[j][1]);
                j++;
            }
            // 选择右端点最大的区间
            while (sum_d < nums[i] && !pq.empty() && pq.top() >= i) {
                sum_d++;
                diff[pq.top() + 1]--;
                pq.pop();
            }
            if (sum_d < nums[i]) {
                return -1;
            }
        }
        return pq.size();
    }
};
```

```go [sol-Go]
func maxRemoval(nums []int, queries [][]int) int {
	slices.SortFunc(queries, func(a, b []int) int { return a[0] - b[0] })
	h := hp{}
	diff := make([]int, len(nums)+1)
	sumD, j := 0, 0
	for i, x := range nums {
		sumD += diff[i]
		// 维护左端点 <= i 的区间
		for ; j < len(queries) && queries[j][0] <= i; j++ {
			heap.Push(&h, queries[j][1])
		}
		// 选择右端点最大的区间
		for sumD < x && h.Len() > 0 && h.IntSlice[0] >= i {
			sumD++
			diff[heap.Pop(&h).(int)+1]--
		}
		if sumD < x {
			return -1
		}
	}
	return h.Len()
}

type hp struct{ sort.IntSlice }
func (h hp) Less(i, j int) bool { return h.IntSlice[i] > h.IntSlice[j] }
func (h *hp) Push(v any)        { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() any          { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + q\log q)$，其中 $n$ 是 $\textit{nums}$ 的长度，$q$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n+q)$。

## 相似题目

[871. 最低加油次数](https://leetcode.cn/problems/minimum-number-of-refueling-stops/)

另外，可以做做下面贪心题单中的「**§1.9 反悔贪心**」。

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
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `一、贪心策略 / §1.9 反悔贪心`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、贪心策略 / §1.9 反悔贪心`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
