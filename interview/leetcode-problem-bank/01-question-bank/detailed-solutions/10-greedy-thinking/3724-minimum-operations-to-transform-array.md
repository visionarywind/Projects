# 3724. 转换数组的最少操作次数

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-operations-to-transform-array/
- 题目 slug：`minimum-operations-to-transform-array`
- 来源专题：贪心与思维
- 来源分类路径：五、思维题 / §5.8 分类讨论
- 难度分：1790
- 外部题解来源：https://leetcode.cn/problems/minimum-operations-to-transform-array/solutions/3815528/tan-xin-fen-lei-tao-lun-pythonjavacgo-by-yvxx/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心 + 分类讨论，附思考题（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-operations-to-transform-array/solutions/3815528/tan-xin-fen-lei-tao-lun-pythonjavacgo-by-yvxx/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-fen-lei-tao-lun-pythonjavacgo-by-yvxx`
- topic id：`3815528`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-17 11:13:14 +0800

## 理解题意

由于 $\textit{nums}_2$ 的长度恰好比 $\textit{nums}_1$ 多一，所以恰好有一个元素要追加到 $\textit{nums}_1$ 的末尾。

其余元素必须一一对应，对于 $[0,n-1]$ 中的 $i$，$x = \textit{nums}_1[i]$ 要变成 $y = \textit{nums}_2[i]$。

## 思路

设 $\textit{target}$ 是 $\textit{nums}_2$ 的最后一个数。

贪心地想，能不能在从 $x$ 变成 $y$ 的过程中，**顺带**把 $\textit{target}$ 追加到 $\textit{nums}_1$ 的末尾？

不妨设 $x\le y$，分类讨论：

- 如果 $\textit{target}$ 在 $x$ 和 $y$ 之间，由于每次操作只能把 $x$ 增大/减少 $1$，只能一步一步挪动，所以可以在 $x$ 变成 $\textit{target}$ 的那一刻，把 $\textit{target}$ 追加到 $\textit{nums}_1$ 的末尾。
- 如果 $\textit{target} < x$，那么先把 $x$ 追加到 $\textit{nums}_1$ 的末尾，再花费 $x-\textit{target}$ 的操作次数。
- 如果 $\textit{target} > y$，那么先把 $x$ 变成 $y$，把 $y$ 追加到 $\textit{nums}_1$ 的末尾，再花费 $\textit{target}-y$ 的操作次数。

对于每一对 $(x,y)$，计算处理 $\textit{target}$ 操作次数，取最小值。

[本题视频讲解](https://www.bilibili.com/video/BV1zxxNzcERu/?t=9m22s)，欢迎点赞关注~

```py [sol-Python3]
# 手写 min max 更快
min = lambda a, b: b if b < a else a
max = lambda a, b: b if b > a else a

class Solution:
    def minOperations(self, nums1: List[int], nums2: List[int]) -> int:
        target = nums2[-1]
        ans = 1  # 把元素追加到 nums1 的末尾需要一次操作
        mn = inf
        for x, y in zip(nums1, nums2):
            if x > y:
                x, y = y, x  # 保证 x <= y，简化后续逻辑
            ans += y - x
            # 如果 target 在 [x,y] 中，那么在从 x 变成 y 的过程中，可以顺带把 target 追加到 nums1 的末尾，代价为 0
            # 如果 target < x，代价为 x-target
            # 如果 target > y，代价为 target-y
            if mn > 0:  # 如果 target 还不在任何 [x,y] 中，则计算
                mn = min(mn, max(x - target, target - y))
        return ans + max(mn, 0)  # 如果 target 在 [x,y] 中，上面可能会算出负数
```

```java [sol-Java]
class Solution {
    public long minOperations(int[] nums1, int[] nums2) {
        int n = nums1.length;
        int target = nums2[n];
        long ans = 1; // 把元素追加到 nums1 的末尾需要一次操作
        int mn = Integer.MAX_VALUE;
        for (int i = 0; i < n; i++) {
            // 保证 x <= y，简化后续逻辑
            int x = Math.min(nums1[i], nums2[i]);
            int y = Math.max(nums1[i], nums2[i]);
            ans += y - x;
            // 如果 target 在 [x,y] 中，那么在从 x 变成 y 的过程中，可以顺带把 target 追加到 nums1 的末尾，代价为 0
            // 如果 target < x，代价为 x-target
            // 如果 target > y，代价为 target-y
            mn = Math.min(mn, Math.max(x - target, target - y));
        }
        return ans + Math.max(mn, 0); // 如果 target 在 [x,y] 中，上面可能会算出负数
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    long long minOperations(vector<int>& nums1, vector<int>& nums2) {
        int target = nums2.back();
        long long ans = 1; // 把元素追加到 nums1 的末尾需要一次操作
        int mn = INT_MAX;
        for (int i = 0; i < nums1.size(); i++) {
            int x = nums1[i], y = nums2[i];
            if (x > y) {
                swap(x, y); // 保证 x <= y，简化后续逻辑
            }
            ans += y - x;
            // 如果 target 在 [x,y] 中，那么在从 x 变成 y 的过程中，可以顺带把 target 追加到 nums1 的末尾，代价为 0
            // 如果 target < x，代价为 x-target
            // 如果 target > y，代价为 target-y
            mn = min(mn, max(x - target, target - y));
        }
        return ans + max(mn, 0); // 如果 target 在 [x,y] 中，上面可能会算出负数
    }
};
```

```go [sol-Go]
func minOperations(nums1, nums2 []int) int64 {
	target := nums2[len(nums2)-1]
	ans := 1 // 把元素追加到 nums1 的末尾需要一次操作
	mn := math.MaxInt
	for i, x := range nums1 {
		y := nums2[i]
		if x > y {
			x, y = y, x // 保证 x <= y，简化后续逻辑
		}
		ans += y - x
		// 如果 target 在 [x,y] 中，那么在从 x 变成 y 的过程中，可以顺带把 target 追加到 nums1 的末尾，代价为 0
		// 如果 target < x，代价为 x-target
		// 如果 target > y，代价为 target-y
		mn = min(mn, max(x-target, target-y))
	}
	return int64(ans + max(mn, 0)) // 如果 target 在 [x,y] 中，上面可能会算出负数
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n)$，其中 $n$ 是 $\textit{nums}_1$ 的长度。
- 空间复杂度：$\mathcal{O}(1)$。

## 思考题

1. 如果 $\textit{nums}_2$ 的长度是 $m\ (m>n)$，怎么做？
2. 如果 $\textit{nums}_2$ 的长度是 $n+1$，但操作三改成可以插在 $\textit{nums}_1$ 的任意位置，怎么做？

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

本题来自 `五、思维题 / §5.8 分类讨论`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `五、思维题 / §5.8 分类讨论`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
