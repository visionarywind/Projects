# 3781. 二进制交换后的最大分数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/maximum-score-after-binary-swaps/
- 题目 slug：`maximum-score-after-binary-swaps`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.2 进阶
- 难度分：1823
- 外部题解来源：https://leetcode.cn/problems/maximum-score-after-binary-swaps/solutions/3861994/zui-xiao-dui-wei-hu-dong-tai-qian-k-da-j-n6zd/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：从左到右 / 从右到左（Python/Java/C++/Go）](https://leetcode.cn/problems/maximum-score-after-binary-swaps/solutions/3861994/zui-xiao-dui-wei-hu-dong-tai-qian-k-da-j-n6zd/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`zui-xiao-dui-wei-hu-dong-tai-qian-k-da-j-n6zd`
- topic id：`3861994`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：从左到右

把 $s[i] = \texttt{1}$ 理解成一根红色箭头，指向 $\textit{nums}[i]$。我们需要最大化箭头指向（选择）的元素之和。

一次操作相当于把一根箭头左移一个单位（如果左边有空位的话）。

示例 1 的 $\textit{nums}=[2,1,5,2,3]$，$s=\texttt{01010}$。

- 对于第一根箭头来说，它只能选择 $[2,1]$ 中的数。贪心地，选择最大的数 $2$。
- 对于第二根箭头来说，它可以选择 $[5,2]$，以及前面剩下的 $1$。贪心地，选择最大的数 $5$。
- 答案为 $2+5=7$。

我们需要一个数据结构，支持添加遍历过的元素，查询最大值，以及删除最大值（同一个元素选了就不能再选了）。最适合的数据结构是**最大堆**。

```py [sol-Python3]
class Solution:
    def maximumScore(self, nums: List[int], s: str) -> int:
        ans = 0
        h = []
        for x, ch in zip(nums, s):
            heappush(h, -x)  # 取相反数，把 h 视作最大堆
            if ch == '1':
                ans += -heappop(h)
        return ans
```

```py [sol-Python3 写法二]
class Solution:
    def maximumScore(self, nums: List[int], s: str) -> int:
        ans = 0
        h = []
        for x, ch in zip(nums, s):
            if ch == '0':
                heappush(h, -x)  # 取相反数，把 h 视作最大堆
            else:
                ans += -heappushpop(h, -x)            
        return ans
```

```java [sol-Java]
class Solution {
    public long maximumScore(int[] nums, String s) {
        PriorityQueue<Integer> pq = new PriorityQueue<>((a, b) -> b - a);
        long ans = 0;
        for (int i = 0; i < nums.length; i++) {
            pq.add(nums[i]);
            if (s.charAt(i) == '1') {
                ans += pq.poll();
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maximumScore(vector<int>& nums, string s) {
        priority_queue<int> pq;
        long long ans = 0;
        for (int i = 0; i < nums.size(); i++) {
            pq.push(nums[i]);
            if (s[i] == '1') {
                ans += pq.top();
                pq.pop();
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumScore(nums []int, s string) (ans int64) {
	h := hp{}
	for i, x := range nums {
		heap.Push(&h, x)
		if s[i] == '1' {
			ans += int64(heap.Pop(&h).(int))
		}
	}
	return
}

type hp struct{ sort.IntSlice }

func (h hp) Less(i, j int) bool { return h.IntSlice[i] > h.IntSlice[j] }
func (h *hp) Push(v any)        { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() any          { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log k_0)$，其中 $n$ 是 $\textit{nums}$ 的长度，$k_0$ 是 $s$ 中的 $\texttt{0}$ 的个数。
- 空间复杂度：$\mathcal{O}(k_0)$。

## 方法二：从右到左

示例 1 的 $\textit{nums}=[2,1,5,2,3]$，$s=\texttt{01010}$。

由于箭头只能左移，无法右移，所以最右边的箭头右侧的数 $[3]$，无法选择。

从最右边的箭头，到倒数第二个箭头（不含）之间的数呢？在示例 1 中，子数组 $b=[5,2]$ 只能被最右边的箭头选择。贪心地，选择其中最大的元素 $\max(b)=5$。如果前面发现更大的，再替换。

从倒数第二个箭头，到倒数第三个箭头（不含）之间的数呢？在示例 1 中，子数组 $b=[2,1]$ 可以被两个箭头选择：

- 倒着看，我们先选择 $1$。目前这两根箭头指向的数为 $[1,5]$。
- 继续倒着遍历，由于 $2>1$，我们舍弃最小的 $1$，改成 $2$。目前这两根箭头指向的数为 $[2,5]$。
- 假如左边还有数，比如 $9$，由于 $9>2$，我们舍弃最小的 $2$，改成 $9$。目前这两根箭头指向的数为 $[9,5]$。
- 假如左边还有数，比如 $99$，由于 $99>5$，我们舍弃最小的 $5$，改成 $99$。目前这两根箭头指向的数为 $[99,9]$。

这个过程类似维护前 $2$ 大，可以用**最小堆**实现：先把元素入堆，再把堆顶（最小值）弹出。

一般地，在倒着遍历的过程中，维护前 $k$ 大元素，其中 $k$ 是遍历过的 $s[i]=\texttt{1}$ 的个数。

- 先把 $\textit{nums}[i]$ 入堆。
- 如果 $s[i]=\texttt{1}$，无需额外操作（新增一个箭头指向的元素）；否则，把堆顶弹出。

最后返回堆中元素之和。

[本题视频讲解](https://www.bilibili.com/video/BV14LqmBMECK/?t=8m20s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def maximumScore(self, nums: List[int], s: str) -> int:
        h = []
        for x, ch in zip(reversed(nums), reversed(s)):
            heappush(h, x)
            if ch == '0':
                heappop(h)  # 更快的写法见【Python3 写法二】
        return sum(h)
```

