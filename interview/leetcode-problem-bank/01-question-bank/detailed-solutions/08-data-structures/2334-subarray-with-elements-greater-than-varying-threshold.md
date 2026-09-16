# 2334. 元素值大于变化阈值的子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/subarray-with-elements-greater-than-varying-threshold/
- 题目 slug：`subarray-with-elements-greater-than-varying-threshold`
- 来源专题：常用数据结构
- 来源分类路径：七、并查集 / §7.4 数组上的并查集
- 难度分：2381
- 外部题解来源：https://leetcode.cn/problems/subarray-with-elements-greater-than-varying-threshold/solutions/1658410/by-endlesscheng-j6pp/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种做法：并查集 / 单调栈（Python/Java/C++/Go）](https://leetcode.cn/problems/subarray-with-elements-greater-than-varying-threshold/solutions/1658410/by-endlesscheng-j6pp/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`by-endlesscheng-j6pp`
- topic id：`1658410`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1Le4y1R7xu) 已出炉，欢迎点赞三连~

---

## 方法一：并查集

#### 提示 1

数组中的元素越大越好，不妨从大往小考虑 $\textit{nums}[i]$。

#### 提示 2

子数组的长度 $k$ 越大，$\dfrac{\textit{threshold}}{k}$ 就越小，越能满足要求。

#### 提示 3

把考虑过的元素都串起来，这条链的长度就是 $k$。

于是关键在于如何动态维护每条链的长度，高效地串联两条链。

#### 提示 4

用并查集，遍历到 $\textit{nums}[i]$ 时，用并查集合并 $i$ 和 $i+1$，这样可以把连续访问过的位置串起来，同时维护链的长度。

```py [sol1-Python3]
class Solution:
    def validSubarraySize(self, nums: List[int], threshold: int) -> int:
        n = len(nums)
        fa = list(range(n + 1))
        sz = [0] * (n + 1)
        def find(x: int) -> int:
            if fa[x] != x:
                fa[x] = find(fa[x])
            return fa[x]
        for num, i in sorted(zip(nums, range(n)), reverse=True):
            j = find(i + 1)
            fa[i] = j  # 合并 i 和 i+1
            sz[j] += sz[i] + 1
            if num > threshold // sz[j]: return sz[j]
        return -1
```

```java [sol1-Java]
class Solution {
    int[] fa;

    public int validSubarraySize(int[] nums, int threshold) {
        var n = nums.length;
        fa = new int[n + 1];
        for (var i = 0; i <= n; i++) fa[i] = i;
        var sz = new int[n + 1];

        var ids = IntStream.range(0, n).boxed().toArray(Integer[]::new);
        Arrays.sort(ids, (i, j) -> nums[j] - nums[i]);
        for (var i : ids) {
            var j = find(i + 1);
            fa[i] = j; // 合并 i 和 i+1
            sz[j] += sz[i] + 1;
            if (nums[i] > threshold / sz[j]) return sz[j];
        }
        return -1;
    }

    int find(int x) {
        if (fa[x] != x) fa[x] = find(fa[x]);
        return fa[x];
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int validSubarraySize(vector<int> &nums, int threshold) {
        int n = nums.size();
        int fa[n + 1], sz[n + 1];
        iota(fa, fa + n + 1, 0);
        memset(sz, 0, sizeof(sz));
        function<int(int)> find = [&](int x) -> int { return fa[x] == x ? x : fa[x] = find(fa[x]); };

        int ids[n];
        iota(ids, ids + n, 0);
        sort(ids, ids + n, [&](int i, int j) { return nums[i] > nums[j]; });
        for (int i : ids) {
            int j = find(i + 1);
            fa[i] = j; // 合并 i 和 i+1
            sz[j] += sz[i] + 1;
            if (nums[i] > threshold / sz[j]) return sz[j];
        }
        return -1;
    }
};
```

