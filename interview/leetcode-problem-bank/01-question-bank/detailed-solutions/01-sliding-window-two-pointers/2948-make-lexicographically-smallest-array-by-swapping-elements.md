# 2948. 交换得到字典序最小的数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/make-lexicographically-smallest-array-by-swapping-elements/
- 题目 slug：`make-lexicographically-smallest-array-by-swapping-elements`
- 来源专题：滑动窗口与双指针
- 来源分类路径：六、分组循环
- 难度分：2047
- 外部题解来源：https://leetcode.cn/problems/make-lexicographically-smallest-array-by-swapping-elements/solutions/2542762/pai-xu-fen-zu-xun-huan-pythonjavacgo-by-pbfey/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[结论题，三种写法，附相似题目（Python/Java/C++/Go）](https://leetcode.cn/problems/make-lexicographically-smallest-array-by-swapping-elements/solutions/2542762/pai-xu-fen-zu-xun-huan-pythonjavacgo-by-pbfey/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-fen-zu-xun-huan-pythonjavacgo-by-pbfey`
- topic id：`2542762`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

对于两个元素 $x$ 和 $y$，如果 $|x-y| \le \textit{limit}$，那么把元素视作图中的节点，在 $x$ 和 $y$ 之间连一条无向边，可以得到一个无向图。

