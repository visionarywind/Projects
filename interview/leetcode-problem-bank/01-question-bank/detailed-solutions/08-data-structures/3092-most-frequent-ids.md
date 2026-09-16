# 3092. 最高频率的 ID

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/most-frequent-ids/
- 题目 slug：`most-frequent-ids`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.6 懒删除堆
- 难度分：1793
- 外部题解来源：https://leetcode.cn/problems/most-frequent-ids/solutions/2704858/ha-xi-biao-you-xu-ji-he-pythonjavacgo-by-7brw/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：哈希表+有序集合/懒删除堆（Python/Java/C++/Go）](https://leetcode.cn/problems/most-frequent-ids/solutions/2704858/ha-xi-biao-you-xu-ji-he-pythonjavacgo-by-7brw/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`ha-xi-biao-you-xu-ji-he-pythonjavacgo-by-7brw`
- topic id：`2704858`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：哈希表 + 有序集合

做法类似 [2671. 频率跟踪器](https://leetcode.cn/problems/frequency-tracker/)：

- 用哈希表 $\textit{cnt}$ 记录 $x=\textit{nums}[i]$ 的出现次数 $\textit{cnt}[x]$（用 $\textit{freq}$ 更新出现次数）。
- 用有序集合记录 $\textit{cnt}[x]$ 的出现次数，从而可以 $\mathcal{O}(\log n)$ 知道最大的 $\textit{cnt}[x]$ 是多少。

[视频讲解](https://www.bilibili.com/video/BV1wr421h7xY/) 第三题。

```py [sol-Python3]
class Solution:
    def mostFrequentIDs(self, nums: List[int], freq: List[int]) -> List[int]:
        cnt = defaultdict(int)
        sl = SortedList()
        ans = []
        for x, f in zip(nums, freq):
            sl.discard(cnt[x])  # 多个 cnt[x] 只会移除一个
            cnt[x] += f
            sl.add(cnt[x])
            ans.append(sl[-1])
        return ans
```

```java [sol-Java]
class Solution {
    public long[] mostFrequentIDs(int[] nums, int[] freq) {
        Map<Integer, Long> cnt = new HashMap<>();
        TreeMap<Long, Integer> m = new TreeMap<>();
        int n = nums.length;
        long[] ans = new long[n];
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            if (cnt.containsKey(x) && m.containsKey(cnt.get(x)) && m.merge(cnt.get(x), -1, Integer::sum) == 0) { // --m[cnt[x]] == 0
                m.remove(cnt.get(x));
            }
            long c = cnt.merge(x, (long) freq[i], Long::sum); // cnt[x] += freq[i]
            m.merge(c, 1, Integer::sum); // ++m[cnt[x]]
            ans[i] = m.lastKey();
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<long long> mostFrequentIDs(vector<int> &nums, vector<int> &freq) {
        unordered_map<int, long long> cnt;
        multiset<long long> m;
        int n = nums.size();
        vector<long long> ans(n);
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            auto it = m.find(cnt[x]);
            if (it != m.end()) {
                m.erase(it);
            }
            cnt[x] += freq[i];
            m.insert(cnt[x]);
            ans[i] = *m.rbegin();
        }
        return ans;
    }
};
```

```go [sol-Go]
// https://pkg.go.dev/github.com/emirpasic/gods/v2@v2.0.0-alpha
func mostFrequentIDs(nums, freq []int) []int64 {
	cnt := map[int]int{}
	t := redblacktree.New[int, int]()
	ans := make([]int64, len(nums))
	for i, x := range nums {
		// 减少一次 cnt[x] 的出现次数
		node := t.GetNode(cnt[x])
		if node != nil {
			node.Value--
			if node.Value == 0 {
				t.Remove(node.Key)
			}
		}

		cnt[x] += freq[i]

		// 增加一次 cnt[x] 的出现次数
		node = t.GetNode(cnt[x])
		if node == nil {
			t.Put(cnt[x], 1)
		} else {
			node.Value++
		}
		ans[i] = int64(t.Right().Key)
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：哈希表 + 懒删除堆

也可以不用有序集合，而是用一个**最大堆**保存数对 $(\textit{cnt}[x], x)$。

在堆中查询 $\textit{cnt}[x]$ 的最大值时，如果堆顶保存的数据并不是目前实际的 $\textit{cnt}[x]$，那么就弹出堆顶。

```py [sol-Python3]
class Solution:
    def mostFrequentIDs(self, nums: List[int], freq: List[int]) -> List[int]:
        cnt = defaultdict(int)
        h = []
        ans = []
        for x, f in zip(nums, freq):
            cnt[x] += f
            heappush_max(h, (cnt[x], x))
            while h[0][0] != cnt[h[0][1]]:  # 堆顶保存的数据已经发生变化
                heappop_max(h)  # 删除
            ans.append(h[0][0])
        return ans
```

```java [sol-Java]
class Solution {
    public long[] mostFrequentIDs(int[] nums, int[] freq) {
        int n = nums.length;
        long[] ans = new long[n];
        Map<Integer, Long> cnt = new HashMap<>();
        PriorityQueue<Pair<Long, Integer>> pq = new PriorityQueue<>((a, b) -> Long.compare(b.getKey(), a.getKey()));
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            long c = cnt.merge(x, (long) freq[i], Long::sum);
            pq.add(new Pair<>(c, x));
            while (!pq.peek().getKey().equals(cnt.get(pq.peek().getValue()))) { // 堆顶保存的数据已经发生变化
                pq.poll(); // 删除
            }
            ans[i] = pq.peek().getKey();
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<long long> mostFrequentIDs(vector<int> &nums, vector<int> &freq) {
        int n = nums.size();
        vector<long long> ans(n);
        unordered_map<int, long long> cnt;
        priority_queue<pair<long long, int>> pq;
        for (int i = 0; i < n; i++) {
            int x = nums[i];
            cnt[x] += freq[i];
            pq.emplace(cnt[x], x);
            while (pq.top().first != cnt[pq.top().second]) { // 堆顶保存的数据已经发生变化
                pq.pop(); // 删除
            }
            ans[i] = pq.top().first;
        }
        return ans;
    }
};
```

```go [sol-Go]
func mostFrequentIDs(nums, freq []int) []int64 {
	ans := make([]int64, len(nums))
	cnt := make(map[int]int)
	h := hp{}
	heap.Init(&h)
	for i, x := range nums {
		cnt[x] += freq[i]
		heap.Push(&h, pair{cnt[x], x})
		for h[0].c != cnt[h[0].x] { // 堆顶保存的数据已经发生变化
			heap.Pop(&h) // 删除
		}
		ans[i] = int64(h[0].c)
	}
	return ans
}

type pair struct{ c, x int }
type hp []pair
func (h hp) Len() int           { return len(h) }
func (h hp) Less(i, j int) bool { return h[i].c > h[j].c } // 最大堆
func (h hp) Swap(i, j int)      { h[i], h[j] = h[j], h[i] }
func (h *hp) Push(v any)        { *h = append(*h, v.(pair)) }
func (h *hp) Pop() any          { a := *h; v := a[len(a)-1]; *h = a[:len(a)-1]; return v }
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 为 $\textit{nums}$ 的长度。由于至多入堆 $\mathcal{O}(n)$ 次，所以出堆也至多 $\mathcal{O}(n)$ 次，二重循环的时间复杂度为 $\mathcal{O}(n\log n)$。
- 空间复杂度：$\mathcal{O}(n)$。

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

本题来自 `五、堆（优先队列） / §5.6 懒删除堆`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.6 懒删除堆`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
