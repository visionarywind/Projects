# 3321. 计算子数组的 x-sum II

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/find-x-sum-of-all-k-long-subarrays-ii/
- 题目 slug：`find-x-sum-of-all-k-long-subarrays-ii`
- 来源专题：常用数据结构
- 来源分类路径：五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）
- 难度分：2598
- 外部题解来源：https://leetcode.cn/problems/find-x-sum-of-all-k-long-subarrays-ii/solutions/2948867/liang-ge-you-xu-ji-he-wei-hu-qian-x-da-p-2rcz/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两个有序集合维护前 x 大二元组（Python/Java/C++/Go）](https://leetcode.cn/problems/find-x-sum-of-all-k-long-subarrays-ii/solutions/2948867/liang-ge-you-xu-ji-he-wei-hu-qian-x-da-p-2rcz/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`liang-ge-you-xu-ji-he-wei-hu-qian-x-da-p-2rcz`
- topic id：`2948867`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置题目**：

1. [295. 数据流的中位数](https://leetcode.cn/problems/find-median-from-data-stream/)，[我的题解](https://leetcode.cn/problems/find-median-from-data-stream/solutions/3015873/ru-he-zi-ran-yin-ru-da-xiao-dui-jian-ji-4v22k/)。
2. [480. 滑动窗口中位数](https://leetcode.cn/problems/sliding-window-median/)，[我的题解](https://leetcode.cn/problems/sliding-window-median/solutions/3628827/295-ti-lan-shan-chu-dui-pythonjavacgojsr-66ch/)。
3. [3013. 将数组分成最小总代价的子数组 II](https://leetcode.cn/problems/divide-an-array-into-subarrays-with-minimum-cost-ii/)，[我的题解](https://leetcode.cn/problems/divide-an-array-into-subarrays-with-minimum-cost-ii/solutions/2614067/liang-ge-you-xu-ji-he-wei-hu-qian-k-1-xi-zdzx/)。

在 3013 题中，我们用两个有序集合维护前 $k-1$ 小元素及其总和。

本题要维护前 $x$ 大的二元组 $(\textit{cnt}[x], x)$，以及 $\textit{cnt}[x]\cdot x$ 的总和。其中 $\textit{cnt}[x]$ 表示 $x$ 在子数组（滑动窗口）中的出现次数。

当元素进入窗口时：

1. 把 $(\textit{cnt}[x], x)$ 从有序集合中移除。
2. 把 $\textit{cnt}[x]$ 加一。
3. 把 $(\textit{cnt}[x], x)$ 加入有序集合。

当元素离开窗口时：

1. 把 $(\textit{cnt}[x], x)$ 从有序集合中移除。
2. 把 $\textit{cnt}[x]$ 减一。
3. 把 $(\textit{cnt}[x], x)$ 加入有序集合。

添加删除的同时维护 $\textit{cnt}[x]\cdot x$ 的总和。

**其余逻辑同 3013 题**。

[本题视频讲解](https://www.bilibili.com/video/BV1zU2zYiEa4/?t=32m55s)，欢迎点赞关注~

```py [sol-Python3]
from sortedcontainers import SortedList

class Solution:
    def findXSum(self, nums: List[int], k: int, x: int) -> List[int]:
        cnt = defaultdict(int)
        L = SortedList()  # 保存 tuple (出现次数，元素值)
        R = SortedList()
        sum_l = 0  # L 的元素和

        def add(val: int) -> None:
            if cnt[val] == 0:
                return
            p = (cnt[val], val)
            if L and p > L[0]:  # p 比 L 中最小的还大
                nonlocal sum_l
                sum_l += p[0] * p[1]
                L.add(p)
            else:
                R.add(p)

        def remove(val: int) -> None:
            if cnt[val] == 0:
                return
            p = (cnt[val], val)
            if p in L:
                nonlocal sum_l
                sum_l -= p[0] * p[1]
                L.remove(p)
            else:
                R.remove(p)

        def l2r() -> None:
            nonlocal sum_l
            p = L[0]
            sum_l -= p[0] * p[1]
            L.remove(p)
            R.add(p)

        def r2l() -> None:
            nonlocal sum_l
            p = R[-1]
            sum_l += p[0] * p[1]
            R.remove(p)
            L.add(p)

        ans = [0] * (len(nums) - k + 1)
        for r, in_ in enumerate(nums):
            # 添加 in_
            remove(in_)
            cnt[in_] += 1
            add(in_)

            l = r + 1 - k
            if l < 0:
                continue

            # 维护大小
            while R and len(L) < x:
                r2l()
            while len(L) > x:
                l2r()
            ans[l] = sum_l

            # 移除 out
            out = nums[l]
            remove(out)
            cnt[out] -= 1
            add(out)
        return ans
```

```java [sol-Java]
class Solution {
    private final TreeSet<int[]> L = new TreeSet<>((a, b) -> a[0] != b[0] ? a[0] - b[0] : a[1] - b[1]);
    private final TreeSet<int[]> R = new TreeSet<>(L.comparator());
    private final Map<Integer, Integer> cnt = new HashMap<>();
    private long sumL = 0;

    public long[] findXSum(int[] nums, int k, int x) {
        long[] ans = new long[nums.length - k + 1];
        for (int r = 0; r < nums.length; r++) {
            // 添加 in
            int in = nums[r];
            del(in);
            cnt.merge(in, 1, Integer::sum); // cnt[in]++
            add(in);

            int l = r + 1 - k;
            if (l < 0) {
                continue;
            }

            // 维护大小
            while (!R.isEmpty() && L.size() < x) {
                r2l();
            }
            while (L.size() > x) {
                l2r();
            }
            ans[l] = sumL;

            // 移除 out
            int out = nums[l];
            del(out);
            cnt.merge(out, -1, Integer::sum); // cnt[out]--
            add(out);
        }
        return ans;
    }

    // 添加元素
    private void add(int val) {
        int c = cnt.get(val);
        if (c == 0) {
            return;
        }
        int[] p = new int[]{c, val};
        if (!L.isEmpty() && L.comparator().compare(p, L.first()) > 0) { // p 比 L 中最小的还大
            sumL += (long) p[0] * p[1];
            L.add(p);
        } else {
            R.add(p);
        }
    }

    // 删除元素
    private void del(int val) {
        int c = cnt.getOrDefault(val, 0);
        if (c == 0) {
            return;
        }
        int[] p = new int[]{c, val};
        if (L.contains(p)) {
            sumL -= (long) p[0] * p[1];
            L.remove(p);
        } else {
            R.remove(p);
        }
    }

    // 从 L 移动一个元素到 R
    private void l2r() {
        int[] p = L.pollFirst();
        sumL -= (long) p[0] * p[1];
        R.add(p);
    }

    // 从 R 移动一个元素到 L
    private void r2l() {
        int[] p = R.pollLast();
        sumL += (long) p[0] * p[1];
        L.add(p);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<long long> findXSum(vector<int>& nums, int k, int x) {
        using pii = pair<int, int>; // 出现次数，元素值
        set<pii> L, R;
        long long sum_l = 0; // L 的元素和
        unordered_map<int, int> cnt;
        auto add = [&](int x) {
            pii p = {cnt[x], x};
            if (p.first == 0) {
                return;
            }
            if (!L.empty() && p > *L.begin()) { // p 比 L 中最小的还大
                sum_l += (long long) p.first * p.second;
                L.insert(p);
            } else {
                R.insert(p);
            }
        };
        auto del = [&](int x) {
            pii p = {cnt[x], x};
            if (p.first == 0) {
                return;
            }
            auto it = L.find(p);
            if (it != L.end()) {
                sum_l -= (long long) p.first * p.second;
                L.erase(it);
            } else {
                R.erase(p);
            }
        };
        auto l2r = [&]() {
            pii p = *L.begin();
            sum_l -= (long long) p.first * p.second;
            L.erase(p);
            R.insert(p);
        };
        auto r2l = [&]() {
            pii p = *R.rbegin();
            sum_l += (long long) p.first * p.second;
            R.erase(p);
            L.insert(p);
        };

        vector<long long> ans(nums.size() - k + 1);
        for (int r = 0; r < nums.size(); r++) {
            // 添加 in
            int in = nums[r];
            del(in);
            cnt[in]++;
            add(in);

            int l = r + 1 - k;
            if (l < 0) {
                continue;
            }

            // 维护大小
            while (!R.empty() && L.size() < x) {
                r2l();
            }
            while (L.size() > x) {
                l2r();
            }
            ans[l] = sum_l;

            // 移除 out
            int out = nums[l];
            del(out);
            cnt[out]--;
            add(out);
        }
        return ans;
    }
};
```

```go [sol-Go]
import "github.com/emirpasic/gods/v2/trees/redblacktree"

type pair struct{ c, x int } // 出现次数，元素值

func less(p, q pair) int {
	return cmp.Or(p.c-q.c, p.x-q.x)
}

func findXSum(nums []int, k, x int) []int64 {
	L := redblacktree.NewWith[pair, struct{}](less)
	R := redblacktree.NewWith[pair, struct{}](less)

	sumL := 0 // L 的元素和
	cnt := map[int]int{}
	add := func(x int) {
		p := pair{cnt[x], x}
		if p.c == 0 {
			return
		}
		if !L.Empty() && less(p, L.Left().Key) > 0 { // p 比 L 中最小的还大
			sumL += p.c * p.x
			L.Put(p, struct{}{})
		} else {
			R.Put(p, struct{}{})
		}
	}
	del := func(x int) {
		p := pair{cnt[x], x}
		if p.c == 0 {
			return
		}
		if _, ok := L.Get(p); ok {
			sumL -= p.c * p.x
			L.Remove(p)
		} else {
			R.Remove(p)
		}
	}
	l2r := func() {
		p := L.Left().Key
		sumL -= p.c * p.x
		L.Remove(p)
		R.Put(p, struct{}{})
	}
	r2l := func() {
		p := R.Right().Key
		sumL += p.c * p.x
		R.Remove(p)
		L.Put(p, struct{}{})
	}

	ans := make([]int64, len(nums)-k+1)
	for r, in := range nums {
		// 添加 in
		del(in)
		cnt[in]++
		add(in)

		l := r + 1 - k
		if l < 0 {
			continue
		}

		// 维护大小
		for !R.Empty() && L.Size() < x {
			r2l()
		}
		for L.Size() > x {
			l2r()
		}
		ans[l] = int64(sumL)

		// 移除 out
		out := nums[l]
		del(out)
		cnt[out]--
		add(out)
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log k)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 专题训练

见下面数据结构题单的「**§5.7 对顶堆**」。

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

本题来自 `五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、堆（优先队列） / §5.7 对顶堆（动态第 K 小/大）`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