根据 [为什么同一连通块中的元素可以随意交换？](https://zhuanlan.zhihu.com/p/2027293665970721660)这篇文章，我们可以把同一连通块中的 $\textit{nums}[i]$ 从小到大排序，再从左到右填入答案的相应位置。

> 比如交换 $4,3,2,1$ 中的 $4$ 和 $1$，如何操作？先按照 $(4,3), (3,2), (2,1)$ 的顺序操作（交换括号内的元素对），得到 $1,4,3,2$，把 $1$ 换到了最左边；然后按照 $(3,2), (4,3)$ 的顺序操作，得到 $1,3,2,4$，把 $4$ 换到了最右边；其余元素位置顺序不变。这说明我们可以交换同一连通块中的任意一对元素。

例如 $\textit{nums} = [*,2,*,3,1,*]$（$*$ 表示其他无关元素），$\textit{limit}=2$。其中 $2,3,1$ 在同一个连通块中。把元素排序，填入答案，得到 $\textit{ans} = [*,1,*,2,3,*]$。

如何找到所有连通块？

如果直接连边建图，那么复杂度是 $\mathcal{O}(n^2)$ 的，太慢了。

其实，只需要对排序后的相邻元素连边。例如 $1,2,3$，如果知道 $1$ 和 $2$ 在同一个连通块，$2$ 和 $3$ 在同一个连通块，那么 $1$ 和 $3$ 就一定在同一个连通块。

如何排序？

如果直接对 $\textit{nums}$ 排序，那么 $\textit{nums}[i]$ 的原始位置信息就丢失了。我们只能在连通块内部排序，元素不能跑到连通块外面。

可以额外创建一个下标数组 $\textit{pos}$，初始值 $\textit{pos}[i] = i$。然后对 $\textit{pos}$ 按照 $\textit{nums}[\textit{pos}[i]]$ 的值从小到大排序。

排序后，从左到右遍历 $\textit{pos}$，就相当于在从小到大遍历 $\textit{nums}$。如果发现相邻元素之差大于 $k$，那么就确定了一个新的连通块，排序元素，填入答案。

## 写法一

```py [sol-Python3]
class Solution:
    def lexicographicallySmallestArray(self, nums: List[int], limit: int) -> List[int]:
        n = len(nums)
        pos = sorted(range(n), key=lambda i: nums[i])
        # 排序后，nums[pos[i]] 是递增的

        ans = [0] * n
        start = 0
        for i, p in enumerate(pos):
            if i == n - 1 or nums[pos[i + 1]] - nums[p] > limit:  # 这一段的末尾
                # sub_pos 是 ans 中的一组空位（不一定有序）
                # 我们需要把 sub_pos 对应的 nums 中的数从小到大地填入空位（从左到右填）
                # 为了能从左到右填，需要把 sub_pos 排序
                sub_pos = sorted(pos[start: i + 1])
                for j, q in enumerate(sub_pos):
                    ans[q] = nums[pos[start + j]]
                start = i + 1
        return ans
```

```java [sol-Java]
class Solution {
    public int[] lexicographicallySmallestArray(int[] nums, int limit) {
        int n = nums.length;
        Integer[] pos = new Integer[n];
        Arrays.setAll(pos, i -> i); // 初始化 pos[i] = i
        Arrays.sort(pos, (i, j) -> nums[i] - nums[j]);
        // 排序后，nums[pos[i]] 是递增的

        int[] ans = new int[n];
        int start = 0;
        for (int i = 0; i < n; i++) {
            if (i == n - 1 || nums[pos[i + 1]] - nums[pos[i]] > limit) { // 这一段的末尾
                // subPos 是 ans 中的一组空位（不一定有序）
                // 我们需要把 subPos 对应的 nums 中的数从小到大地填入空位（从左到右填）
                // 为了能从左到右填，需要把 subPos 排序
                Integer[] subPos = Arrays.copyOfRange(pos, start, i + 1);
                Arrays.sort(subPos);
                for (int j = 0; j < subPos.length; j++) {
                    ans[subPos[j]] = nums[pos[start + j]];
                }
                start = i + 1;
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> lexicographicallySmallestArray(vector<int>& nums, int limit) {
        int n = nums.size();
        vector<int> pos(n);
        ranges::iota(pos, 0); // 初始化 pos[i] = i
        ranges::sort(pos, {}, [&](int i) { return nums[i]; });
        // 排序后，nums[pos[i]] 是递增的

        vector<int> ans(n);
        int start = 0;
        for (int i = 0; i < n; i++) {
            if (i == n - 1 || nums[pos[i + 1]] - nums[pos[i]] > limit) { // 这一段的末尾
                // sub_pos 是 ans 中的一组空位（不一定有序）
                // 我们需要把 sub_pos 对应的 nums 中的数从小到大地填入空位（从左到右填）
                // 为了能从左到右填，需要把 sub_pos 排序
                vector<int> sub_pos(pos.begin() + start, pos.begin() + i + 1);
                ranges::sort(sub_pos);
                for (int j = 0; j < sub_pos.size(); j++) {
                    ans[sub_pos[j]] = nums[pos[start + j]];
                }
                start = i + 1;
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func lexicographicallySmallestArray(nums []int, limit int) []int {
	n := len(nums)
	pos := make([]int, n)
	for i := range pos {
		pos[i] = i
	}
	slices.SortFunc(pos, func(i, j int) int { return nums[i] - nums[j] })
	// 排序后，nums[pos[i]] 是递增的

	ans := make([]int, n)
	start := 0
	for i, p := range pos {
		if i == n-1 || nums[pos[i+1]]-nums[p] > limit { // 这一段的末尾
			// subPos 是 ans 中的一组空位（不一定有序）
			// 我们需要把 subPos 对应的 nums 中的数从小到大地填入空位（从左到右填）
			// 为了能从左到右填，需要把 subPos 排序
			subPos := slices.Clone(pos[start : i+1])
			slices.Sort(subPos)
			for j, q := range subPos {
				ans[q] = nums[pos[start+j]]
			}
			start = i + 1
		}
	}
	return ans
}
```

## 写法二

记录每段的元素到 $\textit{groups}[i]$ 中，其中 $\textit{groups}[i]$ 表示段 $i$ 的元素（升序），用队列保存，方便从小到大取出。

记录 $\textit{nums}$ 的下标 $0,1,2,\ldots,n-1$ 分别属于哪个段。定义 $\textit{belong}[i]$ 表示 $\textit{nums}$ 下标 $i$ 属于编号为 $\textit{belong}[i]$ 的段。

最后遍历 $\textit{belong}$，取出 $\textit{groups}[\textit{belong}[i]]$ 的队首，即为 $\textit{ans}[i]$。

```py [sol-Python3]
class Solution:
    def lexicographicallySmallestArray(self, nums: List[int], limit: int) -> List[int]:
        n = len(nums)
        pos = sorted(range(n), key=lambda i: nums[i])
        # 排序后，nums[pos[i]] 是递增的

        groups = []
        belong = [0] * n
        for i, p in enumerate(pos):
            if i == 0 or nums[p] - nums[pos[i - 1]] > limit:
                groups.append(deque())  # 新的段
            # 保存同一段内的数据，同时记录 pos[i] 属于哪一段
            groups[-1].append(nums[p])
            belong[p] = len(groups) - 1

        ans = [0] * n
        for i, gid in enumerate(belong):
            ans[i] = groups[gid].popleft()
        return ans
```

```java [sol-Java]
class Solution {
    public int[] lexicographicallySmallestArray(int[] nums, int limit) {
        int n = nums.length;
        Integer[] pos = new Integer[n];
        Arrays.setAll(pos, i -> i); // 初始化 pos[i] = i
        Arrays.sort(pos, (i, j) -> nums[i] - nums[j]);
        // 排序后，nums[pos[i]] 是递增的

        ArrayList<ArrayDeque<Integer>> groups = new ArrayList<>();
        int[] belong = new int[n];
        for (int i = 0; i < n; i++) {
            int p = pos[i];
            if (i == 0 || nums[p] - nums[pos[i - 1]] > limit) {
                groups.add(new ArrayDeque<>()); // 新的段
            }
            // 保存同一段内的数据，同时记录 pos[i] 属于哪一段
            groups.getLast().add(nums[p]);
            belong[p] = groups.size() - 1;
        }

        int[] ans = new int[n];
        for (int i = 0; i < n; i++) {
            ans[i] = groups.get(belong[i]).poll();
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> lexicographicallySmallestArray(vector<int>& nums, int limit) {
        int n = nums.size();
        vector<int> pos(n);
        ranges::iota(pos, 0); // 初始化 pos[i] = i
        ranges::sort(pos, {}, [&](int i) { return nums[i]; });
        // 排序后，nums[pos[i]] 是递增的

        vector<queue<int>> groups;
        vector<int> belong(n);
        for (int i = 0; i < n; i++) {
            int p = pos[i];
            if (i == 0 || nums[p] - nums[pos[i - 1]] > limit) {
                groups.emplace_back(); // 新的段
            }
            // 保存同一段内的数据，同时记录 pos[i] 属于哪一段
            groups.back().push(nums[p]);
            belong[p] = groups.size() - 1;
        }

        vector<int> ans(n);
        for (int i = 0; i < n; i++) {
            auto& q = groups[belong[i]];
            ans[i] = q.front();
            q.pop();
        }
        return ans;
    }
};
```

```go [sol-Go]
func lexicographicallySmallestArray(nums []int, limit int) []int {
	n := len(nums)
	pos := make([]int, n)
	for i := range pos {
		pos[i] = i
	}
	slices.SortFunc(pos, func(i, j int) int { return nums[i] - nums[j] })
	// 排序后，nums[pos[i]] 是递增的

	groups := [][]int{}
	belong := make([]int, n)
	for i, p := range pos {
		if i == 0 || nums[p]-nums[pos[i-1]] > limit {
			groups = append(groups, []int{}) // 新的段
		}
		// 保存同一段内的数据，同时记录 pos[i] 属于哪一段
		gid := len(groups) - 1
		groups[gid] = append(groups[gid], nums[p])
		belong[p] = gid
	}

	ans := make([]int, n)
	for i, gid := range belong {
		ans[i] = groups[gid][0]
		groups[gid] = groups[gid][1:]
	}
	return ans
}
```

## 写法三

由于同一段的元素下标在 $\textit{pos}$ 中是连续的，$\textit{groups}[i]$ 只需记录这一段的开始位置。

每次取出元素后，把 $\textit{groups}[i]$ 增加一。

```py [sol-Python3]
class Solution:
    def lexicographicallySmallestArray(self, nums: List[int], limit: int) -> List[int]:
        n = len(nums)
        pos = sorted(range(n), key=lambda i: nums[i])
        # 排序后，nums[pos[i]] 是递增的

        groups = []
        belong = [0] * n
        for i, p in enumerate(pos):
            if i == 0 or nums[p] - nums[pos[i - 1]] > limit:
                groups.append(i)  # 新的段，只需记录开始下标
            # 记录 pos[i] 属于哪一段
            belong[p] = len(groups) - 1

        ans = [0] * n
        for i, gid in enumerate(belong):
            ans[i] = nums[pos[groups[gid]]]
            groups[gid] += 1
        return ans
```

```java [sol-Java]
class Solution {
    public int[] lexicographicallySmallestArray(int[] nums, int limit) {
        int n = nums.length;
        Integer[] pos = new Integer[n];
        Arrays.setAll(pos, i -> i); // 初始化 pos[i] = i
        Arrays.sort(pos, (i, j) -> nums[i] - nums[j]);
        // 排序后，nums[pos[i]] 是递增的

        ArrayList<Integer> groups = new ArrayList<>();
        int[] belong = new int[n];
        for (int i = 0; i < n; i++) {
            int p = pos[i];
            if (i == 0 || nums[p] - nums[pos[i - 1]] > limit) {
                groups.add(i); // 新的段，只需记录开始下标
            }
            // 记录 pos[i] 属于哪一段
            belong[p] = groups.size() - 1;
        }

        int[] ans = new int[n];
        for (int i = 0; i < n; i++) {
            int gid = belong[i];
            int curIdx = groups.get(gid);
            ans[i] = nums[pos[curIdx]];
            groups.set(gid, curIdx + 1);
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> lexicographicallySmallestArray(vector<int>& nums, int limit) {
        int n = nums.size();
        vector<int> pos(n);
        ranges::iota(pos, 0); // 初始化 pos[i] = i
        ranges::sort(pos, {}, [&](int i) { return nums[i]; });
        // 排序后，nums[pos[i]] 是递增的

        vector<int> groups;
        vector<int> belong(n);
        for (int i = 0; i < n; i++) {
            int p = pos[i];
            if (i == 0 || nums[p] - nums[pos[i - 1]] > limit) {
                groups.push_back(i); // 新的段，只需记录开始下标
            }
            // 记录 pos[i] 属于哪一段
            belong[p] = groups.size() - 1;
        }

        vector<int> ans(n);
        for (int i = 0; i < n; i++) {
            int& cur_idx = groups[belong[i]];
            ans[i] = nums[pos[cur_idx]];
            cur_idx++;
        }
        return ans;
    }
};
```

```go [sol-Go]
func lexicographicallySmallestArray(nums []int, limit int) []int {
	n := len(nums)
	pos := make([]int, n)
	for i := range pos {
		pos[i] = i
	}
	slices.SortFunc(pos, func(i, j int) int { return nums[i] - nums[j] })
	// 排序后，nums[pos[i]] 是递增的

	groups := []int{}
	belong := make([]int, n)
	for i, p := range pos {
		if i == 0 || nums[p]-nums[pos[i-1]] > limit {
			groups = append(groups, i) // 新的段，只需记录开始下标
		}
		// 记录 pos[i] 属于哪一段
		belong[p] = len(groups) - 1
	}

	ans := make([]int, n)
	for i, gid := range belong {
		ans[i] = nums[pos[groups[gid]]]
		groups[gid]++
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n\log n)$，其中 $n$ 是 $\textit{nums}$ 的长度。瓶颈在排序上。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

- [1202. 交换字符串中的元素](https://leetcode.cn/problems/smallest-string-with-swaps/) 1855
- [1722. 执行交换操作后的最小汉明距离](https://leetcode.cn/problems/minimize-hamming-distance-after-swap-operations/) 1892
- [3695. 交换元素后的最大交替和](https://leetcode.cn/problems/maximize-alternating-sum-using-swaps/) 1984

## 分类题单

[如何科学刷题？](https://leetcode.cn/discuss/post/3141566/ru-he-ke-xue-shua-ti-by-endlesscheng-q3yd/)

1. [滑动窗口与双指针（定长/不定长/单序列/双序列/三指针/分组循环）](https://leetcode.cn/discuss/post/3578981/ti-dan-hua-dong-chuang-kou-ding-chang-bu-rzz7/)
2. [二分算法（二分答案/最小化最大值/最大化最小值/第K小）](https://leetcode.cn/discuss/post/3579164/ti-dan-er-fen-suan-fa-er-fen-da-an-zui-x-3rqn/)
3. [单调栈（基础/矩形面积/贡献法/最小字典序）](https://leetcode.cn/discuss/post/3579480/ti-dan-dan-diao-zhan-ju-xing-xi-lie-zi-d-u4hk/)
4. [网格图（DFS/BFS/综合应用）](https://leetcode.cn/discuss/post/3580195/fen-xiang-gun-ti-dan-wang-ge-tu-dfsbfszo-l3pa/)
5. [位运算（基础/性质/拆位/试填/恒等式/思维）](https://leetcode.cn/discuss/post/3580371/fen-xiang-gun-ti-dan-wei-yun-suan-ji-chu-nth4/)
6. [图论算法（DFS/BFS/拓扑排序/基环树/最短路/最小生成树/网络流）](https://leetcode.cn/discuss/post/3581143/fen-xiang-gun-ti-dan-tu-lun-suan-fa-dfsb-qyux/)
7. [动态规划（入门/背包/划分/状态机/区间/状压/数位/数据结构优化/树形/博弈/概率期望）](https://leetcode.cn/discuss/post/3581838/fen-xiang-gun-ti-dan-dong-tai-gui-hua-ru-007o/)
8. [常用数据结构（前缀和/差分/栈/队列/堆/字典树/并查集/树状数组/线段树）](https://leetcode.cn/discuss/post/3583665/fen-xiang-gun-ti-dan-chang-yong-shu-ju-j-bvmv/)
9. [数学算法（数论/组合/概率期望/博弈/计算几何/随机算法）](https://leetcode.cn/discuss/post/3584388/fen-xiang-gun-ti-dan-shu-xue-suan-fa-shu-gcai/)
10. [贪心与思维（基本贪心策略/反悔/区间/字典序/数学/思维/脑筋急转弯/构造）](https://leetcode.cn/discuss/post/3091107/fen-xiang-gun-ti-dan-tan-xin-ji-ben-tan-k58yb/)
11. [链表、树与回溯（前后指针/快慢指针/DFS/BFS/直径/LCA）](https://leetcode.cn/discuss/post/3142882/fen-xiang-gun-ti-dan-lian-biao-er-cha-sh-6srp/)
12. [字符串（KMP/Z函数/Manacher/字符串哈希/AC自动机/后缀数组/子序列自动机）](https://leetcode.cn/discuss/post/3144832/fen-xiang-gun-ti-dan-zi-fu-chuan-kmpzhan-ugt4/)

[我的题解精选（已分类）](https://github.com/EndlessCheng/codeforces-go/blob/master/leetcode/SOLUTIONS.md)

欢迎关注 [B站@灵茶山艾府](https://space.bilibili.com/206214)

## 本地原创解析

### 1. 题意重述

本题来自 `六、分组循环`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、分组循环`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
