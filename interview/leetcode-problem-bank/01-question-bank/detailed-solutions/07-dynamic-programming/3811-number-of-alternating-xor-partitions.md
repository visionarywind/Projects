# 3811. 交替按位异或分割的数目

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/number-of-alternating-xor-partitions/
- 题目 slug：`number-of-alternating-xor-partitions`
- 来源专题：动态规划
- 来源分类路径：六、状态机 DP / §6.3 进阶
- 难度分：2006
- 外部题解来源：https://leetcode.cn/problems/number-of-alternating-xor-partitions/solutions/3883194/he-fa-zi-xu-lie-dppythonjavacgo-by-endle-ivxh/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[合法子序列 DP + 状态机 DP（Python/Java/C++/Go）](https://leetcode.cn/problems/number-of-alternating-xor-partitions/solutions/3883194/he-fa-zi-xu-lie-dppythonjavacgo-by-endle-ivxh/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`he-fa-zi-xu-lie-dppythonjavacgo-by-endle-ivxh`
- topic id：`3883194`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 16:46:02 +0800

**前置题目**：[560. 和为 K 的子数组](https://leetcode.cn/problems/subarray-sum-equals-k/)，[我的题解](https://leetcode.cn/problems/subarray-sum-equals-k/solutions/2781031/qian-zhui-he-ha-xi-biao-cong-liang-ci-bi-4mwr/)。

想一想，最后一步发生了什么？

设整个数组的异或和为 $s$。

- 如果最后一段的异或和是 $\textit{target}_1$，分割出最后一段后，剩余元素（$\textit{nums}$ 的前缀）的异或和为 $s\oplus \textit{target}_1$，问题变成：有多少个异或和是 $s\oplus \textit{target}_1$ 的前缀，满足题目异或和交替的要求，且最后一段的异或和是 $\textit{target}_2$？
- 如果最后一段的异或和是 $\textit{target}_2$，分割出最后一段后，剩余元素（$\textit{nums}$ 的前缀）的异或和为 $s\oplus \textit{target}_2$，问题变成：有多少个异或和是 $s\oplus \textit{target}_2$ 的前缀，满足题目异或和交替的要求，且最后一段的异或和是 $\textit{target}_1$？

这都是**规模更小的子问题**，所以可以用 DP 解决。

在 560 题中，我们用哈希表记录每个前缀和的出现次数。

本题借鉴 560 题的思想，用两个哈希表记录每个前缀和对应的划分方案数：

- 定义 $f_1[s]$ 表示分割异或和为 $s$ 的前缀，满足题目异或和交替的要求，且最后一段的异或和是 $\textit{target}_1$ 的方案数。
- 定义 $f_2[s]$ 表示分割异或和为 $s$ 的前缀，满足题目异或和交替的要求，且最后一段的异或和是 $\textit{target}_2$ 的方案数。

从左到右遍历 $\textit{nums}$，同时计算前缀异或和 $\textit{preSum}$。根据上面的讨论，有如下转移方程

$$
\begin{aligned}
f_1[\textit{preSum}] &\ += f_2[\textit{preSum} \oplus \textit{target}_1]      \\
f_2[\textit{preSum}] &\ += f_1[\textit{preSum} \oplus \textit{target}_2]      \\
\end{aligned}
$$

注意这两个式子要同时计算。

初始值：$f_2[0] = 1$。相当于在第一段前面有一个空前缀，异或和为 $0$。这样我们计算第一段时，就可以让第一段的方案数是 $1$。

答案为最后一轮循环（更新 $f_1$ 和 $f_2$ 之前）的 $f_1[\textit{preSum} \oplus \textit{target}_2] + f_2[\textit{preSum} \oplus \textit{target}_1]$，表示分割整个数组的方案数。注意不能在遍历完 $\textit{nums}$ 后，从 $f_1$ 和 $f_2$ 中取数据，这可能会把「异或和恰好等于整个数组的异或和」的前缀的划分方案也算进来。

代码实现时，注意取模。为什么可以在**中途取模**？原理见 [模运算的世界：当加减乘除遇上取模](https://leetcode.cn/circle/discuss/mDfnkW/)。

[本题视频讲解](https://www.bilibili.com/video/BV1MVkxBZE4D/?t=8m29s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def alternatingXOR(self, nums: List[int], target1: int, target2: int) -> int:
        MOD = 1_000_000_007
        f1 = defaultdict(int)
        f2 = defaultdict(int)
        f2[0] = 1
        pre_sum = 0
        for i, x in enumerate(nums):
            pre_sum ^= x
            last1 = f2[pre_sum ^ target1]  # [0,i] 的最后一段的异或和是 target1 的方案数
            last2 = f1[pre_sum ^ target2]  # [0,i] 的最后一段的异或和是 target2 的方案数
            if i == len(nums) - 1:
                return (last1 + last2) % MOD
            f1[pre_sum] = (f1[pre_sum] + last1) % MOD
            f2[pre_sum] = (f2[pre_sum] + last2) % MOD
```

```java [sol-Java]
class Solution {
    public int alternatingXOR(int[] nums, int target1, int target2) {
        final int MOD = 1_000_000_007;
        Map<Integer, Integer> f1 = new HashMap<>();
        Map<Integer, Integer> f2 = new HashMap<>();
        f2.put(0, 1);
        int preSum = 0;
        for (int i = 0; ; i++) {
            preSum ^= nums[i];
            int last1 = f2.getOrDefault(preSum ^ target1, 0); // [0,i] 的最后一段的异或和是 target1 的方案数
            int last2 = f1.getOrDefault(preSum ^ target2, 0); // [0,i] 的最后一段的异或和是 target2 的方案数
            if (i == nums.length - 1) {
                return (last1 + last2) % MOD;
            }
            f1.put(preSum, (f1.getOrDefault(preSum, 0) + last1) % MOD);
            f2.put(preSum, (f2.getOrDefault(preSum, 0) + last2) % MOD);
        }
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    int alternatingXOR(vector<int>& nums, int target1, int target2) {
        constexpr int MOD = 1'000'000'007;
        unordered_map<int, int> f1;
        unordered_map<int, int> f2 = {{0, 1}};
        int pre_sum = 0;
        for (int i = 0; ; i++) {
            pre_sum ^= nums[i];
            int last1 = f2[pre_sum ^ target1]; // [0,i] 的最后一段的异或和是 target1 的方案数
            int last2 = f1[pre_sum ^ target2]; // [0,i] 的最后一段的异或和是 target2 的方案数
            if (i == nums.size() - 1) {
                return (last1 + last2) % MOD;
            }
            f1[pre_sum] = (f1[pre_sum] + last1) % MOD;
            f2[pre_sum] = (f2[pre_sum] + last2) % MOD;
        }
    }
};
```

```go [sol-Go]
func alternatingXOR(nums []int, target1, target2 int) int {
	const mod = 1_000_000_007
	f1 := map[int]int{}
	f2 := map[int]int{0: 1}
	preSum := 0
	for i, x := range nums {
		preSum ^= x
		last1 := f2[preSum^target1] // [0,i] 的最后一段的异或和是 target1 的方案数
		last2 := f1[preSum^target2] // [0,i] 的最后一段的异或和是 target2 的方案数
		if i == len(nums)-1 {
			return (last1 + last2) % mod
		}
		f1[preSum] = (f1[preSum] + last1) % mod
		f2[preSum] = (f2[preSum] + last2) % mod
	}
	panic("unreachable")
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。

**注**：本题还可以利用「前 $k=1,2,3,\ldots$ 段的异或和是周期 $T=4$ 的序列」这一性质，写一个状态机 DP。然而，如果题目把异或运算改成加法运算，没有周期，这种方法就失效了。上面的算法没有用到周期性质，改成加法运算仍然可用。

## 专题训练

见下面动态规划题单的「**§7.4 合法子序列 DP**」。

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

本题来自 `六、状态机 DP / §6.3 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `六、状态机 DP / §6.3 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