```py [sol-Python3 写法二]
class Solution:
    def maximumScore(self, nums: List[int], s: str) -> int:
        h = []
        for x, ch in zip(reversed(nums), reversed(s)):
            if ch == '1':
                heappush(h, x)
            else:
                heappushpop(h, x)
        return sum(h)
```

```java [sol-Java]
class Solution {
    public long maximumScore(int[] nums, String s) {
        PriorityQueue<Integer> pq = new PriorityQueue<>();
        long ans = 0;
        for (int i = nums.length - 1; i >= 0; i--) {
            int x = nums[i];
            ans += x;
            pq.offer(x);
            if (s.charAt(i) == '0') {
                ans -= pq.poll();
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long maximumScore(vector<int>& nums, string s) {
        priority_queue<int, vector<int>, greater<>> pq;
        long long ans = 0;
        for (int i = nums.size() - 1; i >= 0; i--) {
            int x = nums[i];
            ans += x;
            pq.push(x);
            if (s[i] == '0') {
                ans -= pq.top();
                pq.pop();
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func maximumScore(nums []int, s string) (ans int64) {
	h := hp{}
	for i, x := range slices.Backward(nums) {
		ans += int64(x)
		heap.Push(&h, x)
		if s[i] == '0' {
			ans -= int64(heap.Pop(&h).(int))
		}
	}
	return
}

type hp struct{ sort.IntSlice }

func (h *hp) Push(v any) { h.IntSlice = append(h.IntSlice, v.(int)) }
func (h *hp) Pop() any   { a := h.IntSlice; v := a[len(a)-1]; h.IntSlice = a[:len(a)-1]; return v }
```

```go [sol-Go 写法二]
func maximumScore(nums []int, s string) (ans int64) {
	h := hp{}
	for i, x := range slices.Backward(nums) {
		if s[i] == '1' {
			ans += int64(x)
			heap.Push(&h, x)
		} else if h.Len() > 0 && x > h.IntSlice[0] {
			ans += int64(x - h.IntSlice[0])
			h.IntSlice[0] = x
			heap.Fix(&h, 0)
		}
	}
	return
}

type hp struct{ sort.IntSlice }

func (h *hp) Push(v any) { h.IntSlice = append(h.IntSlice, v.(int)) }
func (hp) Pop() (_ any)  { return }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log k_1)$，其中 $n$ 是 $\textit{nums}$ 的长度，$k_1$ 是 $s$ 中的 $\texttt{1}$ 的个数。
- 空间复杂度：$\mathcal{O}(k_1)$。

## 专题训练

见下面数据结构题单的「**五、堆（优先队列）**」。

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
