# 3919. 在下标间移动的最小代价

## 元信息

- LeetCode 链接：https://leetcode.cn/problems/minimum-cost-to-move-between-indices/
- 题目 slug：`minimum-cost-to-move-between-indices`
- 来源专题：常用数据结构
- 来源分类路径：一、前缀和 / §1.5 进阶
- 难度分：1777
- 外部题解来源：https://leetcode.cn/problems/minimum-cost-to-move-between-indices/solutions/3962725/tan-xin-qian-zhui-he-pythonjavacgo-by-en-0fe7/
- 外部题解授权状态：authorized-import
- 本地解析状态：draft-preview
- C++ 验证状态：not-run
- 生成时间：2026-09-16 11:11:08 +0800

## 授权导入：灵茶山艾府题解过程

- 题解标题：[贪心 + 前缀和（Python/Java/C++/Go）](https://leetcode.cn/problems/minimum-cost-to-move-between-indices/solutions/3962725/tan-xin-qian-zhui-he-pythonjavacgo-by-en-0fe7/)
- 作者：灵茶山艾府 (`endlesscheng`)
- 题解 slug：`tan-xin-qian-zhui-he-pythonjavacgo-by-en-0fe7`
- topic id：`3962725`
- 授权状态：authorized-by-user-confirmation
- 导入时间：2026-09-16 11:19:29 +0800

由于 $\textit{nums}$ 是严格递增的，对于 $i<j<k$，从 $i$ 直接跳到 $k$ 的代价，等于从 $i$ 跳到 $j$ 再跳到 $k$ 的代价之和。所以我们可以**一步步移动**。

> 用数学语言描述，就是 $\textit{nums}[k] - \textit{nums}[i] = (\textit{nums}[k] - \textit{nums}[j]) + (\textit{nums}[j] - \textit{nums}[i])$。

由于 $\textit{nums}$ 是严格递增的，所以使用方式一移动到相邻下标的代价至少是 $1$，那么贪心地，**能用方式二移动，就用方式二**。

此外，由于走回头路一定会包含往返的一段，这会花费多余的代价，所以我们不会走回头路。

计算从左到右一步步移动的代价的前缀和，以及从右到左一步步移动的代价的前缀和，即可 $\mathcal{O}(1)$ 回答询问。关于前缀和的原理，请看 [前缀和](https://leetcode.cn/problems/range-sum-query-immutable/solution/qian-zhui-he-ji-qi-kuo-zhan-fu-ti-dan-py-vaar/)。

[本题视频讲解](https://www.bilibili.com/video/BV1719oB4EWf/?t=7m34s)，欢迎点赞关注~

```py [sol-Python3]
class Solution:
    def minCost(self, nums: list[int], queries: list[list[int]]) -> list[int]:
        n = len(nums)
        sum_l = [0] * n  # sum_l[i] 等于从 i 移动到 0 的代价和
        sum_r = [0] * n  # sum_r[i] 等于从 0 移动到 i 的代价和
        for i in range(1, n):
            # 往左走 i -> i-1
            if i < n - 1 and nums[i] - nums[i - 1] > nums[i + 1] - nums[i]:  # closest(i) = i+1
                cost = nums[i] - nums[i - 1]  # 只能用方式一往左走
            else:
                cost = 1
            sum_l[i] = sum_l[i - 1] + cost

            # 往右走 i-1 -> i
            if i > 1 and nums[i - 1] - nums[i - 2] <= nums[i] - nums[i - 1]:  # closest(i-1) = i-2
                cost = nums[i] - nums[i - 1]  # 只能用方式一往右走
            else:
                cost = 1
            sum_r[i] = sum_r[i - 1] + cost

        ans = [0] * len(queries)
        for i, (l, r) in enumerate(queries):
            if l < r:
                # cost(0 -> r) - cost(0 -> l) = cost(l -> r)
                ans[i] = sum_r[r] - sum_r[l]
            else:
                # cost(l -> 0) - cost(r -> 0) = cost(l -> r)
                ans[i] = sum_l[l] - sum_l[r]
        return ans
```

```java [sol-Java]
class Solution {
    public int[] minCost(int[] nums, int[][] queries) {
        int n = nums.length;
        int[] sumL = new int[n]; // sumL[i] 等于从 i 移动到 0 的代价和
        int[] sumR = new int[n]; // sumR[i] 等于从 0 移动到 i 的代价和
        for (int i = 1, cost; i < n; i++) {
            // 往左走 i -> i-1
            if (i < n - 1 && nums[i] - nums[i - 1] > nums[i + 1] - nums[i]) { // closest(i) = i+1
                cost = nums[i] - nums[i - 1]; // 只能用方式一往左走
            } else {
                cost = 1;
            }
            sumL[i] = sumL[i - 1] + cost;

            // 往右走 i-1 -> i
            if (i > 1 && nums[i - 1] - nums[i - 2] <= nums[i] - nums[i - 1]) { // closest(i-1) = i-2
                cost = nums[i] - nums[i - 1]; // 只能用方式一往右走
            } else {
                cost = 1;
            }
            sumR[i] = sumR[i - 1] + cost;
        }

        int[] ans = new int[queries.length];
        for (int i = 0; i < queries.length; i++) {
            int l = queries[i][0];
            int r = queries[i][1];
            if (l < r) {
                // cost(0 -> r) - cost(0 -> l) = cost(l -> r)
                ans[i] = sumR[r] - sumR[l];
            } else {
                // cost(l -> 0) - cost(r -> 0) = cost(l -> r)
                ans[i] = sumL[l] - sumL[r];
            }
        }
        return ans;
    }
}
```

```cpp [sol-C++]
class Solution {
public:
    vector<int> minCost(vector<int>& nums, vector<vector<int>>& queries) {
        int n = nums.size();
        vector<int> sum_l(n); // sum_l[i] 等于从 i 移动到 0 的代价和
        vector<int> sum_r(n); // sum_r[i] 等于从 0 移动到 i 的代价和
        for (int i = 1, cost; i < n; i++) {
            // 往左走 i -> i-1
            if (i < n - 1 && nums[i] - nums[i - 1] > nums[i + 1] - nums[i]) { // closest(i) = i+1
                cost = nums[i] - nums[i - 1]; // 只能用方式一往左走
            } else {
                cost = 1;
            }
            sum_l[i] = sum_l[i - 1] + cost;

            // 往右走 i-1 -> i
            if (i > 1 && nums[i - 1] - nums[i - 2] <= nums[i] - nums[i - 1]) { // closest(i-1) = i-2
                cost = nums[i] - nums[i - 1]; // 只能用方式一往右走
            } else {
                cost = 1;
            }
            sum_r[i] = sum_r[i - 1] + cost;
        }

        vector<int> ans(queries.size());
        for (int i = 0; i < queries.size(); i++) {
            int l = queries[i][0], r = queries[i][1];
            if (l < r) {
                // cost(0 -> r) - cost(0 -> l) = cost(l -> r)
                ans[i] = sum_r[r] - sum_r[l];
            } else {
                // cost(l -> 0) - cost(r -> 0) = cost(l -> r)
                ans[i] = sum_l[l] - sum_l[r];
            }
        }
        return ans;
    }
};
```

```go [sol-Go]
func minCost(nums []int, queries [][]int) []int {
	n := len(nums)
	sumL := make([]int, n) // sumL[i] 等于从 i 移动到 0 的代价和
	sumR := make([]int, n) // sumR[i] 等于从 0 移动到 i 的代价和
	for i := 1; i < n; i++ {
		// 往左走 i -> i-1
		cost := 1
		if i < n-1 && nums[i]-nums[i-1] > nums[i+1]-nums[i] { // closest(i) = i+1
			cost = nums[i] - nums[i-1] // 只能用方式一往左走
		}
		sumL[i] = sumL[i-1] + cost

		// 往右走 i-1 -> i
		cost = 1
		if i > 1 && nums[i-1]-nums[i-2] <= nums[i]-nums[i-1] { // closest(i-1) = i-2
			cost = nums[i] - nums[i-1] // 只能用方式一往右走
		}
		sumR[i] = sumR[i-1] + cost
	}

	ans := make([]int, len(queries))
	for i, q := range queries {
		l, r := q[0], q[1]
		if l < r {
			// cost(0 -> r) - cost(0 -> l) = cost(l -> r)
			ans[i] = sumR[r] - sumR[l]
		} else {
			// cost(l -> 0) - cost(r -> 0) = cost(l -> r)
			ans[i] = sumL[l] - sumL[r]
		}
	}
	return ans
}
```

#### 复杂度分析

- 时间复杂度：$\mathcal{O}(n + q)$，其中 $n$ 是 $\textit{nums}$ 的长度，$q$ 是 $\textit{queries}$ 的长度。
- 空间复杂度：$\mathcal{O}(n)$。返回值不计入。

## 专题训练

见下面数据结构题单的「**一、前缀和**」。

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

本题来自 `一、前缀和 / §1.5 进阶`。先把题目抽象为该分类下的标准模型：确定要维护的对象、合法状态和答案更新时机，再用来源题解正文校准细节。

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

回到来源分类 `一、前缀和 / §1.5 进阶`，选择同小节后续题目训练。若新题只是维护量变化，优先复用当前框架；若合法性条件变化，再调整枚举顺序、收缩策略或状态转移。
