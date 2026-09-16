# 2163. 删除元素后和的最小差值

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-difference-in-sums-after-removal-of-elements/
- 题目 slug：`minimum-difference-in-sums-after-removal-of-elements`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.2 进阶
- 难度分：2225
- 外部题解来源：https://leetcode.cn/problems/minimum-difference-in-sums-after-removal-of-elements/solutions/1247074/qian-zhui-zui-xiao-he-hou-zhui-zui-da-he-yz3d/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前后缀分解 + 堆（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-difference-in-sums-after-removal-of-elements/solutions/1247074/qian-zhui-zui-xiao-he-hou-zhui-zui-da-he-yz3d/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-zui-xiao-he-hou-zhui-zui-da-he-yz3d`
- topic id：`1247074`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

删除元素是障眼法，把重点放在怎么选 $2n$ 个数上。

**题意**：把 $\textit{nums}$ 分割成两部分，第一部分选 $n$ 个数求和，记作 $s_1$，第二部分选 $n$ 个数求和，记作 $s_2$。计算 $s_1-s_2$ 的最小值。

由于 $s_1$ 和 $s_2$ 互相独立，为了让 $s_1-s_2$ 尽量小，那么 $s_1$ 越小越好，$s_2$ 越大越好。

**枚举**分割位置，保证两部分都至少有 $n$ 个数。对于每个分割位置，计算最小 $s_1$ 和最大 $s_2$，所有 $s_1-s_2$ 的最小值就是答案。

一个 $n=4$ 的例子：