```go [sol1-Go]
func validSubarraySize(nums []int, threshold int) int {
	n := len(nums)
	type pair struct{ v, i int }
	a := make([]pair, n)
	for i, v := range nums {
		a[i] = pair{v, i}
	}
	sort.Slice(a, func(i, j int) bool { return a[i].v > a[j].v })

	fa := make([]int, n+1)
	for i := range fa {
		fa[i] = i
	}
	sz := make([]int, n+1)
	var find func(int) int
	find = func(x int) int {
		if fa[x] != x {
			fa[x] = find(fa[x])
		}
		return fa[x]
	}
	for _, p := range a {
		i := p.i
		j := find(i + 1)
		fa[i] = j // 合并 i 和 i+1
		sz[j] += sz[i] + 1
		if p.v > threshold/sz[j] {
			return sz[j]
		}
	}
	return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：单调栈

#### 提示 1

枚举每个元素，假设它是子数组中的最小值。

#### 提示 2

子数组的左右边界最远能到哪？

#### 提示 3

用**单调栈**来计算左右边界。

不了解单调栈的同学可以看一下 [496. 下一个更大元素 I](https://leetcode.cn/problems/next-greater-element-i/)。本题求的是更小元素。

知道了左右边界也就知道了子数组的长度 $k$。

```py [sol1-Python3]
class Solution:
    def validSubarraySize(self, nums: List[int], threshold: int) -> int:
        n = len(nums)
        left, st = [-1] * n, []  # left[i] 为左侧小于 nums[i] 的最近元素位置（不存在时为 -1）
        for i, v in enumerate(nums):
            while st and nums[st[-1]] >= v: st.pop()
            if st: left[i] = st[-1]
            st.append(i)

        right, st = [n] * n, []  # right[i] 为右侧小于 nums[i] 的最近元素位置（不存在时为 n）
        for i in range(n - 1, -1, -1):
            while st and nums[st[-1]] >= nums[i]: st.pop()
            if st: right[i] = st[-1]
            st.append(i)

        for num, l, r in zip(nums, left, right):
            k = r - l - 1
            if num > threshold // k: return k
        return -1
```

```java [sol1-Java]
class Solution {
    public int validSubarraySize(int[] nums, int threshold) {
        var n = nums.length;
        var left = new int[n]; // left[i] 为左侧小于 nums[i] 的最近元素位置（不存在时为 -1）
        var st = new ArrayDeque<Integer>();
        for (var i = 0; i < n; i++) {
            while (!st.isEmpty() && nums[st.peek()] >= nums[i]) st.pop();
            left[i] = st.isEmpty() ? -1 : st.peek();
            st.push(i);
        }

        var right = new int[n]; // right[i] 为右侧小于 nums[i] 的最近元素位置（不存在时为 n）
        st = new ArrayDeque<>();
        for (var i = n - 1; i >= 0; i--) {
            while (!st.isEmpty() && nums[st.peek()] >= nums[i]) st.pop();
            right[i] = st.isEmpty() ? n : st.peek();
            st.push(i);
        }

        for (var i = 0; i < n; ++i) {
            var k = right[i] - left[i] - 1;
            if (nums[i] > threshold / k) return k;
        }
        return -1;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    int validSubarraySize(vector<int> &nums, int threshold) {
        int n = nums.size();
        int left[n]; // left[i] 为左侧小于 nums[i] 的最近元素位置（不存在时为 -1）
        stack<int> s;
        for (int i = 0; i < n; ++i) {
            while (!s.empty() && nums[s.top()] >= nums[i]) s.pop();
            left[i] = s.empty() ? -1 : s.top();
            s.push(i);
        }

        int right[n]; // right[i] 为右侧小于 nums[i] 的最近元素位置（不存在时为 n）
        s = stack<int>();
        for (int i = n - 1; i >= 0; --i) {
            while (!s.empty() && nums[s.top()] >= nums[i]) s.pop();
            right[i] = s.empty() ? n : s.top();
            s.push(i);
        }

        for (int i = 0; i < n; ++i) {
            int k = right[i] - left[i] - 1;
            if (nums[i] > threshold / k) return k;
        }
        return -1;
    }
};
```

```go [sol1-Go]
func validSubarraySize(nums []int, threshold int) int {
	n := len(nums)
	left := make([]int, n) // left[i] 为左侧小于 nums[i] 的最近元素位置（不存在时为 -1）
	st := []int{-1}
	for i, v := range nums {
		for len(st) > 1 && nums[st[len(st)-1]] >= v {
			st = st[:len(st)-1]
		}
		left[i] = st[len(st)-1]
		st = append(st, i)
	}

	right := make([]int, n) // right[i] 为右侧小于 nums[i] 的最近元素位置（不存在时为 n）
	st = []int{n}
	for i := n - 1; i >= 0; i-- {
		for len(st) > 1 && nums[st[len(st)-1]] >= nums[i] {
			st = st[:len(st)-1]
		}
		right[i] = st[len(st)-1]
		st = append(st, i)
	}

	for i, num := range nums {
		k := right[i] - left[i] - 1
		if num > threshold/k {
			return k
		}
	}
	return -1
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$。
- 空间复杂度：$\mathcal{O}(n)$。

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
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/circle/discuss/g6KTKL/)
11. [链表、二叉树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA/一般树）](https://leetcode.cn/circle/discuss/K0n2gO/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/circle/discuss/SJFwQI/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `七、并查集 / §7.4 数组上的并查集`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `七、并查集 / §7.4 数组上的并查集`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
