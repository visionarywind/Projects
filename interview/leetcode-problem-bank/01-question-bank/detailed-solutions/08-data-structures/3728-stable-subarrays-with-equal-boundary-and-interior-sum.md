# 3728. 边界与内部和相等的稳定子数组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/stable-subarrays-with-equal-boundary-and-interior-sum/
- 题目 slug：`stable-subarrays-with-equal-boundary-and-interior-sum`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.2 前缀和与哈希表
- 难度分：1909
- 外部题解来源：https://leetcode.cn/problems/stable-subarrays-with-equal-boundary-and-interior-sum/solutions/3815641/qian-zhui-he-yu-ha-xi-biao-shi-zi-bian-x-d6vf/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[前缀和与哈希表 + 式子变形（Python/Java/C++/Go）](https://leetcode.cn/problems/stable-subarrays-with-equal-boundary-and-interior-sum/solutions/3815641/qian-zhui-he-yu-ha-xi-biao-shi-zi-bian-x-d6vf/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`qian-zhui-he-yu-ha-xi-biao-shi-zi-bian-x-d6vf`
- topic id：`3815641`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

**前置题目**：[560. 和为 K 的子数组](https://leetcode.cn/problems/subarray-sum-equals-k/)，[我的题解](https://leetcode.cn/problems/subarray-sum-equals-k/solutions/2781031/qian-zhui-he-ha-xi-biao-cong-liang-ci-bi-4mwr/)。

设 $\textit{capacity}$ 的前缀和数组为 $s$。关于 $s$ 的定义，见 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

题目要求的式子，等价于

$$
\textit{capacity}[l] = \textit{capacity}[r] = s[r] - s[l+1]
$$

这等价于如下两个式子**同时成立**（第二个式子做了移项）

$$
\begin{aligned}
& \textit{capacity}[l] = \textit{capacity}[r]    \\
& \textit{capacity}[l] + s[l+1] = s[r]  \\
\end{aligned}
$$

枚举 $r$，问题变成：

- 有多少个左端点 $l$ 满足子数组长度 $r-l+1\ge 3$，且二元组 $(\textit{capacity}[l],\textit{capacity}[l] + s[l+1])$ 等于二元组 $(\textit{capacity}[r],s[r])$？

枚举 $r$，用哈希表维护左边的 $(\textit{capacity}[l],\textit{capacity}[l] + s[l+1])$ 的个数。

为保证 $r-l + 1\ge 3$，可以在枚举 $r$ 的同时，先查询哈希表更新答案，再把二元组 $(\textit{capacity}[r-1],\textit{capacity}[r-1] + s[r])$ 加到哈希表，这样对于下一轮循环的 $r+1$ 来说，把添加的 $r-1$ 作为 $l$，与 $r+1$ 构成的子数组长度就是 $(r+1)-(r-1)+1 = 3$，满足要求。

此外，按照上述方式更新哈希表，$s$ 可以简化成一个变量。

[本题视频讲解](https://www.bilibili.com/video/BV1eqxNzXE8v/?t=10m52s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def countStableSubarrays(self, capacity: List[int]) -> int:
        cnt = defaultdict(int)
        s = capacity[0]  # 前缀和
        ans = 0
        for last, x in pairwise(capacity):
            ans += cnt[(x, s)]
            cnt[(last, last + s)] += 1
            s += x
        return ans
```

```java [sol-Java]
class Solution {
    private record Pair(int x, long s) {
    }

    public long countStableSubarrays(int[] capacity) {
        Map<Pair, Integer> cnt = new HashMap<>();
        long sum = capacity[0]; // 前缀和
        long ans = 0;
        for (int r = 1; r < capacity.length; r++) {
            ans += cnt.getOrDefault(new Pair(capacity[r], sum), 0);
            cnt.merge(new Pair(capacity[r - 1], capacity[r - 1] + sum), 1, Integer::sum);
            sum += capacity[r];
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long countStableSubarrays(vector<int>& capacity) {
        map<pair<int, long long>, int> cnt; // 另见【C++ 自定义哈希】
        long long sum = capacity[0]; // 前缀和
        long long ans = 0;
        for (int r = 1; r < capacity.size(); r++) {
            ans += cnt[{capacity[r], sum}];
            cnt[{capacity[r - 1], capacity[r - 1] + sum}]++;
            sum += capacity[r];
        }
        return ans;
    }
};
```

```cpp [sol-C++ 自定义哈希]
struct TupleHash {
    template<typename T>
    static void hash_combine(size_t& seed, const T& v) {
        // 参考 boost::hash_combine
        seed ^= hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<typename Tuple, size_t Index = 0>
    static void hash_tuple(size_t& seed, const Tuple& t) {
        if constexpr (Index < tuple_size_v<Tuple>) {
            hash_combine(seed, get<Index>(t));
            hash_tuple<Tuple, Index + 1>(seed, t);
        }
    }

    template<typename... Ts>
    size_t operator()(const tuple<Ts...>& t) const {
        size_t seed = 0;
        hash_tuple(seed, t);
        return seed;
    }
};

class Solution {
public:
    long long countStableSubarrays(vector<int>& capacity) {
        unordered_map<tuple<int, long long>, int, TupleHash> cnt;
        long long sum = capacity[0]; // 前缀和
        long long ans = 0;
        for (int r = 1; r < capacity.size(); r++) {
            ans += cnt[{capacity[r], sum}];
            cnt[{capacity[r - 1], capacity[r - 1] + sum}]++;
            sum += capacity[r];
        }
        return ans;
    }
};
```

```go [sol-Go]
func countStableSubarrays(capacity []int) (ans int64) {
	type pair struct{ x, s int }
	cnt := map[pair]int{}
	sum := capacity[0] // 前缀和
	for r := 1; r < len(capacity); r++ {
		ans += int64(cnt[pair{capacity[r], sum}])
		cnt[pair{capacity[r-1], capacity[r-1] + sum}]++
		sum += capacity[r]
	}
	return
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{capacity}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 思考题

把最小长度 $3$ 改成 $4$，代码应该如何修改？改成 $k$ 呢？

## 专题训练

见下面数据结构题单的「**§1.2 前缀和与哈希表**」。

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

本题来自 `一、前缀和 / §1.2 前缀和与哈希表`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.2 前缀和与哈希表`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