![](https://pic.leetcode.cn/1644495549-IzYFpw-LC2163.drawio.png)

具体地，我们需要计算出 $\textit{nums}$ 的前缀最小和 $\textit{preMin}[i]$，即 $\textit{nums}[0]$ 到 $\textit{nums}[i]$ 中最小的 $n$ 个元素之和；以及后缀最大和 $\textit{sufMax}[i]$，即 $\textit{nums}[i]$ 到 $\textit{nums}[3n-1]$ 中最大的 $n$ 个元素之和。答案为 $\textit{preMin}[i]-\textit{sufMax}[i+1]$ 中的最小值。

对于后缀最大和，可以从右往左遍历 $\textit{nums}$ 计算，思路同经典题 [703. 数据流中的第 K 大元素](https://leetcode.cn/problems/kth-largest-element-in-a-stream/)（Top K），维护一个包含 $n$ 个元素的**最小堆**（及其元素和）。不断向左遍历 $\textit{nums}$ 中的元素 $v$，若 $v$ 比堆顶元素大，则弹出堆顶，并将 $v$ 入堆，这可以让堆中元素和更大。

同理，对于前缀最小和，则需要维护一个包含 $n$ 个元素的**最大堆**（及其元素和）。不断向右遍历 $\textit{nums}$ 中的元素 $v$，若 $v$ 比堆顶元素小，则弹出堆顶，并将 $v$ 入堆，这可以让堆中元素和更小。

代码实现时，可以在计算前缀最小和的同时计算答案。

```py [sol-Python3]
class Solution:
    def minimumDifference(self, nums: List[int]) -> int:
        m = len(nums)
        n = m // 3
        min_h = nums[-n:]
        heapify(min_h)

        suf_max = [0] * (m - n + 1)  # 后缀最大和
        suf_max[-1] = sum(min_h)
        for i in range(m - n - 1, n - 1, -1):
            suf_max[i] = suf_max[i + 1] + nums[i] - heappushpop(min_h, nums[i])

        max_h = [-x for x in nums[:n]]  # 所有元素取反，表示最大堆
        heapify(max_h)

        pre_min = -sum(max_h)  # 前缀最小和
        ans = pre_min - suf_max[n]  # i=n-1 时的答案
        for i in range(n, m - n):
            pre_min += nums[i] + heappushpop(max_h, -nums[i])
            ans = min(ans, pre_min - suf_max[i + 1])
        return ans
```

```java [sol-Java]
class Solution {
    public long minimumDifference(int[] nums) {
        int m = nums.length;
        int n = m / 3;
        PriorityQueue<Integer> minPQ = new PriorityQueue<>();
        long sum = 0;
        for (int i = m - 1; i >= m - n; i--) {
            minPQ.offer(nums[i]);
            sum += nums[i];
        }

        long[] sufMax = new long[m - n + 1]; // 后缀最大和
        sufMax[m - n] = sum;
        for (int i = m - n - 1; i >= n; i--) {
            int v = nums[i];
            if (v > minPQ.peek()) {
                sum += v - minPQ.poll();
                minPQ.offer(v);
            }
            sufMax[i] = sum;
        }

        PriorityQueue<Integer> maxPQ = new PriorityQueue<>((a, b) -> b - a);
        long preMin = 0; // 前缀最小和
        for (int i = 0; i < n; ++i) {
            maxPQ.offer(nums[i]);
            preMin += nums[i];
        }

        long ans = preMin - sufMax[n]; // i=n-1 时的答案
        for (int i = n; i < m - n; i++) {
            int v = nums[i];
            if (v < maxPQ.peek()) {
                preMin += v - maxPQ.poll();
                maxPQ.offer(v);
            }
            ans = Math.min(ans, preMin - sufMax[i + 1]);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minimumDifference(vector<int>& nums) {
        int m = nums.size(), n = m / 3;

        priority_queue<int, vector<int>, greater<>> min_pq(nums.end() - n, nums.end());
        long long sum = reduce(nums.end() - n, nums.end(), 0LL);

        vector<long long> suf_max(m - n + 1); // 后缀最大和
        suf_max[m - n] = sum;
        for (int i = m - n - 1; i >= n; i--) {
            int v = nums[i];
            if (v > min_pq.top()) {
                sum += v - min_pq.top();
                min_pq.pop();
                min_pq.push(v);
            }
            suf_max[i] = sum;
        }

        priority_queue<int> max_pq(nums.begin(), nums.begin() + n);
        long long pre_min = reduce(nums.begin(), nums.begin() + n, 0LL); // 前缀最小和

        long long ans = pre_min - suf_max[n]; // i=n-1 时的答案
        for (int i = n; i < m - n; i++) {
            int v = nums[i];
            if (v < max_pq.top()) {
                pre_min += v - max_pq.top();
                max_pq.pop();
                max_pq.push(v);
            }
            ans = min(ans, pre_min - suf_max[i + 1]);
        }
        return ans;
    }
};
```

```go [sol-Go]
func minimumDifference(nums []int) int64 {
	m := len(nums)
	n := m / 3
	minH := minHeap{nums[m-n:]}
	heap.Init(&minH)
	sum := 0
	for _, v := range nums[m-n:] {
		sum += v
	}

	sufMax := make([]int, m-n+1) // 后缀最大和
	sufMax[m-n] = sum
	for i := m - n - 1; i >= n; i-- {
		if v := nums[i]; v > minH.IntSlice[0] {
			sum += v - minH.IntSlice[0]
			minH.IntSlice[0] = v
			heap.Fix(&minH, 0)
		}
		sufMax[i] = sum
	}

	maxH := maxHeap{nums[:n]}
	heap.Init(&maxH)
	preMin := 0 // 前缀最小和
	for _, v := range nums[:n] {
		preMin += v
	}

	ans := preMin - sufMax[n]
	for i := n; i < m-n; i++ { // i=n-1 时的答案
		if v := nums[i]; v < maxH.IntSlice[0] {
			preMin += v - maxH.IntSlice[0]
			maxH.IntSlice[0] = v
			heap.Fix(&maxH, 0)
		}
		ans = min(ans, preMin-sufMax[i+1])
	}
	return int64(ans)
}

type minHeap struct{ sort.IntSlice }
func (minHeap) Push(any) {}
func (minHeap) Pop() (_ any) { return }

type maxHeap struct{ sort.IntSlice }
func (h maxHeap) Less(i, j int) bool { return h.IntSlice[i] > h.IntSlice[j] }
func (maxHeap) Push(any) {}
func (maxHeap) Pop() (_ any) { return }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$。其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

下面动态规划题单的「**专题：前后缀分解**」。

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
