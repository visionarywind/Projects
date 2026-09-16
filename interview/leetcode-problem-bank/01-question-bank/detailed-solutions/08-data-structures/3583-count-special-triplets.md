# 3583. 统计特殊三元组

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/count-special-triplets/
- 题目 slug：`count-special-triplets`
- 来源专题：常用数据结构
- 来源分类路径：零、常用枚举技巧 / §0.2 枚举中间 / §0.2.1 基础
- 难度分：1510
- 外部题解来源：https://leetcode.cn/problems/count-special-triplets/solutions/3700554/mei-ju-zhong-jian-qian-hou-zhui-fen-jie-5uad8/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[两种方法：枚举中间 / 一次遍历（Python/Java/C++/Go）](https://leetcode.cn/problems/count-special-triplets/solutions/3700554/mei-ju-zhong-jian-qian-hou-zhui-fen-jie-5uad8/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`mei-ju-zhong-jian-qian-hou-zhui-fen-jie-5uad8`
- topic id：`3700554`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

## 方法一：枚举中间

三变量问题，一般**枚举中间的变量**最简单。为什么？对比一下：

- 枚举 $i$，后续计算中还需保证 $j < k$。
- 枚举 $j$，那么 $i$ 和 $k$ 自动被 $j$ 隔开，互相独立，后续计算中无需关心 $i$ 和 $k$ 的位置关系。

枚举中间的 $j$，问题变成：

- 在 $[0,j-1]$ 中，$\textit{nums}[j]\cdot 2$ 的出现次数。
- 在 $[j+1,n-1]$ 中，$\textit{nums}[j]\cdot 2$ 的出现次数。
- 在这些出现次数中，左右两边各选一个。根据乘法原理，把这两个出现次数相乘，加到答案中。

用哈希表（或者数组）统计 $j$ 左右每个数的出现次数。

- 右边的元素出现次数，可以先统计整个数组，然后再次遍历数组，**撤销** $[0,j]$ 中统计的元素出现次数，即为 $[j+1,n-1]$ 中的元素出现次数。
- 左边的元素出现次数，可以一边遍历 $\textit{nums}$，一边统计。

由于答案不超过 $n\cdot 10^5\cdot 10^5 \le 10^{15}$，可以只在返回时取模。

