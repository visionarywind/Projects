# 2343. 裁剪数字后查询第 K 小的数字

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/query-kth-smallest-trimmed-number/
- 题目 slug：`query-kth-smallest-trimmed-number`
- 来源专题：常用数据结构
- 来源分类路径：专题：离线算法
- 难度分：1652
- 外部题解来源：https://leetcode.cn/problems/query-kth-smallest-trimmed-number/solutions/1676180/pai-xu-by-endlesscheng-ylft/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种做法：直接排序 / 离线+增量排序（Python/Java/C++/Go）](https://leetcode.cn/problems/query-kth-smallest-trimmed-number/solutions/1676180/pai-xu-by-endlesscheng-ylft/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`pai-xu-by-endlesscheng-ylft`
- topic id：`1676180`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

本题 [视频讲解](https://www.bilibili.com/video/BV1GV4y1J7kc) 已出炉，欢迎点赞三连~

---

## 方法一：直接排序

对每个询问，按照题目要求排序，取第 $k$ 小的元素的下标。

#### 复杂度分析

- 时间复杂度：$O(qmn\log n)$，其中 $q$ 是数组 $\textit{queries}$ 的长度，$n$ 是数组 $\textit{nums}$ 的长度，$m$ 是每个 $\textit{nums}[i]$ 的长度。每次询问，都需要对一个长为 $n$ 的数组排序，排序共发生 $O(n\log n)$ 次比较，每次比较的耗时为 $O(m)$，故总的时间复杂度为 $O(qmn\log n)$。
- 空间复杂度：$O(n)$。返回值不计入空间复杂度。

```py [sol1-Python3]
class Solution:
    def smallestTrimmedNumbers(self, nums: List[str], queries: List[List[int]]) -> List[int]:
        return [sorted((s[-trim:], i) for i, s in enumerate(nums))[k - 1][1] for k, trim in queries]
```

```java [sol1-Java]
class Solution {
    public int[] smallestTrimmedNumbers(String[] nums, int[][] queries) {
        var ans = new int[queries.length];
        var m = nums[0].length();
        for (var p = 0; p < queries.length; p++) {
            var q = queries[p];
            var idx = new ArrayList<>(Arrays.asList(IntStream.range(0, nums.length).boxed().toArray(Integer[]::new)));
            Collections.sort(idx, (i, j) -> nums[i].substring(m - q[1]).compareTo(nums[j].substring(m - q[1]))); // 稳定排序
            ans[p] = idx.get(q[0] - 1);
        }
        return ans;
    }
}
```

```cpp [sol1-C++]
class Solution {
public:
    vector<int> smallestTrimmedNumbers(vector<string> &nums, vector<vector<int>> &queries) {
        vector<int> ans(queries.size());
        int n = nums.size(), m = nums[0].length();
        int idx[n];
        for (int i = 0; i < queries.size(); ++i) {
            auto &q = queries[i];
            iota(idx, idx + n, 0);
            stable_sort(idx, idx + n, [&](int a, int b) {
                auto &s = nums[a], &t = nums[b];
                for (int j = m - q[1]; j < m; ++j)
                    if (s[j] != t[j]) return s[j] < t[j];
                return false;
            });
            ans[i] = idx[q[0] - 1];
        }
        return ans;
    }
};
```

```go [sol1-Go]
func smallestTrimmedNumbers(nums []string, queries [][]int) []int {
	ans := make([]int, len(queries))
	type pair struct { s string; i int }
	ps := make([]pair, len(nums))
	for i, q := range queries {
		for j, s := range nums {
			ps[j] = pair{s[len(s)-q[1]:], j}
		}
		// 也可以用稳定排序，但是要慢一些 sort.SliceStable(ps, func(i, j int) bool { return ps[i].s < ps[j].s })
		sort.Slice(ps, func(i, j int) bool { a, b := ps[i], ps[j]; return a.s < b.s || a.s == b.s && a.i < b.i })
		ans[i] = ps[q[0]-1].i
	}
	return ans
}
```

## 方法二：离线 + 增量排序

方法一排序时，每次都需要比较整个字符串。若采用增量排序，每次排序只需要比较单个字符。

具体来说，先将询问按照 $\textit{trim}$ 从小到大排序，并按照这一顺序回答询问。随着 $\textit{tirm}$ 的不断增加，我们可以在排好序的字符串数组的基础上，向每个字符串的前面添加一个对应的字符，由于字符串数组已经是有序的，我们只需要比较这个新增的字符的大小，即可比较整个字符串的大小。

#### 复杂度分析

- 时间复杂度：$O(q\log q+mn\log n)$ 或 $O(q\log q+mn)$。对询问排序需要 $O(q\log q)$，后面至多排序 $m$ 次，每次排序的时间复杂度为 $O(n\log n)$，故总的时间复杂度为 $O(q\log q+mn\log n)$。如果用基数排序可以做到 $O(q\log q+mn)$。
- 空间复杂度：$O(q+n)$。

```py [sol2-Python3]
class Solution:
    def smallestTrimmedNumbers(self, nums: List[str], queries: List[List[int]]) -> List[int]:
        idx = list(range(len(nums)))
        ans, j = [0] * len(queries), 1
        for qi, (k, trim) in sorted(enumerate(queries), key=lambda q: q[1][1]):  # 按 trim 排序
            while j <= trim:
                idx.sort(key=lambda i: nums[i][-j])  # 只比较倒数第 j 个字符的大小
                j += 1
            ans[qi] = idx[k - 1]
        return ans
```

```java [sol2-Java]
class Solution {
    public int[] smallestTrimmedNumbers(String[] nums, int[][] queries) {
        var qid = IntStream.range(0, queries.length).boxed().toArray(Integer[]::new);
        Arrays.sort(qid, (i, j) -> queries[i][1] - queries[j][1]);

        var m = nums[0].length();
        var idx = new ArrayList<>(Arrays.asList(IntStream.range(0, nums.length).boxed().toArray(Integer[]::new)));

        var ans = new int[queries.length];
        var p = 1;
        for (var qi : qid) {
            var q = queries[qi];
            while (p <= q[1]) {
                final var pp = p++;
                Collections.sort(idx, (i, j) -> nums[i].charAt(m - pp) - nums[j].charAt(m - pp)); // 稳定排序
            }
            ans[qi] = idx.get(q[0] - 1);
        }
        return ans;
    }
}
```

```cpp [sol2-C++]
class Solution {
public:
    vector<int> smallestTrimmedNumbers(vector<string> &nums, vector<vector<int>> &queries) {
        int nq = queries.size();
        int qid[nq];
        iota(qid, qid + nq, 0);
        sort(qid, qid + nq, [&](int a, int b) { return queries[a][1] < queries[b][1]; });

        int n = nums.size(), m = nums[0].length();
        int idx[n];
        iota(idx, idx + n, 0);

        vector<int> ans(nq);
        int p = 1;
        for (int qi : qid) {
            auto &q = queries[qi];
            for (; p <= q[1]; ++p)
                stable_sort(idx, idx + n, [&](int a, int b) { return nums[a][m - p] < nums[b][m - p]; });
            ans[qi] = idx[q[0] - 1];
        }
        return ans;
    }
};
```

```go [sol2-Go]
func smallestTrimmedNumbers(nums []string, queries [][]int) (ans []int) {
	for i, q := range queries {
		q[0] |= i << 32 // 把询问的下标整合到 k 里面，相比 append 到 q 里面可以避免扩容
	}
	sort.Slice(queries, func(i, j int) bool { return queries[i][1] < queries[j][1] }) // 按 trim 排序

	m := len(nums[0])
	type pair struct { s string; i int }
	ps := make([]pair, len(nums))
	for i, s := range nums {
		ps[i] = pair{s, i}
	}

	ans = make([]int, len(queries))
	p := 1
	for _, q := range queries {
		for ; p <= q[1]; p++ {
			sort.SliceStable(ps, func(i, j int) bool { return ps[i].s[m-p] < ps[j].s[m-p] }) // 只比较第 m-p 个字符的大小
		}
		ans[q[0]>>32] = ps[q[0]&math.MaxUint32-1].i
	}
	return
}
```

## 本地原创解析

### 1. 题意重述

本题来自 `专题：离线算法`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `专题：离线算法`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