[本题视频讲解](https://www.bilibili.com/video/BV1qsMxz6EEd/?t=9m46s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def specialTriplets(self, nums: List[int]) -> int:
        MOD = 1_000_000_007
        suf = Counter(nums)

        ans = 0
        pre = defaultdict(int)  # 比 Counter 快
        for x in nums:  # x = nums[j]
            suf[x] -= 1  # 撤销
            # 现在 pre 中的是 [0,j-1]，suf 中的是 [j+1,n-1]
            ans += pre[x * 2] * suf[x * 2]
            pre[x] += 1
        return ans % MOD
```

```java [sol-Java]
// 更快的写法见【Java 数组】
class Solution {
    public int specialTriplets(int[] nums) {
        final int MOD = 1_000_000_007;
        Map<Integer, Integer> suf = new HashMap<>();
        for (int x : nums) {
            suf.merge(x, 1, Integer::sum); // suf[x]++
        }

        long ans = 0;
        Map<Integer, Integer> pre = new HashMap<>();
        for (int x : nums) { // x = nums[j]
            suf.merge(x, -1, Integer::sum); // suf[x]-- // 撤销
            // 现在 pre 中的是 [0,j-1]，suf 中的是 [j+1,n-1]
            ans += (long) pre.getOrDefault(x * 2, 0) * suf.getOrDefault(x * 2, 0);
            pre.merge(x, 1, Integer::sum); // pre[x]++
        }
        return (int) (ans % MOD);
    }
}
```

```java [sol-Java 数组]
class Solution {
    public int specialTriplets(int[] nums) {
        final int MOD = 1_000_000_007;
        int mx = 0;
        for (int x : nums) {
            mx = Math.max(mx, x);
        }

        int[] suf = new int[mx + 1];
        for (int x : nums) {
            suf[x]++;
        }

        long ans = 0;
        int[] pre = new int[mx + 1];
        for (int x : nums) {
            suf[x]--;
            if (x * 2 <= mx) {
                ans += (long) pre[x * 2] * suf[x * 2];
            }
            pre[x]++;
        }
        return (int) (ans % MOD);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int specialTriplets(vector<int>& nums) {
        const int MOD = 1'000'000'007;
        unordered_map<int, int> suf;
        for (int x : nums) {
            suf[x]++;
        }

        long long ans = 0;
        unordered_map<int, int> pre;
        for (int x : nums) { // x = nums[j]
            suf[x]--; // 撤销
            // 现在 pre 中的是 [0,j-1]，suf 中的是 [j+1,n-1]
            ans += 1LL * pre[x * 2] * suf[x * 2];
            pre[x]++;
        }
        return ans % MOD;
    }
};
```

```go [sol-Go]
func specialTriplets(nums []int) (ans int) {
	const mod = 1_000_000_007
	suf := map[int]int{}
	for _, x := range nums {
		suf[x]++
	}

	pre := map[int]int{}
	for _, x := range nums { // x = nums[j]
		suf[x]-- // 撤销
		// 现在 pre 中的是 [0,j-1]，suf 中的是 [j+1,n-1]
		ans += pre[x*2] * suf[x*2]
		pre[x]++
	}
	return ans % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 方法二：枚举右，维护左（一次遍历）

枚举 $k$，设 $x=\textit{nums}[k]$，问题变成：

- 有多少个二元组 $(i,j)$，满足 $i<j<k$ 且 $\textit{nums}[i]=x$ 且 $\textit{nums}[j] = \dfrac{x}{2}$。用哈希表 $\textit{cnt}_{12}$ 记录这样的二元组个数。

这个问题也可以枚举右维护左，即枚举 $j$，问题变成：

- 在 $j$ 左边有多少个数等于 $\textit{nums}[j]\cdot 2$？用哈希表 $\textit{cnt}_{1}$ 记录。

```py [sol-Python3]
class Solution:
    def specialTriplets(self, nums: List[int]) -> int:
        MOD = 1_000_000_007
        cnt1 = defaultdict(int)
        cnt12 = defaultdict(int)
        cnt123 = 0
        for x in nums:
            if x % 2 == 0:
                cnt123 += cnt12[x // 2]  # 把 x 当作 nums[k]
            cnt12[x] += cnt1[x * 2]  # 把 x 当作 nums[j]
            cnt1[x] += 1  # 把 x 当作 nums[i]
        return cnt123 % MOD
```

```java [sol-Java]
class Solution {
    public int specialTriplets(int[] nums) {
        final int MOD = 1_000_000_007;
        Map<Integer, Integer> cnt1 = new HashMap<>();
        Map<Integer, Long> cnt12 = new HashMap<>();
        long cnt123 = 0;
        for (int x : nums) {
            if (x % 2 == 0) {
                cnt123 += cnt12.getOrDefault(x / 2, 0L); // 把 x 当作 nums[k]
            }
            cnt12.merge(x, (long) cnt1.getOrDefault(x * 2, 0), Long::sum); // 把 x 当作 nums[j]
            cnt1.merge(x, 1, Integer::sum); // 把 x 当作 nums[i]
        }
        return (int) (cnt123 % MOD);
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int specialTriplets(vector<int>& nums) {
        const int MOD = 1'000'000'007;
        unordered_map<int, int> cnt1;
        unordered_map<int, long long> cnt12;
        long long cnt123 = 0;
        for (int x : nums) {
            if (x % 2 == 0) {
                cnt123 += cnt12[x / 2]; // 把 x 当作 nums[k]
            }
            cnt12[x] += cnt1[x * 2]; // 把 x 当作 nums[j]
            cnt1[x]++; // 把 x 当作 nums[i]
        }
        return cnt123 % MOD;
    }
};
```

```go [sol-Go]
func specialTriplets(nums []int) (cnt123 int) {
	const mod = 1_000_000_007
	cnt1 := map[int]int{}
	cnt12 := map[int]int{}
	for _, x := range nums {
		if x%2 == 0 {
			cnt123 += cnt12[x/2] // 把 x 当作 nums[k]
		}
		cnt12[x] += cnt1[x*2] // 把 x 当作 nums[j]
		cnt1[x]++ // 把 x 当作 nums[i]
	}
	return cnt123 % mod
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

## 相似题目

- [2874. 有序三元组中的最大值 II](https://leetcode.cn/problems/maximum-value-of-an-ordered-triplet-ii/)
- [2552. 统计上升四元组](https://leetcode.cn/problems/count-increasing-quadruplets/)

更多相似题目，见下面数据结构题单的「**§0.2 枚举中间**」和动态规划题单的「**专题：前后缀分解**」。

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

## 本地原创解析

### 1. 题意重述

本题来自 `零、常用枚举技巧 / §0.2 枚举中间 / §0.2.1 基础`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `零、常用枚举技巧 / §0.2 枚举中间 / §0.2.1 基础`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
